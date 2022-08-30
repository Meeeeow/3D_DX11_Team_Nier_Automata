#include "stdafx.h"
#include "..\Public\LogoScreen.h"

CLogoScreen::CLogoScreen(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{

}

CLogoScreen::CLogoScreen(const CLogoScreen & rhs)
	: CProjectionBase(rhs)
{

}

HRESULT CLogoScreen::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLogoScreen::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_SCREEN, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = g_iWinCX;
	fSizeY = g_iWinCY;
	fX = g_iWinCX >> 1;
	fY = g_iWinCY >> 1;
	
	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX - (g_iWinCX >> 1), -fY + (g_iWinCY >> 1), 0.f, 1.f));
	m_iDistanceZ = 0;
	m_iPassIndex = 16;
	
	return S_OK;
}

_int CLogoScreen::Tick(_double dTimeDelta)
{
	return 0;
}

_int CLogoScreen::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return 0;
}

HRESULT CLogoScreen::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLogoScreen * CLogoScreen::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLogoScreen* pInstance = new CLogoScreen(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLogoScreen * CLogoScreen::Clone(void * pArg)
{
	CLogoScreen* pInstance = new CLogoScreen(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogoScreen::Free()
{
	Safe_Release(m_pTexture);

	__super::Free();
}
