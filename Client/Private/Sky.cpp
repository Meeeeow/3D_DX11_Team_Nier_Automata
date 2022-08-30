#include "stdafx.h"
#include "..\Public\Sky.h"

CSky::CSky(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CSky::CSky(const CSky & rhs)
	: CGameObjectModel(rhs)
{

}

CSky * CSky::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CSky* pInstance = new CSky(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CSky::Clone(void * pArg)
{
	CSky* pInstance = new CSky(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CSky::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
}


HRESULT CSky::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSky::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// Pass_Anim
	m_iPassIndex = (_uint)MESH_PASS_INDEX::SKY;
	
	return S_OK;
}

HRESULT CSky::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	LEVEL eNextLevel = (LEVEL)pGameInstance->Get_NextLevelID();
	switch (eNextLevel)
	{
	case Client::LEVEL::STAGE1:
		break;
	case Client::LEVEL::ROBOTMILITARY:
		break;
	case Client::LEVEL::ZHUANGZISTAGE:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY_MORNING, TEXT("Com_Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
		break;
	case Client::LEVEL::ROBOTGENERAL:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY_AFTERNOON, TEXT("Com_Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
		break;
	case Client::LEVEL::AMUSEMENTPARK:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY, TEXT("Com_Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
		break;
	case Client::LEVEL::OPERABACKSTAGE:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SKY, TEXT("Com_Model"), (CComponent**)&m_pModel)))
			return E_FAIL;
		break;
	default:
		break;
	}

	return S_OK;
}

_int CSky::Tick(_double dTimeDelta)
{
	return (_int)m_eState;
}

_int CSky::LateTick(_double dTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	m_pTransform->Set_State(CTransform::STATE::POSITION, pGameInstance->Get_CameraPosition());

	m_pTransform->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::PRIORITY, this);

	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

HRESULT CSky::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CSky::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		/* Empty */
	}
}

_int CSky::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		//if (*check == CHECKSUM_SKY)
		{
			return TRUE;
		}
	}
	return FALSE;
}
