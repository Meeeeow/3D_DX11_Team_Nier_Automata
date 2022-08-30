#include "stdafx.h"
#include "..\Public\Inventory.h"

CInventory::CInventory(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{
}

CInventory::CInventory(const CInventory& rhs)
	: CComponent(rhs)
	, m_vecItemDesc(rhs.m_vecItemDesc)
	, m_iInventorySize(rhs.m_iInventorySize)
	, m_iInventoryItemCount(rhs.m_iInventoryItemCount)
{
}

CInventory * CInventory::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CInventory* pInstance = new CInventory(pGraphicDevice, pContextDevice);
	
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CInventory::Clone(void * pArg)
{
	CInventory* pInstance = new CInventory(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory::Free()
{
	__super::Free();
}

HRESULT CInventory::NativeConstruct_Prototype()
{
	Initialize_Inventory();

	Set_InventoryItemCount();

	return S_OK;
}

HRESULT CInventory::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CInventory::Initialize_Inventory()
{
	m_iInventorySize = (_uint)ITEMCATEGORY::COUNT * CInventory::ITEMCOUNT_PER_CATEGORY;

	m_vecItemDesc.reserve((_uint)m_iInventorySize);

	for (_uint i = 0; i < m_iInventorySize; ++i)
	{
		ITEMDESC	tDesc = {};

		tDesc.iItemCategory = i / 10;
		tDesc.iItemCode = i;
		tDesc.iItemCount = 0;

		switch (tDesc.iItemCategory)
		{
		case 0:
			Initialize_Money(tDesc, i % 10);
			break;
		case 1:
			Initialize_Expendable(tDesc, i % 10);
			break;
		case 2:
			Initialize_Resources(tDesc, i % 10);
			break;
		case 3:
			Initialize_Specials(tDesc, i % 10);
			break;
		case 4:
			Initialize_ShortSword(tDesc, i % 10);
			break;
		case 5:
			Initialize_LongSword(tDesc, i % 10);
			break;
		case 6:
			Initialize_Spear(tDesc, i % 10);
			break;
		case 7:
			Initialize_Gauntlet(tDesc, i % 10);
			break;
		case 8:
			Initialize_EnforceChip(tDesc, i % 10);
			break;
		default:
			break;
		}
		m_vecItemDesc.push_back(tDesc);
	}
}

void CInventory::Initialize_Money(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("");

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("GOLD");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		pDesc.iItemCount = 99999999;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_Expendable(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_EXPENDABLES");

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("POTION_SMALL");
		pDesc.szDetailElemKey = TEXT("POTION_SMALL_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		pDesc.iItemCount = 3;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("POTION_MIDDLE");
		pDesc.szDetailElemKey = TEXT("POTION_MIDDLE_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		pDesc.iItemCount = 1;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("POTION_LARGE");
		pDesc.szDetailElemKey = TEXT("POTION_LARGE_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("POTION_EXLARGE");
		pDesc.szDetailElemKey = TEXT("POTION_EXLARGE_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_Resources(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_RESOURCES");

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("CHUNK");
		pDesc.szDetailElemKey = TEXT("CHUNK_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("COPPER");
		pDesc.szDetailElemKey = TEXT("COPPER_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("STEEL");
		pDesc.szDetailElemKey = TEXT("STEEL_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("AMBER");
		pDesc.szDetailElemKey = TEXT("AMBER_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("SILVER");
		pDesc.szDetailElemKey = TEXT("SILVER_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("MOLDABYTE");
		pDesc.szDetailElemKey = TEXT("MOLDABYTE_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("METEORITE");
		pDesc.szDetailElemKey = TEXT("METEORITE_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("PEARL");
		pDesc.szDetailElemKey = TEXT("PEARL_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("TITAN");
		pDesc.szDetailElemKey = TEXT("TITAN_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("SM_ALLOY");
		pDesc.szDetailElemKey = TEXT("SM_ALLOY_DETAIL");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_Specials(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_SPECIAL");

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_ShortSword(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_SHORTSWORD");
	pDesc.iItemCount = 1;

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("WHITE_COVENANT");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_LongSword(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_LONGSWORD");
	pDesc.iItemCount = 1;

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("WHITE_CONTRACT");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_Spear(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_SPEAR");
	pDesc.iItemCount = 0;

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("40TH_SPEAR");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_Gauntlet(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_GAUNTLET");
	pDesc.iItemCount = 0;

	switch (iIndex)
	{
	case 0:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 1:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 2:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		break;
	case 3:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 4:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		break;
	case 5:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 6:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		break;
	case 7:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 8:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("40TH_KNUCKLE");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

void CInventory::Initialize_EnforceChip(ITEMDESC & pDesc, _uint iIndex)
{
	pDesc.szIconElemKey = TEXT("ICON_CHIP_OFF");
	pDesc.iItemCount = 0;

	switch (iIndex)
	{
	case 0:
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		pDesc.szTextElemKey = TEXT("SKILL_OS");		
		pDesc.szIconElemKey = TEXT("ICON_CHIP_ON");
		pDesc.szDetailElemKey = TEXT("SKILL_OS_DETAIL");
		pDesc.iItemCount = 1;
		break;
	case 1:
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		pDesc.szTextElemKey = TEXT("SKILL_MACHINEGUN");
		pDesc.szIconElemKey = TEXT("ICON_CHIP_ON");
		pDesc.szDetailElemKey = TEXT("SKILL_MACHINEGUN_DETAIL");
		pDesc.iItemCount = 1;
		break;
	case 2:
		pDesc.iItemRarity = (_uint)ITEMRARITY::NORMAL;
		pDesc.szTextElemKey = TEXT("SKILL_HANG");
		pDesc.szIconElemKey = TEXT("ICON_CHIP_ON");
		pDesc.szDetailElemKey = TEXT("SKILL_HANG_DETAIL");
		pDesc.iItemCount = 1;
		break;
	case 3:
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		pDesc.szTextElemKey = TEXT("SKILL_LASER");
		pDesc.szIconElemKey = TEXT("ICON_CHIP_OFF");
		pDesc.szDetailElemKey = TEXT("SKILL_LASER_DETAIL");
		pDesc.iItemCount = 0;
		break;
	case 4:
		pDesc.iItemRarity = (_uint)ITEMRARITY::RARE;
		pDesc.szTextElemKey = TEXT("SKILL_LIGHT");
		pDesc.szIconElemKey = TEXT("ICON_CHIP_OFF");
		pDesc.szDetailElemKey = TEXT("SKILL_LIGHT_DETAIL");
		pDesc.iItemCount = 0;
		break;
	case 5:
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		pDesc.szTextElemKey = TEXT("MASTERY_SHORTSWORD");
		break;
	case 6:
		pDesc.iItemRarity = (_uint)ITEMRARITY::UNIQUE;
		pDesc.szTextElemKey = TEXT("MASTERY_LONGSWORD");
		break;
	case 7:
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		pDesc.szTextElemKey = TEXT("MASTERY_SPEAR");
		break;
	case 8:
		pDesc.iItemRarity = (_uint)ITEMRARITY::LEGENDARY;
		pDesc.szTextElemKey = TEXT("MASTERY_GAUNTLET");
		break;
	case 9:
		pDesc.szTextElemKey = TEXT("");
		pDesc.iItemRarity = (_uint)ITEMRARITY::EPIC;
		break;
	default:
		break;
	}
	return;
}

ITEMDESC * CInventory::Open_ItemSlot(_uint iSlotIndex)
{
	if (m_iInventorySize < iSlotIndex)
	{
		return nullptr;
	}
	return &m_vecItemDesc[iSlotIndex];
}

void CInventory::Sort_Inventory(SORTBY eSortBy)
{
}

void CInventory::Clear_Inventory()
{
	for (auto& pDesc : m_vecItemDesc)
	{
		pDesc.iItemCount = 0;
	}
}

void CInventory::Set_InventoryItemCount()
{
	_uint iResult = 0;

	for (auto& pDesc : m_vecItemDesc)
	{
		if (pDesc.iItemCategory == (_uint)ITEMCATEGORY::EXPENDABLE
			|| pDesc.iItemCategory == (_uint)ITEMCATEGORY::RESOURCE
			|| pDesc.iItemCategory == (_uint)ITEMCATEGORY::SPECIAL)
		{
			if (0 < pDesc.iItemCount && lstrcmp(pDesc.szTextElemKey, TEXT("")))
			{
				++iResult;
			}
		}
	}
	m_iInventoryItemCount = iResult;
}

void CInventory::Set_InventoryWeaponCount()
{
	_uint iResult = 0;

	for (auto& pDesc : m_vecItemDesc)
	{
		if (pDesc.iItemCategory == (_uint)ITEMCATEGORY::SHORTSWORD
			|| pDesc.iItemCategory == (_uint)ITEMCATEGORY::LONGSWORD
			|| pDesc.iItemCategory == (_uint)ITEMCATEGORY::SPEAR
			|| pDesc.iItemCategory == (_uint)ITEMCATEGORY::GAUNTLET)
		{
			if (0 < pDesc.iItemCount && lstrcmp(pDesc.szTextElemKey, TEXT("")))
			{
				++iResult;
			}
		}
	}
	m_iInventoryWeaponCount = iResult;
}

void CInventory::Set_InventoryEquipmentCount()
{
	_uint iResult = 0;

	for (auto& pDesc : m_vecItemDesc)
	{
		if (pDesc.iItemCategory == (_uint)ITEMCATEGORY::ENFORCECHIP)
		{
			if (0 < pDesc.iItemCount && lstrcmp(pDesc.szTextElemKey, TEXT("")))
			{
				++iResult;
			}
		}
	}
	m_iInventoryEquipmentCount = iResult;
}
