#pragma once

#ifndef __CLIENT_ZHUANGZIDOOR_H__
#define __CLIENT_ZHUANGZIDOOR_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CZhuangziDoor final : public CGameObjectModel
{
private:
	CZhuangziDoor(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CZhuangziDoor(const CZhuangziDoor& rhs);
	virtual ~CZhuangziDoor() = default;

public:
	virtual	HRESULT						NativeConstruct_Prototype() override;
	virtual HRESULT						NativeConstruct(void* pArg) override;
	virtual _int						Tick(_double dTimeDelta);
	virtual _int						LateTick(_double dTimeDelta);
	virtual HRESULT						Render();

public:
	void								Check_StopTime();
	HRESULT								Check_PlayerCell();

public:
	void								Set_Exit(_bool bCheck) { m_bExit = bCheck; }

private:
	void								Check_Key();

public:
	virtual _bool						Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void						Collision(CGameObject* pGameObject) override;

private:
	_bool								m_bCellOn = false;
	_bool								m_bExit = false;
	_bool								m_bStop = false;

private:
	HRESULT								SetUp_Components();

public:
	static	CZhuangziDoor*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END

#endif
