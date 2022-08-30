#include "stdafx.h"
#include "..\Public\LogoCopyRight.h"


CLogoCopyRight::CLogoCopyRight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{
}

CLogoCopyRight::CLogoCopyRight(const CLogoCopyRight & rhs)
	: CProjectionBase(rhs)
{
}

HRESULT CLogoCopyRight::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLogoCopyRight::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_COPYRIGHT, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 400;
	fSizeY = 120;
	fX = 0;
	fY = 330;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX, -fY, 0.f, 1.f));
	m_iDistanceZ = 1;
	m_iPassIndex = 16;
	return S_OK;
}

_int CLogoCopyRight::Tick(_double dTimeDelta)
{
	return _int();
}

_int CLogoCopyRight::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);

	return _int();
}

HRESULT CLogoCopyRight::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLogoCopyRight * CLogoCopyRight::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLogoCopyRight* pInstance = new CLogoCopyRight(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLogoCopyRight * CLogoCopyRight::Clone(void * pArg)
{
	CLogoCopyRight* pInstance = new CLogoCopyRight(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogoCopyRight::Free()
{
	Safe_Release(m_pTexture);

	__super::Free();
}
