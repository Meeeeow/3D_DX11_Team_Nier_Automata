#pragma once
#ifndef __CLIENT_AMUSEMENT_PARK_H__
#define __CLIENT_AMUSEMENT_PARK_H__

#include "Level.h"
#include "Client_Defines.h"

class CAmusementPark final : public CLevel
{
private:
	CAmusementPark(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CAmusementPark() DEFAULT; 

public:
	virtual	HRESULT					NativeConstruct(_uint iLevelID) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;


public:
	static	CAmusementPark*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void					Free() override;
};

#endif // !__CLIENT_AMUSEMENT_PARK_H__