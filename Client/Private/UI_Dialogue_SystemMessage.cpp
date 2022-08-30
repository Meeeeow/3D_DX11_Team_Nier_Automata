#include "stdafx.h"
#include "..\Public\UI_Dialogue_SystemMessage.h"
#include "SoundMgr.h"

CUI_Dialogue_SystemMessage::CUI_Dialogue_SystemMessage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Dialogue_SystemMessage::CUI_Dialogue_SystemMessage(const CUI_Dialogue_SystemMessage & rhs)
	: CUI(rhs)
{
}

CUI_Dialogue_SystemMessage * CUI_Dialogue_SystemMessage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Dialogue_SystemMessage* pInstance = new CUI_Dialogue_SystemMessage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Dialogue_SystemMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Dialogue_SystemMessage::Clone(void * pArg)
{
	CUI_Dialogue_SystemMessage* pInstance = new CUI_Dialogue_SystemMessage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Dialogue_SystemMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Dialogue_SystemMessage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pDesc : m_listRenderSysMsg)
	{
		Safe_Delete(pDesc);
	}
	m_listRenderSysMsg.clear();
}

HRESULT CUI_Dialogue_SystemMessage::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Dialogue_SystemMessage::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 1;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_SYSMSG"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Dialogue_SystemMessage::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Dialogue_SystemMessage::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Dialogue_SystemMessage::Render()
{
	for (auto& pDesc : m_listRenderSysMsg)
	{
		_tchar*	szIconElemKey = nullptr;
		_tchar*	szTextElemKey = nullptr;
		_tchar*	szItemElemKey = nullptr;

		// 태그 출력
		switch (pDesc->eDialogueState)
		{
		case DIALOGUESTATE::CURRENT:
			szIconElemKey = TEXT("SYSMSG_ACTIVE");
			break;
		case DIALOGUESTATE::PREVIOUS:
			szIconElemKey = TEXT("SYSMSG_DEACTIVE");
			break;
		default:
			break;
		}

		// 메시지 종류 출력
		switch (pDesc->iMsgCategory)
		{
		case iMsgUsed:
			szTextElemKey = TEXT("USED");
			break;
		case iMsgEquiped:
			szTextElemKey = TEXT("EQUIPED");
			break;
		case iMsgAchieved:
			szTextElemKey = TEXT("ACHIEVED");
			break;
		case iMsgOnQuest:
			szTextElemKey = TEXT("ON_QUEST");
			break;
		case iMsgNeedChip:
			szTextElemKey = TEXT("NEED_CHIP");
			break;
		default:
			break;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, szIconElemKey);
		SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
		SetUp_DefaultRawValue();

		m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(1);

		m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left) + 10.f;

		if (TEXT("NEED_CHIP") == szTextElemKey)
		{
			Find_ElemKey(pDesc->iItemCode, &szItemElemKey);

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, szItemElemKey);
			SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
			SetUp_DefaultRawValue();

			m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;
			m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(1);

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("-"));
			SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
			SetUp_DefaultRawValue();

			m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;
			m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(1);
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, szTextElemKey);
		SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
		SetUp_DefaultRawValue();

		m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;
		m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(1);		

		if (TEXT("ON_QUEST") == szTextElemKey || TEXT("NEED_CHIP") == szTextElemKey)
		{
			m_fAdditionalTextOffsetX = 0.f;
			continue;
		}

		// '개' 출력
		if (0 < pDesc->iItemCount)
		{
			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("COUNT"));
			SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
			SetUp_DefaultRawValue();

			m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;
			m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(1);

			
			// 아이템 갯수 출력
			m_iBufferNumber = pDesc->iItemCount;

			do
			{
				m_iRenderNumber = m_iBufferNumber % 10;
				m_iBufferNumber = m_iBufferNumber / 10;

				_tchar	buf[2];

				ZeroMemory(buf, sizeof(_tchar) * 2);
				_stprintf_s(buf, TEXT("%d"), m_iRenderNumber);

				SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
				SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
				SetUp_DefaultRawValue();

				m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;

				m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
				m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
				m_pModelCom->Render(1);

				m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);
			} while (0 < m_iBufferNumber);
		}

		// 아이템 코드 출력
		Find_ElemKey(pDesc->iItemCode, &szItemElemKey);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, szItemElemKey);

		m_fAdditionalTextOffsetX -= _float(m_ScreenPosRect.right - m_ScreenPosRect.left);

		SetUp_Transform_AlignRight(m_fBasePositionX + m_fAdditionalTextOffsetX, m_fBasePosY + pDesc->fOffsetPosY, m_fBaseScale, m_fBaseScale);
		SetUp_DefaultRawValue();

		m_vecColorOverlays[0].w = pDesc->fExpiredAlpha;

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(1);

		m_fAdditionalTextOffsetX = 0.f;
	}
	return S_OK;
}

void CUI_Dialogue_SystemMessage::Find_ElemKey(_uint iItemCode, _tchar ** szElemKey)
{
	switch (iItemCode)
	{
	case ITEMCODE_MONEY:
		*szElemKey = TEXT("GOLD");
		break;
	case ITEMCODE_EXPENDABLES_RECOVERY_SMALL:
		*szElemKey = TEXT("POTION_SMALL");
		break;
	case ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE:
		*szElemKey = TEXT("POTION_MIDDLE");
		break;
	case ITEMCODE_EXPENDABLES_RECOVERY_LARGE:
		*szElemKey = TEXT("POTION_LARGE");
		break;
	case ITEMCODE_EXPENDABLES_RECOVERY_EXLARGE:
		*szElemKey = TEXT("POTION_EXLARGE");
		break;
	case ITEMCODE_RESOURCES_CHUNK:
		*szElemKey = TEXT("CHUNK");
		break;
	case ITEMCODE_RESOURCES_COPPER:
		*szElemKey = TEXT("COPPER");
		break;
	case ITEMCODE_RESOURCES_STEEL:
		*szElemKey = TEXT("STEEL");
		break;
	case ITEMCODE_RESOURCES_AMBER:
		*szElemKey = TEXT("AMBER");
		break;
	case ITEMCODE_RESOURCES_SILVER:
		*szElemKey = TEXT("SILVER");
		break;
	case ITEMCODE_RESOURCES_MOLDABYTE:
		*szElemKey = TEXT("MOLDABYTE");
		break;
	case ITEMCODE_RESOURCES_METEORITE:
		*szElemKey = TEXT("METEORITE");
		break;
	case ITEMCODE_RESOURCES_PEARL:
		*szElemKey = TEXT("PEARL");
		break;
	case ITEMCODE_RESOURCES_TITAN:
		*szElemKey = TEXT("TITAN");
		break;
	case ITEMCODE_RESOURCES_SM_ALLOY:
		*szElemKey = TEXT("SM_ALLOY");
		break;
	case ITEMCODE_ENFORCECHIP_MASTERY_SHORTSWORD:
		*szElemKey = TEXT("MASTERY_SHORTSWORD");
		break;
	case ITEMCODE_ENFORCECHIP_MASTERY_LONGSWORD:
		*szElemKey = TEXT("MASTERY_LONGSWORD");
		break;
	case ITEMCODE_ENFORCECHIP_MASTERY_SPEAR:
		*szElemKey = TEXT("MASTERY_SPEAR");
		break;
	case ITEMCODE_ENFORCECHIP_MASTERY_GAUNTLET:
		*szElemKey = TEXT("MASTERY_GAUNTLET");
		break;
	case ITEMCODE_ENFORCECHIP_SKILL_LASER:
		*szElemKey = TEXT("SKILL_LASER");
		break;
	case ITEMCODE_ENFORCECHIP_SKILL_LIGHT:
		*szElemKey = TEXT("SKILL_LIGHT");
		break;
	case ITEMCODE_ENFORCECHIP_SKILL_OS:
		*szElemKey = TEXT("SKILL_OS");
		break;
	case ITEMCODE_ENFORCECHIP_SKILL_MACHINEGUN:
		*szElemKey = TEXT("SKILL_MACHINEGUN");
		break;
	case ITEMCODE_ENFORCECHIP_SKILL_HANG:
		*szElemKey = TEXT("SKILL_HANG");
		break;
	case ITEMCODE_EQUIPMENT_WHITE_COVENANT:
		*szElemKey = TEXT("WHITE_COVENANT");
		break;
	case ITEMCODE_EQUIPMENT_WHITE_CONTRACT:
		*szElemKey = TEXT("WHITE_CONTRACT");
		break;
	case ITEMCODE_EQUIPMENT_40TH_SPEAR:
		*szElemKey = TEXT("40TH_SPEAR");
		break;
	case ITEMCODE_EQUIPMENT_40TH_KNUCKLE:
		*szElemKey = TEXT("40TH_KNUCKLE");
		break;
	default:
		break;
	}

	return;
}

HRESULT CUI_Dialogue_SystemMessage::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::Enable(_double dTimeDelta)
{
	auto& it = m_listRenderSysMsg.begin();
	auto& ite = m_listRenderSysMsg.end();

	while (it != ite)
	{
		UISYSMSGDESC*	pDesc = *it;

		pDesc->dTimeDurationMax -= dTimeDelta;

		// 5줄 이상 로그가 뜰 경우 즉시 삭제
		if (800.f < pDesc->fOffsetPosY)
		{
			pDesc->dTimeDurationMax = -44.44;
		}

		// 유효시간이 끝나면 Alpha값을 빠르게 낮춰서 렌더
		if (pDesc->dTimeDurationMax < 0.0)
		{
			pDesc->fExpiredAlpha -= m_fExpiredAlphaSpeed * (_float)dTimeDelta;
		}

		// LineCount 1개당 0.5초 동안 OffsetY 추가 부여.
		if (0 < pDesc->iOffsetLineCount)
		{

			pDesc->fOffsetPosY -= m_fOffsetYSpeed * (_float)dTimeDelta;
			pDesc->dTextPushTimeAcc += dTimeDelta;
			if (m_dTimeTextPushMax < pDesc->dTextPushTimeAcc)
			{
				pDesc->dTextPushTimeAcc = 0.0;
				--pDesc->iOffsetLineCount;
			}
		}

		// Alpha값이 0이 되면 컨테이너에서 제거
		if (pDesc->fExpiredAlpha < -255.0)
		{
			Safe_Delete(*it);
			it = m_listRenderSysMsg.erase(it);
		}
		else
		{
			++it;
		}
	}
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_SystemMessage::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Dialogue_SystemMessage::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISYSMSGDESC*	pCopiedDesc = new UISYSMSGDESC;

		memcpy(pCopiedDesc, (UISYSMSGDESC*)pPacket->pData, sizeof(UISYSMSGDESC));

		if (nullptr != pCopiedDesc)
		{
			list<UISYSMSGDESC*>::iterator	it = m_listRenderSysMsg.begin();
			list<UISYSMSGDESC*>::iterator	ite = m_listRenderSysMsg.end();

			while (it != ite)
			{
				UISYSMSGDESC*	pDesc = (*it);

				if (DIALOGUESTATE::CURRENT == pDesc->eDialogueState)
				{
					pDesc->eDialogueState = DIALOGUESTATE::PREVIOUS;
				}
				++pDesc->iOffsetLineCount;

				++it;
			}
		}

		if (pCopiedDesc->iMsgCategory == iMsgOnQuest)
		{
			CSoundMgr::Get_Instance()->PlaySound(L"OnQuest.mp3", CSoundMgr::CHANNELID::UI);
		}
		m_listRenderSysMsg.emplace_back(pCopiedDesc);
	}
}

_int CUI_Dialogue_SystemMessage::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_DIALOGUE_SYSMSG)
		{
			return TRUE;
		}
	}
	return FALSE;
}
