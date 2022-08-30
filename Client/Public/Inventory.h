#pragma once

#include "Client_Defines.h"
#include "Component.h"

BEGIN(Client)

class CInventory final : public CComponent
{
public:
	static const _uint	ITEMCOUNT_PER_CATEGORY = 10;

public:
	enum class SORTBY
	{
		NAME		= 0,
		COUNT		= 1,
		CATEGORY	= 2,
		ITEMCODE	= 3,
		NONE		= 4
	};

	enum class ITEMCLASS
	{
		ITEM		= 0,
		WEAPON		= 1,
		EQUIPMENT	= 2,
		ALL			= 3
	};

public:
	typedef std::vector<ITEMDESC>	INVENTORY;

private:
	CInventory(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CInventory(const CInventory& rhs);
	virtual ~CInventory() DEFAULT;

public:
	static	CInventory*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg) override;

private:
	void					Initialize_Inventory();
	void					Initialize_Money(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_Expendable(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_Resources(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_Specials(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_ShortSword(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_LongSword(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_Spear(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_Gauntlet(ITEMDESC& pDesc, _uint iIndex);
	void					Initialize_EnforceChip(ITEMDESC& pDesc, _uint iIndex);


public:
	ITEMDESC*				Open_ItemSlot(_uint iSlotIndex);
	const _uint&			Get_InventorySize() const { return m_iInventorySize; }
	void					Sort_Inventory(SORTBY eSortBy = SORTBY::ITEMCODE);
	void					Clear_Inventory();

	void					Set_InventoryItemCount();
	const _uint&			Get_InventoryItemCount() const { return m_iInventoryItemCount; }
	void					Set_InventoryWeaponCount();
	const _uint&			Get_InventoryWeaponCount() const { return m_iInventoryWeaponCount; }
	void					Set_InventoryEquipmentCount();
	const _uint&			Get_InventoryEquipmentCount() const { return m_iInventoryEquipmentCount; }

	const _uint&			Get_CurrentMoney() const { return m_vecItemDesc[0].iItemCount; }


private:
	INVENTORY	m_vecItemDesc;

	_uint		m_iInventorySize = 0;
	
	_uint		m_iInventoryItemCount = 0;
	_uint		m_iInventoryWeaponCount = 0;
	_uint		m_iInventoryEquipmentCount = 0;
};
END