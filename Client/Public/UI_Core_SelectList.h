#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_SelectList final : public CUI
{
public:
	static const _tchar*	LINEOBSERVERKEY[];
	static const _uint		LINEMAXCOUNT;
	static const _float		LINEOFFSETY;
	static const _uint		COMMAND_INITIALIZE	= 0x00000000;
	static const _uint		COMMAND_OPEN		= 0x00000001;
	static const _uint		COMMAND_CLOSE		= 0x00000002;

public:
	typedef struct tagUISelectStateDesc
	{
		_uint			iCommandKey = CUI_Core_SelectList::COMMAND_INITIALIZE;
		ITEMDESC		tItemDesc;
		_float			fLinePosX = 0.f;
		_float			fLinePosY = 0.f;
	}UISELECTDESC;

private:
	CUI_Core_SelectList(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_SelectList(const CUI_Core_SelectList& rhs);
	virtual ~CUI_Core_SelectList() DEFAULT;

public:
	static CUI_Core_SelectList*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	void						Update_SelectList();

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	std::deque<CObserver*>	m_dqLineObservers;

	UISTATE					m_eNextState = CUI::UISTATE::NONE;
	UISELECTDESC			m_tSelectDesc;

	_uint					m_iCommandFlag = CUI_Core_SelectList::COMMAND_INITIALIZE;
	_float					m_fBasePosX = 450.f;
	_float					m_fBasePosY = 0.f;
};

END
