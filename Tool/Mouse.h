#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CCollider;
class CNavigation;
END

class CMouse final : public CGameObject
{
private:
	CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMouse(const CMouse& rhs);
	virtual ~CMouse() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	const _bool Get_Shift() const {
		return m_bSHIFT;
	}
	_bool		Get_PickingOn() const {
		return m_bPickingOn;
	}
	
	void		Set_PickingOn(_bool bCheck) {
		m_bPickingOn = bCheck;
	}

private:
	CTransform*				m_pTransformCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CCollider*				m_pSphereCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

private:
	_bool					m_bPickingOn = false;
	_bool					m_bSHIFT = false;
	_uint					m_iIndex = 0;

private:
	HRESULT SetUp_Components();

public:
	static CMouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};