#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_Player_Contents.h"

CUI_HUD_Hpbar_Player_Contents::CUI_HUD_Hpbar_Player_Contents(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_Player_Contents::CUI_HUD_Hpbar_Player_Contents(const CUI_HUD_Hpbar_Player_Contents & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_Player_Contents * CUI_HUD_Hpbar_Player_Contents::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_Player_Contents* pInstance = new CUI_HUD_Hpbar_Player_Contents(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_Player_Contents");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_Player_Contents::Clone(void * pArg)
{
	CUI_HUD_Hpbar_Player_Contents* pInstance = new CUI_HUD_Hpbar_Player_Contents(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_Player_Contents");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_Player_Contents::Free()
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

HRESULT CUI_HUD_Hpbar_Player_Contents::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_Player_Contents::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-50.f, -60.f, -100.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_HPBAR_PLAYER"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Hpbar_Player_Contents::Tick(_double TimeDelta)
{
	auto& it = m_listRenderHPBar.begin();
	auto& ite = m_listRenderHPBar.end();

	while (it != ite)
	{
		UIHPBARPLAYERDESC*	pHPBarDesc = *it;

		// UV 연출용 패러미터 설정
		if (pHPBarDesc->fCrntHPUV < pHPBarDesc->fPrevHPUV)
		{
			pHPBarDesc->fPrevHPUV -= _float(TimeDelta * 0.3f);
		}
		if (pHPBarDesc->fPrevHPUV <= pHPBarDesc->fCrntHPUV)
		{
			pHPBarDesc->fPrevHPUV = pHPBarDesc->fCrntHPUV;
		}

		// 대상의 HP가 0이 되었을 경우 즉시 삭제
		// 현재는 삭제하지않고 일단 대기
		if (pHPBarDesc->fCrntHPUV <= 0.0)
		{
			pHPBarDesc->fCrntHPUV = 0.0;
			++it;

			//Safe_Delete(*it);
			//it = m_listRenderHPBar.erase(it);
		}
		else
		{
			++it;
		}
	}
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_Player_Contents::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Render()
{
	for (auto& pDesc : m_listRenderHPBar)
	{
		if (pDesc->dTimeDuration < 0.0)
		{
			pDesc->fPrevHPUV = pDesc->fCrntHPUV;
			continue;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_CONTENTS"));
		SetUp_Transform(198.f, 48.f, 1.045f, 0.7f);
		SetUp_DefaultRawValue();
		_bool bGrayScale = m_pRendererCom->Get_GrayDistortion();
		m_pModelCom->SetUp_RawValue("g_bGrayScale", &bGrayScale, sizeof(_bool));
		m_pModelCom->SetUp_RawValue("g_fCrntHPUV", &pDesc->fCrntHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fPrevHPUV", &pDesc->fPrevHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(6);
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Player_Contents::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Hpbar_Player_Contents::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIHPBARPLAYERDESC*	pCopiedDesc = new UIHPBARPLAYERDESC;

		memcpy(pCopiedDesc, (UIHPBARPLAYERDESC*)pPacket->pData, sizeof(UIHPBARPLAYERDESC));

		list<UIHPBARPLAYERDESC*>::iterator	it = m_listRenderHPBar.begin();
		list<UIHPBARPLAYERDESC*>::iterator	ite = m_listRenderHPBar.end();

		UIHPBARPLAYERDESC*	pMatchedDesc = nullptr;

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
			pMatchedDesc->fCrntHPUV = pCopiedDesc->fCrntHPUV;
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

_int CUI_HUD_Hpbar_Player_Contents::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_PLAYER_HPBAR)
		{
			return FALSE;
		}
	}
	return TRUE;
}
