#include "stdafx.h"
#include "..\Public\UI_Core_HeadButton.h"
#include "UI_Core_Title.h"
#include "UI_Core_ScrollList.h"
#include "UI_Core_Controller.h"
#include "SoundMgr.h"

CUI_Core_HeadButton::CUI_Core_HeadButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_HeadButton::CUI_Core_HeadButton(const CUI_Core_HeadButton & rhs)
	: CUI(rhs)
{
}

CUI_Core_HeadButton * CUI_Core_HeadButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_HeadButton* pInstance = new CUI_Core_HeadButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_HeadButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_HeadButton::Clone(void * pArg)
{
	CUI_Core_HeadButton* pInstance = new CUI_Core_HeadButton(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_HeadButton");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_HeadButton::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_HeadButton::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_HeadButton::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_HeadButton::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Color Pallette
	// 194 192 168 -> System Base Color
	// 174 170 149 -> Icon Back
	// 80 77 66 -> Highlighted

	// Background Default : 224, 223, 211, 128

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-60.f, -60.f, -80.f, 50.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(-40.f, -40.f, -40.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Background A

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-144.f, -145.f, -145.f, 50.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(-125.f, -125.f, -125.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Background B

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Enable Icon

	XMStoreFloat4(&vColorOverlay, XMVectorSet(80.f, 77.f, 66.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Activate Icon

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 2;
	m_bCollision = true;
	
	CUI_Core_HeadButton::BUTTONTYPE eType = BUTTONTYPE::COUNT;

	if (nullptr != pArg)
	{
		eType = *(CUI_Core_HeadButton::BUTTONTYPE*)pArg;
		m_tDesc.eType = eType;
	}

	const _tchar*	szObserverKey = nullptr;

	switch (m_tDesc.eType)
	{
	case CUI_Core_HeadButton::BUTTONTYPE::MAP:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_MAP");
		m_tDesc.szTextElemKey = TEXT("MAP");
		m_tDesc.szIconElemKey = TEXT("BUTTON_MAP");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::QUEST:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_QUEST");
		m_tDesc.szTextElemKey = TEXT("QUEST");
		m_tDesc.szIconElemKey = TEXT("BUTTON_QUEST");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::ITEM:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_ITEM");
		m_tDesc.szTextElemKey = TEXT("ITEM");
		m_tDesc.szIconElemKey = TEXT("BUTTON_ITEM");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::WEAPON:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_WEAPON");
		m_tDesc.szTextElemKey = TEXT("WEAPON");
		m_tDesc.szIconElemKey = TEXT("BUTTON_WEAPON");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::SKILL:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_EQUIPMENT");
		m_tDesc.szIconElemKey = TEXT("BUTTON_EQUIPMENT");
		m_tDesc.szTextElemKey = TEXT("EQUIPMENT");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::DATA:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_DATA");
		m_tDesc.szIconElemKey = TEXT("BUTTON_DATA");
		m_tDesc.szTextElemKey = TEXT("DATA");
		break;
	case CUI_Core_HeadButton::BUTTONTYPE::SYSTEM:
		szObserverKey = TEXT("OBSERVER_HEADBUTTON_SYSTEM");
		m_tDesc.szIconElemKey = TEXT("BUTTON_SYSTEM");
		m_tDesc.szTextElemKey = TEXT("SYSTEM");
		break;
	}

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(szObserverKey, this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	} 

	CObserver*	pObserver = pGameInstance->Get_Observer(szObserverKey);

	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CUI_Core_HeadButton::SetUp_Components()
{
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_RECT, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_ATLAS, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

_int CUI_Core_HeadButton::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_HeadButton::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_HeadButton::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fRenderPosBaseX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY, 0.45f, 0.15f);	// 144 * 48
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(2);	// Atlas_Button_Activate
	
	////////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, m_tDesc.szIconElemKey);
	SetUp_Transform(m_fRenderPosBaseX - m_fRenderIconPosOffsetX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY, 0.6f, 0.6f);
	SetUp_DefaultRawValue();

	if (m_eUIState == CUI::UISTATE::ENABLE)
	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[4], sizeof(_float4));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[5], sizeof(_float4));
	}
	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[5], sizeof(_float4));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[4], sizeof(_float4));
	}
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
	m_pModelCom->Render(11); // Atlas_Color_Overlay_On_Inverse

	////////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_tDesc.szTextElemKey);
	SetUp_Transform(m_fRenderPosBaseX + m_fRenderTextPosOffsetX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY, 1.f, 1.f);
	SetUp_DefaultRawValue();

	if (m_eUIState == CUI::UISTATE::ENABLE)
	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
	}
	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// Font Origin Color
	}
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12); // Atlas_Color_Overlay_On_One

	////////////////////////////////////////////////////////////////////////////////

	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		SetUp_Transform(m_fRenderPosBaseX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY - m_fActivateLineOffsetY + m_fPressedAnimPosOffsetY, 1.44f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		SetUp_Transform(m_fRenderPosBaseX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY + m_fActivateLineOffsetY - m_fPressedAnimPosOffsetY, 1.44f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);
	}

	////////////////////////////////////////////////////////////////////////////////

	// PtInRect Scope Setting
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fRenderPosBaseX + (m_fRenderIntervalPosOffsetX * (_float)m_tDesc.eType), m_fRenderPosBaseY, 0.45f, 0.15f);	// 144 * 48

	return S_OK;
}

HRESULT CUI_Core_HeadButton::Activate(_double dTimeDelta)
{
	if (m_fUVAnimCursorXMin == m_fUVAnimCursorX && false == m_bBodyCallLock)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
		CSoundMgr::Get_Instance()->PlaySound(L"ActivateButton.mp3", CSoundMgr::CHANNELID::UI);

		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		CUI_Core_Title::UITITLEDESC		tTitleDesc;

		tTitleDesc.eState = CUI::UISTATE::ACTIVATE;

		tTitleDesc.eType = (CUI_Core_Title::TITLETYPE)m_tDesc.eType;

		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_TITLE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_TITLE, &tTitleDesc));

		////////////////////////////////////////////////////////////////////
		
		CUI_Core_ScrollList::UISCROLLSTATEDESC	tScrollStateDesc;
		CUI_Core_Controller::COMMANDDESC		tCommandDesc;

		tScrollStateDesc.eState = CUI::UISTATE::ACTIVATE;
		
		switch (m_tDesc.eType)
		{
		case BUTTONTYPE::ITEM:
			tScrollStateDesc.eClass = CInventory::ITEMCLASS::ITEM;
			tCommandDesc.iCommandFlag = COMMAND_CORE_ITEM_BODY_OPEN;
			break;
		case BUTTONTYPE::WEAPON:
			tScrollStateDesc.eClass = CInventory::ITEMCLASS::WEAPON;
			tCommandDesc.iCommandFlag = COMMAND_CORE_WEAPON_BODY_OPEN;
			break;
		case BUTTONTYPE::SKILL:
			tScrollStateDesc.eClass = CInventory::ITEMCLASS::EQUIPMENT;
			tCommandDesc.iCommandFlag = COMMAND_CORE_CHIP_BODY_OPEN;
			break;
		default:
			tScrollStateDesc.eState = CUI::UISTATE::DISABLE;
			tCommandDesc.iCommandFlag = COMMAND_CORE_ALL_BODY_CLOSE;
			break;
		}
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tCommandDesc));
		pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST, &tScrollStateDesc));
	}

	if (m_fUVAnimCursorX < m_fUVAnimCursorXMax)
	{
		m_fUVAnimCursorX += _float(dTimeDelta);
	}
	else
	{
		m_fUVAnimCursorX = m_fUVAnimCursorXMax;
	}
	if (m_fActivateLineOffsetY < m_fActivateLineOffsetYMax)
	{
		m_fActivateLineOffsetY += _float(dTimeDelta) * m_fActivateLineOffsetYSpeed;
	}
	else 
	{
		m_fActivateLineOffsetY = m_fActivateLineOffsetYMax;
	}
	return S_OK;
}

HRESULT CUI_Core_HeadButton::Enable(_double dTimeDelta)
{
	if (m_fEnableAnimAlpha < m_fEnableAnimAlphaMax)
	{
		m_fEnableAnimAlpha += (_float)dTimeDelta * m_fEnableAnimAlphaSpeed;
	}
	else
	{
		m_fEnableAnimAlpha = m_fEnableAnimAlphaMax;
	}
	m_vecColorOverlays[0].w = m_fEnableAnimAlpha;
	m_vecColorOverlays[1].w = m_fEnableAnimAlpha;

	// For ColorOverlay which have zero-alpha
	if (m_fEnableAnimAlpha <= 0.f)
	{
		m_vecColorOverlays[2].w = m_fEnableAnimAlpha;
		m_vecColorOverlays[3].w = m_fEnableAnimAlpha;
		m_vecColorOverlays[4].w = m_fEnableAnimAlpha;
		m_vecColorOverlays[5].w = m_fEnableAnimAlpha;
	}
	return S_OK;
}

HRESULT CUI_Core_HeadButton::Inactivate(_double dTimeDelta)
{
	_uint	iFlag = 0x00000000;

	if (m_fUVAnimCursorXMin < m_fUVAnimCursorX)
	{
		m_fUVAnimCursorX -= _float(dTimeDelta * 2);
	}
	else
	{
		m_fUVAnimCursorX = m_fUVAnimCursorXMin;

		iFlag |= 0x00000001;
	}

	if (m_fActivateLineOffsetYMin < m_fActivateLineOffsetY)
	{
		m_fActivateLineOffsetY -= _float(dTimeDelta) * m_fActivateLineOffsetYSpeed;
	}
	else
	{
		m_fActivateLineOffsetY = m_fActivateLineOffsetYMin;

		iFlag |= 0x00000002;
	}

	if (iFlag & 0x00000001 && iFlag & 0x00000002)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
		iFlag = 0;
	}

	return S_OK;
}

HRESULT CUI_Core_HeadButton::Disable(_double dTimeDelta)
{
	// Initialize Parameter for Anim

	m_fUVAnimCursorX = 0.f;

	m_fEnableAnimAlpha = m_fEnableAnimAlphaMin;

	m_fActivateLineOffsetY = m_fActivateLineOffsetYMin;

	m_fPressedAnimPosOffsetY = 0.f;

	return S_OK;
}

HRESULT CUI_Core_HeadButton::Pressed(_double dTimeDelta)
{
	m_fUVAnimCursorX = m_fUVAnimCursorXMax;

	// When the Button is Pressed, should still be held button activate lock
	if (m_bPressed == true)
	{
		if (0.f == m_fPressedAnimPosOffsetY)
		{
			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
			CSoundMgr::Get_Instance()->PlaySound(L"PressButton.mp3", CSoundMgr::CHANNELID::UI);

			CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

			CUI_Core_Title::UITITLEDESC		tTitleDesc;

			tTitleDesc.eState = CUI::UISTATE::ACTIVATE;
			tTitleDesc.eType = (CUI_Core_Title::TITLETYPE)m_tDesc.eType;
			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_TITLE"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_TITLE, &tTitleDesc));

			/////////////////////////////

			CUI_Core_ScrollList::UISCROLLSTATEDESC	tScrollStateDesc;
			CUI_Core_Controller::COMMANDDESC		tCommandDesc;
				
			tScrollStateDesc.eState = CUI::UISTATE::ACTIVATE;
			tCommandDesc.iCommandFlag |= COMMAND_CORE_BUTTON_LOCK;

			switch (m_tDesc.eType)
			{
			case BUTTONTYPE::ITEM:
				tScrollStateDesc.eClass = CInventory::ITEMCLASS::ITEM;
				tCommandDesc.iCommandFlag |= COMMAND_CORE_ITEM_BODY_OPEN;
				break;
			case BUTTONTYPE::WEAPON:
				tScrollStateDesc.eClass = CInventory::ITEMCLASS::WEAPON;
				tCommandDesc.iCommandFlag |= COMMAND_CORE_WEAPON_BODY_OPEN;
				break;
			case BUTTONTYPE::SKILL:
				tScrollStateDesc.eClass = CInventory::ITEMCLASS::EQUIPMENT;
				tCommandDesc.iCommandFlag |= COMMAND_CORE_CHIP_BODY_OPEN;
				break;
			default:
				tScrollStateDesc.eState = CUI::UISTATE::DISABLE;
				tCommandDesc.iCommandFlag |= COMMAND_CORE_ALL_BODY_CLOSE;
				break;
			}
			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tCommandDesc));
			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SCROLLLIST, &tScrollStateDesc));
		}

		// 180 degree In 0.5s
		m_fPressedAnimTimeAcc += (_float)dTimeDelta * 360.f;

		if (180.f < m_fPressedAnimTimeAcc)
		{
			m_fPressedAnimPosOffsetY = 0.f;
			m_fPressedAnimTimeAcc = 0.f;
			m_bPressed = false;
		}

		// resultY == 0 ~ (1 * ratio)
		m_fPressedAnimPosOffsetY = sinf(XMConvertToRadians(m_fPressedAnimTimeAcc)) * m_fPressedAnimPosOffsetRatioY;
	}
	return S_OK;
}

HRESULT CUI_Core_HeadButton::Released(_double dTimeDelta)
{
	CUI_Core_Controller::COMMANDDESC	tDesc;

	tDesc.iCommandFlag = COMMAND_CORE_BUTTON_UNLOCK;

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));

	m_eUIState = CUI::UISTATE::INACTIVATE;

	return S_OK;
}

void CUI_Core_HeadButton::Notify(void * pMessage)
{
	_int iResult = VerifyChecksum(pMessage);

	if (TRUE == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		if (pState == UISTATE::PRESSED)
		{
			m_bPressed = true;
		}

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::INACTIVATE == pState)	{ m_eUIState = pState; }
			if (UISTATE::PRESSED == pState)		{ m_eUIState = pState; }
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::ACTIVATE == pState)	{ m_eUIState = pState; }
			if (UISTATE::PRESSED == pState)		{ m_eUIState = pState; }
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			if (UISTATE::ACTIVATE == pState)	{ m_eUIState = pState; }
			if (UISTATE::PRESSED == pState)		{ m_eUIState = pState; }
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ENABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::PRESSED:
			if (UISTATE::INACTIVATE == pState)	{ m_eUIState = pState; }
			if (UISTATE::RELEASED == pState)	{ m_eUIState = pState; }
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::RELEASED:
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::NONE:
			break;
		default:
			break;
		}
	}
	else if (2 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		_bool	bIsLocked = *(_bool*)pPacket->pData;

		m_bBodyCallLock = bIsLocked;
	}
}

_int CUI_Core_HeadButton::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_HEADBUTTON)
		{
			return TRUE;
		}
		else if (*check == CHECKSUM_UI_CORE_HEADBUTTON_LOCK)
		{
			return 2;
		}
	}
	return FALSE;
}
