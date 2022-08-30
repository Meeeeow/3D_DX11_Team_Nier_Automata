#include "stdafx.h"
#include "..\Public\UI_HUD_Hpbar_MiniGame.h"

CUI_HUD_Hpbar_MiniGame::CUI_HUD_Hpbar_MiniGame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_Hpbar_MiniGame::CUI_HUD_Hpbar_MiniGame(const CUI_HUD_Hpbar_MiniGame & rhs)
	: CUI(rhs)
{
}

CUI_HUD_Hpbar_MiniGame * CUI_HUD_Hpbar_MiniGame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_Hpbar_MiniGame* pInstance = new CUI_HUD_Hpbar_MiniGame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_Hpbar_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_Hpbar_MiniGame::Clone(void * pArg)
{
	CUI_HUD_Hpbar_MiniGame* pInstance = new CUI_HUD_Hpbar_MiniGame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_Hpbar_MiniGame");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_Hpbar_MiniGame::Free()
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

HRESULT CUI_HUD_Hpbar_MiniGame::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_Hpbar_MiniGame::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(-50.f, -60.f, -100.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::DISABLE;

	//CUI_HUD_Hpbar_MiniGame::HPBARDESC*	tDesc = new CUI_HUD_Hpbar_MiniGame::HPBARDESC;

	//tDesc->eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::ENEMY;
	//tDesc->fCrntHPUV = 1.f;
	//tDesc->fPrevHPUV = 1.f;
	//tDesc->iObjID = 3;

	//m_listRenderHPBar.push_back(tDesc);

	//CUI_HUD_Hpbar_MiniGame::HPBARDESC*	tDesc2 = new CUI_HUD_Hpbar_MiniGame::HPBARDESC;

	//tDesc2->eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::PLAYER;
	//tDesc2->fCrntHPUV = 1.f;
	//tDesc2->fPrevHPUV = 1.f;
	//tDesc2->iObjID = 4;

	//m_listRenderHPBar.push_back(tDesc2);


	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::SetUp_Components()
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

	pGameInstance->Create_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"), this);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_Hpbar_MiniGame::Tick(_double TimeDelta)
{
	auto& it = m_listRenderHPBar.begin();
	auto& ite = m_listRenderHPBar.end();

	while (it != ite)
	{
		HPBARDESC*	pHPBarDesc = *it;

		// UV 연출용 패러미터 설정
		if (pHPBarDesc->fCrntHPUV < pHPBarDesc->fPrevHPUV)
		{
			pHPBarDesc->fPrevHPUV -= _float(TimeDelta * 0.1f);
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
			++it;

			//Safe_Delete(*it);
			//it = m_listRenderHPBar.erase(it);
		}
		else
		{
			++it;
		}
	}
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_Hpbar_MiniGame::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Render()
{
	SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::IRON2, TEXT("IRON2_BG"));
	SetUp_Transform(640.f, 150.f, 0.5f, 0.5f);
	SetUp_DefaultRawValue();
	_bool bGrayScale = m_pRendererCom->Get_GrayDistortion();
	m_pModelCom->SetUp_RawValue("g_bGrayScale", &bGrayScale, sizeof(_bool));
	m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
	m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::IRON2));
	m_pModelCom->Render(1);

	for (auto& pDesc : m_listRenderHPBar)
	{
		_float	fOffsetX = 0.f;
		_float	fAddOffSetX = 0.f;
		_uint	iPass = 6;
		const _tchar*	szElemKey = TEXT("IRON2_9S");

		switch (pDesc->eTarget)
		{
		case TARGET::PLAYER:
			fAddOffSetX = 240.f;
			fOffsetX = 640.f + 250.f;
			break;
		case TARGET::ENEMY:
			fAddOffSetX = -240.f;
			fOffsetX = 640.f - 250.f;
			iPass = 17;
			szElemKey = TEXT("IRON2_HS");
			break;
		default:
			break;
		}

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::HUD, TEXT("HPBAR_CONTENTS"));
		SetUp_Transform(fOffsetX, 150.f, 1.2f, 2.5f);
		SetUp_DefaultRawValue();
		bGrayScale = m_pRendererCom->Get_GrayDistortion();
		m_pModelCom->SetUp_RawValue("g_bGrayScale", &bGrayScale, sizeof(_bool));
		m_pModelCom->SetUp_RawValue("g_fCrntHPUV", &pDesc->fCrntHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_fPrevHPUV", &pDesc->fPrevHPUV, sizeof(_float));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[0], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::HUD));
		m_pModelCom->Render(iPass);

		SetUp_AtlasUV((_uint)CAtlas_Manager::CATEGORY::IRON2, szElemKey);
		SetUp_Transform(fOffsetX + fAddOffSetX, 150.f, 0.5f, 0.5f);
		SetUp_DefaultRawValue();
		bGrayScale = m_pRendererCom->Get_GrayDistortion();
		m_pModelCom->SetUp_RawValue("g_bGrayScale", &bGrayScale, sizeof(_bool));
		m_pModelCom->SetUp_RawValue("g_vecColorOverlay", &m_vecColorOverlays[1], sizeof(_float4));
		m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::IRON2));
		m_pModelCom->Render(1);
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Enable(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Disable(_double dTimeDelta)
{
	if (false == m_listRenderHPBar.empty())
	{
		for (auto& pDesc : m_listRenderHPBar)
		{
			Safe_Delete(pDesc);
		}
		m_listRenderHPBar.clear();
	}
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_Hpbar_MiniGame::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_Hpbar_MiniGame::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult && m_eUIState == CUI::UISTATE::ENABLE)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		HPBARDESC*	pCopiedDesc = new HPBARDESC;

		memcpy(pCopiedDesc, (HPBARDESC*)pPacket->pData, sizeof(HPBARDESC));

		list<HPBARDESC*>::iterator	it = m_listRenderHPBar.begin();
		list<HPBARDESC*>::iterator	ite = m_listRenderHPBar.end();

		HPBARDESC*	pMatchedDesc = nullptr;

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
			pMatchedDesc->eTarget = pCopiedDesc->eTarget;
			Safe_Delete(pCopiedDesc);
		}
		else
		{
			m_listRenderHPBar.emplace_back(pCopiedDesc);
		}
	}
	else if (2 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	eState = *(UISTATE*)pPacket->pData;

		m_eUIState = eState;
	}
}

_int CUI_HUD_Hpbar_MiniGame::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_MINIGAME_HPBAR)
		{
			return 1;
		}
		else if (*check == CHECKSUM_UI_HUD_MINIGAME_HPBAR_STATE)
		{
			return 2;
		}
	}
	return FALSE;
}
