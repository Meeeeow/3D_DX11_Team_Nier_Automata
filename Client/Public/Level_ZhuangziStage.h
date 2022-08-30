#pragma once

#ifndef __CLIENT_LEVEL_ZHUANGZISTAGE_H__
#define __CLIENT_LEVEL_ZHUANGZISTAGE_H__

#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_ZhuangziStage final : public CLevel
{
private:
	CLevel_ZhuangziStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_ZhuangziStage() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct(_uint iLevelID) override;
	virtual	_int						Tick(_double dTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	HRESULT								Add_CollisionTag();
	HRESULT								Set_Light(CGameInstance* pGameInstance);
	HRESULT								Set_Player(CGameInstance* pGameInstance);
	HRESULT								Load_EventCollider(CGameInstance* pGameInstance, const _tchar* pEventColliderDataFilePath);

private:
	void								Trace_SpotLight(CGameInstance* pGameInstance);
	void								TurnOn_DirectionLight(CGameInstance* pGameInstance, _double dTimeDelta);

public:
	void								Electricity_Supply();

private:
	_bool								m_bSupply = false;

public:
	static	CLevel_ZhuangziStage*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void						Free() override;
};

END

#endif