#include "stdafx.h"
#include "..\Public\Beauvoir_Skirt.h"

CBeauvoir_Skirt::CBeauvoir_Skirt(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_Skirt::CBeauvoir_Skirt(const CBeauvoir_Skirt & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoir_Skirt::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_Skirt::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iPassIndex = (_uint)MESH_PASS_INDEX::NONANIM;
	m_fDissolveSpeed = BEAUVOIR_SKIRT_DISSOLVE_SPEED;
	m_fDissolveValue = 0.f;
	m_bIsDissolve = false;

	Check_Pause();
	return S_OK;
}

_int CBeauvoir_Skirt::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bIsDissolve)
	{
		m_fDissolveValue += m_fDissolveSpeed * static_cast<_float>(dTimeDelta);
		if (m_fDissolveValue > 1.f)
		{
			m_bIsEnd = true;
		}
	}

	return (_int)m_eState;
}

_int CBeauvoir_Skirt::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	return _int();
}

HRESULT CBeauvoir_Skirt::Render()
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

void CBeauvoir_Skirt::Set_WorldMatrix(_matrix WorldMatrix)
{
	if (nullptr != m_pTransform)
		m_pTransform->Set_WorldMatrix(WorldMatrix);
}

void CBeauvoir_Skirt::Notify(void * pMessage)
{
}

_int CBeauvoir_Skirt::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_Skirt::SetUp_Components()
{
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::tagTransformDesc tTransformDesc;
	tTransformDesc.bJump = true;
	tTransformDesc.fJumpPower = JUMPPOWER;
	tTransformDesc.fJumpTime = 0.0f;

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_BEAUVOIR_SKIRT, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;


	/* For.Com_DissolveTexture*/
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;	


	return S_OK;
}

HRESULT CBeauvoir_Skirt::SetUp_ConstantTable()
{
	__super::SetUp_ConstantTable();
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_bIsDissolve)
	{
		m_pModel->SetUp_RawValue("g_fDissolve", &m_fDissolveValue, sizeof(_float));
		m_pModel->SetUp_Texture("g_texDissolve", m_pDissolveTexture->Get_SRV());
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBeauvoir_Skirt * CBeauvoir_Skirt::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_Skirt*	pGameInstance = new CBeauvoir_Skirt(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Skirt");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_Skirt::Clone(void * pArg)
{
	CBeauvoir_Skirt*	pGameInstance = new CBeauvoir_Skirt(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_Skirt");
	}

	return pGameInstance;
}

void CBeauvoir_Skirt::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pDissolveTexture);
}
