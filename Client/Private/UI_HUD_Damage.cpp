#include "stdafx.h"
#include "..\Public\UI_HUD_Damage.h"

CUI_HUD_Damage::CUI_HUD_Damage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Damage::CUI_HUD_Damage(const CUI_HUD_Damage & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Damage * CUI_HUD_Damage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Damage* pInstance = new CUI_HUD_Damage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Damage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Damage::Clone(void * pArg)
{
	CUI_HUD_Damage* pInstance = new CUI_HUD_Damage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Damage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Damage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pDesc : m_listRenderDamage)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderDamage.clear();
}

HRESULT CUI_HUD_Damage::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Damage::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Damage::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	m_fLetterInterval = 0.f;
	m_dTimeOutSpeed = 50.0;
	m_fAlphaOutSpeed = 200.0;

	// Test Code
	//for (int i = 0; i < 10; ++i)
	//{
	//	UIDAMAGEDESC* pDesc = new UIDAMAGEDESC;

	//	pDesc->dTimeAcc = rand() % 5;
	//	pDesc->eType = DAMAGETYPE(rand() % (_uint)DAMAGETYPE::NONE);
	//	pDesc->iDamageAmount = rand() % 10000;
	//	XMStoreFloat4(&pDesc->vWorldPos, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));

	//	m_listRenderDamage.emplace_back(pDesc);
	//}

	return S_OK;
}

HRESULT CUI_HUD_Damage::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_DAMAGE"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Damage::Tick(_double TimeDelta)
{
	auto& it = m_listRenderDamage.begin();
	auto& ite = m_listRenderDamage.end();

	while (it != ite)
	{
		if (0.0 < (*it)->dTimeAcc)
		{
			(*it)->dTimeAcc -= (_float)TimeDelta * m_dTimeOutSpeed;
		}
		if ((*it)->dTimeAcc <= 0.0)
		{
			(*it)->fAlphaOffset -= (_float)TimeDelta * m_fAlphaOutSpeed;
		}
		if ((*it)->fAlphaOffset < -255.f)
		{
			Safe_Delete(*it);
			it = m_listRenderDamage.erase(it);
		}
		else
		{
			++it;
		}
	}
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Damage::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Damage::Render()
{
	for (auto& pDesc : m_listRenderDamage)
	{
		m_vecColorOverlays[0].w = pDesc->fAlphaOffset;
		m_iBufferNumber = pDesc->iDamageAmount;

		do
		{
			m_iRenderNumber = m_iBufferNumber % 10;
			m_iBufferNumber = m_iBufferNumber / 10;

			_tchar	buf[2];

			ZeroMemory(buf, sizeof(_tchar) * 2);
			_stprintf_s(buf, TEXT("%d"), m_iRenderNumber);

			CAtlas_Manager::CATEGORY	eCategory;

			if (pDesc->eType == DAMAGETYPE::PLAYER || pDesc->eType == DAMAGETYPE::MELEE)
				eCategory = CAtlas_Manager::CATEGORY::DAMAGE_A;
			if (pDesc->eType == DAMAGETYPE::MONSTER || pDesc->eType == DAMAGETYPE::RANGE)
				eCategory = CAtlas_Manager::CATEGORY::DAMAGE_B;

			SetUp_AtlasUV((_uint)eCategory, buf);
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX + m_fLetterInterval, m_fWorldToScreenY - 20.f, 1.0f, 1.0f);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)eCategory));
			m_pModelCom->Render(1);

			m_fLetterInterval -= 20.f;
		} while (0 < m_iBufferNumber);

		m_fLetterInterval = 0.f;
	}
	return S_OK;
}

HRESULT CUI_HUD_Damage::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Damage::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Damage::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Damage::Disable(_double dTimeDelta)
{
	if (false == m_listRenderDamage.empty())
	{
		Release_UI();
	}
	return S_OK;
}

HRESULT CUI_HUD_Damage::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Damage::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Damage::Release_UI()
{
	for (auto& pDesc : m_listRenderDamage)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderDamage.clear();
}

void CUI_HUD_Damage::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIDAMAGEDESC*	pDesc = new UIDAMAGEDESC;

		memcpy(pDesc, (UIDAMAGEDESC*)pPacket->pData, sizeof(UIDAMAGEDESC));

		m_listRenderDamage.emplace_back(pDesc);
	}
}

_int CUI_HUD_Damage::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_DAMAGE)
		{
			return FALSE;
		}
	}
	return TRUE;
}
