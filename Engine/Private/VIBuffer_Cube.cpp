#include "..\Public\VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Cube::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));
	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXCUBETEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXCUBETEX) * m_iNumVertices);

	((VTXCUBETEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[0].vTexUVW = ((VTXCUBETEX*)m_pVertices)[0].vPosition;

	((VTXCUBETEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[1].vTexUVW = ((VTXCUBETEX*)m_pVertices)[1].vPosition;

	((VTXCUBETEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[2].vTexUVW = ((VTXCUBETEX*)m_pVertices)[2].vPosition;

	((VTXCUBETEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[3].vTexUVW = ((VTXCUBETEX*)m_pVertices)[3].vPosition;

	((VTXCUBETEX*)m_pVertices)[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[4].vTexUVW = ((VTXCUBETEX*)m_pVertices)[4].vPosition;

	((VTXCUBETEX*)m_pVertices)[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[5].vTexUVW = ((VTXCUBETEX*)m_pVertices)[5].vPosition;

	((VTXCUBETEX*)m_pVertices)[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[6].vTexUVW = ((VTXCUBETEX*)m_pVertices)[6].vPosition;

	((VTXCUBETEX*)m_pVertices)[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[7].vTexUVW = ((VTXCUBETEX*)m_pVertices)[7].vPosition;

	m_tVB_SubResourceData.pSysMem = m_pVertices;
	if (FAILED(Create_VertexBuffer()))
		return E_FAIL;


	m_iNumPrimitive = 12;
	m_iNumIndicesPerPT = 3;
	m_iFaceIndexSize = sizeof(FACEINDICES16);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));
	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.MiscFlags = 0;
	m_tIB_Desc.CPUAccessFlags = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);


	((FACEINDICES16*)m_pIndices)[0]._1 = 1; ((FACEINDICES16*)m_pIndices)[0]._2 = 5; ((FACEINDICES16*)m_pIndices)[0]._3 = 6; // +X
	((FACEINDICES16*)m_pIndices)[1]._1 = 1; ((FACEINDICES16*)m_pIndices)[1]._2 = 6; ((FACEINDICES16*)m_pIndices)[1]._3 = 2; // +X

	((FACEINDICES16*)m_pIndices)[2]._1 = 4; ((FACEINDICES16*)m_pIndices)[2]._2 = 0; ((FACEINDICES16*)m_pIndices)[2]._3 = 3; // -X
	((FACEINDICES16*)m_pIndices)[3]._1 = 4; ((FACEINDICES16*)m_pIndices)[3]._2 = 3; ((FACEINDICES16*)m_pIndices)[3]._3 = 7; // -X

	((FACEINDICES16*)m_pIndices)[4]._1 = 4; ((FACEINDICES16*)m_pIndices)[4]._2 = 5; ((FACEINDICES16*)m_pIndices)[4]._3 = 1; // +Y
	((FACEINDICES16*)m_pIndices)[5]._1 = 4; ((FACEINDICES16*)m_pIndices)[5]._2 = 1; ((FACEINDICES16*)m_pIndices)[5]._3 = 0; // +Y

	((FACEINDICES16*)m_pIndices)[6]._1 = 3; ((FACEINDICES16*)m_pIndices)[6]._2 = 2; ((FACEINDICES16*)m_pIndices)[6]._3 = 6; // -Y
	((FACEINDICES16*)m_pIndices)[7]._1 = 3; ((FACEINDICES16*)m_pIndices)[7]._2 = 6; ((FACEINDICES16*)m_pIndices)[7]._3 = 7; // -Y

	((FACEINDICES16*)m_pIndices)[8]._1 = 5; ((FACEINDICES16*)m_pIndices)[8]._2 = 4; ((FACEINDICES16*)m_pIndices)[8]._3 = 7; // +Z
	((FACEINDICES16*)m_pIndices)[9]._1 = 5; ((FACEINDICES16*)m_pIndices)[9]._2 = 7; ((FACEINDICES16*)m_pIndices)[9]._3 = 6; // +Z

	((FACEINDICES16*)m_pIndices)[10]._1 = 0; ((FACEINDICES16*)m_pIndices)[10]._2 = 1; ((FACEINDICES16*)m_pIndices)[10]._3 = 2; // -Z
	((FACEINDICES16*)m_pIndices)[11]._1 = 0; ((FACEINDICES16*)m_pIndices)[11]._2 = 2; ((FACEINDICES16*)m_pIndices)[11]._3 = 3; // -Z

	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC			tElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(tElementDesc, 2, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Cube::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_Cube * CVIBuffer_Cube::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}
