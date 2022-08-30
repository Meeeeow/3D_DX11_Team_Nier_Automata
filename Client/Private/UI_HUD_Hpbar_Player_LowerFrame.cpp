#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_Player_LowerFrame.h"

CUI_HUD_Hpbar_Player_LowerFrame::CUI_HUD_Hpbar_Player_LowerFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_Player_LowerFrame::CUI_HUD_Hpbar_Player_LowerFrame(const CUI_HUD_Hpbar_Player_LowerFrame & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_Player_LowerFrame * CUI_HUD_Hpbar_Player_LowerFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_Player_LowerFrame* pInstance = new CUI_HUD_Hpbar_Player_LowerFrame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_Player_LowerFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_Player_LowerFrame::Clone(void * pArg)
{
	CUI_HUD_Hpbar_Player_LowerFrame* pInstance = new CUI_HUD_Hpbar_Player_LowerFrame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_Player_LowerFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_Player_LowerFrame::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_HUD_Hpbar_Player_LowerFrame::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_LowerFrame::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_Player_LowerFrame::SetUp_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(__super::NativeConstruct(pArg)))
	{
		return E_FAIL;
	}
	m_OrthoDesc.fTexSizeX = (_float)m_pTextureCom->Get_TexMetaData((_uint)CAtlas_Manager::CATEGORY::HUD).width;
	m_OrthoDesc.fTexSizeY = (_float)m_pTextureCom->Get_TexMetaData((_uint)CAtlas_Manager::CATEGORY::HUD).height;

	CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

	ATLASDESC* atlasDesc = nullptr;

	atlasDesc = pInstance->Get_AtlasDesc((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_FRAME"));

	Set_AtlasUV(atlasDesc->fLeft, atlasDesc->fRight, atlasDesc->fTop, atlasDesc->fBottom);

	Set_OrthoDesc(200.f, 70.f, 1.0f, 1.0f);

	RELEASE_INSTANCE(CAtlas_Manager);

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_LowerFrame::SetUp_Components()
{
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_RECT, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_ATLAS, TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_LowerFrame::SetUp_ConstantTable()
{
	if (nullptr == m_pModelCom)
	{
		return E_FAIL;
	}
	return S_OK;
}

_int CUI_HUD_Hpbar_Player_LowerFrame::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_Player_LowerFrame::LateTick(_double TimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

	return CUI::LateTick(TimeDelta);
}

HRESULT CUI_HUD_Hpbar_Player_LowerFrame::Render()
{
	if (nullptr == m_pModelCom)
	{
		return E_FAIL;
	}
	if (FAILED(SetUp_ConstantTable()))
	{
		return E_FAIL;
	}

	XMStoreFloat4(&m_vecColorOverlay, XMVectorSet(100.f, 90.f, 50.f, 0.f));

	////////////////////////////////////////////////////////////////////////

	{
		CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

		ATLASDESC* atlasDesc = nullptr;

		atlasDesc = pInstance->Get_AtlasDesc((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_FRAME"));

		Set_AtlasUV(atlasDesc->fLeft, atlasDesc->fRight, atlasDesc->fTop, atlasDesc->fBottom);

		Set_OrthoDesc(200.f, 70.f, 1.0f, 1.0f);

		RELEASE_INSTANCE(CAtlas_Manager);

		SetUp_Transform();

		m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matView", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

		m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlay, sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));

		m_pModelCom->Render(1);
	}

	////////////////////////////////////////////////////////////////////////

	{
		CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

		ATLASDESC* atlasDesc = nullptr;

		atlasDesc = pInstance->Get_AtlasDesc((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_BIG_CIRCLE"));

		Set_AtlasUV(atlasDesc->fLeft, atlasDesc->fRight, atlasDesc->fTop, atlasDesc->fBottom);

		Set_OrthoDesc(30.f, 75.f, 0.5f, 0.5f);

		RELEASE_INSTANCE(CAtlas_Manager);

		SetUp_Transform();

		m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matView", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

		m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlay, sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));

		m_pModelCom->Render(1);

	}
	//////////////////////////////////////////////////////////////////////////////////

	{
		CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

		ATLASDESC* atlasDesc = nullptr;

		atlasDesc = pInstance->Get_AtlasDesc((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_SMALL_CIRCLE"));

		Set_AtlasUV(atlasDesc->fLeft, atlasDesc->fRight, atlasDesc->fTop, atlasDesc->fBottom);

		Set_OrthoDesc(365.f, 75.f, 0.5f, 0.5f);

		RELEASE_INSTANCE(CAtlas_Manager);

		SetUp_Transform();

		m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matView", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
		m_pModelCom->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

		m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlay, sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));

		m_pModelCom->Render(1);

	}

	return S_OK;
}
