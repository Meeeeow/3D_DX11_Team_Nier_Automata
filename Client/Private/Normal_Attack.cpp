#include "stdafx.h"
#include "..\Public\Normal_Attack.h"

CNormal_Attack::CNormal_Attack(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice,pContextDevice)
{
}

CNormal_Attack::CNormal_Attack(const CNormal_Attack & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CNormal_Attack::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CNormal_Attack::NativeConstruct(void * pArg)
{
	HUMANOID_ATKINFO* tInfo = nullptr;

	if (nullptr == pArg)
	{
		return E_FAIL;
	}

	tInfo = (HUMANOID_ATKINFO*)pArg;

	if (FAILED(Set_Parameter(tInfo)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo->Matrix));

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	return S_OK;
}

_int CNormal_Attack::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
	{
		return (_int)m_eState;
	}
	Update_Collider();

	Check_TimeLimit(dTimeDelta);

	return (_int)m_eState;
}

_int CNormal_Attack::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CNormal_Attack::Render()
{
	return S_OK;
}

HRESULT CNormal_Attack::Set_Parameter(HUMANOID_ATKINFO* tInfo)
{
	if (nullptr != tInfo)
	{
		m_eAttackType = (ATTACK)tInfo->iNumber;

		Engine::CCollider::DESC tColliderDesc;

		switch (m_eAttackType)
		{
		case ATTACK::PUNCH:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 301;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 5;
			m_fForce = PLAYER_HIT_RECOVERY + 0.3f;
			tColliderDesc.fRadius = 0.45f;
			break;
		case ATTACK::RUSH:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 302;
			m_dDurationTime = 4.0;
			m_tObjectStatusDesc.iAtt = 3;
			m_fForce = PLAYER_HIT_RECOVERY - 0.1f;
			tColliderDesc.fRadius = 0.9f;
			break;
		case ATTACK::HEAD:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 303;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 7;
			m_fForce = PLAYER_HIT_RECOVERY + 0.5f;
			tColliderDesc.fRadius = 0.9f;
			break;
		case ATTACK::BIGATTACK:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 304;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 10;
			m_fForce = PLAYER_HIT_RECOVERY + 2.f;
			tColliderDesc.fRadius = 3.f;
			break;
		case ATTACK::MONINGSTAR:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 305;
			m_dDurationTime = 4.0;
			m_tObjectStatusDesc.iAtt = 30;
			m_fForce = PLAYER_HIT_RECOVERY + 3.f;
			tColliderDesc.fRadius = 1.f;
			break;
		case ATTACK::WHEEL_LEFT:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 306;
			m_dDurationTime = 4.0;
			m_tObjectStatusDesc.iAtt = 20;
			m_fForce = PLAYER_HIT_RECOVERY + 2.f;
			tColliderDesc.fRadius = 3.5f;
			break;
		case ATTACK::WHEEL_RIGHT:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 307;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 20;
			m_fForce = PLAYER_HIT_RECOVERY + 2.f;
			tColliderDesc.fRadius = 3.5f;
			break;
		case ATTACK::WHEEL_BOTH:
			m_eHitType = HIT::SINGLE;
			m_iTargetAnimation = 308;
			m_dDurationTime = 0.5;
			m_tObjectStatusDesc.iAtt = 40;
			m_fForce = PLAYER_HIT_RECOVERY + 5.f;
			tColliderDesc.fRadius = 8.f;
			break;
		case ATTACK::ZHUANGZI_BEATING:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 2000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 20;
			m_fForce = PLAYER_HIT_RECOVERY + 0.8f;
			tColliderDesc.fRadius = 4.f;
			break;
		case ATTACK::ZHUANGZI_SWEEP:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 2000 + rand() % 1000;
			m_dDurationTime = 4.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.8f;
			tColliderDesc.fRadius = 4.f;
			break;
		case ATTACK::ZHUANGZI_STABTAIL:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 2000 + rand() % 1000;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 60;
			m_fForce = PLAYER_HIT_RECOVERY + 0.8f;
			tColliderDesc.fRadius = 4.f;
			break;
		case ATTACK::ZHUANGZI_SPIKETAIL:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 2000 + rand() % 1000;
			m_dDurationTime = 1.0;
			m_tObjectStatusDesc.iAtt = 70;
			m_fForce = PLAYER_HIT_RECOVERY + 0.8f;
			tColliderDesc.fRadius = 3.f;
			break;
		case ATTACK::ZHUANGZI_WEAPONTRAIL:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 2000 + rand() % 1000;
			m_dDurationTime = 3.0;
			m_tObjectStatusDesc.iAtt = 40;
			m_fForce = PLAYER_HIT_RECOVERY + 0.5f;
			tColliderDesc.fRadius = 1.f;
			break;
		case ATTACK::ENGELS_CLAP:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 15.f;
			break;
		case ATTACK::ENGELS_SPIKERIGHTARM:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 6.f;
			break;
		case ATTACK::ENGELS_CONSECUTIVE_SPIKE:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 6.f;
			break;
		case ATTACK::ENGELS_REACHARM:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 5.f;
			break;
		case ATTACK::ENGELS_REACHARM_LAST:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 15.f;
			break;
		case ATTACK::ENGELS_CONSECUTIVE_SPIKE_END:
			m_eHitType = HIT::MULTI;
			m_iTargetAnimation = 3000 + rand() % 1000;
			m_dDurationTime = 10.0;
			m_tObjectStatusDesc.iAtt = 50;
			m_fForce = PLAYER_HIT_RECOVERY + 0.2f;
			tColliderDesc.fRadius = 15.f;
			break;
		case ATTACK::NONE:
			m_tObjectStatusDesc.iAtt = 0;
			m_fForce = 0.0f;
			break;
		default:
			break;
		}

		if (m_eAttackType < ATTACK::NONE)
		{
			if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
			{
				MSGBOX("Failed To Initialize Normal_Attack");
				return E_FAIL;
			}
			m_bCollision = true;
		}
		return S_OK;
	}
	return E_FAIL;
}

_bool CNormal_Attack::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CNormal_Attack::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		_uint iDamage = m_tObjectStatusDesc.iAtt;

		if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage, true))
		{
			if (m_eHitType == HIT::SINGLE)
			{
				m_bCollision = false;
			}
		}

		/*
		��־����� �⺻������ �ö��̴��� ������ ��ġ�� ������Ʈ �� �� UI ȣ��� ������ ����� CPlayer::Collision()���� ���.
		Collision_Tick() ���� ���� ������ CPlayer::Collision()���� �ϰ������� �ϸ� ������,
		���Ϳ� ���� NormalAttack�� ���� �� �ְ� �׷��� ���� �� �־ ��ĩ ������ ���̰� �� ���輺�� ������.
		���� �÷��̾� �������� Sphere�� �浹���� ���� ó���� ���� �����Ѵ���
		���;��� �������� AABB�� �浹���� �� m_bCollision�� false ó���Ѵٸ� �������� ����
		�� ������ �� ��� ��� ���ð� �÷��̾ �浹�� ������ ������, UI ó������ m_bCollision = false; �� �ǹ����� ��ȿȭ�ǹ���.
		*/
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CNormal_Attack::Check_TimeLimit(_double dTimeDelta)
{

	/*
	- Normal_Attack�� �з����� ���Ű� OBJSTATE ������ �������� Update_NormalAttack�� ���� �ϰ����ְ� ����
	- Normal_Attack�� ������ �� �� �ִ°� �����ڰ� ��� ������ ����ɷ��� ����ߴ���
	  ������ Collision_Tick()���� �������� �� �ֵ��� ������ġ�� �Ŵ� �� ������ ����.
	*/

	if (m_dContinuingTime < m_dDurationTime)
	{
		m_dContinuingTime += dTimeDelta;
	}
	if (m_dDurationTime <= m_dContinuingTime)
	{
		m_bCollision = false;
	}
}

void CNormal_Attack::Notify(void * pMessage)
{
}

_int CNormal_Attack::VerifyChecksum(void * pMessage)
{
	return _int();
}

CNormal_Attack * CNormal_Attack::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CNormal_Attack* pInstance = new CNormal_Attack(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CNormal_Attack::Clone(void * pArg)
{
	CNormal_Attack* pInstance = new CNormal_Attack(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CNormal_Attack::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pAttackBox);
}
