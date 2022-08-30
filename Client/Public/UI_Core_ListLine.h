#pragma once
#include "UI.h"

#define COMMAND_CORE_LISTLINE_INITIALIZE	0x00000000
#define COMMAND_CORE_LISTLINE_LOCK			0x00000001
#define COMMAND_CORE_LISTLINE_UNLOCK		0x00000002
#define COMMAND_CORE_LISTLINE_SCROLL		0x00000004

BEGIN(Client)

class CUI_Core_ListLine final : public CUI
{
public:
	typedef struct tagUIListLineDesc
	{
		_float			fPrevLineOffsetScaleY = 0.f;
		_float			fCrntLineOffsetScaleY = 0.f;
		ITEMDESC		tItemDesc;
	}UILISTLINEDESC;

private:
	CUI_Core_ListLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_ListLine(const CUI_Core_ListLine& rhs);
	virtual ~CUI_Core_ListLine() DEFAULT;

public:
	static CUI_Core_ListLine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	// General Parameter
	UILISTLINEDESC	m_tLineDesc;
	_uint			m_iCommandFlag = COMMAND_CORE_LISTLINE_INITIALIZE;
	_uint			m_iInitialLineNumber = 0;

	// For Render
	const _float	m_fRenderPosBaseX = 200.f;
	const _float	m_fRenderPosBaseY = 295.f;
	const _float	m_fRenderIntervalPosOffsetY = 42.f;
	const _float	m_fRenderIconPosOffsetX = 120.f;
	const _float	m_fRenderTextPosOffsetX = -100.f;

	_uint			m_iRenderNumber = 0;
	_uint			m_iBufferNumber = 0;
	const _float	m_fRenderAmountPosOffsetX = 130.f;
	_float			m_fRenderAmountLetterIntervalAccX = 0.f;
	const _float	m_fRenderAmountLetterIntervalX = 14.f;

	// For Enable
	_float			m_fEnableAnimAlpha = -128.f;
	const _float	m_fEnableAnimAlphaSpeed = 178.f;
	const _float	m_fEnableAnimAlphaMin = -128.f;	// Deco::Background 원본 텍스처의 Alpha값
	const _float	m_fEnableAnimAlphaMax = 50.f;	// Deco::Background 렌더 시 최종 목표 Alpha값

	// For Activate
	_float			m_fUVAnimCursorX = 0.f;
	const _float	m_fUVAnimCursorXMin = 0.f;
	const _float	m_fUVAnimCursorXMax = 1.f;

	_float			m_fActivateLineOffsetY = 16.f;
	const _float	m_fActivateLineOffsetYMin = 16.f;
	const _float	m_fActivateLineOffsetYMax = 20.f;
	const _float	m_fActivateLineOffsetYSpeed = 40.f;

	// For Pressed
	_bool			m_bPressed = false;
	_float			m_fPressedAnimTimeAcc = 0.f;
	_float			m_fPressedAnimPosOffsetY = 0.f;
	const _float	m_fPressedAnimPosOffsetRatioY = 2.f;

	// For Controller
	//_bool			m_bDetailCallLock = false;
};

END