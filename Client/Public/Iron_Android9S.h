#pragma once

#ifndef __CLIENT_IRON_ANDROID_9S_H__
#define __CLIENT_IRON_ANDROID_9S_H__

#include "Client_Defines.h"
#include "GameObjectModel.h"
#include "IronGauntlet.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

static const _uint				ANIM_IRON9S_RUN = 0;

static const _uint				ANIM_IRON9S_JUMP_START = 1;
static const _uint				ANIM_IRON9S_JUMP_DW = 2;
static const _uint				ANIM_IRON9S_JUMP_END = 3;
static const _uint				ANIM_IRON9S_DOUBLEJUMP = 4;

static const _uint				ANIM_IRON9S_PUNCH1 = 5;
static const _uint				ANIM_IRON9S_PUNCH2 = 6;
static const _uint				ANIM_IRON9S_PUNCH3 = 7;

static const _uint				ANIM_IRON9S_KICK1 = 8;

static const _uint				ANIM_IRON9S_DASH_PUNCH = 9;
static const _uint				ANIM_IRON9S_DASH_KICK = 10;

static const _uint				ANIM_IRON9S_SKILL_UPPERCUT = 11;
static const _uint				ANIM_IRON9S_SKILL_THROW = 12;

static const _uint				ANIM_IRON9S_JUMPATTACK_START = 13;
static const _uint				ANIM_IRON9S_JUMPATTACK_DW = 14;
static const _uint				ANIM_IRON9S_JUMPATTACK_END = 15;

static const _uint				ANIM_IRON9S_POWERPUNCH = 16;

static const _uint				ANIM_IRON9S_HIT1 = 17;
static const _uint				ANIM_IRON9S_HIT2 = 18;
static const _uint				ANIM_IRON9S_HIT3 = 19;

static const _uint				ANIM_IRON9S_KNOCKOUT = 20;

static const _uint				ANIM_IRON9S_DEAD = 21;

static const _uint				ANIM_IRON9S_IDLE = 23;

static const _uint				ANIM_IRON9S_APPEARANCE = 26;

static const _float				JUMPATTACKSPEED = 40.f;

class CIron_Android9S final : public CGameObjectModel
{
public:
	enum class ANIMSTATE {IDLE, RUN, HIT, JUMP, ATTACK, JUMPATTACK, DEAD, NONE };

public:
	CIron_Android9S(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CIron_Android9S(const CIron_Android9S& rhs);
	virtual ~CIron_Android9S() DEFAULT;

public:
	virtual HRESULT					NativeConstruct_Prototype();
	virtual HRESULT					NativeConstruct(void* pArg);
	virtual _int					Tick(_double TimeDelta);
	virtual _int					LateTick(_double TimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_Gauntlet(CIronGauntlet* pGauntlet);

public:
	void							Set_Animation(_uint iAnimIndex);

public:
	virtual void					Update_HitType() override;

public:
	void							Key_Check(_double TimeDelta);
	void							Check_Idle();
	void							Check_JumpState(_double TimeDelta, _bool& bKeyDown);
	void							Check_AttackState(_double TimeDelta, _bool& bLKeyDown);
	void							Check_RightAttackState(_double TimeDelta, _bool& bRKeyDown);
	void							Check_HitState(_double TimeDelta);

public:
	void							Check_Dead();

public:
	_uint							Get_Animationindex() const { return m_iAnimation; }

public:
	void							Set_GauntletIdle();

public:
	void							LerpToCameraRight(_double TimeDelta);
	void							LerpToCameraLook(_double TimeDelta);
	void							LerpToCameraBack(_double TimeDelta);
	void							LerpToCameraLeft(_double TimeDelta);

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	void							Check_WeaponState();

public:
	void							Fix_PositionZ();

public:
	void							Set_FirstLook();

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

private:
	_bool							m_bContinue = false;
	_uint							m_iAnimation = 0;
	ANIMSTATE						m_eAnimState = ANIMSTATE::IDLE;

private:
	_float4							m_vHitTargetPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double							m_HitLerpTime = 0.0;
	_uint							m_iGetDamage = 0;
	_uint							m_iPreHitAnim = ANIM_IRON9S_HIT2;

private:
	_bool							m_bAppearanceSound = false;

private:
	_double							m_LookLerpTime = 0.0;
	_float3							m_vCameraRight = _float3(0.f, 0.f, 0.f);
	_float3							m_vCameraLeft = _float3(0.f, 0.f, 0.f);
	_float3							m_vCameraLook = _float3(0.f, 0.f, 0.f);
	_float3							m_vCameraBack = _float3(0.f, 0.f, 0.f);

private:
	_bool							m_bLbuttonDown = false;
	_bool							m_bRbuttonDown = false;



private:
	CIronGauntlet*					m_pGauntlet = nullptr;

private:
	DIRECTION						m_eDirection = DIRECTION::RIGHT;

private:
	_double							m_dCallHPBarUITimeDuration = 0.0;

private:
	HRESULT							Create_JumpAttackEffect();
	_float							m_fBaseYPosition = 0.f;
	_bool							m_bIsBaseYInit = false;
private:
	virtual HRESULT					SetUp_Components() override;

public:
	static CIron_Android9S*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*			Clone(void* pArg);
	virtual void					Free();
};

END

#endif