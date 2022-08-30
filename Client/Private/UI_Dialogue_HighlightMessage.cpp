#include "stdafx.h"
#include "..\Public\UI_Dialogue_HighlightMessage.h"

CUI_Dialogue_HighlightMessage::CUI_Dialogue_HighlightMessage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Dialogue_HighlightMessage::CUI_Dialogue_HighlightMessage(const CUI_Dialogue_HighlightMessage & rhs)
	: CUI(rhs)
{
}

CUI_Dialogue_HighlightMessage * CUI_Dialogue_HighlightMessage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Dialogue_HighlightMessage* pInstance = new CUI_Dialogue_HighlightMessage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Dialogue_HighlightMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Dialogue_HighlightMessage::Clone(void * pArg)
{
	CUI_Dialogue_HighlightMessage* pInstance = new CUI_Dialogue_HighlightMessage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Dialogue_HighlightMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Dialogue_HighlightMessage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	//for (auto& pDesc : m_listRenderSysMsg)
	//{
	//	Safe_Delete(pDesc);
	//}
	//m_listRenderSysMsg.clear();
}

HRESULT CUI_Dialogue_HighlightMessage::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Dialogue_HighlightMessage::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, m_fAlphaAnimMin));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, m_fAlphaAnimMin));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 4;

	m_eUIState = CUI::UISTATE::DISABLE;

	m_bCollision = false;

	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_HIGHLIGHT_MSG"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Dialogue_HighlightMessage::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Dialogue_HighlightMessage::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Dialogue_HighlightMessage::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::H_MSG, TEXT("BG"));
	SetUp_Transform(m_fBasePosX, m_fBasePosY, 0.5f, 0.5f);	// 1280 * 100
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::H_MSG));
	m_pModelCom->Render(1);

	//if (true == m_bIsMessageRender)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szTextElemKey);
		SetUp_Transform(m_fBasePosX, m_fBasePosY, 2.f, 2.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(1);
	}
	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Activate(_double dTimeDelta)
{
	m_dTimeAcc += dTimeDelta;

	if (m_dTimeAcc < 2.0)
	{
		if (m_fAlphaAnim < m_fAlphaAnimMax)
		{
			m_fAlphaAnim += m_fAlphaAnimSpeed * (_float)dTimeDelta;
		}
		else
		{
			m_fAlphaAnim = m_fAlphaAnimMax;
			m_bIsMessageRender = true;
		}
	}
	else
	{
		m_dTimeAcc = 0.0;
		m_eUIState = CUI::UISTATE::INACTIVATE;
		m_bIsMessageRender = false;
	}
	m_vecColorOverlays[0].w = m_fAlphaAnim;
	m_vecColorOverlays[1].w = m_fAlphaAnim;

	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Enable(_double dTimeDelta)
{

	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Inactivate(_double dTimeDelta)
{
	if (m_fAlphaAnimMin < m_fAlphaAnim)
	{
		m_fAlphaAnim -= m_fAlphaAnimSpeed * (_float)dTimeDelta;
	}
	else
	{
		m_fAlphaAnim = m_fAlphaAnimMin;
		m_eUIState = CUI::UISTATE::DISABLE;
	}
	m_vecColorOverlays[0].w = m_fAlphaAnim;
	m_vecColorOverlays[1].w = m_fAlphaAnim;

	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Disable(_double dTimeDelta)
{
	//m_fAlphaAnim = m_fAlphaAnimMin;

	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_HighlightMessage::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Dialogue_HighlightMessage::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		if (UISTATE::DISABLE == m_eUIState)
		{
			m_eUIState = UISTATE::ACTIVATE;

			EVENTID	eID = *(EVENTID*)pPacket->pData;

			switch (eID)
			{
			case Client::CUI_Dialogue_HighlightMessage::EVENTID::BANNED:
				m_szTextElemKey = TEXT("HMSG_BANNED");
				break;
			case Client::CUI_Dialogue_HighlightMessage::EVENTID::MISSION_CLEAR:
				m_szTextElemKey = TEXT("HMSG_MISSION_CLEAR");
				break;
			case Client::CUI_Dialogue_HighlightMessage::EVENTID::MISSION_END:
				m_szTextElemKey = TEXT("HMSG_MISSION_END");
				break;
			case Client::CUI_Dialogue_HighlightMessage::EVENTID::THANKS:
				m_szTextElemKey = TEXT("HMSG_THANKS");
				break;
			case Client::CUI_Dialogue_HighlightMessage::EVENTID::NONE:
				m_szTextElemKey = nullptr;
				break;
			default:
				break;
			}
		}
	}
}

_int CUI_Dialogue_HighlightMessage::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_DIALOGUE_HIGHLIGHTMSG)
		{
			return TRUE;
		}
	}
	return FALSE;
}
