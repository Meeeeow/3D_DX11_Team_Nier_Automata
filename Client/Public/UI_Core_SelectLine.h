#pragma once
#include "UI.h"

#define COMMAND_CORE_SELECTLINE_INITIALIZE	0x00000000
#define COMMAND_CORE_SELECTLINE_LOCK		0x00000001
#define COMMAND_CORE_SELECTLINE_UNLOCK		0x00000002
#define COMMAND_CORE_SELECTLINE_SCROLL		0x00000004

BEGIN(Client)

class CUI_Core_SelectLine final : public CUI
{
public:
	enum class ROLE : _uint
	{
		USE			= 0,
		DISPOSE		= 1,
		EQUIP_MAIN	= 2,
		EQUIP_SUB	= 3,
		RELEASE		= 4,
		INSTALL		= 5,
		UNINSTALL	= 6,
		NONE		= 7
	};

	typedef struct tagUISelectLineDesc
	{
		ROLE			eRole = ROLE::NONE;
		ITEMDESC		tItemDesc;
		_float			fPosX = 0.f;
		_float			fPosY = 0.f;
	}UISELECTLINEDESC;

private:
	CUI_Core_SelectLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_SelectLine(const CUI_Core_SelectLine& rhs);
	virtual ~CUI_Core_SelectLine() DEFAULT;

public:
	static CUI_Core_SelectLine*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	// General Parameter
	UISELECTLINEDESC	m_tLineDesc;
	ROLE				m_eLineRole = CUI_Core_SelectLine::ROLE::NONE;
	_uint				m_iCommandFlag = COMMAND_CORE_SELECTLINE_INITIALIZE;
	_uint				m_iInitialLineNumber = 0;

	// For Render
	const _float	m_fRenderPosBaseX = 450.f;
	
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

};

END