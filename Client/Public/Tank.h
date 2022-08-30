#pragma once
#ifndef __CLIENT_TANK_H__
#define __CLIENT_TANK_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CTank final : public CGameObjectModel
{
public:
	enum class M_ANIMSTATE : _uint
	{
		IDLE = 42,
		M_FOWARD = 0,
		M_BACKWARD = 1,
		TURN_LEFT = 2,
		STUN_WEAK = 3,
		STUN_STRONG = 4,
		DESTROYED = 5,
		HIT_NORMAL = 6,
		GROGGY = 7,
		SWEEP_L = 12,
		SWEEP_R = 60,
		STAMP_L = 34,
		STAMP_R = 61,
		STAMP_B = 22,
		RUSH = 23,
		SHOOTING = 63,
		MONINGSTAR = 35,
		NONE = 999
	};

	enum class M_PATTERN : _uint
	{
		PATTERN_1 = 1,
		PATTERN_2 = 2,
		PATTERN_3 = 3,
		PATTERN_4 = 4,
		PATTERN_5 = 5,
		DESTROYED = 6,
		NONE = 999
	};

private:
	CTank(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CTank(const CTank& rhs);
	virtual ~CTank() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Chase(_double TimeDelta);
	void						Check_Target(_double TimeDelta);
	void						Check_AnimState(_double TimeDelta);
	void						Check_Times(_double TimeDelta);
	void						Update_Bone();
	void						Update_RightArm();
	void						Update_LeftArm();
	void						Update_MoningStar();
	void						Update_Turret();
	void						Update_Arms();

	void						Set_UI_Engage_Cinematic();
	void						Set_UI_Engage();

private:
	void						Update_NormalAttack();

	// For Effect
	void						Set_LCrashPos();
	void						Set_RCrashPos();

public:
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

public:
	virtual void				Notify(void* pMessage);

protected:
	virtual _int				VerifyChecksum(void* pMessage);

protected:
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;

public:
	static	CTank*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

private:
	CTexture*					m_pDissolveTexture = nullptr;
	_float						m_fDissolveVal = 0.f;
	_float						m_fDissolveSpeed = 0.f;


//////////////////////////////////
// For Effects
private:
	HRESULT			Update_Effects(_float _fTimeDelta);

private:
	HRESULT			Create_Smoke(_float4x4 _vMatrix);
	HRESULT			Create_LeftArmSpark();
	HRESULT			Create_RightArmSpark();
	HRESULT			Create_Comfetti();
	HRESULT			Create_Laser();

private:
	HRESULT			Update_Laser();

private:
	HRESULT			Delete_LeftArmSpark();
	HRESULT			Delete_RightArmSpark();
	HRESULT			Delete_Laser();

private:
	CGameObject*	m_pLeftArmSpark = nullptr;
	CGameObject*	m_pRightArmSpark = nullptr;
	CGameObject*	m_pLaser = nullptr;
//////////////////////////////////

private:

	//for Anim
	_uint			m_iAnimIndex = 0;

	//for Pattern
	M_ANIMSTATE		m_eAnimState = M_ANIMSTATE::NONE;
	M_PATTERN		m_ePattern = M_PATTERN::NONE;
	_uint			m_iPatternNum = 0;
	_double			m_dPatternTime = 0.0;
	_bool			m_bPatternStart = false;
	_bool			m_bShakeBigOnce = false;
	_bool			m_bShakeMidOnce1 = false;
	_bool			m_bShakeMidOnce2 = false;
	_bool			m_bPlusOnce = false;
	_bool			m_bTraceFast = false;
	_bool			m_bCamHold = false;
	_bool			m_bCreateOnce = false;

	//for Chase
	_float			m_fRatio = 0.f;
	_float			m_fDistance = 0.f;

	//for Pattern
	_float4x4		m_LArmPos;
	_float4x4		m_RArmPos;
	_float4x4		m_MStarPos;
	_float4x4		m_TurretMatrix;
	_float4x4		m_BothPos;

	std::vector<class CNormal_Attack*>	m_vecNormalAttacks;

	//for Debug
	CCollider*		m_RightArm = nullptr;
	CCollider*		m_LeftArm = nullptr;
	CCollider*		m_MoningStar = nullptr;
	CCollider*		m_Turret = nullptr;

	// For UI
	_double			m_dCallHPBarUITimeDuration = 3.0;

	_bool			m_bIsLArm = false;
	_bool			m_bIsRArm = false;
	_bool			m_bIsMstar = false;
	_bool			m_bIsBoth = false;

	_bool			m_bPlayPattern = true;

	//For Effect
	_float4			m_LCrashPos;
	_float4			m_RCrashPos;
};
END


#endif // !__CLIENT_BEAUVOIR_H__

