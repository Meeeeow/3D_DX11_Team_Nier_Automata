#include "stdafx.h"
#include "..\Public\UI_Core_WeaponSlot.h"
#include "Player.h"

CUI_Core_WeaponSlot::CUI_Core_WeaponSlot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_WeaponSlot::CUI_Core_WeaponSlot(const CUI_Core_WeaponSlot & rhs)
	: CUI(rhs)
{
}

CUI_Core_WeaponSlot * CUI_Core_WeaponSlot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_WeaponSlot* pInstance = new CUI_Core_WeaponSlot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_WeaponSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_WeaponSlot::Clone(void * pArg)
{
	CUI_Core_WeaponSlot* pInstance = new CUI_Core_WeaponSlot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_WeaponSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_WeaponSlot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_WeaponSlot::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_WeaponSlot::SetUp_Components()))
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
	
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::SetUp_Components()
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

_int CUI_Core_WeaponSlot::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_WeaponSlot::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_WeaponSlot::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform_AlignLeft(m_fRenderMainPosBaseX, m_fRenderMainPosBaseY, 1.8f, 0.55f);	// 576 * 160
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimY", (void*)&m_fHeaderUVRangeY, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Head
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));	// Body
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(13);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::WEAPON, m_szMainIconElemKey);
	SetUp_Transform(m_fRenderMainPosBaseX + m_fRenderIconOffsetX, m_fRenderMainPosBaseY + m_fRenderIconOffsetY, 0.9f, 0.9f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::WEAPON));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szMainTextElemKey);
	SetUp_Transform_AlignRight(m_fRenderMainPosBaseX + m_fRenderTextOffsetX, m_fRenderMainPosBaseY + m_fRenderTextOffsetY, 1.2f, 1.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("MAIN_WEAPON"));
	SetUp_Transform_AlignLeft(m_fRenderMainPosBaseX + m_fRenderTitleOffsetX, m_fRenderMainPosBaseY + m_fRenderTitleOffsetY, 1.15f, 1.15f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("SIMPLE_LINE"));
	//SetUp_Transform_AlignLeft(m_fRenderMainPosBaseX, 435.f, 1.8f, 0.2f);
	//SetUp_DefaultRawValue();

	//m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
	//m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform_AlignLeft(m_fRenderSubPosBaseX, m_fRenderSubPosBaseY, 1.8f, 0.55f);	// 576 * 160
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimY", (void*)&m_fHeaderUVRangeY, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Head
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));	// Body
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(13);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::WEAPON, m_szSubIconElemKey);
	SetUp_Transform(m_fRenderSubPosBaseX + m_fRenderIconOffsetX, m_fRenderSubPosBaseY + m_fRenderIconOffsetY, 0.9f, 0.9f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::WEAPON));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szSubTextElemKey);
	SetUp_Transform_AlignRight(m_fRenderSubPosBaseX + m_fRenderTextOffsetX, m_fRenderSubPosBaseY + m_fRenderTextOffsetY, 1.2f, 1.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("SUB_WEAPON"));
	SetUp_Transform_AlignLeft(m_fRenderSubPosBaseX + m_fRenderTitleOffsetX, m_fRenderSubPosBaseY + m_fRenderTitleOffsetY, 1.15f, 1.15f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::Activate(_double dTimeDelta)
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

HRESULT CUI_Core_WeaponSlot::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::Disable(_double dTimeDelta)
{
	m_fActivateAlpha = m_fActivateAlphaMin;

	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_WeaponSlot::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_WeaponSlot::SetUp_Parameter()
{
	CPlayer::EQUIPSTATE	eMainWeapon = CPlayer::EQUIPSTATE::NONE;
	CPlayer::EQUIPSTATE	eSubWeapon = CPlayer::EQUIPSTATE::NONE;

	eMainWeapon = (CPlayer::EQUIPSTATE)m_tDesc.iMainWeapon;
	eSubWeapon = (CPlayer::EQUIPSTATE)m_tDesc.iSubWeapon;

	switch (eMainWeapon)
	{
	case Client::CPlayer::EQUIPSTATE::KATANA:
		m_szMainTextElemKey = TEXT("WHITE_COVENANT");
		m_szMainIconElemKey = TEXT("KATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::LONGKATANA:
		m_szMainTextElemKey = TEXT("WHITE_CONTRACT");
		m_szMainIconElemKey = TEXT("LONGKATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::SPEAR:
		m_szMainTextElemKey = TEXT("40TH_SPEAR");
		m_szMainIconElemKey = TEXT("SPEAR");
		break;
	case Client::CPlayer::EQUIPSTATE::GAUNTLET:
		m_szMainTextElemKey = TEXT("40TH_KNUCKLE");
		m_szMainIconElemKey = TEXT("GAUNTLET");
		break;
	case Client::CPlayer::EQUIPSTATE::NONE:
		m_szMainTextElemKey = nullptr;
		m_szMainIconElemKey = nullptr;
		break;
	default:
		m_szMainTextElemKey = nullptr;
		m_szMainIconElemKey = nullptr;
		break;
	}

	switch (eSubWeapon)
	{
	case Client::CPlayer::EQUIPSTATE::KATANA:
		m_szSubTextElemKey = TEXT("WHITE_COVENANT");
		m_szSubIconElemKey = TEXT("KATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::LONGKATANA:
		m_szSubTextElemKey = TEXT("WHITE_CONTRACT");
		m_szSubIconElemKey = TEXT("LONGKATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::SPEAR:
		m_szSubTextElemKey = TEXT("40TH_SPEAR");
		m_szSubIconElemKey = TEXT("SPEAR");
		break;
	case Client::CPlayer::EQUIPSTATE::GAUNTLET:
		m_szSubTextElemKey = TEXT("40TH_KNUCKLE");
		m_szSubIconElemKey = TEXT("GAUNTLET");
		break;
	case Client::CPlayer::EQUIPSTATE::NONE:
		m_szSubTextElemKey = nullptr;
		m_szSubIconElemKey = nullptr;
		break;
	default:
		m_szSubTextElemKey = nullptr;
		m_szSubIconElemKey = nullptr;
		break;
	}
}

void CUI_Core_WeaponSlot::Notify(void * pMessage)
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
		PACKET*	pPacket = (PACKET*)pMessage;

		WEAPONSLOTDESC*	pDesc = (WEAPONSLOTDESC*)pPacket->pData;

		memcpy(&m_tDesc, pDesc, sizeof(WEAPONSLOTDESC));

		SetUp_Parameter();
	}
}

_int CUI_Core_WeaponSlot::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_WEAPONSLOT_STATE)
		{
			return 1;
		}
		if (*check == CHECKSUM_UI_CORE_WEAPONSLOT_UPDATE)
		{
			return 2;
		}
	}
	return FALSE;
}

