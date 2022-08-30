#pragma once

#ifndef __CLIENT_BEAUVOIR_NAVIBULLET__
#define __CLIENT_BEAUVOIR_NAVIBULLET__

#include "GameObjectModel.h"

BEGIN(Client)

class CBeauvoir_NaviBullet final : public CGameObjectModel
{
private:
	CBeauvoir_NaviBullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoir_NaviBullet(const CBeauvoir_NaviBullet& rhs);
	virtual ~CBeauvoir_NaviBullet() DEFAULT;

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
	_float							m_fSpeed = 3.f;
	_float3							m_vLerpLook = _float3(0.f, 0.f, 0.f);
	_float4							m_vTargetPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double							m_FirstSlowTime = 0.0;

private:
	HRESULT							SetUp_Components();

public:
	static	CBeauvoir_NaviBullet*	Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif

