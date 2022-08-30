#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Dialogue_HighlightMessage final : public CUI
{
public:
	enum class EVENTID : _uint
	{
		BANNED			= 1,
		MISSION_CLEAR	= 2,
		MISSION_END		= 3,
		THANKS			= 4,
		NONE			= 5
	};

private:
	CUI_Dialogue_HighlightMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Dialogue_HighlightMessage(const CUI_Dialogue_HighlightMessage& rhs);
	virtual ~CUI_Dialogue_HighlightMessage() DEFAULT;

public:
	static CUI_Dialogue_HighlightMessage*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*					Clone(void* pArg)				override;
	virtual void							Free()							override;
	virtual HRESULT							NativeConstruct_Prototype()		override;
	virtual HRESULT							NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT							SetUp_Components()				override;

public:
	virtual _int							Tick(_double TimeDelta)			override;
	virtual _int							LateTick(_double TimeDelta)		override;
	virtual HRESULT							Render()						override;

private:
	virtual HRESULT							Activate(_double dTimeDelta)	override;
	virtual HRESULT							Enable(_double dTimeDelta)		override;
	virtual HRESULT							Inactivate(_double dTimeDelta)	override;
	virtual HRESULT							Disable(_double dTimeDelta)		override;
	virtual HRESULT							Pressed(_double dTimeDelta)		override;
	virtual HRESULT							Released(_double dTimeDelta)	override;

	virtual void							Release_UI() override {}

public:
	virtual void							Notify(void* pMessage)			override;

protected:
	virtual _int							VerifyChecksum(void* pMessage)	override;

private:
	EVENTID			m_eEventID = EVENTID::NONE;

	_bool			m_bIsMessageRender = false;
	_double			m_dTimeAcc = 0.f;
	const _tchar*	m_szTextElemKey = nullptr;

	const _float	m_fBasePosX = _float(g_iWinCX >> 1);
	const _float	m_fBasePosY = _float(g_iWinCY >> 1);

	_float			m_fAlphaAnim = -255.f;
	const _float	m_fAlphaAnimMin = -255.f;
	const _float	m_fAlphaAnimMax = 0.f;
	const _float	m_fAlphaAnimSpeed = 510.f;
};

END