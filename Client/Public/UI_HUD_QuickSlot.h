#pragma once
#include "UI.h"
#include "Player.h"

BEGIN(Client)

class CUI_HUD_QuickSlot final : public CUI
{
private:
	CUI_HUD_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_QuickSlot(const CUI_HUD_QuickSlot& rhs);
	virtual ~CUI_HUD_QuickSlot() DEFAULT;

public:
	static CUI_HUD_QuickSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	const _float					m_fBasePosX = 300.f;
	const _float					m_fBasePosY = 600.f;

	_float							m_fAlphaAnimOffset = 0.f;
	const _float					m_fAlphaAnimOffsetMin = 0.f;
	const _float					m_fAlphaAnimOffsetMax = -255.f;
	const _float					m_fAlphaAnimSpeed = 255.f;

	CPlayer::EQUIPSTATE				m_eEquipState = CPlayer::EQUIPSTATE::NONE;
};

END


