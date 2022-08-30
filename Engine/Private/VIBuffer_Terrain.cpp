#include "..\Public\VIBuffer_Terrain.h"


CVIBuffer_Terrain::CVIBuffer_Terrain(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar * pHeightMapFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == 0)
		FAILMSG("VIBuffer Terrain - CreateFile");

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	_ulong*		pPixel = new _ulong[m_iNumVertices];

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVertices, &dwByte, nullptr);

	m_iStride = sizeof(VTXRECTNORMTEX);
	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXRECTNORMTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXRECTNORMTEX)* m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			((VTXRECTNORMTEX*)m_pVertices)[iIndex].vPosition = _float3(_float(j), (pPixel[iIndex] & 0x000000ff) / 10.f, _float(i));
			((VTXRECTNORMTEX*)m_pVertices)[iIndex].vNormal = _float3(0.f, 0.f, 0.f); // 지금 채울 수 없다. 포지션 정점을 다 채운후에 채울 수 있음.
			((VTXRECTNORMTEX*)m_pVertices)[iIndex].vTexUV = _float2(_float(j) / m_iNumVerticesX - 1, _float(i) / m_iNumVerticesZ - 1);
		}
	}

	ZeroMemory(&m_tVB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tVB_SubResourceData.pSysMem = m_pVertices; /* m_pVertices가 void* 인 이유, pSystem 이 const void* 임 */

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2; /* 그릴 삼각형의 개수 */
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; /* 그릴 방식 */
	m_iNumIndicesPerPT = 3; /* 도형을 하나 그릴때 필요한 개수 */
	m_iFaceIndexSize = sizeof(FACEINDICES32);


	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.MiscFlags = 0;
	m_tIB_Desc.CPUAccessFlags = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32)* m_iNumPrimitive);

	_uint		iNumFace = 0;

	VTXRECTNORMTEX*	pVertices = (VTXRECTNORMTEX*)m_pVertices;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector		vSour, vDest, vNormal;				/* Normal 벡터 계산을 위한 Sour Dest 벡터 */

			((FACEINDICES32*)m_pIndices)[iIndex]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iIndex]._2 = iIndices[1];
			((FACEINDICES32*)m_pIndices)[iIndex]._3 = iIndices[2];

			vSour = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition) -
				XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vPosition);
			vDest = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vPosition) -
				XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal) + vNormal));

			++iNumFace;

			((FACEINDICES32*)m_pIndices)[iIndex]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iIndex]._2 = iIndices[2];
			((FACEINDICES32*)m_pIndices)[iIndex]._3 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition) -
				XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vPosition);
			vDest = XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vPosition) -
				XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._1].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._2].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[((FACEINDICES32*)m_pIndices)[iNumFace]._3].vNormal) + vNormal));

			++iNumFace;
		}
	}
	ZeroMemory(&m_tIB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_VertexBuffer()))
		FAILMSG("VIBuffer Terrain - Create_VertexBuffer");

	if (FAILED(__super::Create_IndexBuffer()))
		FAILMSG("VIBuffer Terrain - Create_IndexBuffer");


	/*
	Element Desc 정보
	#1. Name
	#2. Index
	#3. DXGI_FORMAT
	#4. Input Slot
	#5. OffSet
	#6. Input Class
	#7. 인스턴스 별 동일한 데이터를 그리는 인스턴스 수. 단, #6. 이 D3D11_INPUT_PER_VERTEX_DATA 이면 0이어야 한다.
	*/

	D3D11_INPUT_ELEMENT_DESC	tElementDesc[] = {
		{ "POSITION"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(tElementDesc, 3, pShaderFilePath, pTechniqueName)))
		FAILMSG("VIBuffer Terrain - Compile_Shader");

	Safe_Delete_Array(pPixel);
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(_float4 vTerrainInfo, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	return S_OK;
}

_bool CVIBuffer_Terrain::Picking(_float3 * pOut, _matrix matWorld, _float & fDist)
{
	
	return _bool();
}

_float4 CVIBuffer_Terrain::Compute_Height(_fvector vPosition)
{
	return _float4();
}

_float4 CVIBuffer_Terrain::Compute_Height(_fvector vPosition, _fmatrix matTerrainMatrix)
{
	return _float4();
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pHeightMapFilePath, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	return nullptr;
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _float4 vTerrainInfo, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	return nullptr;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	return nullptr;
}

void CVIBuffer_Terrain::Free()
{
}
