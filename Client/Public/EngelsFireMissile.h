#pragma once

#ifndef __CLIENT_ENGELSFIREMISSILE_H__
#define __CLIENT_ENGELSFIREMISSILE_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CEngelsFireMissile final : public CGameObjectModel
{
public:
	CEngelsFireMissile(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CEngelsFireMissile(const CEngelsFireMissile& rhs);
	virtual ~CEngelsFireMissile() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_Speed(_float fSpeed) { m_fSpeed = fSpeed; }
	void							Set_TargetPos(_vector vPos) { XMStoreFloat4(&m_vTargetPos, vPos); }

public:
	void							Check_DeleteTime(_double TimeDelta);
	void							Go_Direction(_double TimeDelta);

public:
	virtual void					Notify(void* pMessage);

protected:
	virtual _int					VerifyChecksum(void* pMessage);

private:
	_double							m_DeleteTime = 0.0;
	_double							m_LookLerpTime = 0.0;
	_float							m_fSpeed = 20.f;
	_float3							m_vLerpLook = _float3(0.f, 0.f, 0.f);
	_float4							m_vTargetPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double							m_FirstSlowTime = 0.0;
	_float							m_fRandomY = 0.f;

private:
	_bool							m_bLerpStop = false;

private:
	_bool							m_bMonsterAttack = false;

private:
	CGameObject*					m_pTrail = nullptr;

private:
	static _float3					m_vFarPoint1;
	static _float3					m_vFarPoint2;
	static _bool					m_bIsFarPointInit;

private:
	static void						Compute_FarTwoPoint();
	void							Update_Trail();
	void							Start_Trail();

private:
	HRESULT							SetUp_Components();
	virtual HRESULT					SetUp_ConstantTable() override;

private:
	HRESULT							Update_Effects(_double _dTimeDelta);

private:
	HRESULT							Create_Explosion();
	_bool							Is_UnderNavigationMesh();

public:
	// For Collision
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;


public:
	static	CEngelsFireMissile*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif