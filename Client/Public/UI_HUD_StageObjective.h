#pragma once
#include "UI.h"

BEGIN(Client)

class CUI_HUD_StageObjective final : public CUI
{
public:
	enum class QUEST : _uint
	{
		TRESUREBOX	= 0,
		ZHUANGZI	= 1,
		ENGELS		= 2,
		CHARIOT		= 3,
		BEAUVOIR	= 4,
		NONE		= 5
	};

	typedef struct tagUIObjectiveDesc
	{
		QUEST			eQuest = QUEST::NONE;
		_uint			iEventCount = 0;
		_uint			iEventCountMax = 0;

		tagUIObjectiveDesc() {}
		tagUIObjectiveDesc(const QUEST& eQ)
			: eQuest(eQ)
			, iEventCount(1)
			, iEventCountMax(10) {}
	}UIOBJECTIVEDESC;

private:
	CUI_HUD_StageObjective(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HUD_StageObjective(const CUI_HUD_StageObjective& rhs);
	virtual ~CUI_HUD_StageObjective() DEFAULT;

public:
	static CUI_HUD_StageObjective*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
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

	void					SetUp_Player();
	HRESULT					SetUp_RenderParameter();
	const _tchar*			SetUp_ElemKey(QUEST eQuest);

	void					Render_Counter(UIOBJECTIVEDESC* pDesc);

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	std::list<UIOBJECTIVEDESC*>	m_listObjectiveDesc;

	class CPlayer*	m_pPlayer = nullptr;

	_float			m_fRotationRadian = 0.f;
	_float4			m_vPlayerLook;
	_float4			m_vBackgroundPos;
	_float4			m_vTextBasePos;

	_float4x4		m_matTextPosOffset;
	_float4x4		m_matCounterPosOffset;
	
	_float4			m_vBackgroundScale = _float4(2.5f, 0.0f, 1.f, 0.f);
	const _float4	m_vBackgroundScaleMax = _float4(2.5f, 1.5f, 1.f, 0.f);
	_float4			m_vYorhaMarkScale = _float4(2.4f, 0.0f, 1.f, 0.f);
	const _float4	m_vYorhaMarkScaleMax = _float4(2.4f, 0.8f, 1.f, 0.f);
	
	const _float4	m_vCircleLineScale = _float4(18.f, 9.f, 1.f, 0.f);
	
	const _float4	m_vTextScale = _float4(5.f, 5.f, 1.f, 0.f);	

	_float			m_fRenderCounterLetterIntervalAccX = 0.2f;
	const _float	m_fRenderCounterLetterIntervalX = 0.015f;
	const _float	m_fRenderCounterLetterIntervalDefaultX = 0.2f;

	// For Activate / Inactivate
	_float			m_fActivateScaleY = 0.f;
	const _float	m_fActivateScaleMinY = 0.f;
	const _float	m_fActivateScaleMaxY = 1.f;
};

END