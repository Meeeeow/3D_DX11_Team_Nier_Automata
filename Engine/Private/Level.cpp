#include "..\Public\Level.h"

CLevel::CLevel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CLevel::NativeConstruct(_uint iLevelID)
{
	m_iCurrentLevelID = iLevelID;
	
	return S_OK;
}

_int CLevel::Tick(_double dTimeDelta)
{

	return _int();
}

_int CLevel::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CLevel::Render()
{
	return S_OK;
}

CLevel * CLevel::Get_NextLevel()
{
	return m_pNextLevel;
}

void CLevel::Set_NextLevel(CLevel * pNextLevel)
{
	m_pNextLevel = pNextLevel;
}

void CLevel::Free()
{
	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);
}
