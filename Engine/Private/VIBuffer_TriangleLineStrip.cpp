#include "../Public/VIBuffer_TriangleLineStrip.h"

CVIBuffer_TriangleLineStrip::CVIBuffer_TriangleLineStrip(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_TriangleLineStrip::CVIBuffer_TriangleLineStrip(const CVIBuffer_TriangleLineStrip & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_TriangleLineStrip::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName, _float3 * pPoints)
{

	m_iStride = sizeof(VTXNAVCOL);
	m_iNumVertices = 3;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXNAVCOL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNAVCOL) * m_iNumVertices);

	((VTXNAVCOL*)m_pVertices)[0].vPosition = pPoints[0];
	((VTXNAVCOL*)m_pVertices)[0].vColor = _float4(0.0f, 1.0f, 0.0f, 0.1f);

	((VTXNAVCOL*)m_pVertices)[1].vPosition = pPoints[1];
	((VTXNAVCOL*)m_pVertices)[1].vColor = _float4(0.0f, 1.0f, 0.0f, 0.1f);

	((VTXNAVCOL*)m_pVertices)[2].vPosition = pPoints[2];
	((VTXNAVCOL*)m_pVertices)[2].vColor = _float4(0.0f, 1.0f, 0.0f, 0.1f);

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 1;
	m_iNumIndicesPerPT = 4; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(LINESTRIPFACE16);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	m_pIndices = new LINESTRIPFACE16[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(LINESTRIPFACE16) * m_iNumPrimitive);

	((LINESTRIPFACE16*)m_pIndices)[0]._1 = 0;
	((LINESTRIPFACE16*)m_pIndices)[0]._2 = 1;
	((LINESTRIPFACE16*)m_pIndices)[0]._3 = 2;
	((LINESTRIPFACE16*)m_pIndices)[0]._4 = 0;

	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"   , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 2, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVertices);

	return S_OK;
}

HRESULT CVIBuffer_TriangleLineStrip::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_TriangleLineStrip * CVIBuffer_TriangleLineStrip::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName, _float3 * pPoints)
{
	CVIBuffer_TriangleLineStrip* pInstance = new CVIBuffer_TriangleLineStrip(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName, pPoints)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CVIBuffer_TriangleLineStrip::Clone(void * pArg)
{
	CVIBuffer_TriangleLineStrip* pInstance = new CVIBuffer_TriangleLineStrip(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVIBuffer_TriangleLineStrip::Free()
{
	unsigned long dwRefCnt = 0;

	__super::Free();
}
