#pragma once

#ifndef __CLIENT_LEVEL_ROBOTGENERAL_H__
#define __CLIENT_LEVEL_ROBOTGENERAL_H__

#include "Level.h"
#include "Client_Defines.h"

BEGIN(Client)

class CLevel_EngelsStage final : public CLevel
{
public:
	CLevel_EngelsStage(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_EngelsStage() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct(_uint iLevelID) override;
	virtual	_int						Tick(_double dTimeDelta) override;
	virtual HRESULT						Render() override;

private:
	HRESULT								Add_CollisionTag();
	HRESULT								Set_Light(CGameInstance* pGameInstance);
	HRESULT								Set_Player(CGameInstance* pGameInstance);
	HRESULT								Load_EventCollider(CGameInstance* pGameInstance, const _tchar* pEventColliderDataFilePath);

public:
	static	CLevel_EngelsStage*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void						Free() override;

};

END

#endif