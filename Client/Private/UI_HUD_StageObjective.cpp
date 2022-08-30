#include "stdafx.h"
#include "..\Public\UI_HUD_StageObjective.h"
#include "Player.h"

CUI_HUD_StageObjective::CUI_HUD_StageObjective(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_StageObjective::CUI_HUD_StageObjective(const CUI_HUD_StageObjective & rhs)
	: CUI(rhs)
{
}

CUI_HUD_StageObjective * CUI_HUD_StageObjective::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_StageObjective* pInstance = new CUI_HUD_StageObjective(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_StageObjective");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_StageObjective::Clone(void * pArg)
{
	CUI_HUD_StageObjective* pInstance = new CUI_HUD_StageObjective(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_StageObjective");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_StageObjective::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayer);

	for (auto& pDesc : m_listObjectiveDesc)
	{
		Safe_Delete(pDesc);
	}
	m_listObjectiveDesc.clear();
}

HRESULT CUI_HUD_StageObjective::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_StageObjective::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));
	//XMStoreFloat4(&vColorOverlay, XMVectorSet(100.f, 100.f, 100.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -60.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-100.f, -100.f, -100.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// YorHa

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -255.f));
	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, -60.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Background

	m_iDistanceZ = 0;
	m_eUIState = CUI::UISTATE::DISABLE;

	//{
	//	UIOBJECTIVEDESC*	pDesc = new UIOBJECTIVEDESC;

	//	pDesc->eQuest = CUI_HUD_StageObjective::QUEST::TRESUREBOX;
	//	pDesc->iEventCount = 0;
	//	pDesc->iEventCountMax = 4;

	//	m_listObjectiveDesc.push_back(pDesc);
	//}
	//{
	//	UIOBJECTIVEDESC*	pDesc = new UIOBJECTIVEDESC;

	//	pDesc->eQuest = CUI_HUD_StageObjective::QUEST::ZHUANGZI;
	//	pDesc->iEventCount = 0;
	//	pDesc->iEventCountMax = 1;

	//	m_listObjectiveDesc.push_back(pDesc);
	//}
	//{
	//	UIOBJECTIVEDESC*	pDesc = new UIOBJECTIVEDESC;

	//	pDesc->eQuest = CUI_HUD_StageObjective::QUEST::ENGELS;
	//	pDesc->iEventCount = 0;
	//	pDesc->iEventCountMax = 1;

	//	m_listObjectiveDesc.push_back(pDesc);
	//}

	XMStoreFloat4x4(&m_matTextPosOffset, XMMatrixTranslationFromVector(XMVectorSet(-0.2f, 0.f, 0.f, 1.f)));

	return S_OK;
}

HRESULT CUI_HUD_StageObjective::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_StageObjective::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_StageObjective::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_StageObjective::Render()
{
	if (FAILED(SetUp_RenderParameter()))
	{
		return S_OK;
	}

	m_vBackgroundScale.y = m_vBackgroundScaleMax.y * m_fActivateScaleY;

	_matrix		matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vBackgroundScale));
	_matrix		matRotationY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotationRadian);
	_matrix		matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vBackgroundPos)) * XMMatrixTranslationFromVector(XMLoadFloat4(&m_vPlayerLook));

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CANVAS, TEXT("SIMPLE_RECT"));
	SetUp_Transform_ForAtlasPP(matScale * matRotationY * matTranslate);
	SetUp_DefaultRawvalue_ForAtlasPP();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CANVAS));
	m_pModelCom->Render(14);

	m_vYorhaMarkScale.y = m_vYorhaMarkScaleMax.y * m_fActivateScaleY;

	matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vYorhaMarkScale));
	matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vBackgroundPos)) * XMMatrixTranslationFromVector(0.99f * XMLoadFloat4(&m_vPlayerLook));

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::LOADING, TEXT("YORHA_ALL"));
	SetUp_Transform_ForAtlasPP(matScale * matRotationY * matTranslate);
	SetUp_DefaultRawvalue_ForAtlasPP();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::LOADING));
	m_pModelCom->Render(14);

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	if (CUI::UISTATE::ENABLE == m_eUIState)
	{
		m_vTextBasePos.y += 0.2f;

		matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vCircleLineScale));
		matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vTextBasePos)) * XMMatrixTranslationFromVector(0.96f * XMLoadFloat4(&m_vPlayerLook));

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("CIRCLE_LINE"));
		SetUp_Transform_ForAtlasPP(matScale * matRotationY * matTranslate);
		SetUp_DefaultRawvalue_ForAtlasPP();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(14);

		m_vTextBasePos.y -= 0.2f;

		for (auto& pDesc : m_listObjectiveDesc)
		{
			const _tchar*	szElemKey = SetUp_ElemKey(pDesc->eQuest);

			matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vTextScale));
			matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vTextBasePos)) * XMMatrixTranslationFromVector(0.98f * XMLoadFloat4(&m_vPlayerLook));

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, szElemKey);
			SetUp_Transform_AlignLeft_ForAtlasPP(XMLoadFloat4x4(&m_matTextPosOffset) * matScale * matRotationY * matTranslate);
			SetUp_DefaultRawvalue_ForAtlasPP();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(14);

			Render_Counter(pDesc);

			m_vTextBasePos.y -= 0.35f;
		}

		m_vTextBasePos.y += 0.15f;

		matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vCircleLineScale));
		matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vTextBasePos)) * XMMatrixTranslationFromVector(0.96f * XMLoadFloat4(&m_vPlayerLook));

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CORE, TEXT("CIRCLE_LINE"));
		SetUp_Transform_ForAtlasPP(matScale * matRotationY * matTranslate);
		SetUp_DefaultRawvalue_ForAtlasPP();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CORE));
		m_pModelCom->Render(14);
	}

	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Activate(_double dTimeDelta)
{
	if (m_fActivateScaleY < m_fActivateScaleMaxY)
	{
		m_fActivateScaleY += (_float)dTimeDelta * 4.f;
	}
	else
	{
		m_fActivateScaleY = m_fActivateScaleMaxY;
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Inactivate(_double dTimeDelta)
{
	if (m_fActivateScaleMinY < m_fActivateScaleY)
	{
		m_fActivateScaleY -= (_float)dTimeDelta * 4.f;
	}
	else
	{
		m_fActivateScaleY = m_fActivateScaleMinY;
		m_eUIState = CUI::UISTATE::DISABLE;
	}
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_StageObjective::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_StageObjective::Release_UI()
{
	for (auto& pDesc : m_listObjectiveDesc)
	{
		Safe_Delete(pDesc);
	}
	m_listObjectiveDesc.clear();
}

void CUI_HUD_StageObjective::SetUp_Player()
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	if (nullptr == m_pPlayer)
	{
		MSGBOX("Failed to Access Player - DirectionIndicator");
		return;
	}
	Safe_AddRef(m_pPlayer);
}

HRESULT	CUI_HUD_StageObjective::SetUp_RenderParameter()
{
	if (nullptr == m_pPlayer)
	{
		return E_FAIL;
	}

	CTransform*		pPlayerTransform = nullptr;

	pPlayerTransform = (CTransform*)m_pPlayer->Get_Component(COM_KEY_TRANSFORM);

	XMStoreFloat4(&m_vPlayerLook, pPlayerTransform->Get_State(CTransform::STATE::LOOK));

	_vector			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	
	const _vector	vPosOffset = XMVectorSet(0.f, 1.8f, 0.f, 0.f);

	XMStoreFloat4(&m_vBackgroundPos, vPlayerPos + vPosOffset);

	const _vector	vTextOffset = XMVectorSet(0.f, 2.15f, 0.f, 0.f);

	XMStoreFloat4(&m_vTextBasePos, vPlayerPos + vTextOffset);

	_float		fPlayerLookRadian = acosf(XMVectorGetX(XMVector3Dot(XMLoadFloat4(&m_vPlayerLook), XMVectorSet(0.f, 0.f, 1.f, 0.f))));
	_float		fPlayerLookCross = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Cross(XMLoadFloat4(&m_vPlayerLook), XMVectorSet(0.f, 0.f, 1.f, 0.f))));

	if (0 < fPlayerLookCross)
	{
		fPlayerLookRadian *= -1.f;
	}
	m_fRotationRadian = fPlayerLookRadian;

	return S_OK;
}

const _tchar * CUI_HUD_StageObjective::SetUp_ElemKey(QUEST eQuest)
{
	switch (eQuest)
	{
	case CUI_HUD_StageObjective::QUEST::TRESUREBOX:
		return TEXT("QUEST1");
	case CUI_HUD_StageObjective::QUEST::ZHUANGZI:
		return TEXT("QUEST2");
	case CUI_HUD_StageObjective::QUEST::ENGELS:
		return TEXT("QUEST3");
	case CUI_HUD_StageObjective::QUEST::CHARIOT:
		return TEXT("QUEST4");
	case CUI_HUD_StageObjective::QUEST::BEAUVOIR:
		return TEXT("QUEST5");
	case CUI_HUD_StageObjective::QUEST::NONE:
		break;
	default:
		break;
	}
	return nullptr;
}

void CUI_HUD_StageObjective::Render_Counter(UIOBJECTIVEDESC* pDesc)
{
	_matrix		matScale = XMMatrixScalingFromVector(XMLoadFloat4(&m_vTextScale));
	_matrix		matRotationY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRotationRadian);
	_matrix		matTranslate = XMMatrixTranslationFromVector(XMLoadFloat4(&m_vTextBasePos)) * XMMatrixTranslationFromVector(0.97f * XMLoadFloat4(&m_vPlayerLook));

	////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT(")"));
	SetUp_Transform_AlignRight_ForAtlasPP(XMMatrixTranslationFromVector(XMVectorSet(m_fRenderCounterLetterIntervalAccX, 0.f, 0.f, 1.f)) * matScale * matRotationY * matTranslate);
	SetUp_DefaultRawvalue_ForAtlasPP();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Font Inversed Color
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12);

	m_fRenderCounterLetterIntervalAccX -= m_fRenderCounterLetterIntervalX;

	////////////////////////////////////////////////////////////////////////////
	
	_uint	iBufferNumber = 0;
	_uint	iRenderNumber = 0;

	iBufferNumber = pDesc->iEventCountMax;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight_ForAtlasPP(XMMatrixTranslationFromVector(XMVectorSet(m_fRenderCounterLetterIntervalAccX, 0.f, 0.f, 1.f)) * matScale * matRotationY * matTranslate);
		SetUp_DefaultRawvalue_ForAtlasPP();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderCounterLetterIntervalAccX -= m_fRenderCounterLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderCounterLetterIntervalAccX -= 0.005f;

	////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("SLASH"));
	SetUp_Transform_AlignRight_ForAtlasPP(XMMatrixTranslationFromVector(XMVectorSet(m_fRenderCounterLetterIntervalAccX, 0.f, 0.f, 1.f)) * matScale * matRotationY * matTranslate);
	SetUp_DefaultRawvalue_ForAtlasPP();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Font Inversed Color
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12);

	m_fRenderCounterLetterIntervalAccX -= m_fRenderCounterLetterIntervalX;

	////////////////////////////////////////////////////////////////////////////

	m_fRenderCounterLetterIntervalAccX -= 0.005f;

	iRenderNumber = 0;
	iBufferNumber = pDesc->iEventCount;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight_ForAtlasPP(XMMatrixTranslationFromVector(XMVectorSet(m_fRenderCounterLetterIntervalAccX, 0.f, 0.f, 1.f)) * matScale * matRotationY * matTranslate);
		SetUp_DefaultRawvalue_ForAtlasPP();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderCounterLetterIntervalAccX -= m_fRenderCounterLetterIntervalX;
	} while (0 < iBufferNumber);

	////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("("));
	SetUp_Transform_AlignRight_ForAtlasPP(XMMatrixTranslationFromVector(XMVectorSet(m_fRenderCounterLetterIntervalAccX, 0.f, 0.f, 1.f)) * matScale * matRotationY * matTranslate);
	SetUp_DefaultRawvalue_ForAtlasPP();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Font Inversed Color
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12);

	m_fRenderCounterLetterIntervalAccX -= m_fRenderCounterLetterIntervalX;

	////////////////////////////////////////////////////////////////////////////

	m_fRenderCounterLetterIntervalAccX = m_fRenderCounterLetterIntervalDefaultX;
}

void CUI_HUD_StageObjective::Notify(void * pMessage)
{
	_int iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UIOBJECTIVEDESC*	pCopiedDesc = new UIOBJECTIVEDESC;

		memcpy(pCopiedDesc, (UIOBJECTIVEDESC*)pPacket->pData, sizeof(UIOBJECTIVEDESC));

		list<UIOBJECTIVEDESC*>::iterator	it = m_listObjectiveDesc.begin();
		list<UIOBJECTIVEDESC*>::iterator	ite = m_listObjectiveDesc.end();

		UIOBJECTIVEDESC*	pMatchedDesc = nullptr;

		while (it != ite)
		{
			if ((*it)->eQuest == pCopiedDesc->eQuest)
			{
				pMatchedDesc = (*it);
				break;
			}
			++it;
		}
		if (nullptr != pMatchedDesc)
		{			
			pMatchedDesc->iEventCount += pCopiedDesc->iEventCount;
			if (pMatchedDesc->iEventCountMax < pMatchedDesc->iEventCount)
				pMatchedDesc->iEventCount = pMatchedDesc->iEventCountMax;

			Safe_Delete(pCopiedDesc);
		}
		else
		{
			m_listObjectiveDesc.emplace_back(pCopiedDesc);
		}
	}
	else if (2 == iResult)
	{
		if (nullptr == m_pPlayer)
		{
			SetUp_Player();
		}

		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::INACTIVATE == pState) { m_eUIState = pState; }
			if (UISTATE::DISABLE == pState) { m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::INACTIVATE == pState) { m_eUIState = pState; }
			if (UISTATE::DISABLE == pState) { m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			if (UISTATE::DISABLE == pState) { m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ACTIVATE == pState) { m_eUIState = pState; }
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
	}
	else if (3 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	eState = *(UISTATE*)pPacket->pData;

		m_eUIState = eState;
	}
	else if (4 == iResult)
	{
		Release_UI();
	}
}

_int CUI_HUD_StageObjective::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_STAGE_OBJECTIVE)
		{
			return 1;
		}
		else if (*check == CHECKSUM_UI_HUD_STAGE_OBJECTIVE_STATE)
		{
			return 2;
		}
		if (*check == CHECKSUM_ONLY_UISTATE)
		{
			return 3;
		}
		if (*check == CHECKSUM_RELEASE_UI)
		{
			return 4;
		}
	}
	return FALSE;
}
