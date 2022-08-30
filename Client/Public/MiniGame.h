#pragma once
#ifndef __CLIENT_MINI_GAME_H__
#define __CLIENT_MINI_GAME_H__

#include "Base.h"
#include "GameObject.h"

class CMiniGame abstract : public CBase
{
protected:
	CMiniGame(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CMiniGame() DEFAULT;

protected:
	void					Add_GameObject(const _tchar* pTag, CGameObject* pGameObject);

public:
	void					Set_Tag(const _tchar* pTag) { m_pTag = pTag; }
	const _tchar*			Get_Tag() { return m_pTag; }

public:						// CGameInstance ���� �ѹ��� Pause��Ű�� ������ �ش��ϴ� �̴ϰ����� �ƴϸ� �� �� ���⼭ �ɷ��ش�.
	void					Set_MiniGamePauseForLayer(_bool bCheck);
	virtual void			Set_End();

public:
	const _bool				Get_Start() { return m_bStart; }

public:
	virtual CGameObject*	Find_Player();
	virtual CGameObject*	Find_Object(const _tchar* pTag);
	virtual void			Set_Cam();
	virtual void			Set_PreCam();

public:
	virtual _int			Tick(_double dTimeDelta);

public:
	virtual void			Check_Start(_double dTimeDelta);
	virtual void			Check_End(_double dTimeDelta);

public:
	virtual void			Initialize();

protected:
	virtual HRESULT			NativeConstruct();

protected:
	_pGraphicDevice			m_pGraphicDevice = nullptr;
	_pContextDevice			m_pContextDevice = nullptr;

protected:
	unordered_map<const _tchar*, CGameObject*>		m_Layers;

protected:
	_bool					m_bStart = false;
	_bool					m_bEnd = false;

	const _tchar*			m_pTag = nullptr;
	const _tchar*			m_pPreCamTag = nullptr;

public:
	virtual void			Free() override;
};


#endif // !__CLIENT_MINI_GAME_H__