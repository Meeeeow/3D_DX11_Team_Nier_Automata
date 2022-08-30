#include "stdafx.h"
#include "Mouse.h"
#include "GameInstance.h"

CMouse::CMouse(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMouse::CMouse(const CMouse & rhs)
	: CGameObject(rhs)
{
}


HRESULT CMouse::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMouse::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CMouse::Tick(_double TimeDelta)
{
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	return 0;
}

_int CMouse::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		const _tchar* pLayerTag = nullptr; /*pGameInstance->Get_PickingLayerTag();*/

		/* 메쉬 피킹 */

		//if (true == pGameInstance->Picking(pGameInstance->Get_GameObjectPtr(LEVEL_MAIN, pLayerTag, 0), TEXT("Com_Model"), TEXT("Com_Transform"), &vPickingPos))
		//{
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPickingPos));

		//}
		//if (true == pGameInstance->Picking(pGameInstance->Get_GameObjectPtr(LEVEL_MAIN, L"Layer_Object", m_iIndex), TEXT("Com_Model"), TEXT("Com_Transform"), &vPickingPos))
		//{
		//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPickingPos));

		//}

		m_bSHIFT = true;
	}
	else
	{
		m_bSHIFT = false;

		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	if (m_bSHIFT)
	{
		/*_float3 vPickingPos{};
		if (true == pGameInstance->Picking_Model(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Layer_Object", 0), TEXT("Com_Model"), TEXT("Com_Transform"), &vPickingPos))
		{
			m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMLoadFloat3(&vPickingPos));

		}*/
	}

	if (pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_RB))
		m_bPickingOn = true;

	RELEASE_INSTANCE(CGameInstance);

	if (m_bSHIFT)
		m_pRendererCom->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pSphereCom);

	return 0;
}

HRESULT CMouse::Render()
{
	return S_OK;
}

HRESULT CMouse::SetUp_Components()
{

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::DESC		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	CCollider::DESC ColliderDesc;
	ColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 0.3f;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CMouse * CMouse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMouse* pGameInstance = new CMouse(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMouse");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CMouse::Clone(void * pArg)
{
	CMouse* pGameInstance = new CMouse(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMouse");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CMouse::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pSphereCom);
	Safe_Release(m_pRendererCom);
}
