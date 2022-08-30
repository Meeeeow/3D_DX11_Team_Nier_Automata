#pragma once
#ifndef __CLIENT_GAME_OBJECT_MODEL_H__
#define __CLIENT_GAME_OBJECT_MODEL_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCollider;
END

BEGIN(Client)

class CGameObjectModel abstract : public CGameObject
{

protected:
	CGameObjectModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CGameObjectModel(const CGameObjectModel& rhs);
	virtual ~CGameObjectModel() DEFAULT;

public:
	virtual	CGameObject*		Clone(void * pArg) PURE;
	virtual void				Free()							override;
	virtual	HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;

public:
	virtual _int				Tick(_double dTimeDelta)		override;
	virtual _int				LateTick(_double dTimeDelta)	override;
	virtual HRESULT				Render()						override;
	virtual HRESULT				Render_ShadowDepth()			override;
	virtual HRESULT				Render_MotionBlur()				override;
	virtual HRESULT				Render_RimDodge()				override;

protected:
	virtual HRESULT				SetUp_Components();
	virtual HRESULT				SetUp_ConstantTable();
	virtual HRESULT				SetUp_DepthConstantTable();
	virtual HRESULT				SetUp_BlurConstantTable();
	virtual HRESULT				SetUp_RimDodgeConstantTable();

public:
	// For Visual Debugging
	virtual const char*			Get_PassName(_uint iIndex);
	virtual _uint				Get_MaxPassIndex();

protected:
	virtual _bool				Ray_Collision();

public:
	// For Collision
	virtual _bool				Check_Collision(CGameObject* pGameObject, unsigned int iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

	virtual BoundingBox*		Get_AABB()					override;
	virtual BoundingSphere*		Get_Sphere()				override;
	virtual BoundingSphere*		Get_MiddlePoint();
	virtual class CCollider*	Get_ColliderAABB()			override;
	virtual class CCollider*	Get_ColliderSphere()		override;
	virtual class CCollider*	Get_ColliderMiddlePoint();

public:
	virtual void				Update_Force(_double TimeDelta, class CNavigation* pNavigation = nullptr) override;
	virtual void				Update_CollisionTime(_double TimeDelta);
	virtual _float4x4			Get_AtkMatrix() { return m_AttackMatrix; }
	virtual _bool				Get_AtkDead() { return m_bAtkDead; }
	virtual OBJSTATE			Get_ObjState() { return m_eState; }

public:
	void						Set_CurrentMatrix();

public:
	virtual void				Notify(void* pMessage) override;

protected:
	virtual void				Compute_DOF_Parameters();

protected:
	virtual _int				VerifyChecksum(void* pMessage) override;

protected:
	virtual HRESULT				Update_Collider();
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;

protected:
	CRenderer*					m_pRenderer = nullptr;
	CTransform*					m_pTransform = nullptr;
	CModel*						m_pModel = nullptr;
	CNavigation*				m_pNavigation = nullptr;
	
	CCollider*					m_pHitBox = nullptr;		// AABB, For Hit detection 
	CCollider*					m_pAttackBox = nullptr;		// Sphere, For Attack
	CCollider*					m_pMiddlePoint = nullptr;	// Sphere, For Interaction & Debug

	_float4x4					m_AttackMatrix;
	_float4x4					m_matHitPosition;
	_bool						m_bAtkDead = false;

	_float4						m_vDOFParams = _float4(0.f, 75.f, 150.f, 1.f);
};

END
#endif // !__CLIENT_GAME_OBJECT_MODEL_H__