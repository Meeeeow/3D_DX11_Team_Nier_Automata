#include "stdafx.h"
#include "..\Public\UI_HUD_PodCoolTime.h"

CUI_HUD_PodCoolTime::CUI_HUD_PodCoolTime(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_PodCoolTime::CUI_HUD_PodCoolTime(const CUI_HUD_PodCoolTime & rhs)
	: CUI(rhs)
{
}

CUI_HUD_PodCoolTime * CUI_HUD_PodCoolTime::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_PodCoolTime* pInstance = new CUI_HUD_PodCoolTime(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_PodCoolTime");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_PodCoolTime::Clone(void * pArg)
{
	CUI_HUD_PodCoolTime* pInstance = new CUI_HUD_PodCoolTime(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_PodCoolTime");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_PodCoolTime::Free()
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

HRESULT CUI_HUD_PodCoolTime::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_PodCoolTime::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, -30.f));
	
	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -128.f));
	
	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -30.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_PODCOOLTIME"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_PodCoolTime::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_PodCoolTime::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_PodCoolTime::Render()
{
	for (auto& pDesc : m_listRenderTarget)
	{
		if (pDesc->fMaxCoolTimeAcc <= pDesc->fCrntCoolTimeAcc)
		{
			continue;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CIRCLE, TEXT("COOLTIME_OUTER"));
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY + m_fPosYOffset, 0.4f, 0.4f);
		SetUp_DefaultRawValue();

		_float fCrntCoolTimeRatio = pDesc->fCrntCoolTimeAcc / pDesc->fMaxCoolTimeAcc;
		
		m_pModelCom->SetUp_RawValue("g_fCrntDegree", &fCrntCoolTimeRatio, sizeof(_float));
 		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CIRCLE));
		m_pModelCom->Render(9);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CIRCLE, TEXT("COOLTIME_INNER"));
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY + m_fPosYOffset, 0.3f, 0.3f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CIRCLE));
		m_pModelCom->Render(1);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("SKL"));
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY + m_fPosYOffset - 15.f, 1.0f, 1.0f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(1);


		m_iBufferNumber = _uint((pDesc->fMaxCoolTimeAcc - pDesc->fCrntCoolTimeAcc) * 60.0f);

		_uint	iBuffer = m_iBufferNumber;
		_uint	iDecimalOffsetCount = 0;
		
		// 가운데 정렬 될 수 있도록 자릿수에 따른 Offset 설정
		while (10 <= iBuffer)
		{
			iBuffer /= 10;
			++iDecimalOffsetCount;
		}
		
		do
		{
			m_iRenderNumber = m_iBufferNumber % 10;
			m_iBufferNumber = m_iBufferNumber / 10;

			_tchar	buf[2];

			ZeroMemory(buf, sizeof(_tchar) * 2);
			_stprintf_s(buf, TEXT("%d"), m_iRenderNumber);

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX + m_fLetterInterval + (iDecimalOffsetCount * m_fDecimalInterval), m_fWorldToScreenY + m_fPosYOffset + 10.f, 1.0f, 1.0f);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(1);

			m_fLetterInterval -= 16.f;
		} while (0 < m_iBufferNumber);

		m_fLetterInterval = 0.f;
	}
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Disable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_PodCoolTime::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_PodCoolTime::Release_UI()
{
	for (auto& pDesc : m_listRenderTarget)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderTarget.clear();
}

void CUI_HUD_PodCoolTime::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIPODCOOLTIMEDESC*	pCopiedDesc = new UIPODCOOLTIMEDESC;

		memcpy(pCopiedDesc, (UIPODCOOLTIMEDESC*)pPacket->pData, sizeof(UIPODCOOLTIMEDESC));

		list<UIPODCOOLTIMEDESC*>::iterator	it = m_listRenderTarget.begin();
		list<UIPODCOOLTIMEDESC*>::iterator	ite = m_listRenderTarget.end();

		UIPODCOOLTIMEDESC*	pMatchedDesc = nullptr;

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
			pMatchedDesc->fCrntCoolTimeAcc = pCopiedDesc->fCrntCoolTimeAcc;

			Safe_Delete(pCopiedDesc);
		}
		else
		{
			// objID 가 일치하는 desc는 없는데 컨테이너에 뭔가 있을 경우
			if (false == m_listRenderTarget.empty())
			{
				Safe_Delete(*(m_listRenderTarget.begin()));
				m_listRenderTarget.clear();
			}
			m_listRenderTarget.emplace_back(pCopiedDesc);
		}
	}
	else if (2 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	eState = *(UISTATE*)pPacket->pData;

		m_eUIState = eState;
	}
	else if (3 == iResult)
	{
		Release_UI();
	}
}

_int CUI_HUD_PodCoolTime::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_PODCOOLTIME)
		{
			return 1;
		}
		if (*check == CHECKSUM_ONLY_UISTATE)
		{
			return 2;
		}
		if (*check == CHECKSUM_RELEASE_UI)
		{
			return 3;
		}
	}
	return FALSE;
}
