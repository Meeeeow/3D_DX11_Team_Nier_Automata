#include "stdafx.h"
#include "..\Public\Beauvoir_Bullet.h"

CBeauvoir_Bullet::CBeauvoir_Bullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CEffectBullet(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_Bullet::CBeauvoir_Bullet(const CBeauvoir_Bullet & rhs)
	: CEffectBullet(rhs)
{
}

HRESULT CBeauvoir_Bullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_Bullet::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//if (nullptr != pArg)
	//	memcpy(&m_eBulletType, pArg, sizeof(BULLET_TYPE));



	Check_Pause();
	return S_OK;
}

_int CBeauvoir_Bullet::Tick(_double dTimeDelta)
{
	// Create_Explosion would be call by super class
	if (m_eState == OBJSTATE::DEAD)
	{
		if (!m_bCreatedExplosion)
			Create_Explosion();

		return (_int)m_eState;
	}

	//Check_LifeTime(dTimeDelta);
	//Go_Direction(dTimeDelta);
	//m_pNavigation->Update(XMMatrixIdentity());
	//m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());
	//return (_int)m_eState;

	if ((_int)OBJSTATE::DEAD == __super::Tick(dTimeDelta))
	{
		m_eState = OBJSTATE::DEAD;
	}


	if (Is_UnderNavigatoinMesh())
	{
		m_eState = OBJSTATE::DEAD;
	}


	return (_int)OBJSTATE::ENABLE;
}

_int CBeauvoir_Bullet::LateTick(_double dTimeDelta)
{

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CBeauvoir_Bullet::Render()
{
	return S_OK;
}

//void CBeauvoir_Bullet::Check_LifeTime(_double TimeDelta)
//{
//	m_fCurLifeTime += TimeDelta;
//
//	if (m_fCurLifeTime > m_fMaxLifeTime)
//		m_eState = OBJSTATE::DEAD;
//}
//
//void CBeauvoir_Bullet::Go_Direction(_double TimeDelta)
//{
//	if (nullptr != m_pTransform)
//	{
//		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_tMovement.fSpeed);
//		m_pTransform->Jump(TimeDelta);
//
//		_float fY = 0.f;
//		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
//		{
//			_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
//			vPosition = XMVectorSetY(vPosition, fY);
//
//			m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
//			
//			m_eState = OBJSTATE::DEAD;
//		}
//	}
//}
//
//_bool CBeauvoir_Bullet::Is_UnderNavigatoinMesh()
//{
//	_float fY = 0.f;
//	if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
//	{
//		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
//		vPosition = XMVectorSetY(vPosition, fY);
//
//		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
//		return true;
//	}
//	return false;
//}

void CBeauvoir_Bullet::Notify(void * pMessage)
{
}

_int CBeauvoir_Bullet::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_Bullet::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::tagTransformDesc tTransformDesc;
	tTransformDesc.bJump = true;
	tTransformDesc.fJumpPower = BEAUVOIR_BULLET_JUMP;
	tTransformDesc.fJumpTime = 0.0f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	Engine::CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = 0.7f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	return S_OK;
}

CBeauvoir_Bullet * CBeauvoir_Bullet::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_Bullet* pGameInstance = new CBeauvoir_Bullet(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Bullet");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_Bullet::Clone(void * pArg)
{
	CBeauvoir_Bullet* pGameInstance = new CBeauvoir_Bullet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Bullet");
	}

	return pGameInstance;
}

void CBeauvoir_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
}
