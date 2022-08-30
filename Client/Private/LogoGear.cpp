#include "stdafx.h"
#include "..\Public\LogoGear.h"

CLogoGear::CLogoGear(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{

}

CLogoGear::CLogoGear(const CLogoGear & rhs)
	: CProjectionBase(rhs)
{

}

HRESULT CLogoGear::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLogoGear::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_GEAR, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 20;
	fSizeY = 20;
	fX = -216.5;
	fY = 77.5;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX , fY , 0.f, 1.f));
	m_pTransform->Set_RotationSpeed(2.5);
	m_iDistanceZ = 2;
	m_iPassIndex = 16;

	return S_OK;
}

_int CLogoGear::Tick(_double dTimeDelta)
{
	m_pTransform->Rotation_Axis(dTimeDelta, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	return _int();
}

_int CLogoGear::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLogoGear::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLogoGear * CLogoGear::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLogoGear* pInstance = new CLogoGear(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLogoGear * CLogoGear::Clone(void * pArg)
{
	CLogoGear* pInstance = new CLogoGear(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogoGear::Free()
{
	Safe_Release(m_pTexture);
	__super::Free();
}
