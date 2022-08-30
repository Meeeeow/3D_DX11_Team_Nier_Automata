#pragma once

#ifndef __CLIENT_ENGELS_HITPART_H__
#define __CLIENT_ENGELS_HITPART_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CEngelsHitPart final : public CGameObjectModel
{
public:
	CEngelsHitPart(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CEngelsHitPart(const CEngelsHitPart& rhs);
	virtual ~CEngelsHitPart() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta) override;
	virtual _int				LateTick(_double dTimeDelta) override;
	virtual HRESULT				Render() override;

public:
	void						Check_Hp();

public:
	void						Set_WorldMatrix(_matrix WorldMatrix);

public:
	virtual void				Notify(void* pMessage);

public:
	virtual _int				VerifyChecksum(void* pMessage);

public:
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

public:
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;

private:
	_float						m_fPreHP = 0.0f;

private:
	HRESULT						SetUp_Components();

public:
	static	CEngelsHitPart*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

static const _float				HITPART_TEMP_HP = 100000.f;

END

#endif