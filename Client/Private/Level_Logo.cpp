#include "stdafx.h"
#include "..\Public\Level_Logo.h"
#include "Level_Loading.h"
#include "SoundMgr.h"

#include "UI_HUD_Hpbar_Player_Frame.h"
#include "UI_HUD_Hpbar_Player_Contents.h"
#include "UI_HUD_Hpbar_Monster_Frame.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_PodCoolTime.h"
#include "UI_HUD_Damage.h"
#include "UI_HUD_Pointer.h"
#include "UI_HUD_Target.h"
#include "UI_Core_Background.h"
#include "UI_Core_Cursor.h"
#include "UI_Dialogue_EngageMessage.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_Dialogue_HighlightMessage.h"
#include "UI_Dialogue_HackingMessage.h"
#include "UI_Dialogue_HackingSuccessMessage.h"
#include "UI_Core_CinematicScreen.h"
#include "UI_Core_HeadButton.h"
#include "UI_Core_DecoLine.h"
#include "UI_Core_Controller.h"
#include "UI_Core_Status.h"
#include "UI_Core_Title.h"
#include "UI_Core_ScrollList.h"
#include "UI_Core_ListLine.h"
#include "UI_Core_SelectList.h"
#include "UI_Core_SelectLine.h"
#include "UI_Core_Detail.h"
#include "UI_Core_WeaponSlot.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "UI_HUD_MiniGameScreen.h"
#include "UI_HUD_Controller.h"
#include "UI_HUD_Hpbar_MiniGame.h"
#include "UI_HUD_QuickSlot.h"

CLevel_Logo::CLevel_Logo(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CLevel(pGraphicDevice, pContextDevice)
{

}

HRESULT CLevel_Logo::NativeConstruct(_uint iLevelID)
{
	if (FAILED(__super::NativeConstruct(iLevelID)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::LOGO, L"Logo", L"LayerLogo")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::LOGO, L"LogoTitle", L"LayerLogo")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::LOGO, L"LogoGear", L"LayerLogo")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::LOGO, L"LogoAnyKey", L"LayerLogo")))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::LOGO, L"LogoCopyRight", L"LayerLogo")))
		return E_FAIL;

	if (FAILED(SetUp_UserInterface(pGameInstance)))
		return E_FAIL;

	if (FAILED(Load_UserInterface(pGameInstance)))
		return E_FAIL;

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	
	
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::BGM2, 0.1f);
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::ENVIRONMENT_SOUND, 0.1f);
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::P_WEAPONSOUND, 1.f);
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::POD, 0.75f);
	pSoundMgr->Set_Volume(CSoundMgr::CHANNELID::HACKING_FINISHED, 1.f);

	return S_OK;
}

_int CLevel_Logo::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();


	if (pGameInstance->Key_Down(DIK_A))
	{
		CSoundMgr::Get_Instance()->PlayLoopSound(L"Logo_BGM.mp3", CSoundMgr::CHANNELID::BGM);
		CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.1f);
	}
	if (pGameInstance->Key_Down(DIK_SPACE))
	{
		CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::STAGE1, (_uint)LEVEL::LOADING);
		if (pLevel == nullptr)
			return -1;

		if (FAILED(pGameInstance->Open_Level(pLevel)))
			return -1;
	}
	return 0;
}

HRESULT CLevel_Logo::Render()
{
	return S_OK;
}

HRESULT CLevel_Logo::SetUp_UserInterface(CGameInstance * pGameInstance)
{
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Hpbar_Player_Frame"), CUI_HUD_Hpbar_Player_Frame::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Hpbar_Player_Contents"), CUI_HUD_Hpbar_Player_Contents::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Hpbar_Monster_Contents"), CUI_HUD_Hpbar_Monster_Contents::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_PodCoolTime"), CUI_HUD_PodCoolTime::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Damage"), CUI_HUD_Damage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Target"), CUI_HUD_Target::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Background"), CUI_Core_Background::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Cursor"), CUI_Core_Cursor::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Dialogue_SystemMessage"), CUI_Dialogue_SystemMessage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Dialogue_EngageMessage"), CUI_Dialogue_EngageMessage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Dialogue_HighlightMessage"), CUI_Dialogue_HighlightMessage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Dialogue_HackingMessage"), CUI_Dialogue_HackingMessage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Dialogue_HackingSuccessMessage"), CUI_Dialogue_HackingSuccessMessage::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Pointer"), CUI_HUD_Pointer::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Cinematic"), CUI_Core_CinematicScreen::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_HeadButton"), CUI_Core_HeadButton::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_DecoLine"), CUI_Core_DecoLine::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Controller"), CUI_Core_Controller::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Status"), CUI_Core_Status::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Detail"), CUI_Core_Detail::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_WeaponSlot"), CUI_Core_WeaponSlot::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_Title"), CUI_Core_Title::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_ScrollList"), CUI_Core_ScrollList::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_ListLine"), CUI_Core_ListLine::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_SelectList"), CUI_Core_SelectList::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_Core_SelectLine"), CUI_Core_SelectLine::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_DirectionIndicator"), CUI_HUD_DirectionIndicator::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_StageObjective"), CUI_HUD_StageObjective::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_MiniGameScreen"), CUI_HUD_MiniGameScreen::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Hpbar_MiniGame"), CUI_HUD_Hpbar_MiniGame::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_QuickSlot"), CUI_HUD_QuickSlot::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI_HUD_Controller"), CUI_HUD_Controller::Create(m_pGraphicDevice, m_pContextDevice))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Load_UserInterface(CGameInstance * pGameInstance, const wchar_t* pLayerTag)
{
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Hpbar_Player_Frame"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Hpbar_Player_Contents"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Hpbar_Monster_Contents"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Target"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Damage"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_PodCoolTime"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Pointer"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_DirectionIndicator"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_StageObjective"), pLayerTag)))
		return E_FAIL;	

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_MiniGameScreen"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_QuickSlot"), pLayerTag)))
		return E_FAIL;	

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Hpbar_MiniGame"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_HUD_Controller"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Dialogue_SystemMessage"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Dialogue_EngageMessage"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Dialogue_HighlightMessage"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Dialogue_HackingMessage"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Dialogue_HackingSuccessMessage"), pLayerTag)))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Background"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Cursor"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Cinematic"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_DecoLine"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Status"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Detail"), pLayerTag)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_WeaponSlot"), pLayerTag)))
		return E_FAIL;	

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Title"), pLayerTag)))
		return E_FAIL;
	
	_uint iTypeCount = (_uint)CUI_Core_HeadButton::BUTTONTYPE::COUNT;

	for (_uint idx = 0; idx < iTypeCount; ++idx)
	{
		CUI_Core_HeadButton::BUTTONTYPE	eButtonType;

		eButtonType = (CUI_Core_HeadButton::BUTTONTYPE)idx;

		if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_HeadButton"), pLayerTag, &eButtonType)))
			return E_FAIL;
	}

	for (_uint idx = 0; idx < CUI_Core_ScrollList::LINEMAXCOUNT; ++idx)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_ListLine"), pLayerTag, &idx)))
			return E_FAIL;
	}

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_ScrollList"), pLayerTag)))
		return E_FAIL;

	for (_uint idx = 0; idx < CUI_Core_SelectList::LINEMAXCOUNT; ++idx)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_SelectLine"), pLayerTag, &idx)))
			return E_FAIL;
	}	

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_SelectList"), pLayerTag)))
		return E_FAIL;	

	if (FAILED(pGameInstance->Add_GameObjectToLayer((_uint)LEVEL::STATIC, TEXT("Prototype_GameObject_UI_Core_Controller"), pLayerTag)))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo * CLevel_Logo::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iLevelID)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iLevelID)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
