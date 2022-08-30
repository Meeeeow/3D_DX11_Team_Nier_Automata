#pragma once
#ifndef __ENGINE_GAME_OBJECT_H__
#define __ENGINE_GAME_OBJECT_H__

#include "Base.h"
#include "Collision.h"

BEGIN(Engine)
class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() DEFAULT;

public:
	typedef unordered_map<const _tchar*, class CComponent*>		COMPONENTS;

public:
	virtual	HRESULT					NativeConstruct_Prototype();
	virtual HRESULT					NativeConstruct(void* pArg);
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();
	virtual HRESULT					Render_ShadowDepth();
	virtual HRESULT					Render_MotionBlur();
	virtual HRESULT					Render_RimDodge();

public:
	virtual void					Picking();
	virtual _int					Collect_Event();
	virtual void					Event();

public:
	virtual void					Notify(void* pMessage);

protected:
	virtual _int					VerifyChecksum(void* pMessage);

public:
	class CComponent*				Get_Component(const _tchar* pComponentKey);
	virtual const char*				Get_PassName(_uint iIndex);
	virtual _uint					Get_MaxPassIndex();
	_uint							Get_PassIndex() { return m_iPassIndex; }
	const OBJSTATE&					Get_State() const { return m_eState; }
	virtual const char*				Get_BufferContainerPassName(_uint iIndex, _uint iContainerSize);
	virtual _uint					Get_BufferContainerMaxPassIndex(_uint iIndex);
	virtual _uint					Get_BufferContainerSize();
	virtual _uint					Get_BufferContainerPassIndex(_uint iIndex);
	_bool							Get_BufferContainer() { return m_bBuffers; }	
	
	virtual _vector					Get_MinPos();
	virtual _vector					Get_MaxPos();

	const	_float					Get_Alpha() const { return m_fAlpha; }

public:
	void							Set_PassIndex(_uint iPassIndex) { m_iPassIndex = iPassIndex; }
	void							Set_State(const OBJSTATE& _eState) { m_eState = _eState; }
	virtual void					Set_BufferContainerPassIndex(_uint iIndex, _uint iPassIndex);
	void							Set_Collision(_bool bCollision) { m_bCollision = bCollision; }
	void							Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	void							Set_ObjID(_uint iObjID) { m_iObjID = iObjID; }
	_uint							Get_ObjID() { return m_iObjID; }

protected:
	HRESULT							Add_Components(_uint iLevelIndex, const _tchar* pComponentTag, const _tchar* pComponentKey
		, class CComponent** ppOut, void* pArg = nullptr);

	COMPONENTS						m_mapComponents;

protected:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;
	_float							m_fAlpha = 1.0f;

///////////////////////////// Collision /////////////////////////////
public:
	_bool							Get_Collision() { return m_bCollision; }
	virtual _bool					Check_Collision(CGameObject* pGameObject, unsigned int iColliderType);
	virtual void					Collision(CGameObject* pGameObject);

	virtual BoundingBox*			Get_AABB();
	virtual BoundingSphere*			Get_Sphere();
	virtual class CCollider*		Get_ColliderAABB();		// For Check_Collision();
	virtual class CCollider*		Get_ColliderSphere();	// For Check_Collision();
	virtual _fvector                Get_RayOrigin() { return XMVectorZero(); };
	virtual _fvector                Get_RayDirection() { return XMVectorZero(); };
	
	_float							Get_Force() { return m_fForce; }
	HIT								Get_HitType() { return m_eHitType; }
	HIT								Get_HittedType() { return m_eHittedType; }
	const _double					Get_HittedTime() { return m_HittedTime; }
	const _double					Get_HitDelayTime() { return m_HitDelayTime; }
	const _uint						Get_TargetAnimation() const { return m_iTargetAnimation; }
	const _uint						Get_HittedAnimation() const { return m_iHittedAnimation; }

public:
	virtual void					Set_Force(_float3 vDir, _float fValue);
	virtual void					Update_Force(_double TimeDelta, class CNavigation* pNavigation = nullptr) {}
	virtual void					Set_HitDelayTime(_double Time) { m_HitDelayTime = Time; }
	virtual void					Set_TargetAnimation(_uint iIndex) { m_iTargetAnimation = iIndex; }
	virtual void					Set_HittedType(HIT eType) { m_eHittedType = eType; }
	virtual void					Set_HittedTime(_double Time) { m_HittedTime = Time; }
	virtual void					Set_HittedAnimation(_uint iAnimation) { m_iHittedAnimation = iAnimation; }
	virtual void					Update_HitType() {}

protected:
	_uint							m_iFlagCollision = 0x00000000;	//
	_bool							m_bCollision = false;	

protected:
	_float							m_fForce = 0.0f;
	_float							m_fHitRecovery = 0.0f;
	_float3							m_vForceDir = _float3(0.f, 0.f, 0.f);

	HIT								m_eHitType = HIT::SINGLE;
	HIT								m_eHittedType = HIT::SINGLE;
	_double							m_HittedTime = 0.0;
	_double							m_HitDelayTime = 0.0;
	_uint							m_iTargetAnimation = 0;
	_uint							m_iHittedAnimation = 0;

///////////////////////////// UI /////////////////////////////
public:
	const _uint&					Get_DistanceZ() const { return m_iDistanceZ; }	// For Z Sorting

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime);

	_uint							m_iFlagUI = 0x00000000;	
	_uint							m_iObjID = 0;
	_uint							m_iDistanceZ = 0;

protected:
	OBJSTATE						m_eState = OBJSTATE::ENABLE;
	
protected:
	_float							m_fCameraDistance = 0.f;
	_uint							m_iPassIndex = 0;
	_bool							m_bBuffers = false;

public:
	_float							Get_CameraDistance() { return m_fCameraDistance; }
	void							Compute_CamDistanceForAlpha(class CTransform * pTransform);
	void							Compute_CamDistanceForAlpha(class CTransform * pTransform, _float& fDistance);

public:
	const OBJSTATUSDESC				Get_ObjectStatusDesc() const { return m_tObjectStatusDesc; }
	const _float					Get_Hp() const { return m_tObjectStatusDesc.fHp; }
	const _float					Get_MaxHp() const { return m_tObjectStatusDesc.fMaxHp; }
	const _uint						Get_Att() const { return m_tObjectStatusDesc.iAtt; }
	const _uint						Get_RandomAtt() const;
	const _bool						Get_WholeRangeAttack() { return m_bWholeRangeAttack; }

public:
	void							Set_ObjectStatusDesc(OBJSTATUSDESC tDesc) { m_tObjectStatusDesc = tDesc; }
	void							Set_Hp(_float fHp) { m_tObjectStatusDesc.fHp = fHp; }
	void							Set_MaxHp(_float fMaxHp) { m_tObjectStatusDesc.fMaxHp = fMaxHp; }
	void							Set_Att(_uint iAtt) { m_tObjectStatusDesc.iAtt = iAtt; }
	void							Set_Pause(_bool bPause) { m_bPause = bPause; }
	void							Set_MiniGamePause(_bool bPause) { m_bMiniGamePause = bPause; }
	
public:
	void							Set_Focus(_bool bCheck) { m_bFocus = bCheck; }
	void							Initialize_FocusTime() { m_FocusTime = 0.0; }

protected:
	HRESULT							Check_Pause();

protected:
	OBJSTATUSDESC					m_tObjectStatusDesc;

protected:
	_bool							m_bPause = false;

protected:		// For. MiniGame
	_bool							m_bMiniGamePause = false;

protected:
	_bool							m_bWholeRangeAttack = false;

protected:
	_bool							m_bFocus = false;			// Default: false (chase player), true (chase 9s)
	_double							m_FocusTime = 0.0;
	_double							m_FocusDelayTime = 5.0;

public:
	virtual	CGameObject*			Clone(void* pArg) PURE;
	virtual void					Free() override;
};
END
#endif // !__ENGINE_GAME_OBJECT_H__