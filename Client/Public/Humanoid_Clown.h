#pragma once
#ifndef __CLIENT_HUMANOID_CLOWN__
#define __CLIENT_HUMANOID_CLOWN__

#include "GameObjectModel.h"

BEGIN(Client)
class CHumanoid_Clown final : public CGameObjectModel
{
public:
	static	_uint	iObjIDAcc;

public:
	enum class M_ANIMSTATE : _uint
	{
		WALK = 0,
		CONFETTI = 3,
		DANCE_BEGIN = 8,
		DANCING = 9,
		DANCE_END = 10,
		IDLE = 14,
		START_DANCE = 998,
		NONE = 999
	};
public:
	enum class DANCE_STATE : _uint
	{
		BEGIN = 99,
		DOING= 88,
		FINISH= 998,
		NONE = 999
	};

public:
	enum class M_PATTERN : _uint
	{
		PATTERN_1 = 1,
		PATTERN_2 = 2,
	};

private:
	explicit CHumanoid_Clown(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CHumanoid_Clown(const CHumanoid_Clown& rhs);
	virtual ~CHumanoid_Clown() DEFAULT;

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
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

public:
	HRESULT							Create_DeathEffect();

	void							Chase(_double TimeDelta);
	//for pattern_1
	void							Check_Target1(_double TimeDelta);
	void							Check_AnimState1(_double TimeDelta);
	void							Check_Times1(_double TimeDelta);
	//for pattern_2
	void							Check_Target2(_double TimeDelta);
	void							Check_AnimState2(_double TimeDelta);
	void							Check_Times2(_double TimeDelta);
	void							Set_Pattern2() { m_ePattern = M_PATTERN::PATTERN_2; }

private:
	void							Update_LeftArm();
	void							Gain_Pattern();
	void							Play_Pattern(_double TimeDelta);
	void							Pattern_1(_double TimeDelta);
	void							Pattern_2(_double TimeDelta);

private:
	HRESULT							Create_Comfetti();


public:
	static	CHumanoid_Clown*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

private:
	_float			m_fDistance = 0.f;
	_float			m_fWalkDistance = 3.f;
	_float			m_fRatio = 0.f;
	_bool			m_bChase = false;
	_uint			m_iAnimIndex = 0;

	// For Culling
	_bool			m_bCulling = false;

	// For Dance 
	_bool			m_bDanceCool = false;
	_double			m_fDanceCoolTime = 0.0;

	// For Turn
	_bool			m_bTurn = false;
	_double			m_dTurningCoolTime = 0.0;
	_double			m_dTurningTime = 0.0;

	// For Effect
	_float4x4		m_LArmPos;
	_bool			m_bCreateOnce1 = false;
	_bool			m_bCreateOnce2 = false;

	// For Test
	_bool			m_bCallHPBarUI = false;
	_double			m_dCallHPBarUITimeDuration = 3.0;

	//for Enum
	M_PATTERN		m_ePattern;
	M_ANIMSTATE		m_eAnimState = M_ANIMSTATE::NONE;
	DANCE_STATE		m_eDanceState = DANCE_STATE::NONE;
};

END

#endif
