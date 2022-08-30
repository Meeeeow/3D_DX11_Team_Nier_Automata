#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Level_Stage1.h"
#include "Level_AmusementPark.h"
#include "Level_OperaBackStage.h"
#include "ObjectPool.h"
#include "Level_ZhuangziStage.h"
#include "Level_EngelsStage.h"

#include "LoadingScreen.h"
#include "LoadingPercent.h"
#include "LoadingEmil.h"
#include "SoundMgr.h"

CLevel_Loading::CLevel_Loading(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{

}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel, _uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	m_eNextLevel = eNextLevel;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pLoadingScreen = dynamic_cast<CLoadingScreen*>(pGameInstance->Clone_GameObject(L"LoadingScreen"));
	m_pLoadingPercent = dynamic_cast<CLoadingPercent*>(pGameInstance->Clone_GameObject(L"LoadingPercent"));
	m_pLoadingEmil = dynamic_cast<CLoadingEmil*>(pGameInstance->Clone_GameObject(L"LoadingEmil"));

	pGameInstance->Set_NextLevelID((_uint)m_eNextLevel);

	m_pLoader = CLoader::Create(m_pGraphicDevice, m_pContextDevice, m_eNextLevel);
	if (m_pLoader == nullptr)
		return E_FAIL;


	CSoundMgr::Get_Instance()->StopAll();
	CSoundMgr::Get_Instance()->PlayLoopSound(L"Loading_BGM.mp3",CSoundMgr::CHANNELID::BGM);

	return S_OK;
}

_int CLevel_Loading::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pLoadingScreen->Tick(dTimeDelta);
	m_pLoadingPercent->Tick(dTimeDelta);
	m_pLoadingEmil->Tick(dTimeDelta);

	m_pLoadingScreen->LateTick(dTimeDelta);
	m_pLoadingPercent->LateTick(dTimeDelta);
	m_pLoadingEmil->LateTick(dTimeDelta);

	if (m_pLoader->Get_Finished() == true && pGameInstance->Key_Down(DIK_RETURN))
	{
		CLevel* pLevel = nullptr;
		pGameInstance->Clear_CollisionTag();
		pGameInstance->Clear_Light();

		switch (m_eNextLevel)
		{
		case Client::LEVEL::STAGE1:
  			pLevel = CLevel_Stage1::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::STAGE1);
			break;
		case Client::LEVEL::ROBOTMILITARY:
			break;
		case Client::LEVEL::AMUSEMENTPARK:
			pLevel = CLevel_AmusementPark::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::AMUSEMENTPARK);
			break;
		case Client::LEVEL::OPERABACKSTAGE:
			pLevel = CLevel_OperaBackStage::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::OPERABACKSTAGE);
			break;
		case Client::LEVEL::ZHUANGZISTAGE:
			pLevel = CLevel_ZhuangziStage::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::ZHUANGZISTAGE);
			break;
		case Client::LEVEL::ROBOTGENERAL:
			pLevel = CLevel_EngelsStage::Create(m_pGraphicDevice, m_pContextDevice, (_uint)LEVEL::ROBOTGENERAL);
			break;
		default:
			break;
		}
		if (pLevel == nullptr)
			return -1;

		if (FAILED(pGameInstance->Open_Level(pLevel)))
 			return -1;
	}

	return 0;
}

_int CLevel_Loading::LateTick(_double dTimeDelta)
{


	return _int();
}

HRESULT CLevel_Loading::Render()
{
	return S_OK;
}

void CLevel_Loading::Increase_Percent()
{
	if (m_pLoadingPercent == nullptr)
		return;

	m_pLoadingPercent->Increase_Percent();
}

CLevel_Loading * CLevel_Loading::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, LEVEL eNextLevel, _uint iLevelIndex)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(eNextLevel, iLevelIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Loading::Free()
{
	Safe_Release(m_pLoadingEmil);
	Safe_Release(m_pLoadingScreen);
	Safe_Release(m_pLoadingPercent);
	__super::Free();
	Safe_Release(m_pLoader);
}
