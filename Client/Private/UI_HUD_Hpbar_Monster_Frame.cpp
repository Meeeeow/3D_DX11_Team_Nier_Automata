#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_Monster_Frame.h"

CUI_HUD_Hpbar_Monster_Frame::CUI_HUD_Hpbar_Monster_Frame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_Monster_Frame::CUI_HUD_Hpbar_Monster_Frame(const CUI_HUD_Hpbar_Monster_Frame & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_Monster_Frame * CUI_HUD_Hpbar_Monster_Frame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_Monster_Frame* pInstance = new CUI_HUD_Hpbar_Monster_Frame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_Monster_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_Monster_Frame::Clone(void * pArg)
{
	CUI_HUD_Hpbar_Monster_Frame* pInstance = new CUI_HUD_Hpbar_Monster_Frame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_Monster_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_Monster_Frame::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_Monster_Frame::SetUp_Components()))
	{
		return E_FAIL;
	}

	m_iDistanceZ = 0;

	//m_OrthoDesc.fTexSizeX = (_float)m_pTextureCom->Get_TexMetaData((_uint)CAtlas_Manager::CATEGORY::HUD).width;
	//m_OrthoDesc.fTexSizeY = (_float)m_pTextureCom->Get_TexMetaData((_uint)CAtlas_Manager::CATEGORY::HUD).height;

	//CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

	//ATLASDESC* atlasDesc = nullptr;

	//atlasDesc = pInstance->Get_AtlasDesc((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_FRAME"));

	//Set_AtlasUV(atlasDesc->fLeft, atlasDesc->fRight, atlasDesc->fTop, atlasDesc->fBottom);

	//RELEASE_INSTANCE(CAtlas_Manager);

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::SetUp_Components()
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

//HRESULT CUI_HUD_Hpbar_Monster_Frame::SetUp_ConstantTable()
//{
//	if (nullptr == m_pModelCom)
//	{
//		return E_FAIL;
//	}
//
//	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
//
//	XMStoreFloat4(&m_vecColorOverlay, XMVectorSet(100.f, 90.f, 50.f, 0.f));
//
//	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
//	m_pModelCom->SetUp_RawValue("g_matView", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
//	m_pModelCom->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));
//
//	m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
//	m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
//	m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
//	m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));
//
//	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlay, sizeof(_float4));
//	
//	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV(0));
//	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
//
//	RELEASE_INSTANCE(CGameInstance);
//
//	return S_OK;
//}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Released(_double dTimeDelta)
{
	return S_OK;
}

_int CUI_HUD_Hpbar_Monster_Frame::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_Monster_Frame::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Hpbar_Monster_Frame::Render()
{
	m_pModelCom->Render(1);

	return S_OK;
}
