#pragma once
#ifndef __CLIENT_SKY_H__
#define __CLIENT_SKY_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CSky final : public CGameObjectModel
{
private:
	CSky(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CSky(const CSky& rhs);
	virtual ~CSky() DEFAULT;

public:
	static	CSky*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;

private:
	HRESULT					SetUp_Components();

public:
	virtual _int			Tick(_double dTimeDelta);
	virtual _int			LateTick(_double dTimeDelta);
	virtual HRESULT			Render();

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	_float					m_fRotation = 0.f;
};

END

#endif // !__CLIENT_SKY_H__