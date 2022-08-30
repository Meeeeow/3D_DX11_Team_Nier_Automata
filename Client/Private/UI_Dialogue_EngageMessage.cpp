#include "stdafx.h"
#include "..\Public\UI_Dialogue_EngageMessage.h"

CUI_Dialogue_EngageMessage::CUI_Dialogue_EngageMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Dialogue_EngageMessage::CUI_Dialogue_EngageMessage(const CUI_Dialogue_EngageMessage & rhs)
	: CUI(rhs)
{
}

CUI_Dialogue_EngageMessage * CUI_Dialogue_EngageMessage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Dialogue_EngageMessage* pInstance = new CUI_Dialogue_EngageMessage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Dialogue_EngageMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Dialogue_EngageMessage::Clone(void * pArg)
{
	CUI_Dialogue_EngageMessage* pInstance = new CUI_Dialogue_EngageMessage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Dialogue_EngageMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Dialogue_EngageMessage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Dialogue_EngageMessage::NativeConstruct_Prototype()
{
	if (FAILED(CUI::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Dialogue_EngageMessage::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(78.f, 74.f, 66.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// DecoLine

	XMStoreFloat4(&vColorOverlay, XMVectorSet(78.f, 74.f, 66.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Background

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Contents

	XMStoreFloat4(&vColorOverlay, XMVectorSet(194.f, 192.f, 168.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Contents : Line

	_float	fLetterOffsetX = 0.f;

	m_vecLetterOffsetX.push_back(fLetterOffsetX);
					 
	m_vecLetterOffsetX.push_back(fLetterOffsetX);
					 
	m_vecLetterOffsetX.push_back(fLetterOffsetX);
					 
	m_vecLetterOffsetX.push_back(fLetterOffsetX);

	m_iDistanceZ = 1;

	m_eUIState = CUI::UISTATE::DISABLE;

	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Dialogue_EngageMessage::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Dialogue_EngageMessage::LateTick(_double TimeDelta)
{
	if (m_eUIState < UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Dialogue_EngageMessage::Render()
{
	for (auto& vColor : m_vecColorOverlays)
	{
		vColor.w = m_fAlphaAnim;
	}

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(640.f, m_fUIPosY, 4.0f, 0.5f * m_fScaleYRatio);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(10);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("RACE_TOP"));
	SetUp_Transform(640.f, m_fUIPosY - m_fDecoLineOffsetY, 0.4f, 0.4f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(10);
	
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("RACE_BOTTOM"));
	SetUp_Transform(640.f, m_fUIPosY + m_fDecoLineOffsetY, 0.4f, 0.4f);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(10);
	
	//_float fResultScale = 1.5f * m_fScaleYRatioForFont;
	_float fResultScale = 1.5f * m_fScaleYRatio;

	{
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("-"));
		SetUp_Transform(640.f - m_vecLetterOffsetX[3], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("W"));
		SetUp_Transform(640.f - m_vecLetterOffsetX[2], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("A"));
		SetUp_Transform(640.f - m_vecLetterOffsetX[1], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("R"));
		SetUp_Transform(640.f - m_vecLetterOffsetX[0], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("N"));
		SetUp_Transform(640.f, m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("I"));
		SetUp_Transform(640.f + m_vecLetterOffsetX[0], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("N"));
		SetUp_Transform(640.f + m_vecLetterOffsetX[1], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("G"));
		SetUp_Transform(640.f + m_vecLetterOffsetX[2], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, TEXT("-"));
		SetUp_Transform(640.f + m_vecLetterOffsetX[3], m_fUIPosY - 30.f, 1.5f, fResultScale);
		SetUp_DefaultRawValue();

		m_pModelCom->Render(10);
	}

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("WARNING_LINE"));
	SetUp_Transform(640.f, m_fUIPosY, 0.4f, 0.4f * m_fScaleYRatio);
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[3], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
	m_pModelCom->Render(10);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szBossNameElemKey);
	SetUp_Transform(640.f, m_fUIPosY + m_fLetterOffsetY + 60.f, 1.5f, fResultScale);
	SetUp_DefaultRawValue();	

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[2], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
	m_pModelCom->Render(10);

	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Activate(_double dTimeDelta)
{
	m_fAlphaAnim += _float(m_dAnimSpeed * dTimeDelta);

	_float	fStep = 1.f;

	// 각 글자들이 받을 offset값을 TimeDelta * Speed * 배율만큼 업데이트
	for (auto& fVal : m_vecLetterOffsetX)
	{
		fVal += m_fLetterMoveSpeedX * (_float)dTimeDelta * fStep;

		fStep += 1.f;
	}

	if (m_fLetterOffsetY < m_fLetterOffsetYMax)
	{
		m_fLetterOffsetY -= m_fLetterMoveSpeedY * (_float)dTimeDelta;
	}
	else
	{
		m_fLetterOffsetY = m_fLetterOffsetYMax;
	}

	if (255.f < m_fAlphaAnim)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Enable(_double dTimeDelta)
{
	m_dEnableDurationAcc += dTimeDelta;

	if (m_dEnableDurationTime < m_dEnableDurationAcc)
	{
		m_eUIState = CUI::UISTATE::INACTIVATE;
		m_dEnableDurationAcc = 0.0;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Inactivate(_double dTimeDelta)
{
	m_fAlphaAnim -= _float(m_dAnimSpeed / 2.0 * dTimeDelta);

	m_fDecoLineOffsetY -= (_float)dTimeDelta * 100.f;

	m_fScaleYRatio -= _float(dTimeDelta);

	//if (0.f < m_fScaleYRatioForFont)
	//{
	//	m_fScaleYRatioForFont -= _float(dTimeDelta) * 2.0f;	// 일반 텍스처의 2배속으로 글자 쪼그라들게 하기
	//}
	//else
	//{
	//	m_fScaleYRatioForFont = 0.f;
	//}
	// 글자만 2배속하는게 안 된다..... 왜인지는 모름...

	// 종료했을 경우 패러미터 원상복구
	if (m_fScaleYRatio < 0.f)
	{
		for (auto& fVal : m_vecLetterOffsetX)
		{
			fVal = 0.f;
		}
		m_fLetterOffsetY = 0.f;

		m_fScaleYRatio = 1.f;

		//m_fScaleYRatioForFont = 1.f;

		m_fDecoLineOffsetY = 100.f;

		m_fAlphaAnim = 0.f;

		m_eUIState = CUI::UISTATE::DISABLE;
	}

	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_EngageMessage::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Dialogue_EngageMessage::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		BOSSID	eID = *(BOSSID*)pPacket->pData;

		if (UISTATE::DISABLE == m_eUIState)
		{
			if (BOSSID::NONE != eID)
			{
				m_eUIState = UISTATE::ACTIVATE;
				m_eBossID = eID;

				switch (m_eBossID)
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
			}
		}
	}
}

_int CUI_Dialogue_EngageMessage::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_DIALOGUE_ENGAGEMSG)
		{
			return TRUE;
		}
	}
	return FALSE;
}
