#pragma once

#ifndef __CLIENT_ZHUANGZI_H__
#define __CLIENT_ZHUANGZI_H__

#include "GameObjectModel.h"
#include "Normal_Attack.h"

class CZhuangzi final : public CGameObjectModel
{
public:
	enum class PHASE { PHASE_APPEARANCE, PHASE_NORMAL, PHASE_SPHERE, NONE };
	enum class PATTERN { APPEARANCE_IDLE, APPEARANCE_JUMP,
						 UNBEATABLE,
						 SWEEPFLOOR, SHOOTTRAIL, BLOW_JUMPBLOW, BEATING,
						 POWERFAILURE, 
						 STABTAIL, SPIKETAIL, SHOOTMISSILE, 
						 GROGGY, FIRSTGROGGY,
						 CHANGE_SPHERE, CHANGE_JUMPANDCHASE, CHANGE_POWERJUMP, CHANGE_DIZZY, CHANGE_CHASE, CHANGE_JUMPSPIKE,
						 NONE};

	enum class MESH_CONTAINER : _int
	{
		MAINBODY		= 0,	
		ROOTLEG			= 1,	
		MAINBODY_LEFT	= 2,	
		BODY_BOTTOM		= 10,	
		RIGHT_LEG_0		= 11,
		RIGHT_LEG_1		= 12,
		RIGHT_LEG_2		= 13,
		LEFT_LEG_0		= 14,
		LEFT_LEG_1		= 15,
		LEFT_LEG_2		= 16,
		FRONT_BLADE		= 18,
		OUT_BODY		= 19,
		INNER_TAIL		= 22,
		MIDDLE_TAIL		= 23,
		OUTTER_TAIL		= 24,
	};

private:
	CZhuangzi(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CZhuangzi(const CZhuangzi& rhs);
	virtual ~CZhuangzi() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Check_Pattern(_double TimeDelta);
	void						Check_Idle(_double TimeDelta);
	void						LerpLookToTarget(_double TimeDelta);
	void						Check_JumpSpike(const _bool& bCheck);

public:
	void						Set_FirstLook();
	void						Set_Start(_bool bCheck) { m_bStart = bCheck; }
	void						Set_BridgeAnimationStart();
	void						Set_BridgeClose();

public:
	void						Check_PlayerCell();

public:
	HRESULT						Change_ColliderBox();

public:
	void						Update_NormalAttack();
	_matrix						Get_MatrixForNormalAttack(_uint iCount = 0);

public:
	void						Check_90PercentHP();
	void						Check_15PercentHP();

public:
	HRESULT						Create_Missile(_double TimeDelta);
	HRESULT						Create_WeaponTrail();

private:
	HRESULT						Create_BodyShield();
	HRESULT						Create_LegShield();
	HRESULT						Create_Pile();
	HRESULT						Create_ImpactEffects();
	HRESULT						Create_Ring(_vector _vPos);
	HRESULT						Create_Ring_Spark(_vector _vPos);
	HRESULT						Create_FrontBlade_Spark(_vector _vPos);
	HRESULT						Create_TailSweep_Spark(_vector _vPos);
	HRESULT						Create_Rollin_Spark(_vector _vPos);
	HRESULT						Create_DeathEffect();

private:
	_double						m_dRollinElapsedTime = 0.0;
	_double						m_dSweepSparkElaspedTime = 0.0;

private:
	HRESULT						Delete_BodyShield();
	HRESULT						Delete_LegShields();
	HRESULT						Delete_Pile();
public:
	void						Set_DiagonalRight();
	void						Set_Back();
	void						Set_OppositeRight();

	void						Set_UI_Engage_Cinematic();
	void						Set_UI_Engage();

private:
	HRESULT						Update_Effects(_float fTimeDelta);

private:
	void						BlackOut(_double dTimeDelta);
	void						Change_FogParams(_double dTimeDelta);

private:
	bool						m_bChangeFog = false;

	_float						m_fFogEnd = 120.f;
	_float						m_fFogBegin = 60.f;

public:
	virtual void				Update_HitType() override;

public:
	virtual void				Notify(void* pMessage);

public:
	virtual _int				VerifyChecksum(void* pMessage);

public:
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

public:
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;

public:
	void						Set_Start_True() { m_bStart = true; }

private:
	PHASE						m_eCurPhase = PHASE::PHASE_NORMAL;
	PATTERN						m_eCurPattern = PATTERN::UNBEATABLE;

private:
	_uint						m_iAnimation = ANIM_ZHUANGZI_APPEARANCE_IDLE;
	_bool						m_bContinue = false;

private:
	_double						m_IdleTime = 0.0;
	_double						m_IdleDelayTime = 0.0;
	_bool						m_bIdle = false;
	_bool						m_bPassNext = false;

private:
	_double						m_GroggyTime = 0.0;
	_double						m_GroggyDelayTime = 7.0;
	const _double				m_FirstGroggyDelayTime = 5.0;

private:
	_bool						m_bSoundTiming = false;

private:
	_uint						m_iRepeatChaseCount = 0;
	_uint						m_iRepeatCount = 0;
	_bool						m_bCheckIf = false;				// 단순 if문 체크용
	_bool						m_bStart = false;

private:
	_bool						m_bLerpStop = false;
	_bool						m_bLookLerp = false;
	_double						m_LookLerpTime = 0.0;

private:
	_bool						m_bCreateOnce = false;
	_bool						m_bPowerFailure = false;
	_bool						m_bBlackOut = false;

private:
	_double						m_CreateTime = 0.0;
	_double						m_CreateDelayTime = 0.0;

private:
	_double						m_dCallHPBarUITimeDuration = 0.0;

private:
	_bool						m_bFirstGroggy = false;
	PATTERN						m_eMemoryPattern = PATTERN::NONE;
	_uint						m_iMemoryAnimation = 0;
	
private:
	std::vector<class CNormal_Attack*>	m_vecNormalAttacks;

private:
	_float3						m_vFrontBladeRimLightColor = _float3(0.f, 0.f, 0.f);

private:
	_float3						m_vTailRimLightColor = _float3(0.f, 0.f, 0.f);

// Effect
private:
	CGameObject*				m_pBodyShield = nullptr;
	CGameObject*				m_pLeftLegShield0 = nullptr;
	CGameObject*				m_pLeftLegShield1 = nullptr;
	CGameObject*				m_pLeftLegShield2 = nullptr;
	CGameObject*				m_pRightLegShield0 = nullptr;
	CGameObject*				m_pRightLegShield1 = nullptr;
	CGameObject*				m_pRightLegShield2 = nullptr;
	CGameObject*				m_pPileEffect = nullptr;


private:
	_bool						m_bTempRightLeg0 = false;
	_bool						m_bTempRightLeg1 = false;
	_bool						m_bTempRightLeg2 = false;
	_bool						m_bTempLeftLeg0 = false;
	_bool						m_bTempLeftLeg1 = false;
	_bool						m_bTempLeftLeg2 = false;

	// 0 : RightLeg0
	// 1 : RightLeg1
	// 2 : RightLeg2
	// 3 : LeftLeg0
	// 4 : LeftLeg1
	// 5 : LeftLeg2
	_float3						m_arrLegRimLightColor[6];
	_bool						m_arrTempFrontBladeSpark[13];
	_float3						m_vWholeBodyRimLightColor = _float3(0.f, 0.f, 0.f);


	_uint						m_iNumMissile = 0;



private:
	HRESULT						SetUp_Components();

public:
	static	CZhuangzi*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

#endif

