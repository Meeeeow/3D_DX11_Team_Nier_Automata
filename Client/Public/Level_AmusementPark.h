#pragma once
#ifndef __CLIENT_LEVEL_AMUSEMENT_PARK_H__
#define __CLIENT_LEVEL_AMUSEMENT_PARK_H__
 
#include "Level.h"
#include "Client_Defines.h"
BEGIN(Client)
class CLevel_AmusementPark final : public CLevel
{
private:
	CLevel_AmusementPark(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_AmusementPark() DEFAULT;

public:
	virtual	HRESULT							NativeConstruct(_uint iLevelID) override;
	virtual _int							Tick(_double dTimeDelta) override;
	virtual HRESULT							Render() override;

private:
	HRESULT									Add_CollisionTag(CGameInstance* pGameInstance);
	HRESULT									Set_Light(CGameInstance* pGameInstance);
	HRESULT									Set_Player(CGameInstance* pGameInstance);
	HRESULT									Set_Sky(CGameInstance* pGameInstance);
	HRESULT									Load_EventCollider(CGameInstance* pGameInstance, const _tchar* pEventColliderDataFilePath);
	HRESULT									Add_Effects(CGameInstance* pGameInstance);
	HRESULT									Make_Flyers(CGameInstance* pGameInstance);

private:
	_double		m_dTimeAccUIActivate = 0.0;
	_bool		m_bOnceUIActivate = false;

public:
	static	CLevel_AmusementPark*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void							Free() override;
};
END
#endif // !__CLIENT_LEVEL_AMUSEMENT_PARK_H__