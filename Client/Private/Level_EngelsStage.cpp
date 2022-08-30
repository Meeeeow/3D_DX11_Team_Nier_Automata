#include "stdafx.h"
#include "..\Public\Level_EngelsStage.h"
#include "LoadDatFiles.h"
#include "Player.h"
#include "Event_Collider.h"
#include "Level_Loading.h"
#include "UI_HUD_Controller.h"
#include "SoundMgr.h"

CLevel_EngelsStage::CLevel_EngelsStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{
}

HRESULT CLevel_EngelsStage::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/EngelsStageNavi.dat"))))
		return E_FAIL;

	if (FAILED(Set_Player(pGameInstance)))
		return E_FAIL;

	CLoadDatFiles::Get_Instance()->Load_Object_Info(LEVEL::ROBOTGENERAL, L"EngelsScene");

	if (FAILED(Load_EventCollider(pGameInstance, L"../Bin/Data/EventColliderData/Engels.dat")))
		return E_FAIL;

	if (FAILED(Set_Light(pGameInstance)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::ROBOTGENERAL, TEXT("Prototype_GameObject_Sky"), L"Sky")))
		return E_FAIL;

	if (FAILED(Add_CollisionTag()))
		return E_FAIL;

	CUI_HUD_Controller::COMMANDDESC	tDesc;

	tDesc.iCommandFlag = COMMAND_HUD_ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));
	
	CSoundMgr::Get_Instance()->StopAll();
	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::ENVIRONMENT_SOUND, 0.2f);
	//CSoundMgr::Get_Instance()->PlayLoopSound(L"Engels_Environment.wav", CSoundMgr::CHANNELID::ENVIRONMENT_SOUND);
	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::ENVIRONMENT_SOUND, 0.2f);

	return S_OK;
}

_int CLevel_EngelsStage::Tick(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Down(DIK_8))
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

	if (pGameInstance->Key_Down(DIK_9))
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

	if (pGameInstance->Key_Down(DIK_0))
	{
		//CUI_HUD_Controller::COMMANDDESC	tDesc;

		//tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::ZHUANGZISTAGE, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (m_pNextLevel != nullptr)
		pGameInstance->Open_Level(m_pNextLevel);


	return _int();
}

HRESULT CLevel_EngelsStage::Render()
{
	return S_OK;
}

HRESULT CLevel_EngelsStage::Add_CollisionTag()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);					// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);			// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

/*	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::SPHERE);	

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::SPHERE);		*/	

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE,
		(_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE,
		(_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"BossPart", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Event_Collider", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::ROBOTGENERAL, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	// For Normal Attack : Must keep this order!!!
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::ROBOTGENERAL, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::ROBOTGENERAL, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
	}

	// For Weapon Sphere VS MonsterAttack Sphere
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::ROBOTGENERAL, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);
	}

	return S_OK;
}

HRESULT CLevel_EngelsStage::Set_Light(CGameInstance * pGameInstance)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;


	tLightDesc.vDiffuse = CProportionConverter()(_LAVENDERBLUSH, 80.f);
	tLightDesc.vAmbient = CProportionConverter()(_LAVENDERBLUSH, 75.f);
	tLightDesc.vSpecular = CProportionConverter()(_LAVENDERBLUSH, 0.f, true);
	tLightDesc.vDirection = _float4(-1.f, -1.f, 1.f, 0.f);

	tLightDesc.bLightOn = true;
	tLightDesc.eState = LIGHTSTATE::ALWAYS;

	LIGHTDEPTHDESC tDepthDesc;
	tDepthDesc.bDepth = true;
	tDepthDesc.fFar = 450.f;
	tDepthDesc.fNear = 0.2f;
	tDepthDesc.fFOV = XMConvertToRadians(60.f);
	tDepthDesc.fWidth = SHADOWMAP_WIDTH;
	tDepthDesc.fHeigth = SHADOWMAP_HEIGHT;

	tDepthDesc.vAt = _float4(0.f, 1.f, 0.f, 1.f);
	tDepthDesc.vEye = _float4(15.f, 45.f, -15.f, 1.f);
	tDepthDesc.vAxis = _float4(1.f, 1.f, 1.f, 0.f);


	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"ShadowDepth", Shader_Viewport, DefaultTechnique, tLightDesc, tDepthDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EngelsStage::Set_Player(CGameInstance * pGameInstance)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer"));
	if (pPlayer == nullptr)
		return E_FAIL;

	if (FAILED(pPlayer->SceneChange_ROBOTGENERAL()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EngelsStage::Load_EventCollider(CGameInstance * pGameInstance, const _tchar * pEventColliderDataFilePath)
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

		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, L"Prototype_GameObject_EventCollider", L"Event_Collider", &pEvent_Collider)))
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

CLevel_EngelsStage * CLevel_EngelsStage::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_EngelsStage* pGameInstance = new CLevel_EngelsStage(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct(iLevelID)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CLevel_EngelsStage");
	}

	return pGameInstance;
}

void CLevel_EngelsStage::Free()
{
	__super::Free();
}
