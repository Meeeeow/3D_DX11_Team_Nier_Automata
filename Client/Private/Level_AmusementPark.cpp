#include "stdafx.h"
#include "..\Public\Level_AmusementPark.h"
#include "LoadDatFiles.h"
#include "Level_Loading.h"
#include "Player.h"
#include "Event_Collider.h"
#include "EffectFactory.h"
#include "Clown_Fly.h"
#include "UI_HUD_Controller.h"
#include "UI_HUD_StageObjective.h"
#include "SoundMgr.h"

CLevel_AmusementPark::CLevel_AmusementPark(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{

}

HRESULT CLevel_AmusementPark::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/AmusementParkNav.dat"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_COASTER, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/CoasterNavi.dat"))))
		return E_FAIL;

	if (FAILED(Load_EventCollider(pGameInstance, L"../Bin/Data/EventColliderData/AmusementPark.dat")))
		return E_FAIL;

	if (FAILED(Make_Flyers(pGameInstance)))
		return E_FAIL;

	if (FAILED(Add_CollisionTag(pGameInstance)))
		return E_FAIL;

	if (FAILED(Set_Light(pGameInstance)))
		return E_FAIL;

	if (FAILED(Set_Sky(pGameInstance)))
		return E_FAIL;

	if (FAILED(Set_Player(pGameInstance)))
		return E_FAIL;

//	if (FAILED(Add_Effects(pGameInstance)))
//		return E_FAIL;

	CLoadDatFiles::Get_Instance()->Load_Object_Info(LEVEL::AMUSEMENTPARK, L"AmusementPark_2nd");

	CUI_HUD_StageObjective::UIOBJECTIVEDESC	tDesc;

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::TRESUREBOX;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 4;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::CHARIOT;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 1;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::BEAUVOIR;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 1;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	RELEASE_INSTANCE(CGameInstance);

	CSoundMgr::Get_Instance()->StopAll();

	CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.6f);
	//CSoundMgr::Get_Instance()->PlayLoopSound(L"AmuseParkBGM.mp3", CSoundMgr::CHANNELID::BGM);
	//CSoundMgr::Get_Instance()->PauseBGM(true);
	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.6f);
	//CSoundMgr::Get_Instance()->PauseBGM(false);

	return S_OK;
}

_int CLevel_AmusementPark::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	if (m_dTimeAccUIActivate < 1.0)
	{
		m_dTimeAccUIActivate += dTimeDelta;
	}
	else if (false == m_bOnceUIActivate)
	{
		CUI_HUD_Controller::COMMANDDESC	tDesc;

		tDesc.iCommandFlag = COMMAND_HUD_ENABLE;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		m_bOnceUIActivate = true;
	}

	if (pGameInstance->Key_Down(DIK_8))
	{
		CUI_HUD_Controller::COMMANDDESC	tDesc;

		tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;
		tDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::STAGE1, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (pGameInstance->Key_Down(DIK_9))
	{
		//CUI_HUD_Controller::COMMANDDESC	tDesc;

		//tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::OPERABACKSTAGE, (_uint)LEVEL::LOADING);
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

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::ZHUANGZISTAGE, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (pGameInstance->Key_Down(DIK_M))
	{
		CUI_HUD_Controller::COMMANDDESC	tDesc;

		tDesc.iCommandFlag |= COMMAND_HUD_DISABLE;
		tDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::ROBOTGENERAL, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		m_pNextLevel = pLevel;
	}

	if (m_pNextLevel != nullptr)
		pGameInstance->Open_Level(m_pNextLevel);

	return _int();
}

HRESULT CLevel_AmusementPark::Render()
{
	return S_OK;
}

HRESULT CLevel_AmusementPark::Add_CollisionTag(CGameInstance* pGameInstance)
{
	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"MonsterRayAtt8ack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Event_Collider", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::AMUSEMENTPARK, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	// For Tank
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Bullet", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::AMUSEMENTPARK, L"Bullet", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"Boss", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::AMUSEMENTPARK, L"Monster", (_uint)CCollider::TYPE::AABB);
	}

	// For Normal Attack : Must keep this order!!!
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::AMUSEMENTPARK, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::AMUSEMENTPARK, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
	}

	// For Weapon Sphere VS MonsterAttack Sphere
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::AMUSEMENTPARK, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);
	}

	return S_OK;
}

HRESULT CLevel_AmusementPark::Set_Light(CGameInstance * pGameInstance)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;


	tLightDesc.vDiffuse = CProportionConverter()(_LAVENDERBLUSH, 95.f);
	tLightDesc.vAmbient = CProportionConverter()(_LAVENDERBLUSH, 83.f);
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
	tDepthDesc.vAxis = _float4(0.f, 1.f, 0.f, 0.f);


	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"ShadowDepth", Shader_Viewport, DefaultTechnique, tLightDesc, tDepthDesc)))
		return E_FAIL;

	



	return S_OK;
}

HRESULT CLevel_AmusementPark::Set_Player(CGameInstance * pGameInstance)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer"));
	if (nullptr != pPlayer)
	{
		pPlayer->SceneChange_AmusementPark();
	}

	return S_OK;
}

HRESULT CLevel_AmusementPark::Set_Sky(CGameInstance * pGameInstance)
{
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::AMUSEMENTPARK, TEXT("Prototype_GameObject_Sky"), L"Sky")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_AmusementPark::Load_EventCollider(CGameInstance * pGameInstance, const _tchar* pEventColliderDataFilePath)
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

		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::AMUSEMENTPARK, L"Prototype_GameObject_EventCollider", L"Event_Collider", &pEvent_Collider)))
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

HRESULT CLevel_AmusementPark::Add_Effects(CGameInstance * pGameInstance)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET		pEffectPacket;
	CGameObject*		pGameObj = nullptr;


	pEffectPacket.eLevel = LEVEL::AMUSEMENTPARK;
	pEffectPacket.fMaxLifeTime = -1.f;

	pEffectPacket.tEffectMovement.vPosition = _float4(263.4f, 56.4f, 117.5f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_BLUE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Blue.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(263.4f, 56.4f, 99.4f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_GREEN;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Green.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(263.4f, 56.4f, 81.9f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_ORANGE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Orange.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	//////////////////

	pEffectPacket.tEffectMovement.vPosition = _float4(223.f, 52.f, 238.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_PURPLE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Purple.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(245.f, 30.f, 292, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_RED;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Red.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(358.f, 22.f, 225.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_YELLOW;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Yellow.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}
	pEffectPacket.tEffectMovement.vPosition = _float4(336.f, 22.f, 229.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_BLUE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Blue.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(283.f, 54.f, 205.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_GREEN;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Green.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(287.f, 64.f, 178, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_ORANGE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Orange.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(258.f, 20.f, 23.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_PURPLE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Purple.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(249.f, 20.f, -32.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_RED;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Red.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}


	pEffectPacket.tEffectMovement.vPosition = _float4(318.f, 22.f, -24.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_YELLOW;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Yellow.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(340.f, 53.f, -19.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_BLUE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Blue.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(371.f, 22.f, -5.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_GREEN;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Green.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}

	pEffectPacket.tEffectMovement.vPosition = _float4(371.f, 22.f, 20.f, 1.f);
	pEffectPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_ORANGE;
	pGameObj = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Orange.dat", &pEffectPacket);
	if (nullptr == pGameObj)
	{
		assert(FALSE);
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_AmusementPark::Make_Flyers(CGameInstance * pGameInstance)
{
	CClown_Fly* pMonster = nullptr;
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	CTransform* pTransform = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly", L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(1.f, 1000.f, 1.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Wave(CClown_Fly::WAVE::WAVE_1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);

	++CClown_Fly::m_iPatternNumber;


	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly", L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(0.f, 1000.f, 0.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Wave(CClown_Fly::WAVE::WAVE_1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);

	++CClown_Fly::m_iPatternNumber;


	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly", L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(6.f, 1000.f, -6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Wave(CClown_Fly::WAVE::WAVE_0);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);

	++CClown_Fly::m_iPatternNumber;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly", L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-6.f, 1000.f, -6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Wave(CClown_Fly::WAVE::WAVE_0);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);

	++CClown_Fly::m_iPatternNumber;
	
	return S_OK;
}

CLevel_AmusementPark * CLevel_AmusementPark::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_AmusementPark* pInstance = new CLevel_AmusementPark(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_AmusementPark::Free()
{
	__super::Free();
}
