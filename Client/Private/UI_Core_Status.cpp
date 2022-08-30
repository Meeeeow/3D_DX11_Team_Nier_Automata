#include "stdafx.h"
#include "..\Public\UI_Core_Status.h"

CUI_Core_Status::CUI_Core_Status(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Status::CUI_Core_Status(const CUI_Core_Status & rhs)
	: CUI(rhs)
{
}

CUI_Core_Status * CUI_Core_Status::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Status* pInstance = new CUI_Core_Status(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Status");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Status::Clone(void * pArg)
{
	CUI_Core_Status* pInstance = new CUI_Core_Status(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Status");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Status::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_Status::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_Status::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Status::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Background Default : 224, 223, 211, 128

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-145.f, -145.f, -145.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Head

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-20.f, -20.f, -20.f, 50.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Body

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 2;
	m_bCollision = false;
	
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_STATUS"), this)))
	{
		return E_FAIL;
	}	
	return S_OK;
}

HRESULT CUI_Core_Status::SetUp_Components()
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
	return S_OK;
}

_int CUI_Core_Status::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Status::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_Status::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fRenderPosBaseX, 440.f, 1.1f, 1.21f);	// 352 * 387
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_fUVAnimY", (void*)&m_fHeaderUVRangeY, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));	// Head
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay2", &m_vecColorOverlays[1], sizeof(_float4));	// Body
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(13);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("STATUS"));
	SetUp_Transform(m_fRenderPosBaseX, 265.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("2B"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 40.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("LEVEL"));
	SetUp_Transform(m_fRenderPosBaseX + 90.f, m_fRenderPosBaseY + 40.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_Number(121, 1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("SIMPLE_LINE"));
	SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + 80.f, 1.f, 0.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
	m_pModelCom->Render(1);
	
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("HP"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 120.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_HP(m_tDesc.iAmountHp, m_tDesc.iAmountMaxHp, 3);
	
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("ATT_LIGHT"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 160.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_Number(m_tDesc.iAmountAtt, 4);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("ATT_SHOOT"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 200.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_Number(m_tDesc.iAmountAttShoot, 5);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("DEF"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 240.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_Number(m_tDesc.iAmountDef, 6);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("SIMPLE_LINE"));
	SetUp_Transform(m_fRenderPosBaseX, m_fRenderPosBaseY + 280.f, 1.f, 0.2f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
	m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("MONEY"));
	SetUp_Transform_AlignLeft(m_fRenderPosBaseX - m_fRenderTextPosOffsetX, m_fRenderPosBaseY + 320.f, 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(1);

	Render_Number(m_tDesc.iAmountMoney, 8);

	return S_OK;
}

HRESULT CUI_Core_Status::Activate(_double dTimeDelta)
{
	if (m_fActivateAlpha < m_fActivateAlphaMax)
	{
		m_fActivateAlpha += (_float)dTimeDelta * 128.f;
	}
	else
	{
		m_fActivateAlpha = m_fActivateAlphaMax;
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Core_Status::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Status::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Status::Disable(_double dTimeDelta)
{
	m_fActivateAlpha = m_fActivateAlphaMin;

	return S_OK;
}

HRESULT CUI_Core_Status::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Status::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Status::Render_Number(_uint iAmount, _uint iLineNumber)
{
	_uint	iBufferNumber = 0;
	_uint	iRenderNumber = 0;

	iBufferNumber = iAmount;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX = 0.f;
}

void CUI_Core_Status::Render_HP(_uint iCrnt, _uint iMax, _uint iLineNumber)
{
	_uint	iBufferNumber = 0;
	_uint	iRenderNumber = 0;

	iBufferNumber = iMax;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX -= 10.f;

	////////////////////////////////////////////////////////////////////////////

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("SLASH"));
	SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(12);

	m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;

	////////////////////////////////////////////////////////////////////////////

	m_fRenderAmountLetterIntervalAccX -= 10.f;

	iRenderNumber = 0;
	iBufferNumber = iCrnt;

	do
	{
		iRenderNumber = iBufferNumber % 10;
		iBufferNumber = iBufferNumber / 10;

		_tchar	buf[2];

		ZeroMemory(buf, sizeof(_tchar) * 2);
		_stprintf_s(buf, TEXT("%d"), iRenderNumber);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, buf);
		SetUp_Transform_AlignRight(m_fRenderPosBaseX + m_fRenderAmountPosOffsetX + m_fRenderAmountLetterIntervalAccX, m_fRenderPosBaseY + (m_fRenderIntervalPosOffsetY * (_float)iLineNumber), 1.f, 1.f);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));	// Font Inversed Color
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(12);

		m_fRenderAmountLetterIntervalAccX -= m_fRenderAmountLetterIntervalX;
	} while (0 < iBufferNumber);

	m_fRenderAmountLetterIntervalAccX = 0.f;
}

void CUI_Core_Status::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::ENABLE:
			if (UISTATE::DISABLE == pState)		{ m_eUIState = pState; }
			break;
		case Client::CUI::UISTATE::INACTIVATE:
			break;
		case Client::CUI::UISTATE::DISABLE:
			if (UISTATE::ENABLE == pState)		{ m_eUIState = pState; }
			if (UISTATE::ACTIVATE == pState)	{ m_eUIState = pState; }
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
	else if (2 == iResult)
	{
		if (CUI::UISTATE::ENABLE == m_eUIState || CUI::UISTATE::ACTIVATE == m_eUIState)
		{
			PACKET*	pPacket = (PACKET*)pMessage;

			UISTATUSDESC tDesc = *(UISTATUSDESC*)pPacket->pData;

			memcpy(&m_tDesc, &tDesc, sizeof(UISTATUSDESC));
		}
	}
}

_int CUI_Core_Status::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_STATUS)
		{
			return 1;
		}
		if (*check == CHECKSUM_UI_CORE_STATUS_UPDATE)
		{
			return 2;
		}
	}
	return FALSE;
}

