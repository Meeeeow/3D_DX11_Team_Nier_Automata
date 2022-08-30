#include "stdafx.h"
#include "ToolNavigation.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"
#include "NavigationTab.h"

CToolNavigation::CToolNavigation(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice, pContextDevice)
{

}

HRESULT CToolNavigation::NativeConstruct_Prototype(NAVINFO tNavigation)
{
	m_tNavInfo = tNavigation;

	m_pVIBuffer = CVIBuffer_Triangle::Create(m_pGraphicDevice, m_pContextDevice, Shader_NavigationTex, DefaultTechnique, m_tNavInfo.vPoint);
	m_pVIBufferLine = CVIBuffer_TriangleLineStrip::Create(m_pGraphicDevice, m_pContextDevice, Shader_Navigation, DefaultTechnique, m_tNavInfo.vPoint);

	CCollider::DESC tColliderDesc;
	tColliderDesc.fRadius = 0.5f;

	tColliderDesc.vPivot = tNavigation.vPoint[0];
	tColliderDesc.vPivot.y += 0.3f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, L"Point_A", (CComponent**)&m_pPointCollider[0], &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = tNavigation.vPoint[1];
	tColliderDesc.vPivot.y += 0.3f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, L"Point_B", (CComponent**)&m_pPointCollider[1], &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = tNavigation.vPoint[2];
	tColliderDesc.vPivot.y += 0.3f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, L"Point_C", (CComponent**)&m_pPointCollider[2], &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Debug_NavMeshTexture", COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4(&m_vCenterPos, (XMLoadFloat3(&m_tNavInfo.vPoint[0]) + XMLoadFloat3(&m_tNavInfo.vPoint[1]) + XMLoadFloat3(&m_tNavInfo.vPoint[2])) / 3.f);
	m_vCenterPos.w = 1.f;
	m_fCenterRadius = max(fabs(XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vCenterPos) - XMLoadFloat3(&m_tNavInfo.vPoint[0]))))
		, fabs(XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vCenterPos) - XMLoadFloat3(&m_tNavInfo.vPoint[1])))));
	m_fCenterRadius = max(m_fCenterRadius, fabs(XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_vCenterPos) - XMLoadFloat3(&m_tNavInfo.vPoint[2])))));



	return S_OK;
}

_int CToolNavigation::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < 3; ++i)
		m_pPointCollider[i]->Update(m_pTransform->Get_WorldMatrix());

	return _int();
}

_int CToolNavigation::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if(pGameInstance->Culling(XMLoadFloat4(&m_vCenterPos), m_pTransform->Get_WorldMatrix(), m_fCenterRadius))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	
	return _int();
}

HRESULT CToolNavigation::Render()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	for (_uint i = 0; i < 3; ++i)
		m_pPointCollider[i]->Render();

	if (m_bTargetOn == false)
	{
		m_pVIBufferLine->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
		m_pVIBufferLine->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		m_pVIBufferLine->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
		m_pVIBufferLine->Render();
	}

	if (m_bTargetOn == true)
	{
		m_pVIBuffer->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
		m_pVIBuffer->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		m_pVIBuffer->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
		m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV((_uint)m_tNavInfo.eOption));
		m_pVIBuffer->Render();
	}

	return S_OK;
}

void CToolNavigation::Picking()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float3		vPos;
	_float		fDistance = 0.f;

	if (Picking(pGameInstance->Get_Pivot(), pGameInstance->Get_Ray(), vPos, fDistance) == true)
	{
		CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

		CNavigationTab::PNMDESC tDesc;
		tDesc.vPoint = vPos;
		tDesc.vPoint.y -= 0.3f;
		tDesc.fDistance = fDistance;
		pToolView->m_pMainTab->m_pNavigationTab->Insert_NavDesc(tDesc);
	}
}

_bool CToolNavigation::Picking(_fvector vOrigin, _fvector vRay, _float3 & vPos, _float & fDistance)
{
	if (m_pPointCollider[0]->Collision_Sphere(vOrigin, vRay, vPos, fDistance) == true)
		return true;

	if (m_pPointCollider[1]->Collision_Sphere(vOrigin, vRay, vPos, fDistance) == true)
		return true;

	if (m_pPointCollider[2]->Collision_Sphere(vOrigin, vRay, vPos, fDistance) == true)
		return true;

	return false;
}

CToolNavigation * CToolNavigation::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, NAVINFO tNavigation)
{
	CToolNavigation* pInstance = new CToolNavigation(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(tNavigation)))
		Safe_Release(pInstance);

	return pInstance;
}

void CToolNavigation::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBufferLine);
	for (_uint i = 0; i < 3; ++i)
		Safe_Release(m_pPointCollider[i]);
}
