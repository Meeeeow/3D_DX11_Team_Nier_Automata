#include "stdafx.h"
#include "..\Public\Trail.h"
#include "VIBuffer_Trail.h"

CTrail::CTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{
}

CTrail::CTrail(const CTrail & rhs)
	: CGameObject(rhs)
{
}

CTrail * CTrail::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CTrail* pInstance = new CTrail(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
		MSGBOX("Failed To Create CTrail");
	}

	return pInstance;
}

CGameObject * CTrail::Clone(void * pArg)
{
	CTrail* pInstance = new CTrail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pInstance);
		MSGBOX("Failed To Clone CTrail");
	}

	return pInstance;
}

void CTrail::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pTexture);
}

HRESULT CTrail::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTrail::NativeConstruct(void * pArg)
{
	m_tDesc = *(static_cast<TRAIL_DESC*>(pArg));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CTrail::Tick(_double dTimeDelta)
{
	if (Get_State() == OBJSTATE::DEAD)
		return static_cast<_int>(OBJSTATE::DEAD);

	return static_cast<_int>(OBJSTATE::ENABLE);
}

_int CTrail::LateTick(_double dTimeDelta)
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

HRESULT CTrail::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModel->Render(0);

	return S_OK;
}

HRESULT CTrail::SetUp_Components()
{   
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Prototype_Component_VIBuffer_Trail", L"Com_Model", (CComponent**)&m_pModel, &m_tDesc.iNumTrails)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Prototype_Texture_BlackToWhite", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;


	
	return S_OK;
}

HRESULT CTrail::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	// Already Computed
	//m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));


	_float fFar = pGameInstance->Get_Far();
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

	m_pModel->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

const char * CTrail::Get_PassName(_uint iIndex)
{
	return nullptr;
}

_uint CTrail::Get_MaxPassIndex()
{
	return _uint();
}

void CTrail::Notify(void * pMessage)
{
}

_int CTrail::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CTrail::Add_Position(_float3 _vMinPos, _float3 _vMaxPos)
{
	if (nullptr == m_pModel)
		return E_FAIL;

	m_pModel->Add_LastPosition(_vMinPos, _vMinPos);

	return S_OK;
}

HRESULT CTrail::Init_TrailPosition(_float3 _vMinPos, _float3 _vMaxPos)
{
	if (nullptr == m_pModel)
		return E_FAIL;

	m_pModel->Init_AllPosition(_vMinPos, _vMinPos);

	return S_OK;
}

