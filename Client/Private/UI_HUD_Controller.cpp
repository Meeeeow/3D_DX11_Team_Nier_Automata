#include "stdafx.h"
#include "..\Public\UI_HUD_Controller.h"
#include "UI_Core_Title.h"
#include "UI_Core_ScrollList.h"
#include "UI_Core_ListLine.h"
#include "Player.h"

CUI_HUD_Controller::CUI_HUD_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Controller::CUI_HUD_Controller(const CUI_HUD_Controller & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Controller * CUI_HUD_Controller::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Controller* pInstance = new CUI_HUD_Controller(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Controller::Clone(void * pArg)
{
	CUI_HUD_Controller* pInstance = new CUI_HUD_Controller(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Controller");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Controller::Free()
{
	__super::Free();
}

HRESULT CUI_HUD_Controller::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CUI_HUD_Controller::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Controller::SetUp_Components()))
	{
		return E_FAIL;
	}

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_HUD_CONTROLLER"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Controller::SetUp_Components()
{
	return S_OK;
}

_int CUI_HUD_Controller::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Controller::LateTick(_double TimeDelta)
{

	return 0;
}

HRESULT CUI_HUD_Controller::Render()
{
	return S_OK;
}

HRESULT CUI_HUD_Controller::Activate(_double dTimeDelta)
{
	if (m_iCommandFlag & COMMAND_HUD_ENABLE)
	{
		if (SUCCESS == Command_Enable_HUD(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_HUD_ENABLE;
		}
	}
	if (m_iCommandFlag & COMMAND_HUD_RELEASE)
	{
		if (SUCCESS == Command_Release_HUD(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_HUD_RELEASE;
		}
	}
	if (m_iCommandFlag & COMMAND_HUD_DISABLE)
	{
		if (SUCCESS == Command_Disable_HUD(dTimeDelta))
		{
			m_iCommandFlag ^= COMMAND_HUD_DISABLE;
		}
	}

	// When flag processing is completed properly, will be entered in this branch
	if (m_iCommandFlag == COMMAND_HUD_INITIALIZE)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_HUD_Controller::Enable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_HUD_Controller::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Controller::Disable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_HUD_Controller::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Controller::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Controller::Release_UI()
{
	m_dTimeOpenCoreMenu = 0.0;

	m_iCommandFlag = COMMAND_HUD_INITIALIZE;
}

_uint CUI_HUD_Controller::Command_Enable_HUD(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eEnable = CUI::UISTATE::ENABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eEnable));
	//pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eEnable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_PODCOOLTIME"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eEnable));

	return SUCCESS;
}

_uint CUI_HUD_Controller::Command_Disable_HUD(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI::UISTATE	eDisable = CUI::UISTATE::DISABLE;

	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eDisable));
	//pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eDisable));
	pGameInstance->Get_Observer(TEXT("OBSERVER_PODCOOLTIME"))->Notify((void*)&PACKET(CHECKSUM_ONLY_UISTATE, &eDisable));

	return SUCCESS;
}

_uint CUI_HUD_Controller::Command_Release_HUD(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_RELEASE_UI, nullptr));
	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_RELEASE_UI, nullptr));
	pGameInstance->Get_Observer(TEXT("OBSERVER_PODCOOLTIME"))->Notify((void*)&PACKET(CHECKSUM_RELEASE_UI, nullptr));

	return SUCCESS;
}

void CUI_HUD_Controller::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
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

_int CUI_HUD_Controller::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_CONTROLLER)
		{
			return 1;
		}
	}
	return FALSE;
}
