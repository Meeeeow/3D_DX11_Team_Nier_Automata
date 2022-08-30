#include "stdafx.h"
#include "..\Public\UI_HUD_QuickSlot.h"

CUI_HUD_QuickSlot::CUI_HUD_QuickSlot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_QuickSlot::CUI_HUD_QuickSlot(const CUI_HUD_QuickSlot & rhs)
	: CUI(rhs)
{
}

CUI_HUD_QuickSlot * CUI_HUD_QuickSlot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_QuickSlot* pInstance = new CUI_HUD_QuickSlot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_QuickSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_QuickSlot::Clone(void * pArg)
{
	CUI_HUD_QuickSlot* pInstance = new CUI_HUD_QuickSlot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_QuickSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_QuickSlot::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_HUD_QuickSlot::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_QuickSlot::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	
	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::DISABLE;

	m_bCollision = false;

	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_QUICKSLOT"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_QuickSlot::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_QuickSlot::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_QuickSlot::Render()
{
	const _tchar*	szElemKey = nullptr;

	switch (m_eEquipState)
	{
	case Client::CPlayer::EQUIPSTATE::KATANA:
		szElemKey = TEXT("KATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::LONGKATANA:
		szElemKey = TEXT("LONGKATANA");
		break;
	case Client::CPlayer::EQUIPSTATE::SPEAR:
		szElemKey = TEXT("SPEAR");
		break;
	case Client::CPlayer::EQUIPSTATE::GAUNTLET:
		szElemKey = TEXT("GAUNTLET");
		break;
	case Client::CPlayer::EQUIPSTATE::NONE:
		szElemKey = TEXT("");
		break;
	default:
		break;
	}

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::WEAPON, szElemKey);
	SetUp_Transform(m_fBasePosX, m_fBasePosY, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::WEAPON));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Activate(_double dTimeDelta)
{
	if (m_fAlphaAnimOffsetMax < m_fAlphaAnimOffset)
	{
		m_fAlphaAnimOffset -= (_float)dTimeDelta * m_fAlphaAnimSpeed;
	}
	else
	{
		m_fAlphaAnimOffset = m_fAlphaAnimOffsetMax;
		m_eUIState = CUI::UISTATE::DISABLE;
	}
	m_vecColorOverlays[0].w = m_fAlphaAnimOffset;

	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Disable(_double dTimeDelta)
{
	m_fAlphaAnimOffset = m_fAlphaAnimOffsetMin;

	m_vecColorOverlays[0].w = m_fAlphaAnimOffset;

	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_QuickSlot::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_QuickSlot::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		if (UISTATE::DISABLE == m_eUIState)
		{
			m_eUIState = UISTATE::ACTIVATE;

			m_eEquipState = *(CPlayer::EQUIPSTATE*)pPacket->pData;
		}
	}
}

_int CUI_HUD_QuickSlot::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_QUICKSLOT)
		{
			return TRUE;
		}
	}
	return FALSE;
}
