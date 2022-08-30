#include "stdafx.h"
#include "..\Public\MiniGame.h"
#include "CameraMgr.h"

CMiniGame::CMiniGame(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

void CMiniGame::Add_GameObject(const _tchar * pTag, CGameObject * pGameObject)
{
	Safe_AddRef(pGameObject);

	m_Layers.emplace(pTag, pGameObject);
}

void CMiniGame::Set_MiniGamePauseForLayer(_bool bCheck)
{
	for (auto& Pair : m_Layers)
		Pair.second->Set_MiniGamePause(bCheck);
}

void CMiniGame::Set_End()
{
}

CGameObject * CMiniGame::Find_Player()
{
	auto& iter = find_if(m_Layers.begin(), m_Layers.end(), CTagFinder(MINIGAME_PLAYER));
	if (iter == m_Layers.end())
		return nullptr;

	return iter->second;
}

CGameObject * CMiniGame::Find_Object(const _tchar * pTag)
{
	auto& iter = find_if(m_Layers.begin(), m_Layers.end(), CTagFinder(pTag));
	if (iter == m_Layers.end())
		return nullptr;

	return iter->second;
}

void CMiniGame::Set_Cam()
{
}

void CMiniGame::Set_PreCam()
{
	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	pCameraMgr->Set_MainCam_On();
}

_int CMiniGame::Tick(_double dTimeDelta)
{
	return _int();
}

void CMiniGame::Check_Start(_double dTimeDelta)
{
}

void CMiniGame::Check_End(_double dTimeDelta)
{
}

void CMiniGame::Initialize()
{
}

HRESULT CMiniGame::NativeConstruct()
{
	return S_OK;
}

void CMiniGame::Free()
{
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);

	for (auto& Pair : m_Layers)
		Safe_Release(Pair.second);
}
