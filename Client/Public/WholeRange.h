#pragma once

#ifndef __CLIENT_WHOLERANGE_H__
#define __CLIENT_WHOLERANGE_H__

#include "GameObjectModel.h"

BEGIN(Client)

class CWholeRange final : public CGameObjectModel
{
private:
	CWholeRange(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CWholeRange(const CWholeRange& rhs);
	virtual ~CWholeRange() = default;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

private:
	HRESULT							SetUp_Components();

public:
	static	CWholeRange*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif