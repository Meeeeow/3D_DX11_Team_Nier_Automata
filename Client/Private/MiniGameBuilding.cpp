#include "stdafx.h"
#include "..\Public\MiniGameBuilding.h"

CMiniGameBuilding::CMiniGameBuilding(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{
}

CMiniGameBuilding::CMiniGameBuilding(const CMiniGameBuilding & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniGameBuilding::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMiniGameBuilding::NativeConstruct(void * pArg)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRONFIST_BACKGROUND, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	//m_pModel->SetUp_BoundingBox(m_pTransform->Get_WorldMatrix());

	m_vDOFParams = _float4(-10.f, 75.f, 95.f, 0.97f);
	m_iPassIndex = 2;

	return S_OK;
}

_int CMiniGameBuilding::Tick(_double dTimeDelta)
{
	return _int();
}

_int CMiniGameBuilding::LateTick(_double dTimeDelta)
{
	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	
	return _int();
}

HRESULT CMiniGameBuilding::Render()
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

void CMiniGameBuilding::Notify(void * pMessage)
{
}

_int CMiniGameBuilding::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CMiniGameBuilding::SetUp_ConstantTable()
{
	//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	//_float fFar = pGameInstance->Get_Far();

	//m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	//m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	//m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	//m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	//m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));

	//Compute_CamDistanceForAlpha(m_pTransform);

	//m_pModel->SetUp_RawValue("vDofParams", &m_vDOFParams, sizeof(_float4));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();
	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

	Compute_CamDistanceForAlpha(m_pTransform);
	_float4 m_vDOFParams;
	m_vDOFParams.y = Get_CameraDistance() + 1.5f;
	m_vDOFParams.z = m_vDOFParams.y * 7.f;
	m_vDOFParams.x = 0.f;
	m_vDOFParams.w = 1.f;

	m_pModel->SetUp_RawValue("vDofParams", &m_vDOFParams, sizeof(_float4));

	return S_OK;
}

const char * CMiniGameBuilding::Get_PassName(_uint iIndex)
{
	return m_pModel->Get_PassName(iIndex);
}

_uint CMiniGameBuilding::Get_MaxPassIndex()
{
	return m_pModel->Get_MaxPassIndex();
}

CMiniGameBuilding * CMiniGameBuilding::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CMiniGameBuilding* pGameInstance = new CMiniGameBuilding(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CMiniGameBuilding");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CMiniGameBuilding::Clone(void * pArg)
{
	CMiniGameBuilding* pGameInstance = new CMiniGameBuilding(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CMiniGameBuilding");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CMiniGameBuilding::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
}
