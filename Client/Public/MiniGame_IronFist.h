#pragma once

#ifndef __CLIENT_MINIGAME_IRONFIST_H__
#define __CLIENT_MINIGAME_IRONFIST_H__

#include "Client_Defines.h"
#include "MiniGame.h"
#include "GameObject.h"

BEGIN(Client)

class CMiniGame_IronFist final : public CMiniGame
{
private:
	CMiniGame_IronFist(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CMiniGame_IronFist() DEFAULT;

public:
	_int							Tick(_double dTimeDelta) override;

public:
	virtual void					Check_Start(_double dTimeDelta) override;
	virtual void					Check_End(_double dTimeDelta) override;
	virtual void					Set_End() override;

public:
	void							Set_StartMessageOn(_bool bCheck) { m_bStartMessageOn = bCheck; }

public:
	virtual void					Set_Cam() override;

public:
	virtual CGameObject*			Find_Player();

public:
	virtual void					Initialize();

private:
	HRESULT							Create_Layers();

private:
	virtual HRESULT					NativeConstruct() override;

private:
	_double							m_HackingStartTime = 0.0;
	_double							m_HackingEndTime = 0.0;

private:
	_bool							m_bStartMessageOn = false;
	_bool							m_bEndMessageOn = false;

	_double			m_ScreenTimeAcc = 0.0;
	const _double	m_ScreenTimeAccMax = 2.0;

public:
	static CMiniGame_IronFist*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual void					Free() override;
};

END

#endif