#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_Detail final : public CUI
{
private:
	CUI_Core_Detail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_Detail(const CUI_Core_Detail& rhs);
	virtual ~CUI_Core_Detail() DEFAULT;

public:
	static CUI_Core_Detail*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	void						Render_Number(_uint iAmount, _uint iLineNumber);
	void						Render_HP(_uint iCrnt, _uint iMax, _uint iLineNumber);

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	// For Render
	ITEMDESC		m_tDesc;

	_float	m_fActivateAlpha = -128.f;
	_float	m_fActivateAlphaMin = -128.f;
	_float	m_fActivateAlphaMax = 0.f;

	// For Render
	const _float	m_fHeaderUVRangeY = 0.07f;
	const _float	m_fRenderPosBaseX = 640.f;
	const _float	m_fRenderPosBaseY = 275.f;
	const _float	m_fRenderIntervalPosOffsetY = 40.f;
	const _float	m_fRenderTextPosOffsetX = 155.f;
	const _float	m_fRenderAmountPosOffsetX = 155.f;
	
	_float			m_fRenderAmountLetterIntervalAccX = 0.f;
	const _float	m_fRenderAmountLetterIntervalX = 14.f;	
};

END