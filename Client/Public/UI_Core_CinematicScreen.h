#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_Core_CinematicScreen final : public CUI
{

	// On / Off 여부, 지속시간, 위아래 Offset에 대한 정보 받는 구조체 정의 필요


private:
	explicit CUI_Core_CinematicScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_Core_CinematicScreen(const CUI_Core_CinematicScreen& rhs);
	virtual ~CUI_Core_CinematicScreen() DEFAULT;

public:
	static CUI_Core_CinematicScreen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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
	_double		m_dUVAnimSpeed = 500.0;
	_double		m_dUVAnimTimeAcc = 0.0;
	_double		m_dUVAnimTimeMax = 1.0;

	_float		m_fActivatePosOffsetY = 100.f;
	_float		m_fActivatePosOffsetYSpeed = 100.f;
};

END