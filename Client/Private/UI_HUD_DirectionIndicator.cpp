#include "stdafx.h"
#include "..\Public\UI_HUD_DirectionIndicator.h"
#include "Player.h"

const _double	CUI_HUD_DirectionIndicator::DURATION_ENEMY = 0.3;
const _double	CUI_HUD_DirectionIndicator::DURATION_OBJECTIVE = 600.0;

CUI_HUD_DirectionIndicator::CUI_HUD_DirectionIndicator(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CUI(pDevice, pDeviceContext)
{
}

CUI_HUD_DirectionIndicator::CUI_HUD_DirectionIndicator(const CUI_HUD_DirectionIndicator & rhs)
	: CUI(rhs)
{
}

CUI_HUD_DirectionIndicator * CUI_HUD_DirectionIndicator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_HUD_DirectionIndicator* pInstance = new CUI_HUD_DirectionIndicator(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CUI_HUD_DirectionIndicator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_HUD_DirectionIndicator::Clone(void * pArg)
{
	CUI_HUD_DirectionIndicator* pInstance = new CUI_HUD_DirectionIndicator(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CUI_HUD_DirectionIndicator");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_HUD_DirectionIndicator::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pPlayer);

	for (auto& pDesc : m_listIndicatorDesc)
	{
		Safe_Delete(pDesc);
	}
	m_listIndicatorDesc.clear();
}

HRESULT CUI_HUD_DirectionIndicator::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::NativeConstruct(void * pArg)
{
	if (FAILED(CUI_HUD_DirectionIndicator::SetUp_Components()))
	{
		return E_FAIL;
	}
	_float4	vColorOverlay;

	XMStoreFloat4(&vColorOverlay, XMVectorSet(0.f, 0.f, 0.f, 0.f));

	m_vecColorOverlays.push_back(vColorOverlay);

	m_iDistanceZ = 0;

	m_eUIState = CUI::UISTATE::ENABLE;

	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::SetUp_Components()
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

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_int CUI_HUD_DirectionIndicator::Tick(_double TimeDelta)
{
	return CUI::Tick(TimeDelta);
}

_int CUI_HUD_DirectionIndicator::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI_HUD_DirectionIndicator::Render()
{
	_float			fOffsetY = 0.1f;
	CTransform*		pPlayerTransform = nullptr;

	if (nullptr != m_pPlayer)
	{
		pPlayerTransform = (CTransform*)m_pPlayer->Get_Component(COM_KEY_TRANSFORM);
	}

	for (auto& pDesc : m_listIndicatorDesc)
	{
		if (0.0 == pDesc->dTimeDuration || OBJSTATE::DISABLE == pDesc->eObjState || OBJSTATE::DEAD == pDesc->eObjState)
		{
			continue;
		}

		_vector			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
		_float4			vPlayerPosConv;

		XMStoreFloat4(&vPlayerPosConv, vPlayerPos);

		vPlayerPosConv.y += fOffsetY;
		fOffsetY += 0.01f;

		XMLoadFloat4(&vPlayerPosConv);

		_matrix			matPlayerOnlyPos = XMMatrixIdentity();
		_float4x4		matPlayerOnlyPos2;

		XMStoreFloat4x4(&matPlayerOnlyPos2, matPlayerOnlyPos);

		memcpy((_float4*)&matPlayerOnlyPos2.m[3][0], &vPlayerPosConv, sizeof(_float4));

		pDesc->vTargetWorldPos.y = vPlayerPosConv.y;

		_vector		vTargetDir = XMVector3Normalize(XMLoadFloat4(&pDesc->vTargetWorldPos) - XMLoadFloat4(&vPlayerPosConv));
	
		_float		fResultRadian = acosf(XMVectorGetX(XMVector3Dot(vTargetDir, XMVectorSet(0.f, 0.f, 1.f, 0.f))));
		_float		fResultCross = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Cross(vTargetDir, XMVectorSet(0.f, 0.f, 1.f, 0.f))));

		if (0 < fResultCross)
		{
			fResultRadian *= -1.f;
		}

		_matrix		matScale = XMMatrixScaling(0.5f, 1.0f, 1.f);
		_matrix		matRotationX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
		_matrix		matRotationY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fResultRadian);		
		_matrix		matTranslate = XMLoadFloat4x4(&matPlayerOnlyPos2) * XMMatrixTranslationFromVector(vTargetDir);
		
		SetUp_Transform_ForPP(matScale * matRotationX * matRotationY * matTranslate);
		SetUp_DefaultRawvalue_ForPP();

		if (CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE == pDesc->eIndicatorType)
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::INDICATOR_B));
		else if (CUI_HUD_DirectionIndicator::INDICATORTYPE::ENEMY == pDesc->eIndicatorType)
			m_pModelCom->SetUp_Texture("g_texDiffuse", m_pTextureCom->Get_SRV((_uint)CAtlas_Manager::CATEGORY::INDICATOR_R));

		m_pModelCom->Render(0);
	}

	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Activate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Enable(_double dTimeDelta)
{
	auto& it = m_listIndicatorDesc.begin();
	auto& ite = m_listIndicatorDesc.end();

	while (it != ite)
	{
		UIINDICATORDESC*	pDesc = *it;

		if (OBJSTATE::DISABLE == pDesc->eObjState || OBJSTATE::DEAD == pDesc->eObjState)
		{
			Safe_Delete(*it);
			it = m_listIndicatorDesc.erase(it);
		}
		else
		{
			pDesc->dTimeDuration -= dTimeDelta;

			if (pDesc->dTimeDuration < 0.0)
			{
				pDesc->dTimeDuration = 0.0;
			}
			++it;
		}
	}
	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Inactivate(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Disable(_double dTimeDelta)
{
	Release_UI();

	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Pressed(_double dTimeDelta)
{
	return S_OK;
}

HRESULT CUI_HUD_DirectionIndicator::Released(_double dTimeDelta)
{
	return S_OK;
}

void CUI_HUD_DirectionIndicator::Release_UI()
{
	for (auto& pDesc : m_listIndicatorDesc)
	{
		Safe_Delete(pDesc);
	}
	m_listIndicatorDesc.clear();
}

//_matrix CUI_HUD_DirectionIndicator::Calculate_Direction(const _float& fPosOffsetY)
//{
//	CTransform*		pPlayerTransform = (CTransform*)m_pPlayer->Get_Component(COM_KEY_TRANSFORM);
//	_vector			vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
//	_float4			vPlayerPosConv;
//
//	XMStoreFloat4(&vPlayerPosConv, vPlayerPos);
//
//	vPlayerPosConv.y += fPosOffsetY;
//	//fOffsetY += 0.01f;
//
//	XMLoadFloat4(&vPlayerPosConv);
//
//	_matrix			matPlayerOnlyPos = XMMatrixIdentity();
//	_float4x4		matPlayerOnlyPos2;
//
//	XMStoreFloat4x4(&matPlayerOnlyPos2, matPlayerOnlyPos);
//
//	memcpy((_float4*)&matPlayerOnlyPos2.m[3][0], &vPlayerPosConv, sizeof(_float4));
//
//	_vector		vTargetDir = XMVector3Normalize(XMLoadFloat4(&pDesc->vTargetWorldPos) - XMLoadFloat4(&vPlayerPosConv));
//
//	_float		fResultRadian = acosf(XMVectorGetX(XMVector3Dot(vTargetDir, XMVectorSet(0.f, 0.f, 1.f, 0.f))));
//	_float		fResultCross = XMVectorGetX(XMVector3Dot(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVector3Cross(vTargetDir, XMVectorSet(0.f, 0.f, 1.f, 0.f))));
//
//	if (0 < fResultCross)
//	{
//		fResultRadian *= -1.f;
//	}
//
//	_matrix		matScale = XMMatrixScaling(0.5f, 1.0f, 1.f);
//	_matrix		matRotationX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
//	_matrix		matRotationY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fResultRadian);
//	_matrix		matTranslate = XMLoadFloat4x4(&matPlayerOnlyPos2) * XMMatrixTranslationFromVector(vTargetDir);
//
//	return matScale * matRotationX * matRotationY * matTranslate;
//}

void CUI_HUD_DirectionIndicator::SetUp_Player()
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

void CUI_HUD_DirectionIndicator::Notify(void * pMessage)
{
	_uint iResult = VerifyChecksum(pMessage);

	if (1 == iResult)
	{
		if (CUI::UISTATE::DISABLE == m_eUIState)
		{
			return;
		}
		if (nullptr == m_pPlayer)
		{
			SetUp_Player();
		}

		PACKET*	pPacket = (PACKET*)pMessage;

		UIINDICATORDESC*	pCopiedDesc = new UIINDICATORDESC;

		memcpy(pCopiedDesc, (UIINDICATORDESC*)pPacket->pData, sizeof(UIINDICATORDESC));

		list<UIINDICATORDESC*>::iterator	it = m_listIndicatorDesc.begin();
		list<UIINDICATORDESC*>::iterator	ite = m_listIndicatorDesc.end();

		UIINDICATORDESC*	pMatchedDesc = nullptr;

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
			pMatchedDesc->eObjState = pCopiedDesc->eObjState;
			pMatchedDesc->vTargetWorldPos = pCopiedDesc->vTargetWorldPos;
			pMatchedDesc->dTimeDuration = pCopiedDesc->dTimeDuration;

			Safe_Delete(pCopiedDesc);
		}
		else
		{
			m_listIndicatorDesc.emplace_back(pCopiedDesc);
		}
	}
	else if (2 == iResult)
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		UISTATE	eState = *(UISTATE*)pPacket->pData;

		m_eUIState = eState;
	}
	else if (3 == iResult)
	{
		Release_UI();
	}
}

_int CUI_HUD_DirectionIndicator::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_UI_HUD_DIRECTION_INDICATOR)
		{
			return 1;
		}
		if (*check == CHECKSUM_ONLY_UISTATE)
		{
			return 2;
		}
		if (*check == CHECKSUM_RELEASE_UI)
		{
			return 3;
		}			
	}
	return FALSE;
}
