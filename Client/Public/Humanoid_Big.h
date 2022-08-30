#pragma once
#ifndef __CLIENT_HUMANOID_BIG__
#define __CLIENT_HUMANOID_BIG__


#include "GameObjectModel.h"


BEGIN(Client)
class CHumanoid_Big final : public CGameObjectModel
{

public:
	enum class M_ANIMSTATE : _uint
	{
		IDLE = 51,
		WALK = 139,
		ENGAGE = 147,
		SHOOTING = 998,
		BACKJUMP = 89,
		BACKSTEP = 137,
		FRONTRUN = 210,
		RIGHTSTEP = 237,
		LEFTSTEP = 236,
		ANGRY = 72,
		DROP = 27,
		DROPING = 228,
		LANDING = 107,
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
	};

private:
	explicit CHumanoid_Big(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CHumanoid_Big(const CHumanoid_Big& rhs);
	virtual ~CHumanoid_Big() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();

public:
	void							Chase(_double TimeDelta);
	void							Play_Pattern(_double dTimeDelta);
	void							Check_Pattern_Cool(_double TimeDelta);

	//for Pattern_1
	void							Pattern_1(_double TimeDelta);
	void							Check_Target1(_double TimeDelta);
	void							Check_AnimState1(_double TimeDelta);
	void							Check_Times1(_double TimeDelta);

	void							Set_Jump_True() { m_bJump = true; }
	CTransform*						Get_TransformCom() { return m_pTransform; }
	void							Set_Wave(WAVE _Wave) { m_eWave = _Wave; }
	void							Set_Start_False() { m_bStart = false; }
	void							Set_Start_True() { m_bStart = true; }
	WAVE							Get_Wave() { return m_eWave; }

private:
	void							Update_Bone();
	void							Update_NormalAttack();

private:
	void							Update_FocusTime(_double TimeDelta);		// Seed Monster need
	_fvector						Get_TargetPosition();				// Seed Monster need

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
	HRESULT							Update_Effect(_float fTimeDelta);

private:
	HRESULT							Shoot_Bullet();
	HRESULT							Shoot_Bullet_Big();
	HRESULT							Create_DeathEffect();

public:
	static	CHumanoid_Big*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;	
	virtual void					Free() override;

private:
	//for Anim
	_uint			m_iAnimIndex = 0;

	//for Enums
	M_PATTERN		m_ePattern;
	M_ANIMSTATE		m_eAnimState;

	//for Chase
	_float			m_fRatio = 0.f;
	_float			m_fDistance = 0.f;

	//for Status
	_bool			m_bRangeAttack = false;
	_bool			m_bEngage = false;

	//for Pattern
	_bool			m_bShootCool = false;
	_double			m_dShootCoolTime = 0.0;
	_bool			m_bMeleeCool = false;
	_double			m_dMeleeCoolTime = 0.0;
	_bool			m_bCreateOnce = false;

	_float4x4		m_MeleePos;

	std::vector<class CNormal_Attack*>	m_vecNormalAttacks;

	//for Shooting
	_double			m_dBulletTime = 0.0;
	_uint			m_iNumBullet = 0;
	_bool			m_bLaserOnce = false;

	//for BackStep 
	_double			m_dBackStepTime = 0.0;

	//for BackJump
	_double			m_dBackJumpTime = 0.0;
	_bool			m_bBackJumpCool = false;

	_bool			m_bJump = false;

	_bool			m_bStart = true;

	WAVE			m_eWave = WAVE::WAVE0;

	_bool			m_bPlayOnce = false;

	// For UI
	_double			m_dCallHPBarUITimeDuration = 3.0;

	CTexture*		m_pColorTexture = nullptr;
	CTexture*		m_pAlphaOneTexture = nullptr;
	_int			m_iAlphaOneTextureIndex = 0;
	_float			m_fAlphaOneTextureDuration = 0.f;
	_float			m_fAlphaOneTextureElapsedTime = 0.f;
};
END

#endif

