#include "..\Public\VIBuffer_Viewport.h"

CVIBuffer_Viewport::CVIBuffer_Viewport(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Viewport::CVIBuffer_Viewport(const CVIBuffer_Viewport & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Viewport::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName, _float4 vScreenInfo)
{
	m_iStride = sizeof(VTXRECTTEX);
	m_iNumVertices = 4;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_pVertices = new VTXRECTTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXRECTTEX) * m_iNumVertices);

	D3D11_VIEWPORT		tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint				iNumViewports = 1;
	m_pContextDevice->RSGetViewports(&iNumViewports, &tViewportDesc);

	_float				fLeftX = vScreenInfo.x;
	_float				fLeftY = vScreenInfo.y;
	_float				fSizeX = vScreenInfo.z;
	_float				fSizeY = vScreenInfo.w;


	((VTXRECTTEX*)m_pVertices)[0].vPosition = _float3(
		fLeftX / (tViewportDesc.Width * 0.5f) - 1.f, fLeftY / (tViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXRECTTEX*)m_pVertices)[0].vTexUV = _float2(0.0f, 0.f);

	((VTXRECTTEX*)m_pVertices)[1].vPosition = _float3(
		(fLeftX + fSizeX) / (tViewportDesc.Width * 0.5f) - 1.f, fLeftY / (tViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXRECTTEX*)m_pVertices)[1].vTexUV = _float2(1.0f, 0.f);

	((VTXRECTTEX*)m_pVertices)[2].vPosition = _float3(
		(fLeftX + fSizeX) / (tViewportDesc.Width * 0.5f) - 1.f, (fLeftY + fSizeY) / (tViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXRECTTEX*)m_pVertices)[2].vTexUV = _float2(1.0f, 1.f);

	((VTXRECTTEX*)m_pVertices)[3].vPosition = _float3(
		(fLeftX) / (tViewportDesc.Width * 0.5f) - 1.f, (fLeftY + fSizeY) / (tViewportDesc.Height * -0.5f) + 1.f, 0.f);
	((VTXRECTTEX*)m_pVertices)[3].vTexUV = _float2(0.0f, 1.f);
	ZeroMemory(&m_tVB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 2;
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

	((FACEINDICES16*)m_pIndices)[1]._1 = 0;
	((FACEINDICES16*)m_pIndices)[1]._2 = 2;
	((FACEINDICES16*)m_pIndices)[1]._3 = 3;

	ZeroMemory(&m_tIB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(__super::Compile_Shader(ElementDesc, 2, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Viewport::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_Viewport * CVIBuffer_Viewport::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName, _float4 vScreenInfo)
{
	CVIBuffer_Viewport* pInstance = new CVIBuffer_Viewport(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName, vScreenInfo)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CVIBuffer_Viewport::Clone(void * pArg)
{
	CVIBuffer_Viewport* pInstance = new CVIBuffer_Viewport(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVIBuffer_Viewport::Free()
{
	__super::Free();
}
