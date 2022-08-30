#include "stdafx.h"
#include "..\Public\Building.h"
#include "OctreeNode.h"

CBuilding::CBuilding(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{

}

CBuilding::CBuilding(const CBuilding & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBuilding::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBuilding::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	MO_INFO tInfo = *(MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo.m_matWorld));

	_uint iLevel = CGameInstance::Get_Instance()->Get_LevelID();
	if (iLevel == (_uint)LEVEL::LOADING)
		iLevel = CGameInstance::Get_Instance()->Get_NextLevelID();
	if (FAILED(Add_Components(iLevel, tInfo.szModelComName, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->SetUp_BoundingBox(m_pTransform->Get_WorldMatrix());

	m_iPassIndex = 2;
	if (CGameInstance::Get_Instance()->Get_NextLevelID() == (_uint)LEVEL::STAGE1)
		m_iPassIndex = 0;

	return S_OK;
}

_int CBuilding::Tick(_double dTimeDelta)
{

	return _int();
}

_int CBuilding::LateTick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	return _int();
}

HRESULT CBuilding::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		m_pModel->Render(i, m_iPassIndex);
	}
	return S_OK;
}


void CBuilding::Notify(void * pMessage)
{

}

_int CBuilding::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBuilding::SetUp_Components()
{
	return S_OK;
}

HRESULT CBuilding::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();

	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));

	m_pModel->SetUp_RawValue("vDofParams", m_pRenderer->Get_DOF_Parmas(), sizeof(_float4));

	return S_OK;
}

const char * CBuilding::Get_PassName(_uint iIndex)
{
	return m_pModel->Get_PassName(iIndex);
}

_uint CBuilding::Get_MaxPassIndex()
{
	return m_pModel->Get_MaxPassIndex();
}

CBuilding * CBuilding::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBuilding* pInstance = new CBuilding(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBuilding::Clone(void * pArg)
{
	CBuilding* pInstance = new CBuilding(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBuilding::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
}
