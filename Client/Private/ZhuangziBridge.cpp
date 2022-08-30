#include "stdafx.h"
#include "..\Public\ZhuangziBridge.h"
#include "SoundMgr.h"

CZhuangziBridge::CZhuangziBridge(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CZhuangziBridge::CZhuangziBridge(const CZhuangziBridge & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CZhuangziBridge::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CZhuangziBridge::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	m_pModel->Update_CombinedTransformationMatrix(0.1);

	m_iObjID = OriginID_ZhuangziEnviron + 5;

	return S_OK;
}

_int CZhuangziBridge::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;
	
	if (m_eState == OBJSTATE::DISABLE)
		return (_int)OBJSTATE::ENABLE;

	if (m_bPause)
		dTimeDelta = 0.0;

	Check_StopTime();

	if (FAILED(Check_PlayerCell()))
		return -1;

	m_pModel->Set_Animation(0, false);
	m_pModel->Synchronize_Animation(m_pTransform);


	return (_int)m_eState;
}

_int CZhuangziBridge::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bExit)
	{
		if (m_bPause || (m_bClose && false == m_bCellOn))
			dTimeDelta = 0.0;
	}
	else
	{
		if (m_bPause || (m_bClose && false == m_bCellOn))
			dTimeDelta = 0.0;
	}

	if (m_eState != OBJSTATE::DISABLE)
		m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	return (_int)m_eState;
}

HRESULT CZhuangziBridge::Render()
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

void CZhuangziBridge::Check_StopTime()
{
	if (!m_bSceneClear)
	{
		if (nullptr == m_pModel || m_bCellOn)
			return;

		if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			m_bClose = true;
	}
	else
	{
		if (nullptr == m_pModel)
			return;

		if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		{
			m_bClose = true;
			m_bCellOn = false;
		}
	}


}

HRESULT CZhuangziBridge::Check_PlayerCell()
{
	if (m_bCellOn || m_bExit)
		return S_OK;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CNavigation* pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_NAVIGATION, 0);
	
	if (nullptr == pNavigation)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	_uint iCurSel = pNavigation->Get_Index();

	if (ZHUANGZI_SCENE_ONCELL == iCurSel)
		m_bCellOn = true;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CZhuangziBridge::Initialize_Animation()
{
	if (nullptr == m_pModel)
		return;

	m_pModel->Initialize_RootMotion();
	m_pModel->Initialize_Time();
}

_bool CZhuangziBridge::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	return _bool();
}

void CZhuangziBridge::Collision(CGameObject * pGameObject)
{
}

HRESULT CZhuangziBridge::SetUp_Components()
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
	if (FAILED(__super::Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZIBRIDGE, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CZhuangziBridge * CZhuangziBridge::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CZhuangziBridge* pGameInstance = new CZhuangziBridge(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziBridge");
	}

	return pGameInstance;
}

CGameObject * CZhuangziBridge::Clone(void * pArg)
{
	CZhuangziBridge* pGameInstance = new CZhuangziBridge(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziBridge");
	}

	return pGameInstance;
}

void CZhuangziBridge::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
}
