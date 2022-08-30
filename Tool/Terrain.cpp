#include "stdafx.h"
#include "Terrain.h"
#include "GameInstance.h"

CTerrain::CTerrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTerrain::CTerrain(const CTerrain & rhs)
	: CGameObject(rhs)
{
}


HRESULT CTerrain::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::NativeConstruct(void * pArg)
{
	_uint iVertices[2] = { 0 };

	if (nullptr != pArg)
		memcpy(iVertices, pArg, sizeof(_uint) * 2);

	m_iVerticesX = iVertices[0];
	m_iVerticesZ = iVertices[1];

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CTerrain::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Mouse_Pressing(CInput_Device::MOUSE_LEFT))
	//	Picking();
	//else
	//{
	//	if (m_bRectangle)
	//		m_pModelCom->Set_RectanglePickingContinue(false);
	//}


#ifdef _DEBUG
	//m_pNavigationCom->Tick(m_pTransformCom->Get_WorldMatrix());
#endif

	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

_int CTerrain::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;


	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::NONALPHA, this);

	return _int();
}

HRESULT CTerrain::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pModelCom->Render(m_iPass);

	return S_OK;
}

void CTerrain::Picking()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_float3 vOut{ 0.f, 0.f, 0.f };

	//if (m_bCircle)
	//	pGameInstance->Circle_Picking(this, COM_KEY_MODEL, COM_KEY_TRANSFORM, &vOut, m_iRange, m_bDir);

	//else if (m_bRectangle)
	//	pGameInstance->Rectangle_Picking(this, COM_KEY_MODEL, COM_KEY_TRANSFORM, &vOut, m_iRange, m_bDir);

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CTerrain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Model */
	_uint iVertices[2] = { m_iVerticesX, m_iVerticesZ };
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_TERRAIN, COM_KEY_MODEL, (CComponent**)&m_pModelCom, iVertices)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_TERRAIN, COM_KEY_TEXTURE, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Filter */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_FILTER0, COM_KEY_FILTER, (CComponent**)&m_pFilterCom)))
		return E_FAIL;

	/* For.Com_Brush */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BRUSH0, COM_KEY_BRUSH, (CComponent**)&m_pBrushCom)))
		return E_FAIL;

	///* For.Com_Navigatoin*/
	//if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigationCom)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CTerrain::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModelCom->SetUp_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_ViewMatrix", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_ProjMatrix", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModelCom->SetUp_Texture("g_SourTexture", m_pTextureCom->Get_SRV(0));
	m_pModelCom->SetUp_Texture("g_DestTexture", m_pTextureCom->Get_SRV(1));
	m_pModelCom->SetUp_Texture("g_FilterTexture", m_pFilterCom->Get_SRV(0));
	m_pModelCom->SetUp_Texture("g_BrushTexture", m_pBrushCom->Get_SRV(0));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTerrain * CTerrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTerrain* pGameInstance = new CTerrain(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CTerrain::Clone(void * pArg)
{
	CTerrain* pGameInstance = new CTerrain(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTerrain");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CTerrain::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pBrushCom);
	Safe_Release(m_pFilterCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
