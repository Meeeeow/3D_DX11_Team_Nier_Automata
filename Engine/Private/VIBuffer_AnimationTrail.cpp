#include "..\Public\VIBuffer_AnimationTrail.h"

CVIBuffer_AnimationTrail::CVIBuffer_AnimationTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CVIBuffer(pGraphicDevice, pContextDevice)
{

}

CVIBuffer_AnimationTrail::CVIBuffer_AnimationTrail(const CVIBuffer_AnimationTrail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_AnimationTrail::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;


	D3D11_INPUT_ELEMENT_DESC	tElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(tElementDesc, 2, pShaderFilePath, pTechniqueName)))
		FAILMSG("VIBuffer Trail - Compile_Shader");


	return S_OK;
}

HRESULT CVIBuffer_AnimationTrail::NativeConstruct(void * pArg)
{
	_int* pNumX = (_int*)pArg;

	//m_iNumVerticesX = *pNumX * 2;
	m_iNumVerticesX = *pNumX;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;

	m_iNumRawXPosition = *pNumX;
	m_iNumRawPosition = m_iNumRawXPosition * m_iNumVerticesZ;

	m_arrRawPosition = new _float3[m_iNumRawPosition];

	if (FAILED(Init_Buffer()))
	{
		return E_FAIL;
	}

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CVIBuffer_AnimationTrail::Init_Buffer()
{

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
			((VTXRECTTEX*)m_pVertices)[iIndex].vTexUV = _float2(j / static_cast<_float>(m_iNumVerticesX), i / static_cast<_float>(m_iNumVerticesZ));
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

	VTXRECTTEX*	pVertices = (VTXRECTTEX*)m_pVertices;

	_int iNumFace = 0;

	for (_uint i = 0; i < m_iNumVertices - 3; i += 2)
	{
		_uint		iIndex = i;

		_uint		iIndices[] = {
			iIndex,				// 0
			iIndex + 1,			// 1
			iIndex + 2,			// 2
			iIndex + 3,			// 3
		};

		((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
		((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[1];
		((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[2];

		++iNumFace;

		((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[1];
		((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[3];
		((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[2];

		++iNumFace;


	}


	//_int iNumFace = 0;

	//for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	//{
	//	for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
	//	{
	//		_uint		iIndex = i * m_iNumVerticesX + j;

	//		_uint		iIndices[] = {
	//			iIndex + m_iNumVerticesX,
	//			iIndex + m_iNumVerticesX + 1,
	//			iIndex + 1,
	//			iIndex
	//		};

	//		((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
	//		((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[1];
	//		((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[2];

	//		++iNumFace;

	//		((FACEINDICES32*)m_pIndices)[iNumFace]._1 = iIndices[0];
	//		((FACEINDICES32*)m_pIndices)[iNumFace]._2 = iIndices[2];
	//		((FACEINDICES32*)m_pIndices)[iNumFace]._3 = iIndices[3];

	//		++iNumFace;
	//	}
	//}
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

void CVIBuffer_AnimationTrail::Add_LastPosition(_vector _vMinPoint, _vector _vMaxPoint)
{
	for (_uint i = 0; i < m_iNumRawPosition - 2; ++i)
	{
		m_arrRawPosition[i] = m_arrRawPosition[i + 2];
	}
	//_uint iIndex = m_iNumVerticesX * m_iNumVerticesZ - 2;
	_uint iIndex = m_iNumRawPosition - 2;
	XMStoreFloat3(&m_arrRawPosition[iIndex], _vMinPoint);
	XMStoreFloat3(&m_arrRawPosition[iIndex + 1], _vMaxPoint);

	//if (nullptr == m_pContextDevice)
	//	return;

	//D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	//m_pContextDevice->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	//for (_uint i = 0; i < m_iNumVertices - 2; ++i)
	//{
	//	((VTXRECTTEX*)SubResourceData.pData)[i].vPosition = ((VTXRECTTEX*)SubResourceData.pData)[i + 2].vPosition;
	//}
	////_uint iIndex = m_iNumVerticesX * m_iNumVerticesZ - 2;
	//iIndex = m_iNumVertices - 2;
	//XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[iIndex].vPosition, _vMinPoint);
	//XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[iIndex + 1].vPosition, _vMaxPoint);
	//m_pContextDevice->Unmap(m_pVB, 0);
}

void CVIBuffer_AnimationTrail::Init_AllPosition(_vector _vMinPoint, _vector _vMaxPoint)
{
	for (_uint i = 0; i < m_iNumRawPosition; ++i)
	{
		if (i % 2 == 0)
		{
			XMStoreFloat3(&m_arrRawPosition[i], _vMinPoint);
		}
		else
		{
			XMStoreFloat3(&m_arrRawPosition[i], _vMaxPoint);
		}
	}

	//if (nullptr == m_pContextDevice)
	//	return;

	//D3D11_MAPPED_SUBRESOURCE		SubResourceData;
	//m_pContextDevice->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);
	//for (_uint i = 0; i < m_iNumVertices; ++i)
	//{
	//	if (0 == i % 2)
	//	{
	//		XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[i].vPosition, _vMinPoint);
	//	}
	//	else if (1 == i % 2)
	//	{
	//		XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[i].vPosition, _vMaxPoint);
	//	}
	//	else
	//	{
	//		assert(false);
	//	}
	//}
	//m_pContextDevice->Unmap(m_pVB, 0);
}


void CVIBuffer_AnimationTrail::Calculate_Catmullrom()
{
	if (nullptr == m_pContextDevice)
		return;



	// i == 0 ~ 1
	// Min : Even Numbers
	//_vector vMinFirstPos
	//	= XMLoadFloat3(&_float3(m_arrRawPosition[0].x - m_arrRawPosition[2].x
	//						  , m_arrRawPosition[0].y - m_arrRawPosition[2].y
	//						  , m_arrRawPosition[0].z - m_arrRawPosition[2].z));
	_vector vMinFirstPos
		= XMLoadFloat3(&m_arrRawPosition[0]);
	vMinFirstPos = XMVectorSetW(vMinFirstPos, 1.f);


	//_vector vMinLastPos
	//	= XMLoadFloat3(&_float3(m_arrRawPosition[m_iNumRawPosition - 2].x - m_arrRawPosition[m_iNumRawPosition - 4].x
	//						  , m_arrRawPosition[m_iNumRawPosition - 2].y - m_arrRawPosition[m_iNumRawPosition - 4].y
	//						  , m_arrRawPosition[m_iNumRawPosition - 2].z - m_arrRawPosition[m_iNumRawPosition - 4].z));
	_vector vMinLastPos
		= XMLoadFloat3(&m_arrRawPosition[m_iNumRawPosition - 2]);
	vMinLastPos = XMVectorSetW(vMinLastPos, 1.f);


	//_vector vMinLastPos
	//	= XMLoadFloat3(&m_arrRawPosition[m_iNumRawPosition - 2]);
	//vMinLastPos = XMVectorSetW(vMinLastPos, 1.f);

	_int iIndex = static_cast<_int>(m_iNumRawPosition * 0.25f);
	if (iIndex % 2 != 0)
	{
		iIndex += 1;
	}
	_vector vMinSecondPos = XMLoadFloat3(&m_arrRawPosition[iIndex]);
	vMinSecondPos = XMVectorSetW(vMinSecondPos, 1.f);

	iIndex = static_cast<_int>(m_iNumRawPosition * 0.75f);
	if (iIndex % 2 != 0)
	{
		iIndex -= 1;
	}
	_vector vMinThirdPos = XMLoadFloat3(&m_arrRawPosition[iIndex]);
	vMinThirdPos = XMVectorSetW(vMinThirdPos, 1.f);

	// Max : Odd Numbers
	//_vector vMaxFirstPos = XMLoadFloat3(&_float3(m_arrRawPosition[1].x - m_arrRawPosition[3].x
	//										   , m_arrRawPosition[1].y - m_arrRawPosition[3].y
	//										   , m_arrRawPosition[1].z - m_arrRawPosition[3].z));
	_vector vMaxFirstPos = XMLoadFloat3(&m_arrRawPosition[1]);
	vMaxFirstPos = XMVectorSetW(vMaxFirstPos, 1.f);

	//_vector vMaxLastPos = XMLoadFloat3(&_float3(m_arrRawPosition[m_iNumRawPosition - 1].x - m_arrRawPosition[m_iNumRawPosition - 3].x
	//										  , m_arrRawPosition[m_iNumRawPosition - 1].y - m_arrRawPosition[m_iNumRawPosition - 3].y
	//										  , m_arrRawPosition[m_iNumRawPosition - 1].z - m_arrRawPosition[m_iNumRawPosition - 3].z));
	_vector vMaxLastPos = XMLoadFloat3(&m_arrRawPosition[m_iNumRawPosition - 1]);
	vMaxLastPos = XMVectorSetW(vMaxLastPos, 1.f);

	iIndex = static_cast<_int>(m_iNumRawPosition * 0.25f);
	if (iIndex % 2 == 0)
	{
		iIndex += 1;
	}
	_vector vMaxSecondPos = XMLoadFloat3(&m_arrRawPosition[iIndex]);
	vMaxSecondPos = XMVectorSetW(vMaxSecondPos, 1.f);
	iIndex = static_cast<_int>(m_iNumRawPosition * 0.75f);
	if (iIndex % 2 == 0)
	{
		iIndex -= 1;
	}
	_vector vMaxThirdPos = XMLoadFloat3(&m_arrRawPosition[iIndex]);
	vMaxThirdPos = XMVectorSetW(vMaxThirdPos, 1.f);


	D3D11_MAPPED_SUBRESOURCE		SubResourceData;
	m_pContextDevice->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	_float fRatio = 0.f;
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		fRatio = i / static_cast<_float>(m_iNumVertices);

		if (i % 2 == 0)
		{
			XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[i].vPosition, XMVectorCatmullRom(vMinFirstPos, vMinSecondPos, vMinThirdPos, vMinLastPos, fRatio));
		}
		else
		{
			XMStoreFloat3(&((VTXRECTTEX*)SubResourceData.pData)[i].vPosition, XMVectorCatmullRom(vMaxFirstPos, vMaxSecondPos, vMaxThirdPos, vMaxLastPos, fRatio));
		}
	}

	m_pContextDevice->Unmap(m_pVB, 0);
}


CVIBuffer_AnimationTrail * CVIBuffer_AnimationTrail::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CVIBuffer_AnimationTrail* pGameInstance = new CVIBuffer_AnimationTrail(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSGBOX("Failed to Create CVIBuffer_AnimationTrail");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent * CVIBuffer_AnimationTrail::Clone(void * pArg)
{
	CVIBuffer_AnimationTrail* pGameInstance = new CVIBuffer_AnimationTrail(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CVIBuffer_AnimationTrail");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVIBuffer_AnimationTrail::Free()
{
	__super::Free();
	Safe_Delete_Array(m_arrRawPosition);
}
