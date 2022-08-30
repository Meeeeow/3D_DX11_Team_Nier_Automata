#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Dialogue_HackingMessage final : public CUI
{
public:
	static const _tchar*	LETTERELEMKEY[];
	static const _uint		LETTERMAXCOUNT;
	static const _float		LETTEROFFSETX;

private:
	explicit CUI_Dialogue_HackingMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Dialogue_HackingMessage(const CUI_Dialogue_HackingMessage& rhs);
	virtual ~CUI_Dialogue_HackingMessage() DEFAULT;

public:
	static CUI_Dialogue_HackingMessage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	// For General Param
	const _float	m_fBasePosX = 640.f;
	const _float	m_fBasePosY = 360.f;

	_double			m_dCountDownAcc = 0.0;
	_uint			m_iTextIndex = 0;
	const _uint		m_iTextIndexMax = 3;
	const _tchar*	m_szTextElemKey = TEXT("HACKINGTHREE");

	_float			m_fTextAlpha = -128.f;
	const _float	m_fTextAlphaMin = -128.f;
	const _float	m_fTextAlphaMax = 0.f;
	const _float	m_fTextAlphaSpeed = 256.f;

	_float			m_fTextScale = 2.f;
	const _float	m_fTextScaleMin = 1.f;
	const _float	m_fTextScaleMax = 2.f;
	const _float	m_fTextScaleSpeed = 2.f;

	_float			m_fHackingTextScale = 2.f;
	const _float	m_fHackingTextScaleMin = 2.f;
	const _float	m_fHackingTextScaleMax = 2.5f;
	const _float	m_fHackingTextAlpha = 0.f;

	// For - HACKING START - Animation
	vector<_float>	m_vecLetterOffsetX;
	_double			m_fLetterAnimTimeAcc = 0.0;
	const _double	m_fLetterAnimTimeMax = 0.3;
	const _uint		m_iLetterOffsetCount = 8;
	const _float	m_fLetterMoveSpeedX = 200.f;

	_float			m_fLetterAlpha = 0.0;
	const _float	m_fLetterAlphaMin = 0.f;
	const _float	m_fLetterAlphaMax = 255.f;
	const _float	m_fLetterAlphaSpeed = 765.f;

	const _float	m_fLetterInactivateAnimColor = 255.f;

	_bool			m_bIsHackingStart = false;	
	_uint			m_iTimeCount = 0;

	_double			m_dTestTimeAcc = 0.0;
};

END