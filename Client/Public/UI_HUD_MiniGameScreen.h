#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_MiniGameScreen final : public CUI
{
public:
	static const _double	TOTAL_DURATION_TIME;

private:
	CUI_HUD_MiniGameScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_MiniGameScreen(const CUI_HUD_MiniGameScreen& rhs);
	virtual ~CUI_HUD_MiniGameScreen() DEFAULT;

public:
	static CUI_HUD_MiniGameScreen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg)				override;
	virtual void					Free()							override;
	virtual HRESULT					NativeConstruct_Prototype()		override;
	virtual HRESULT					NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT					SetUp_Components()				override;

public:
	virtual _int					Tick(_double TimeDelta)			override;
	virtual _int					LateTick(_double TimeDelta)		override;
	virtual HRESULT					Render()						override;

private:
	virtual HRESULT					Activate(_double dTimeDelta)	override;
	virtual HRESULT					Enable(_double dTimeDelta)		override;
	virtual HRESULT					Inactivate(_double dTimeDelta)	override;
	virtual HRESULT					Disable(_double dTimeDelta)		override;
	virtual HRESULT					Pressed(_double dTimeDelta)		override;
	virtual HRESULT					Released(_double dTimeDelta)	override;

	virtual void					Release_UI() override {}

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

private:
	const _float					m_fBasePosX = _float(g_iWinCX >> 1);
	const _float					m_fBasePosY = _float(g_iWinCY >> 1);

	_float							m_fActivatePosOffsetX = 0.f;
	const _float					m_fActivatePosOffsetXMin = -300.f;
	const _float					m_fActivatePosOffsetXMax = 200.f;
	const _float					m_fActivatePosOffsetSpeed = 600.f;

	_float							m_fBackGroundColorAnimOffset = 255.f;
	const _float					m_fBackGroundColorAnimOffsetMin = 0.f;
	const _float					m_fBackGroundColorAnimOffsetMax = 255.f;
	const _float					m_fBackGroundColorAnimSpeed = 765.f;

	_float							m_fBackGroundAlphaAnimOffset = 0.f;
	const _float					m_fBackGroundAlphaAnimOffsetMin = 0.f;
	const _float					m_fBackGroundAlphaAnimOffsetMax = -255.f;
	const _float					m_fBackGroundAlphaAnimSpeed = 765.f;

	_float							m_fCharacterAlphaOffset = -128.f;
	const _float					m_fCharacterAlphaOffsetMin = -128.f;
	const _float					m_fCharacterAlphaOffsetMax = 0.f;
	const _float					m_fCharacterAlphaOffsetSpeed = 256.f;

	_double							m_fEnableTimaAcc = 0.f;
	const _double					m_fEnableTimaAccMax = 1.5f;
};

END


