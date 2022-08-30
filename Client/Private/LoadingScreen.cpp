#include "stdafx.h"
#include "..\Public\LoadingScreen.h"

CLoadingScreen::CLoadingScreen(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{
}

CLoadingScreen::CLoadingScreen(const CLoadingScreen & rhs)
	: CProjectionBase(rhs)
{
}

HRESULT CLoadingScreen::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLoadingScreen::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_SCREEN, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
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

_int CLoadingScreen::Tick(_double dTimeDelta)
{
	return _int();
}

_int CLoadingScreen::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLoadingScreen::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLoadingScreen * CLoadingScreen::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLoadingScreen* pInstance = new CLoadingScreen(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLoadingScreen * CLoadingScreen::Clone(void * pArg)
{
	CLoadingScreen* pInstance = new CLoadingScreen(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoadingScreen::Free()
{
	Safe_Release(m_pTexture);
	__super::Free();
}
