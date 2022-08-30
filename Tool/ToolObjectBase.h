#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
END

class CToolObjectBase abstract : public CGameObject
{
protected:
	CToolObjectBase(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CToolObjectBase(const CToolObjectBase& rhs);
	virtual ~CToolObjectBase() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual _int			Tick(_double dTimeDelta) override;
	virtual _int			LateTick(_double dTimeDelta) override;
	virtual HRESULT			Render() override;

protected:
	HRESULT					SetUp_Components();

protected:
	virtual HRESULT			SetUp_ConstantTable();

protected:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CCollider*				m_pCollider = nullptr;




public:
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;

};

