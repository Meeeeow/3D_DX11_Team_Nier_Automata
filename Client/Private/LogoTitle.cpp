#include "stdafx.h"
#include "..\Public\LogoTitle.h"

CLogoTitle::CLogoTitle(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{

}

CLogoTitle::CLogoTitle(const CLogoTitle & rhs)
	: CProjectionBase(rhs)
{

}

HRESULT CLogoTitle::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLogoTitle::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_TITLE, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 640.f;
	fSizeY = 160.f;
	fX = 0.f;
	fY = 60.f;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX, fY, 0.f, 1.f));
	m_iDistanceZ = 1;
	m_iPassIndex = 16;

	return S_OK;
}

_int CLogoTitle::Tick(_double dTimeDelta)
{
	return _int();
}

_int CLogoTitle::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLogoTitle::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLogoTitle * CLogoTitle::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLogoTitle* pInstance = new CLogoTitle(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLogoTitle * CLogoTitle::Clone(void * pArg)
{
	CLogoTitle* pInstance = new CLogoTitle(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogoTitle::Free()
{
	Safe_Release(m_pTexture);

	__super::Free();
}
