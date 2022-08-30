#include "..\Public\VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	D3D11_INPUT_ELEMENT_DESC	tElementDesc[] = {
		{ "POSITION"	, 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD" , 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(tElementDesc, 2, pShaderFilePath, pTechniqueName)))
		FAILMSG("VIBuffer Trail - Compile_Shader");


	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void * pArg)
{
	_int* pNumX = (_int*)pArg;

	m_iNumVerticesX = *pNumX;
	if (FAILED(Init_Buffer()))
	{
		return E_FAIL;
	}

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_Trail::Init_Buffer()
{
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iStride = sizeof(VTXRECTTEX);
	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXRECTTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXRECTTEX)* m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			((VTXRECTTEX*)m_pVertices)[iIndex].vPosition = _float3(0.f, 0.f, 0.f);
			((VTXRECTTEX*)m_pVertices)[iIndex].vTexUV = _float2(_float(j) / m_iNumVerticesX - 1, _float(i) / m_iNumVerticesZ - 1);
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

	//VTXRECTTEX*	pVertices = (VTXRECTTEX*)m_pVertices;

	_int iNumFace = 0;

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

			((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[1];
			((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[2];

			++iNumFace;

			((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
			((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[2];
			((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[3];

			++iNumFace;
		}
	}
	ZeroMemory(&m_tIB_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(__super::Create_VertexBuffer()))
		FAILMSG("VIBuffer Trail - Create_VertexBuffer");

	if (FAILED(__super::Create_IndexBuffer()))
		FAILMSG("VIBuffer Trail - Create_IndexBuffer");



	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVertices);


	return S_OK;
}

void CVIBuffer_Trail::Add_LastPosition(_float3 _vMinPoint, _float3 _vMaxPoint)
{
	if (nullptr == m_pContextDevice)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	m_pContextDevice->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	// i == 0 ~ 1
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;
			if (iIndex + m_iNumVerticesZ > m_iNumVertices - 1)
			{
				break;
			}

			((VTXRECTTEX*)SubResourceData.pData)[iIndex].vPosition = ((VTXRECTTEX*)SubResourceData.pData)[iIndex + m_iNumVerticesZ].vPosition;
		}
	}
	_uint iIndex = m_iNumVerticesX * m_iNumVerticesZ - 2;
	((VTXRECTTEX*)SubResourceData.pData)[iIndex].vPosition = _vMinPoint;
	((VTXRECTTEX*)SubResourceData.pData)[iIndex + 1].vPosition = _vMaxPoint;
	m_pContextDevice->Unmap(m_pVB, 0);
}

void CVIBuffer_Trail::Init_AllPosition(_float3 _vMinPoint, _float3 _vMaxPoint)
{
	if (nullptr == m_pContextDevice)
		return;

	D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	m_pContextDevice->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	// i == 0 ~ 1
	for (_uint j = 0; j < m_iNumVerticesX; ++j)
	{
		for (_uint i = 0; i < m_iNumVerticesZ; ++i)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			if (0 == i)
			{
				((VTXRECTTEX*)SubResourceData.pData)[iIndex].vPosition = _vMinPoint;
			}
			else if (1 == i)
			{
				((VTXRECTTEX*)SubResourceData.pData)[iIndex].vPosition = _vMaxPoint;
			}
			else
			{
				assert(false);
			}

		}
	}
	m_pContextDevice->Unmap(m_pVB, 0);
}

CVIBuffer_Trail * CVIBuffer_Trail::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_Trail* pGameInstance = new CVIBuffer_Trail(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSGBOX("Failed to Create CVIBuffer_Trail");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail* pGameInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CVIBuffer_Trail");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

}
