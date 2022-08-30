#pragma once

#ifndef __CLIENT_IRON_HUMANOID_SMALL_H__
#define __CLIENT_IRON_HUMANOID_SMALL_H__

#include "GameObjectModel.h"

BEGIN(Client)

static const _uint ANIM_IRONHS_GUARD				= 2;

static const _uint ANIM_IRONHS_IDLE					= 12;

static const _uint ANIM_IRONHS_WALK					= 15;
static const _uint ANIM_IRONHS_MOONWALK				= 16;

static const _uint ANIM_IRONHS_DASH_JUMP			= 20;

static const _uint ANIM_IRONHS_HIT1					= 29;
static const _uint ANIM_IRONHS_HIT2					= 31;
static const _uint ANIM_IRONHS_HIT3					= 33;

static const _uint ANIM_IRONHS_AIRBORNE_HIT			= 41;
static const _uint ANIM_IRONHS_AIRBORNE_HIT_LANDING	= 58;
static const _uint ANIM_IRONHS_AIRBORNE_HIT_STAND	= 59;

static const _uint ANIM_IRONHS_DEAD_START			= 73;
static const _uint ANIM_IRONHS_DEAD_END				= 74;

static const _uint ANIM_IRONHS_HEADATTACK_START		= 77;
static const _uint ANIM_IRONHS_HEADATTACK_DW		= 78;
static const _uint ANIM_IRONHS_HEADATTACK_END		= 79;

static const _uint ANIM_IRONHS_APPEARANCE			= 87;

static const _uint ANIM_IRONHS_DASHATTACK_START		= 98;
static const _uint ANIM_IRONHS_DASHATTACK_DW		= 99;
static const _uint ANIM_IRONHS_DASHATTACK_END		= 100;

static const _float FIRST_AIRBORNEJUMPTIME = 0.12f;

class CIron_Humanoid_Small final : public CGameObjectModel
{
public:
	enum ANIMSTATE { IDLE, ATTACK, WALK, HIT, AIRBORNE, GUARD, DEAD, NONE };

public:
	CIron_Humanoid_Small(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CIron_Humanoid_Small(const CIron_Humanoid_Small& rhs);
	virtual ~CIron_Humanoid_Small() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct_Prototype() override;
	virtual HRESULT						NativeConstruct(void* pArg) override;
	virtual _int						Tick(_double dTimeDelta);
	virtual _int						LateTick(_double dTimeDelta);
	virtual HRESULT						Render();

public:
	ANIMSTATE							Get_AnimState() const { return m_eAnimState; }

public:
	void								Set_FirstLook();
	void								Check_Distance();
	void								Check_Dead();

public:
	void								Check_Airborne(_double TimeDelta);
	void								Activate_Guard();
	void								Check_GuardState();
	void								Check_AttackPattern(_double TimeDelta);

public:
	void								Set_PlayAnimation(_bool bCheck);
	void								Set_Animation(_uint iIndex) { m_iAnimation = iIndex; }

public:
	void								Set_LooktoTarget();

public:
	virtual void						Update_HitType() override;

public:
	void								LerpLooktoTarget(_double TimeDelta);

public:
	void								Fix_PositionZ();

public:
	virtual _bool						Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void						Collision(CGameObject* pGameObject) override;

public:
	virtual HRESULT						SetUp_Components() override;

public:
	virtual void						Notify(void* pMessage)			override;

protected:
	virtual _int						VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT						Update_UI(_double dDeltaTime)	override;

private:
	_uint								m_iAnimation = 0;
	_bool								m_bContinue = false;
	ANIMSTATE							m_eAnimState = ANIMSTATE::IDLE;

private:
	DIRECTION							m_eDirection = DIRECTION::RIGHT;
	_double								m_LookLerpTime = 1.0;

private:
	_float3								m_vLeft = _float3(0.f, 0.f, 0.f);
	_float3								m_vRight = _float3(0.f, 0.f, 0.f);

private:
	_double								m_WaitTime = 0.0;
	const _double						m_WaitDelayTime = 3.0;

private:
	_bool								m_bAirborne = false;
	_bool								m_bFirstAirborne = false;

private:
	_bool								m_bPlayAnimation = false;

private:
	_uint								m_iTempTargetAnimation = 0;

private:
	HRESULT								Create_PurplishRedFireEffect();
	HRESULT								Create_BlueFireEffect();
	HRESULT								Create_ElectricityEffect();


public:
	static	CIron_Humanoid_Small*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END

#endif