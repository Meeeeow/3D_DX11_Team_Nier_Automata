#include "stdafx.h"
#include "..\Public\Humanoid_Bullet.h"

CHumanoid_Bullet::CHumanoid_Bullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CHumanoid_Bullet::CHumanoid_Bullet(const CHumanoid_Bullet & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CHumanoid_Bullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHumanoid_Bullet::NativeConstruct(void * pArg)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	M_BULLETINFO* tInfo = (M_BULLETINFO*)pArg;

	if (tInfo->bLaser == true)
		m_eBulletType = BULLET::LASER;
	else
		m_eBulletType = BULLET::NORMAL;
	
	_float4x4 Matrix = tInfo->Matrix;

	Set_Transform(Matrix);
	Set_Target();
	Change_Transform();
	Set_Collider();

	return S_OK;
}

_int CHumanoid_Bullet::Tick(_double dTimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
	{
		return (_int)m_eState;
	}

	if (m_eBulletType == BULLET::NORMAL)
	{
		
		m_dContinuingTime += dTimeDelta;
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

		vLook = XMVector3Normalize(vLook);

		vPos = vPos + (vLook * (_float)dTimeDelta * m_fSpeed);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

		m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());

	}
	if (m_eBulletType == BULLET::LASER)
	{
		m_dContinuingTime += dTimeDelta * 0.5;
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

		vLook = XMVector3Normalize(vLook);

		vPos = vPos + (vLook * (_float)dTimeDelta * m_fSpeed * 1.2f);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

		m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());
	}

	return (_int)m_eState;
}

_int CHumanoid_Bullet::LateTick(_double dTimeDelta)
{
	if (m_eBulletType == BULLET::NORMAL)
	{
		if (m_dContinuingTime >= 3.0)
			m_eState = OBJSTATE::DEAD;
	
			m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	}
	else
	{
		if (m_dContinuingTime >= 3.0)
			m_eState = OBJSTATE::DEAD;

			m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	}
	


	return 0;
}

HRESULT CHumanoid_Bullet::Render()
{
	return S_OK;
}

void CHumanoid_Bullet::Set_Transform(_float4x4 Matrix)
{
	_float4 vRight, vUp , vLook , vPos;
	memcpy(&vRight, &Matrix.m[0], sizeof(_float4));
	memcpy(&vUp, &Matrix.m[1], sizeof(_float4));
	memcpy(&vLook, &Matrix.m[2], sizeof(_float4));
	memcpy(&vPos, &Matrix.m[3], sizeof(_float4));

	_vector vecRight, vecUp, vecLook, vecPos;
	vecRight = XMLoadFloat4(&vRight);
	vecUp = XMLoadFloat4(&vUp);
	vecLook = XMLoadFloat4(&vLook);
	vecPos = XMLoadFloat4(&vPos);


	vecRight = XMVector3Normalize(vecRight);
	vecUp = XMVector3Normalize(vecUp);
	vecLook = XMVector3Normalize(vecLook);
	if (m_eBulletType == BULLET::NORMAL)
		vecPos = vecPos + vecUp * 0.3f;
	else
		vecPos = vecPos + vecUp * 0.3f;

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vecRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vecUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vecLook);	
	m_pTransform->Set_State(CTransform::STATE::POSITION, vecPos);
}

HRESULT CHumanoid_Bullet::Set_Collider()
{
	if (m_eBulletType == BULLET::NORMAL)
	{
		Engine::CCollider::DESC tColliderDesc;
		tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
		tColliderDesc.fRadius = 0.3f;

		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
			return E_FAIL;
	}
	else
	{
		Engine::CCollider::DESC tColliderDesc;
		tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
		tColliderDesc.fRadius = 1.f;

		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
			return E_FAIL;
	}
	return S_OK;
}

void CHumanoid_Bullet::Set_Target()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pPlayerTransformCom = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	RELEASE_INSTANCE(CGameInstance);
}

void CHumanoid_Bullet::Change_Transform()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vBulletPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float fPlayerY = XMVectorGetY(vPlayerPos);
	vBulletPos = XMVectorSetY(vBulletPos, fPlayerY);
	_vector vUp, vRight, vLook;

	_vector vScale = m_pTransform->Get_Scale();

	vLook = vPlayerPos - vBulletPos;
	vLook = XMVector3Normalize(vLook);
	vUp = { 0.f , 1.f , 0.f , 0.f };

	vRight = XMVector3Cross(vUp, vLook);
	vUp = XMVector3Cross(vLook, vRight);

	vRight = vRight * XMVectorGetX(vScale);
	vUp = vUp * XMVectorGetY(vScale);
	vLook = vLook * XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vUp);

}

void CHumanoid_Bullet::Notify(void * pMessage)
{
}

_int CHumanoid_Bullet::VerifyChecksum(void * pMessage)
{
	return _int();
}

CHumanoid_Bullet * CHumanoid_Bullet::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CHumanoid_Bullet* pInstance = new CHumanoid_Bullet(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CHumanoid_Bullet::Clone(void * pArg)
{
	CHumanoid_Bullet* pInstance = new CHumanoid_Bullet(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHumanoid_Bullet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pAttackBox);
}
