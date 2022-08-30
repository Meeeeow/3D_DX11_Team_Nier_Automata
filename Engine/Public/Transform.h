#pragma once
#ifndef __ENGINE_TRANSFORM_H__
#define __ENGINE_TRANSFORM_H__

#include "Component.h"
BEGIN(Engine)
class ENGINE_DLL CTransform final : public CComponent
{
private:
	CTransform(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() DEFAULT;

public:
	enum class STATE : _uint {
		RIGHT		= 0,
		UP			= 1,
		LOOK		= 2,
		POSITION	= 3
	};

	typedef struct tagTransformDesc {
		tagTransformDesc() {
			ZeroMemory(this, sizeof(tagTransformDesc));
		}
		_double				dSpeedPerSec;
		_double				dIncreaseSpeed;
		_double				dDecreaseSpeed;
		_double				dRotationPerSec;

		_float				fJumpPower;
		_float				fJumpTime;
		_uint				iAttCount;
		_bool				bJump;
		_bool				bPowerAtt;
	}DESC;

public:
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;

public:
	void					Move_Front(_double dTimeDelta, class CNavigationMesh* pNavigation = nullptr);
	void					Move_Back(_double dTimeDelta, class CNavigationMesh* pNavigation = nullptr);
	void					Move_Left(_double dTimeDelta, class CNavigationMesh* pNavigation = nullptr);
	void					Move_Right(_double dTimeDelta, class CNavigationMesh* pNavigation = nullptr);

	void					Move_Trace(_double dTimeDelta, _fvector vGoalPos, _float fDistance = 1.f , class CNavigationMesh* pNavigation = nullptr);
	void					Move_Trace(_double dTimeDelta, CTransform* pTargetTransform, _float fDistance = 1.f);

	void					Move_Up(_double dTimeDelta);
	void					Move_Down(_double dTimeDelta);

	void					Add_Position(_vector vPosition);

	void					Rotation_Axis(_double dTimeDelta, _fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f));
	void					Rotation_AxisDegree(_float fDegree, _fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f));
	void					Rotation_AxisLerp(_double dTimeDelta, _float fRatio, _fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f));

	_matrix					Rotation_Axis_Matrix(_double dTimeDelta, _matrix Matrix, _fvector vAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f));

	void					Rotation_Reset();
	void					Rotation_Degree(const _float3& vRotate);

	void					LookAt(CTransform* pTargetTransform);
	void					LookAt(_vector vTargetPosition);
	void					LookAt_Direction(_vector vLook);

	_float					Jump(_double TimeDelta, class CNavigation* pNavigation = nullptr);
	_float					Fall(_float fFallPos, _double dFallTime);

	_float4					Get_ProjectilePowerToTarget(_float4 vStartPos, _float4 vTargetPos, _float dProjectileMaxTime, _float fDegree);
	void					Projectile(_float4 vStartPos, _float4 vProjectilePower, _float dProjectileTime, _float fDegree);

public: // Get
	_fvector				Get_State(STATE eState) {	return XMLoadFloat4((_float4*)&m_matWorld.m[(_uint)eState][0]);	}
	_fvector				Get_Scale();

	_fmatrix				Get_WorldMatrix() const;
	_float4x4				Get_WorldMatrix4x4() const;
	_float4x4*				Get_WorldMatrix4x4Ptr();
	_float4x4				Get_WorldMatrix4x4Transpose() const;
	_fmatrix				Get_OldWorldMatrix() const;
	_float4x4				Get_OldWorldMatrix4x4Transpose() const;

	_double					Get_Speed() const { return m_tTransformDesc.dSpeedPerSec; }
	_double					Get_IncreaseSpeed() const { return m_tTransformDesc.dIncreaseSpeed; }
	_double					Get_DecreaseSpeed() const { return m_tTransformDesc.dDecreaseSpeed; }
	_double					Get_RotationSpeed() const { return m_tTransformDesc.dRotationPerSec; }

	tagTransformDesc		Get_TransformDesc() const { 
		return m_tTransformDesc; 
	}

	_uint					Get_AttCount() const { return m_tTransformDesc.iAttCount; }
	_bool					Get_PowerAtt() const { return m_tTransformDesc.bPowerAtt; }
	_bool					IsJump() const { return m_tTransformDesc.bJump; }
	_float					Get_TopJumpTime() const { return ((m_tTransformDesc.fJumpPower * 2) / 9); }
	_float					Get_JumpTime() const { return m_tTransformDesc.fJumpTime; }
	_float					Get_FallTime() const { return ((m_tTransformDesc.fJumpPower * 2) / 9) + m_fGapFallValue; }



public: // Set
	void					Set_State(STATE eState, _fvector vState);

	void					Set_Scale(STATE eState, _float fScale);
	void					Set_Scale(_fvector vScale);
	void					Add_Scale(_float fAddScale);
	void					Set_OldMatrix();

	void					Set_TransformDesc(const DESC& tTransformDesc);
	void					Set_Speed(_double dSpeedPerSec) { m_tTransformDesc.dSpeedPerSec = dSpeedPerSec; }
	void					Set_Increase(_double dIncrease) { m_tTransformDesc.dIncreaseSpeed = dIncrease; }
	void					Set_Decrease(_double dDecrease) { m_tTransformDesc.dDecreaseSpeed = dDecrease; }
	void					Set_RotationSpeed(_double dRotationSpeed) { m_tTransformDesc.dRotationPerSec = dRotationSpeed; }

	void					Initialize_JumpTime();
	void					Initialize_AttCount();

	void					Set_WorldMatrix(_fmatrix matWorld) { XMStoreFloat4x4(&m_matWorld, matWorld); }
	void					Add_AttCount() { ++m_tTransformDesc.iAttCount; }
	void					Set_FallTime();
	void					Set_Jump(_bool bCheck) { m_tTransformDesc.bJump = bCheck; }
	void					Set_PowerAtt(_bool bCheck) { m_tTransformDesc.bPowerAtt = bCheck; }
	void					Set_JumpPower(_float fPower) { m_tTransformDesc.fJumpPower = fPower; }
	void					Set_JumpTime(_float fTime) { m_tTransformDesc.fJumpTime = fTime; }

public:
	void					Go_Dir(_double TimeDelta, _vector vDir);
	void					Go_DirWithSpeed(_double TimeDelta, _vector vDir, _float fSpeed, class CNavigation* pNavigation = nullptr);
	_bool					IsLowThanCell(class CNavigation* pNavigation, _float* pY);
	_bool					IsEqualorLow(class CNavigation* pNavigation, _float* pY);

private:
	_float4x4				m_matOldWorld;
	_float4x4				m_matWorld;
	DESC					m_tTransformDesc;
	const _float			m_fGapFallValue = 0.022f;

public:
	static	CTransform*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;

};
END
#endif // !__ENGINE_TRANSFORM_H__