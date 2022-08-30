#include "stdafx.h"
#include "..\Public\Trail.h"
#include "VIBuffer_AnimationTrail.h"
#include "..\Public\AnimationTrail.h"

CAnimationTrail::CAnimationTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{
}

CAnimationTrail::CAnimationTrail(const CAnimationTrail & rhs)
	: CGameObject(rhs)
{
}

CAnimationTrail * CAnimationTrail::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CAnimationTrail* pInstance = new CAnimationTrail(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSGBOX("Failed To Create CAnimationTrail");
	}

	return pInstance;
}

CGameObject * CAnimationTrail::Clone(void * pArg)
{
	CAnimationTrail* pInstance = new CAnimationTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pInstance);
		MSGBOX("Failed To Clone CAnimationTrail");
	}

	return pInstance;
}

void CAnimationTrail::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pTexture);
}

HRESULT CAnimationTrail::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAnimationTrail::NativeConstruct(void * pArg)
{
	m_tDesc = *(static_cast<TRAIL_DESC*>(pArg));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CAnimationTrail::Tick(_double dTimeDelta)
{
	if (Get_State() == OBJSTATE::DEAD)
		return static_cast<_int>(OBJSTATE::DEAD);

	return static_cast<_int>(OBJSTATE::ENABLE);
}

_int CAnimationTrail::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_eState != OBJSTATE::DISABLE
		&& m_eState != OBJSTATE::DEAD)
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);
	}

	return _int();
}

HRESULT CAnimationTrail::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModel->Render(0);

	return S_OK;
}

HRESULT CAnimationTrail::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Prototype_Component_VIBuffer_AnimationTrail", L"Com_Model", (CComponent**)&m_pModel, &m_tDesc.iNumTrails)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Prototype_Texture_BlackToWhite", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;



	return S_OK;
}

HRESULT CAnimationTrail::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Already Computed
	//m_pModel->SetUp_RawValue("g_matWorld", &m_matWorldMatrixTranspose4x4, sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));


	_float fFar = pGameInstance->Get_Far();
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

	m_pModel->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());

	_int iIsDistortion = 1;
	m_pModel->SetUp_RawValue("g_iIsDistortion", &iIsDistortion, sizeof(_int));


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

const char * CAnimationTrail::Get_PassName(_uint iIndex)
{
	return nullptr;
}

_uint CAnimationTrail::Get_MaxPassIndex()
{
	return _uint();
}

void CAnimationTrail::Notify(void * pMessage)
{
}

_int CAnimationTrail::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CAnimationTrail::Update_Position()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	// Max
	_vector vMaxPos = XMLoadFloat3(&m_vFarPoint1);
	vMaxPos = XMVectorSetW(vMaxPos, 1.f);
	_float		fWeightW = 1.f - (m_vMaxBlendWieght.x + m_vMaxBlendWieght.y + m_vMaxBlendWieght.z);

	_matrix		matBone = XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.x])) * m_vMaxBlendWieght.x
						+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.y])) * m_vMaxBlendWieght.y
						+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.z])) * m_vMaxBlendWieght.z
						+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.w])) * fWeightW;

	vMaxPos = XMVector4Transform(vMaxPos, matBone);
	vMaxPos = XMVector4Transform(vMaxPos, XMLoadFloat4x4(&m_matWorldMatrix4x4));




	// Min
	_vector vMinPos = XMLoadFloat3(&m_vFarPoint2);
	vMinPos = XMVectorSetW(vMinPos, 1.f);
	fWeightW = 1.f - (m_vMinBlendWeight.x + m_vMinBlendWeight.y + m_vMinBlendWeight.z);

	matBone = XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.x])) * m_vMinBlendWeight.x
			+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.y])) * m_vMinBlendWeight.y
			+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.z])) * m_vMinBlendWeight.z
			+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.w])) * fWeightW;

	vMinPos = XMVector4Transform(vMinPos, matBone);
	vMinPos = XMVector4Transform(vMinPos, XMLoadFloat4x4(&m_matWorldMatrix4x4));

	m_pModel->Add_LastPosition(vMinPos, vMaxPos);
	m_pModel->Calculate_Catmullrom();
	return S_OK;
}

HRESULT CAnimationTrail::Init_TrailPosition()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	// Max
	_vector vMaxPos = XMLoadFloat3(&m_vFarPoint1);
	vMaxPos = XMVectorSetW(vMaxPos, 1.f);
	_float		fWeightW = 1.f - (m_vMaxBlendWieght.x + m_vMaxBlendWieght.y + m_vMaxBlendWieght.z);
	_matrix		matBone = XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.x])) * m_vMaxBlendWieght.x
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.y])) * m_vMaxBlendWieght.y
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.z])) * m_vMaxBlendWieght.z
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMaxBlendIndex.w])) * fWeightW;

	vMaxPos = XMVector4Transform(vMaxPos, matBone);
	vMaxPos = XMVector4Transform(vMaxPos, XMLoadFloat4x4(&m_matWorldMatrix4x4));

	// Min
	_vector vMinPos = XMLoadFloat3(&m_vFarPoint2);
	vMinPos = XMVectorSetW(vMinPos, 1.f);
	fWeightW = 1.f - (m_vMinBlendWeight.x + m_vMinBlendWeight.y + m_vMinBlendWeight.z);

	matBone = XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.x])) * m_vMinBlendWeight.x
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.y])) * m_vMinBlendWeight.y
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.z])) * m_vMinBlendWeight.z
		+ XMMatrixTranspose(XMLoadFloat4x4(&m_tBonMatrices[m_vMinBlendIndex.w])) * fWeightW;

	vMinPos = XMVector4Transform(vMinPos, matBone);
	vMinPos = XMVector4Transform(vMinPos, XMLoadFloat4x4(&m_matWorldMatrix4x4));

	m_pModel->Init_AllPosition(vMinPos, vMaxPos);

	return S_OK;
}

void CAnimationTrail::Add_BoneMatrices(_float4x4 * _tBoneMatrices)
{
	memcpy(m_tBonMatrices, _tBoneMatrices, sizeof(_float4x4) * 128);
}

