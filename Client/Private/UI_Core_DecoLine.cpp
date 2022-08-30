#include "stdafx.h"
#include "..\Public\UI_Core_DecoLine.h"

CUI_Core_DecoLine::CUI_Core_DecoLine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_DecoLine::CUI_Core_DecoLine(const CUI_Core_DecoLine & rhs)
	: CUI(rhs)
{
}

CUI_Core_DecoLine * CUI_Core_DecoLine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_DecoLine* pInstance = new CUI_Core_DecoLine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_DecoLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_DecoLine::Clone(void * pArg)
{
	CUI_Core_DecoLine* pInstance = new CUI_Core_DecoLine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_DecoLine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_DecoLine::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_DecoLine::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_DecoLine::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_DecoLine::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(80.f, 77.f, 66.f, 240.f));
	
	m_vecColorOverlays.push_back(vColorOverlay);	// DecoLine, Same as Pressed Button

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 2;
	m_bCollision = false;
	
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_DECOLINE"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
	
	return S_OK;
}

HRESULT CUI_Core_DecoLine::SetUp_Components()
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

_int CUI_Core_DecoLine::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_DecoLine::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_DecoLine::Render()
{
	// 원과 직선의 경우 포토샵에서 뽑을 때 안티앨리어싱 적용해서 뽑아야함
	// reference : https://creativestudio.kr/2585 

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("CIRCLE"));
	//SetUp_Transform(0.f, 0.f, 1.00f, 1.00f);
	//SetUp_DefaultRawValue();

	//m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	//m_pModelCom->Render(1);

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("CIRCLE"));
	//SetUp_Transform(0.f, 0.f, 1.05f, 1.05f);
	//SetUp_DefaultRawValue();

	//m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	//m_pModelCom->Render(1);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("RACE_BOTTOM"));
	SetUp_Transform(640.f, 90.f, 0.5f, 0.5f);
	SetUp_DefaultRawValue();

	_bool	bIsRightToLeft = true;

	m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimR2LX, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_bIsRtoL", &bIsRightToLeft, sizeof(_bool));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(3);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("RACE_BOTTOM"));
	SetUp_Transform(640.f, 680.f, 0.5f, 0.5f);
	SetUp_DefaultRawValue();
	
	bIsRightToLeft = false;

	m_pModelCom->SetUp_RawValue("g_fUVAnimX", &m_fUVAnimL2RX, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_bIsRtoL", &bIsRightToLeft, sizeof(_bool));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(3);

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("CIRCLE"));
	//SetUp_Transform(1280.f, 720.f, 1.f, 1.f);
	//SetUp_DefaultRawValue();

	//m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	//m_pModelCom->Render(1);

	//SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("CIRCLE"));
	//SetUp_Transform(1280.f, 720.f, 1.05f, 1.05f);
	//SetUp_DefaultRawValue();

	//m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	//m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	//m_pModelCom->Render(1);
	//
	return S_OK;
}

HRESULT CUI_Core_DecoLine::Activate(_double dTimeDelta)
{
	DWORD	dwFlag = 0;

	if (m_fUVAnimL2RX < 1.f)
	{
		m_fUVAnimL2RX += (_float)dTimeDelta;

		if (1.f < m_fUVAnimL2RX)
		{
			m_fUVAnimL2RX = 1.f;

			dwFlag |= 1;
		}
	}

	if (0.f < m_fUVAnimR2LX)
	{
		m_fUVAnimR2LX = 1.f - m_fUVAnimL2RX;
	}
	else
	{
		m_fUVAnimR2LX = 0.f;
		dwFlag |= 2;
	}

	if (dwFlag & 1 && dwFlag & 2)
	{
		m_eUIState = CUI::UISTATE::ENABLE;
	}
	return S_OK;
}

HRESULT CUI_Core_DecoLine::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_DecoLine::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_DecoLine::Disable(_double dTimeDelta)
{
	m_fUVAnimL2RX = 0.f;
	m_fUVAnimR2LX = 1.f;

	return S_OK;
}

HRESULT CUI_Core_DecoLine::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_DecoLine::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_DecoLine::Release_UI()
{
	m_fUVAnimL2RX = 0.f;
	m_fUVAnimR2LX = 1.f;
}

void CUI_Core_DecoLine::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	pState = *(UISTATE*)pPacket->pData;

		switch (m_eUIState)
		{
		case Client::CUI::UISTATE::ACTIVATE:
			if (UISTATE::DISABLE == pState)
			{
				Release_UI();
				m_eUIState = pState;
			}
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
}

_int CUI_Core_DecoLine::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_DECOLINE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
