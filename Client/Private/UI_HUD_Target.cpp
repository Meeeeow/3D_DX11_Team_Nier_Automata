#include "stdafx.h"
#include "..\Public\UI_HUD_Target.h"

CUI_HUD_Target::CUI_HUD_Target(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Target::CUI_HUD_Target(const CUI_HUD_Target & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Target * CUI_HUD_Target::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Target* pInstance = new CUI_HUD_Target(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Target::Clone(void * pArg)
{
	CUI_HUD_Target* pInstance = new CUI_HUD_Target(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Target::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pDesc : m_listRenderTarget)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderTarget.clear();
}

HRESULT CUI_HUD_Target::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Target::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Target::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Target::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_TARGET"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Target::Tick(_double TimeDelta)
{
	m_dAlphaAnimTimeAcc += TimeDelta;

	m_vecColorOverlays[0].w = (abs(sinf((_float)m_dAlphaAnimTimeAcc)) - 1.f) * 255.f;

	if (360.0 < m_dAlphaAnimTimeAcc)
	{
		m_dAlphaAnimTimeAcc = 0.0;
	}
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Target::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Target::Render()
{
	for (auto& pDesc : m_listRenderTarget)
	{
		if (pDesc->dTimeDuration <= 0.0)
		{
			continue;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::TARGET, TEXT("BASIC"));
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, 1.5f, 1.5f);

		m_pTransformCom->Rotation_AxisDegree(m_fRotateDegree, XMVectorSet(0.f, 0.f, 1.f, 0.f));

		SetUp_DefaultRawValue();
		
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::TARGET));
		m_pModelCom->Render(1);
	}
	return S_OK;
}

HRESULT CUI_HUD_Target::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Target::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Target::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Target::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Target::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Target::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Target::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UITARGETDESC*	pCopiedDesc = new UITARGETDESC;

		memcpy(pCopiedDesc, (UITARGETDESC*)pPacket->pData, sizeof(UITARGETDESC));

		list<UITARGETDESC*>::iterator	it = m_listRenderTarget.begin();
		list<UITARGETDESC*>::iterator	ite = m_listRenderTarget.end();

		UITARGETDESC*	pMatchedDesc = nullptr;

		// objID 가 일치하는 desc 검색
		while (it != ite)
		{
			if ((*it)->iObjID == pCopiedDesc->iObjID)
			{
				pMatchedDesc = (*it);
				break;
			}
			++it;
		}
		// objID 가 일치하는 desc가 있을 경우 값만 업데이트
		if (nullptr != pMatchedDesc)
		{
			pMatchedDesc->vWorldPos = pCopiedDesc->vWorldPos;
			pMatchedDesc->dTimeDuration = pCopiedDesc->dTimeDuration;

			Safe_Delete(pCopiedDesc);
		}
		// objID 가 일치하는 desc는 없는데 컨테이너에 뭔가 있을 경우
		else
		{
			if (false == m_listRenderTarget.empty())
			{
				Safe_Delete(*(m_listRenderTarget.begin()));
				m_listRenderTarget.clear();
			}
			m_listRenderTarget.emplace_back(pCopiedDesc);
		}
	}
}

_int CUI_HUD_Target::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_TARGET)
		{
			return FALSE;
		}
	}
	return TRUE;
}
