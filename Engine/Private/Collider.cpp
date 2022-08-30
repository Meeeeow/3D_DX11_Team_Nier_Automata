#include "..\Public\Collider.h"
#include "PipeLine.h"
#include "Transform.h"
#include "Navigation.h"
#include "GameObject.h"
#include "Cell.h"

CCollider::CCollider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs), m_eColliderType(rhs.m_eColliderType), m_pBatch(rhs.m_pBatch), m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
{
	
	Safe_AddRef(m_pInputLayout);
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eColliderType)
{
	m_eColliderType = eColliderType;
	if (m_eColliderType == TYPE::NONE)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pGraphicDevice);
	if (m_pEffect == nullptr)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = nullptr;
	size_t			iShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pGraphicDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_pBatch = new DirectX::PrimitiveBatch<DirectX::VertexPositionColor>(m_pContextDevice);
	if (m_pBatch == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_tDesc, pArg, sizeof(DESC));

	switch (m_eColliderType)
	{
	case Engine::CCollider::TYPE::AABB:
		m_pAABB[(_uint)STATE::PIVOT] = new BoundingBox(m_tDesc.vPivot, m_tDesc.vScale);
		m_pAABB[(_uint)STATE::TRANSFORM] = new BoundingBox(*m_pAABB[(_uint)STATE::PIVOT]);
		break;
	case Engine::CCollider::TYPE::OBB:
		m_pOBB[(_uint)STATE::PIVOT] = new BoundingOrientedBox(m_tDesc.vPivot, m_tDesc.vScale, m_tDesc.vOrientation);
		m_pOBB[(_uint)STATE::TRANSFORM] = new BoundingOrientedBox(*m_pOBB[(_uint)STATE::PIVOT]);
		break;
	case Engine::CCollider::TYPE::SPHERE:
		m_pSphere[(_uint)STATE::PIVOT] = new BoundingSphere(m_tDesc.vPivot, m_tDesc.fRadius);
		m_pSphere[(_uint)STATE::TRANSFORM] = new BoundingSphere(*m_pSphere[(_uint)STATE::PIVOT]);
		break;
	default:
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix matWorld)
{
	switch (m_eColliderType)
	{
	case Engine::CCollider::TYPE::AABB:
		m_pAABB[(_uint)STATE::PIVOT]->Transform(*m_pAABB[(_uint)STATE::TRANSFORM], Remove_MatrixRotation(matWorld));
		break;
	case Engine::CCollider::TYPE::OBB:
		m_pOBB[(_uint)STATE::PIVOT]->Transform(*m_pOBB[(_uint)STATE::TRANSFORM], matWorld);
		break;
	case Engine::CCollider::TYPE::SPHERE:
		m_pSphere[(_uint)STATE::PIVOT]->Transform(*m_pSphere[(_uint)STATE::TRANSFORM], matWorld);
		break;
	default:
		break;
	}
}

HRESULT CCollider::Render()
{
	CPipeLine*	pPipeLine = CPipeLine::Get_Instance();
	m_pBatch->Begin();

	m_pContextDevice->IASetInputLayout(m_pInputLayout);

	// View, Proj 행렬 세팅
	m_pEffect->SetView(pPipeLine->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform(CPipeLine::TRANSFORM::D3DTS_PROJ));

	m_pEffect->Apply(m_pContextDevice);

	_vector		vColor = m_bIsCollision == true ? DirectX::Colors::Coral : DirectX::Colors::Azure;

	switch (m_eColliderType)
	{
	case Engine::CCollider::TYPE::AABB:
		DX::Draw(m_pBatch, *m_pAABB[(_uint)STATE::TRANSFORM], vColor);
		break;
	case Engine::CCollider::TYPE::OBB:
		DX::Draw(m_pBatch, *m_pOBB[(_uint)STATE::TRANSFORM], vColor);
		break;
	case Engine::CCollider::TYPE::SPHERE:
		DX::Draw(m_pBatch, *m_pSphere[(_uint)STATE::TRANSFORM], vColor);
		break;
	default:
		break;
	}

	m_pBatch->End();
	return S_OK;
}

_matrix CCollider::Remove_MatrixRotation(_fmatrix matWorld)
{
	// fmatrix가 const이므로 새로 받아서 변환 후 반환 해줘야 한다.

	_matrix			matTransformation;
	matTransformation = matWorld;

	// XMMATRIX는 .r 을 통해 행 접근이 가능하다.
	matTransformation.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(matWorld.r[0]));
	matTransformation.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(matWorld.r[1]));
	matTransformation.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(matWorld.r[2]));

	return matTransformation;
}

_bool CCollider::Collision_Sphere(_fvector vOrigin, _fvector vRay, _float3 & vPos, _float & fDistance)
{
	if (m_pSphere[(_uint)STATE::TRANSFORM]->Intersects(vOrigin, vRay, fDistance))
	{
		vPos = m_pSphere[(_uint)STATE::TRANSFORM]->Center;
		return true;
	}
	return false;
}

_bool CCollider::Collision_OBB(_fvector vOrigin, _fvector vRay, _float & fDistance)
{
	return m_pOBB[(_uint)STATE::TRANSFORM]->Intersects(vOrigin, vRay, fDistance);
}

_bool CCollider::Collision_AABB(_fvector vOrigin, _fvector vRay, _float & fDistance)
{
	_float fDist = false;

	_float4 TempOrigin;
	_float3 TempRay;

	XMStoreFloat4(&TempOrigin, vOrigin);
	XMStoreFloat3(&TempRay, vRay);

	return m_pAABB[(_uint)STATE::TRANSFORM]->Intersects(vOrigin, vRay, fDistance);
}

_bool CCollider::Collision_SphereToSphere(CCollider * pCollider)
{
	return m_pSphere[(_uint)STATE::TRANSFORM]->Intersects(*pCollider->Get_Sphere());
}

_bool CCollider::Collision_SphereToAABB(CCollider * pCollider)
{
	return m_pSphere[(_uint)STATE::TRANSFORM]->Intersects(*pCollider->Get_AABB());
}

_bool CCollider::Collision_AABBToSphere(CCollider * pCollider)
{
	return m_pAABB[(_uint)STATE::TRANSFORM]->Intersects(*pCollider->Get_Sphere());
}

_bool CCollider::Collision_AABBToAABB(CCollider * pCollider)
{
	return m_pAABB[(_uint)STATE::TRANSFORM]->Intersects(*pCollider->Get_AABB());
}

_vector CCollider::Get_WorldPos(TYPE eType)
{
	_vector	vResult;

	switch (eType)
	{
	case Engine::CCollider::TYPE::AABB:
		vResult = XMLoadFloat3(&m_pAABB[(_uint)STATE::TRANSFORM]->Center);
		break;
	case Engine::CCollider::TYPE::OBB:
		vResult = XMLoadFloat3(&m_pOBB[(_uint)STATE::TRANSFORM]->Center);
		break;
	case Engine::CCollider::TYPE::SPHERE:
		vResult = XMLoadFloat3(&m_pSphere[(_uint)STATE::TRANSFORM]->Center);
		break;
	case Engine::CCollider::TYPE::NONE:
		break;
	default:
		break;
	}
	return vResult;
}

BoundingBox* CCollider::Get_AABB_Transform()
{
	return m_pAABB[(_uint)STATE::TRANSFORM];
}

_bool CCollider::Result_AABBToAABB(CCollider * pTargetCollider, CTransform * pTargetTransform, CNavigation* pTargetNavigation)
{
	_vector	vSourMin = Get_Min(), vSourMax = Get_Max();
	_vector vDestMin = pTargetCollider->Get_Min(), vDestMax = pTargetCollider->Get_Max();

	_float3 vRange;
	
	if (max(XMVectorGetX(vSourMin), XMVectorGetX(vDestMin)) > min(XMVectorGetX(vSourMax), XMVectorGetX(vDestMax)))
		return false;

	if (XMVectorGetX(vSourMin) < XMVectorGetX(vDestMin))
		vRange.x = XMVectorGetX(vSourMax - vDestMin);
	else
		vRange.x = XMVectorGetX(vSourMin - vDestMax);

	vRange.y = 0;

	if (max(XMVectorGetZ(vSourMin), XMVectorGetZ(vDestMin)) > min(XMVectorGetZ(vSourMax), XMVectorGetZ(vDestMax)))
		return false;

	if (XMVectorGetZ(vSourMin) < XMVectorGetZ(vDestMin))
		vRange.z = XMVectorGetZ(vSourMax - vDestMin);
	else
		vRange.z = XMVectorGetZ(vSourMin - vDestMax);

	_float fX = fabsf(vRange.x);
	_float fZ = fabsf(vRange.z);
	CCell* pCell = nullptr;

	if (fX >= fZ)
	{
		_vector vPosition = pTargetTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetZ(vPosition, XMVectorGetZ(vPosition) + vRange.z);

		if (pTargetNavigation)
		{
			_vector vOut = vPosition;
			if (pTargetNavigation->IsOn(vOut,&pCell))
			{
				if (!XMVector4Equal(XMVectorZero(), vOut))
				{
					if (pTargetTransform->IsJump())
					{
						vOut = XMVectorSetY(vOut, XMVectorGetY(vPosition));
						pTargetTransform->Set_State(CTransform::STATE::POSITION, vOut);
					}
					else
						pTargetTransform->Set_State(CTransform::STATE::POSITION, vOut);
				}
			}
		}
		else
			pTargetTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	}
	else
	{
		_vector vPosition = pTargetTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetX(vPosition, XMVectorGetX(vPosition) + vRange.x);

		if (pTargetNavigation)
		{
			_vector vOut = vPosition;

			if (pTargetNavigation->IsOn(vOut, &pCell))
			{
				if (!XMVector4Equal(XMVectorZero(), vOut))
				{
					if (pTargetTransform->IsJump())
					{
						vOut = XMVectorSetY(vOut, XMVectorGetY(vPosition));
						pTargetTransform->Set_State(CTransform::STATE::POSITION, vOut);
					}
					else
						pTargetTransform->Set_State(CTransform::STATE::POSITION, vOut);
				}
			}
		}
		else
			pTargetTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	}

	return true;
}

_bool CCollider::Result_SphereToAABB(class CGameObject* pMyObject, CGameObject* pTargetObject, CCollider * pTargetCollider, _uint* pDamage, _bool bMonster)
{
	if (nullptr == m_pSphere[(_uint)STATE::TRANSFORM])
		return false;

	if (nullptr == pTargetCollider->Get_AABB_Transform())
		return false;

	_float	fRadius = m_pSphere[(_uint)STATE::TRANSFORM]->Radius;
	_float3 vCenter = m_pSphere[(_uint)STATE::TRANSFORM]->Center;

	_vector vTargetCenter = pTargetCollider->Get_WorldPos(CCollider::TYPE::AABB);

	_vector vDist = vTargetCenter - XMLoadFloat3(&vCenter);

	_float fDistX = fabs(XMVectorGetX(vDist));
	_float fDistZ = fabs(XMVectorGetZ(vDist));

	_float fSumRadiusX = fRadius + (pTargetCollider->Get_AABB_Transform()->Extents.x);
	_float fSumRadiusZ = fRadius + (pTargetCollider->Get_AABB_Transform()->Extents.z);

	if (fSumRadiusX > fDistX && fSumRadiusZ > fDistZ)
	{
		if (pTargetObject->Get_HittedAnimation() != pMyObject->Get_TargetAnimation())
		{
			pTargetObject->Set_HittedType(pMyObject->Get_HitType());
			pTargetObject->Set_HittedAnimation(pMyObject->Get_TargetAnimation());

			_float3 vDir = _float3(0.f, 0.f, 0.f);

			_vector vTempTargetCenter = XMVectorZero();
			vTempTargetCenter = XMVectorSetY(vTargetCenter, XMVectorGetY(XMLoadFloat3(&vCenter)));

			_vector vTempDist = vTempTargetCenter - XMLoadFloat3(&vCenter);
			XMStoreFloat3(&vDir, XMVector3Normalize(vTempDist));

			pTargetObject->Set_Force(vDir, pMyObject->Get_Force());
			// HP를 깎는다.
			*pDamage = pMyObject->Get_RandomAtt();

			if (!bMonster)
				pTargetCollider->Set_CollisionInitTime(0.0);

			return true;
		}
		else
		{
			if (HIT::MULTI == pTargetObject->Get_HittedType())
			{
				if (pTargetObject->Get_HittedTime() >= pMyObject->Get_HitDelayTime())
				{
					if (!bMonster)
						pTargetObject->Set_HittedTime(0.0);

					_float3 vDir = _float3(0.f, 0.f, 0.f);

					_vector vTempTargetCenter = XMVectorZero();
					vTempTargetCenter = XMVectorSetY(vTargetCenter, XMVectorGetY(XMLoadFloat3(&vCenter)));

					_vector vTempDist = vTempTargetCenter - XMLoadFloat3(&vCenter);
					XMStoreFloat3(&vDir, XMVector3Normalize(vTempDist));

					pTargetObject->Set_Force(vDir, pMyObject->Get_Force());

					// HP를 깎는다.
					*pDamage = pMyObject->Get_RandomAtt();
					return true;
				}
			}
			else if (HIT::SINGLE == pTargetObject->Get_HittedType())
			{
				if (pTargetCollider->Get_CollisionInitTime() >= 1.0)
				{
					if (!bMonster)
						pTargetObject->Set_HittedTime(0.0);

					_float3 vDir = _float3(0.f, 0.f, 0.f);

					_vector vTempTargetCenter = XMVectorZero();
					vTempTargetCenter = XMVectorSetY(vTargetCenter, XMVectorGetY(XMLoadFloat3(&vCenter)));

					_vector vTempDist = vTempTargetCenter - XMLoadFloat3(&vCenter);
					XMStoreFloat3(&vDir, XMVector3Normalize(vTempDist));

					pTargetObject->Set_Force(vDir, pMyObject->Get_Force());

					// HP를 깎는다.
					*pDamage = pMyObject->Get_RandomAtt();

					if (!bMonster)
						pTargetCollider->Set_CollisionInitTime(0.0);

					return true;
				}
			}
		}
	}

	return false;
}

_vector CCollider::Get_Min()
{
	if(m_pAABB[(_uint)STATE::TRANSFORM] == nullptr)
		return XMVectorZero();

	return XMVectorSet(m_pAABB[(_uint)STATE::TRANSFORM]->Center.x - m_pAABB[(_uint)STATE::TRANSFORM]->Extents.x
		, m_pAABB[(_uint)STATE::TRANSFORM]->Center.y - m_pAABB[(_uint)STATE::TRANSFORM]->Extents.y
		, m_pAABB[(_uint)STATE::TRANSFORM]->Center.z - m_pAABB[(_uint)STATE::TRANSFORM]->Extents.z
		, 0.f);
}

_vector CCollider::Get_Max()
{
	if (m_pAABB[(_uint)STATE::TRANSFORM] == nullptr)
		return XMVectorZero();

	return XMVectorSet(m_pAABB[(_uint)STATE::TRANSFORM]->Center.x + m_pAABB[(_uint)STATE::TRANSFORM]->Extents.x
		, m_pAABB[(_uint)STATE::TRANSFORM]->Center.y + m_pAABB[(_uint)STATE::TRANSFORM]->Extents.y
		, m_pAABB[(_uint)STATE::TRANSFORM]->Center.z + m_pAABB[(_uint)STATE::TRANSFORM]->Extents.z
		, 0.f);
}

void CCollider::Set_ColliderDesc(tagColliderDesc _tColliderDesc)
{
	Safe_Delete(m_pAABB[(_uint)STATE::PIVOT]);
	Safe_Delete(m_pAABB[(_uint)STATE::TRANSFORM]);

	memcpy(&m_tDesc, &_tColliderDesc, sizeof(tagColliderDesc));

	m_pAABB[(_uint)STATE::PIVOT] = new BoundingBox(m_tDesc.vPivot, m_tDesc.vScale);
	m_pAABB[(_uint)STATE::TRANSFORM] = new BoundingBox(*m_pAABB[(_uint)STATE::PIVOT]);
}

CCollider * CCollider::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, TYPE eColliderType)
{
	CCollider* pInstance = new CCollider(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(eColliderType)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider* pInstnace = new CCollider(*this);
	if (FAILED(pInstnace->NativeConstruct(pArg)))
		Safe_Release(pInstnace);

	return pInstnace;
}

void CCollider::Free()
{
	__super::Free();

	if (m_bCloned == false)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	for (_uint i = 0; i < (_uint)STATE::NONE; ++i)
	{
		Safe_Delete(m_pAABB[i]);
		Safe_Delete(m_pOBB[i]);
		Safe_Delete(m_pSphere[i]);
	}

	Safe_Release(m_pInputLayout);
}
