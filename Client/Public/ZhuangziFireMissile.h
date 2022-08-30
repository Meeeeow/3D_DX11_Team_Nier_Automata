#pragma once

#ifndef __CLIENT_ZHUANGZI_FIREMISSILE__
#define __CLIENT_ZHUANGZI_FIREMISSILE__

#include "GameObjectModel.h"

BEGIN(Client)

class CZhuangziFireMissile final : public CGameObjectModel
{


private:
	explicit CZhuangziFireMissile(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CZhuangziFireMissile(const CZhuangziFireMissile& rhs);
	virtual ~CZhuangziFireMissile() DEFAULT;

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
	_float							m_fSpeed = 9.f;
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
	void							Update_Trail();
	void							Start_Trail();

private:
	HRESULT							SetUp_Components();
	virtual HRESULT					SetUp_ConstantTable() override;
private:
	HRESULT							Create_Explosion();
	_bool							Is_UnderNavigationMesh();

public:
	// For Collision
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;


public:
	static	CZhuangziFireMissile*	Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif