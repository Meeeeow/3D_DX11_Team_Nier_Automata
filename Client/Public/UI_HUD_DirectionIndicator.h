#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_DirectionIndicator final : public CUI
{
public:
	static const _double	DURATION_ENEMY;
	static const _double	DURATION_OBJECTIVE;

	enum class INDICATORTYPE : _uint
	{
		OBJECTIVE	= 0,
		ENEMY		= 1,
		NONE		= 2
	};

	typedef struct tagUIIndicatorDesc
	{
		_uint			iObjID;
		OBJSTATE		eObjState;
		INDICATORTYPE	eIndicatorType;
		_float4			vTargetWorldPos;
		_double			dTimeDuration = CUI_HUD_DirectionIndicator::DURATION_OBJECTIVE;
	}UIINDICATORDESC;

private:
	CUI_HUD_DirectionIndicator(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_DirectionIndicator(const CUI_HUD_DirectionIndicator& rhs);
	virtual ~CUI_HUD_DirectionIndicator() DEFAULT;

public:
	static CUI_HUD_DirectionIndicator*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg)				override;
	virtual void			Free()							override;
	virtual HRESULT			NativeConstruct_Prototype()		override;
	virtual HRESULT			NativeConstruct(void* pArg)		override;

private:
	virtual HRESULT			SetUp_Components()				override;

public:
	virtual _int			Tick(_double TimeDelta)			override;
	virtual _int			LateTick(_double TimeDelta)		override;
	virtual HRESULT			Render()						override;

private:
	virtual HRESULT			Activate(_double dTimeDelta)	override;
	virtual HRESULT			Enable(_double dTimeDelta)		override;
	virtual HRESULT			Inactivate(_double dTimeDelta)	override;
	virtual HRESULT			Disable(_double dTimeDelta)		override;
	virtual HRESULT			Pressed(_double dTimeDelta)		override;
	virtual HRESULT			Released(_double dTimeDelta)	override;

	virtual void			Release_UI()					override;

	//_float					Calculate_Direction(const _float& fPosOffsetY);
	void					SetUp_Player();

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIINDICATORDESC*>	m_listIndicatorDesc;

	class CPlayer*	m_pPlayer = nullptr;

	_float4x4		m_matTest;
	_float			m_fRotateDegree = 0.f;
	_double			m_dRotationTimeAcc = 0.0;
	const _double	m_dRotationTimeMax = 1.0;

	const _float	m_fDefaultScale = 1.f;
	const _float	m_fDefaultRadius = 1.5f;

};

END