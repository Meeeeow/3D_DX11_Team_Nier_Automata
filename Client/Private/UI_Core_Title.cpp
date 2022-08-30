#include "stdafx.h"
#include "..\Public\UI_Core_Title.h"

CUI_Core_Title::CUI_Core_Title(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Title::CUI_Core_Title(const CUI_Core_Title & rhs)
	: CUI(rhs)
{
}

CUI_Core_Title * CUI_Core_Title::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Title* pInstance = new CUI_Core_Title(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Title");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Title::Clone(void * pArg)
{
	CUI_Core_Title* pInstance = new CUI_Core_Title(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Title");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Title::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_Title::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_Title::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Title::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Background Default : 224, 223, 211, 128

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-40.f, -40.f, -40.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Shadow

	m_eUIState = CUI::UISTATE::DISABLE;
	m_eTitleType = CUI_Core_Title::TITLETYPE::COUNT;
	m_szTextElemKey = TEXT("ITEM");
	m_iDistanceZ = 2;
	m_bCollision = false;
	
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_TITLE"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CUI_Core_Title::SetUp_Components()
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

_int CUI_Core_Title::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Title::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_Title::Render()
{
	// Font Shadow

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szTextElemKey);
	SetUp_Transform_AlignLeft(50.f + m_fShadowFontOffset, 150.f + m_fShadowFontOffset, 2.2f, 2.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);
	
	// Font Origin

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szTextElemKey);
	SetUp_Transform_AlignLeft(50.f + m_fActivateAnimOffsetPos, 150.f + m_fActivateAnimOffsetPos, 2.2f, 2.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_Title::Activate(_double dTimeDelta)
{
	DWORD	dwFlag = 0;

	if (m_fActivateAnimAlpha < m_fActivateAnimAlphaMax)
	{
		m_fActivateAnimAlpha += m_fActivateAnimAlphaSpeed * (_float)dTimeDelta;
	}
	else
	{
		m_fActivateAnimAlpha = m_fActivateAnimAlphaMax;
		dwFlag |= 1;
	}

	if (m_fActivateAnimOffsetPosMax < m_fActivateAnimOffsetPos)
	{
		m_fActivateAnimOffsetPos -= m_fActivateAnimOffsetPosSpeed * (_float)dTimeDelta;
	}
	else
	{
		m_fActivateAnimOffsetPos = m_fActivateAnimOffsetPosMax;
		dwFlag |= 2;
	}

	m_vecColorOverlays[0].w = m_fActivateAnimAlpha;

	if (dwFlag & 3)
	{
		m_vecColorOverlays[1].w = m_fActivateAnimAlphaMax;

		if (m_fShadowFontOffset < m_fShadowFontOffsetMax)
		{
			m_fShadowFontOffset += m_fActivateAnimOffsetPosSpeed * (_float)dTimeDelta;
		}
		else
		{
			m_fShadowFontOffset = m_fShadowFontOffsetMax;
			m_eUIState = CUI::UISTATE::ENABLE;
			dwFlag = 0;
		}
	}
	return S_OK;
}

HRESULT CUI_Core_Title::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Title::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Title::Disable(_double dTimeDelta)
{
	m_vecColorOverlays[0].w = m_fActivateAnimAlphaMin;
	m_vecColorOverlays[1].w = m_fActivateAnimAlphaForShadow;

	m_fActivateAnimAlpha = m_fActivateAnimAlphaMin;
	m_fActivateAnimOffsetPos = m_fActivateAnimOffsetPosMin;
	m_fShadowFontOffset = m_fShadowFontOffsetMin;

	return S_OK;
}

HRESULT CUI_Core_Title::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Title::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Title::Release_UI()
{
	m_vecColorOverlays[0].w = m_fActivateAnimAlphaMin;
	m_vecColorOverlays[1].w = m_fActivateAnimAlphaForShadow;

	m_fActivateAnimAlpha = m_fActivateAnimAlphaMin;
	m_fActivateAnimOffsetPos = m_fActivateAnimOffsetPosMin;
	m_fShadowFontOffset = m_fShadowFontOffsetMin;
}

void CUI_Core_Title::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UITITLEDESC pDesc = *(UITITLEDESC*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::DISABLE == pDesc.eState)	{ m_eUIState = pDesc.eState; }
			if (UISTATE::ACTIVATE == pDesc.eState) { m_eUIState = pDesc.eState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::DISABLE == pDesc.eState)	{ m_eUIState = pDesc.eState; }
			if (UISTATE::ACTIVATE == pDesc.eState) { m_eUIState = pDesc.eState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ENABLE == pDesc.eState)	{ m_eUIState = pDesc.eState; }
			if (UISTATE::ACTIVATE == pDesc.eState)	{ m_eUIState = pDesc.eState; }
			break;
		case Client::CUI::UISTATE::PRESSED:
			break;
		case Client::CUI::UISTATE::RELEASED:
			break;
		case Client::CUI::UISTATE::NONE:
			break;
		default:
			break;
		}

		if (m_eUIState == CUI::UISTATE::ENABLE || m_eUIState == CUI::UISTATE::ACTIVATE)
		{
			if (m_eTitleType != pDesc.eType)
			{
				Release_UI();
			}
			m_eTitleType = pDesc.eType;

			switch (pDesc.eType)
			{
			case CUI_Core_Title::TITLETYPE::MAP:
				m_szTextElemKey = TEXT("MAP");
				break;
			case CUI_Core_Title::TITLETYPE::QUEST:
				m_szTextElemKey = TEXT("QUEST");
				break;
			case CUI_Core_Title::TITLETYPE::ITEM:
				m_szTextElemKey = TEXT("ITEM");
				break;
			case CUI_Core_Title::TITLETYPE::WEAPON:
				m_szTextElemKey = TEXT("WEAPON");
				break;
			case CUI_Core_Title::TITLETYPE::SKILL:
				m_szTextElemKey = TEXT("EQUIPMENT");
				break;
			case CUI_Core_Title::TITLETYPE::DATA:
				m_szTextElemKey = TEXT("DATA");
				break;
			case CUI_Core_Title::TITLETYPE::SYSTEM:
				m_szTextElemKey = TEXT("SYSTEM");
				break;
			case CUI_Core_Title::TITLETYPE::COUNT:
				break;
			default:
				break;
			}
		}
	}
}

_int CUI_Core_Title::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_TITLE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
