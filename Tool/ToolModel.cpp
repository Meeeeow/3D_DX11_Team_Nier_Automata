#include "stdafx.h"
#include "ToolModel.h"
#include "GameInstance.h"
#include "MainFrm.h"
#include "ToolView.h"
#include "MainTab.h"
#include "NavigationTab.h"


CToolModel::CToolModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice, pContextDevice)
{

}

CToolModel::CToolModel(const CToolModel & rhs)
	: CToolObjectBase(rhs)
{

}


HRESULT CToolModel::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CToolModel::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;
	
	DESC tDesc;
	memcpy(&tDesc, pArg, sizeof(DESC));


	if (FAILED(Add_Components((_uint)LEVEL::STATIC, tDesc.szModelTag, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;
	Set_ModelComponentTag(tDesc.szModelTag);


	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (m_pModel->Get_HaveAnim() == true)
	{
		m_bAnim = true;
		m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;
		m_pModel->Set_Animation(0, true);
	}
	else
	{
		m_bAnim = false;
		m_iPassIndex = 2;
	}

	_float3 vPoint = m_pModel->Get_MiddlePoint();

	
	return S_OK;
}

_int CToolModel::Tick(_double dTimeDelta)
{
	//if (m_pModel->Get_HaveAnim() == true)
	//{
	//	_double Ratio = 0.59996636182;

	//	if (Ratio <= m_pModel->Get_PlayTimeRatio())
	//		m_bTest = true;

	//}
	return _int();
}

_int CToolModel::LateTick(_double dTimeDelta)
{
	//if (m_bTest)
	//	dTimeDelta = 0.0;

	if (m_bAnim)
		m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	return _int();
}

_int CToolModel::SelectLateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CToolModel::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		if (m_bAnim == true)
		{
			m_pModel->Render(i, m_iPassIndex);
		}
		else
			m_pModel->Render(i, 2);
	}


	return S_OK;
}

HRESULT CToolModel::SelectRender()
{
	return S_OK;
}

HRESULT CToolModel::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();
	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));
	m_pModel->SetUp_RawValue("vDofParams", m_pRenderer->Get_DOF_Parmas(), sizeof(_float4));

	return S_OK;
}

void CToolModel::Set_LoadData(MO_INFO tInfo)
{
	memcpy(&m_tModelObjectInfo, &tInfo, sizeof(MO_INFO));
	Set_ModelComponentTag(m_tModelObjectInfo.szModelComName);
	Set_ModelObjectTag(m_tModelObjectInfo.szModelObjectName);
	Syncronize_Matrix(m_tModelObjectInfo.m_matWorld);
}

void CToolModel::Set_Position(_vector vPosition)
{
	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
}

void CToolModel::Add_Position(_vector vPosition)
{
	m_pTransform->Add_Position(vPosition);
}

void CToolModel::Set_Scale(_vector vScale)
{
	m_pTransform->Set_Scale(vScale);
}

void CToolModel::Set_Rotation(_float3 vAngle)
{
	m_pTransform->Rotation_Degree(vAngle);
}

void CToolModel::Picking()
{
	_float3 vPos;
	_float	fDistance;
	if (Picking(&vPos, fDistance))
	{
		CMainFrame*	pMain = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());
		CToolView*	pToolView = dynamic_cast<CToolView*>(pMain->GetActiveView());

		CNavigationTab::PNMDESC tDesc;
		tDesc.vPoint = vPos;
		tDesc.fDistance = fDistance;
		pToolView->m_pMainTab->m_pNavigationTab->Insert_NavDesc(tDesc);
	}
}

_bool CToolModel::Picking(_float3 * pOut, _float & fDistance)
{
	return m_pModel->Picking(pOut, m_pTransform->Get_WorldMatrix(), fDistance);
}

_vector CToolModel::Get_Position()
{
	return m_pTransform->Get_State(CTransform::STATE::POSITION);
}

_vector CToolModel::Get_Scale()
{
	return m_pTransform->Get_Scale();
}

void CToolModel::Set_ModelObjectTag(const wchar_t * pTag)
{
	lstrcpy(m_tModelObjectInfo.szModelObjectName, pTag);
}

void CToolModel::Set_ModelComponentTag(const wchar_t * pTag)
{
	lstrcpy(m_tModelObjectInfo.szModelComName, pTag);
}

void CToolModel::Syncronize_Matrix()
{
	m_tModelObjectInfo.m_matWorld = m_pTransform->Get_WorldMatrix4x4();
}

void CToolModel::Syncronize_Matrix(_float4x4 matWorld)
{
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
}

const MO_INFO & CToolModel::Get_SaveData()
{
	// TODO: ���⿡ ��ȯ ������ �����մϴ�.
	Syncronize_Matrix();

	return m_tModelObjectInfo;
}

CToolModel * CToolModel::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CToolModel* pInstance = new CToolModel(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CToolModel::Clone(void * pArg)
{
	CToolModel* pInstance = new CToolModel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CToolModel::Free()
{
	__super::Free();
	Safe_Release(m_pModel);
}
