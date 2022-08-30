#include "stdafx.h"
#include "..\Public\UI_HUD_Pointer.h"

CUI_HUD_Pointer::CUI_HUD_Pointer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Pointer::CUI_HUD_Pointer(const CUI_HUD_Pointer & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Pointer * CUI_HUD_Pointer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Pointer* pInstance = new CUI_HUD_Pointer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Pointer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Pointer::Clone(void * pArg)
{
	CUI_HUD_Pointer* pInstance = new CUI_HUD_Pointer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Pointer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Pointer::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pDesc : m_listRenderPointer)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderPointer.clear();
}

HRESULT CUI_HUD_Pointer::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Pointer::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Pointer::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));
	//XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Pointer::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_POINTER"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Pointer::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Pointer::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Pointer::Render()
{
	for (auto& pDesc : m_listRenderPointer)
	{
		if (pDesc->dTimeDuration <= 0.0)
		{
			continue;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("POINTER"));
		
		pDesc->vWorldPos.y += 1.5f + m_fFloatingY;	// 대상의 크기에 맞춰서 적절히 보정되도록 해야함
				
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));

		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, 0.5f, 0.5f);

		// 화면에서 바로 올릴 경우 멀어졌을 때 이상하게 렌더링 됨

		//SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY - 100.f, 0.5f, 0.5f);

		SetUp_DefaultRawValue();
		
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);
	}
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Enable(_double dTimeDelta)
{
	auto& it = m_listRenderPointer.begin();
	auto& ite = m_listRenderPointer.end();

	m_fFloatingTimeAcc += (_float)dTimeDelta * 2.5f;

	m_fFloatingY = sinf(m_fFloatingTimeAcc) / 5.f;

	if (360.f <= m_fFloatingTimeAcc)
		m_fFloatingTimeAcc = 0.f;

	while (it != ite)
	{
		if ((*it)->dTimeDuration <= 0.0)
		{
			Safe_Delete(*it);
			it = m_listRenderPointer.erase(it);
		}
		else
		{
			++it;
		}
	}
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Disable(_double dTimeDelta)
{
	if (false == m_listRenderPointer.empty())
	{
		Release_UI();
	}
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Pointer::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Pointer::Release_UI()
{
	for (auto& pDesc : m_listRenderPointer)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderPointer.clear();

	return;
}

void CUI_HUD_Pointer::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIPOINTERDESC*	pCopiedDesc = new UIPOINTERDESC;

		memcpy(pCopiedDesc, (UIPOINTERDESC*)pPacket->pData, sizeof(UIPOINTERDESC));

		list<UIPOINTERDESC*>::iterator	it = m_listRenderPointer.begin();
		list<UIPOINTERDESC*>::iterator	ite = m_listRenderPointer.end();

		UIPOINTERDESC*	pMatchedDesc = nullptr;

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
			//if (false == m_listRenderPointer.empty())
			//{
			//	Safe_Delete(*(m_listRenderPointer.begin()));
			//	m_listRenderPointer.clear();
			//}
			m_listRenderPointer.emplace_back(pCopiedDesc);
		}
	}
}

_int CUI_HUD_Pointer::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_POINTER)
		{
			return FALSE;
		}
	}
	return TRUE;
}
