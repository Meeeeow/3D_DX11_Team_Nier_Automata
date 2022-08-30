#include "stdafx.h"
#include "..\Public\EngelsHitPart.h"
#include "Engels.h"

CEngelsHitPart::CEngelsHitPart(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CEngelsHitPart::CEngelsHitPart(const CEngelsHitPart & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CEngelsHitPart::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEngelsHitPart::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		CCollider::tagColliderDesc tColliderDesc;
		memcpy(&tColliderDesc, pArg, sizeof(CCollider::tagColliderDesc));

		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
			return E_FAIL;
	}

	m_tObjectStatusDesc.fHp = HITPART_TEMP_HP;
	m_tObjectStatusDesc.fMaxHp = HITPART_TEMP_HP;

	m_fPreHP = HITPART_TEMP_HP;

	m_bCollision = true;

	return S_OK;
}

_int CEngelsHitPart::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	Check_Hp();
	Update_Collider();
	Update_CollisionTime(dTimeDelta);

	return (_int)m_eState;
}

_int CEngelsHitPart::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return (_int)OBJSTATE::DEAD;

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	return (_int)m_eState;
}

HRESULT CEngelsHitPart::Render()
{
	return S_OK;
}

void CEngelsHitPart::Check_Hp()
{
	if (m_fPreHP > m_tObjectStatusDesc.fHp)
	{
		_uint iAtt = _uint(m_fPreHP - m_tObjectStatusDesc.fHp);

		CGameInstance* pGameinstance = CGameInstance::Get_Instance();

		CEngels* pEngels = (CEngels*)pGameinstance->Get_GameObjectPtr((_uint)LEVEL::ROBOTGENERAL, L"Boss", 0);
		if (nullptr == pEngels)
		{
			m_eState = OBJSTATE::DEAD;
			return;
		}

		_float fEngelsHP = pEngels->Get_Hp();
		fEngelsHP -= iAtt;

		pEngels->Set_Hp(fEngelsHP);

		m_fPreHP = m_tObjectStatusDesc.fHp;
	}
}

void CEngelsHitPart::Set_WorldMatrix(_matrix WorldMatrix)
{
	if (nullptr == m_pTransform)
		return;

	m_pTransform->Set_WorldMatrix(WorldMatrix);
}

void CEngelsHitPart::Notify(void * pMessage)
{
}

_int CEngelsHitPart::VerifyChecksum(void * pMessage)
{
	return _int();
}

_bool CEngelsHitPart::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::SPHERE)
	{
		if (m_pHitBox->Collision_AABBToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}

	return false;
}

void CEngelsHitPart::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{

	}
	else
	{

	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

HRESULT CEngelsHitPart::Update_UI(_double dDeltaTime)
{
	return S_OK;
}

HRESULT CEngelsHitPart::SetUp_Components()
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

	return S_OK;
}

CEngelsHitPart * CEngelsHitPart::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CEngelsHitPart* pGameInstance = new CEngelsHitPart(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed to Create CEngelsHitPart");
	}

	return pGameInstance;
}

CGameObject * CEngelsHitPart::Clone(void * pArg)
{
	CEngelsHitPart* pGameInstance = new CEngelsHitPart(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed to Create CEngelsHitPart");
	}

	return pGameInstance;
}

void CEngelsHitPart::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pHitBox);
}
