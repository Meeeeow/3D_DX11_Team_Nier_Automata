#include "stdafx.h"
#include "..\Public\Beauvoir_NaviBullet.h"

CBeauvoir_NaviBullet::CBeauvoir_NaviBullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_NaviBullet::CBeauvoir_NaviBullet(const CBeauvoir_NaviBullet & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoir_NaviBullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_NaviBullet::NativeConstruct(void * pArg)
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

_int CBeauvoir_NaviBullet::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)OBJSTATE::DEAD;

	Check_DeleteTime(dTimeDelta);
	Go_Direction(dTimeDelta);

	m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());

	return (_int)m_eState;
}

_int CBeauvoir_NaviBullet::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CBeauvoir_NaviBullet::Render()
{
	return S_OK;
}

void CBeauvoir_NaviBullet::Check_DeleteTime(_double TimeDelta)
{
	m_DeleteTime += TimeDelta;
	if (m_DeleteTime > 10)
		m_eState = OBJSTATE::DEAD;
}

void CBeauvoir_NaviBullet::Go_Direction(_double TimeDelta)
{
	if (nullptr != m_pTransform)
	{
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), BEAUVOIR_NAVIBULLET_SPEED);
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		CCell* pCell = nullptr;
		if (!m_pNavigation->IsOn(vPos, &pCell))
			m_eState = OBJSTATE::DEAD;
		else
			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

		_float fY = 0.f;
		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			vPos = XMVectorSetY(vPos, fY);
			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		}
	}
}

void CBeauvoir_NaviBullet::Notify(void * pMessage)
{
}

_int CBeauvoir_NaviBullet::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_NaviBullet::SetUp_Components()
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

CBeauvoir_NaviBullet * CBeauvoir_NaviBullet::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_NaviBullet* pGameInstance = new CBeauvoir_NaviBullet(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_NaviBullet");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_NaviBullet::Clone(void * pArg)
{
	CBeauvoir_NaviBullet* pGameInstance = new CBeauvoir_NaviBullet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_NaviBullet");
	}

	return pGameInstance;
}

void CBeauvoir_NaviBullet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
}
