#include "stdafx.h"
#include "..\Public\UI_Dialogue_HackingMessage.h"

const _tchar*	CUI_Dialogue_HackingMessage::LETTERELEMKEY[] = {
	TEXT("HACKING_-"),
	TEXT("HACKING_H"),
	TEXT("HACKING_A"),
	TEXT("HACKING_C"),
	TEXT("HACKING_K"),
	TEXT("HACKING_I"),
	TEXT("HACKING_N"),
	TEXT("HACKING_G"),
	TEXT("HACKING_SPACE"),
	TEXT("HACKING_S"),
	TEXT("HACKING_T"),
	TEXT("HACKING_A"),
	TEXT("HACKING_R"),
	TEXT("HACKING_T"),
	TEXT("HACKING_-")
};

const	_uint		CUI_Dialogue_HackingMessage::LETTERMAXCOUNT = sizeof(CUI_Dialogue_HackingMessage::LETTERELEMKEY) / sizeof(_tchar*);
const	_float		CUI_Dialogue_HackingMessage::LETTEROFFSETX = 30.f;

CUI_Dialogue_HackingMessage::CUI_Dialogue_HackingMessage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Dialogue_HackingMessage::CUI_Dialogue_HackingMessage(const CUI_Dialogue_HackingMessage & rhs)
	: CUI(rhs)
{
}

CUI_Dialogue_HackingMessage * CUI_Dialogue_HackingMessage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Dialogue_HackingMessage* pInstance = new CUI_Dialogue_HackingMessage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Dialogue_HackingMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Dialogue_HackingMessage::Clone(void * pArg)
{
	CUI_Dialogue_HackingMessage* pInstance = new CUI_Dialogue_HackingMessage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Dialogue_HackingMessage");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Dialogue_HackingMessage::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Dialogue_HackingMessage::NativeConstruct_Prototype()
{
	if (FAILED(CUI::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Dialogue_HackingMessage::SetUp_Components()))
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

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font : Origin

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font : Origin2


	for (_uint i = 0; i < m_iLetterOffsetCount; ++i)
	{
		m_vecLetterOffsetX.push_back(0.f);
	}

	m_iDistanceZ = 1;

	m_eUIState = CUI::UISTATE::DISABLE;

	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_DIALOGUE_HACKING"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Dialogue_HackingMessage::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Dialogue_HackingMessage::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Dialogue_HackingMessage::Render()
{
	if (false == m_bIsHackingStart)
	{
		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, m_szTextElemKey);
		SetUp_Transform(m_fBasePosX, m_fBasePosY, m_fTextScale, m_fTextScale);
		SetUp_DefaultRawValue();

		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[4], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
		m_pModelCom->Render(1);
	}
	else
	{
		for (_uint i = 0; i < LETTERMAXCOUNT; ++i)
		{
			_uint	iOffsetIndex = 0;
			_float	fSign = 0;

			if (i < m_iLetterOffsetCount - 1)	// i < 7
			{
				fSign = -1.0;
				iOffsetIndex = m_iLetterOffsetCount - i - 1;	// 8 - 6 - 1
			}
			else if (m_iLetterOffsetCount - 1 < i)	// 7 < i
			{
				fSign = 1.0;
				iOffsetIndex = i - (m_iLetterOffsetCount - 1);	// 8 - 8 - 1
			}

			SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::FONT, LETTERELEMKEY[i]);
			SetUp_Transform(m_fBasePosX + fSign * m_vecLetterOffsetX[iOffsetIndex], m_fBasePosY, m_fHackingTextScale, m_fHackingTextScale);
			SetUp_DefaultRawValue();

			m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[5], sizeof(_float4));
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::FONT));
			m_pModelCom->Render(1);
		}
	}
	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Activate(_double dTimeDelta)
{
	if (m_dCountDownAcc < 1.0)
	{
		m_dCountDownAcc += dTimeDelta;

		if (m_fTextScaleMin < m_fTextScale)
		{
			m_fTextScale -= (_float)dTimeDelta * m_fTextScaleSpeed;
		}
		else
		{
			m_fTextScale = m_fTextScaleMin;
		}
		if (m_fTextAlpha < m_fTextAlphaMax)
		{
			m_fTextAlpha += (_float)dTimeDelta * m_fTextAlphaSpeed;
		}
		else
		{
			m_fTextAlpha = m_fTextAlphaMax;
		}
		m_vecColorOverlays[4].w = m_fTextAlpha;
	}
	else
	{
		m_dCountDownAcc = 0.0;
		++m_iTimeCount;
		if (m_iTextIndex < m_iTextIndexMax)
		{
			++m_iTextIndex;
		}
		m_fTextAlpha = m_fTextAlphaMin;
		m_fTextScale = m_fTextScaleMax;
	}

	if (true == m_bIsHackingStart)
	{
		if (m_fLetterAnimTimeAcc < m_fLetterAnimTimeMax)
		{
			m_fLetterAnimTimeAcc += dTimeDelta;

			_float	fStep = 1.f;

			for (_uint i = 1; i < m_vecLetterOffsetX.size(); ++i)
			{
				m_vecLetterOffsetX[i] += m_fLetterMoveSpeedX * (_float)dTimeDelta * fStep;

				fStep += 1.f;
			}
		}
	}

	switch (m_iTimeCount)
	{
	case 0:
		m_szTextElemKey = TEXT("HACKINGTHREE");
		break;
	case 1:
		m_szTextElemKey = TEXT("HACKINGTWO");
		break;
	case 2:
		m_szTextElemKey = TEXT("HACKINGONE");
		break;
	case 3:
		m_bIsHackingStart = true;
		break;
	case 4:
		m_eUIState = UISTATE::INACTIVATE;
		break;
	};

	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Inactivate(_double dTimeDelta)
{
	//if (m_fLetterAlpha < m_fLetterAlphaMax)
	//{
	//	m_fLetterAlpha += dTimeDelta * m_fLetterAlphaSpeed;
	//}
	//else
	//{
	//	m_fLetterAlpha = m_fLetterAlphaMax;
	//	m_eUIState = CUI::UISTATE::DISABLE;
	//}
	//m_vecColorOverlays[5].w = m_fLetterAlpha * (-1.f);

	//if (m_fHackingTextScaleMin < m_fHackingTextScale)
	//{
	//	m_fHackingTextScale -= dTimeDelta;
	//}
	//else
	//{
	//	m_fHackingTextScale = m_fHackingTextScaleMin;
	//}

	if (m_fLetterAlpha < m_fLetterAlphaMax)
	{
		m_fLetterAlpha += (_float)dTimeDelta * m_fLetterAlphaSpeed;
	}
	else
	{
		m_fLetterAlpha = m_fLetterAlphaMax;
		m_eUIState = CUI::UISTATE::DISABLE;
	}
	m_vecColorOverlays[5].w = m_fLetterAlpha * (-1.f);

	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Disable(_double dTimeDelta)
{
	if (true == m_bIsHackingStart)
	{
		m_dCountDownAcc = 0.0;
		m_iTextIndex = 0;

		m_szTextElemKey = TEXT("HACKINGTHREE");
		m_fTextAlpha = m_fTextAlphaMin;
		m_fTextScale = m_fTextScaleMax;

		for (auto& fOffset : m_vecLetterOffsetX)
		{
			fOffset = 0.f;
		}
		m_fLetterAnimTimeAcc = 0.0;
		m_bIsHackingStart = false;
		m_iTimeCount = 0;

		m_fLetterAlpha = m_fLetterAlphaMin;
		m_vecColorOverlays[5].w = m_fLetterAlpha;

		m_fHackingTextScale = m_fHackingTextScaleMin;
	}
	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Dialogue_HackingMessage::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Dialogue_HackingMessage::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		if (UISTATE::DISABLE == m_eUIState)
		{
			m_eUIState = UISTATE::ACTIVATE;
		}
	}
}

_int CUI_Dialogue_HackingMessage::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_DIALOGUE_HACKINGMSG)
		{
			return TRUE;
		}
	}
	return FALSE;
}
