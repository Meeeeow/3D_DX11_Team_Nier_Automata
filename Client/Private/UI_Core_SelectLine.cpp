#include "stdafx.h"
#include "..\Public\UI_Core_SelectLine.h"
#include "UI_Core_Controller.h"
#include "UI_Core_ScrollList.h"
#include "Inventory.h"
#include "Player.h"
#include "SoundMgr.h"

CUI_Core_SelectLine::CUI_Core_SelectLine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_SelectLine::CUI_Core_SelectLine(const CUI_Core_SelectLine & rhs)
	: CUI(rhs)
{
}

CUI_Core_SelectLine * CUI_Core_SelectLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_SelectLine* pInstance = new CUI_Core_SelectLine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_SelectLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_SelectLine::Clone(void * pArg)
{
	CUI_Core_SelectLine* pInstance = new CUI_Core_SelectLine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_SelectLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_SelectLine::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_SelectLine::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_SelectLine::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_SelectLine::SetUp_Components()))
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
	m_iDistanceZ = 5;
	m_bCollision = true;

	if (nullptr != pArg)
	{
		m_iInitialLineNumber = *(_uint*)pArg;
	}

	const _tchar*	szObserverKey = nullptr;

	switch (m_iInitialLineNumber)
	{
	case 0:
		szObserverKey = TEXT("OBSERVER_SELECTLINE_0");
		break;
	case 1:
		szObserverKey = TEXT("OBSERVER_SELECTLINE_1");
		break;
	case 2:
		szObserverKey = TEXT("OBSERVER_SELECTLINE_2");
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

HRESULT CUI_Core_SelectLine::SetUp_Components()
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

_int CUI_Core_SelectLine::Tick(_double TimeDelta)
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

_int CUI_Core_SelectLine::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_SelectLine::Render()
{
	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		SetUp_Transform(m_tLineDesc.fPosX, m_tLineDesc.fPosY - m_fActivateLineOffsetY, 1.91f, 0.5f);	// 288
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("BUTTON_ACTIVATE"));
		SetUp_Transform(m_tLineDesc.fPosX, m_tLineDesc.fPosY + m_fActivateLineOffsetY, 1.91f, 0.5f);	// 288
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// 색상 보정 X
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(1);
	}

	////////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_tLineDesc.fPosX, m_tLineDesc.fPosY, 0.6f, 0.1f);	// 192 * 32
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimCursorX, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(2);	// Atlas_Button_Activate
	
	////////////////////////////////////////////////////////////////////////////////

	const _tchar*	szRollElemKey = nullptr;

	switch (m_tLineDesc.eRole)
	{
	case ROLE::USE:
		szRollElemKey = TEXT("ROLE_USE");
		break;
	case ROLE::DISPOSE:
		szRollElemKey = TEXT("ROLE_DISPOSE");
		break;
	case ROLE::EQUIP_MAIN:
		szRollElemKey = TEXT("ROLE_EQUIP_MAIN");
		break;
	case ROLE::EQUIP_SUB:
		szRollElemKey = TEXT("ROLE_EQUIP_SUB");
		break;
	case ROLE::RELEASE:
		szRollElemKey = TEXT("ROLE_RELEASE");
		break;
	case ROLE::INSTALL:
		szRollElemKey = TEXT("ROLE_INSTALL");
		break;
	case ROLE::UNINSTALL:
		szRollElemKey = TEXT("ROLE_UNINSTALL");
		break;
	default:
		szRollElemKey = nullptr;
		break;
	}

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, szRollElemKey);
	SetUp_Transform(m_tLineDesc.fPosX, m_tLineDesc.fPosY, 1.f, 1.f);
	SetUp_DefaultRawValue();

	if (m_eUIState == CUI::UISTATE::ENABLE)
	{
		if (true == m_bCollision)
			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		else
			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// Font Origin Color
	}
	if (m_eUIState == CUI::UISTATE::ACTIVATE || m_eUIState == CUI::UISTATE::INACTIVATE || m_eUIState == CUI::UISTATE::PRESSED)
	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));	// Font Origin Color
	}
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12); // Atlas_Color_Overlay_On_One

	// PtInRect Scope Setting
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_tLineDesc.fPosX, m_tLineDesc.fPosY, 0.6f, 0.1f);	// 192 * 32

	return S_OK;
}

HRESULT CUI_Core_SelectLine::Activate(_double dTimeDelta)
{
	if (m_fUVAnimCursorXMin == m_fUVAnimCursorX)
	{
		CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
		CSoundMgr::Get_Instance()->PlaySound(L"ActivateButton.mp3", CSoundMgr::CHANNELID::UI);
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

HRESULT CUI_Core_SelectLine::Enable(_double dTimeDelta)
{
	m_fEnableAnimAlpha = m_fEnableAnimAlphaMax;
	m_bCollision = true;

	if (ROLE::USE == m_tLineDesc.eRole)
	{
		if ((_uint)ITEMCATEGORY::RESOURCE == m_tLineDesc.tItemDesc.iItemCategory
			|| 0 == m_tLineDesc.tItemDesc.iItemCount)
		{
			m_bCollision = false;
		}
	}
	else if (ROLE::DISPOSE == m_tLineDesc.eRole)
	{
		if (0 == m_tLineDesc.tItemDesc.iItemCount)
		{
			m_bCollision = false;
		}
	}
	else if (ROLE::EQUIP_MAIN == m_tLineDesc.eRole)
	{
		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		CPlayer*		pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

		_uint iCrntEquipedItemCode = 0;

		switch ((CPlayer::EQUIPSTATE)pPlayer->Get_EquipStateMain())
		{
		case CPlayer::EQUIPSTATE::KATANA:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_WHITE_COVENANT;
			break;
		case CPlayer::EQUIPSTATE::LONGKATANA:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_WHITE_CONTRACT;
			break;
		case CPlayer::EQUIPSTATE::SPEAR:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_40TH_SPEAR;
			break;
		case CPlayer::EQUIPSTATE::GAUNTLET:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_40TH_KNUCKLE;
			break;
		default:
			break;
		}

		if (iCrntEquipedItemCode == m_tLineDesc.tItemDesc.iItemCode)
		{
			m_bCollision = false;
		}
	}
	else if (ROLE::EQUIP_SUB == m_tLineDesc.eRole)
	{
		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		CPlayer*		pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

		_uint iCrntEquipedItemCode = 0;

		switch ((CPlayer::EQUIPSTATE)pPlayer->Get_EquipStateSub())
		{
		case CPlayer::EQUIPSTATE::KATANA:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_WHITE_COVENANT;
			break;
		case CPlayer::EQUIPSTATE::LONGKATANA:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_WHITE_CONTRACT;
			break;
		case CPlayer::EQUIPSTATE::SPEAR:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_40TH_SPEAR;
			break;
		case CPlayer::EQUIPSTATE::GAUNTLET:
			iCrntEquipedItemCode = ITEMCODE_EQUIPMENT_40TH_KNUCKLE;
			break;
		default:
			break;
		}

		if (iCrntEquipedItemCode == m_tLineDesc.tItemDesc.iItemCode)
		{
			m_bCollision = false;
		}
	}
	else if (ROLE::INSTALL == m_tLineDesc.eRole)
	{
		if (TEXT("ICON_CHIP_ON") == m_tLineDesc.tItemDesc.szIconElemKey)
		{
			m_bCollision = false;
		}
		if (TEXT("SKILL_OS") == m_tLineDesc.tItemDesc.szTextElemKey)
		{
			m_bCollision = false;
		}
	}
	else if (ROLE::UNINSTALL == m_tLineDesc.eRole)
	{
		if (TEXT("ICON_CHIP_OFF") == m_tLineDesc.tItemDesc.szIconElemKey)
		{
			m_bCollision = false;
		}
		if (TEXT("SKILL_OS") == m_tLineDesc.tItemDesc.szTextElemKey)
		{
			m_bCollision = false;
		}
	}


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

HRESULT CUI_Core_SelectLine::Inactivate(_double dTimeDelta)
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

HRESULT CUI_Core_SelectLine::Disable(_double dTimeDelta)
{
	// Initialize Parameter for Anim

	m_fUVAnimCursorX = 0.f;

	m_fEnableAnimAlpha = m_fEnableAnimAlphaMin;

	m_fActivateLineOffsetY = m_fActivateLineOffsetYMin;

	//m_tLineDesc.fPrevLineOffsetScaleY = (_float)m_iInitialLineNumber;

	//m_fPressedAnimPosOffsetY = 0.f;
	//m_fPressedAnimTimeAcc = 0.f;
	//m_bPressed = false;

	return S_OK;
}

HRESULT CUI_Core_SelectLine::Pressed(_double dTimeDelta)
{
	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
	CSoundMgr::Get_Instance()->PlaySound(L"PressButton.mp3", CSoundMgr::CHANNELID::UI);

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CInventory*		pPlayerInventory = nullptr;

	pPlayerInventory = (CInventory*)(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Get_Component(COM_KEY_INVENTORY));

	if (nullptr == pPlayerInventory)
	{
		MSGBOX("Failed to Access Player Inventory - ScrollList");
		return E_FAIL;
	}

	ITEMDESC*		pPlayerItemSlotDesc = pPlayerInventory->Open_ItemSlot(m_tLineDesc.tItemDesc.iItemCode);
	CPlayer*		pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);
	OBJSTATUSDESC	tStatusDesc = pPlayer->Get_ObjectStatusDesc();
	_uint			iItemCount = 0;

	switch (m_tLineDesc.eRole)
	{
	case CUI_Core_SelectLine::ROLE::USE:
		iItemCount = pPlayerItemSlotDesc->iItemCount;
		if (0 < iItemCount)
		{
			pPlayerItemSlotDesc->iItemCount -= 1;
			switch (pPlayerItemSlotDesc->iItemCode)
			{
			case 10:
				tStatusDesc.fHp += tStatusDesc.fMaxHp * 0.25f;
				if (tStatusDesc.fMaxHp < tStatusDesc.fHp)
					tStatusDesc.fHp = tStatusDesc.fMaxHp;
				pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Set_ObjectStatusDesc(tStatusDesc);
				break;
			case 13:
				tStatusDesc.fHp += tStatusDesc.fMaxHp * 0.5f;
				if (tStatusDesc.fMaxHp < tStatusDesc.fHp)
					tStatusDesc.fHp = tStatusDesc.fMaxHp;
				pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Set_ObjectStatusDesc(tStatusDesc);
				break;
			case 15:
				tStatusDesc.fHp += tStatusDesc.fMaxHp * 0.75f;
				if (tStatusDesc.fMaxHp < tStatusDesc.fHp)
					tStatusDesc.fHp = tStatusDesc.fMaxHp;
				pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Set_ObjectStatusDesc(tStatusDesc);
				break;
			case 17:
				tStatusDesc.fHp = tStatusDesc.fMaxHp;
				pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Set_ObjectStatusDesc(tStatusDesc);
				break;
			default:
				break;
			}
		}
		break;
	case CUI_Core_SelectLine::ROLE::DISPOSE:
		iItemCount = pPlayerItemSlotDesc->iItemCount;
		if (0 < iItemCount)
		{
			pPlayerItemSlotDesc->iItemCount -= 1;
		}
		break;
	case CUI_Core_SelectLine::ROLE::EQUIP_MAIN:
		switch (pPlayerItemSlotDesc->iItemCode)
		{
		case 49:
			pPlayer->Set_EquipWeaponState(1, CPlayer::EQUIPSTATE::KATANA);
			break;
		case 59:
			pPlayer->Set_EquipWeaponState(1, CPlayer::EQUIPSTATE::LONGKATANA);
			break;
		case 69:
			pPlayer->Set_EquipWeaponState(1, CPlayer::EQUIPSTATE::SPEAR);
			break;
		case 79:
			pPlayer->Set_EquipWeaponState(1, CPlayer::EQUIPSTATE::GAUNTLET);
			break;
		}		
		break;
	case CUI_Core_SelectLine::ROLE::EQUIP_SUB:
		switch (pPlayerItemSlotDesc->iItemCode)
		{
		case 49:
			pPlayer->Set_EquipWeaponState(2, CPlayer::EQUIPSTATE::KATANA);
			break;
		case 59:
			pPlayer->Set_EquipWeaponState(2, CPlayer::EQUIPSTATE::LONGKATANA);
			break;
		case 69:
			pPlayer->Set_EquipWeaponState(2, CPlayer::EQUIPSTATE::SPEAR);
			break;
		case 79:
			pPlayer->Set_EquipWeaponState(2, CPlayer::EQUIPSTATE::GAUNTLET);
			break;
		}
		break;
	case CUI_Core_SelectLine::ROLE::RELEASE:
		break;
	case CUI_Core_SelectLine::ROLE::INSTALL:
		switch (pPlayerItemSlotDesc->iItemCode)
		{
		case 80:
			pPlayer->Install_ProgramChip(CPlayer::PROGRAM_OS);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_ON");
			break;
		case 81:
			pPlayer->Install_ProgramChip(CPlayer::PROGRAM_MACHINEGUN);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_ON");
			break;
		case 82:
			pPlayer->Install_ProgramChip(CPlayer::PROGRAM_HANG);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_ON");
			break;
		case 83:
			pPlayer->Install_ProgramChip(CPlayer::PROGRAM_LASER);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_ON");
			break;
		case 84:
			pPlayer->Install_ProgramChip(CPlayer::PROGRAM_LIGHT);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_ON");
			break;
		default:
			break;
		}
		break;
	case CUI_Core_SelectLine::ROLE::UNINSTALL:
		switch (pPlayerItemSlotDesc->iItemCode)
		{
		case 80:
			pPlayer->Uninstall_ProgramChip(CPlayer::PROGRAM_OS);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_OFF");
			break;
		case 81:
			pPlayer->Uninstall_ProgramChip(CPlayer::PROGRAM_MACHINEGUN);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_OFF");
			break;
		case 82:
			pPlayer->Uninstall_ProgramChip(CPlayer::PROGRAM_HANG);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_OFF");
			break;
		case 83:
			pPlayer->Uninstall_ProgramChip(CPlayer::PROGRAM_LASER);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_OFF");
			break;
		case 84:
			pPlayer->Uninstall_ProgramChip(CPlayer::PROGRAM_LIGHT);
			pPlayerItemSlotDesc->szIconElemKey = TEXT("ICON_CHIP_OFF");
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	m_eUIState = CUI::UISTATE::DISABLE;

	return S_OK;
}

HRESULT CUI_Core_SelectLine::Released(_double dTimeDelta)
{
	//CUI_Core_Controller::COMMANDDESC	tDesc;

	//tDesc.iCommandFlag = COMMAND_CORE_LINE_UNLOCK;

	//CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));

	//RELEASE_INSTANCE(CGameInstance);

	//m_eUIState = CUI::UISTATE::INACTIVATE;

	return S_OK;
}

void CUI_Core_SelectLine::Notify(void * pMessage)
{
	_int iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

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
			//if (UISTATE::ENABLE == pState) {
			//	m_fEnableAnimAlpha = m_fEnableAnimAlphaMin;
			//	m_eUIState = pState;
			//}
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
		if (UISTATE::DISABLE != m_eUIState)
		{
			PACKET*		pPacket = (PACKET*)pMessage;

			UISELECTLINEDESC*	pLineDesc = (UISELECTLINEDESC*)pPacket->pData;

			memcpy(&m_tLineDesc, pLineDesc, sizeof(UISELECTLINEDESC));

			return;
		}
	}
}

_int CUI_Core_SelectLine::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_SELECTLINE_STATE)
		{
			return 1;
		}
		else if (*check == CHECKSUM_UI_CORE_SELECTLINE_UPDATE)
		{
			return 2;
		}
	}
	return FALSE;
}
