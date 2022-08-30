#pragma once
#ifndef __CLIENT_CLOWN_FLY__
#define __CLIENT_CLOWN_FLY__

#include "GameObjectModel.h"

BEGIN(Client)
class CClown_Fly final : public CGameObjectModel
{
public:
	static	_uint	iObjIDAcc;

public:
	enum class M_ANIMSTATE : _uint
	{
		IDLE = 15,
		ATTACK = 26,
		STUN = 10,
		HIT = 13,
		FLOAT_UP = 20,
		FLOAT_DOWN = 21,
		NONE = 999
	};

public:
	enum class SIDE : _uint
	{
		LEFT = 1,
		RIGHT = 2,
		NONE = 999
	};

public:
	enum class WAVE : _uint
	{
		WAVE_0 = 0,
		WAVE_1 = 1,
		WAVE_2 = 2,
		NONE = 999
	};

private:
	explicit CClown_Fly(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CClown_Fly(const CClown_Fly& rhs);
	virtual ~CClown_Fly() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct_Prototype() override;
	virtual HRESULT						NativeConstruct(void* pArg) override;
	virtual _int						Tick(_double dTimeDelta);
	virtual _int						LateTick(_double dTimeDelta);
	virtual HRESULT						Render();

private:
	HRESULT								SetUp_Components();
	HRESULT								SetUp_ConstantTable();
	HRESULT								Set_Position_From_Coaster_First();
	HRESULT								Set_Position_From_Coaster_Floating(_double dTimeDelta);
	void								Check_Target(_double TimeDelta);
	void								Check_AnimState1(_double TimeDelta);
	void								Check_Times1(_double TimeDelta);	

private:
	HRESULT								Shoot_Bullet();

public:
	virtual _bool						Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void						Collision(CGameObject* pGameObject) override;
	CTransform*							Get_TransformCom() { return m_pTransform; }
	void								Set_Wave(WAVE _Wave) { m_eWave = _Wave; }
	void								Set_Start_False() { m_bStart = false; }
	void								Set_Start_True() { m_bStart = true; }
	WAVE								Get_Wave() { return m_eWave; }

public:
	virtual void						Notify(void* pMessage)			override;

protected:
	virtual _int						VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT						Update_UI(_double dDeltaTime)	override;

public:
	HRESULT								Create_DeathEffect();

public:
	static	CClown_Fly*					Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;

public:
	static int		m_iPatternNumber;

private:
	M_ANIMSTATE		m_eAnimState = M_ANIMSTATE::NONE;
	M_ANIMSTATE		m_eLastAnimState = M_ANIMSTATE::NONE;

	// For Test
	_bool			m_bCallHPBarUI = false;
	_double			m_dCallHPBarUITimeDuration = 3.0;

	_uint			m_iAnimIndex = 0;
	_uint			m_iLastAnimIndex = 0;

	_bool			m_bStart = false;

	SIDE			m_eSide = SIDE::NONE;

	_bool			m_bOnce = false;

	_float			m_fDistance = 0.f;

	_double			m_dBulletCoolTime = 0.0;
	_double			m_dBulletCoolDownTime = 0.0;
	_bool			m_bFireCool = false;
	_bool			m_bFireOnce = false;

	_float4			m_vecBulletLook;

	WAVE			m_eWave = WAVE::NONE;

	_float			m_fXDistanceL = 0.f;
	_float			m_fXDistanceR = 0.f;


};
END

#endif

