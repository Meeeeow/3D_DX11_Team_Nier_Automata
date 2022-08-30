#include "..\Public\Component.h"


CComponent::CComponent(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice), m_bCloned(false)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

CComponent::CComponent(const CComponent & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice), m_pContextDevice(rhs.m_pContextDevice), m_bCloned(true)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CComponent::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CComponent::NativeConstruct(void * pArg)
{
	return S_OK;
}

void CComponent::Free()
{
	unsigned long dwRefCnt = 0;

	dwRefCnt = Safe_Release(m_pContextDevice);
	dwRefCnt = Safe_Release(m_pGraphicDevice);
}
