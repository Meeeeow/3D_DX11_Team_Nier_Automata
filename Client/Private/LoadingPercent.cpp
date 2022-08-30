#include "stdafx.h"
#include "..\Public\LoadingPercent.h"

CLoadingPercent::CLoadingPercent(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{
}

CLoadingPercent::CLoadingPercent(const CLoadingPercent & rhs)
	: CProjectionBase(rhs)
{
}

HRESULT CLoadingPercent::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLoadingPercent::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_PERCENT, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 128.f;
	fSizeY = 128.f;
	fX = 544.f;
	fY = 264.f;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX, -fY, 0.f, 1.f));
	m_iDistanceZ = 1;
	m_iPassIndex = 16;
	m_fAlpha = 0.0f;

	return S_OK;
}

_int CLoadingPercent::Tick(_double dTimeDelta)
{
	if (m_iIndex == 6)
	{
		m_fAlpha += (_float)dTimeDelta * m_iMulti;

		if (m_fAlpha <= 0.0f || m_fAlpha >= 1.f)
		{
			m_iMulti *= -1;
		}
	}

	return _int();
}

_int CLoadingPercent::LateTick(_double dTimedelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLoadingPercent::Render()
{
	bool bAlpha = true;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));
	m_pVIBuffer->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof(float));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV(m_iIndex));
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

void CLoadingPercent::Increase_Percent()
{
	if (m_iIndex >= 6)
		return;

	++m_iIndex;
}

CLoadingPercent * CLoadingPercent::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLoadingPercent* pInstance = new CLoadingPercent(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLoadingPercent * CLoadingPercent::Clone(void * pArg)
{
	CLoadingPercent* pInstance = new CLoadingPercent(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoadingPercent::Free()
{
	Safe_Release(m_pTexture);
	__super::Free();
}
