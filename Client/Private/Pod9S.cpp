#include "stdafx.h"
#include "..\Public\Pod9S.h"
#include "MainCamera.h"

CPod9S::CPod9S(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CGameObjectModel(pDevice, pDeviceContext)
{
}

CPod9S::CPod9S(const CPod9S & rhs)
	:CGameObjectModel(rhs)
{
}

HRESULT CPod9S::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPod9S::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	XMStoreFloat4x4(&m_ParentFloat4x4, XMMatrixIdentity());

	m_iAnimation = 27;
	m_bContinue = false;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCPod9S_InstanceID = OriginID_Pod9S;
	m_iObjID = iCPod9S_InstanceID++;

	
	Check_Pause();
	return S_OK;
}

_int CPod9S::Tick(_double TimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	m_pModel->Synchronize_Animation(m_pTransform);

	Check_State();
	Update(TimeDelta);
	Check_Teleport(TimeDelta);

	return (_int)m_eState;
}

_int CPod9S::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);

	Check_Idle();

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	return _int();
}

HRESULT CPod9S::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, 1);
	}

	return S_OK;
}

void CPod9S::Update_Position(_vector vPos)
{
	if (!XMVector4Equal(XMLoadFloat4(&m_vDestPos), vPos))
	{
		m_AccelTime = 0.0;
		m_TimeDelta = 0.0;
	}

	XMStoreFloat4(&m_vDestPos, vPos);

	m_bOnCoaster = false;
}

void CPod9S::Update_PositionForCoaster(_vector vPos)
{
	m_bOnCoaster = true;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CAndroid_9S* pPlayer = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);

	if (pPlayer == nullptr)
		return;

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("Android_9S"), TEXT("Com_Transform"), 0);

	if (pPlayerTransform == nullptr)
		return;

	_vector vPlayerRight, vPlayerUp, vPlayerLook;

	vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE::UP);
	vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vPlayerRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vPlayerUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vPlayerLook);

	XMStoreFloat4(&m_vDestPos, vPos);

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&m_vDestPos));
}

void CPod9S::Update(_double TimeDelta)
{
	if (m_bTeleport)
		return;

	if (m_bOnCoaster)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix		CamWorldMatrix = pCamera->Get_WorldMatrix();

	if (XMMatrixIsIdentity(CamWorldMatrix))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_float fAccelPower = 0.0f;
	_vector vAccellPower = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float fValue = (_float)TimeDelta * 2.f;
	_vector vMaxAccellPower = XMVectorSet(fValue, fValue, fValue, fValue);
	m_AccelTime += TimeDelta;

	if (m_AccelTime > 1.0)
		m_AccelTime = 1.0;

	vMaxAccellPower = XMVectorLerp(vMaxAccellPower, vAccellPower, (_float)m_AccelTime);

	m_TimeDelta += (TimeDelta * 0.01);

	if (m_TimeDelta > 1.0)
		m_TimeDelta = 1.0;

	_double Time = m_TimeDelta + XMVectorGetX(vMaxAccellPower);
	if (Time > 1.0)
		Time = 1.0;

	_float4x4	CamFloat4x4;
	XMStoreFloat4x4(&CamFloat4x4, CamWorldMatrix);

	_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);		// 초기화 하기
	_vector vUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	memcpy(&vRight, &CamFloat4x4.m[0], sizeof(_vector));
	memcpy(&vUp, &CamFloat4x4.m[1], sizeof(_vector));
	memcpy(&vLook, &CamFloat4x4.m[2], sizeof(_vector));

	_vector vLocalUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vLook = XMVector3Cross(vRight, vLocalUp);
	vUp = XMVector3Cross(vLook, vRight);

	_vector vDestPos = XMLoadFloat4(&m_vDestPos);
	vDestPos = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE::POSITION), vDestPos, (_float)Time);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vDestPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CPod9S::Check_State()
{
	if (PODSTATE::IDLE == m_ePodState)
		m_iAnimation = 27;
}

void CPod9S::Check_Idle()
{
	m_ePodState = PODSTATE::IDLE;
}

HRESULT CPod9S::SceneChange_AmusementPark()
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	return S_OK;
}

HRESULT CPod9S::SceneChange_OperaBackStage()
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, OPERA_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pTransform->Set_State(CTransform::STATE::POSITION, OPERA_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	return S_OK;
}

HRESULT CPod9S::SceneChange_ZhuangziStage()
{
	/*Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		m_mapComponents.erase(iter);
	}
*/

	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}


	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());
	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));

	return S_OK;
}

HRESULT CPod9S::SceneChange_ROBOTGENERAL()
{
	Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	Safe_Release(iter->second);
	m_mapComponents.erase(iter);

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pTransform->Set_State(CTransform::STATE::POSITION, ENGELS_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	return S_OK;
}

void CPod9S::Set_Position_Immediately(_vector vPos)
{
	if (nullptr != m_pTransform)
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
}

void CPod9S::Set_Teleport(_bool bCheck)
{
	m_bTeleport = bCheck;
}

void CPod9S::Check_Teleport(_double TimeDelta)
{
	if (m_bTeleport)
	{
		if (!m_bAppearance)
			m_fAlpha -= (_float)TimeDelta;
		else
			m_fAlpha += (_float)TimeDelta;

		if (0 >= m_fAlpha)
			m_bAppearance = true;
		else if (m_fAlpha >= 1.f && m_bAppearance)
		{
			m_fAlpha = 1.f;

			if (m_bAppearance)
			{
				m_bAppearance = false;
				m_bTeleport = false;
			}
		}
	}
}

void CPod9S::Set_PodState(PODSTATE eState)
{
	m_ePodState = eState;
}

HRESULT CPod9S::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_POD9S, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	/* For.Com_Navigation */
	_uint iCurIndex = 0;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurIndex)))
		return E_FAIL;

	return S_OK;
}

CPod9S * CPod9S::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPod9S* pGameInstance = new CPod9S(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPod9S");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CPod9S::Clone(void * pArg)
{
	CPod9S* pGameInstance = new CPod9S(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPod9S");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPod9S::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pNavigation);
}
