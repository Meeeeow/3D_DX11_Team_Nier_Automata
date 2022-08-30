#include "stdafx.h"
#include "..\Public\MinigameManager.h"
#include "MiniGame_IronFist.h"
#include "CameraMgr.h"
#include "Beauvoir.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CMinigameManager)

CMinigameManager::CMinigameManager()
{

}

void CMinigameManager::Setting(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CMiniGame_IronFist* pMiniGame = CMiniGame_IronFist::Create(pGraphicDevice, pContextDevice);
	if (nullptr == pMiniGame)
		return;

	pMiniGame->Set_Tag(MINIGAME_IRONFIST);

	m_MiniGames.emplace(MINIGAME_IRONFIST, pMiniGame);
}

void CMinigameManager::Add_MiniGame(const _tchar* pTag, CMiniGame * pMiniGame)
{
	if (nullptr == pMiniGame)
		return;

	m_MiniGames.emplace(pTag, pMiniGame);
}

void CMinigameManager::Set_MiniGame(const _tchar * pTag)
{
	auto& iter = find_if(m_MiniGames.begin(), m_MiniGames.end(), CTagFinder(pTag));
	if (iter == m_MiniGames.end())
		return;

	m_pCurMiniGame = iter->second;

	Safe_AddRef(m_pCurMiniGame);
}

void CMinigameManager::Set_End()
{
	if (nullptr == m_pCurMiniGame)
		return;

	m_pCurMiniGame->Set_End();
}

CGameObject * CMinigameManager::Find_Player(const _tchar * pMiniGameTag)
{
	auto& iter = find_if(m_MiniGames.begin(), m_MiniGames.end(), CTagFinder(pMiniGameTag));

	if (iter == m_MiniGames.end())
		return nullptr;

	return 	iter->second->Find_Player();
}

CGameObject * CMinigameManager::Find_Object(const _tchar * pMiniGameTag, const _tchar * pObjectTag)
{
	auto& iter = find_if(m_MiniGames.begin(), m_MiniGames.end(), CTagFinder(pMiniGameTag));

	if (iter == m_MiniGames.end())
		return nullptr;

	return iter->second->Find_Object(pObjectTag);
}

void CMinigameManager::Start_MiniGame()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Set_MiniGamePause(true);

	for (auto& Pair : m_MiniGames)
	{
		if (Pair.second == m_pCurMiniGame)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/IF9s2.dat", 6);
			continue;
		}

		Pair.second->Set_MiniGamePauseForLayer(false);
	}
}

void CMinigameManager::End_MiniGame()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Set_MiniGamePause(false);
	m_pCurMiniGame->Set_PreCam();

	if (nullptr == m_pCurMiniGame)
		return;

	m_pCurMiniGame->Initialize();

	Safe_Release(m_pCurMiniGame);

	m_pCurMiniGame = nullptr;

	CBeauvoir* pBeauvoir = dynamic_cast<CBeauvoir*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Boss"));
	pBeauvoir->Set_HackingFinished();

}

CMiniGame * CMinigameManager::Get_CurrentMiniGame()
{
	return m_pCurMiniGame;
}

_int CMinigameManager::Tick(_double dTimeDelta)
{
	if (nullptr == m_pCurMiniGame)
		return 0;

	return m_pCurMiniGame->Tick(dTimeDelta);
}

void CMinigameManager::Free()
{
	for (auto& Pair : m_MiniGames)
	{
		Safe_Release(Pair.second);
	}
	m_MiniGames.clear();

	Safe_Release(m_pCurMiniGame);
}
