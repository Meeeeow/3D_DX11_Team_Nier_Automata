#include "stdafx.h"
#include "..\Public\UI_Core_SelectList.h"
#include "UI_Core_SelectLine.h"
//#include "UI_Core_ScrollList.h"

const	_tchar*		CUI_Core_SelectList::LINEOBSERVERKEY[] = {
	TEXT("OBSERVER_SELECTLINE_0"),
	TEXT("OBSERVER_SELECTLINE_1"),
	TEXT("OBSERVER_SELECTLINE_2")
};

const	_uint		CUI_Core_SelectList::LINEMAXCOUNT = sizeof(CUI_Core_SelectList::LINEOBSERVERKEY) / sizeof(_tchar*);
const	_float		CUI_Core_SelectList::LINEOFFSETY = 42.f;

CUI_Core_SelectList::CUI_Core_SelectList(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_Core_SelectList::CUI_Core_SelectList(const CUI_Core_SelectList & rhs)
	: CUI(rhs)
{
}

CUI_Core_SelectList * CUI_Core_SelectList::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Core_SelectList* pInstance = new CUI_Core_SelectList(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_Core_SelectList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Core_SelectList::Clone(void * pArg)
{
	CUI_Core_SelectList* pInstance = new CUI_Core_SelectList(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_Core_SelectList");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Core_SelectList::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (auto& pObserver : m_dqLineObservers)
	{
		Safe_Release(pObserver);
	}
	m_dqLineObservers.clear();
}

HRESULT CUI_Core_SelectList::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_Core_SelectList::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_Core_SelectList::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;
	
	// Background Default : 224, 223, 211, 128

	//XMStoreFloat4(&vColorOverlay, XMVectorSet(-145.f, -145.f, -145.f, 50.f));

	//m_vecColorOverlays.push_back(vColorOverlay);	// Head

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-40.f, -40.f, -40.f, 128.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Body

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-110.f, -110.f, -110.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Inversed

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);	// Font Origin

	m_eUIState = CUI::UISTATE::DISABLE;
	m_iDistanceZ = 4;
	m_bCollision = false;

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_CORE_SELECTLIST"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);


	for (_int i = 0; i < CUI_Core_SelectList::LINEMAXCOUNT; ++i)
	{
		CObserver*		pLineObserver = nullptr;

		pLineObserver = pGameInstance->Get_Observer(CUI_Core_SelectList::LINEOBSERVERKEY[i]);

		if (nullptr == pLineObserver)
		{
			return E_FAIL;
		}
		else
		{
			m_dqLineObservers.push_back(pLineObserver);
			Safe_AddRef(pLineObserver);
		}
	}	
	return S_OK;
}

HRESULT CUI_Core_SelectList::SetUp_Components()
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

_int CUI_Core_SelectList::Tick(_double TimeDelta)
{
	Update_SelectList();

	return CUI::Tick(TimeDelta);
}

_int CUI_Core_SelectList::LateTick(_double TimeDelta)
{
	//m_iCommandFlag = CUI_Core_SelectList::COMMAND_INITIALIZE;

	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_Core_SelectList::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::DECO, TEXT("BACKGROUND"));
	SetUp_Transform(m_fBasePosX, m_fBasePosY, 0.6f, 0.3f);	// 192 * 96
	SetUp_DefaultRawValue();

	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::DECO));
	m_pModelCom->Render(1);

	return S_OK;
}

HRESULT CUI_Core_SelectList::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_SelectList::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_SelectList::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_SelectList::Disable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_SelectList::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_Core_SelectList::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_Core_SelectList::Release_UI()
{
	
}

void CUI_Core_SelectList::Update_SelectList()
{
	// Parse Command
	if (m_iCommandFlag & COMMAND_OPEN)
	{
		m_eNextState = CUI::UISTATE::ENABLE;

		m_fBasePosY = m_tSelectDesc.fLinePosY;

		CUI_Core_SelectLine::ROLE	eRole;
		
		switch (m_tSelectDesc.tItemDesc.iItemCategory)
		{
		case (_uint)ITEMCATEGORY::EXPENDABLE:
		case (_uint)ITEMCATEGORY::RESOURCE:
			eRole = CUI_Core_SelectLine::ROLE::USE;
			break;
		case (_uint)ITEMCATEGORY::SHORTSWORD:
		case (_uint)ITEMCATEGORY::LONGSWORD:
		case (_uint)ITEMCATEGORY::SPEAR:
		case (_uint)ITEMCATEGORY::GAUNTLET:
			eRole = CUI_Core_SelectLine::ROLE::EQUIP_MAIN;
			break;
		case (_uint)ITEMCATEGORY::ENFORCECHIP:
			eRole = CUI_Core_SelectLine::ROLE::INSTALL;
			break;
		default:
			eRole = CUI_Core_SelectLine::ROLE::NONE;
			break;
		}

		_uint iLineCount = 2;
		_uint iOffsetY = 0;

		for (auto& pLineObserver : m_dqLineObservers)
		{
			if (iLineCount == 0)
				break;

			CUI_Core_SelectLine::UISELECTLINEDESC	tDesc;

			tDesc.eRole = CUI_Core_SelectLine::ROLE((_uint)eRole + iOffsetY);
			tDesc.tItemDesc = m_tSelectDesc.tItemDesc;
			tDesc.fPosX = m_fBasePosX;
			tDesc.fPosY = m_fBasePosY - 21.f + (42.f * (_float)iOffsetY);

			pLineObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &m_eNextState));
			pLineObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_UPDATE, &tDesc));

			++iOffsetY;
			--iLineCount;
		}
	}
	else if (m_iCommandFlag & COMMAND_CLOSE)
	{
		m_eNextState = CUI::UISTATE::DISABLE;

		for (auto& pLineObserver : m_dqLineObservers)
		{
			pLineObserver->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &m_eNextState));
		}
	}	
	m_iCommandFlag = CUI_Core_SelectList::COMMAND_INITIALIZE;

	// Update State
	switch (m_eUIState)
	{
	case Client::CUI::UISTATE::ACTIVATE:
		break;
	case Client::CUI::UISTATE::ENABLE:
		if (UISTATE::DISABLE == m_eNextState) { m_eUIState = m_eNextState; }
		break;
	case Client::CUI::UISTATE::INACTIVATE:
		break;
	case Client::CUI::UISTATE::DISABLE:
		if (UISTATE::ENABLE == m_eNextState) { m_eUIState = m_eNextState; }
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
	return;
}

void CUI_Core_SelectList::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISELECTDESC	tDesc = *(UISELECTDESC*)pPacket->pData;

		if (!(m_iCommandFlag & tDesc.iCommandKey))
		{
			m_iCommandFlag |= tDesc.iCommandKey;

			if (tDesc.iCommandKey & COMMAND_OPEN)
			{
				m_tSelectDesc = tDesc;
			}
			if ((m_iCommandFlag & COMMAND_OPEN) && (m_iCommandFlag & COMMAND_CLOSE))
			{
				m_iCommandFlag ^= COMMAND_CLOSE;
			}
		}
	}

	// test
	//m_iCommandFlag = m_iCommandFlag;
}

_int CUI_Core_SelectList::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_CORE_SELECTLIST)
		{
			return 1;
		}
	}
	return FALSE;
}
