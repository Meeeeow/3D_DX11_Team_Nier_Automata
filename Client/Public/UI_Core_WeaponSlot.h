#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_WeaponSlot final : public CUI
{
public:
	typedef struct tagUIWeaponSlotDesc
	{
		_uint		iMainWeapon = 0;
		_uint		iSubWeapon = 0;
	}WEAPONSLOTDESC;

private:
	CUI_Core_WeaponSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_WeaponSlot(const CUI_Core_WeaponSlot& rhs);
	virtual ~CUI_Core_WeaponSlot() DEFAULT;

public:
	static CUI_Core_WeaponSlot*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	virtual void				Release_UI() override {}
	void						SetUp_Parameter();

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	// For Render
	WEAPONSLOTDESC	m_tDesc;

	const _tchar*	m_szMainTextElemKey = nullptr;
	const _tchar*	m_szMainIconElemKey = nullptr;
	const _tchar*	m_szSubTextElemKey = nullptr;
	const _tchar*	m_szSubIconElemKey = nullptr;

	_float	m_fActivateAlpha = -128.f;
	_float	m_fActivateAlphaMin = -128.f;
	_float	m_fActivateAlphaMax = 0.f;

	// For Render
	const _float	m_fHeaderUVRangeY = 0.15f;
	const _float	m_fRenderMainPosBaseX = 450.f;
	const _float	m_fRenderMainPosBaseY = 335.f;
	const _float	m_fRenderSubPosBaseX = 450.f;
	const _float	m_fRenderSubPosBaseY = 545.f;

	//const _float	m_fRenderBaseLinePosY = 445.f;

	const _float	m_fRenderTitleOffsetX = 10.f;
	const _float	m_fRenderTitleOffsetY = -70.f;
	const _float	m_fRenderIconOffsetX = 280.f;
	const _float	m_fRenderIconOffsetY = 20.f;
	const _float	m_fRenderTextOffsetX = 555.f;
	const _float	m_fRenderTextOffsetY = 55.f;
};

END