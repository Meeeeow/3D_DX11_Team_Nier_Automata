#pragma once

#ifndef __CLIENT_BEAUVOIR_SKIRT__
#define __CLIENT_BEAUVOIR_SKIRT__

#include "GameObjectModel.h"

BEGIN(Client)

class CBeauvoir_Skirt final : public CGameObjectModel
{
private:
	CBeauvoir_Skirt(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CBeauvoir_Skirt(const CBeauvoir_Skirt& rhs);
	virtual ~CBeauvoir_Skirt() DEFAULT;

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

private:
	_bool							m_bIsDissolve = false;
	_float							m_fDissolveValue = 0.f;
	_float							m_fDissolveSpeed = 0.f;

private:
	_bool							m_bIsEnd = false;
public:
	const _bool&					Get_IsEnd() const {
		return m_bIsEnd;
	}

private:
	CTexture*						m_pDissolveTexture = nullptr;

public:
	void							Set_DissolveTrue() {
		m_bIsDissolve = true;
		m_iPassIndex = static_cast<_int>(MESH_PASS_INDEX::NONANIM_DISSOLVE);
	}


public:
	static	CBeauvoir_Skirt*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};

END

#endif