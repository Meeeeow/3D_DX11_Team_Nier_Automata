#include "stdafx.h"
#include "..\Public\WholeRange.h"

CWholeRange::CWholeRange(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CWholeRange::CWholeRange(const CWholeRange & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CWholeRange::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CWholeRange::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_tObjectStatusDesc, pArg, sizeof(OBJSTATUSDESC));

	m_bWholeRangeAttack = true;
	m_bCollision = false;

	m_eHitType = HIT::SINGLE;
	m_iTargetAnimation = rand() % 1000;

	return S_OK;
}

_int CWholeRange::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());

	return (_int)m_eState;
}

_int CWholeRange::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return (_int)OBJSTATE::ERR;

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return (_int)m_eState;
}

HRESULT CWholeRange::Render()
{
	return S_OK;
}

_bool CWholeRange::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pAttackBox->Collision_SphereToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	return false;
}

void CWholeRange::Collision(CGameObject * pGameObject)
{
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

HRESULT CWholeRange::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	CCollider::tagColliderDesc tColliderDesc;

	tColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	tColliderDesc.fRadius = WHOLERANGERADIUS;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CWholeRange * CWholeRange::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CWholeRange* pGameInstance = new CWholeRange(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CWholeRange");
	}

	return pGameInstance;
}

CGameObject * CWholeRange::Clone(void * pArg)
{
	CWholeRange* pGameInstance = new CWholeRange(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CWholeRange");
	}

	return pGameInstance;
}

void CWholeRange::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pAttackBox);
}
