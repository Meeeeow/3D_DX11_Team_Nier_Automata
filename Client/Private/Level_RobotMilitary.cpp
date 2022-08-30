#include "stdafx.h"
#include "..\Public\Level_RobotMilitary.h"
#include "UI_HUD_Controller.h"

CLevel_RobotMilitary::CLevel_RobotMilitary(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{
}

HRESULT CLevel_RobotMilitary::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	return S_OK;
}

_int CLevel_RobotMilitary::Tick(_double dTimeDelta)
{

	return _int();
}

HRESULT CLevel_RobotMilitary::Render()
{
	return S_OK;
}

CLevel_RobotMilitary * CLevel_RobotMilitary::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_RobotMilitary* pInstance = new CLevel_RobotMilitary(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);
	return pInstance;
}

void CLevel_RobotMilitary::Free()
{
	__super::Free();
}
