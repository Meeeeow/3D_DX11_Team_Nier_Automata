#include "stdafx.h"
#include "Event_Collider.h"


CEvent_Collider::CEvent_Collider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice,pContextDevice)
{
}

HRESULT CEvent_Collider::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEvent_Collider::NativeConstruct(void * pArg)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.vScale = { 1.f , 1.f , 1.f };
	
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, L"Collider_Com", (CComponent**)&m_pCollider, &tColliderDesc)))
		return E_FAIL;
	return S_OK;
}

_int CEvent_Collider::Tick(_double dTimeDelta)
{
	m_pCollider->Update(m_pTransform->Get_WorldMatrix());
	return _int();
}

_int CEvent_Collider::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pCollider);

	return _int();
}

HRESULT CEvent_Collider::Render()
{
	m_pCollider->Render();
	return S_OK;
}

CEvent_Collider * CEvent_Collider::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void * pArg)
{
	CEvent_Collider* pGameInstance = new CEvent_Collider(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CEvent_Collider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CEvent_Collider::Clone(void * pArg)
{
	CEvent_Collider* pGameInstance = new CEvent_Collider(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone	CEventCollider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CEvent_Collider::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
}
