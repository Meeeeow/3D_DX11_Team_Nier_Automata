#include "stdafx.h"
#include "..\Public\LogoAnyKey.h"

CLogoAnyKey::CLogoAnyKey(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{
}

CLogoAnyKey::CLogoAnyKey(const CLogoAnyKey & rhs)
	: CProjectionBase(rhs)
{
}

HRESULT CLogoAnyKey::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLogoAnyKey::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::LOGO, PROTO_KEY_LOGO_ANYKEY, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 461;
	fSizeY = 67;
	fX = 0;
	fY = 200;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX, -fY, 0.f, 1.f));
	m_iDistanceZ = 1;
	m_iPassIndex = 16;
	m_iMulti = -1;
	m_fAlpha = 0.0f;

	return S_OK;
}

_int CLogoAnyKey::Tick(_double dTimeDelta)
{

	m_fAlpha += (_float)dTimeDelta * m_iMulti;

	if (m_fAlpha <= 0.0f || m_fAlpha >= 1.f)
	{
		m_iMulti *= -1;
	}

	return _int();
}

_int CLogoAnyKey::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLogoAnyKey::Render()
{
	bool bAlpha = true;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));
	m_pVIBuffer->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof(float));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

CLogoAnyKey * CLogoAnyKey::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLogoAnyKey* pInstance = new CLogoAnyKey(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLogoAnyKey * CLogoAnyKey::Clone(void * pArg)
{
	CLogoAnyKey* pInstance = new CLogoAnyKey(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLogoAnyKey::Free()
{
	Safe_Release(m_pTexture);

	__super::Free();
}
