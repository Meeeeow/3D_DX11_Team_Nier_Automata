#pragma once
#ifndef __CLIENT_PROJECTION_BASE_H__
#define __CLIENT_PROJECTION_BASE_H__

#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)
class CProjectionBase abstract : public CGameObject
{
protected:
	CProjectionBase(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CProjectionBase(const CProjectionBase& rhs);
	virtual ~CProjectionBase() DEFAULT;

public:
	virtual HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual HRESULT					SetUp_Components();
	virtual HRESULT					SetUp_ConstantTable();

public:
	virtual void			Notify(void* pMessage);

protected:
	virtual _int			VerifyChecksum(void* pMessage);

protected:
	CVIBuffer_Rect*					m_pVIBuffer = nullptr;
	CTransform*						m_pTransform = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	CTexture*						m_pTexture = nullptr;
	_float4x4						m_matProj;

public:
	virtual	void					Free() override;

};
END
#endif // !__CLIENT_PROJECTION_BASE_H__