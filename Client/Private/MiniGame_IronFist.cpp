#include "stdafx.h"
#include "..\Public\MiniGame_IronFist.h"
#include "Iron_Android9S.h"
#include "MinigameManager.h"
#include "CameraMgr.h"
#include "IronGauntlet.h"
#include "MiniGameBuilding.h"
#include "Iron_Humanoid_Small.h"
#include "SoundMgr.h"
#include "UI_HUD_Hpbar_MiniGame.h"
#include "Sky.h"

CMiniGame_IronFist::CMiniGame_IronFist(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CMiniGame(pGraphicDevice, pContextDevice)
{
}

_int CMiniGame_IronFist::Tick(_double dTimeDelta)
{
	Check_Start(dTimeDelta);
	Check_End(dTimeDelta);

	return _int();
}

void CMiniGame_IronFist::Check_Start(_double dTimeDelta)
{
	if (m_bEnd)
		return;

	if (m_ScreenTimeAcc < m_ScreenTimeAccMax)
	{
		if (m_ScreenTimeAcc == 0.0)
		{
			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_MINIGAMESCREEN"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAMESCREEN, nullptr));
		}

		CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::BGM);
		CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::HACKING);

		m_ScreenTimeAcc += dTimeDelta;
	}

	if (m_bStartMessageOn)
	{
		m_HackingStartTime += dTimeDelta;

		CUI::UISTATE	eState = CUI::UISTATE::ENABLE;

		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR_STATE, &eState));
	}	

	if (m_HackingStartTime >= 4.5)
	{
		for (auto& Pair : m_Layers)
			Pair.second->Set_State(OBJSTATE::ENABLE);
	}

	if (m_bStart)
		return;

	if (m_ScreenTimeAccMax <= m_ScreenTimeAcc)	// Screen End
	{
		CMinigameManager* pManager = CMinigameManager::Get_Instance();
		pManager->Start_MiniGame();
		m_bStart = true;
	}
}

void CMiniGame_IronFist::Check_End(_double dTimeDelta)
{
	if (false == m_bEnd)
		return;

	m_HackingEndTime += dTimeDelta;

	if (m_HackingEndTime >= 0.7)
	{
		if (false == m_bEndMessageOn)
		{
			CSoundMgr::Get_Instance()->PlaySound(L"KO.wav", CSoundMgr::CHANNELID::HACKING_FINISHED);
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_HACKING_SSC"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HACKINGSSCMSG, nullptr));

			m_bEndMessageOn = true;

			CUI::UISTATE	eState = CUI::UISTATE::DISABLE;

			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR_STATE, &eState));
		}
		CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::BGM);
		CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::HACKING);
	}

	if (m_HackingEndTime >= 3.4)
	{
		m_bEnd = false;
		CMinigameManager* pManager = CMinigameManager::Get_Instance();
		pManager->End_MiniGame();
	}
}

void CMiniGame_IronFist::Set_End()
{
	m_bEnd = true;
}

void CMiniGame_IronFist::Set_Cam()
{
	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	CCamera_IronFist* pCamera = pCameraMgr->Get_IronFistCamera();
	if (nullptr == pCamera)
		return;

	pCamera->Set_LookAt();
	pCameraMgr->Set_IronFistCam_On();
}

CGameObject * CMiniGame_IronFist::Find_Player()
{
	auto& iter = find_if(m_Layers.begin(), m_Layers.end(), CTagFinder(MINIGAME_PLAYER));
	if (iter == m_Layers.end())
		return nullptr;

	return iter->second;
}

void CMiniGame_IronFist::Initialize()
{
	/*m_bStart = false;

	m_bEnd = false;*/
}

HRESULT CMiniGame_IronFist::Create_Layers()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CIron_Android9S* pGameObject = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_IRON9S, MINIGAME_PLAYER, (CGameObject**)&pGameObject)))
		return E_FAIL;

	if (nullptr == pGameObject)
		return E_FAIL;

	Add_GameObject(MINIGAME_PLAYER, pGameObject);

	CMiniGameBuilding* pGameObjectBuilding = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_IRONFIST_BACKGROUND, MINIGAME_IRONFIST_BACKGROUND, (CGameObject**)&pGameObjectBuilding)))
		return E_FAIL;

	if (nullptr == pGameObjectBuilding)
		return E_FAIL;

	Add_GameObject(MINIGAME_IRONFIST_BACKGROUND, pGameObjectBuilding);

	CIron_Humanoid_Small* pMonster = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_IRONHUMANOID_SMALL, MINIGAME_MONSTER, (CGameObject**)&pMonster)))
		return E_FAIL;

	Add_GameObject(MINIGAME_MONSTER, pMonster);

	CIronGauntlet* pGauntlet = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, PROTO_KEY_IRONGAUNTLET, MINIGAME_WEAPON, (CGameObject**)&pGauntlet, (void*)pGameObject)))
		return E_FAIL;

	if (nullptr == pGauntlet)
		return E_FAIL;

	Add_GameObject(MINIGAME_WEAPON, pGauntlet);

	CSky*	pSky = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, L"Prototype_GameObject_Sky", L"Sky", (CGameObject**)&pSky)))
		return E_FAIL;

	Add_GameObject(L"Sky", pSky);

	return S_OK;
}

HRESULT CMiniGame_IronFist::NativeConstruct()
{
	if (FAILED(Create_Layers()))
		return E_FAIL;

	return S_OK;
}

CMiniGame_IronFist * CMiniGame_IronFist::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CMiniGame_IronFist* pGameInstance = new CMiniGame_IronFist(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct()))
	{
		MSGBOX("Failed to Create CMiniGame_IronFist");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CMiniGame_IronFist::Free()
{
	__super::Free();
}
