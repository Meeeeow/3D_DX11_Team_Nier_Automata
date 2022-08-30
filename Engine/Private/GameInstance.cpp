#include "..\Public\GameInstance.h"
#include "Light.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pGraphicDevice(CGraphicDevice::Get_Instance()), m_pInputDevice(CInputDevice::Get_Instance()), m_pTimerManager(CTimerManager::Get_Instance())
	, m_pComponentManager(CComponentManager::Get_Instance()), m_pLevelManager(CLevelManager::Get_Instance()), m_pGameObjectManager(CGameObjectManager::Get_Instance())
	, m_pPipeLine(CPipeLine::Get_Instance()), m_pLightManager(CLightManager::Get_Instance()), m_pRenderTargetManager(CRenderTargetManager::Get_Instance())
	, m_pPicking(CPicking::Get_Instance()), m_pFrustum(CFrustum::Get_Instance()), m_pObserverManager(CObserverManager::Get_Instance())
	, m_pRandomManager(CRandomManager::Get_Instance()), m_pFontManager(CFontManager::Get_Instance())
	, m_pSubjectManager(CSubjectManager::Get_Instance()), m_pGraphicDebugger(CGraphicDebugger::Get_Instance())
{
	/* System Category Manager */
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pInputDevice);
	Safe_AddRef(m_pTimerManager);

	/* Utility Manager */
	Safe_AddRef(m_pComponentManager);
	Safe_AddRef(m_pLevelManager);
	Safe_AddRef(m_pGameObjectManager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pLightManager);
	Safe_AddRef(m_pRenderTargetManager);
	Safe_AddRef(m_pPicking);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pObserverManager);
	Safe_AddRef(m_pSubjectManager);
	Safe_AddRef(m_pRandomManager);
	Safe_AddRef(m_pFontManager);

	/* Graphic Debugger */
	Safe_AddRef(m_pGraphicDebugger);

}
#ifdef _DEBUG
HRESULT CGameInstance::Initialize(HWND hMainWnd)
{
	return m_pGraphicDebugger->Initialize(hMainWnd);
}
#endif // _DEBUG

HRESULT CGameInstance::Initialize(HINSTANCE hInstance, CGraphicDevice::DESC & tGraphicDesc, _uint iNumLevels, _pGraphicDevice * ppOutGraphicDevice, _pContextDevice * ppOutContextDevice)
{
	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (m_pGraphicDevice->Initialize(tGraphicDesc, ppOutGraphicDevice, ppOutContextDevice))
		return E_FAIL;

	m_hWnd = tGraphicDesc.hWnd;

	if (m_pInputDevice->NativeConsturct(hInstance, tGraphicDesc.hWnd))
		return E_FAIL;
	
	if (m_pRenderTargetManager->NativeConstruct(*ppOutGraphicDevice, *ppOutContextDevice))
		return E_FAIL;

	if (m_pPicking->NativeConstruct(tGraphicDesc.hWnd))
		return E_FAIL;

	if (m_pComponentManager->Reserve_Container(iNumLevels))
		return E_FAIL;

	if (m_pGameObjectManager->Reserve_Container(iNumLevels))
		return E_FAIL;

	m_pRandomManager->Range(1, 10);

	m_pPipeLine->Set_WindowSize(tGraphicDesc.iWinCX, tGraphicDesc.iWinCY);

	m_iMaxLevel = iNumLevels;

	m_bDebugDraw = true;

	return S_OK;
}

HRESULT CGameInstance::Initialize(HINSTANCE hInstance, CGraphicDevice::DESC & tGraphicDesc, HWND hWnd, _uint iNumLevels, _pGraphicDevice * ppOutGraphicDevice, _pContextDevice * ppOutContextDevice)
{
	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;

	if (FAILED(m_pGraphicDevice->Initialize(tGraphicDesc, ppOutGraphicDevice, ppOutContextDevice)))
		return E_FAIL;

	m_hWnd = tGraphicDesc.hWnd;

	if (FAILED(m_pInputDevice->NativeConsturct(hInstance, hWnd)))
		return E_FAIL;

	if (FAILED(m_pRenderTargetManager->NativeConstruct(*ppOutGraphicDevice ,*ppOutContextDevice)))
		return E_FAIL;

	if (FAILED(m_pPicking->NativeConstruct(hWnd)))
		return E_FAIL;

	if (FAILED(m_pComponentManager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pGameObjectManager->Reserve_Container(iNumLevels)))
		return E_FAIL;

	m_pPipeLine->Set_WindowSize(tGraphicDesc.iWinCX, tGraphicDesc.iWinCY);

	return S_OK;
}

_int CGameInstance::Tick(_double dTimeDelta)
{
	if (m_pInputDevice == nullptr)
		FAILMSG("InputDevice is nullptr - GameInstance Tick");

	if (FAILED(m_pInputDevice->Update_InputDeviceState()))
		return E_FAIL;


#ifdef _DEBUG
	m_pPipeLine->Add_PlayTime(dTimeDelta);
#endif // _DEBUG

	_uint iLevel = 0;
	if (m_pLevelManager->Get_CurrentLevel() != nullptr)
		iLevel = m_pLevelManager->Get_CurrentLevelID();

	if (m_pLevelManager->Tick(dTimeDelta) < 0)
		FAILMSG("LevelManager Tick return under the 0 - GameInstance Tick");

	//if (m_pLevelManager->LateTick(dTime) < 0)
	//	FAILMSG("LevelManager LateTick return under the 0 - GameInstance Tick");

	

	m_pPipeLine->Set_TimeDelta(dTimeDelta);

	_double dTime = dTimeDelta;

	if (m_bBulletTime)
	{
		m_dBulletTime -= dTime;
		dTime /= m_dBulletLevel;
		if (m_dBulletTime <= 0.0)
		{
			m_dBulletTime = 0.0;
			m_dBulletLevel = 1.0;
			m_bBulletTime = false;
		}
	}

	if (iLevel != m_iMaxLevel - 1)	// LoadingÀÌ ¾Æ´Ò¶§
	{
		if (m_pGameObjectManager->Tick(dTime) < 0)
			FAILMSG("ObjectManager Tick return under the 0 - GameInstnace Tick");

		if (m_pLightManager->Tick(dTime) < 0)
			FAILMSG("Light Manager Tick return under the 0 - GameInstance Tick");
	}

	if (m_pPipeLine == nullptr)
		FAILMSG("PipeLine is nullptr - GameInstance Tick");

	if (m_pPipeLine->Update_Variable())
		return E_FAIL;

	m_pFrustum->Make_Frustum();

	if (FAILED(m_pPicking->PreConvert()))
		return E_FAIL;

	if (m_pGameObjectManager->Collect_Event() < 0)
		return E_FAIL;

	if (iLevel != m_iMaxLevel - 1)	// LoadingÀÌ ¾Æ´Ò¶§
	{
		if (m_pGameObjectManager->LateTick(dTime) < 0)
			FAILMSG("ObjectManager LateTick return under the 0 - GameInstance Tick");

		if (m_pLightManager->LateTick(dTime) < 0)
			FAILMSG("Light Manager Tick return under the 0 - GameInstance Tick");
	}


	if (m_pLevelManager == nullptr)
		FAILMSG("LevelManager is nullptr - GameInstance Tick");

	if (iLevel != m_iMaxLevel - 1)	// LoadingÀÌ ¾Æ´Ò¶§
	{
		if (FAILED(m_pGameObjectManager->CollisionTick(dTime)))
			FAILMSG("ObjectManager Collision Tick");
	}

	return 0;
}

void CGameInstance::Clear_Container(_uint iNumLevels)
{
	m_pGameObjectManager->Clear(iNumLevels);
	m_pComponentManager->Clear(iNumLevels);
}

HWND CGameInstance::Get_hWnd() const
{
	return m_hWnd;
}

HRESULT CGameInstance::Clear_BackBufferView(XMFLOAT4 vClearColor)
{
	return m_pGraphicDevice->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencilView()
{
	return m_pGraphicDevice->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphicDevice->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte btKeyID)
{
	return m_pInputDevice->Get_DIKeyState(btKeyID);
}

_byte CGameInstance::Get_DIMouseButtonState(CInputDevice::DIMB eMouseButton)
{
	return m_pInputDevice->Get_DIMouseButtonState(eMouseButton);
}

_long CGameInstance::Get_DIMouseMoveState(CInputDevice::DIMM eMouseMove)
{
	return m_pInputDevice->Get_DIMouseMoveState(eMouseMove);
}

_bool CGameInstance::Key_Pressing(_ubyte btKeyID)
{
	return m_pInputDevice->Key_Pressing(btKeyID);
}

_bool CGameInstance::Key_Down(_ubyte btKeyID)
{
	return m_pInputDevice->Key_Down(btKeyID);
}

_bool CGameInstance::Key_Up(_ubyte btKeyID)
{
	return m_pInputDevice->Key_Up(btKeyID);
}

_bool CGameInstance::Mouse_Pressing(CInputDevice::DIMB eMouseButton)
{
	return m_pInputDevice->Mouse_Pressing(eMouseButton);
}

_bool CGameInstance::Mouse_Down(CInputDevice::DIMB eMouseButton)
{
	return m_pInputDevice->Mouse_Down(eMouseButton);
}

_bool CGameInstance::Mouse_Up(CInputDevice::DIMB eMouseButton)
{
	return m_pInputDevice->Mouse_Up(eMouseButton);
}

HRESULT CGameInstance::Add_Timer(const _tchar * pTimerTag)
{
	return m_pTimerManager->Add_Timer(pTimerTag);
}

_double CGameInstance::Compute_Timer(const _tchar * pTimerTag)
{
	return m_pTimerManager->Compute_Timer(pTimerTag);
}

HRESULT CGameInstance::Add_Component(_uint iLevelIndex, const _tchar * pComponentTag, CComponent * pComponent)
{
	return m_pComponentManager->Add_Component(iLevelIndex, pComponentTag, pComponent);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pComponentTag, void * pArg)
{
	return m_pComponentManager->Clone_Component(iLevelIndex, pComponentTag, pArg);
}

HRESULT CGameInstance::Open_Level(CLevel * pLevel)
{
	return m_pLevelManager->Open_Level(pLevel);
}

HRESULT CGameInstance::Render_Level()
{
	return m_pLevelManager->Render();
}

_uint CGameInstance::Get_LevelID() const
{
	return m_pLevelManager->Get_CurrentLevelID();
}

const _uint & CGameInstance::Get_NextLevelID() const
{
	return m_pLevelManager->Get_NextLevelID();
}

void CGameInstance::Set_NextLevelID(const _uint & iLevelID)
{
	m_pLevelManager->Set_NextLevelID(iLevelID);
}

CLevel * CGameInstance::Get_CurrentLevel()
{
	return m_pLevelManager->Get_CurrentLevel();
}

CGameObject * CGameInstance::Get_GameObjectPtr(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
	return m_pGameObjectManager->Get_GameObject(iLevelIndex, pLayerTag, iIndex);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	return m_pGameObjectManager->Clone_GameObject(pPrototypeTag, pArg);
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	CGameObject* pGameObject = m_pGameObjectManager->Get_GameObject(iLevelIndex, pLayerTag, iIndex);

	if (nullptr == pGameObject)
		return nullptr;
	

	return pGameObject->Get_Component(pComponentTag);
}

HRESULT CGameInstance::Add_GameObject(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	return m_pGameObjectManager->Add_GameObject(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	return m_pGameObjectManager->Add_GameObjectToLayer(iLevelIndex, pPrototypeTag, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObjectToLayer(_uint iLevelIndex, CGameObject * pInstance, const _tchar * pLayerTag)
{
	return m_pGameObjectManager->Add_GameObjectToLayer(iLevelIndex, pInstance, pLayerTag);
}

HRESULT CGameInstance::Add_GameObjectToLayerWithPtr(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, CGameObject ** ppOut, void * pArg)
{
	return m_pGameObjectManager->Add_GameObjectToLayerWithPtr(iLevelIndex, pPrototypeTag, pLayerTag, ppOut, pArg);
}

HRESULT CGameInstance::Add_CollisionTag(_uint iFirstLevel, const wchar_t* pFirstTag, _uint iFirstType, _uint iSecondLevel, const wchar_t* pSecondTag, _uint iSecondType)
{
	COLLISION_TAG* pCollisTag = new COLLISION_TAG;
	pCollisTag->Set_CollisionValue(iFirstLevel, pFirstTag, iFirstType, iSecondLevel, pSecondTag, iSecondType);

	return m_pGameObjectManager->Add_CollisionTag(pCollisTag);
}

HRESULT CGameInstance::Clear_CollisionTag()
{
	return m_pGameObjectManager->Clear_CollisionTag();
}

void CGameInstance::Clear_GameObject(_uint iIndex)
{
	m_pGameObjectManager->Clear(iIndex);
}

_bool CGameInstance::IsFind_Prototype(const _tchar * pPrototypeTag)
{
	return m_pGameObjectManager->IsFind_Prototype(pPrototypeTag);
}

list<CGameObject*>* CGameInstance::Get_ObjectList(_uint iLevelIndex, const _tchar * pLayerTag)
{
	return m_pGameObjectManager->Get_Layer(iLevelIndex, pLayerTag);
}

_matrix CGameInstance::Get_Transform(CPipeLine::TRANSFORM eState)
{
	return m_pPipeLine->Get_Transform(eState);
}

_matrix CGameInstance::Get_Transpose(CPipeLine::TRANSFORM eState)
{
	return m_pPipeLine->Get_Transpose(eState);
}

_matrix CGameInstance::Get_Inverse(CPipeLine::TRANSFORM eState)
{
	return m_pPipeLine->Get_Inverse(eState);
}

_matrix CGameInstance::Get_OldTransform(CPipeLine::TRANSFORM eState)
{
	return m_pPipeLine->Get_OldTransform(eState);
}

_matrix CGameInstance::Get_OldTranspose(CPipeLine::TRANSFORM eState)
{
	return m_pPipeLine->Get_OldTranspose(eState);
}

_vector CGameInstance::Get_CameraPosition()
{
	return m_pPipeLine->Get_CameraPosition();
}

_float CGameInstance::Get_Far()
{
	return m_pPipeLine->Get_Far();
}

_float CGameInstance::Get_Near()
{
	return m_pPipeLine->Get_Near();
}

_float CGameInstance::Get_FOV()
{
	return m_pPipeLine->Get_FOV();;
}

_double CGameInstance::Get_TimeDelta()
{
	return m_pPipeLine->Get_TimeDelta();
}

HRESULT CGameInstance::Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pLightTag, const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	return m_pLightManager->Add_Light(pGraphicDevice, pContextDevice, pLightTag, pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc);
}

HRESULT CGameInstance::Add_Light_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pLightTag, const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	return m_pLightManager->Add_Light_ForTool(pGraphicDevice, pContextDevice, pLightTag, pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc);
}

HRESULT CGameInstance::Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pPrototypeTag, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	return m_pLightManager->Add_Light_Prototype(pGraphicDevice, pContextDevice, pPrototypeTag, pShaderFilePath, pTechniqueName);
}

HRESULT CGameInstance::Add_Light(const _tchar * pPrototypeLightTag, const _tchar * pLightTag, const LIGHTDESC & tLightDesc)
{
	return m_pLightManager->Add_Light(pPrototypeLightTag, pLightTag, tLightDesc);
}

void CGameInstance::Trace_SpotLight(const _tchar * pLightTag, _fvector vTargetPosition)
{
	m_pLightManager->Trace_SpotLight(pLightTag, vTargetPosition);
}

void CGameInstance::Trace_SpotLight_EyeToAt(const _tchar * pLightTag, _fvector vEye, _fvector vAt)
{
	m_pLightManager->Trace_SpotLight_EyeToAt(pLightTag, vEye, vAt);
}

HRESULT CGameInstance::Light_Active(const _tchar * pLightTag)
{
	return m_pLightManager->LightMap_Active(pLightTag);
}

HRESULT CGameInstance::Light_DeActive(const _tchar * pLightTag)
{
	return m_pLightManager->LightMap_DeActive(pLightTag);
}


CLight * CGameInstance::Get_Light(const _tchar * pLightTag, _uint iLightIndex)
{
	return m_pLightManager->Get_Light(pLightTag, iLightIndex);
}

LIGHTDESC * CGameInstance::Get_LightDesc(const _tchar * pLightTag, _uint iLightIndex)
{
	return m_pLightManager->Get_LightDesc(pLightTag, iLightIndex);
}

LIGHTDEPTHDESC* CGameInstance::Get_LightDepthDesc(const _tchar * pLightTag, _uint iLightIndex)
{
	return m_pLightManager->Get_LightDepthDesc(pLightTag, iLightIndex);
}

list<class CLight*>* CGameInstance::Get_LightList(const _tchar * pLightTag)
{
	return m_pLightManager->Find_LightMap(pLightTag);
}

void CGameInstance::ResetPosition_Shadow_DirectionLight(const _tchar * pLightTag, _fvector vPosition, _uint iLightIndex)
{
	m_pLightManager->ResetPosition_Shadow_DirectionLight(pLightTag, vPosition, iLightIndex);
}

void CGameInstance::Increase_LightColor(const _tchar* pLightTag, _float4 vIncreasePower, _float4 vMaxIncreasePower, _uint iColor)
{
	m_pLightManager->Increase_LightColor(pLightTag, vIncreasePower, vMaxIncreasePower, iColor);
}

void CGameInstance::Decrease_LightColor(const _tchar* pLightTag, _float4 vDecreasePower, _float4 vMinIncreasePower, _uint iColor)
{
	m_pLightManager->Decrease_LightColor(pLightTag, vDecreasePower, vMinIncreasePower, iColor);
}

void CGameInstance::Increase_InnerDegree(const _tchar * pLightTag, _float fIncreaseDegree, _float fMaxDegree)
{
	m_pLightManager->Increase_InnerDegree(pLightTag, fIncreaseDegree, fMaxDegree);
}

void CGameInstance::Decrease_InnerDegree(const _tchar * pLightTag, _float fDecreaseDegree)
{
	m_pLightManager->Decrease_InnerDegree(pLightTag, fDecreaseDegree);
}

void CGameInstance::Increase_OuterDegree(const _tchar * pLightTag, _float fIncreaseDegree, _float fMaxDegree)
{
	m_pLightManager->Increase_OuterDegree(pLightTag, fIncreaseDegree, fMaxDegree);
}

void CGameInstance::Decrease_OuterDegree(const _tchar * pLightTag, _float fDecreaseDegree)
{
	m_pLightManager->Decrease_OuterDegree(pLightTag, fDecreaseDegree);
}

void CGameInstance::Increase_Range(const _tchar * pLightTag, _float fIncreaseRange, _float fMaxRange)
{
	m_pLightManager->Increase_Range(pLightTag, fIncreaseRange, fMaxRange);
}

void CGameInstance::Decrease_Range(const _tchar * pLightTag, _float fDecreaseRange, _float fMinRange)
{
	m_pLightManager->Decrease_Range(pLightTag, fDecreaseRange, fMinRange);
}

HRESULT CGameInstance::Delete_LightIndex(const _tchar * pLightTag, _uint iLightIndex)
{
	return m_pLightManager->Delete_LightIndex(pLightTag, iLightIndex);
}

HRESULT CGameInstance::Delete_LightList(const _tchar * pLightTag)
{
	return m_pLightManager->Delete_LightList(pLightTag);
}

HRESULT CGameInstance::DeActiveAll()
{
	return m_pLightManager->DeActive_All();
}

HRESULT CGameInstance::Clear_Light()
{
	return m_pLightManager->Clear();
}

HRESULT CGameInstance::Add_RenderTarget(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex ,_float4 vClearColor)
{
	return m_pRenderTargetManager->Add_RenderTarget(pGraphicDevice, pContextDevice, pTargetTag, iWidth, iHeight, ePixelFormat, iRenderIndex, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	return m_pRenderTargetManager->Add_MRT(pMRTTag, pTargetTag);
}

ID3D11ShaderResourceView * CGameInstance::Get_SRV(const _tchar * pTargetTag)
{
	return m_pRenderTargetManager->Get_SRV(pTargetTag);
}

_bool CGameInstance::Picking(_fvector v0, _fvector v1, _fvector v2, _float fDistance)
{
	return m_pPicking->Picking(v0, v1, v2, fDistance);
}

_fvector CGameInstance::Get_Pivot()
{
	return m_pPicking->Get_Pivot();
}

_fvector CGameInstance::Get_Ray()
{
	return m_pPicking->Get_Ray();
}

_bool CGameInstance::Culling(CTransform * pTransform, _float fRadius)
{
	return m_pFrustum->Culling_Radius(pTransform, fRadius);
}

_bool CGameInstance::Culling(_fvector vPos, _matrix matWorld, _float fRadius)
{
	return m_pFrustum->Culling_PointRadius(vPos, matWorld, fRadius);
}

_bool CGameInstance::Culling(_fvector vPos, _float fRadius)
{
	return m_pFrustum->Culling_PointRadius(vPos, fRadius);
}

_bool CGameInstance::Culling(BoundingBox * pBoundingBox)
{
	return m_pFrustum->Culling_BoundingBox(pBoundingBox);
}

HRESULT CGameInstance::Create_Observer(const _tchar * pObserverTag, class CGameObject* pTarget)
{
	return m_pObserverManager->Create_Observer(pObserverTag, pTarget);
}

CObserver * CGameInstance::Get_Observer(const _tchar * pObserverTag)
{
	return m_pObserverManager->Get_Observer(pObserverTag);
}

HRESULT CGameInstance::Delete_Observer(const _tchar * pObserverTag)
{
	return m_pObserverManager->Delete_Observer(pObserverTag);
}

void CGameInstance::Release_Observer()
{
	m_pObserverManager->Release_Observer();
}

HRESULT CGameInstance::Create_Subject(const _tchar * pSubjectTag)
{
	return m_pSubjectManager->Create_Subject(pSubjectTag);
}

CSubject * CGameInstance::Get_Subject(const _tchar * pSubjectTag)
{
	return m_pSubjectManager->Get_Subject(pSubjectTag);
}

HRESULT CGameInstance::Delete_Subject(const _tchar * pSubjectTag)
{
	return m_pSubjectManager->Delete_Subject(pSubjectTag);
}

void CGameInstance::Release_Subject()
{
	m_pSubjectManager->Release_Subject();
}

HRESULT CGameInstance::Random_Range(int min, int max)
{
	return m_pRandomManager->Range(min,max);
}

int CGameInstance::Random()
{
	return m_pRandomManager->Random();
}

int CGameInstance::Random_From_10000(int min, int max)
{
	return m_pRandomManager->Random_From_10000(min, max);
}

HRESULT CGameInstance::Add_Font(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	return m_pFontManager->Add_Font(pGraphicDevice, pContextDevice, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, _float2 vPosition, _fvector vColor)
{
	return m_pFontManager->Render_Font(pFontTag, pText, vPosition, vColor);
}

void CGameInstance::Notice(const char * pNotify, _bool bLinebreak)
{
	m_pGraphicDebugger->Notice(pNotify, bLinebreak);
}

void CGameInstance::Request()
{
	m_pGraphicDebugger->Request();
}

void CGameInstance::Set_BulletLevel(_double dBulletLevel)
{
	m_dBulletLevel = dBulletLevel;
}

void CGameInstance::Set_BulletRemaining(_double dBulletTime)
{
	m_dBulletTime = dBulletTime;
}

void CGameInstance::Set_BulletTime(_bool bBulletTime)
{
	if (!bBulletTime)
	{
		m_dBulletTime = 0.0;
		m_dBulletLevel = 1.0;
	}
	m_bBulletTime = bBulletTime;
}

void CGameInstance::Set_Pause(_bool bPause)
{
	m_bPause = bPause;
	m_pGameObjectManager->Set_Pause(bPause);
}

void CGameInstance::Set_MiniGamePause(_bool bPause)
{
	m_bMiniGamePause = bPause;
	m_pGameObjectManager->Set_MiniGamePause(bPause);
}

void CGameInstance::Release_Engine()
{
	unsigned long dwRefCnt = 0;
	dwRefCnt = CGameInstance::Get_Instance()->Destroy_Instance();

	dwRefCnt = CLevelManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CPicking::Get_Instance()->Destroy_Instance();

	dwRefCnt = CFontManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CRandomManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CGameObjectManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CSubjectManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CObserverManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CComponentManager::Get_Instance()->Destroy_Instance();
	
	dwRefCnt = CRenderTargetManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CLightManager::Get_Instance()->Destroy_Instance();
	
	dwRefCnt = CFrustum::Get_Instance()->Destroy_Instance();

	dwRefCnt = CPipeLine::Get_Instance()->Destroy_Instance();

	dwRefCnt = CTimerManager::Get_Instance()->Destroy_Instance();

	dwRefCnt = CGraphicDebugger::Get_Instance()->Destroy_Instance();

	dwRefCnt = CInputDevice::Get_Instance()->Destroy_Instance();

	dwRefCnt = CGraphicDevice::Get_Instance()->Destroy_Instance();

}

void CGameInstance::Free()
{
	unsigned long dwRefCnt = 0;
	dwRefCnt = Safe_Release(m_pGraphicDebugger);

	dwRefCnt = Safe_Release(m_pTimerManager);

	/* Utility Manager */
	dwRefCnt = Safe_Release(m_pLevelManager);
	dwRefCnt = Safe_Release(m_pPipeLine);
	
	dwRefCnt = Safe_Release(m_pPicking);
	dwRefCnt = Safe_Release(m_pFrustum);
	dwRefCnt = Safe_Release(m_pRandomManager);
	dwRefCnt = Safe_Release(m_pFontManager);
	dwRefCnt = Safe_Release(m_pGameObjectManager);
	dwRefCnt = Safe_Release(m_pObserverManager);
	dwRefCnt = Safe_Release(m_pSubjectManager);
	dwRefCnt = Safe_Release(m_pComponentManager);
	dwRefCnt = Safe_Release(m_pLightManager);
	dwRefCnt = Safe_Release(m_pRenderTargetManager);
	dwRefCnt = Safe_Release(m_pGraphicDevice);
	dwRefCnt = Safe_Release(m_pInputDevice);
	
	m_hWnd = NULL;
}
