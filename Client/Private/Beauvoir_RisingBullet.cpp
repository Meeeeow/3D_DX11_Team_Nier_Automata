#include "stdafx.h"
#include "..\Public\Beauvoir_RisingBullet.h"

CBeauvoir_RisingBullet::CBeauvoir_RisingBullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_RisingBullet::CBeauvoir_RisingBullet(const CBeauvoir_RisingBullet & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoir_RisingBullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_RisingBullet::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_float4x4 WorldFloat4x4 = (*(_float4x4*)pArg);
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&WorldFloat4x4));
	}
	Check_Pause();
	return S_OK;
}

_int CBeauvoir_RisingBullet::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)OBJSTATE::DEAD;

	Check_DeleteTime(dTimeDelta);
	Go_Direction(dTimeDelta);

	m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());

	return (_int)m_eState;
}

_int CBeauvoir_RisingBullet::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CBeauvoir_RisingBullet::Render()
{
	return S_OK;
}

void CBeauvoir_RisingBullet::Check_DeleteTime(_double TimeDelta)
{
	m_DeleteTime += TimeDelta;

	if (m_DeleteTime > 7.0)
		m_eState = OBJSTATE::DEAD;

}

void CBeauvoir_RisingBullet::Go_Direction(_double TimeDelta)
{
	if (nullptr != m_pTransform)
	{
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), 3.f);
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::UP), 0.4f);

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		CCell* pCell = nullptr;
		if (!m_pNavigation->IsOn(vPos, &pCell))
			m_eState = OBJSTATE::DEAD;
	}
}

void CBeauvoir_RisingBullet::Notify(void * pMessage)
{
}

_int CBeauvoir_RisingBullet::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_RisingBullet::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::tagTransformDesc tTransformDesc;
	tTransformDesc.bJump = true;
	tTransformDesc.fJumpPower = JUMPPOWER;
	tTransformDesc.fJumpTime = 0.0f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	Engine::CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = 0.5f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	return S_OK;
}

CBeauvoir_RisingBullet * CBeauvoir_RisingBullet::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_RisingBullet* pGameInstance = new CBeauvoir_RisingBullet(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_RisingBullet");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_RisingBullet::Clone(void * pArg)
{
	CBeauvoir_RisingBullet* pGameInstance = new CBeauvoir_RisingBullet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_RisingBullet");
	}

	return pGameInstance;
}

void CBeauvoir_RisingBullet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
}
