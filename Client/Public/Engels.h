#pragma once

#ifndef __CLIENT_ENGELS_H__
#define __CLIENT_ENGELS_H__

#include "GameObjectModel.h"
#include "Normal_Attack.h"
#include "EngelsHitPart.h"


#define WATER_SPLASH_CREATE_DELAY		1.0
#define HIT_TEXTURE_NUMBER				4
#define HIT_TEXTURE_MAX_TIME			1.0
#define HIT_TEXTURE_INTERVAL			0.05
BEGIN(Client)

class CEngels final : public CGameObjectModel
{
	static const _double ROTATEPOWER_MAX;
public:
	enum class DIRECTION { LEFT, CENTER, RIGHT, NONE };
	enum class PHASE { PHASE_APPEARANCE, PHASE_NORMAL, NONE };
	enum class PATTERN { APPEARNCE, 
						 CLAP, HOWL, SPIKERIGHTARM, SHOOTMISSILE, CONSECUTIVE_SPIKE,
						 SECOND_HOWL, REACHARM,
						 SWEEPRIGHTARM,
						 DEAD,
						 NONE };
	enum class MESH_CONTAINER : _int
	{
		HEAD =				7,		
		CHAINSAW =			20,
		ARM =				21,
		COUNT =				22,
	};


private:
	explicit CEngels(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CEngels(const CEngels& rhs);
	virtual ~CEngels() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						LerpLookToFirstLook(_double TimeDelta);
	void						Check_Pattern(_double TimeDelta);
	void						Create_FireMissile(_double TimeDelta);
	void						Create_MainBullet(_double TimeDelta);
	void						Create_RandomBullet(_double TimeDelta);
	void						Create_HitPart(_matrix Matrix);
	void						Delete_HitPart();
	void						Update_TimeDelta(_double& TimeDelta);

public:
	void						Check_DeadPattern();

public:
	void						Rotate_Wheel(_double TimeDelta);
	void						Initialize_RotateAngle();

public:
	void						Update_NormalAttack(_double TimeDelta);
	void						Update_ReachArmPos(_double TimeDelta);
	_matrix						Get_MatrixForNormalAttack(_uint iCount = 0, _vector& _vPos = XMVectorZero());
	void						Compute_DOF_Parameters();

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
	void						Set_UI_Engage_Cinematic();
	void						Set_UI_Engage();

private:
	_uint						m_iAnimation = 0;
	_bool						m_bContinue = false;
	_double						m_dCallHPBarUITimeDuration = 1.0;

private:
	_bool						m_bCreateOnce = false;

private:
	PHASE						m_eCurPhase = PHASE::PHASE_APPEARANCE;
	PATTERN						m_eCurPattern = PATTERN::APPEARNCE;

private:
	_float3						m_vFirstLook = _float3(0.f, 0.f, 0.f);
	_bool						m_bFirstLookLerp = false;
	_double						m_FirstLookLerpTime = 0.0;

private:
	_float						m_fRotateAngle = 0.f;
	_bool						m_bReverse = false;

private:
	_double						m_CreateTime = 0.0;
	_double						m_CreateMainBulletTime = 0.0;
	const _double				m_CreateFireMissileDelayTime = 0.75;
	const _double				m_CreateBulletDelayTime = 0.25;
	const _double				m_CreateMainBulletDelayTime = 0.3;

private:
	_double						m_RotatePower = ROTATEPOWER_MAX;

private:
	_double						m_DeadTime = 0.0;

public:
	_bool						m_bStart = false;
	_bool						m_bShowUI = false;


private:
	_bool						m_bIsWaterSplashCreated = false;
	_float						m_fWaterSplashCreateDelay = 0.f;
	_bool						m_bIsDeathWaterSplashCreated = false;


private:
	_float3						m_arrRimLightColor[static_cast<_int>(MESH_CONTAINER::COUNT)];
	_int						m_arrGlow[static_cast<_int>(MESH_CONTAINER::COUNT)];
private:
	_bool						m_bIsCreatedMissileLaunchEffect = true;
	_uint						m_iNumMissile = 0;

private:
	std::vector<CNormal_Attack*>	m_vecNormalAttacks;
	std::vector<CEngelsHitPart*>	m_vecHitPart;

private:
	HRESULT						Create_WaterSplash(_float4 _vPos);
	HRESULT						Create_ClapEffect(_float4 _vPos);
	HRESULT						Create_SpikeEffect(_float4 _vPos);
	HRESULT						Create_RightStraight_SparkHoldEffect(_float4 _vPos);
	HRESULT						Create_LeftStraight_SparkHoldEffect(_float4 _vPos);
	HRESULT						Create_RightSpike_SparkHoldEffect(_float4 _vPos);
	HRESULT						Create_LeftSpike_SparkHoldEffect(_float4 _vPos);
	HRESULT						Create_OnlyRightSpike_SparkHoldEffect(_float4 _vPos);
	HRESULT						Create_MissileLaunchEffect(_float4 _vPos);

private:
	void						Set_RimLightColor(const _int& i);
	void						Set_RimLightColorClear(const _int& i);

private:
	HRESULT						Update_Effects(_double _dTimeDelta);

private:
	CTexture*					m_pHitTexture = nullptr;
	_double						m_dHitTextureTime = 0.0;
	_double						m_dHitTextureIntervalTime = 0.0;
	_int						m_iHitTextureIndex = 0;

private:
	HRESULT						SetUp_Components();

public:
	static	CEngels*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

END

#endif

static const _uint ANIM_ENGELS_CONSECUTIVE_SPIKE_START = 19;	// ¿ÞÆÈ ¿À¸¥ÆÈ ÇÑ ¹ø¾¿ ³»·ÁÂï±â
static const _uint ANIM_ENGELS_REACHARM_DW			   = 20;	// ¿À¸¥ÆÈ °ÅµÎ°í ¿ÞÆÈ ¿À¸¥ÆÈ ÇÑ ¹ø¾¿ »¸±â
static const _uint ANIM_ENGELS_HOWL_START			   = 21;
static const _uint ANIM_ENGELS_SPIKE_RIGHTARM		   = 22;
static const _uint ANIM_ENGELS_CONSECUTIVE_SPIKE_END   = 31;	// ¿À¸¥ÆÈ ¿Ã¸®°í ¿ÞÆÈ ¿À¸¥ÆÈ µ¿½Ã¿¡ ³»·ÁÂï°í µ¹¾Æ¿À±â
static const _uint ANIM_ENGELS_CLAP					   = 32;
static const _uint ANIM_ENGELS_SHOOTMISSILE_DW		   = 43;
static const _uint ANIM_ENGELS_CONSECUTIVE_SPIKE_DW    = 46;	// ¿À¸¥ÆÈ ¿Ã¸®°í ¿ÞÆÈ ¿À¸¥ÆÈ ÇÑ ¹ø¾¿ ³»·ÁÂï±â
static const _uint ANIM_ENGELS_REACHARM_END			   = 56;	// ¿À¸¥ÆÈ °ÅµÎ°í ¿ÞÆÈ ¿À¸¥ÆÈ µ¿½Ã¿¡ »¸°í µ¹¾Æ¿À±â
static const _uint ANIM_ENGELS_SWEEP_RIGHTARM		   = 61;
static const _uint ANIM_ENGELS_SHOOTMISSILE_END		   = 71;
static const _uint ANIM_ENGELS_REACHARM_START		   = 82;	// ¿ÞÆÈ ¿À¸¥ÆÈ ÇÑ ¹ø¾¿ »¸±â
static const _uint ANIM_ENGELS_APPREANCE			   = 85;
static const _uint ANIM_ENGELS_DEAD					   = 90;
static const _uint ANIM_ENGELS_HOWL_DW				   = 91;
static const _uint ANIM_ENGELS_HOWL_END				   = 98;
static const _uint ANIM_ENGELS_SHOOTMISSILE_START	   = 101;
static const _uint ANIM_ENGELS_IDLE					   = 108;

static const _float	ENGELS_RANDOM_BULLET_SPEED		   = 30.f;
static const _float ENGELS_MAIN_BULLET_SPEED		   = 30.f;
static const _float	ENGELS_RANDOM_BULLET_MAX_LIFETIME  = 15.f;

static const _uint  ENGELS_CONSECUTIVE_SPIKE_END_ATTACKCOUNT = 1;

static const _double ENGELS_REACHARM_START_FIRSTATTACK_MAXRATIO  = 0.5852;
static const _double ENGELS_REACHARM_START_SECONDATTACK_MAXRATIO = 0.9699;

static const _double ENGELS_REACHARM_DW_FIRSTATTACK_MAXRATIO = 0.46549;
static const _double ENGELS_REACHARM_DW_SECONDATTACK_MAXRATIO = 0.96;
static const _float  ENGELS_REACHARM_SPEED = 20.f;


static const _double ENGELS_CLAP_DELETE_HITPART_RATIO = 0.66;
static const _double ENGELS_SPIKERIGHTARM_DELETE_HITPART_RATIO = 0.7656;
static const _double ENGELS_CONSECUTIVE_SPIKE_START_DELETE_HITPART_RATIO = 0.724;
static const _double ENGELS_CONSECUTIVE_SPIKE_DW_DELETE_HITPART_RATIO = 0.1;
static const _double ENGELS_CONSECUTIVE_SPIKE_DW_SEC_DELETE_HITPART_RATIO = 0.5815;
static const _double ENGELS_CONSECUTIVE_SPIKE_END_DELETE_HITPART_RATIO = 0.06;
static const _double ENGELS_CONSECUTIVE_SPIKE_END_SEC_DELETE_HITPART_RATIO = 0.5789;
static const _double ENGELS_REACHARM_START_CREATE_HITPART_RATIO = 0.5771;
static const _double ENGELS_REACHARM_START_DELETE_HITPART_RATIO = 0.7746;
static const _double ENGELS_REACHARM_START_SEC_CREATE_HITPART_RATIO = 0.9613;
static const _double ENGELS_REACHARM_DW_DELETE_HITPART_RATIO = 0.2255;
static const _double ENGELS_REACHARM_DW_CREATE_HITPART_RATIO = 0.4485;
static const _double ENGELS_REACHARM_DW_SEC_DELETE_HITPART_RATIO = 0.7169;
static const _double ENGELS_REACHARM_DW_SEC_CREATE_HITPART_RATIO = 0.9483;
static const _double ENGELS_REACHARM_END_DELETE_HITPART_RATIO = 0.13;
static const _double ENGELS_REACHARM_END_CREATE_HITPART_RATIO = 0.4686;
static const _double ENGELS_REACHARM_END_SEC_DELETE_HITPART_RATIO = 0.6969;


// CLAP
static const _double ENGELS_CLAP_UPDATETIMEDELTA_START_RATIO = 0.4741;
static const _double ENGELS_CLAP_UPDATETIMEDELTA_END_RATIO = 0.66;

// DEAD
static const _double ENGELS_DEAD_DELAYTIME = 2.0;
