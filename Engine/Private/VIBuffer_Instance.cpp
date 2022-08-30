#include "..\Public\VIBuffer_Instance.h"

CVIBuffer_Instance::CVIBuffer_Instance(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance & rhs)
	: CVIBuffer(rhs)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pVBInstance(rhs.m_pVBInstance)
	, m_VBInstanceDesc(rhs.m_VBInstanceDesc)
	, m_VBInstanceSubResourceData(rhs.m_VBInstanceSubResourceData)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_pInstanceVertices(rhs.m_pInstanceVertices)
{
	Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Instance::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Instance::Render(_uint iPassIndex)
{
	if (iPassIndex >= m_vecPassDesc.size())
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint				iStrides[] = {
		m_iStride,
		m_iInstanceStride
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContextDevice->IASetVertexBuffers(0, 2, pVertexBuffers, iStrides, iOffsets);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/* 쉐이터 패스를 시작한다. */
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_pContextDevice->DrawIndexedInstanced(m_iNumIndicesPerPT * 2, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer_Instance::Create_VertexInstanceBuffer()
{
	if (FAILED(m_pGraphicDevice->CreateBuffer(&m_VBInstanceDesc, &m_VBInstanceSubResourceData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
}
