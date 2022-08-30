#include "stdafx.h"
#include "..\Public\Level_Stage1.h"
#include "Camera.h"
#include "Player.h"
#include "Katana.h"
#include "LongKatana.h"
#include "Spear.h"
#include "Gauntlet.h"
#include "Pod.h"
#include "Pod9S.h"
#include "Android_9S.h"
#include "LoadDatFiles.h"
#include "Humanoid_Big.h"
#include "Tank.h"
#include "Humanoid_Dragoon.h"
#include "Beauvoir.h"
#include "Beauvoir_Skirt.h"
#include "Beauvoir_Hair.h"
#include "Humanoid_Clown.h"
#include "Humanoid_Small.h"
#include "VendingMachine.h"

#include "Level_Loading.h"
#include "Level_OperaBackStage.h"
#include "SoundMgr.h"

#include "UI_HUD_DirectionIndicator.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_HUD_StageObjective.h"
#include "UI_HUD_Controller.h"

//temp
#include "Zhuangzi.h"
#include "Engels.h"
//

int CLevel_Stage1::m_iMonsterCountStatic = 0;

CLevel_Stage1::CLevel_Stage1(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{
}

HRESULT CLevel_Stage1::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/BattleFieldNav.dat"))))
		return E_FAIL;

	CLoadDatFiles::Get_Instance()->Load_Object_Info(LEVEL::STAGE1, L"Scene1");

	if (FAILED(Load_Light(L"ShadowDepth", pGameInstance)))
		return E_FAIL;

	if (FAILED(Load_Player(pGameInstance)))
		return E_FAIL;

	if (FAILED(Add_CollisionTag()))
		return E_FAIL;

	if (FAILED(Make_Monster_Wave1(pGameInstance)))
		return E_FAIL;

	if (FAILED(Make_Monster_Wave2(pGameInstance)))
		return E_FAIL;

	CUI_HUD_StageObjective::UIOBJECTIVEDESC	tDesc;

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::TRESUREBOX;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 2;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::ZHUANGZI;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 1;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	tDesc.eQuest = CUI_HUD_StageObjective::QUEST::ENGELS;
	tDesc.iEventCount = 0;
	tDesc.iEventCountMax = 1;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tDesc));

	CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

	pSoundMgr->StopAll();
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.2f);
	//pSoundMgr->PlayLoopSound(TEXT("Stage1_Battle_BGM.mp3"), CSoundMgr::CHANNELID::BGM);
	//pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.2f);

	return S_OK;
}

_int CLevel_Stage1::Tick(_double dTimeDelta)  
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_bOnce == false)
	{
		m_iMonsterCount_W0 = m_iMonsterCountStatic;
		m_bOnce = true;
	}
	
	//// For Test
	//if (pGameInstance->Key_Down(DIK_NUMPAD9))
	//{
	//	CTrailInstance::TRAIL_INST_DESC tTrailInstDesc;
	//	ZeroMemory(&tTrailInstDesc, sizeof(CTrailInstance::TRAIL_INST_DESC));
	//	tTrailInstDesc.fEntireLifeTime = 2.f;
	//	tTrailInstDesc.fDensity = 0.f;
	//	tTrailInstDesc.fDirectionDensity = 2.f;
	//	tTrailInstDesc.fLifeTime = 2.f;
	//	tTrailInstDesc.fSpeed = 10.f;
	//	tTrailInstDesc.fSpeedDensity = 2.f;
	//	tTrailInstDesc.iNumTrails = 50;
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, L"Prototype_GameObject_TrailInstance", L"Layer_Effect", &tTrailInstDesc)))
	//		assert(false);
	//}

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

	if (m_iMonsterCount_W0 == 0 && !m_bWave1)
	{
		Active_Monster_Wave1(pGameInstance);
		m_bWave1 = true;
	}

	if (m_iMonsterCount_W1 == 0 && !m_bWave2)
	{
		Active_Monster_Wave2(pGameInstance);
		m_bWave2 = true;
	}
	if (m_iMonsterCount_W2 == 0 && m_bWave2)
	{
		CVendingMachine* pInstance = dynamic_cast<CVendingMachine*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STAGE1, L"Environment"));
		if (pInstance != nullptr)
		{
			pInstance->Set_AlphaTest();
		}

		if (false == m_bCallDirectionUI)
		{
			CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

			tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
			tDesc.eObjState = OBJSTATE::ENABLE;
			tDesc.iObjID = pInstance->Get_ObjID();

			XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pInstance->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

			pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));
		
			m_bCallDirectionUI = true;

			CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(0, 0, CUI_Dialogue_SystemMessage::iMsgOnQuest);

			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
		}
	}

	if (m_pNextLevel != nullptr)
		pGameInstance->Open_Level(m_pNextLevel);

	return _int();
}

HRESULT CLevel_Stage1::Render()
{
	return S_OK;
}

HRESULT CLevel_Stage1::Add_CollisionTag()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);					// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"WholeRange", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"WholeRange", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);			// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::SPHERE);				// For. Zhuangzi

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STAGE1, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	// For IronFist
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, MINIGAME_PLAYER, (_uint)CCollider::TYPE::AABB);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_WEAPON, (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::AABB);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, MINIGAME_WEAPON, (_uint)CCollider::TYPE::SPHERE);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, MINIGAME_PLAYER, (_uint)CCollider::TYPE::AABB);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_PLAYER, (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::SPHERE);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_PLAYER, (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::AABB);

		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, MINIGAME_MONSTER, (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STATIC, MINIGAME_PLAYER, (_uint)CCollider::TYPE::SPHERE);
	}

	// For Normal Attack : Must keep this order!!!
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STAGE1, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::STAGE1, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
	}

	// For Weapon Sphere VS MonsterAttack Sphere
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STAGE1, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);
	}

	return S_OK;
}

HRESULT CLevel_Stage1::Load_Light(const _tchar* pLayerTag, CGameInstance* pGameInstance)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;
	tLightDesc.vDiffuse = CProportionConverter()(_ALICEBLUE, 93.f);
	tLightDesc.vAmbient = CProportionConverter()(_ALICEBLUE, 88.f);
	tLightDesc.vSpecular = CProportionConverter()(_ALICEBLUE, 0.0f, true);
	tLightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

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
	tDepthDesc.vEye = _float4(-15.f, 30.f, -15.f, 1.f);
	tDepthDesc.vAxis = _float4(0.f, 1.f, 0.f, 0.f);
	

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, pLayerTag, Shader_Viewport, DefaultTechnique, tLightDesc, tDepthDesc)))
		return E_FAIL;


	tLightDesc.eType = LIGHTDESC::TYPE::SPOT;
	tLightDesc.vPosition = _float4(-1.f, 5.f, -1.f, 1.f);
	tLightDesc.vTargetPosition = _float4(0.f, 0.f, 0.f, 1.f);
	tLightDesc.vDiffuse = _SALMON;
	tLightDesc.vAmbient = CProportionConverter()(_SALMON, 86.f);
	tLightDesc.vSpecular = CProportionConverter()(_SALMON, 0.f, true);
 	tLightDesc.fInnerDegree = 5.5f;
	tLightDesc.fOuterDegree = 10.5f;
	tLightDesc.fLength = 7.f;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Test_SpotLight", Shader_Viewport, DefaultTechnique, tLightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Load_Player(CGameInstance * pGameInstance)
{
	CPlayer* pPlayer = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_PLAYER, L"LayerPlayer", (CGameObject**)&pPlayer)))
		return E_FAIL;

	if (!pPlayer)
		return E_FAIL;

	CKatana* pKatana = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_KATANA, L"Weapon", (CGameObject**)&pKatana)))
		return E_FAIL;

	if (!pKatana)
		return E_FAIL;

	pPlayer->Set_Katana(pKatana);

	CLongKatana*	pLongKatana = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_LONGKATANA, L"Weapon", (CGameObject**)&pLongKatana)))
		return E_FAIL;

	if (!pLongKatana)
		return E_FAIL;

	pPlayer->Set_LongKatana(pLongKatana);

	CSpear*			pSpear = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_SPEAR, L"Weapon", (CGameObject**)&pSpear)))
		return E_FAIL;

	if (!pSpear)
		return E_FAIL;

	pSpear->Set_OwnerLayerTag(L"LayerPlayer");
	pPlayer->Set_Spear(pSpear);

	CGauntlet*		pGauntlet = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_GAUNTLET, L"Weapon", (CGameObject**)&pGauntlet)))
		return E_FAIL;

	if (!pGauntlet)
		return E_FAIL;

	pPlayer->Set_Gauntlet(pGauntlet);

	CPod*			pPod = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_POD, L"Pod", (CGameObject**)&pPod)))
		return E_FAIL;

	if (!pPod)
		return E_FAIL;

	pPlayer->Set_Pod(pPod);

	// 9S
	CAndroid_9S*	pAndroid_9S = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_9S, L"Android_9S", (CGameObject**)&pAndroid_9S)))
		return E_FAIL;

	if (!pAndroid_9S)
		return E_FAIL;

	pPlayer->Set_Android9S(pAndroid_9S);

	pSpear = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_SPEAR, L"Weapon", (CGameObject**)&pSpear)))
		return E_FAIL;

	if (!pSpear)
		return E_FAIL;

	pSpear->Set_OwnerLayerTag(L"Android_9S");
	pSpear->Set_Att(ANDROID_9S_SPEAR_DAMAGE);
	pAndroid_9S->Set_Spear(pSpear);

	CPod9S*		pPod9S = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_POD9S, L"Pod", (CGameObject**)&pPod9S)))
		return E_FAIL;

	pAndroid_9S->Set_Pod(pPod9S);

	CCamera::DESC tDesc;
	tDesc.tTransformDesc.dSpeedPerSec = 10.f;
	tDesc.tTransformDesc.dRotationPerSec = 10.f;

	tDesc.fAspect = g_iWinCX / (_float)g_iWinCY;
	tDesc.fFar = 200.f;
	tDesc.fNear = 0.2f;
	tDesc.vAt = _float3(0.f, 0.f, 0.f);
	tDesc.vEye = _float3(0.f, 2.f, -5.f);
	tDesc.vAxis = _float3(0.f, 1.f, 0.f);
	tDesc.fFOV = XMConvertToRadians(60.f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, PROTO_KEY_CAMERA, L"Camera", &tDesc)))
		return E_FAIL;

	tDesc.tTransformDesc.dSpeedPerSec = 15.f;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_Camera_Debug"), L"Camera_Debug", &tDesc)))
		return E_FAIL;

	tDesc.fFar = 500.f; 

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_Camera_Cutscene"), L"Camera_Cutscene", &tDesc)))
		return E_FAIL;

	tDesc.fFar = 150.f;
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, PROTO_KEY_IRONFIST_CAMERA, MINIGAME_IRONFIST_CAMERA, &tDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Stage1::Active_Monster_Wave1(CGameInstance* pGameInstance)
{
	list<CGameObject*>* pMonsterList = pGameInstance->Get_ObjectList((_uint)LEVEL::STAGE1, L"Monster");
	
	if (nullptr == pMonsterList)
		return S_OK;

	for (auto& pMonster : *pMonsterList)
	{
		_uint iObjID = pMonster->Get_ObjID();

		if (iObjID >= 500) // humanoid big
		{
			if (dynamic_cast<CHumanoid_Big*>(pMonster)->Get_Wave() == CHumanoid_Big::WAVE::WAVE1)
			{
				dynamic_cast<CHumanoid_Big*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
				pMonster->Set_Collision(true);
			}

			else
			{
				continue;
			}
		}

		else if (iObjID >= 100 && iObjID < 500) // humanoid small
		{
			if (dynamic_cast<CHumanoid_Small*>(pMonster)->Get_Wave() == CHumanoid_Small::WAVE::WAVE1)
			{
				dynamic_cast<CHumanoid_Small*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
				pMonster->Set_Collision(true);
			}
			
			else
			{
				continue;
			}
		}
	}
	
	return S_OK;
}

HRESULT CLevel_Stage1::Active_Monster_Wave2(CGameInstance * pGameInstance)
{
	list<CGameObject*>* pMonsterList = pGameInstance->Get_ObjectList((_uint)LEVEL::STAGE1, L"Monster");

	if (nullptr == pMonsterList)
		return S_OK;

	for (auto& pMonster : *pMonsterList)
	{
		_uint iObjID = pMonster->Get_ObjID();

		if (iObjID >= 500) // humanoid big
		{
			if (dynamic_cast<CHumanoid_Big*>(pMonster)->Get_Wave() == CHumanoid_Big::WAVE::WAVE2)
			{
				dynamic_cast<CHumanoid_Big*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
				pMonster->Set_Collision(true);
			}

			else
			{
				continue;
			}
		}

		else if (iObjID >= 100 && iObjID < 500) // humanoid small
		{
			if (dynamic_cast<CHumanoid_Small*>(pMonster)->Get_Wave() == CHumanoid_Small::WAVE::WAVE2)
			{
				dynamic_cast<CHumanoid_Small*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
				pMonster->Set_Collision(true);
			}

			else
			{
				continue;
			}
		}
	}

	return S_OK;
}

HRESULT CLevel_Stage1::Make_Monster_Wave1(CGameInstance * pGameInstance)
{
	CHumanoid_Small* pMonster = nullptr;
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	CTransform* pTransform = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(2.f, 8.f, 2.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W1();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-2.f, 8.f, -2.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W1();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(6.f, 8.f, 6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W1();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-6.f, 8.f, -6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE1);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W1();

	CHumanoid_Big* pBigMonster = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_BIG, L"Monster", (CGameObject**)&pBigMonster)))
		return E_FAIL;

	pTransform = pBigMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(6.f, 8.f, -6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pBigMonster->Set_Jump_True();
	pBigMonster->Set_Wave(CHumanoid_Big::WAVE::WAVE1);
	pBigMonster->Set_Start_False();
	pBigMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W1();

	return S_OK;
}

HRESULT CLevel_Stage1::Make_Monster_Wave2(CGameInstance * pGameInstance)
{
	CHumanoid_Small* pMonster = nullptr;
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	CTransform* pTransform = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-2.f, 8.f, 2.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE2);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W2();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-2.f, 8.f, 2.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE2);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W2();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(6.f, 8.f,- 6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE2);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W2();

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_SMALL, L"Monster", (CGameObject**)&pMonster)))
		return E_FAIL;

	pTransform = pMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-6.f, 8.f, -6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pMonster->Set_Jump_True();
	pMonster->Set_Wave(CHumanoid_Small::WAVE::WAVE2);
	pMonster->Set_Start_False();
	pMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W2();

	CHumanoid_Big* pBigMonster = nullptr;

	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STAGE1, PROTO_KEY_HUMANOID_BIG, L"Monster", (CGameObject**)&pBigMonster)))
		return E_FAIL;

	pTransform = pBigMonster->Get_TransformCom();

	if (pTransform == nullptr)
		return E_FAIL;

	vPos = XMVectorSet(-6.f, 8.f, 6.f, 1.f);
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	pBigMonster->Set_Jump_True();
	pBigMonster->Set_Wave(CHumanoid_Big::WAVE::WAVE2);
	pBigMonster->Set_Start_False();
	pBigMonster->Set_State(OBJSTATE::DISABLE);
	pMonster->Set_Collision(false);

	Plus_MonsterCount_W2();

	return S_OK;
}

CLevel_Stage1 * CLevel_Stage1::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_Stage1* pInstance = new CLevel_Stage1(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Stage1::Free()
{
	__super::Free();
}
