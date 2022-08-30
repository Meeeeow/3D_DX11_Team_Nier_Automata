#pragma once
#ifndef __ENGINE_COLLIDER_H__
#define __ENGINE_COLLIDER_H__


#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)
class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() DEFAULT;

public:
	enum class TYPE : _uint {
		AABB			= 0,
		OBB				= 1,
		SPHERE			= 2,
		RAY				= 3,
		CIRCLE			= 4,
		DONUT0			= 5,
		DONUT1			= 6,
		DONUT2			= 7,
		BEAUVOIR_RING	= 8,
		ZHUANGZI_RING	= 9,
		NONE			= 10
	};

	enum class STATE : _uint {
		PIVOT			= 0,
		TRANSFORM		= 1,
		NONE			= 2
	};

public:
	typedef struct tagColliderDesc {
		tagColliderDesc() {
			ZeroMemory(this, sizeof(tagColliderDesc));
		}
		_float3			vScale;
		_float3			vPivot;
		_float4			vOrientation;
		_float			fRadius;
	}DESC;

	typedef DirectX::PrimitiveBatch<DirectX::VertexPositionColor> BATCH;

public:
	virtual	HRESULT					NativeConstruct_Prototype(TYPE eColliderType);
	virtual HRESULT					NativeConstruct(void* pArg) override;
	void							Update(_fmatrix matWorld);
	HRESULT							Render();
	void							Update_CollisionTime(_double Time) { m_CollisionInitTime += Time; }

private:
	_matrix							Remove_MatrixRotation(_fmatrix matWorld);
	
public: /* Ray Collision */
	_bool							Collision_Sphere(_fvector vOrigin, _fvector vRay, _float3& vPos, _float& fDistance);
	_bool							Collision_OBB(_fvector vOrigin, _fvector vRay, _float& fDistance);
	_bool							Collision_AABB(_fvector vOrigin, _fvector vRay, _float& fDistance);

public: /* Collision to Collider */
	_bool							Collision_SphereToSphere(CCollider* pCollider);
	_bool							Collision_SphereToAABB(CCollider* pCollider);
	_bool							Collision_AABBToSphere(CCollider* pCollider);
	_bool							Collision_AABBToAABB(CCollider* pCollider);

public:
	const _double					Get_CollisionInitTime() const { return m_CollisionInitTime; }

public:
	void							Set_CollisionInitTime(_double Time) { m_CollisionInitTime = Time; }

public:
	_vector							Get_WorldPos(TYPE eType);
	BoundingBox*					Get_AABB_Transform();

public: /* For AABB to AABB Collision Result */
	_bool							Result_AABBToAABB(CCollider* pTargetCollider, class CTransform* pTargetTransform, class CNavigation* pTargetNavigation = nullptr);
	_bool							Result_SphereToAABB(class CGameObject* pMyObject, class CGameObject* pTargetObject, CCollider * pTargetCollider, _uint* pDamage, _bool bMonster = false);

public: /* Get Collider */
	BoundingOrientedBox*			Get_OBB() { return m_pOBB[(_uint)STATE::TRANSFORM]; }
	BoundingBox*					Get_AABB() { return m_pAABB[(_uint)STATE::TRANSFORM]; }
	_vector							Get_Min();
	_vector							Get_Max();
	BoundingSphere*					Get_Sphere() { return m_pSphere[(_uint)STATE::TRANSFORM]; }

public:
	void Set_ColliderDesc(tagColliderDesc _tColliderDesc);
	tagColliderDesc Get_ColliderDesc() { return m_tDesc; }

private:
	TYPE							m_eColliderType = TYPE::NONE;
	DESC							m_tDesc;
	_bool							m_bIsCollision = false;

	BoundingBox*					m_pAABB[(_uint)STATE::NONE] = { nullptr };
	BoundingOrientedBox*			m_pOBB[(_uint)STATE::NONE] = { nullptr };
	BoundingSphere*					m_pSphere[(_uint)STATE::NONE] = { nullptr };

private:
	BasicEffect*					m_pEffect = nullptr;
	ID3D11InputLayout*				m_pInputLayout = nullptr;
	BATCH*							m_pBatch = nullptr;

private:
	_double							m_CollisionInitTime = 0.0;

public:
	static	CCollider*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, TYPE eColliderType);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;

};

END
#endif // !__ENGINE_COLLIDER_H__