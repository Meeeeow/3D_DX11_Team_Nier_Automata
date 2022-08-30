#include "stdafx.h"
#include "..\Public\Level_OperaBackStage.h"
#include "LoadDatFiles.h"
#include "Player.h"
#include "SoundMgr.h"
#include "UI_Dialogue_EngageMessage.h"
#include "Beauvoir.h"
#include "ObjectPool.h"
#include "Level_Loading.h"
#include "Event_Collider.h"
#include "UI_HUD_Controller.h"

_bool CLevel_OperaBackStage::m_bCallScream = false;
_bool CLevel_OperaBackStage::m_bCallWarning = false;
_bool CLevel_OperaBackStage::m_bInitialEngage = false;
_bool CLevel_OperaBackStage::m_bStart = false;
_bool CLevel_OperaBackStage::m_bEngage_UI = false;

CLevel_OperaBackStage::CLevel_OperaBackStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
	, m_pMiniGameManager(CMinigameManager::Get_Instance())
{
	Safe_AddRef(m_pMiniGameManager);
}

HRESULT CLevel_OperaBackStage::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, CNavigation::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Data/NavigationData/BeauvoirNav.dat"))))
		return E_FAIL;

	if (FAILED(CLoadDatFiles::Get_Instance()->Load_Object_Info(LEVEL::OPERABACKSTAGE, L"AmusementPark_1st")))
		return E_FAIL;

	if (FAILED(Load_EventCollider(pGameInstance, L"../Bin/Data/EventColliderData/Beauvoir.dat")))
		return E_FAIL;

	if (FAILED(Set_Player(pGameInstance)))
		return E_FAIL;

	if (FAILED(Set_Light(pGameInstance)))
		return E_FAIL;

	//CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

	//pSoundMgr->StopAll();
	//pSoundMgr->PlayLoopSound(TEXT("BeauvoirBattle.ogg"), CSoundMgr::CHANNELID::BGM);
	//pSoundMgr->PlayLoopSound(TEXT("BeauvoirHacking.ogg"), CSoundMgr::CHANNELID::HACKING);
	//pSoundMgr->Set_Volume(CSoundMgr::HACKING, 0.f);

	RELEASE_INSTANCE(CGameInstance);
	
	if (FAILED(Add_CollisionTag()))
		return E_FAIL;

	CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();
	pSoundMgr->StopAll();

	pSoundMgr->FadeOut_Volume(CSoundMgr::BGM2);

	// MiniGame
	CMinigameManager::Get_Instance()->Setting(m_pGraphicDevice, m_pContextDevice);

	CUI_HUD_Controller::COMMANDDESC	tDesc;

	tDesc.iCommandFlag = COMMAND_HUD_ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

	return S_OK;
}

_int CLevel_OperaBackStage::Tick(_double dTimeDelta)
{

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (m_bStart)
	{
		if (m_bShowOnce == false)
		{
			CUI::UISTATE	eState = CUI::UISTATE::ENABLE;

			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CINEMATIC"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CINEMATIC, &eState));
		}

		CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();
		StageLighting_Director(dTimeDelta);

		if (false == m_bCallWarning)
			m_dEngageScreamTimeAcc += dTimeDelta;

		if (false == m_bCallScream)
		{
			pSoundMgr->StopAll();
			pSoundMgr->PlaySound(TEXT("BeauvoirEngage.ogg"), CSoundMgr::SCREAM);
			pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::SCREAM, 0.4f);
			m_bCallScream = true;
		}
		if (false == m_bInitialEngage)
		{
			if (m_dEngageScreamTimeMax < m_dEngageScreamTimeAcc)
			{
				//pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.4f);
				//pSoundMgr->PlayLoopSound(TEXT("BeauvoirBattle.ogg"), CSoundMgr::CHANNELID::BGM);
				//pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.4f);
				//pSoundMgr->PlayLoopSound(TEXT("BeauvoirHacking.ogg"), CSoundMgr::CHANNELID::HACKING);
				//pSoundMgr->Set_Volume(CSoundMgr::HACKING, 0.f);
				m_bInitialEngage = true;
			}
		}
		if (false == m_bCallWarning && m_bEngage_UI == true)
		{
			if (m_dEngageWarningTimeMax < m_dEngageScreamTimeAcc)
			{
				//CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

				CUI_Dialogue_EngageMessage::BOSSID	eID = CUI_Dialogue_EngageMessage::BOSSID::BEAUVOIR;

				pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eID));

				//pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eState));


				CUI::UISTATE	eState;

				eState = CUI::UISTATE::INACTIVATE;

				pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CINEMATIC"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CINEMATIC, &eState));

				m_bCallWarning = true;
				m_bShowOnce = true;
			}
		}
	}

	if (0 > m_pMiniGameManager->Tick(dTimeDelta))
		MSGBOX("CMiniGameManager->Tick < 0 : to Level->Tick");


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
		//tDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tDesc));

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

HRESULT CLevel_OperaBackStage::Render()
{
	return S_OK;
}

HRESULT CLevel_OperaBackStage::Add_CollisionTag()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::SPHERE);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"MonsterRayAttack", (_uint)CCollider::TYPE::RAY);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Bullet", (_uint)CCollider::TYPE::SPHERE
		, (_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	/*	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);*/

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Event_Collider", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY
		, (_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Monster", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"Boss", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"PodRayAttack", (_uint)CCollider::TYPE::RAY);

	// For.MiniGame_IronFist

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


	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut0", (_uint)CCollider::TYPE::DONUT0);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut1", (_uint)CCollider::TYPE::DONUT1);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut2", (_uint)CCollider::TYPE::DONUT2);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut0", (_uint)CCollider::TYPE::DONUT0);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut1", (_uint)CCollider::TYPE::DONUT1);

	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Donut2", (_uint)CCollider::TYPE::DONUT2);

	//Beauvoir_RingWave
	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Beauvoir_RingWave", (_uint)CCollider::TYPE::BEAUVOIR_RING);
	pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
		, (_uint)LEVEL::OPERABACKSTAGE, L"Beauvoir_RingWave", (_uint)CCollider::TYPE::BEAUVOIR_RING);

	// For Normal Attack : Must keep this order!!!
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::OPERABACKSTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"LayerPlayer", (_uint)CCollider::TYPE::AABB);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB
			, (_uint)LEVEL::OPERABACKSTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE);
		pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Android_9S", (_uint)CCollider::TYPE::AABB);
	}

	// For Weapon Sphere VS MonsterAttack Sphere
	{
		pGameInstance->Add_CollisionTag((_uint)LEVEL::OPERABACKSTAGE, L"MonsterAttack", (_uint)CCollider::TYPE::SPHERE
			, (_uint)LEVEL::STATIC, L"Weapon", (_uint)CCollider::TYPE::SPHERE);
	}

	return S_OK;
}

HRESULT CLevel_OperaBackStage::Set_Light(CGameInstance * pGameInstance)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eType = LIGHTDESC::TYPE::DIRECTIONAL;
	tLightDesc.vDiffuse = CProportionConverter()(_FLORALWHITE, 85.f);
	tLightDesc.vAmbient = CProportionConverter()(_FLORALWHITE, 65.f);
	tLightDesc.vSpecular = CProportionConverter()(_FLORALWHITE, 0.f, true);
	tLightDesc.vDirection = _float4(0.f, -1.f, 1.f, 0.f);

	tLightDesc.bLightOn = true;
	tLightDesc.eState = LIGHTSTATE::ALWAYS;

	LIGHTDEPTHDESC tDepthDesc;
	tDepthDesc.bDepth = true;
	tDepthDesc.fFar = 450.f;
	tDepthDesc.fNear = 0.2f;
	tDepthDesc.fFOV = XMConvertToRadians(60.f);
	tDepthDesc.fWidth = (_float)SHADOWMAP_WIDTH;
	tDepthDesc.fHeigth = (_float)SHADOWMAP_HEIGHT;

	tDepthDesc.vAt = _float4(1.f, 1.f, 1.f, 1.f);
	tDepthDesc.vEye = _float4(-10.f, 55.f, 10.f, 1.f);
	tDepthDesc.vAxis = _float4(0.f, 1.f, 0.f, 0.f);


	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"ShadowDepth", Shader_Viewport, DefaultTechnique, tLightDesc, tDepthDesc)))
		return E_FAIL;

	tLightDesc.eType = LIGHTDESC::TYPE::SPOT;
	tLightDesc.vDiffuse = CProportionConverter()(_GOLDENROD, 70.f);
	tLightDesc.vAmbient = CProportionConverter()(_GOLDENROD, 55.f);
	tLightDesc.vSpecular = CProportionConverter()(_GOLDENROD, 0.f, true);

	tLightDesc.eState = LIGHTSTATE::WAIT;

	tLightDesc.vPosition = _float4(21.f, 20.5f, 0.2f, 1.f);
	tLightDesc.vTargetPosition = _float4(0.f, 0.f, 0.f, 1.f);
	
	tLightDesc.fInnerDegree = 0.f;
	tLightDesc.fOuterDegree = 0.f;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Spot_Villain", Shader_Viewport, DefaultTechnique, tLightDesc)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Spot_Heroine", Shader_Viewport, DefaultTechnique, tLightDesc)))
		return E_FAIL;

	LIGHTDESC tStageLight;
	tStageLight.eType = LIGHTDESC::TYPE::SPOT;
	tStageLight.vDiffuse = CProportionConverter()(_DARKGOLDENROD, 80.f);
	tStageLight.vAmbient = CProportionConverter()(_DARKGOLDENROD, 75.f);
	tStageLight.vSpecular = CProportionConverter()(_DARKGOLDENROD, 0.f, true);

	tStageLight.vPosition = _float4(33.f, -5.7f, 16.7f, 1.f);
	tStageLight.vTargetPosition = _float4(43.5f, 6.8f, 9.2f, 1.f);
	tStageLight.eState = LIGHTSTATE::ALWAYS;

	tStageLight.fInnerDegree = 10.f;
	tStageLight.fOuterDegree = 15.f;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"StageSpotLight", Shader_Viewport, DefaultTechnique, tStageLight)))
		return E_FAIL;

	tStageLight.vPosition = _float4(34.4f, -5.7f, -15.4f, 1.f);
	tStageLight.vTargetPosition = _float4(43.6f, 7.3f, -9.4f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"StageSpotLight", Shader_Viewport, DefaultTechnique, tStageLight)))
		return E_FAIL;


	tStageLight.vDiffuse = CProportionConverter()(_DARKMAGENTA, 80.f);
	tStageLight.vAmbient = CProportionConverter()(_DARKMAGENTA, 75.f);
	tStageLight.vSpecular = CProportionConverter()(_DARKMAGENTA, 0.f, true);

	tStageLight.fInnerDegree = 11.f;
	tStageLight.fOuterDegree = 15.f;

	tStageLight.vPosition = _float4(53.f, 23.f, 10.f, 1.f);
	tStageLight.vTargetPosition = _float4(53.f , -4.2f, 10.3f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"StageSpotLight", Shader_Viewport, DefaultTechnique, tStageLight)))
		return E_FAIL;

	tStageLight.vPosition = _float4(53.f, 23.f, 0.92f, 1.f);
	tStageLight.vTargetPosition = _float4(53.f, -4.2f, 1.5f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"StageSpotLight", Shader_Viewport, DefaultTechnique, tStageLight)))
		return E_FAIL;

	tStageLight.vPosition = _float4(53.f, 23.f, -9.f, 1.f);
	tStageLight.vTargetPosition = _float4(53.f, -4.2f, -8.7f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"StageSpotLight", Shader_Viewport, DefaultTechnique, tStageLight)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_OperaBackStage::Set_Player(CGameInstance * pGameInstance)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer"));
	if (pPlayer == nullptr)
		return E_FAIL;

	pPlayer->SceneChange_OperaBackStage();

	return S_OK;
}

void CLevel_OperaBackStage::StageLighting_Director(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Light_To_Villain(pGameInstance, dTimeDelta);

	Light_To_Heroine(pGameInstance, dTimeDelta);

}

void CLevel_OperaBackStage::Light_To_Villain(CGameInstance* pGameInstance, _double dTimeDelta)
{
	CTransform* pTransform = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM));
	if (nullptr != pTransform)
	{
		pGameInstance->Trace_SpotLight(L"Spot_Villain", pTransform->Get_State(CTransform::STATE::POSITION));
		LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"Spot_Villain");
		if (pLightDesc == nullptr)
			return;

		if (pLightDesc->eState == LIGHTSTATE::ALWAYS)
		{
			pGameInstance->Increase_InnerDegree(L"Spot_Villain", (_float)dTimeDelta * 1.25f, 2.5f);
			pGameInstance->Increase_OuterDegree(L"Spot_Villain", (_float)dTimeDelta * 2.5f, 5.f);
		}
	}
	
}

void CLevel_OperaBackStage::Light_To_Heroine(CGameInstance* pGameInstance, _double dTimeDelta)
{
	CBeauvoir* pBeauvoir = dynamic_cast<CBeauvoir*>(pGameInstance->Get_GameObjectPtr(m_iCurrentLevelID, L"Boss"));
	if (nullptr != pBeauvoir)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(pBeauvoir->Get_Component(COM_KEY_TRANSFORM));
		if (nullptr != pTransform)
		{
			pGameInstance->Trace_SpotLight(L"Spot_Heroine", pTransform->Get_State(CTransform::STATE::POSITION));
			LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"Spot_Heroine");
			if (pLightDesc == nullptr)
				return;
			
			if (pLightDesc->eState == LIGHTSTATE::ALWAYS)
			{
				pGameInstance->Increase_InnerDegree(L"Spot_Heroine", (_float)dTimeDelta * 8.f, 8.f);
				pGameInstance->Increase_OuterDegree(L"Spot_Heroine", (_float)dTimeDelta * 13.f, 13.f);
			}
		}
	}
	else
	{
		pGameInstance->Decrease_InnerDegree(L"Spot_Heroine", (_float)dTimeDelta * 3.f);
		pGameInstance->Decrease_OuterDegree(L"Spot_Heroine", (_float)dTimeDelta * 6.f);
		LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"Spot_Heroine");
		if (nullptr != pLightDesc)
		{
			if (pLightDesc->fInnerDegree == 0.f)
				pLightDesc->eState = LIGHTSTATE::DISABLE;
		}
	}
}
HRESULT CLevel_OperaBackStage::Load_EventCollider(CGameInstance * pGameInstance, const _tchar * pEventColliderDataFilePath)
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

		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::OPERABACKSTAGE, L"Prototype_GameObject_EventCollider", L"Event_Collider", &pEvent_Collider)))
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
CLevel_OperaBackStage * CLevel_OperaBackStage::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_OperaBackStage* pInstance = new CLevel_OperaBackStage(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);
	return pInstance;
}

void CLevel_OperaBackStage::Free()
{
	__super::Free();

	Safe_Release(m_pMiniGameManager);
	CMinigameManager::Destroy_Instance();
}
