#include "stdafx.h"
#include "..\Public\UI_Core_Cursor.h"

CUI_Core_Cursor::CUI_Core_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_Cursor::CUI_Core_Cursor(const CUI_Core_Cursor & rhs)
	: CUI(rhs)
{
}

CUI_Core_Cursor * CUI_Core_Cursor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_Cursor* pInstance = new CUI_Core_Cursor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_Cursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_Cursor::Clone(void * pArg)
{
	CUI_Core_Cursor* pInstance = new CUI_Core_Cursor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_Cursor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_Cursor::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}

HRESULT CUI_Core_Cursor::NativeConstruct_Prototype()
{
	if (FAILED(CUI::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_Cursor::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_Cursor::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_eUIState = CUI::UISTATE::DISABLE;

	ZeroMemory(&m_MousePos, sizeof(POINT));

	ShowCursor(FALSE);
	m_bCollision = false;

	return S_OK;
}

HRESULT CUI_Core_Cursor::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_CURSOR"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_Core_Cursor::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_Core_Cursor::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		SetUp_Transform((_float)m_MousePos.x + 20.f, (_float)m_MousePos.y + 20.f, 0.2f, 0.2f);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::CURSOR, this);
	}
	return 0;
}

HRESULT CUI_Core_Cursor::Render()
{
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::CURSOR));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_Cursor::Activate(_double dTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	GetCursorPos(&m_MousePos);
	ScreenToClient(pGameInstance->Get_hWnd(), &m_MousePos);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CURSOR, TEXT("PICK"));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Core_Cursor::Enable(_double dTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	GetCursorPos(&m_MousePos);
	ScreenToClient(pGameInstance->Get_hWnd(), &m_MousePos);

	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CURSOR, TEXT("IDLE"));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI_Core_Cursor::Inactivate(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if ((_uint)LEVEL::LOGO != pGameInstance->Get_LevelID())
	{
		D3D11_VIEWPORT		tViewportDesc;
		ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));
		_uint iNumViews = 1;
		m_pContextDevice->RSGetViewports(&iNumViews, &tViewportDesc);
		POINT pt = { (long)(tViewportDesc.Width / 2.f) , (long)(tViewportDesc.Height / 2.f) };

		ClientToScreen(g_hWnd, &pt);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CURSOR, TEXT("IDLE"));

		SetCursorPos(pt.x, pt.y);
	}
	return S_OK;
}

HRESULT CUI_Core_Cursor::Disable(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if ((_uint)LEVEL::LOGO != pGameInstance->Get_LevelID())
	{
		D3D11_VIEWPORT		tViewportDesc;
		ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));
		_uint iNumViews = 1;
		m_pContextDevice->RSGetViewports(&iNumViews, &tViewportDesc);
		POINT pt = { (long)(tViewportDesc.Width / 2.f) , (long)(tViewportDesc.Height / 2.f) };

		ClientToScreen(g_hWnd, &pt);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::CURSOR, TEXT("IDLE"));

		SetCursorPos(pt.x, pt.y);
	}
	return S_OK;
}

HRESULT CUI_Core_Cursor::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_Cursor::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_Cursor::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*		pPacket = (PACKET*)pMessage;

		UISTATE		pState = *(UISTATE*)pPacket->pData;

		if (UISTATE::DISABLE == m_eUIState)
		{
			switch (pState)
			{
			case Client::CUI::UISTATE::ACTIVATE:
				m_eUIState = CUI::UISTATE::ACTIVATE;
				break;
			case Client::CUI::UISTATE::ENABLE:
				m_eUIState = CUI::UISTATE::ENABLE;
				break;
			case Client::CUI::UISTATE::INACTIVATE:
				break;
			case Client::CUI::UISTATE::DISABLE:
				break;
			case Client::CUI::UISTATE::NONE:
				break;
			default:
				break;
			}
		}
		else if (UISTATE::ENABLE == m_eUIState || UISTATE::ACTIVATE == m_eUIState)
		{
			m_eUIState = pState;
		}
	}
}

_int CUI_Core_Cursor::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_UI_CORE_CURSOR)
		{
			return FALSE;
		}
	}
	return TRUE;
}
