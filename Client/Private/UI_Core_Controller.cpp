#include "stdafx.h"
#include "..\Public\UI_Core_Controller.h"
#include "UI_Core_Title.h"
#include "UI_Core_ScrollList.h"
#include "UI_Core_SelectList.h"
#include "UI_Core_ListLine.h"
#include "Player.h"
#include "SoundMgr.h"

CUI_Core_Controller::CUI_Core_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Controller::CUI_Core_Controller(const CUI_Core_Controller & rhs)
	: CUI(rhs)
{
}

CUI_Core_Controller * CUI_Core_Controller::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Controller* pInstance = new CUI_Core_Controller(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Controller::Clone(void * pArg)
{
	CUI_Core_Controller* pInstance = new CUI_Core_Controller(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Controller::Free()
{
	__super::Free();
}

HRESULT CUI_Core_Controller::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CUI_Core_Controller::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Controller::SetUp_Components()))
	{
		return E_FAIL;
	}

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_CONTROLLER"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_Core_Controller::SetUp_Components()
{
	return S_OK;
}

_int CUI_Core_Controller::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Controller::LateTick(_double TimeDelta)
{

	return 0;
}

HRESULT CUI_Core_Controller::Render()
{
	return S_OK;
}

HRESULT CUI_Core_Controller::Activate(_double dTimeDelta)
{
	// If OPEN is already started, must be NEVER disturbed by CLOSE
	if (m_iCommandFlag & COMMAND_CORE_OPEN && !(m_iCommandFlag & COMMAND_CORE_CLOSING))
	{
		m_iCommandFlag |= COMMAND_CORE_OPENING;

		if (m_iCommandFlag & COMMAND_CORE_CLOSE)
		{
			m_iCommandFlag ^= COMMAND_CORE_CLOSE;
		}
		if (SUCCESS == Command_Open_UI(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_OPEN;
			m_iCommandFlag ^= COMMAND_CORE_OPENING;
		}
	}

	// If CLOSE is already started, must be NEVER disturbed by OPEN
	if (m_iCommandFlag & COMMAND_CORE_CLOSE && !(m_iCommandFlag & COMMAND_CORE_OPENING))
	{
		m_iCommandFlag |= COMMAND_CORE_CLOSING;

		if (m_iCommandFlag & COMMAND_CORE_OPEN)
		{
			m_iCommandFlag ^= COMMAND_CORE_OPEN;
		}
		if (SUCCESS == Command_Close_UI(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_CLOSE;
			m_iCommandFlag ^= COMMAND_CORE_CLOSING;
		}
	}

	// If UNLOCK and LOCK command at the same time, UNLOCK will be ignored
	if (m_iCommandFlag & COMMAND_CORE_BUTTON_UNLOCK)
	{
		if (!(m_iCommandFlag & COMMAND_CORE_BUTTON_LOCK))
		{
			if (SUCCESS == Command_HeadButton_Unlock(dTimeDelta))
			{
				m_iCommandFlag ^= COMMAND_CORE_BUTTON_UNLOCK;
			}
		}
		else
		{
			if (SUCCESS == Command_HeadButton_Lock(dTimeDelta))
			{
				m_iCommandFlag ^= COMMAND_CORE_BUTTON_LOCK;
				m_iCommandFlag ^= COMMAND_CORE_BUTTON_UNLOCK;
			}
		}
	}
	if (m_iCommandFlag & COMMAND_CORE_BUTTON_LOCK)
	{
		if (SUCCESS == Command_HeadButton_Lock(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_BUTTON_LOCK;
		}
	}

	// If UNLOCK and LOCK command at the same time, UNLOCK will be ignored
	if (m_iCommandFlag & COMMAND_CORE_LINE_UNLOCK)
	{
		if (!(m_iCommandFlag & COMMAND_CORE_LINE_LOCK))
		{
			if (SUCCESS == Command_ListLine_Unlock(dTimeDelta))
			{
				m_iCommandFlag ^= COMMAND_CORE_LINE_UNLOCK;
			}
		}
		else
		{
			if (SUCCESS == Command_ListLine_Lock(dTimeDelta))
			{
				m_iCommandFlag ^= COMMAND_CORE_LINE_LOCK;
				m_iCommandFlag ^= COMMAND_CORE_LINE_UNLOCK;
			}
		}
	}
	if (m_iCommandFlag & COMMAND_CORE_LINE_LOCK)
	{
		if (SUCCESS == Command_ListLine_Lock(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_LINE_LOCK;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_SCROLL_UP)
	{
		if (SUCCESS == Command_Scroll_Up(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_SCROLL_UP;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_SCROLL_DOWN)
	{
		if (SUCCESS == Command_Scroll_Down(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_SCROLL_DOWN;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_ITEM_BODY_OPEN)
	{
		if (SUCCESS == Command_Item_Body_Open(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_ITEM_BODY_OPEN;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_WEAPON_BODY_OPEN)
	{
		if (SUCCESS == Command_Weapon_Body_Open(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_WEAPON_BODY_OPEN;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_CHIP_BODY_OPEN)
	{
		if (SUCCESS == Command_Chip_Body_Open(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_CHIP_BODY_OPEN;
		}
	}

	if (m_iCommandFlag & COMMAND_CORE_ALL_BODY_CLOSE)
	{
		if (SUCCESS == Command_All_Body_Close(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_CORE_ALL_BODY_CLOSE;
		}
	}	

	// When flag processing is completed properly, will be entered in this branch
	if (m_iCommandFlag == COMMAND_CORE_INITIALIZE)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Core_Controller::Enable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_Core_Controller::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Controller::Disable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_Core_Controller::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Controller::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Controller::Release_UI()
{
	m_dTimeOpenCoreMenu = 0.0;

	m_iCommandFlag = COMMAND_CORE_INITIALIZE;
}

_uint CUI_Core_Controller::Command_Open_UI(_double dTimeDelta)
{
	_uint iResult = FALSE;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UISTATE eBackState = CUI::UISTATE::ACTIVATE;
	CUI::UISTATE eDecoState = CUI::UISTATE::ACTIVATE;

	CUI::UISTATE eStatusState = CUI::UISTATE::ENABLE;
	CUI::UISTATE eCursorState = CUI::UISTATE::ENABLE;
	CUI::UISTATE eButtonState = CUI::UISTATE::ENABLE;
	
	/////////////////////////////////////////////////////////
	
	CUI_Core_ScrollList::UISCROLLSTATEDESC	tScrollStateDesc;

	tScrollStateDesc.eState = CUI::UISTATE::ACTIVATE;
	tScrollStateDesc.eClass = CInventory::ITEMCLASS::ITEM;

	/////////////////////////////////////////////////////////

	CUI_Core_Title::UITITLEDESC		tTitleDesc;

	tTitleDesc.eState = CUI::UISTATE::ACTIVATE;
	tTitleDesc.eType = CUI_Core_Title::TITLETYPE::ITEM;

	/////////////////////////////////////////////////////////

	CPlayer::INPUTSTATE eInputState = CPlayer::INPUTSTATE::UI;

	m_dTimeOpenCoreMenu += dTimeDelta * 10.0;

	_uint	iSwitch = (_uint)m_dTimeOpenCoreMenu;

	// TimeDelta를 10배속한다음 switch문을 돌리기 위해 소숫점 이하 절사
	// 즉, Activate() 최초 진입 후 1초 후부터 0.1초 간격으로 각 버튼에 대한 ENABLE 실시
	
	switch (iSwitch)
	{
	case 1:
#ifdef _DEBUG
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CURSOR"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CURSOR, &eCursorState));
#endif
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_BACKGROUND"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_BACKGROUND, &eBackState));

		if (false == m_bIsOnce)
		{
			m_fBGMVolumeFadeOutTarget = CSoundMgr::Get_Instance()->Get_CrntVolume(CSoundMgr::BGM);
			m_fBGMVolumeFadeInTarget = CSoundMgr::Get_Instance()->Get_CrntVolume(CSoundMgr::BGM);
			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
			CSoundMgr::Get_Instance()->PlaySound(L"OpenMenu.mp3", CSoundMgr::CHANNELID::UI);
			m_bIsOnce = true;
		}
		
		break;
	case 10:
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DECOLINE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DECOLINE, &eDecoState));
		break;
	case 20:
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_TITLE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_TITLE, &tTitleDesc));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST, &tScrollStateDesc));

		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eStatusState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eStatusState));

		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_MAP"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 21:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_QUEST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 22:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_ITEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 23:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_WEAPON"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 24:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_EQUIPMENT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 25:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_DATA"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 26:
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_SYSTEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		break;
	case 27:
		pGameInstance->Get_Observer(TEXT("OBSERVER_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_CONTROLLER_REQUEST, &eInputState));

#ifndef _DEBUG
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CURSOR"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CURSOR, &eCursorState));
#endif
		m_dTimeOpenCoreMenu = 0.0;
		iResult = SUCCESS;
		break;
	default:
		break;
	}
	pGameInstance->Set_Pause(true);

	if (m_fBGMVolumeTargetMin < m_fBGMVolumeFadeOutTarget)
	{
		CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::BGM, &m_fBGMVolumeFadeOutTarget);
	}

	RELEASE_INSTANCE(CGameInstance);

	return iResult;
}

_uint CUI_Core_Controller::Command_Close_UI(_double dTimeDelta)
{
	_uint iResult = FALSE;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CUI::UISTATE eBackState = CUI::UISTATE::INACTIVATE;
	CUI::UISTATE eDecoState = CUI::UISTATE::DISABLE;

	CUI::UISTATE eStatusState = CUI::UISTATE::DISABLE;
	CUI::UISTATE eCursorState = CUI::UISTATE::DISABLE;
	CUI::UISTATE eButtonState = CUI::UISTATE::DISABLE;

	//////////////////////////////////////////////////////

	CUI_Core_ScrollList::UISCROLLSTATEDESC	tScrollStateDesc;

	tScrollStateDesc.eState = CUI::UISTATE::DISABLE;
	tScrollStateDesc.eClass = CInventory::ITEMCLASS::ALL;
	
	//////////////////////////////////////////////////////

	CUI_Core_SelectList::UISELECTDESC	tSelectListDesc;

	tSelectListDesc.iCommandKey = CUI_Core_SelectList::COMMAND_CLOSE;

	//////////////////////////////////////////////////////

	CUI_Core_Title::UITITLEDESC		tTitleDesc;

	tTitleDesc.eState = CUI::UISTATE::DISABLE;
	tTitleDesc.eType = CUI_Core_Title::TITLETYPE::COUNT;

	///////////////////////////////////////////////////////

	CPlayer::INPUTSTATE eInputState = CPlayer::INPUTSTATE::INGAME;

	m_dTimeOpenCoreMenu += dTimeDelta * 10.0;

	CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::BGM, &m_fBGMVolumeFadeInTarget);

	_uint	iSwitch = (_uint)m_dTimeOpenCoreMenu;

	switch (iSwitch)
	{
	case 1:
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
		CSoundMgr::Get_Instance()->PlaySound(L"CloseMenu.mp3", CSoundMgr::CHANNELID::UI);
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_BACKGROUND"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_BACKGROUND, &eBackState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DECOLINE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DECOLINE, &eDecoState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_TITLE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_TITLE, &tTitleDesc));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eStatusState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eStatusState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_STATE, &eStatusState));

		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_MAP"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_QUEST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_ITEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_WEAPON"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_EQUIPMENT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_DATA"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_SYSTEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eButtonState));

		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST, &eButtonState));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SELECTLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLIST, &tSelectListDesc));

		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CURSOR"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CURSOR, &eCursorState));
		break;
	case 15:	// Warning : Hard-Coded Parameter, Based On CUI_Core_Background's expected inactivate time
		pGameInstance->Get_Observer(TEXT("OBSERVER_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_CONTROLLER_REQUEST, &eInputState));
		m_dTimeCloseCoreMenu = 0.0;
		//m_fBGMVolumeTarget = 1.f;
		//m_fBGMVolumeTarget = m_fBGMVolumeFadeInTarget;
		iResult = SUCCESS;
		pGameInstance->Set_Pause(false);
		break;
	default:
		break;
	}
	m_bIsOnce = false;
	

	RELEASE_INSTANCE(CGameInstance);

	return iResult;
}

_uint CUI_Core_Controller::Command_HeadButton_Lock(_double dTimeDelta)
{
	// 버튼락 요청이 들어오면 모든 버튼에 대해 메뉴 제어 기능 제한

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_bool	bIsLock = true;

	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_MAP"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_QUEST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_ITEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_WEAPON"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_EQUIPMENT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_DATA"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_SYSTEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));

	RELEASE_INSTANCE(CGameInstance);

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_HeadButton_Unlock(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_bool	bIsLock = false;

	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_MAP"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_QUEST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_ITEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_WEAPON"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_EQUIPMENT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_DATA"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));
	pGameInstance->Get_Observer(TEXT("OBSERVER_HEADBUTTON_SYSTEM"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON_LOCK, &bIsLock));

	RELEASE_INSTANCE(CGameInstance);

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_ListLine_Lock(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_uint	iFlagCommand = COMMAND_CORE_LISTLINE_LOCK;

	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_0"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_1"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_2"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_3"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_4"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_5"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_6"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_7"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));

	RELEASE_INSTANCE(CGameInstance);

	m_eCurrentInputLimitLevel = INPUTLIMITLEVEL::LISTLINE;

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_ListLine_Unlock(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_uint	iFlagCommand = COMMAND_CORE_LISTLINE_UNLOCK;

	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_0"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_1"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_2"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_3"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_4"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_5"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_6"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));
	pGameInstance->Get_Observer(TEXT("OBSERVER_LISTLINE_7"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_COMMAND, &iFlagCommand));

	RELEASE_INSTANCE(CGameInstance);

	m_eCurrentInputLimitLevel = INPUTLIMITLEVEL::HEADBUTTON;

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_Scroll_Up(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CUI_Core_ScrollList::UISCROLLDESC	tDesc;

	tDesc.bIsUp = true;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST_CONTROL, &tDesc));

	RELEASE_INSTANCE(CGameInstance);

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_Scroll_Down(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CUI_Core_ScrollList::UISCROLLDESC	tDesc;

	tDesc.bIsUp = false;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST_CONTROL, &tDesc));

	RELEASE_INSTANCE(CGameInstance);

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_Item_Body_Open(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eDisable = CUI::UISTATE::DISABLE;
	CUI::UISTATE	eEnable = CUI::UISTATE::ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eEnable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eEnable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_STATE, &eDisable));

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_Weapon_Body_Open(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eDisable = CUI::UISTATE::DISABLE;
	CUI::UISTATE	eEnable = CUI::UISTATE::ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eDisable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eDisable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_STATE, &eEnable));

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_Chip_Body_Open(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eDisable = CUI::UISTATE::DISABLE;
	CUI::UISTATE	eEnable = CUI::UISTATE::ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eEnable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eEnable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_STATE, &eDisable));

	return SUCCESS;
}

_uint CUI_Core_Controller::Command_All_Body_Close(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eDisable = CUI::UISTATE::DISABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL, &eDisable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS, &eDisable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_STATE, &eDisable));

	return SUCCESS;
}

void CUI_Core_Controller::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		if (UISTATE::DISABLE != m_eUIState)
		{
			PACKET*	pPacket = (PACKET*)pMessage;

			COMMANDDESC	tDesc = *(COMMANDDESC*)pPacket->pData;

			m_eUIState = UISTATE::ACTIVATE;

			if (!(m_iCommandFlag & tDesc.iCommandFlag))
			{
				m_iCommandFlag |= tDesc.iCommandFlag;
			}
		}
	}
}

_int CUI_Core_Controller::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_CONTROLLER)
		{
			return TRUE;
		}
	}
	return FALSE;
}
