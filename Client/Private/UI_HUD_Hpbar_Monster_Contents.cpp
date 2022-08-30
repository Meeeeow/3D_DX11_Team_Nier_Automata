#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_Monster_Contents.h"

CUI_HUD_Hpbar_Monster_Contents::CUI_HUD_Hpbar_Monster_Contents(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_Monster_Contents::CUI_HUD_Hpbar_Monster_Contents(const CUI_HUD_Hpbar_Monster_Contents & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_Monster_Contents * CUI_HUD_Hpbar_Monster_Contents::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_Monster_Contents* pInstance = new CUI_HUD_Hpbar_Monster_Contents(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_Monster_Contents");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_Monster_Contents::Clone(void * pArg)
{
	CUI_HUD_Hpbar_Monster_Contents* pInstance = new CUI_HUD_Hpbar_Monster_Contents(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_Monster_Contents");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_Monster_Contents::Free()
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

HRESULT CUI_HUD_Hpbar_Monster_Contents::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_Monster_Contents::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-50.f, -60.f, -100.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(100.f, 90.f, 50.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 255.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_HPBAR_MONSTER"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Hpbar_Monster_Contents::Tick(_double TimeDelta)
{
	auto& it = m_listRenderHPBar.begin();
	auto& ite = m_listRenderHPBar.end();

	while (it != ite)
	{
		UIHPBARMONDESC*	pHPBarDesc = *it;

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

			Safe_Delete(*it);
			it = m_listRenderHPBar.erase(it);
		}
		else
		{
			++it;
		}
	}
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_Monster_Contents::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Render()
{
	for (auto& pDesc : m_listRenderHPBar)
	{
		if (pDesc->dTimeDuration < 0.0)
		{
			pDesc->fPrevHPUV = pDesc->fCrntHPUV;
			continue;
		}

		// 스케일 보정값은 몬스터의 타입과 관련해 재조정되어야한다.
		// HP바는 몬스터의 키나 타입과 관련해 알맞은 위치에 재조정되어야한다.
		
		if (CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS == pDesc->eMonsterType)
		{
			pDesc->vWorldPos.y += pDesc->fPosOffset + 4.f;
			m_fScaleOffset = 1.5f;
		}
		else
		{
			pDesc->vWorldPos.y += pDesc->fPosOffset;
			m_fScaleOffset = 1.f;
		}
		

		if (CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS == pDesc->eMonsterType)
		{
			switch (pDesc->eID)
			{
			case Client::CUI_Dialogue_EngageMessage::BOSSID::ZHUANGZI:
				m_szBossNameElemKey = TEXT("ZHUANGZI");
				break;
			case Client::CUI_Dialogue_EngageMessage::BOSSID::ENGELS:
				m_szBossNameElemKey = TEXT("ENGELS");
				break;
			case Client::CUI_Dialogue_EngageMessage::BOSSID::CHARIOT:
				m_szBossNameElemKey = TEXT("CHARIOT");
				break;
			case Client::CUI_Dialogue_EngageMessage::BOSSID::BEAUVOIR:
				m_szBossNameElemKey = TEXT("BEAUVOIR");
				break;
			case Client::CUI_Dialogue_EngageMessage::BOSSID::NONE:
				m_szBossNameElemKey = nullptr;
				break;
			default:
				m_szBossNameElemKey = nullptr;
				break;
			}

			// LowerFrame 이지만 위에 깔려야함
			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_FRAME"));
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY - m_fScreenOffsetUpperLineY, m_fDefaultScale * m_fScaleOffset, m_fDefaultScale * m_fScaleOffset);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
			m_pModelCom->Render(1);

			// UpperFrame이지만 밑에 깔려야함
			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_UPPER_FRAME"));
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY + m_fScreenOffsetLowerLineY, m_fDefaultScale * m_fScaleOffset, m_fDefaultScale * m_fScaleOffset);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
			m_pModelCom->Render(1);

			// LowerFrame과 같은 Y값의 왼쪽
			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_SMALL_CIRCLE"));
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX - m_fScreenOffsetDownCircleX, m_fWorldToScreenY - m_fScreenOffsetUpperLineY - 3.f, m_fDefaultScale * m_fScaleOffset / 2.f, m_fDefaultScale * m_fScaleOffset / 2.f);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
			m_pModelCom->Render(1);

			// LowerFrame과 같은 Y값의 오른쪽
			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_LOWER_SMALL_CIRCLE"));
			SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
			SetUp_Transform(m_fWorldToScreenX + m_fScreenOffsetDownCircleX, m_fWorldToScreenY - m_fScreenOffsetUpperLineY - 3.f, m_fDefaultScale * m_fScaleOffset / 2.f, m_fDefaultScale * m_fScaleOffset / 2.f);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
			m_pModelCom->Render(1);

			if (nullptr != m_szBossNameElemKey)
			{
				SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szBossNameElemKey);
				SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
				SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY - m_fScreenOffsetUpperLineY - 20.f, 1.0f, 1.0f);
				SetUp_DefaultRawValue();

				m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
				m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
				m_pModelCom->Render(10);
			}
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_CONTENTS"));
		SetUp_WorldToScreen(XMLoadFloat4(&pDesc->vWorldPos));
		SetUp_Transform(m_fWorldToScreenX, m_fWorldToScreenY, m_fDFLHpBarScaleX * m_fScaleOffset, m_fDFLHpBarScaleY * m_fScaleOffset);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_fCrntHPUV", &pDesc->fCrntHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fPrevHPUV", &pDesc->fPrevHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(6);

		// 스케일 보정을 줘서 HPBar를 크게 그리는건 좋은데 그 경우 pos 보정도 거리가 멀어져야함.
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_Monster_Contents::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Hpbar_Monster_Contents::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIHPBARMONDESC*	pCopiedDesc = new UIHPBARMONDESC;

		memcpy(pCopiedDesc, (UIHPBARMONDESC*)pPacket->pData, sizeof(UIHPBARMONDESC));

		list<UIHPBARMONDESC*>::iterator	it = m_listRenderHPBar.begin();
		list<UIHPBARMONDESC*>::iterator ite = m_listRenderHPBar.end();

		UIHPBARMONDESC*	pMatchedDesc = nullptr;

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
			pMatchedDesc->vWorldPos = pCopiedDesc->vWorldPos;
			pMatchedDesc->dTimeDuration = pCopiedDesc->dTimeDuration;

			Safe_Delete(pCopiedDesc);
		}
		else
		{
			m_listRenderHPBar.emplace_back(pCopiedDesc);
		}
	}
}

_int CUI_HUD_Hpbar_Monster_Contents::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_HUD_MONSTER_HPBAR)
		{
			return FALSE;
		}
	}
	return TRUE;
}
