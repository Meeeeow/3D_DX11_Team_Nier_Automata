#pragma once
#ifndef __CLIENT_BEAUVOIR_H__
#define __CLIENT_BEAUVOIR_H__

#include "GameObjectModel.h"
#include "Beauvoir_Skirt.h"
#include "Beauvoir_Hair.h"
#include "CrossAndroid.h"

#define BEAUVOIR_SONIC_COOLTIME 0.15
#define BEAUVOIR_MISSILE_NUM		5

class CBeauvoir final : public CGameObjectModel
{
	static const _uint CROSSANDROID_MAX = 10;
	static const _uint CREATABLE_INDEX = 28;
public:
	enum class PHASE	{ PHASE_APPEARANCE, PHASE1, PHASE2, PHASE3, NONE };
	enum class PATTERN  { CHASE, ROTATE_MELEE, SPREAD_BULLET, FIRE_MISSILES, ROTATE_LASER, LASER_RING, FASTROTATE_MELEE, 
						  FASTSPRED_BULLET, NAVI_BULLET,
						  MOVE_STAGE, MOVE_STAGE_JUMP, STAGE_LOOKAT, STAGE_IDLE, STAGE_JUMP_FLOOR,
						  RISING_BULLET, LOOK_FOWARD,
						  CHANGE_JUMP, JUMP_FLOOR,
						  DEAD, NONE };
private:
	enum class SPARK_BONE_POS
	{
		LEFT_UP = 0,
		LEFT_DOWN = 1,
		RIGHT_UP = 2,
		RIGHT_DOWN = 3,
		BACK = 4,
		COUNT = 5,
	};

	enum class MESH_CONTAINER : _int
	{
		BLADE =					33,
		COUNT =					35,
	};

private:
	CBeauvoir(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoir(const CBeauvoir& rhs);
	virtual ~CBeauvoir() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Check_Pattern(_double TimeDelta);
	void						Look_Target(_double TimeDelta);
	void						Chase_Target(_double TimeDelta);
	void						Look_Forward(_double TimeDelta);
	void						Rotate_Move(_double TimeDelta);
	void						JumpToPlayerPos(_double TimeDelta);

public:
	void						Set_CrossAndroidNum(_uint iNum) { m_iNumCrossAndroid = iNum; }
	const _uint					Get_CrossAndroidNum() const { return m_iNumCrossAndroid; }

public:
	void						Check_CrossAndroid(_double TimeDelta);

public:
	void						Set_ReverseLook();

public:
	void						Check_25PercentHP();
	void						Check_60PercentHP();
	void						Check_40PercentHP();

public:
	void						MoveToStage(_double TimeDelta);

public:
	void						Update_Parts();

public:
	void						Set_Skirt(CBeauvoir_Skirt* pSkirt);
	void						Set_Hair(CBeauvoir_Hair* pHair);

public:
	void						Check_Distance();							
public:
	void						Create_CrossAndroid(_double TimeDelta);

private:
	void						Update_Effects(_float fTimeDelta);
	_float						fSemiLaserDuration = 0.f;
private:
	HRESULT						Create_JumpBullet(_double TimeDelta);
	HRESULT						Create_FireMissile(_double TimeDelta);
	HRESULT						Create_LaserRing(_double TimeDelta);
	HRESULT						Create_RisingBullet(_double TimeDelta);
	HRESULT						Create_NaviBullet(_double TimeDelta);
	HRESULT						Create_JumpSmoke();
	HRESULT						Create_JumpImpact();
	HRESULT						Create_DeathEffect();

private:
	HRESULT						Create_LargeSparkEffect();
	HRESULT						Delete_LargeSparkEffect();
	HRESULT						Create_EndLargeSparkEffect();

private:
	HRESULT						Update_LargeSparkEffectTransform();

private:
	HRESULT						Create_Donut_Up();
	HRESULT						Create_RingWave();
	HRESULT						Check_CreateRingWave_Tetrad();						

private:
	HRESULT						Create_SemiLasers();
	HRESULT						Create_SemiLaser0(_float fAngle);
	HRESULT						Create_SemiLaser1(_float fAngle);
	HRESULT						Create_SemiLaser2(_float fAngle);
	HRESULT						Create_SemiLaser3(_float fAngle);
	HRESULT						Create_SemiLaser4(_float fAngle);
	HRESULT						Create_SemiLaser5(_float fAngle);
	HRESULT						Create_SemiLaser6(_float fAngle);
	HRESULT						Create_SemiLaser7(_float fAngle);
	HRESULT						Create_SemiLaser8(_float fAngle);
	HRESULT						Create_SemiLaser9(_float fAngle);
	HRESULT						Create_SemiLaser10(_float fAngle);
	HRESULT						Create_SemiLaser11(_float fAngle);

	HRESULT						Create_RealLasers();
	HRESULT						Create_RealLaser0(_float fAngle);
	HRESULT						Create_RealLaser1(_float fAngle);
	HRESULT						Create_RealLaser2(_float fAngle);
	HRESULT						Create_RealLaser3(_float fAngle);
	HRESULT						Create_RealLaser4(_float fAngle);
	HRESULT						Create_RealLaser5(_float fAngle);
	HRESULT						Create_RealLaser6(_float fAngle);
	HRESULT						Create_RealLaser7(_float fAngle);
	HRESULT						Create_RealLaser8(_float fAngle);
	HRESULT						Create_RealLaser9(_float fAngle);
	HRESULT						Create_RealLaser10(_float fAngle);
	HRESULT						Create_RealLaser11(_float fAngle);


	HRESULT						Delete_Lasers();
	HRESULT						Update_LaserEffectsPosition(_float fTimeDleta);
	HRESULT						Update_LaserEffect0Position(_float fAngle);
	HRESULT						Update_LaserEffect1Position(_float fAngle);
	HRESULT						Update_LaserEffect2Position(_float fAngle);
	HRESULT						Update_LaserEffect3Position(_float fAngle);
	HRESULT						Update_LaserEffect4Position(_float fAngle);
	HRESULT						Update_LaserEffect5Position(_float fAngle);

public:
	void						Set_Start_True() { m_bStart = true; }

	HRESULT						Update_LaserEffect6Position(_float fAngle);
	HRESULT						Update_LaserEffect7Position(_float fAngle);
	HRESULT						Update_LaserEffect8Position(_float fAngle);
	HRESULT						Update_LaserEffect9Position(_float fAngle);
	HRESULT						Update_LaserEffect10Position(_float fAngle);
	HRESULT						Update_LaserEffect11Position(_float fAngle);



private:
	CGameObject*				m_pLargeSparkEffect = nullptr;


private:
	_float						m_fLaserAngle = 0.f;
	CGameObject*				m_pLaserEffect0 = nullptr;
	CGameObject*				m_pLaserEffect1 = nullptr;
	CGameObject*				m_pLaserEffect2 = nullptr;
	CGameObject*				m_pLaserEffect3 = nullptr;
	CGameObject*				m_pLaserEffect4 = nullptr;
	CGameObject*				m_pLaserEffect5 = nullptr;
	CGameObject*				m_pLaserEffect6 = nullptr;
	CGameObject*				m_pLaserEffect7 = nullptr;
	CGameObject*				m_pLaserEffect8 = nullptr;
	CGameObject*				m_pLaserEffect9 = nullptr;
	CGameObject*				m_pLaserEffect10 = nullptr;
	CGameObject*				m_pLaserEffect11 = nullptr;

public:
	void						Set_Parts_ChangeState();

public:
	const _bool&				Get_Hackable() const { return m_bHackable; }

public:
	void						Set_Hackable(const _bool& bCheck) { m_bHackable = bCheck; }
	void						Set_HackingFinished() { m_bHackFinished = true; }

public:
	void						Check_Hp();
	void						Check_Dead();

public:
	void						CrossAndroid_Pulling();

public:
	virtual void				Update_HitType() override;
	void						Compute_DOF_Parameters();

public:
	virtual void				Notify(void* pMessage);

protected:
	virtual _int				VerifyChecksum(void* pMessage);

public:
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

protected:
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;


private:
	_float3						m_arrRimLightColor[static_cast<_int>(MESH_CONTAINER::COUNT)];
	_int						m_arrIsGlow[static_cast<_int>(MESH_CONTAINER::COUNT)];
private:
	_bool						m_bDeadStop = false;

private:
	_uint						m_iAnimation = 0;				
	_bool						m_bContinue = false;					

private:
	_double						m_PatternDelayTime = 0.0;				
	_double						m_PatternTime = 0.0;					
	PATTERN						m_eCurPattern = PATTERN::CHASE;			
	PHASE						m_eCurPhase = PHASE::PHASE1;			
	_uint						m_iRepeatCount = 0;						

private:
	_double						m_CreateJumpBulletTime = 0.0;			
	_double						m_CreateJumpBulletDelayTime = 0.5;		

	_double						m_CreateFireMissileTime = 0.0;			
	_double						m_CreateFireMissileDelayTime = 1.5;		
	
	_double						m_CreateLaserRingTime = 0.0;			
	_double						m_CreateLaserRingDelayTime = 0.5;		

	_double						m_CreateNaviBulletTime = 0.0;			
	const _double				m_CreateNaviBulletDelayTime = 0.3;		

	_double						m_CreateRisingBulletTime = 0.0;			
	_double						m_CreateRisingBulletDelayTime = 0.2;

private:
	_bool						m_bHackable = false;
	_bool						m_bHackOnce = false;
	_bool						m_bHackFinished = false;

	_float4						m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);

private:
	void						RimLighting(_double dTimeDelta);

private:
	vector<CCrossAndroid*>		m_vecCrossAndroid;

private:
	_bool						m_bRepeat = false;

private:
	_bool						m_b60PerOn = false;

private:
	_bool						m_bFastBullet = false;					
	_bool						m_bMissileDir = false;		

private:
	_bool						m_bChangeJumpStop = false;				
	_bool						m_bChangeLookTarget = false;			
	_bool						m_bChangeJumpLanding = false;			

private:
	_uint						m_iCreateRingWaveTetard = 0;

private:
	_bool						m_bNoneNavigation = false;

private:
	_bool						m_bCreateCrossAndroid = false;

private:
	_float3						m_vLerpLook = _float3(0.f, 0.f, 0.f);	
	_double						m_LookLerpTime = 0.0;					
	_float4						m_vStagePos = BEAUVOIR_JUMPSTART_TO_STAGE_POSITION;
	_bool						m_bJump = false;
	_uint						m_iContinueCount = 0;					

private:
	_float4x4					m_RisingRotateFloat4x4;

private:
	_float						m_fGapAngle = 0.f;						

private:
	CBeauvoir_Skirt*			m_pSkirt = nullptr;
	CBeauvoir_Hair*				m_pHair = nullptr;

private:
	_double						m_dCallHPBarUITimeDuration = 100.0;

private:
	_float3						m_vToPlayerDirection = _float3(0.f, 0.f, 0.f);

private:
	_bool						m_bStart = false;

	_float3						m_vRimLightColor = _float3(0.f, 0.f, 0.f);
	void						Set_RimLightColor(const _float3& _vColor) {
		m_vRimLightColor = _vColor;
	}

private:
	_uint						m_iNumCrossAndroid = 0;

	_bool						m_bIsFirstSonicCreated = false;
	_bool						m_bIsSecondSonicCreated = false;
	_double						m_dThirdSonicElapsedTime = 0.f;
private:
	HRESULT						Create_SonicEffect();

private:
	HRESULT						SetUp_Components();
	HRESULT						SetUp_ConstantTable();

public:
	static	CBeauvoir*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};


#endif // !__CLIENT_BEAUVOIR_H__