#include "..\Public\Transform.h"
#include "Navigation.h"
#include "Cell.h"
#include "Navigation.h"

CTransform::CTransform(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs), m_matWorld(rhs.m_matWorld), m_matOldWorld(rhs.m_matOldWorld)
	, m_tTransformDesc(rhs.m_tTransformDesc)
{

}

HRESULT CTransform::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_matWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&m_matOldWorld, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::NativeConstruct(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_tTransformDesc, pArg, sizeof(CTransform::DESC));

	return S_OK;
}

void CTransform::Move_Front(_double dTimeDelta, CNavigationMesh * pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition += XMVector3Normalize(vLook) * _float(m_tTransformDesc.dSpeedPerSec + m_tTransformDesc.dIncreaseSpeed - m_tTransformDesc.dDecreaseSpeed) * _float(dTimeDelta);

	
	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Back(_double dTimeDelta, CNavigationMesh * pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(STATE::POSITION);
	_vector vLook = Get_State(STATE::LOOK);

	vPosition -= XMVector3Normalize(vLook) * _float(m_tTransformDesc.dSpeedPerSec + m_tTransformDesc.dIncreaseSpeed - m_tTransformDesc.dDecreaseSpeed) * _float(dTimeDelta);


	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Left(_double dTimeDelta, CNavigationMesh * pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition -= XMVector3Normalize(vRight) * _float(m_tTransformDesc.dSpeedPerSec + m_tTransformDesc.dIncreaseSpeed - m_tTransformDesc.dDecreaseSpeed) * _float(dTimeDelta);


	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Right(_double dTimeDelta, CNavigationMesh * pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(STATE::POSITION);
	_vector vRight = Get_State(STATE::RIGHT);

	vPosition += XMVector3Normalize(vRight) * _float(m_tTransformDesc.dSpeedPerSec + m_tTransformDesc.dIncreaseSpeed - m_tTransformDesc.dDecreaseSpeed) * _float(dTimeDelta);


	Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Trace(_double dTimeDelta, _fvector vGoalPos, _float fDistance, CNavigationMesh * pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(STATE::POSITION);

	_vector vUp = Get_State(STATE::UP);
	_vector vDir = vGoalPos - vPosition;
	_vector	vScale = Get_Scale();
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir)) * XMVectorGetX(vScale);

	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp)) * XMVectorGetZ(vScale);

	vPosition += vLook * _float(m_tTransformDesc.dSpeedPerSec + m_tTransformDesc.dIncreaseSpeed - m_tTransformDesc.dDecreaseSpeed) * _float(dTimeDelta);

	if (XMVectorGetX(XMVector4Length(vDir)) >= fDistance)
		Set_State(STATE::POSITION, vPosition);
}

void CTransform::Move_Trace(_double dTimeDelta, CTransform * pTargetTransform, _float fDistance)
{

}

void CTransform::Move_Up(_double dTimeDelta)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(CTransform::STATE::POSITION);
	_vector	vUp = DirectX::XMLoadFloat3(&_float3(0.f, 1.f, 0.f));

	vPosition += XMVector3Normalize(vUp) * _float(m_tTransformDesc.dSpeedPerSec * (_float)dTimeDelta);

	Set_State(CTransform::STATE::POSITION, vPosition);
}

void CTransform::Move_Down(_double dTimeDelta)
{
	_vector	vPosition = Get_State(CTransform::STATE::POSITION);
	_vector	vUp = DirectX::XMLoadFloat3(&_float3(0.f, 1.f, 0.f));

	vPosition -= XMVector3Normalize(vUp) * _float(m_tTransformDesc.dSpeedPerSec * (_float)dTimeDelta);

	Set_State(CTransform::STATE::POSITION, vPosition);
}

void CTransform::Add_Position(_vector vPosition)
{
	m_matOldWorld = m_matWorld;
	_vector vCurrentPosition = Get_State(CTransform::STATE::POSITION);

	Set_State(CTransform::STATE::POSITION, vCurrentPosition += vPosition);
}

void CTransform::Rotation_Axis(_double dTimeDelta, _fvector vAxis)
{
	m_matOldWorld = m_matWorld;
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		matRotation = XMMatrixRotationAxis(vAxis, _float(m_tTransformDesc.dRotationPerSec * dTimeDelta));

	vRight = XMVector4Transform(vRight, matRotation);
	vUp = XMVector4Transform(vUp, matRotation);
	vLook = XMVector4Transform(vLook, matRotation);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation_AxisDegree(_float fDegree, _fvector vAxis)
{
	m_matOldWorld = m_matWorld;
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		matRotation = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fDegree));

	vRight = XMVector4Transform(vRight, matRotation);
	vUp = XMVector4Transform(vUp, matRotation);
	vLook = XMVector4Transform(vLook, matRotation);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation_AxisLerp(_double dTimeDelta, _float fRatio, _fvector vAxis)
{
	m_matOldWorld = m_matWorld;
	_vector		vRight = Get_State(STATE::RIGHT);
	_vector		vUp = Get_State(STATE::UP);
	_vector		vLook = Get_State(STATE::LOOK);

	_matrix		matRotation = XMMatrixRotationAxis(vAxis, (_float)(m_tTransformDesc.dRotationPerSec) * (_float)dTimeDelta);

	_vector		vRotationRight = XMVector3TransformNormal(vRight, matRotation);
	_vector		vRotationUp = XMVector3TransformNormal(vUp, matRotation);
	_vector		vRotationLook = XMVector3TransformNormal(vLook, matRotation);

	vRotationRight = XMQuaternionSlerp(vRight, vRotationRight, fRatio);
	vRotationUp = XMQuaternionSlerp(vUp, vRotationUp, fRatio);
	vRotationLook = XMQuaternionSlerp(vLook, vRotationLook, fRatio);

	Set_State(STATE::RIGHT, vRotationRight);
	Set_State(STATE::UP, vRotationUp);
	Set_State(STATE::LOOK, vRotationLook);
}

_matrix CTransform::Rotation_Axis_Matrix(_double dTimeDelta, _matrix Matrix, _fvector vAxis)
{
	m_matOldWorld = m_matWorld;
	_float4x4 Float4x4;
	XMStoreFloat4x4(&Float4x4, Matrix);

	_vector		vRight = XMVectorZero();
	_vector		vUp = XMVectorZero();
	_vector		vLook = XMVectorZero();

	memcpy(&vRight, &Float4x4.m[0], sizeof(_vector));
	memcpy(&vUp, &Float4x4.m[1], sizeof(_vector));
	memcpy(&vLook, &Float4x4.m[2], sizeof(_vector));

	_matrix		matRotation = XMMatrixRotationAxis(vAxis, _float(m_tTransformDesc.dRotationPerSec * dTimeDelta));

	vRight = XMVector4Transform(vRight, matRotation);
	vUp = XMVector4Transform(vUp, matRotation);
	vLook = XMVector4Transform(vLook, matRotation);

	memcpy(&Float4x4.m[0], &vRight, sizeof(_vector));
	memcpy(&Float4x4.m[1], &vUp, sizeof(_vector));
	memcpy(&Float4x4.m[2], &vLook, sizeof(_vector));

	return XMLoadFloat4x4(&Float4x4);
}

void CTransform::Rotation_Reset()
{
	m_matOldWorld = m_matWorld;
	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Rotation_Degree(const _float3& vRotate)
{
	m_matOldWorld = m_matWorld;
	_matrix		matRotationX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRotate.x));

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	vRight	= XMVector4Transform(vRight, matRotationX);
	vUp		= XMVector4Transform(vUp, matRotationX);
	vLook	= XMVector4Transform(vLook, matRotationX);

	_vector vScale = Get_Scale();

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);
	Set_State(STATE::LOOK, vLook);

	_matrix		matRotationY = XMMatrixRotationAxis(vUp, XMConvertToRadians(vRotate.y));

	vRight = XMVector4Transform(vRight, matRotationY);
	vLook = XMVector4Transform(vLook, matRotationY);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::LOOK, vLook);

	_matrix		matRotationZ = XMMatrixRotationAxis(vLook, XMConvertToRadians(vRotate.z));

	vRight = XMVector4Transform(vRight, matRotationZ);
	vUp = XMVector4Transform(vUp, matRotationZ);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::UP, vUp);

	Set_Scale(vScale);
}

void CTransform::LookAt(CTransform * pTargetTransform)
{
	m_matOldWorld = m_matWorld;
	_vector vTargetPosition = pTargetTransform->Get_State(CTransform::STATE::POSITION);

	_vector vScale = Get_Scale();

	_vector vLook = vTargetPosition - Get_State(CTransform::STATE::POSITION);
	_vector vUp = Get_State(CTransform::STATE::UP);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);
	Set_State(CTransform::STATE::RIGHT, vRight);
	Set_State(CTransform::STATE::UP, vUp);
	Set_State(CTransform::STATE::LOOK, vLook);
}

void CTransform::LookAt(_vector vTargetPosition)
{
	m_matOldWorld = m_matWorld;
	_vector vScale = Get_Scale();

	_vector vLook = vTargetPosition - Get_State(CTransform::STATE::POSITION);
	_vector vUp = Get_State(CTransform::STATE::UP);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);
	Set_State(CTransform::STATE::RIGHT, vRight);
	Set_State(CTransform::STATE::UP, vUp);
	Set_State(CTransform::STATE::LOOK, vLook);
}

void CTransform::LookAt_Direction(_vector _vLook)
{
	m_matOldWorld = m_matWorld;
	_vector vScale = Get_Scale();

	_vector vLook = XMVector3Normalize(_vLook);	
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook)) * XMVectorGetX(vScale);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * XMVectorGetY(vScale);
	vLook = XMVector3Normalize(vLook) * XMVectorGetZ(vScale);

	Set_State(CTransform::STATE::RIGHT, vRight);
	Set_State(CTransform::STATE::UP, vUp);
	Set_State(CTransform::STATE::LOOK, vLook);
}



_float CTransform::Jump(_double TimeDelta, CNavigation* pNavigation)
{
	m_matOldWorld = m_matWorld;
	_float fGravity = GRAVITY;
	m_tTransformDesc.fJumpTime += (_float)TimeDelta;

	_float fValue = m_tTransformDesc.fJumpPower * m_tTransformDesc.fJumpTime + (-0.5f * fGravity * (m_tTransformDesc.fJumpTime * m_tTransformDesc.fJumpTime));

	if (false == m_tTransformDesc.bPowerAtt)
	{
		if (fValue <= 0.00000f)
			fValue *= 0.35f;
	}
	else
		fValue *= 10.0f;


	_vector vPos = Get_State(STATE::POSITION);
	vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fValue);
	Set_State(STATE::POSITION, vPos);
	return XMVectorGetY(vPos);
}

_float CTransform::Fall(_float fFallPos, _double dFallTime)
{
	m_matOldWorld = m_matWorld;
	_float	fY = fFallPos - (0.f * (_float)dFallTime + 0.5f * GRAVITY * (_float)dFallTime * (_float)dFallTime);
	_vector	vPosition = Get_State(STATE::POSITION);
	vPosition = XMVectorSetY(vPosition, fY);

	Set_State(STATE::POSITION, vPosition);

	return fY;
}

void CTransform::Projectile(_float4 vStartPos, _float4 vProjectilePower, _float fProjectileTime, _float fDegree)
{
	m_matOldWorld = m_matWorld;
	_vector vPos = Get_State(CTransform::STATE::POSITION);
	_float4 float4Pos = {};
	XMStoreFloat4(&float4Pos, vPos);

	float4Pos.x = vStartPos.x + vProjectilePower.x * fProjectileTime * cosf(XMConvertToRadians(fDegree));
	float4Pos.z = vStartPos.z + vProjectilePower.z * fProjectileTime * cosf(XMConvertToRadians(fDegree));
	float4Pos.y = vStartPos.y + vProjectilePower.y * fProjectileTime * sinf(XMConvertToRadians(fDegree)) - 0.5f * GRAVITY * fProjectileTime * fProjectileTime;
	float4Pos.w = 1.f;

	vPos = XMLoadFloat4(&float4Pos);

	Set_State(CTransform::STATE::POSITION, vPos);
}

_float4 CTransform::Get_ProjectilePowerToTarget(_float4 vStartPos, _float4 vTargetPos, _float fProjectileMaxTime, _float fDegree)
{
	_float4 vPower;
	vPower.x = (vTargetPos.x - vStartPos.x) / (fProjectileMaxTime * cosf(XMConvertToRadians(fDegree)));

	vPower.z = (vTargetPos.z - vStartPos.z) / (fProjectileMaxTime * cosf(XMConvertToRadians(fDegree)));

	vPower.y = (vTargetPos.y - vStartPos.y + 0.5f * GRAVITY * fProjectileMaxTime * fProjectileMaxTime)
		/ (fProjectileMaxTime * sinf(XMConvertToRadians(fDegree)));
	vPower.w = 0.f;
	return vPower;
}

_fvector CTransform::Get_Scale()
{
	return XMVectorSet(XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT)))
		,XMVectorGetX(XMVector3Length(Get_State(STATE::UP)))
		,XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK)))
		,0.f);
}

_fmatrix CTransform::Get_WorldMatrix() const
{
	return XMLoadFloat4x4(&m_matWorld);
}

_float4x4 CTransform::Get_WorldMatrix4x4() const
{
	return m_matWorld;
}

_float4x4* CTransform::Get_WorldMatrix4x4Ptr()
{
	return &m_matWorld;
}

_float4x4 CTransform::Get_WorldMatrix4x4Transpose() const
{
	_float4x4 matTranspose;

	XMStoreFloat4x4(&matTranspose, XMMatrixTranspose(XMLoadFloat4x4(&m_matWorld)));
	
	return matTranspose;
}

_fmatrix CTransform::Get_OldWorldMatrix() const
{
	return XMLoadFloat4x4(&m_matOldWorld);
}

_float4x4 CTransform::Get_OldWorldMatrix4x4Transpose() const
{
	_float4x4 matTranspose;

	XMStoreFloat4x4(&matTranspose, XMMatrixTranspose(XMLoadFloat4x4(&m_matOldWorld)));

	return matTranspose;
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	m_matOldWorld = m_matWorld;
	_float4		vStateDesc;
	XMStoreFloat4(&vStateDesc, vState);

	memcpy((_float4*)&m_matWorld.m[(_uint)eState][0], &vStateDesc, sizeof(_float4));
}

void CTransform::Set_Scale(STATE eState, _float fScale)
{
	_vector vScale = Get_State(eState);

	Set_State(eState, XMVector4Normalize(vScale) * fScale);
}

void CTransform::Set_Scale(_fvector vScale)
{
	_vector	vRight = Get_State(STATE::RIGHT);
	_vector	vUp = Get_State(STATE::UP);
	_vector	vLook = Get_State(STATE::LOOK);

	Set_State(STATE::RIGHT, XMVector4Normalize(vRight) * XMVectorGetX(vScale));
	Set_State(STATE::UP, XMVector4Normalize(vUp) * XMVectorGetY(vScale));
	Set_State(STATE::LOOK, XMVector4Normalize(vLook) * XMVectorGetZ(vScale));

	//if (!XMVectorGetX(XMVector3Length(vRight)))
	//	Set_State(STATE::RIGHT, XMVector4Normalize(vRight) * XMVectorGetX(vScale));
	//else
	//	Set_State(STATE::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(vScale));

	//if (!XMVectorGetX(XMVector3Length(vUp)))
	//	Set_State(STATE::UP, XMVector4Normalize(vUp) * XMVectorGetY(vScale));
	//else
	//	Set_State(STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetY(vScale));


	//if (!XMVectorGetX(XMVector3Length(vRight)))
	//	Set_State(STATE::LOOK, XMVector4Normalize(vLook) * XMVectorGetZ(vScale));
	//else
	//	Set_State(STATE::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetZ(vScale));

}

void CTransform::Add_Scale(_float fAddScale)
{
	_vector vRight = Get_State(STATE::RIGHT);
	_vector vLook = Get_State(STATE::LOOK);
	
	vRight = XMVectorSetX(vRight, XMVectorGetX(vRight) + fAddScale);
	vLook = XMVectorSetZ(vLook, XMVectorGetZ(vLook) + fAddScale);

	Set_State(STATE::RIGHT, vRight);
	Set_State(STATE::LOOK, vLook);
}

void CTransform::Set_OldMatrix()
{
	m_matOldWorld = m_matWorld;
}

void CTransform::Set_TransformDesc(const DESC & tTransformDesc)
{
	m_tTransformDesc = tTransformDesc;
}

void CTransform::Initialize_JumpTime()
{
	m_tTransformDesc.fJumpTime = 0.0f;
}

void CTransform::Initialize_AttCount()
{
	m_tTransformDesc.iAttCount = 0;
}

void CTransform::Go_Dir(_double TimeDelta, _vector vDir)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(CTransform::STATE::POSITION);

	vPosition += XMVector3Normalize(vDir) * _float(m_tTransformDesc.dSpeedPerSec * TimeDelta);
	Set_State(CTransform::STATE::POSITION, vPosition);
}

void CTransform::Go_DirWithSpeed(_double TimeDelta, _vector vDir, _float fSpeed, CNavigation* pNavigation)
{
	m_matOldWorld = m_matWorld;
	_vector	vPosition = Get_State(CTransform::STATE::POSITION);

	_vector vPrePos = vPosition;
	vPosition += XMVector3Normalize(vDir) * _float(fSpeed * TimeDelta);

 	if (pNavigation)
	{
		CCell* pCell = nullptr;
		if (m_tTransformDesc.bJump == false)
		{
			if (pNavigation->IsOn(vPosition, &pCell))
				Set_State(CTransform::STATE::POSITION, vPosition);
			else
				Set_State(CTransform::STATE::POSITION, vPrePos);
		}
		else
		{
			if (pNavigation->IsJump(vPosition, &pCell))
			{
				Set_State(STATE::POSITION, vPosition);
			}
			else
			{
				if (nullptr == pCell)
				{
					Set_State(CTransform::STATE::POSITION, vPrePos);
				}
				else
				{
					_vector vNormal = pCell->IsJumpSlide(vPrePos);
					_vector vOldLook = Get_State(CTransform::STATE::LOOK);
					_vector vSlideVector = XMVector3Normalize(XMVector3Normalize(vOldLook) - XMVector3Dot(vOldLook, vNormal) * vNormal);
					_float fAngle = XMVectorGetX(XMVector3Dot(vSlideVector, XMVector3Normalize(vOldLook)));

					_vector vSlidePos = vPrePos;
					vSlidePos += XMVector3Normalize(vSlideVector) * fAngle * (_float)TimeDelta * (_float)m_tTransformDesc.dSpeedPerSec;
					if (pNavigation->IsJump(vSlidePos, &pCell))
					{
						Set_State(CTransform::STATE::POSITION, vSlidePos);
					}
					else
					{
						Set_State(CTransform::STATE::POSITION, vPrePos);
					}
				}
			}
		}
	}
	else
		Set_State(CTransform::STATE::POSITION, vPosition);
}

_bool CTransform::IsLowThanCell(CNavigation * pNavigation, _float* pY)
{
	if (nullptr == pNavigation)
		return false;

	_vector vOut = XMVectorZero();
	_vector vPos = Get_State(CTransform::STATE::POSITION);
	
	if (pNavigation->IsLowThanCell(vPos, &vOut))
	{
		if (XMVectorGetY(vPos) < XMVectorGetY(vOut))
		{
			*pY = XMVectorGetY(vOut);
			return true;
		}
	}

	*pY = -1.f;

	return false;
}

_bool CTransform::IsEqualorLow(CNavigation * pNavigation, _float * pY)
{
	if (nullptr == pNavigation)
		return false;

	_vector vOut = XMVectorZero();
	_vector vPos = Get_State(CTransform::STATE::POSITION);

	if (pNavigation->IsLowThanCell(vPos, &vOut))
	{
		if (XMVectorGetY(vPos) <= XMVectorGetY(vOut))
		{
			*pY = XMVectorGetY(vOut);
			return true;
		}
	}

	*pY = -1.f;

	return false;
}

void CTransform::Set_FallTime()
{
	m_tTransformDesc.fJumpTime = ((m_tTransformDesc.fJumpPower * 2) / 9) + m_fGapFallValue;	// 0.41
}

CTransform * CTransform::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CTransform* pInstance = new CTransform(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform* pInstance = new CTransform(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
