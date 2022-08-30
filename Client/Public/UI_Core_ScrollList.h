#pragma once
#include "UI.h"
#include "Inventory.h"

BEGIN(Client)

class CUI_Core_ScrollList final : public CUI
{
public:
	static const _tchar*	LINEOBSERVERKEY[];
	static const _uint		LINEMAXCOUNT;
	static const _float		LINEOFFSETY;

public:
	typedef struct tagUIScrollDesc
	{
		_bool		bIsUp = true;
	}UISCROLLDESC;

	typedef struct tagUIScrollStateDesc
	{
		CUI::UISTATE			eState = UISTATE::DISABLE;
		CInventory::ITEMCLASS	eClass = CInventory::ITEMCLASS::ALL;
	}UISCROLLSTATEDESC;

private:
	CUI_Core_ScrollList(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_ScrollList(const CUI_Core_ScrollList& rhs);
	virtual ~CUI_Core_ScrollList() DEFAULT;

public:
	static CUI_Core_ScrollList*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg)				override;
	virtual void				Free()							override;
	virtual HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;
	
private:
	virtual HRESULT				SetUp_Components()				override;

public:
	virtual _int				Tick(_double TimeDelta)			override;
	virtual _int				LateTick(_double TimeDelta)		override;
	virtual HRESULT				Render()						override;

private:
	virtual HRESULT				Activate(_double dTimeDelta)	override;
	virtual HRESULT				Enable(_double dTimeDelta)		override;
	virtual HRESULT				Inactivate(_double dTimeDelta)	override;
	virtual HRESULT				Disable(_double dTimeDelta)		override;
	virtual HRESULT				Pressed(_double dTimeDelta)		override;
	virtual HRESULT				Released(_double dTimeDelta)	override;

	virtual void				Release_UI()					override;

	// For Notify(1)
	void						SetUp_PlayerInventory();
	HRESULT						Reset_LineObservers();
	void						Refresh();

	// For Notify(2)
	void						Scroll_Set();
	void						Scroll_Up();
	void						Scroll_Down();

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	std::deque<CObserver*>	m_dqLineObservers;

	CInventory*				m_pPlayerInventory = nullptr;
	CInventory::ITEMCLASS	m_eCrntListLineItem = CInventory::ITEMCLASS::ALL;

	_bool					m_bIsScrollLock = false;

	_uint					m_iSelectedLineIndex = 0;
	const _uint				m_iSelectedLineIndexMin = 0;
	_int					m_iSelectedLineIndexMax = 0;

	_double					m_dSelectedLineIndexIntervalAcc = 0.0;
	const _double			m_dSelectedLineIndexIntervalMax = 0.1;

	_int					m_iSelectedSlotIndex = 0;
	const _int				m_iSelectedSlotIndexMin = 0;
	const _int				m_iSelectedSlotIndexMax = (_uint)ITEMCATEGORY::COUNT * CInventory::ITEMCOUNT_PER_CATEGORY - 1;

	_float					m_fScrollPosYOffset = 0.f;
	const _float			m_fScrollPosYOffsetMin = -1.f;
	const _float			m_fScrollPosYOffsetMax = 1.f;

	_uint					m_iScrollUpCount = 0;
	_uint					m_iScrollDownCount = 0;

	// For Enable
	_float					m_fActivateAlpha = -128.f;
	const _float			m_fActivateAlphaMin = -128.f;
	const _float			m_fActivateAlphaMax = 50.f;
};

END
