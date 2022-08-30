#include "..\Public\VIBuffer_Cone.h"

CVIBuffer_Cone::CVIBuffer_Cone(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{
}

CVIBuffer_Cone::CVIBuffer_Cone(const CVIBuffer_Cone & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cone::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName, _uint iResoultion, _float fRadius, _float fHeight)
{


	return E_NOTIMPL;
}

HRESULT CVIBuffer_Cone::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

CVIBuffer_Cone * CVIBuffer_Cone::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	return nullptr;
}

CVIBuffer_Cone * CVIBuffer_Cone::Clone(void * pArg)
{
	return nullptr;
}

void CVIBuffer_Cone::Free()
{
}
