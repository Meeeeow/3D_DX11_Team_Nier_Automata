#pragma once
#ifndef __CLIENT_EVENT_COLLIDER_H__
#define __CLIENT_EVENT_COLLIDER_H__

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

class CEvent_Collider : public CGameObjectModel
{
public:
	enum class EVENT : _uint
	{
		WHEEL_START = 1,
		WHEEL_END = 2,
		COASTER_STAIR = 3,
		COASTER = 4,
		ENTRANCE = 5,
		ZHUANGZI = 6,
		ZHUANGZI_ENTRANCE = 7,
		BEAUVOIR = 8,
		GATE = 9,
		TANK = 10,
		ENGELS = 11,
		COASTER1 = 12,
		COASTER2 = 13,
		COASTER3 = 14,
		NONE = 999
	};
private:
	explicit CEvent_Collider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CEvent_Collider(const CEvent_Collider& rhs);
	virtual ~CEvent_Collider() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_Name(_tchar* szName) { lstrcpy(m_szName,szName); }
	_tchar*							Get_Name() { return m_szName; }
	void							Check_Event();
	void							Set_Event_Number();


private:
	HRESULT							Event_Wheel_Start();
	HRESULT							Event_Wheel_End();
	HRESULT							Event_Coaster_Stair();
	HRESULT							Event_Coaster();
	HRESULT							Event_Entrance();
	HRESULT							Event_Zhuangzi();
	HRESULT							Event_Zhunagzi_Entrance();
	HRESULT							Event_Beauvoir();
	HRESULT							Event_Tank();
	HRESULT							Event_Gate();
	HRESULT							Event_Engels();
	HRESULT							Event_Coaster1();
	HRESULT							Event_Coaster2();
	HRESULT							Event_Coaster3();

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;


public:
	static	CEvent_Collider*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void* pArg = nullptr);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free();

private:
	_tchar			m_szName[MAX_PATH];
	EVENT			m_eEvent = EVENT::NONE;
};

END

#endif
