#pragma once

#ifndef __CLIENT_BEAUVOIR_BULLET__
#define __CLIENT_BEAUVOIR_BULLET__

#include "EffectBullet.h"
BEGIN(Client)

class CBeauvoir_Bullet final : public CEffectBullet
{
private:
	explicit CBeauvoir_Bullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoir_Bullet(const CBeauvoir_Bullet& rhs);
	virtual ~CBeauvoir_Bullet() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();


public:
	virtual void					Notify(void* pMessage);

protected:
	virtual _int					VerifyChecksum(void* pMessage);

private:
	HRESULT							SetUp_Components();

public:
	static	CBeauvoir_Bullet*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif

