#include "..\Public\VIBuffer_Triangle.h"

CVIBuffer_Triangle::CVIBuffer_Triangle(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Triangle::CVIBuffer_Triangle(const CVIBuffer_Triangle & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Triangle::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName, _float3 * pPoints)
{
	m_iStride = sizeof(VTXNAVTEX);
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

	((VTXNAVTEX*)m_pVertices)[0].vPosition = pPoints[0];
	((VTXNAVTEX*)m_pVertices)[0].vTexUV = _float2(0.f, 0.f);

	((VTXNAVTEX*)m_pVertices)[1].vPosition = pPoints[1];
	((VTXNAVTEX*)m_pVertices)[1].vTexUV = _float2(0.f, 1.f);

	((VTXNAVTEX*)m_pVertices)[2].vPosition = pPoints[2];
	((VTXNAVTEX*)m_pVertices)[2].vTexUV = _float2(1.f, 1.f);

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 1;
	m_iNumIndicesPerPT = 3; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(FACEINDICES16);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	((FACEINDICES16*)m_pIndices)[0]._1 = 0;
	((FACEINDICES16*)m_pIndices)[0]._2 = 1;
	((FACEINDICES16*)m_pIndices)[0]._3 = 2;

	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 2, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Triangle::NativeConstruct(void * pArg)
{
	return E_NOTIMPL;
}

CVIBuffer_Triangle * CVIBuffer_Triangle::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName, _float3 * pPoints)
{
	CVIBuffer_Triangle* pInstance = new CVIBuffer_Triangle(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName, pPoints)))
		Safe_Release(pInstance);
	return pInstance;
}

CComponent * CVIBuffer_Triangle::Clone(void * pArg)
{
	return nullptr;
}

void CVIBuffer_Triangle::Free()
{
	__super::Free();
}
