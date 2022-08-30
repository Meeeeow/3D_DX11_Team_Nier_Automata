#include "stdafx.h"
#include "Tool_CamCollider.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"
#include "CameraTab.h"


CTool_CamCollider::CTool_CamCollider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice, pContextDevice)
{
}

HRESULT CTool_CamCollider::NativeConstruct_Prototype(void* pArg)
{

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	 _vector vPos = *(_vector*)pArg;

	 m_pTransform->Set_State(CTransform::STATE::POSITION,vPos); 

	CCollider::DESC tColliderDesc;
	tColliderDesc.fRadius = 0.5f;

	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, L"Collider_Com", (CComponent**)&m_pCollider, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

_int CTool_CamCollider::Tick(_double dTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
		return _int();
}

_int CTool_CamCollider::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pCollider);

	return _int();
}

HRESULT CTool_CamCollider::Render()
{
	m_pCollider->Render();
	return S_OK;
}

void CTool_CamCollider::Picking()
{
}

_bool CTool_CamCollider::Picking(_fvector vOrigin, _fvector vRay, _float3 & vPos, _float & fDistance)
{
	return _bool();
}

CTool_CamCollider * CTool_CamCollider::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void* pArg)
{
	CTool_CamCollider* pInstance = new CTool_CamCollider(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTool_CamCollider::Free()
{
	Safe_Release(m_pCollider);
}
