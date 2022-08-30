#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Dialogue_EngageMessage final : public CUI
{
public:
	enum class BOSSID
	{
		ZHUANGZI	= 0,
		ENGELS		= 1,
		CHARIOT		= 2,
		BEAUVOIR	= 3,
		NONE		= 4
	};

private:
	explicit CUI_Dialogue_EngageMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Dialogue_EngageMessage(const CUI_Dialogue_EngageMessage& rhs);
	virtual ~CUI_Dialogue_EngageMessage() DEFAULT;

public:
	static CUI_Dialogue_EngageMessage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	_float			m_fUIPosY = 360.f;
	_float			m_fDecoLineOffsetY = 100.f;
	_float			m_fAlphaAnim = 0.f;
	_double			m_dAnimSpeed = 500.0;
	BOSSID			m_eBossID = BOSSID::NONE;
	_tchar*			m_szBossNameElemKey = nullptr;

	// For Control Enable Duration
	_double			m_dEnableDurationAcc = 0.0;
	_double			m_dEnableDurationTime = 2.5;

	// For -WARNING- Animation
	vector<_float>	m_vecLetterOffsetX;
	const _float	m_fLetterMoveSpeedX = 120.f;

	// For Boss Name Animation
	_float			m_fLetterOffsetY = 0.f;
	const _float	m_fLetterOffsetYMax = 60.f;
	const _float	m_fLetterMoveSpeedY = 60.f;

	// For Inactivate Param
	_float			m_fScaleYRatio = 1.f;
	_float			m_fScaleYRatioForFont = 1.f;
};

END