#include "stdafx.h"
#include "..\Public\BeauvoirCurtain.h"

CBeauvoirCurtain::CBeauvoirCurtain(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoirCurtain::CBeauvoirCurtain(const CBeauvoirCurtain & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoirCurtain::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoirCurtain::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;


	return S_OK;
}

_int CBeauvoirCurtain::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bPause)
		dTimeDelta = 0.0;
		
	m_pModel->Set_Animation((_uint)m_ePattern, false);
	m_pModel->Synchronize_Animation(m_pTransform);
	return (_int)m_eState;
}

_int CBeauvoirCurtain::LateTick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;
		
	if (m_bPause)
		dTimeDelta = 0.0;


	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	return _int();
}

HRESULT CBeauvoirCurtain::Render()
{
	return __super::Render();
}

void CBeauvoirCurtain::Set_Animation(PATTERN eAnimation)
{
	m_ePattern = eAnimation;
	m_pModel->Set_NoneBlend();
}

HRESULT CBeauvoirCurtain::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For. Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::OPERABACKSTAGE, L"Curtain", COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

CBeauvoirCurtain * CBeauvoirCurtain::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoirCurtain* pInstance = new CBeauvoirCurtain(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBeauvoirCurtain::Clone(void * pArg)
{
	CBeauvoirCurtain* pInstance = new CBeauvoirCurtain(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBeauvoirCurtain::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
}
