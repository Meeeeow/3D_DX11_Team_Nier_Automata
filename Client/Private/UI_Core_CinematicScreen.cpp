#include "stdafx.h"
#include "..\Public\UI_Core_CinematicScreen.h"

CUI_Core_CinematicScreen::CUI_Core_CinematicScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_CinematicScreen::CUI_Core_CinematicScreen(const CUI_Core_CinematicScreen & rhs)
	: CUI(rhs)
{
}

CUI_Core_CinematicScreen * CUI_Core_CinematicScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_CinematicScreen* pInstance = new CUI_Core_CinematicScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_CinematicScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_CinematicScreen::Clone(void * pArg)
{
	CUI_Core_CinematicScreen* pInstance = new CUI_Core_CinematicScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_CinematicScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_CinematicScreen::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_CinematicScreen::NativeConstruct_Prototype()
{
	if (FAILED(CUI::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_CinematicScreen::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -64.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	/*
	ENABLE로 alpha 0.5 상태에서 시작
	INACTIVATE에서 alpha 0.0으로 FadeOut	
	*/

	m_iDistanceZ = 5;

	m_eUIState = CUI::UISTATE::DISABLE;

	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_CINEMATIC"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Core_CinematicScreen::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_CinematicScreen::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_CinematicScreen::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND, TEXT("CINEMATIC"));
	SetUp_Transform(640.f, -240.f - m_fActivatePosOffsetY, 0.4f, 0.4f);	// 640.f, -340.f
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND, TEXT("CINEMATIC"));
	SetUp_Transform(640.f, 960.f + m_fActivatePosOffsetY, 0.4f, 0.4f);	// 640.f, 1060.f (720 + 340)
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::BACKGROUND));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Activate(_double dTimeDelta)
{
	m_fActivatePosOffsetY -= m_fActivatePosOffsetYSpeed * (_float)dTimeDelta;

	if (m_fActivatePosOffsetY < 0.f)
	{
		m_fActivatePosOffsetY = 0.f;
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Enable(_double dTimeDelta)
{
	m_fActivatePosOffsetY = 0.f;

	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Inactivate(_double dTimeDelta)
{
	// Alpha Anim
	m_vecColorOverlays[0].w -= _float(dTimeDelta * m_dUVAnimSpeed);

	if (m_vecColorOverlays[0].w < 0.f)
	{
		m_eUIState = CUI::UISTATE::DISABLE;
	}

	// Pos Anim
	//m_fActivatePosOffsetY += m_fActivatePosOffsetYSpeed * dTimeDelta;

	//if (100.f < m_fActivatePosOffsetY)
	//{
	//	m_fActivatePosOffsetY = 100.f;
	//	m_eUIState = CUI::UISTATE::DISABLE;
	//}
	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Disable(_double dTimeDelta)
{
	m_fActivatePosOffsetY = 100.f;

	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_CinematicScreen::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_CinematicScreen::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		if (UISTATE::DISABLE == m_eUIState)
		{
			if (UISTATE::ENABLE == pState)
			{
				m_eUIState = pState;
			}
			else if (UISTATE::ACTIVATE == pState)
			{
				m_eUIState = pState;
			}
		}
		else if (UISTATE::ENABLE == m_eUIState)
		{
			if (UISTATE::INACTIVATE == pState)
			{
				m_eUIState = pState;
			}
		}
	}
}

_int CUI_Core_CinematicScreen::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_CORE_CINEMATIC)
		{
			return FALSE;
		}
	}
	return TRUE;
}
