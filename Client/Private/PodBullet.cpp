#include "stdafx.h"
#include "..\Public\PodBullet.h"

CPodBullet::CPodBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPodBullet::CPodBullet(const CPodBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPodBullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPodBullet::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
		m_pTransform->Set_WorldMatrix((*(_matrix*)pArg));

	Check_Pause();
	return S_OK;
}

_int CPodBullet::Tick(_double TimeDelta)
{
	Go_Dir(TimeDelta);

	m_pSphere->Update(m_pTransform->Get_WorldMatrix());

	Check_Dead(TimeDelta);

	return (_uint)m_eState;
}

_int CPodBullet::LateTick(_double TimeDelta)
{

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pSphere);

	return _int();
}

HRESULT CPodBullet::Render()
{
	return S_OK;
}

void CPodBullet::Go_Dir(_double TimeDelta)
{
	m_pTransform->Go_Dir(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK));
}

void CPodBullet::Check_Dead(_double TimeDelta)
{
	m_DeadTime += TimeDelta;

	if (m_DeadTime > 2.0)
		m_eState = OBJSTATE::DEAD;
}

HRESULT CPodBullet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 15.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */

	/* For.Com_SPHERE */
	CCollider::tagColliderDesc		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::tagColliderDesc));

	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vScale = _float3(0.5f, 0.5f, 0.5f);
	ColliderDesc.vOrientation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	ColliderDesc.fRadius = 0.15f;

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_SPHERE"), (CComponent**)&m_pSphere, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPodBullet::SetUp_ConstantTable()
{
	return S_OK;
}

CPodBullet * CPodBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPodBullet* pGameInstance = new CPodBullet(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CPodBullet");
	}

	return pGameInstance;
}

CGameObject * CPodBullet::Clone(void * pArg)
{
	CPodBullet* pGameInstance = new CPodBullet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CPodBullet");
	}

	return pGameInstance;
}

void CPodBullet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pSphere);

	//Safe_Release(m_pModel);
	//Safe_Release(m_pTexture);
}
