#include "stdafx.h"
#include "..\Public\ZhuangziDoor.h"
#include "Level_Loading.h"
#include "UI_HUD_DirectionIndicator.h"

CZhuangziDoor::CZhuangziDoor(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CZhuangziDoor::CZhuangziDoor(const CZhuangziDoor & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CZhuangziDoor::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CZhuangziDoor::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	m_pModel->Set_CurrentAnimation(0);

	m_pModel->Set_Animation(0, false);
	m_pModel->Update_CombinedTransformationMatrix(0.2);

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	static _uint iInstanceID = OriginID_ZhuangziEnviron;

	m_iObjID = iInstanceID++;

	return S_OK;
}

_int CZhuangziDoor::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bPause || false == m_bCellOn || m_bStop)
		dTimeDelta = 0.0;

	Check_StopTime();

	if (FAILED(Check_PlayerCell()))
		return -1;
	Check_Key();
	m_pModel->Set_Animation(0, false);
	m_pModel->Synchronize_Animation(m_pTransform);

	return (_int)m_eState;
}

_int CZhuangziDoor::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause || false == m_bCellOn || m_bStop)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);
	
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	return (_int)m_eState;
}

HRESULT CZhuangziDoor::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, m_iPassIndex);
	}

	return S_OK;
}

void CZhuangziDoor::Check_StopTime()
{
	if (false == m_bStop)
	{
		if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			m_bStop = true;
	}
}

HRESULT CZhuangziDoor::Check_PlayerCell()
{
	if (!m_bExit)
		return S_OK;

	if (m_bCellOn)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CNavigation* pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_NAVIGATION, 0);

	if (nullptr == pNavigation)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_uint iCurSel = pNavigation->Get_Index();

	if (ZHUANGZI_SCENE_EXIT_ONCELL == iCurSel)
		m_bCellOn = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CZhuangziDoor::Check_Key()
{
	if (m_bCellOn)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (pGameInstance->Key_Down(DIK_R))
		{
			CZhuangziDoor* pDoor = (CZhuangziDoor*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"ZhuangziDoor", 1);

			CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

			tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
			tDesc.eObjState = OBJSTATE::DISABLE;
			tDesc.iObjID = pDoor->Get_ObjID();

			XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pDoor->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

			pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

			CLevel_Loading* pInstance = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::ROBOTGENERAL, (_uint)LEVEL::LOADING);
			if (pInstance == nullptr)
				return;

			pGameInstance->Get_CurrentLevel()->Set_NextLevel(pInstance);
		}
	}
}

_bool CZhuangziDoor::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	return _bool();
}

void CZhuangziDoor::Collision(CGameObject * pGameObject)
{
}

HRESULT CZhuangziDoor::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For. Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZIDOOR, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CZhuangziDoor * CZhuangziDoor::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CZhuangziDoor* pGameInstance = new CZhuangziDoor(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziDoor");
	}

	return pGameInstance;
}

CGameObject * CZhuangziDoor::Clone(void * pArg)
{
	CZhuangziDoor* pGameInstance = new CZhuangziDoor(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziDoor");
	}

	return pGameInstance;
}

void CZhuangziDoor::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
}
