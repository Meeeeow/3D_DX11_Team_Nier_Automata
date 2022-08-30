#pragma once
#ifndef __CLIENT_MINI_GAME_MANAGER_H__
#define __CLIENT_MINI_GAME_MANAGER_H__

#include "Client_Defines.h"
#include "Base.h"
#include "MiniGame.h"

BEGIN(Client)
class CMinigameManager final : public CBase
{
	DECLARE_SINGLETON(CMinigameManager)
private:
	CMinigameManager();
	virtual ~CMinigameManager() DEFAULT;

public:
	void			Setting(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);

public:
	void			Add_MiniGame(const _tchar* pTag, CMiniGame* pMiniGame);
	void			Set_MiniGame(const _tchar* pTag);

public:
	void			Set_End();

public:
	CGameObject*	Find_Player(const _tchar* pMiniGameTag);
	CGameObject*	Find_Object(const _tchar* pMiniGameTag, const _tchar* pObjectTag);

public:
	void			Start_MiniGame();
	void			End_MiniGame();

public:
	CMiniGame*		Get_CurrentMiniGame();

public:
	_int			Tick(_double dTimeDelta);

private:
	unordered_map<const _tchar*, CMiniGame*>	m_MiniGames;
	CMiniGame*									m_pCurMiniGame = nullptr;

public:
	virtual	void			Free() override;

};

END
#endif // !__CLIENT_MINI_GAME_MANAGER_H__