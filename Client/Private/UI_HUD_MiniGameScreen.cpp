#include "stdafx.h"
#include "..\Public\UI_HUD_MiniGameScreen.h"

CUI_HUD_MiniGameScreen::CUI_HUD_MiniGameScreen(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_MiniGameScreen::CUI_HUD_MiniGameScreen(const CUI_HUD_MiniGameScreen & rhs)
	: CUI(rhs)
{
}

CUI_HUD_MiniGameScreen * CUI_HUD_MiniGameScreen::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_MiniGameScreen* pInstance = new CUI_HUD_MiniGameScreen(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_MiniGameScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_MiniGameScreen::Clone(void * pArg)
{
	CUI_HUD_MiniGameScreen* pInstance = new CUI_HUD_MiniGameScreen(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_MiniGameScreen");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_MiniGameScreen::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_HUD_MiniGameScreen::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_MiniGameScreen::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(
		m_fBackGroundColorAnimOffset,
		m_fBackGroundColorAnimOffset,
		m_fBackGroundColorAnimOffset,
		m_fBackGroundAlphaAnimOffset
	));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, m_fCharacterAlphaOffset));
	
	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 4;

	m_eUIState = CUI::UISTATE::DISABLE;

	m_bCollision = false;

	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_MINIGAMESCREEN"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_MiniGameScreen::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_MiniGameScreen::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_MiniGameScreen::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::IRON, TEXT("IRON_BG"));
	SetUp_Transform(m_fBasePosX, m_fBasePosY, 1.28f, 1.28f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::IRON));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::IRON, TEXT("IRON_HS"));
	SetUp_Transform(m_fActivatePosOffsetX, m_fBasePosY, 1.1f, 1.1f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::IRON));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::IRON, TEXT("IRON_9S"));
	SetUp_Transform((_float)g_iWinCX - m_fActivatePosOffsetX, m_fBasePosY, 1.1f, 1.1f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::IRON));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Activate(_double dTimeDelta)
{
	_uint iFlag = 0;

	if (m_fBackGroundColorAnimOffsetMin < m_fBackGroundColorAnimOffset)
	{
		m_fBackGroundColorAnimOffset -= (_float)dTimeDelta * m_fBackGroundColorAnimSpeed;
	}
	else
	{
		m_fBackGroundColorAnimOffset = m_fBackGroundColorAnimOffsetMin;

		iFlag |= 1;
	}
	if (m_fActivatePosOffsetX < m_fActivatePosOffsetXMax)
	{
		m_fActivatePosOffsetX += (_float)dTimeDelta * m_fActivatePosOffsetSpeed;
	}
	else
	{
		m_fActivatePosOffsetX = m_fActivatePosOffsetXMax;

		iFlag |= 2;
	}
	if (m_fCharacterAlphaOffset < m_fCharacterAlphaOffsetMax)
	{
		m_fCharacterAlphaOffset += (_float)dTimeDelta * m_fCharacterAlphaOffsetSpeed;
	}
	else
	{
		m_fCharacterAlphaOffset = m_fCharacterAlphaOffsetMax;

		iFlag |= 4;
	}

	m_vecColorOverlays[0].x = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[0].y = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[0].z = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[1].w = m_fCharacterAlphaOffset;

	if ((iFlag & 1) && (iFlag & 2) && (iFlag & 4))
	{
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Enable(_double dTimeDelta)
{
	if (m_fEnableTimaAcc < m_fEnableTimaAccMax)
	{
		m_fEnableTimaAcc += dTimeDelta;
	}
	else
	{
		m_fEnableTimaAcc = m_fEnableTimaAccMax;
		m_eUIState = CUI::UISTATE::INACTIVATE;
	}

	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Inactivate(_double dTimeDelta)
{
	_uint iFlag = 0;

	if (m_fBackGroundAlphaAnimOffsetMax < m_fBackGroundAlphaAnimOffset)
	{
		m_fBackGroundAlphaAnimOffset -= (_float)dTimeDelta * m_fBackGroundAlphaAnimSpeed;
	}
	else
	{
		m_fBackGroundAlphaAnimOffset = m_fBackGroundAlphaAnimOffsetMax;

		iFlag |= 1;
	}
	if (m_fActivatePosOffsetXMin < m_fActivatePosOffsetX)
	{
		m_fActivatePosOffsetX -= (_float)dTimeDelta * m_fActivatePosOffsetSpeed * 2.f;
	}
	else
	{
		m_fActivatePosOffsetX = m_fActivatePosOffsetXMin;

		iFlag |= 2;
	}

	if (m_fCharacterAlphaOffsetMin < m_fCharacterAlphaOffset)
	{
		m_fCharacterAlphaOffset -= (_float)dTimeDelta * m_fCharacterAlphaOffsetSpeed;
	}
	else
	{
		m_fCharacterAlphaOffset = m_fCharacterAlphaOffsetMin;

		iFlag |= 4;
	}
	m_vecColorOverlays[0].w = m_fBackGroundAlphaAnimOffset;
	m_vecColorOverlays[1].w = m_fCharacterAlphaOffset;

	if ((iFlag & 1) && (iFlag & 2) && (iFlag & 4))
	{
		m_eUIState = CUI::UISTATE::DISABLE;
	}

	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Disable(_double dTimeDelta)
{
	m_fActivatePosOffsetX = 0.f;

	m_fBackGroundColorAnimOffset = m_fBackGroundColorAnimOffsetMax;

	m_fBackGroundAlphaAnimOffset = m_fBackGroundAlphaAnimOffsetMin;

	m_fCharacterAlphaOffset = m_fCharacterAlphaOffsetMin;

	m_fEnableTimaAcc = 0.f;

	m_vecColorOverlays[0].x = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[0].y = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[0].z = m_fBackGroundColorAnimOffset;
	m_vecColorOverlays[0].w = m_fBackGroundAlphaAnimOffset;

	m_vecColorOverlays[1].w = m_fCharacterAlphaOffset;

	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_MiniGameScreen::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_MiniGameScreen::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		if (UISTATE::DISABLE == m_eUIState)
		{
			m_eUIState = UISTATE::ACTIVATE;
		}
	}
}

_int CUI_HUD_MiniGameScreen::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_MINIGAMESCREEN)
		{
			return TRUE;
		}
	}
	return FALSE;
}
