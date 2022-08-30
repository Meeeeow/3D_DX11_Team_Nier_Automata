#include "stdafx.h"
#include "..\Public\UI_Core_ListLine.h"
#include "UI_Core_Controller.h"
#include "UI_Core_SelectList.h"
#include "UI_Core_Detail.h"
#include "Inventory.h"
#include "SoundMgr.h"

CUI_Core_ListLine::CUI_Core_ListLine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_ListLine::CUI_Core_ListLine(const CUI_Core_ListLine & rhs)
	: CUI(rhs)
{
}

CUI_Core_ListLine * CUI_Core_ListLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_ListLine* pInstance = new CUI_Core_ListLine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_ListLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_ListLine::Clone(void * pArg)
{
	CUI_Core_ListLine* pInstance = new CUI_Core_ListLine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_ListLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_ListLine::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_ListLine::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_ListLine::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_ListLine::SetUp_Components()))
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
	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-40.f, -40.f, -40.f, 50.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(-224.f, -223.f, -211.f, -255.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Background A

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-144.f, -145.f, -145.f, 50.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(-125.f, -125.f, -125.f, 50.f));

	// 까매지는 이유. 백그라운드는 0까지 가야하는데 Enable Anim에서 50까지 올려버리고 있음
	// 그런데 2번 색상은 0까지여서는 곤란함. 이 부분에 대한 수정 필요.

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
	m_iDistanceZ = 3;
	m_bCollision = true;

	if (nullptr != pArg)
	{
		m_iInitialLineNumber = *(_uint*)pArg;
	}

	m_tLineDesc.fCrntLineOffsetScaleY = (_float)m_iInitialLineNumber;
	m_tLineDesc.fPrevLineOffsetScaleY = (_float)m_iInitialLineNumber;

	const _tchar*	szObserverKey = nullptr;

	switch (m_iInitialLineNumber)
	{
	case 0:
		szObserverKey = TEXT("OBSERVER_LISTLINE_0");
		break;
	case 1:
		szObserverKey = TEXT("OBSERVER_LISTLINE_1");
		break;
	case 2:
		szObserverKey = TEXT("OBSERVER_LISTLINE_2");
		break;
	case 3:
		szObserverKey = TEXT("OBSERVER_LISTLINE_3");
		break;
	case 4:
		szObserverKey = TEXT("OBSERVER_LISTLINE_4");
		break;
	case 5:
		szObserverKey = TEXT("OBSERVER_LISTLINE_5");
		break;
	case 6:
		szObserverKey = TEXT("OBSERVER_LISTLINE_6");
		break;
	case 7:
		szObserverKey = TEXT("OBSERVER_LISTLINE_7");
		break;
	case 8:
		szObserverKey = TEXT("OBSERVER_LISTLINE_8");
		break;
	case 9:
		szObserverKey = TEXT("OBSERVER_LISTLINE_9");
		break;
	default:
		szObserverKey = TEXT("");
		break;
	}

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Create_Observer(szObserverKey, this)))
	{	
		return E_FAIL;
	}	
	return S_OK;
}

HRESULT CUI_Core_ListLine::SetUp_Components()
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

_int CUI_Core_ListLine::Tick(_double TimeDelta)
{
	//if (m_tLineDesc.fPrevLineOffsetScaleY != m_tLineDesc.fCrntLineOffsetScaleY)
	//{
	//	// Scroll Up
	//	if (m_tLineDesc.fPrevLineOffsetScaleY < m_tLineDesc.fCrntLineOffsetScaleY)
	//	{
	//		m_tLineDesc.fPrevLineOffsetScaleY += (_float)TimeDelta * 4.f;

	//		if (m_tLineDesc.fCrntLineOffsetScaleY < m_tLineDesc.fPrevLineOffsetScaleY)
	//		{
	//			m_iCommandFlag ^= COMMAND_CORE_LISTLINE_SCROLLUP;

	//			m_tLineDesc.fPrevLineOffsetScaleY = m_tLineDesc.fCrntLineOffsetScaleY;

	//			if (7.f < m_tLineDesc.fPrevLineOffsetScaleY)
	//			{
	//				m_tLineDesc.fPrevLineOffsetScaleY = 0.f;
	//				m_tLineDesc.fCrntLineOffsetScaleY = 0.f;					
	//			}
	//		}
	//	}
	//	// Scroll Down
	//	else
	//	{
	//		m_tLineDesc.fPrevLineOffsetScaleY -= (_float)TimeDelta * 4.f;

	//		if (m_tLineDesc.fPrevLineOffsetScaleY < m_tLineDesc.fCrntLineOffsetScaleY)
	//		{
	//			m_iCommandFlag ^= COMMAND_CORE_LISTLINE_SCROLLDOWN;

	//			m_tLineDesc.fPrevLineOffsetScaleY = m_tLineDesc.fCrntLineOffsetScaleY;

	//			if (m_tLineDesc.fPrevLineOffsetScaleY < 0.f)
	//			{
	//				m_tLineDesc.fPrevLineOffsetScaleY = 7.f;
	//				m_tLineDesc.fCrntLineOffsetScaleY = 7.f;
	//			}				
	//		}
	//	}
	//}

	return CUI::Tick(TimeDelta);
}

_int CUI_Core_ListLine::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_ListLine::Render()
{
	// For Update player's inventory in realtime
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CInventory*		pPlayerInventory = nullptr;

	pPlayerInventory = (CInventory*)(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Get_Component(COM_KEY_INVENTORY));

	if (nullptr == pPlayerInventory)
	{
		MSGBOX("Failed to Access Player Inventory - ScrollList");
		return E_FAIL;
	}

	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		//SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY - m_fActivateLineOffsetY + m_fPressedAnimPosOffsetY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 2.58f, 0.5f);
		SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY - m_fActivateLineOffsetY + m_fPressedAnimPosOffsetY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 2.88f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		//SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + m_fActivateLineOffsetY - m_fPressedAnimPosOffsetY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 2.58f, 0.5f);
		SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + m_fActivateLineOffsetY - m_fPressedAnimPosOffsetY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 2.88f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);
	}

	////////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	//SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 0.8f, 0.1f);	// 256 * 32
	SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 0.9f, 0.1f);	// 288 * 32
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(2);	// Atlas_Button_Activate
	
	////////////////////////////////////////////////////////////////////////////////
	m_tLineDesc.tItemDesc.szIconElemKey = pPlayerInventory->Open_ItemSlot(m_tLineDesc.tItemDesc.iItemCode)->szIconElemKey;

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, pPlayerInventory->Open_ItemSlot(m_tLineDesc.tItemDesc.iItemCode)->szIconElemKey);
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, m_tLineDesc.tItemDesc.szIconElemKey);
	//SetUp_Transform(m_fRenderPosBaseX - m_fRenderIconPosOffsetX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 0.4f, 0.4f);
	SetUp_Transform(m_fRenderPosBaseX - m_fRenderIconPosOffsetX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 0.4f, 0.4f);
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

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_tLineDesc.tItemDesc.szTextElemKey);
	//SetUp_Transform_AlignLeft(m_fRenderPosBaseX + m_fRenderTextPosOffsetX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 1.f, 1.f);
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX + m_fRenderTextPosOffsetX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 1.f, 1.f);
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

	if ((_uint)ITEMCATEGORY::ENFORCECHIP != pPlayerInventory->Open_ItemSlot(m_tLineDesc.tItemDesc.iItemCode)->iItemCategory)
	{
		m_iBufferNumber = pPlayerInventory->Open_ItemSlot(m_tLineDesc.tItemDesc.iItemCode)->iItemCount;
		m_tLineDesc.tItemDesc.iItemCount = m_iBufferNumber;

		do
		{
			m_iRenderNumber = m_iBufferNumber % 10;
			m_iBufferNumber = m_iBufferNumber / 10;

			_tchar	buf[2];

			ZeroMemory(buf, sizeof(_tchar) * 2);
			_stprintf_s(buf, TEXT("%d"), m_iRenderNumber);

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
			SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 1.f, 1.f);
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
			m_pModelCom->Render(12);

			m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
		} while (0 < m_iBufferNumber);

		m_fRenderAmountLetterIntervalAccX = 0.f;
	}

	////////////////////////////////////////////////////////////////////////////////

	// PtInRect Scope Setting
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	//SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)m_tLineDesc.eType), 0.8f, 0.1f);	// 256 * 32
	SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY), 0.9f, 0.1f);	// 288 * 32

	return S_OK;
}

HRESULT CUI_Core_ListLine::Activate(_double dTimeDelta)
{
	if (m_fUVAnimCursorXMin == m_fUVAnimCursorX)
	{
		if (!(m_iCommandFlag & COMMAND_CORE_LISTLINE_LOCK))
		{
			CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL_UPDATE, &m_tLineDesc.tItemDesc));

			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
			CSoundMgr::Get_Instance()->PlaySound(L"ActivateButton.mp3", CSoundMgr::CHANNELID::UI);
		}
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

HRESULT CUI_Core_ListLine::Enable(_double dTimeDelta)
{
	if (m_fEnableAnimAlpha < m_fEnableAnimAlphaMax)
	{
		m_fEnableAnimAlpha += (_float)dTimeDelta * m_fEnableAnimAlphaSpeed;
	}
	else
	{
		m_fEnableAnimAlpha = m_fEnableAnimAlphaMax;
	}
	//m_vecColorOverlays[0].w = m_fEnableAnimAlpha;
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

HRESULT CUI_Core_ListLine::Inactivate(_double dTimeDelta)
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

HRESULT CUI_Core_ListLine::Disable(_double dTimeDelta)
{
	// Initialize Parameter for Anim

	m_fUVAnimCursorX = 0.f;

	m_fEnableAnimAlpha = m_fEnableAnimAlphaMin;

	m_fActivateLineOffsetY = m_fActivateLineOffsetYMin;

	m_tLineDesc.fPrevLineOffsetScaleY = (_float)m_iInitialLineNumber;

	m_fPressedAnimPosOffsetY = 0.f;

	m_fPressedAnimTimeAcc = 0.f;

	m_bPressed = false;

	return S_OK;
}

HRESULT CUI_Core_ListLine::Pressed(_double dTimeDelta)
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

			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_DETAIL"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_DETAIL_UPDATE, &m_tLineDesc.tItemDesc));

			CUI_Core_SelectList::UISELECTDESC	tSelectDesc;

			tSelectDesc.iCommandKey = CUI_Core_SelectList::COMMAND_OPEN;
			//tSelectDesc.eCategory = (ITEMCATEGORY)m_tLineDesc.tItemDesc.iItemCategory;
			tSelectDesc.tItemDesc = m_tLineDesc.tItemDesc;
			tSelectDesc.fLinePosY = m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY);
			
			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SELECTLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLIST, &tSelectDesc));

			CUI_Core_Controller::COMMANDDESC	tDesc;

			tDesc.iCommandFlag = COMMAND_CORE_LINE_LOCK;

			pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));			
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

HRESULT CUI_Core_ListLine::Released(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CUI_Core_Controller::COMMANDDESC	tDesc;

	tDesc.iCommandFlag = COMMAND_CORE_LINE_UNLOCK;

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));

	CUI_Core_SelectList::UISELECTDESC	tSelectDesc;

	tSelectDesc.iCommandKey = CUI_Core_SelectList::COMMAND_CLOSE;
	tSelectDesc.fLinePosY = m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * m_tLineDesc.fPrevLineOffsetScaleY);

	pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_SELECTLIST"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLIST, &tSelectDesc));

	m_eUIState = CUI::UISTATE::INACTIVATE;

	return S_OK;
}

void CUI_Core_ListLine::Release_UI()
{
	m_fUVAnimCursorX = 0.f;

	m_fEnableAnimAlpha = m_fEnableAnimAlphaMin;

	m_fActivateLineOffsetY = m_fActivateLineOffsetYMin;

	m_tLineDesc.fPrevLineOffsetScaleY = (_float)m_iInitialLineNumber;

	m_fPressedAnimPosOffsetY = 0.f;

	m_fPressedAnimTimeAcc = 0.f;

	m_bPressed = false;
}

void CUI_Core_ListLine::Notify(void * pMessage)
{
	_int iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
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
			if (UISTATE::INACTIVATE == pState) { m_eUIState = pState; }
			if (UISTATE::PRESSED == pState) { m_eUIState = pState; }
			if (UISTATE::DISABLE == pState) { m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::ACTIVATE == pState) { m_eUIState = pState; }
			if (UISTATE::PRESSED == pState) { m_eUIState = pState; }
			if (UISTATE::DISABLE == pState) { m_eUIState = pState; }
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
		//if (UISTATE::DISABLE != m_eUIState)
		{
			PACKET*		pPacket = (PACKET*)pMessage;

			ITEMDESC*	pItemDesc = (ITEMDESC*)pPacket->pData;

			memcpy(&m_tLineDesc.tItemDesc, pItemDesc, sizeof(ITEMDESC));

			return;
		}
	}
	else if (3 == iResult)
	{
		if (UISTATE::DISABLE != m_eUIState)
		{
			PACKET*	pPacket = (PACKET*)pMessage;

			_uint	iAcceptedCommandFlag = *(_uint*)pPacket->pData;

			if (!(m_iCommandFlag & iAcceptedCommandFlag))
			{
				m_iCommandFlag |= iAcceptedCommandFlag;

				if (m_iCommandFlag & COMMAND_CORE_LISTLINE_UNLOCK)
				{
					m_iCommandFlag ^= COMMAND_CORE_LISTLINE_LOCK;
					m_iCommandFlag ^= COMMAND_CORE_LISTLINE_UNLOCK;
				}
			}
			return;
		}
	}
	else if (4 == iResult)
	{
		if (UISTATE::DISABLE != m_eUIState)
		{
			PACKET*	pPacket = (PACKET*)pMessage;

			_float fLineOffsetScaleY = *(_float*)pPacket->pData;

			m_tLineDesc.fPrevLineOffsetScaleY = fLineOffsetScaleY;
		}
	}
}

_int CUI_Core_ListLine::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_LISTLINE_STATE)
		{
			return 1;
		}
		else if (*check == CHECKSUM_UI_CORE_LISTLINE_UPDATE)
		{
			return 2;
		}
		else if (*check == CHECKSUM_UI_CORE_LISTLINE_COMMAND)
		{
			return 3;
		}
		else if (*check == CHECKSUM_UI_CORE_LISTLINE_POSITION)
		{
			return 4;
		}
	}
	return FALSE;
}
