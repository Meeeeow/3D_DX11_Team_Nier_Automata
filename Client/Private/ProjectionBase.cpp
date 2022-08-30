#include "stdafx.h"
#include "..\Public\ProjectionBase.h"

CProjectionBase::CProjectionBase(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{

}

CProjectionBase::CProjectionBase(const CProjectionBase & rhs)
	: CGameObject(rhs)
{

}

HRESULT CProjectionBase::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CProjectionBase::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CProjectionBase::Tick(_double dTimeDelta)
{
	return 0;
}

_int CProjectionBase::LateTick(_double dTimeDelta)
{

	return 0;
}

HRESULT CProjectionBase::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectionBase::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_RECT, COM_KEY_RECT, (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectionBase::SetUp_ConstantTable()
{
	m_pVIBuffer->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matView", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_matProj)), sizeof(_float4x4));

	return S_OK;
}

void CProjectionBase::Notify(void * pMessage)
{
}

_int CProjectionBase::VerifyChecksum(void * pMessage)
{
	return _int();
}

void CProjectionBase::Free()
{
	__super::Free();
	
	Safe_Release(m_pRenderer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTransform);
}
