#pragma once

#ifndef __CLIENT_ZHUANGZIBRIDGE_H__
#define __CLIENT_ZHUANGZIBRIDGE_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CZhuangziBridge final : public CGameObjectModel
{
private:
	CZhuangziBridge(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CZhuangziBridge(const CZhuangziBridge& rhs);
	virtual ~CZhuangziBridge() = default;

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
	void								Set_CellOn(_bool bCheck) { m_bCellOn = bCheck; }
	void								Set_SceneClear(_bool bCheck) { m_bSceneClear = bCheck; }
	void								Initialize_Animation();

public:
	virtual _bool						Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void						Collision(CGameObject* pGameObject) override;

private:
	_bool								m_bClose = false;
	_bool								m_bCellOn = false;
	_bool								m_bExit = false;
	_bool								m_bSceneClear = false;
	_bool								m_bPlayOnce = false;
private:
	HRESULT								SetUp_Components();

public:
	static	CZhuangziBridge*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END

#endif
