#include "..\Public\VIBuffer_Sphere.h"

#define PI 3.141592f

CVIBuffer_Sphere::CVIBuffer_Sphere(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere & rhs)
	: CVIBuffer(rhs), m_iNumHorizontal(rhs.m_iNumHorizontal), m_iNumVertical(rhs.m_iNumVertical), m_fRadius(rhs.m_fRadius)
{

}

HRESULT CVIBuffer_Sphere::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iNumVertical, _uint iNumHorizontal, _float fRadius, _float4 vColor)
{
	m_iNumHorizontal = iNumHorizontal;
	m_iNumVertical = iNumVertical;
	m_fRadius = fRadius;

	m_iStride = sizeof(VTXSPHERECOL);


	float fThetaStep = PI / m_iNumHorizontal;
	float fPIStep = PI * 2.f / m_iNumVertical;

	m_iNumVertices = m_iNumHorizontal * m_iNumVertical;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXSPHERECOL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXSPHERECOL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumHorizontal; ++i)
	{
		float theta = i * fThetaStep;

		for (_uint j = 0; j < m_iNumVertical; ++j)
		{
			_uint iIndex = i * m_iNumVertical + j;
			float phi = j * fPIStep;

			float fX = fRadius * sinf(theta) * cosf(phi);
			float fY = fRadius * cosf(theta);
			float fZ = fRadius * sinf(theta) * sinf(phi);

			((VTXSPHERECOL*)m_pVertices)[iIndex].vPosition = _float3(fX, fY, fZ);
			((VTXSPHERECOL*)m_pVertices)[iIndex].vColor = vColor;
		}
	}

	m_tVB_SubResourceData.pSysMem = m_pVertices;
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = (m_iNumHorizontal - 1) * (m_iNumVertical) * 2;
	m_eRenderType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iNumIndicesPerPT = 3;
	m_iFaceIndexSize = sizeof(FACEINDICES32);

	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.MiscFlags = 0;
	m_tIB_Desc.CPUAccessFlags = 0;

	m_pIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES32)* m_iNumPrimitive);

	_uint iNumFace = 0;

	for (_uint i = 0; i < m_iNumHorizontal - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVertical; ++j)
		{
			_uint	iIndex = i * m_iNumVertical + j;

			_uint	iIndices[] = {
				iIndex + m_iNumVertical,
				iIndex + m_iNumVertical + 1,
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

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	D3D11_INPUT_ELEMENT_DESC	tElementDesc[] = {
		{ "POSITION" , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR"    , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(tElementDesc, 2, pShaderFilePath, pTechniqueName)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Sphere::NativeConstruct(void * pArg)
{
	return S_OK;
}

CVIBuffer_Sphere * CVIBuffer_Sphere::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iNumVertical, _uint iNumHorizontal, _float fRadius, _float4 vColor)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName, iNumVertical, iNumHorizontal, fRadius, vColor)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CVIBuffer_Sphere::Clone(void * pArg)
{
	CVIBuffer_Sphere* pInstance = new CVIBuffer_Sphere(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();
}
