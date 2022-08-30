#pragma once
#ifndef __CLIENT_LEVEL_LOGO_H__
#define __CLIENT_LEVEL_LOGO_H__

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CLevel_Logo final : public CLevel
{
private:
	CLevel_Logo(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_Logo() DEFAULT;

public:
	virtual HRESULT					NativeConstruct(_uint iLevelID) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	HRESULT							SetUp_UserInterface(CGameInstance* pGameInstance);
	HRESULT							Load_UserInterface(CGameInstance* pGameInstance, const wchar_t* pLayerTag = L"Layer_UI");

private:

public:
	static	CLevel_Logo*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual void					Free() override;

};
END
#endif // !__CLIENT_LEVEL_LOGO_H__