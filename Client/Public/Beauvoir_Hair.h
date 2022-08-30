#pragma once

#ifndef __CLIENT_BEAUVOIR_HAIR__
#define __CLIENT_BEAUVOIR_HAIR__

#include "GameObjectModel.h"

BEGIN(Client)

class CBeauvoir_Hair final : public CGameObjectModel
{
private:
	CBeauvoir_Hair(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoir_Hair(const CBeauvoir_Hair& rhs);
	virtual ~CBeauvoir_Hair() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_WorldMatrix(_matrix WorldMatrix);

public:
	virtual void					Notify(void* pMessage);

protected:
	virtual _int					VerifyChecksum(void* pMessage);

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();

public:
	static	CBeauvoir_Hair*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif