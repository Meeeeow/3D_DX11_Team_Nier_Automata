#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_Title final : public CUI
{
public:
	enum class TITLETYPE : _uint
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

	typedef struct tagUITitleDesc
	{
		UISTATE			eState = CUI::UISTATE::NONE;
		TITLETYPE		eType = TITLETYPE::COUNT;
	}UITITLEDESC;

private:
	CUI_Core_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_Title(const CUI_Core_Title& rhs);
	virtual ~CUI_Core_Title() DEFAULT;

public:
	static CUI_Core_Title*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	virtual void				Release_UI() override;

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	TITLETYPE		m_eTitleType = TITLETYPE::COUNT;
	const _tchar*	m_szTextElemKey = TEXT("");

	_float			m_fActivateAnimAlpha = -128.f;
	const _float	m_fActivateAnimAlphaMin = -128.f;
	const _float	m_fActivateAnimAlphaMax = 0.f;
	const _float	m_fActivateAnimAlphaSpeed = 256.f;

	const _float	m_fActivateAnimAlphaForShadow = -256.f;

	_float			m_fActivateAnimOffsetPos = 10.f;
	const _float	m_fActivateAnimOffsetPosMin = 10.f;
	const _float	m_fActivateAnimOffsetPosMax = 0.f;
	const _float	m_fActivateAnimOffsetPosSpeed = 20.f;

	_float			m_fShadowFontOffset = 0.f;
	const _float	m_fShadowFontOffsetMin = 0.f;
	const _float	m_fShadowFontOffsetMax = 5.f;
	const _float	m_fShadowFontOffsetPosSpeed = 10.f;
};

END