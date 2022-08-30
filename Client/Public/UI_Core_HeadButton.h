#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_HeadButton final : public CUI
{
public:
	enum class BUTTONTYPE : _uint
	{
		MAP		= 0,
		QUEST	= 1,
		ITEM	= 2,
		WEAPON	= 3,
		SKILL	= 4,
		DATA	= 5,
		SYSTEM	= 6,
		COUNT	= 7
	};

	typedef struct UIHeadButtonDesc
	{
		BUTTONTYPE		eType = BUTTONTYPE::COUNT;
		const _tchar*	szIconElemKey;
		const _tchar*	szTextElemKey;
	}UIHEADBUTTONDESC;

private:
	CUI_Core_HeadButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_HeadButton(const CUI_Core_HeadButton& rhs);
	virtual ~CUI_Core_HeadButton() DEFAULT;

public:
	static CUI_Core_HeadButton*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	virtual void				Parse_Description() {}
	virtual void				Release_UI() override {}

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	// General Parameter
	UIHEADBUTTONDESC	m_tDesc;

	// For Render
	const _float	m_fRenderPosBaseX = 100.f;
	const _float	m_fRenderPosBaseY = 40.f;
	const _float	m_fRenderIntervalPosOffsetX = 180.f;
	const _float	m_fRenderIconPosOffsetX = 45.f;
	const _float	m_fRenderTextPosOffsetX = 15.f;

	// For Enable
	_float			m_fEnableAnimAlpha = -128.f;
	const _float	m_fEnableAnimAlphaSpeed = 178.f;
	const _float	m_fEnableAnimAlphaMin = -128.f;	// Deco::Background 원본 텍스처의 Alpha값
	const _float	m_fEnableAnimAlphaMax = 50.f;	// Deco::Background 렌더 시 최종 목표 Alpha값

	// For Activate
	_float			m_fUVAnimCursorX = 0.f;
	const _float	m_fUVAnimCursorXMin = 0.f;
	const _float	m_fUVAnimCursorXMax = 1.f;

	_float			m_fActivateLineOffsetY = 24.f;
	const _float	m_fActivateLineOffsetYMin = 24.f;
	const _float	m_fActivateLineOffsetYMax = 30.f;
	const _float	m_fActivateLineOffsetYSpeed = 60.f;

	// For Pressed
	_bool			m_bPressed = false;
	_float			m_fPressedAnimTimeAcc = 0.f;
	_float			m_fPressedAnimPosOffsetY = 0.f;
	const _float	m_fPressedAnimPosOffsetRatioY = 2.f;

	// For Controller
	_bool			m_bBodyCallLock = false;
};

END