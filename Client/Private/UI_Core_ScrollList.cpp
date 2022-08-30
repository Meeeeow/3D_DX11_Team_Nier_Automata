#include "stdafx.h"
#include "..\Public\UI_Core_ScrollList.h"
#include "UI_Core_ListLine.h"

const	_tchar*		CUI_Core_ScrollList::LINEOBSERVERKEY[] = {
	TEXT("OBSERVER_LISTLINE_0"),
	TEXT("OBSERVER_LISTLINE_1"),
	TEXT("OBSERVER_LISTLINE_2"),
	TEXT("OBSERVER_LISTLINE_3"),
	TEXT("OBSERVER_LISTLINE_4"),
	TEXT("OBSERVER_LISTLINE_5"),
	TEXT("OBSERVER_LISTLINE_6"),
	TEXT("OBSERVER_LISTLINE_7")
};

const	_uint		CUI_Core_ScrollList::LINEMAXCOUNT = sizeof(CUI_Core_ScrollList::LINEOBSERVERKEY) / sizeof(_tchar*);
const	_float		CUI_Core_ScrollList::LINEOFFSETY = 42.f;

CUI_Core_ScrollList::CUI_Core_ScrollList(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_ScrollList::CUI_Core_ScrollList(const CUI_Core_ScrollList & rhs)
	: CUI(rhs)
{
}

CUI_Core_ScrollList * CUI_Core_ScrollList::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_ScrollList* pInstance = new CUI_Core_ScrollList(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_ScrollList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_ScrollList::Clone(void * pArg)
{
	CUI_Core_ScrollList* pInstance = new CUI_Core_ScrollList(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_ScrollList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_ScrollList::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	if (nullptr != m_pPlayerInventory)
	{
		Safe_Release(m_pPlayerInventory);
	}

	for (auto& pObserver : m_dqLineObservers)
	{
		Safe_Release(pObserver);
	}
	m_dqLineObservers.clear();
}

HRESULT CUI_Core_ScrollList::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_ScrollList::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_ScrollList::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Background Default : 224, 223, 211, 128

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-145.f, -145.f, -145.f, 50.f));

	//m_vecColorOverlays.push_back(vColorOverlay);	// Head

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-20.f, -20.f, -20.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Body

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 2;
	m_bCollision = false;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_SCROLLLIST"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CUI_Core_ScrollList::SetUp_Components()
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

_int CUI_Core_ScrollList::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_ScrollList::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_ScrollList::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(220.f, 440.f, 1.1f, 1.21f);	// 352 * 387
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_ScrollList::Activate(_double dTimeDelta)
{
	if (m_fActivateAlpha < m_fActivateAlphaMax)
	{
		m_fActivateAlpha += (_float)dTimeDelta * 128.f;
	}
	else
	{
		m_fActivateAlpha = m_fActivateAlphaMax;
	}
	return S_OK;
}

HRESULT CUI_Core_ScrollList::Enable(_double dTimeDelta)
{
	m_dSelectedLineIndexIntervalAcc += dTimeDelta;

	if (m_dSelectedLineIndexIntervalMax < m_dSelectedLineIndexIntervalAcc)
	{
		m_dSelectedLineIndexIntervalAcc = 0.0;
	}

	Scroll_Set();

	if (0 < m_iScrollUpCount)
	{
		m_fScrollPosYOffset += (_float)dTimeDelta * 42.f;

		if (m_fScrollPosYOffsetMax < m_fScrollPosYOffset)
		{
			Scroll_Up();

			Refresh();
		}

		_float	iLineAdditionalPosOffsetY = 0.f;

		for (auto& pLineObservers : m_dqLineObservers)
		{
			_float	fResultLinePosY = m_fScrollPosYOffset + iLineAdditionalPosOffsetY;

			pLineObservers->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_POSITION, &fResultLinePosY));

			iLineAdditionalPosOffsetY += 1.f;
		}
	}

	if (0 < m_iScrollDownCount)
	{
		m_fScrollPosYOffset -= (_float)dTimeDelta * 42.f;

		if (m_fScrollPosYOffset < m_fScrollPosYOffsetMin)
		{
			Scroll_Down();

			Refresh();
		}

		_float	iLineAdditionalPosOffsetY = 0.f;

		for (auto& pLineObservers : m_dqLineObservers)
		{
			_float	fResultLinePosY = m_fScrollPosYOffset + iLineAdditionalPosOffsetY;

			pLineObservers->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_POSITION, &fResultLinePosY));

			iLineAdditionalPosOffsetY += 1.f;
		}
	}
	return S_OK;
}

HRESULT CUI_Core_ScrollList::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_ScrollList::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_ScrollList::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_ScrollList::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_ScrollList::Release_UI()
{
	if (false == m_dqLineObservers.empty())
	{
		CUI::UISTATE eLineState = CUI::UISTATE::DISABLE;

		for (auto& pObserver : m_dqLineObservers)
		{			
			pObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eLineState));
			Safe_Release(pObserver);
		}
		m_dqLineObservers.clear();
	}

	m_fActivateAlpha = m_fActivateAlphaMin;

	m_bIsScrollLock = false;

	m_iSelectedLineIndex = 0;

	//m_iSelectedSlotIndex = 0;	

	m_eCrntListLineItem = CInventory::ITEMCLASS::ALL;

	m_fScrollPosYOffset = 0.f;

	m_iScrollUpCount = 0;

	m_iScrollDownCount = 0;
}

void CUI_Core_ScrollList::Scroll_Set()
{
	POINT pt = {};

	GetCursorPos(&pt);

	ScreenToClient(g_hWnd, &pt);

	_float fCursorPosY = 0.f;

	if ((_float)pt.y < 295.f)
	{
		m_iSelectedLineIndex = m_iSelectedLineIndexMin;
		return;
	}
	if ((295.f + CUI_Core_ScrollList::LINEOFFSETY * m_iSelectedLineIndexMax) < (_float)pt.y)
	{
		m_iSelectedLineIndex = m_iSelectedLineIndexMax;
		return;
	}

	fCursorPosY = (((_float)pt.y - 295.f) / CUI_Core_ScrollList::LINEOFFSETY);

	if (0 <= (_uint)fCursorPosY || (_uint)fCursorPosY < (_uint)m_iSelectedLineIndexMax)
	{
		m_iSelectedLineIndex = (_uint)fCursorPosY;
	}
}

void CUI_Core_ScrollList::Scroll_Up()
{
	--m_iScrollUpCount;

	m_fScrollPosYOffset = 0.f;

	CObserver*	pObserverBack = *m_dqLineObservers.rbegin();

	m_dqLineObservers.pop_back();

	m_dqLineObservers.push_front(pObserverBack);

	_int		iValidSlotIndex = m_iSelectedSlotIndex;

	while (true)
	{
		--iValidSlotIndex;
		if (iValidSlotIndex < m_iSelectedSlotIndexMin)
			iValidSlotIndex = m_iSelectedSlotIndexMax;

		ITEMDESC*	pDesc = m_pPlayerInventory->Open_ItemSlot(iValidSlotIndex);

		switch (m_eCrntListLineItem)
		{
		case Client::CInventory::ITEMCLASS::ITEM:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::EXPENDABLE || (_uint)ITEMCATEGORY::SPECIAL < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::WEAPON:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::SHORTSWORD || (_uint)ITEMCATEGORY::GAUNTLET < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::EQUIPMENT:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::ENFORCECHIP)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::ALL:
			break;
		default:
			break;
		}

		if (pDesc->iItemCount <= 0 || !lstrcmp(pDesc->szTextElemKey, TEXT("")))
		{
			continue;
		}
		else
		{
			m_iSelectedSlotIndex = iValidSlotIndex;
			break;
		}
	}
}

void CUI_Core_ScrollList::Scroll_Down()
{
	--m_iScrollDownCount;

	m_fScrollPosYOffset = 0.f;

	CObserver*	pObserverFront = m_dqLineObservers.front();

	m_dqLineObservers.pop_front();

	m_dqLineObservers.push_back(pObserverFront);

	_int		iValidSlotIndex = m_iSelectedSlotIndex;

	while (true)
	{
		++iValidSlotIndex;
		if (m_iSelectedSlotIndexMax <= iValidSlotIndex)
			iValidSlotIndex = m_iSelectedSlotIndexMin;

		ITEMDESC*	pDesc = m_pPlayerInventory->Open_ItemSlot(iValidSlotIndex);

		switch (m_eCrntListLineItem)
		{
		case Client::CInventory::ITEMCLASS::ITEM:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::EXPENDABLE || (_uint)ITEMCATEGORY::SPECIAL < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::WEAPON:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::SHORTSWORD || (_uint)ITEMCATEGORY::GAUNTLET < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::EQUIPMENT:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::ENFORCECHIP)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::ALL:
			break;
		default:
			break;
		}

		if (pDesc->iItemCount <= 0 || !lstrcmp(pDesc->szTextElemKey, TEXT("")))
		{
			continue;
		}
		else
		{
			m_iSelectedSlotIndex = iValidSlotIndex;
			break;
		}
	}
}

void CUI_Core_ScrollList::SetUp_PlayerInventory()
{
	// SetUp Player's Inventory only once
	if (nullptr == m_pPlayerInventory)
	{
		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		m_pPlayerInventory = (CInventory*)(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0)->Get_Component(COM_KEY_INVENTORY));

		if (nullptr == m_pPlayerInventory)
		{
			MSGBOX("Failed to Access Player Inventory - ScrollList");
			return;
		}
		Safe_AddRef(m_pPlayerInventory);
	}
}

HRESULT CUI_Core_ScrollList::Reset_LineObservers()
{
	if (false == m_dqLineObservers.empty())
	{
		return S_OK;
	}

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	_int	iLineCount = (_int)CUI_Core_ScrollList::LINEMAXCOUNT;
	_int	iItemCount = 0;

	switch (m_eCrntListLineItem)
	{
	case Client::CInventory::ITEMCLASS::ITEM:
		m_pPlayerInventory->Set_InventoryItemCount();
		iItemCount = (_int)m_pPlayerInventory->Get_InventoryItemCount();
		break;
	case Client::CInventory::ITEMCLASS::WEAPON:
		m_pPlayerInventory->Set_InventoryWeaponCount();
		iItemCount = (_int)m_pPlayerInventory->Get_InventoryWeaponCount();
		break;
	case Client::CInventory::ITEMCLASS::EQUIPMENT:
		m_pPlayerInventory->Set_InventoryEquipmentCount();
		iItemCount = (_int)m_pPlayerInventory->Get_InventoryEquipmentCount();
		break;
	case Client::CInventory::ITEMCLASS::ALL:
		iItemCount = 0;
		break;
	default:
		break;
	}
	
	if (iItemCount <= (_int)CUI_Core_ScrollList::LINEMAXCOUNT)
	{
		m_bIsScrollLock = true;
		iLineCount = iItemCount;
		m_iSelectedLineIndexMax = iLineCount - 1;

		// Need to prevent underflow
		if (m_iSelectedLineIndexMax < 0)
		{
			m_iSelectedLineIndexMax = 0;
		}
	}
	else
	{
		m_bIsScrollLock = false;
		iLineCount = (_int)CUI_Core_ScrollList::LINEMAXCOUNT;
		m_iSelectedLineIndexMax = iLineCount - 1;		
	}

	CUI::UISTATE	eLineState = CUI::UISTATE::ENABLE;
	_float			fLineAdditionalPosOffsetY = 0.f;

	for (_int i = 0; i < iLineCount; ++i)
	{
		CObserver*		pLineObserver = nullptr;

		pLineObserver = pGameInstance->Get_Observer(LINEOBSERVERKEY[i]);

		if (nullptr == pLineObserver)
		{
			return E_FAIL;
		}
		else
		{
			pLineObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eLineState));
			pLineObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_POSITION, &fLineAdditionalPosOffsetY));
			m_dqLineObservers.push_back(pLineObserver);
			Safe_AddRef(pLineObserver);

			fLineAdditionalPosOffsetY += 1.f;
		}
	}
	return S_OK;
}

void CUI_Core_ScrollList::Refresh()
{	
	std::deque<CObserver*>::iterator	it = m_dqLineObservers.begin();
	std::deque<CObserver*>::iterator	ite = m_dqLineObservers.end();

	// Call Observer's Notify() with ITEMDESC expect Invalid Item Code

	_int	iStartSlotIndex = m_iSelectedSlotIndex;

	while (it != ite)
	{
		ITEMDESC*	pDesc = m_pPlayerInventory->Open_ItemSlot(iStartSlotIndex);

		++iStartSlotIndex;
		if (m_iSelectedSlotIndexMax <= iStartSlotIndex)
			iStartSlotIndex = m_iSelectedSlotIndexMin;

		switch (m_eCrntListLineItem)
		{
		case Client::CInventory::ITEMCLASS::ITEM:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::EXPENDABLE || (_uint)ITEMCATEGORY::SPECIAL < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::WEAPON:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::SHORTSWORD || (_uint)ITEMCATEGORY::GAUNTLET < pDesc->iItemCategory)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::EQUIPMENT:
			if (pDesc->iItemCategory < (_uint)ITEMCATEGORY::ENFORCECHIP)
				continue;
			break;
		case Client::CInventory::ITEMCLASS::ALL:
			break;
		default:
			break;
		}

		if (pDesc->iItemCount <= 0 || !lstrcmp(pDesc->szTextElemKey, TEXT("")))
		{
			continue;
		}
		else
		{
			(*it)->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_UPDATE, pDesc));
			++it;
		}
	}
	return;
}


void CUI_Core_ScrollList::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISCROLLSTATEDESC	tDesc = *(UISCROLLSTATEDESC*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::DISABLE == tDesc.eState)	{ m_eUIState = tDesc.eState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::DISABLE == tDesc.eState)	{ m_eUIState = tDesc.eState; }
			if (UISTATE::ACTIVATE == tDesc.eState)	{ m_eUIState = tDesc.eState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ENABLE == tDesc.eState)	{ m_eUIState = tDesc.eState; }
			if (UISTATE::ACTIVATE == tDesc.eState)	{ m_eUIState = tDesc.eState; }
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

		// If the request result is ACTIVATE, Initialize parameters, setup observers and provide data to the line
		if (UISTATE::ACTIVATE == m_eUIState)
		{
			Release_UI();

			m_eCrntListLineItem = tDesc.eClass;
			
			SetUp_PlayerInventory();

			Reset_LineObservers();

			Refresh();

			m_eUIState = CUI::UISTATE::ENABLE;
		}

		// Parameter initialization when the request result is DISABLE
		if (UISTATE::DISABLE == m_eUIState)
		{
			Release_UI();
		}
	}
	else if (2 == iResult)
	{
		if (0 == m_iSelectedLineIndexMax)
		{
			return;
		}

		if (UISTATE::ENABLE == m_eUIState)
		{
			PACKET*			pPacket = (PACKET*)pMessage;

			UISCROLLDESC	pDesc = *(UISCROLLDESC*)pPacket->pData;

			if (true == pDesc.bIsUp)
			{
				if (m_iSelectedLineIndexMin < m_iSelectedLineIndex)
				{
					if (m_dSelectedLineIndexIntervalAcc < m_dSelectedLineIndexIntervalMax)
					{
						--m_iSelectedLineIndex;
					}					
				}
				else if (false == m_bIsScrollLock)
				{
					++m_iScrollUpCount;
				}
			}
			else
			{
				if (m_iSelectedLineIndex < (_uint)m_iSelectedLineIndexMax)
				{
					if (m_dSelectedLineIndexIntervalAcc < m_dSelectedLineIndexIntervalMax)
					{
						++m_iSelectedLineIndex;
					}
				}
				else if (false == m_bIsScrollLock)
				{
					++m_iScrollDownCount;
				}
			}

			_float	yPos = 295.f + (CUI_Core_ScrollList::LINEOFFSETY * (_float)m_iSelectedLineIndex);

			POINT pt = { (long)210.f , (long)yPos };
					
			ClientToScreen(g_hWnd, &pt);

			SetCursorPos(pt.x, pt.y);
		}
	}
}

_int CUI_Core_ScrollList::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_SCROLLLIST)
		{
			return 1;
		}
		if (*check == CHECKSUM_UI_CORE_SCROLLLIST_CONTROL)
		{
			return 2;
		}
	}
	return FALSE;
}
