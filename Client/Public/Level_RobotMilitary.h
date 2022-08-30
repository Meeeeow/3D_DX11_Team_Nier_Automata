#pragma once
#ifndef __CLIENT_LEVEL_ROBOT_MILITARY_H__
#define __CLIENT_LEVEL_ROBOT_MILITARY_H__

#include "Client_Defines.h"
#include "Level.h"

class CLevel_RobotMilitary final : public CLevel
{
private:
	CLevel_RobotMilitary(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CLevel_RobotMilitary() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct(_uint iLevelID) override;
	virtual _int						Tick(_double dTimeDelta) override;
	virtual HRESULT						Render() override;
	
public:
	static	CLevel_RobotMilitary*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID);
	virtual	void						Free() override;

};


#endif // !__CLIENT_LEVEL_ROBOT_MILITARY_H__