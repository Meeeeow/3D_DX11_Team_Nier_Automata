#include "..\Public\VIBuffer_RectInstance.h"

CVIBuffer_RectInstance::CVIBuffer_RectInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{

}

CVIBuffer_RectInstance::CVIBuffer_RectInstance(const CVIBuffer_RectInstance & rhs)
	: CVIBuffer_Instance(rhs)
	, m_pDropSpeed(rhs.m_pDropSpeed)
{

}

HRESULT CVIBuffer_RectInstance::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXRECTTEX);
	m_iNumVertices = 4;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXRECTTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXRECTTEX) * m_iNumVertices);

	((VTXRECTTEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	((VTXRECTTEX*)m_pVertices)[0].vTexUV = _float2(0.0f, 0.f);

	((VTXRECTTEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	((VTXRECTTEX*)m_pVertices)[1].vTexUV = _float2(1.0f, 0.f);

	((VTXRECTTEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	((VTXRECTTEX*)m_pVertices)[2].vTexUV = _float2(1.0f, 1.f);

	((VTXRECTTEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	((VTXRECTTEX*)m_pVertices)[3].vTexUV = _float2(0.0f, 1.f);

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 2 * m_iNumInstance;
	m_iNumIndicesPerPT = 3; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(FACEINDICES16);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	m_pIndices = new FACEINDICES16[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(FACEINDICES16) * m_iNumPrimitive);

	_uint		iNumInstance = 0;

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((FACEINDICES16*)m_pIndices)[iNumInstance]._1 = 0;
		((FACEINDICES16*)m_pIndices)[iNumInstance]._2 = 1;
		((FACEINDICES16*)m_pIndices)[iNumInstance]._3 = 2;
		++iNumInstance;

		((FACEINDICES16*)m_pIndices)[iNumInstance]._1 = 0;
		((FACEINDICES16*)m_pIndices)[iNumInstance]._2 = 2;
		((FACEINDICES16*)m_pIndices)[iNumInstance]._3 = 3;
		++iNumInstance;
	}


	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;


	m_iInstanceStride = sizeof(VTXMATRIX);
	ZeroMemory(&m_VBInstanceDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstanceDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)m_pInstanceVertices)[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		((VTXMATRIX*)m_pInstanceVertices)[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		((VTXMATRIX*)m_pInstanceVertices)[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		((VTXMATRIX*)m_pInstanceVertices)[i].vPosition = _float4((_float)(rand() % 10), 15.0f, (_float)(rand() % 10), 1.f);
	}

	m_VBInstanceSubResourceData.pSysMem = m_pInstanceVertices;

	if (FAILED(Create_VertexInstanceBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pInstanceVertices);

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 6, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	m_pDropSpeed = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		m_pDropSpeed[i] = (_float)(rand() % 10 + 5);
	}

	return S_OK;
}

HRESULT CVIBuffer_RectInstance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_RectInstance::Update(_double TimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResourceData;

	m_pContextDevice->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y -= m_pDropSpeed[i] * (_float)TimeDelta;

		if (((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y < -1.f)
			((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y = 15.0f;
	}

	m_pContextDevice->Unmap(m_pVBInstance, 0);
}

CVIBuffer_RectInstance * CVIBuffer_RectInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer_RectInstance* pGameInstance = new CVIBuffer_RectInstance(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSGBOX("Failed to Created CVIBuffer_RectInstance");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent * CVIBuffer_RectInstance::Clone(void * pArg)
{
	CVIBuffer_RectInstance* pGameInstance = new CVIBuffer_RectInstance(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_RectInstance");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVIBuffer_RectInstance::Free()
{
	__super::Free();

	if (false == m_bCloned)
		Safe_Delete_Array(m_pDropSpeed);
}
