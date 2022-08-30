#include "..\Public\VIBuffer_MeshInstance.h"

HRESULT CVIBuffer_MeshInstance::Init_Buffer()
{
	m_iStride = sizeof(VTXMESH);
	m_iNumVertices = 1;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = 0;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXMESH;
	ZeroMemory(m_pVertices, sizeof(VTXMESH));

	((VTXMESH*)m_pVertices)[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = m_iNumInstance;
	m_iNumIndicesPerPT = 1; /* 도형하나그릴때 필요한 인덱스 갯수. */
	m_iFaceIndexSize = sizeof(_ushort);

	m_eIndexFormat = DXGI_FORMAT_R16_UINT;

	ZeroMemory(&m_tIB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tIB_Desc.ByteWidth = m_iFaceIndexSize * m_iNumPrimitive;
	m_tIB_Desc.Usage = D3D11_USAGE_IMMUTABLE;
	m_tIB_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tIB_Desc.CPUAccessFlags = 0;
	m_tIB_Desc.MiscFlags = 0;

	m_pIndices = new _ushort[m_iNumPrimitive];
	ZeroMemory(m_pIndices, sizeof(_ushort) * m_iNumPrimitive);

	m_tIB_SubResourceData.pSysMem = m_pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;


	m_iInstanceStride = sizeof(VTXMATRIX_UV_FADE_DIR);
	ZeroMemory(&m_VBInstanceDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstanceDesc.ByteWidth = m_iInstanceStride * m_iNumInstance;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceStride;

	m_pInstanceVertices = new VTXMATRIX_UV_FADE_DIR[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXMATRIX_UV_FADE_DIR) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vPosition = _float4(m_pPosition[i].x, m_pPosition[i].y, m_pPosition[i].z, 1.f);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV = _float4(0.f, 0.f, 0.f, 0.f);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV.z = 1.f / m_tDesc.iWidth;
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vUV.w = 1.f / m_tDesc.iHeight;
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.x = static_cast<_float>(m_tDesc.bFadeIn);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.y = 0.f;
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.z = static_cast<_float>(m_tDesc.bFadeOut);
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vFade.w = 1.f;
		//((VTXMATRIX_UV_FADE_DIR*)m_pInstanceVertices)[i].vDirection = _float4(m_tDesc.vDirection.x, m_tDesc.vDirection.y, m_tDesc.vDirection.z, 0.f);
	}

	m_VBInstanceSubResourceData.pSysMem = m_pInstanceVertices;

	if (FAILED(Create_VertexInstanceBuffer()))
		return E_FAIL;

	Safe_Delete_Array(m_pInstanceVertices);

	Safe_Delete_Array(m_pIndices);
	Safe_Delete_Array(m_pVertices);



	return S_OK;
}

CVIBuffer_MeshInstance::CVIBuffer_MeshInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{
}

CVIBuffer_MeshInstance::CVIBuffer_MeshInstance(const CVIBuffer_MeshInstance & rhs)
	: CVIBuffer_Instance(rhs)
{
}

HRESULT CVIBuffer_MeshInstance::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 5, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 6, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 7, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 9, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_MeshInstance::NativeConstruct(void * pArg)
{
	MESHINST_DESC* pDesc = (MESHINST_DESC*)pArg;
	m_tDesc = *pDesc;

	m_iNumInstance = m_tDesc.iNumber;

	m_pPosition = new _float3[m_iNumInstance];
	m_pDirection = new _float3[m_iNumInstance];
	m_pSpeed = new _float[m_iNumInstance];
	m_pLifeTime = new _float[m_iNumInstance];
	m_pCurrentIndex = new _int[m_iNumInstance];
	m_pSavedLifeTime = new _float[m_iNumInstance];
	ZeroMemory(m_pPosition, sizeof(_float3) * m_iNumInstance);
	ZeroMemory(m_pDirection, sizeof(_float3) * m_iNumInstance);
	ZeroMemory(m_pSpeed, sizeof(_float) * m_iNumInstance);
	ZeroMemory(m_pLifeTime, sizeof(_float) * m_iNumInstance);
	ZeroMemory(m_pCurrentIndex, sizeof(_int) * m_iNumInstance);
	ZeroMemory(m_pSavedLifeTime, sizeof(_float) * m_iNumInstance);


	Update_ByDesc();

	if (FAILED(Init_Buffer()))
	{
		return E_FAIL;
	}

	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_MeshInstance::Render(_uint iPassIndex)
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
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* 쉐이터 패스를 시작한다. */
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_pContextDevice->DrawIndexedInstanced(m_iNumIndicesPerPT, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_MeshInstance::Update(_double TimeDelta)
{
}

void CVIBuffer_MeshInstance::Init()
{
}

void CVIBuffer_MeshInstance::Set_Desc(const POINTINST_DESC & _tDesc)
{
}

void CVIBuffer_MeshInstance::Update_ByDesc()
{
}

CVIBuffer_MeshInstance * CVIBuffer_MeshInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	return nullptr;
}

CComponent * CVIBuffer_MeshInstance::Clone(void * pArg)
{
	return nullptr;
}

void CVIBuffer_MeshInstance::Free()
{
}
