#pragma once
#ifndef __CLIENT_HUMANOID_SMALL_H__
#define __CLIENT_HUMANOID_SMALL_H__

#include "GameObjectModel.h"


BEGIN(Client)
class CHumanoid_Small final : public CGameObjectModel
{
public:
	static	_uint	iObjIDAcc;

public:
	enum class M_ANIMSTATE : _uint
	{
		IDLE = 12,
		WALK = 15,
		ENGAGE = 147,
		SIDESTEP = 1,
		BACKSTEP = 33,
		F_JUMP = 19,
		DROP = 45,
		LANDING = 46,
		STANDING = 63,
		HIT = 998,
		NONE = 999
	};

	enum class ATTACK_STATE : _uint
	{
		PUNCH = 99,
		RUSH = 88,
		HEAD = 998,
		NONE = 999
	};

public:
	enum class WAVE : _uint
	{
		WAVE0 = 0,
		WAVE1 = 1,
		WAVE2 = 2,
		NONE = 999
	};

public:
	enum class M_PATTERN : _uint
	{
		PATTERN_1 = 1,
		PATTERN_2 = 2,
	};

private:
	CHumanoid_Small(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CHumanoid_Small(const CHumanoid_Small& rhs);
	virtual ~CHumanoid_Small() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

	HRESULT							Find_Enemy();

	_bool							Ray_Collision();

public:
	void							Chase(_double TimeDelta);
	//for pattern_1
	void							Check_Target1(_double TimeDelta);
	void							Check_AnimState1(_double TimeDelta);
	void							Check_Times1(_double TimeDelta);
	//for pattern_2
	void							Check_Target2(_double TimeDelta);
	void							Check_AnimState2(_double TimeDelta);
	void							Check_Times2(_double TimeDelta);
	//for pattern_3					
	void							Check_Target3(_double TimeDelta);
	void							Check_AnimState3(_double TimeDelta);
	void							Check_Times3(_double TimeDelta);

	void							Set_Jump_True() { m_bJump = true; }
	CTransform*						Get_TransformCom() { return m_pTransform; }
	void							Set_Wave(WAVE _Wave) { m_eWave = _Wave; }
	void							Set_Start_False() { m_bStart = false; }
	void							Set_Start_True() { m_bStart = true; }
	WAVE							Get_Wave() { return m_eWave; }

private:
	//for Patterns
	void							Gain_Pattern();
	void							Play_Pattern(_double TimeDelta);
	void							Check_Pattern_Cool(_double TimeDelta);
	void							Pattern_1(_double TimeDelta);
	void							Pattern_2(_double TimeDelta);
	void							Pattern_3(_double TimeDelta);

private:
	void							Update_FocusTime(_double TimeDelta);		// Seed Monster need
	 _fvector						Get_TargetPosition();					// Seed Monster need

private:
	void							Drop_RandomItem(ITEMCATEGORY eCategory);


private:
	void							Update_Bone();

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

private:
	_float							m_fRotation = 0.f;

public:
	static	CHumanoid_Small*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;


public:
	HRESULT							Create_DeathEffect();

private:
	HRESULT							Create_Laser_Hit_Effect();

private:
	HRESULT							Create_ElectricityEffect();
	void							Update_Effect(_double _dTimeDelta);

private:
	void							Update_Rush();
	void							Update_LeftArm();
	void							Update_Head();
	void							Update_NormalAttack();



private:
	_bool			m_bTargeting = false;
	_bool			m_bChase = false;
	_float			m_fRatio = 0.f;
	_float			m_fDistance = 0.f;

	//for attack1
	_float			m_fAttack1Time = 0.f;
	_bool			m_bAttack1Cool = false;
	//for attack2 
	_float			m_fAttack2Time = 0.f;
	_bool			m_bAttack2Cool = false;

	_bool			m_bStopCancleAnim = false;
	_uint			m_iAnimIndex = 0;
	_bool			m_bEngagementRagne = false;
	_bool			m_bChargeAttackStart = false;
	_float			m_fChargeTime = 0.f;

	//for backstep
	_bool			m_bBackStepCool = false;
	_float			m_fBackStepTime = 0.f;

	//for sidestep
	_bool			m_bSideStepCool = false;
	_float			m_fSideStepTime = 0.f;

	//for Enum
	M_PATTERN		m_ePattern;
	M_ANIMSTATE		m_eAnimState = M_ANIMSTATE::IDLE;

	// For Test
	_bool			m_bCallHPBarUI = false;
	_double			m_dCallHPBarUITimeDuration = 3.0;

	// For Create Electricity Effect;
	_bool			m_bCreateElectricity = false;
	_double			m_dCreateElectricityTimeDuration = 0.0;

	ATTACK_STATE	m_eAttackState = ATTACK_STATE::NONE;	

	// For Attack
	_float4x4		m_LArmPos;
	_float4x4		m_RushPos;
	_float4x4		m_HeadPos;
	_bool			m_bCreateOnce = false;

	_bool			m_bIsPunch = false;
	_bool			m_bIsRush = false;
	_bool			m_bIsHeading = false;

	_bool			m_bJump = false;

	_bool			m_bStart = true;

	WAVE			m_eWave = WAVE::WAVE0;

	_bool			m_bPlayOnce = false;

	std::vector<class CNormal_Attack*>	m_vecNormalAttacks;

	// For Sound
	static	_uint	m_iBombSoundChannel;
};

END

#endif // !__CLIENT_HUMANOID_SMALL_H__