#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_Player_Frame.h"

CUI_HUD_Hpbar_Player_Frame::CUI_HUD_Hpbar_Player_Frame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_Player_Frame::CUI_HUD_Hpbar_Player_Frame(const CUI_HUD_Hpbar_Player_Frame & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_Player_Frame * CUI_HUD_Hpbar_Player_Frame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_Player_Frame* pInstance = new CUI_HUD_Hpbar_Player_Frame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_Player_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_Player_Frame::Clone(void * pArg)
{
	CUI_HUD_Hpbar_Player_Frame* pInstance = new CUI_HUD_Hpbar_Player_Frame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_Player_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_Player_Frame::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pDesc : m_listRenderHPBar)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderHPBar.clear();
}

HRESULT CUI_HUD_Hpbar_Player_Frame::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_Player_Frame::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(100.f, 90.f, 50.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_HPBAR_PLAYER_FRAME"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Hpbar_Player_Frame::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_Player_Frame::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Render()
{
	for (auto& pDesc : m_listRenderHPBar)
	{
		if (pDesc->dTimeDuration < 0.0)
		{
			continue;
		}

		_bool bGrayScale = m_pRendererCom->Get_GrayDistortion();
		m_pModelCom->SetUp_RawValue("g_bGrayScale", &bGrayScale, sizeof(bool));

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_FRAME"));
		SetUp_Transform(200.f, 70.f, 1.0f, 1.0f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_BIG_CIRCLE"));
		SetUp_Transform(30.f, 75.f, 0.5f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_SMALL_CIRCLE"));
		SetUp_Transform(365.f, 75.f, 0.5f, 0.5f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_UPPER_FRAME"));
		SetUp_Transform(200.f, 33.f, 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Frame::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Hpbar_Player_Frame::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIHPBARPLAYERFRAMEDESC*	pCopiedDesc = new UIHPBARPLAYERFRAMEDESC;

		memcpy(pCopiedDesc, (UIHPBARPLAYERFRAMEDESC*)pPacket->pData, sizeof(UIHPBARPLAYERFRAMEDESC));

		list<UIHPBARPLAYERFRAMEDESC*>::iterator	it = m_listRenderHPBar.begin();
		list<UIHPBARPLAYERFRAMEDESC*>::iterator	ite = m_listRenderHPBar.end();

		UIHPBARPLAYERFRAMEDESC*	pMatchedDesc = nullptr;

		while (it != ite)
		{
			if ((*it)->iObjID == pCopiedDesc->iObjID)
			{
				pMatchedDesc = (*it);
				break;
			}
			++it;
		}
		if (nullptr != pMatchedDesc)
		{
			pMatchedDesc->ePlayerState = pCopiedDesc->ePlayerState;
			pMatchedDesc->dTimeDuration = pCopiedDesc->dTimeDuration;

			Safe_Delete(pCopiedDesc);
		}
		else
		{
			m_listRenderHPBar.emplace_back(pCopiedDesc);
		}
	}
}

_int CUI_HUD_Hpbar_Player_Frame::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_PLAYER_HPBAR_FRAME)
		{
			return FALSE;
		}
	}
	return TRUE;
}
