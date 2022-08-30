#include "stdafx.h"
#include "..\Public\UI_Core_Background.h"

CUI_Core_Background::CUI_Core_Background(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Background::CUI_Core_Background(const CUI_Core_Background & rhs)
	: CUI(rhs)
{
}

CUI_Core_Background * CUI_Core_Background::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Background* pInstance = new CUI_Core_Background(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Background");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Background::Clone(void * pArg)
{
	CUI_Core_Background* pInstance = new CUI_Core_Background(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Background");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Background::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_Background::NativeConstruct_Prototype()
{
	if (FAILED(CUI::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_Background::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Background::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(-61.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// TriGrid A
	m_vecColorOverlays.push_back(vColorOverlay);	// TriGrid B

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// RectGrid

	m_iSwitch = 1;
	m_dAnimSpeed = 500.0;
	m_dTimeAccSwitch = 0.0;
	m_dTimeAccUVAnim = 0.0;
	m_bIsRectGridActive = false;

	m_iDistanceZ = 1;

	m_eUIState = CUI::UISTATE::DISABLE;

	m_bCollision = false;

	return S_OK;
}

HRESULT CUI_Core_Background::SetUp_Components()
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

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_BACKGROUND"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Core_Background::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Background::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_Background::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::TRIGRID, TEXT("TRIGRID_A"));
	SetUp_Transform(640.f, 360.f, 0.4f, 0.4f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::TRIGRID));
	m_pModelCom->Render(7);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::TRIGRID, TEXT("TRIGRID_B"));
	SetUp_Transform(640.f, 360.f, 0.4f, 0.4f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::TRIGRID));
	m_pModelCom->Render(7);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND, TEXT("RECTGRID"));
	SetUp_Transform(640.f, 360.f, 0.8f, 0.8f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND));
	m_pModelCom->Render(8);

	return S_OK;
}

HRESULT CUI_Core_Background::Activate(_double dTimeDelta)
{
	m_dTimeAccSwitch += dTimeDelta;
	m_dTimeAccUVAnim += dTimeDelta;

	m_bIsRectGridActive = true;
	if (m_dTimeAccSwitch >= 0.1)
	{
		m_iSwitch *= -1;
		m_dTimeAccSwitch = 0.0;
	}
	if (m_iSwitch == 1)
	{
		m_vecColorOverlays[0].w += _float(dTimeDelta * m_dAnimSpeed);
		if (255.f <= m_vecColorOverlays[0].w)
			m_vecColorOverlays[0].w = 255.f;
	}
	if (m_iSwitch == -1)
	{
		m_vecColorOverlays[1].w += _float(dTimeDelta * m_dAnimSpeed);
		if (255.f <= m_vecColorOverlays[1].w)
			m_vecColorOverlays[1].w = 255.f;
	}
	if (255.f == m_vecColorOverlays[0].w && 255.f == m_vecColorOverlays[1].w)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
		m_dTimeAccSwitch = 0.0;
		m_dTimeAccUVAnim = 0.0;
	}
	return S_OK;
}

HRESULT CUI_Core_Background::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Background::Inactivate(_double dTimeDelta)
{
	m_dTimeAccSwitch += dTimeDelta;
	m_dTimeAccUVAnim += dTimeDelta;

	if (m_dTimeAccSwitch >= 0.1)
	{
		m_iSwitch *= -1;
		m_dTimeAccSwitch = 0.0;
	}
	if (m_iSwitch == 1)
	{
		m_vecColorOverlays[0].w -= _float(dTimeDelta * m_dAnimSpeed);
	}
	if (m_iSwitch == -1)
	{
		m_vecColorOverlays[1].w -= _float(dTimeDelta * m_dAnimSpeed);
	}
	if (m_vecColorOverlays[0].w < 0.f && m_vecColorOverlays[1].w < 0.f)
	{
		m_bIsRectGridActive = false;
		m_eUIState = CUI::UISTATE::DISABLE;
		m_dTimeAccSwitch = 0.0;
		m_dTimeAccUVAnim = 0.0;
	}
	return S_OK;
}

HRESULT CUI_Core_Background::Disable(_double dTimeDelta)
{
	m_vecColorOverlays[0].w = 0.f;
	m_vecColorOverlays[1].w = 0.f;

	m_bIsRectGridActive = false;
	m_dTimeAccSwitch = 0.0;
	m_dTimeAccUVAnim = 0.0;

	return S_OK;
}

HRESULT CUI_Core_Background::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Background::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Background::Release_UI()
{
	m_vecColorOverlays[0].w = 0.f;
	m_vecColorOverlays[1].w = 0.f;

	m_bIsRectGridActive = false;
	m_dTimeAccSwitch = 0.0;
	m_dTimeAccUVAnim = 0.0;
}

void CUI_Core_Background::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		if (UISTATE::DISABLE == m_eUIState)
		{
			if (UISTATE::ACTIVATE == pState)
			{
				m_eUIState = pState;
			}
		}
		if (UISTATE::INACTIVATE == m_eUIState)
		{
			if (UISTATE::ACTIVATE == m_eUIState)
			{
				Release_UI();
				m_eUIState = pState;
			}
		}
		if (UISTATE::ENABLE == m_eUIState)
		{
			if (UISTATE::INACTIVATE == pState)
			{
				m_eUIState = pState;
			}
		}
	}
}

_int CUI_Core_Background::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_CORE_BACKGROUND)
		{
			return FALSE;
		}
	}
	return TRUE;
}
