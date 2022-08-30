#include "stdafx.h"
#include "..\Public\UI_Core_Detail.h"
#include "Inventory.h"

CUI_Core_Detail::CUI_Core_Detail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Detail::CUI_Core_Detail(const CUI_Core_Detail & rhs)
	: CUI(rhs)
{
}

CUI_Core_Detail * CUI_Core_Detail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Detail* pInstance = new CUI_Core_Detail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Detail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Detail::Clone(void * pArg)
{
	CUI_Core_Detail* pInstance = new CUI_Core_Detail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Detail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Detail::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_Detail::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_Detail::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Detail::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Background Default : 224, 223, 211, 128

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-145.f, -145.f, -145.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Head

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-20.f, -20.f, -20.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Body

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-40.f, -40.f, -40.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Icon Background

	XMStoreFloat4(&vColorOverlay, XMVectorSet(15.f, 15.f, 15.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Icon Color

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 2;
	m_bCollision = false;
	
	m_tDesc.szTextElemKey = TEXT("POTION_SMALL");
	m_tDesc.szIconElemKey = TEXT("ICON_EXPENDABLES");
	m_tDesc.szDetailElemKey = TEXT("POTION_SMALL_DETAIL");

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_DETAIL"), this)))
	{
		return E_FAIL;
	}	
	return S_OK;
}

HRESULT CUI_Core_Detail::SetUp_Components()
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

_int CUI_Core_Detail::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Detail::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_Detail::Render()
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

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fRenderPosBaseX, 440.f, 1.1f, 1.21f);	// 352 * 387
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimY", (void*)&m_fHeaderUVRangeY, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Head
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));	// Body
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(13);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fRenderPosBaseX, 385.f, 1.0f, 0.5f);	// 320 * 128
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[4], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(1);

	

	const _tchar*	szWeaponIconElemKey = nullptr;

	if (TEXT("WHITE_COVENANT") == m_tDesc.szTextElemKey)
		szWeaponIconElemKey = TEXT("KATANA");
	else if (TEXT("WHITE_CONTRACT") == m_tDesc.szTextElemKey)
		szWeaponIconElemKey = TEXT("LONGKATANA");
	else if (TEXT("40TH_SPEAR") == m_tDesc.szTextElemKey)
		szWeaponIconElemKey = TEXT("SPEAR");
	else if (TEXT("40TH_KNUCKLE") == m_tDesc.szTextElemKey)
		szWeaponIconElemKey = TEXT("GAUNTLET");

	if (nullptr != szWeaponIconElemKey)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::WEAPON, szWeaponIconElemKey);
		SetUp_Transform(m_fRenderPosBaseX, 385.f, 0.5f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::WEAPON));
		m_pModelCom->Render(1);
	}
	else
	{
		if ((_uint)ITEMCATEGORY::ENFORCECHIP == pPlayerInventory->Open_ItemSlot(m_tDesc.iItemCode)->iItemCategory)
		{
			m_tDesc.szIconElemKey = pPlayerInventory->Open_ItemSlot(m_tDesc.iItemCode)->szIconElemKey;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, m_tDesc.szIconElemKey);
		SetUp_Transform(m_fRenderPosBaseX, 385.f, 2.f, 2.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);
	}

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("SIMPLE_LINE"));
	SetUp_Transform(m_fRenderPosBaseX, 480.f, 1.f, 0.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_tDesc.szTextElemKey);
	SetUp_Transform(m_fRenderPosBaseX, 265.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_tDesc.szDetailElemKey);
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, 510.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_Detail::Activate(_double dTimeDelta)
{
	if (m_fActivateAlpha < m_fActivateAlphaMax)
	{
		m_fActivateAlpha += (_float)dTimeDelta * 128.f;
	}
	else
	{
		m_fActivateAlpha = m_fActivateAlphaMax;
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Core_Detail::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Detail::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Detail::Disable(_double dTimeDelta)
{
	m_fActivateAlpha = m_fActivateAlphaMin;

	return S_OK;
}

HRESULT CUI_Core_Detail::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Detail::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Detail::Render_Number(_uint iAmount, _uint iLineNumber)
{
	_uint	iBufferNumber = 0;
	_uint	iRenderNumber = 0;

	iBufferNumber = iAmount;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX = 0.f;
}

void CUI_Core_Detail::Render_HP(_uint iCrnt, _uint iMax, _uint iLineNumber)
{
	_uint	iBufferNumber = 0;
	_uint	iRenderNumber = 0;

	iBufferNumber = iMax;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX -= 10.f;

	////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("SLASH"));
	SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12);

	m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;

	////////////////////////////////////////////////////////////////////////////

	m_fRenderAmountLetterIntervalAccX -= 10.f;

	iRenderNumber = 0;
	iBufferNumber = iCrnt;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX = 0.f;
}

void CUI_Core_Detail::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ENABLE == pState)		{ m_eUIState = pState; }
			if (UISTATE::ACTIVATE == pState)	{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::PRESSED:
			break;
		case Client::CUI::UISTATE::RELEASED:
			break;
		case Client::CUI::UISTATE::NONE:
			break;
		default:
			break;
		}
	}
	else if (2 == iResult)
	{
		if (CUI::UISTATE::ENABLE == m_eUIState || CUI::UISTATE::ACTIVATE == m_eUIState)
		{
			PACKET*	pPacket = (PACKET*)pMessage;

			ITEMDESC tDesc = *(ITEMDESC*)pPacket->pData;

			memcpy(&m_tDesc, &tDesc, sizeof(ITEMDESC));
		}
	}
}

_int CUI_Core_Detail::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_DETAIL)
		{
			return 1;
		}
		if (*check == CHECKSUM_UI_CORE_DETAIL_UPDATE)
		{
			return 2;
		}
	}
	return FALSE;
}

