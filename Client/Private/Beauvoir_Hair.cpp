#include "stdafx.h"
#include "..\Public\Beauvoir_Hair.h"

CBeauvoir_Hair::CBeauvoir_Hair(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_Hair::CBeauvoir_Hair(const CBeauvoir_Hair & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoir_Hair::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_Hair::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iPassIndex = (_uint)MESH_PASS_INDEX::NONANIM;

	return S_OK;
}

_int CBeauvoir_Hair::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	return (_int)m_eState;
}

_int CBeauvoir_Hair::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	return _int();
}

HRESULT CBeauvoir_Hair::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, m_iPassIndex);
	}

	return S_OK;
}

void CBeauvoir_Hair::Set_WorldMatrix(_matrix WorldMatrix)
{
	if (nullptr != m_pTransform)
		m_pTransform->Set_WorldMatrix(WorldMatrix);
}

void CBeauvoir_Hair::Notify(void * pMessage)
{
}

_int CBeauvoir_Hair::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_Hair::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::tagTransformDesc tTransformDesc;
	tTransformDesc.bJump = true;
	tTransformDesc.fJumpPower = JUMPPOWER;
	tTransformDesc.fJumpTime = 0.0f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_BEAUVOIR_HAIR, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeauvoir_Hair::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));


	_float fFar = pGameInstance->Get_Far();
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBeauvoir_Hair * CBeauvoir_Hair::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_Hair* pGameInstance = new CBeauvoir_Hair(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Hair");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_Hair::Clone(void * pArg)
{
	CBeauvoir_Hair* pGameInstance = new CBeauvoir_Hair(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Hair");
	}

	return pGameInstance;
}

void CBeauvoir_Hair::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
}
