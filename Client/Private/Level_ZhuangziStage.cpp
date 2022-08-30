#include "stdafx.h"
#include "..\Public\Level_ZhuangziStage.h"
#include "LoadDatFiles.h"
#include "Player.h"
#include "Level_Loading.h"
#include "Event_Collider.h"
#include "ZhuangziBridge.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_Controller.h"
#include "Light.h"
#include "SoundMgr.h"

CLevel_ZhuangziStage::CLevel_ZhuangziStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{
}

HRESULT CLevel_ZhuangziStage::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/ZhuangziStageNavi.dat"))))
		return E_FAIL;

	if (FAILED(Set_Player(pGameInstance)))
		return E_FAIL;

	CLoadDatFiles::Get_Instance()->Load_Object_Info(LEVEL::ZHUANGZISTAGE, L"ZhuangziScene");

	CZhuangziBridge* pBridge = (CZhuangziBridge*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"ZhuangziBridge", 0);

	pBridge->Set_State(OBJSTATE::DISABLE);

	CUI_HUD_DirectionIndicator::UIINDICATORDESC	tIndiDesc;

	tIndiDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
	tIndiDesc.eObjState = OBJSTATE::ENABLE;
	tIndiDesc.iObjID = pBridge->Get_ObjID();

 	XMStoreFloat4(&tIndiDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pBridge->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tIndiDesc));

	if (FAILED(Load_EventCollider(pGameInstance, L"../Bin/Data/EventColliderData/ZhuangZi.dat")))
		return E_FAIL;

	OBJSTATUSDESC tDesc;
	tDesc.iAtt = ZHUANGZI_WHOLERANGEATTACK_DAMAGE;
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_WHOLERANGE, L"WholeRange", &tDesc)))
		return E_FAIL;

	if (FAILED(Set_Light(pGameInstance)))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::ZHUANGZISTAGE, TEXT("Prototype_GameObject_Sky"), L"Sky")))
		return E_FAIL;
	if (FAILED(Add_CollisionTag()))
		return E_FAIL;

	CSoundMgr::Get_Instance()->StopAll();
	CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::ENVIRONMENT_SOUND, 0.5f);
	//CSoundMgr::Get_Instance()->PlayLoopSound(L"ZhuangZi_Environment.wav", CSoundMgr::ENVIRONMENT_SOUND);
	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::ENVIRONMENT_SOUND, 0.5f);

	return S_OK;
}

_int CLevel_ZhuangziStage::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	TurnOn_DirectionLight(pGameInstance, dTimeDelta);
	Trace_SpotLight(pGameInstance);
	
#pragma region HotKey
	if (pGameInstance->Key_Down(DIK_8))
	{
		//CUI_HUD_Controller::COMMANDDESC	tDesc;

		//tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::STAGE1, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (pGameInstance->Key_Down(DIK_9))
	{
		CUI_HUD_Controller::COMMANDDESC	tDesc;

		tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;
		tDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::AMUSEMENTPARK, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (pGameInstance->Key_Down(DIK_0))
	{
		CUI_HUD_Controller::COMMANDDESC	tDesc;

		tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;
		tDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::OPERABACKSTAGE, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (pGameInstance->Key_Down(DIK_M))
	{
		//CUI_HUD_Controller::COMMANDDESC	tDesc;

		//tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::ROBOTGENERAL, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}
#pragma endregion SceneChange

	if (m_pNextLevel != nullptr)
		pGameInstance->Open_Level(m_pNextLevel);
	return _int();
}

HRESULT CLevel_ZhuangziStage::Render()
{
	return S_OK;
}

HRESULT CLevel_ZhuangziStage::Add_CollisionTag()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);					// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"WholeRange", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"WholeRange", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);			// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::SPHERE);				// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::SPHERE);				// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Event_Collider", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Zhuangzi_RingWave", (_uint)CCollider::TYPE::ZHUANGZI_RING);
	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ZHUANGZISTAGE, L"Zhuangzi_RingWave", (_uint)CCollider::TYPE::ZHUANGZI_RING);

	// For Normal Attack : Must keep this order!!!
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::ZHUANGZISTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::ZHUANGZISTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::ZHUANGZISTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
	}

	return S_OK;
}

HRESULT CLevel_ZhuangziStage::Set_Light(CGameInstance * pGameInstance)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;

	tLightDesc.vDiffuse = CProportionConverter()(_SNOW, 83.f);
	tLightDesc.vAmbient = CProportionConverter()(_SNOW, 84.f);
	tLightDesc.vSpecular = CProportionConverter()(_SNOW, 0.f, true);
	tLightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	tLightDesc.bLightOn = true;
	tLightDesc.eState = LIGHTSTATE::ALWAYS;


	LIGHTDEPTHDESC tDepthDesc;
	tDepthDesc.bDepth = true;
	tDepthDesc.fFar = 450.f;
	tDepthDesc.fNear = 0.2f;
	tDepthDesc.fFOV = XMConvertToRadians(60.f);
	tDepthDesc.fWidth = (_float)4096;
	tDepthDesc.fHeigth = (_float)4096;

	tDepthDesc.vAt = _float4(1.f, 1.f, 1.f, 1.f);
	tDepthDesc.vEye = _float4(-10.f, 55.f, -10.f, 1.f);
	tDepthDesc.vAxis = _float4(0.f, 1.f, 0.f, 0.f);


	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"ShadowDepth", Shader_Viewport, DefaultTechnique, tLightDesc, tDepthDesc)))
		return E_FAIL;


	LIGHTDESC tPointLightDesc;

	tPointLightDesc.eType = LIGHTDESC::TYPE::POINT;
	tPointLightDesc.vDiffuse = CProportionConverter()(_DARKSALMON, 100.f);
	tPointLightDesc.vAmbient = CProportionConverter()(_DARKSALMON, 100.f);
	tPointLightDesc.vSpecular = CProportionConverter()(_DARKSALMON, 0.f, true);
	tPointLightDesc.vPosition = _float4(508.f, -22.f, 226.f, 1.f);
	tPointLightDesc.fRange = 125.f;
	tPointLightDesc.bLightOn = true;
	tPointLightDesc.eState = LIGHTSTATE::ALWAYS;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"ZhuanziMagma", Shader_Viewport, DefaultTechnique, tPointLightDesc)))
		return E_FAIL;

	LIGHTDESC tSpotLightDesc;
	tSpotLightDesc.eType = LIGHTDESC::TYPE::SPOT;
	tSpotLightDesc.vDiffuse = CProportionConverter()(_SLATEGRAY, 92.f);
	tSpotLightDesc.vAmbient = CProportionConverter()(_SLATEGRAY, 75.f);
	tSpotLightDesc.vSpecular = CProportionConverter()(_SLATEGRAY, 0.f, true);
	tSpotLightDesc.vPosition = _float4(506.f, 67.75f, 212.f, 1.f);
	tSpotLightDesc.vTargetPosition = _float4(506.f, 48.f, 212.f, 1.f);

	tSpotLightDesc.bLightOn = true;
	tSpotLightDesc.eState = LIGHTSTATE::WAIT;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"BlackOutSpotLight", Shader_Viewport, DefaultTechnique, tSpotLightDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_ZhuangziStage::Set_Player(CGameInstance * pGameInstance)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer"));
	if (pPlayer == nullptr)
		return E_FAIL;

	if (FAILED(pPlayer->SceneChange_ZhuangziStage()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_ZhuangziStage::Load_EventCollider(CGameInstance * pGameInstance, const _tchar * pEventColliderDataFilePath)
{
	_ulong				dwByte = 0;
	_uint				iCount = 0;
	HANDLE				hFile = CreateFile(pEventColliderDataFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return E_FAIL;

	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iCount; ++i)
	{
		_tchar		szName[MAX_PATH]{};
		_float4		vPos{};
		CCollider::DESC		tColliderDesc{};

		ReadFile(hFile, szName, sizeof(_tchar)*MAX_PATH, &dwByte, nullptr);
		ReadFile(hFile, &vPos, sizeof(_float4), &dwByte, nullptr);
		ReadFile(hFile, &tColliderDesc, sizeof(CCollider::DESC), &dwByte, nullptr);

		CGameObject* pEvent_Collider = nullptr;

		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ZHUANGZISTAGE, L"Prototype_GameObject_EventCollider", L"Event_Collider", &pEvent_Collider)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		((CEvent_Collider*)pEvent_Collider)->Set_Name(szName);

		CTransform* pTransform = (CTransform*)pEvent_Collider->Get_Component(COM_KEY_TRANSFORM);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		CCollider* pCollider = (CCollider*)pEvent_Collider->Get_Component(COM_KEY_HITBOX);
		if (nullptr == pCollider)
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&vPos));
		pCollider->Set_ColliderDesc(tColliderDesc);
		((CEvent_Collider*)pEvent_Collider)->Set_Event_Number();

	}


	return S_OK;
}

void CLevel_ZhuangziStage::Trace_SpotLight(CGameInstance * pGameInstance)
{
	if (m_bSupply)
		return;

	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer"));
	if (nullptr != pPlayer)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(COM_KEY_TRANSFORM));
		if (pTransform != nullptr)
		{
			_vector vPosition = pTransform->Get_State(CTransform::STATE::POSITION);
			pGameInstance->Trace_SpotLight_EyeToAt(L"BlackOutSpotLight", XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 20.f), vPosition);
			
		}
	}
}

void CLevel_ZhuangziStage::TurnOn_DirectionLight(CGameInstance * pGameInstance, _double dTimeDelta)
{
	if (m_bSupply)
	{
		LIGHTDESC* pDirLight = pGameInstance->Get_LightDesc(L"ShadowDepth");
		if (pDirLight != nullptr)
		{
			pDirLight->eState = LIGHTSTATE::ALWAYS;
		}

		pGameInstance->Increase_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 8.5f * (_float)dTimeDelta), CProportionConverter()(_SNOW, 83.f), (_uint)CLight::COLOR::DIFFUSE);
		pGameInstance->Increase_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 7.8f * (_float)dTimeDelta), CProportionConverter()(_SNOW, 84.f), (_uint)CLight::COLOR::AMBIENT);
		pGameInstance->Increase_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 0.42f * (_float)dTimeDelta), CProportionConverter()(_SNOW, 30.f), (_uint)CLight::COLOR::SPECULAR);


		pGameInstance->Decrease_InnerDegree(L"BlackOutSpotLight", 0.7f * (_float)dTimeDelta);
		pGameInstance->Decrease_OuterDegree(L"BlackOutSpotLight", 0.98f * (_float)dTimeDelta);
		pGameInstance->Decrease_Range(L"ZhuanziMagma", 100.f * (_float)dTimeDelta, 125.f);

		LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"BlackOutSpotLight");
		pLightDesc->eState = LIGHTSTATE::WAIT;
	}
}

void CLevel_ZhuangziStage::Electricity_Supply()
{
	m_bSupply = true;
}

CLevel_ZhuangziStage * CLevel_ZhuangziStage::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_ZhuangziStage* pInstance = new CLevel_ZhuangziStage(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);
	return pInstance;
}

void CLevel_ZhuangziStage::Free()
{
	__super::Free();
}
