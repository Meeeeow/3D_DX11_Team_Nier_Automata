#pragma once
#ifndef __CLIENT_NORMAL_ATTACK_H__
#define __CLIENT_NORMAL_ATTACK_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CNormal_Attack final : public CGameObjectModel
{
public:
	enum class ATTACK : _uint
	{
		PUNCH					 = 1,
		RUSH					 = 2,
		HEAD					 = 3,
		BIGATTACK				 = 4,
		MONINGSTAR				 = 5,
		WHEEL_LEFT				 = 6,
		WHEEL_RIGHT				 = 7,
		WHEEL_BOTH				 = 8,
		ZHUANGZI_BEATING		 = 9,
		ZHUANGZI_SWEEP			 = 10,
		ZHUANGZI_STABTAIL		 = 11,
		ZHUANGZI_SPIKETAIL		 = 12,
		ZHUANGZI_WEAPONTRAIL	 = 13,
		ENGELS_CLAP				 = 14,
		ENGELS_SPIKERIGHTARM	 = 15,
		ENGELS_CONSECUTIVE_SPIKE = 16,
		ENGELS_REACHARM			 = 17,
		ENGELS_REACHARM_LAST	 = 18,
		ENGELS_CONSECUTIVE_SPIKE_END = 19,
		NONE		= 999
	};

private:
	CNormal_Attack(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CNormal_Attack(const CNormal_Attack & rhs);
	virtual ~CNormal_Attack() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void * pArg) override;

	virtual _int			Tick(_double dTimeDelta);
	virtual _int			LateTick(_double dTimeDelta);
	virtual HRESULT			Render();

private:
	void					Check_TimeLimit(_double dTimeDelta);
	HRESULT					Set_Parameter(HUMANOID_ATKINFO* tInfo);

public:
	const ATTACK&			Get_AttackType() const { return m_eAttackType; }
	const _double&			Get_ContinuingTime() const { return m_dContinuingTime; }

public:
	// For Collision
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;

public:
	void					Notify(void * pMessage);

protected:
	_int					VerifyChecksum(void * pMessage);

public:
	static CNormal_Attack * Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CGameObject *	Clone(void * pArg) override;
	virtual void			Free() override;

private:
	ATTACK					m_eAttackType = ATTACK::NONE;
	_double					m_dContinuingTime = 0.0;
	_double					m_dDurationTime = 0.0;
};
END

#endif
