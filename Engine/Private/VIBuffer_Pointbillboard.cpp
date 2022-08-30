#include "..\Public\VIBuffer_Pointbillboard.h"

CVIBuffer_Pointbillboard::CVIBuffer_Pointbillboard(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_Pointbillboard::CVIBuffer_Pointbillboard(const CVIBuffer_Pointbillboard & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Pointbillboard::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXPOINTUV);
	m_iNumVertices = 1;

	ZeroMemory(&m_tVB_Desc, sizeof(D3D11_BUFFER_DESC));

	m_tVB_Desc.ByteWidth = m_iStride * m_iNumVertices;
	m_tVB_Desc.Usage = D3D11_USAGE_DYNAMIC;
	m_tVB_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tVB_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tVB_Desc.MiscFlags = 0;
	m_tVB_Desc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINTUV;
	ZeroMemory(m_pVertices, sizeof(VTXPOINTUV));

	((VTXPOINTUV*)m_pVertices)[0].vPosition = _float3(0.0f, 0.0f, 0.f);
	((VTXPOINTUV*)m_pVertices)[0].fSize = 1.f;
	((VTXPOINTUV*)m_pVertices)[0].vUV = _float4(0.f, 0.f, 0.f, 0.f);

	m_tVB_SubResourceData.pSysMem = m_pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 1;
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

	D3D11_INPUT_ELEMENT_DESC			ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(__super::Compile_Shader(ElementDesc, 3, pShaderFilePath, pTechniqueName)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Pointbillboard::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	//POINTBILLBOARDDESC* pDesc = (POINTBILLBOARDDESC*)pArg;
	//m_Desc = *pDesc;

	return S_OK;
}

HRESULT CVIBuffer_Pointbillboard::Render(_uint iPassIndex)
{
	if (iPassIndex >= m_vecPassDesc.size())
		return E_FAIL;

	/* 하나의 버퍼를 바인드한다로 가정. */
	/* 다수버퍼를 사용해야하는 경우, 오버라ㅇㅣ드하여 자식에서 호출. */
	_uint		iOffset = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iPassIndex]->pInputLayout);
	m_pContextDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	/* 쉐이터 패스를 시작한다. */
	m_vecPassDesc[iPassIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_pContextDevice->DrawIndexed(m_iNumPrimitive * m_iNumIndicesPerPT, 0, 0);

	return S_OK;
}

void CVIBuffer_Pointbillboard::Update(_double TimeDelta)
{
}

//_bool CVIBuffer_Pointbillboard::Picking(const _float3* pMouseRay, const _float3* pPivot, _float3* pOut, _uint* pIndex)
//{
//	_float	fDist;
//
//	{
//		FXMVECTOR	vPivot = XMLoadFloat3(pPivot);
//		FXMVECTOR	vMouseRay = XMLoadFloat3(pMouseRay);
//
//		FXMVECTOR	vPos0 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[0].vPosition), 1.f);
//		GXMVECTOR	vPos1 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[1].vPosition), 1.f);
//		HXMVECTOR	vPos2 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[2].vPosition), 1.f);
//
//		if (DirectX::TriangleTests::Intersects(vPivot, vMouseRay, vPos0, vPos1, vPos2, fDist))
//		{
//			XMStoreFloat3(pOut, vPivot + vMouseRay * fDist);
//			return true;
//		}
//	}
//	{
//		FXMVECTOR	vPivot = XMLoadFloat3(pPivot);
//		FXMVECTOR	vMouseRay = XMLoadFloat3(pMouseRay);
//
//		FXMVECTOR	vPos0 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[0].vPosition), 1.f);
//		GXMVECTOR	vPos2 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[2].vPosition), 1.f);
//		HXMVECTOR	vPos3 = XMVectorSetW(XMLoadFloat3(&((VTXTEX*)m_pVertices)[3].vPosition), 1.f);
//
//		if (DirectX::TriangleTests::Intersects(vPivot, vMouseRay, vPos0, vPos2, vPos3, fDist))
//		{
//			XMStoreFloat3(pOut, vPivot + vMouseRay * fDist);
//			return true;
//		}
//	}
//
//	return false;
//}

CVIBuffer_Pointbillboard * CVIBuffer_Pointbillboard::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CVIBuffer_Pointbillboard* pGameInstance = new CVIBuffer_Pointbillboard(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
	{
		MSGBOX("Failed to Created CVIBuffer_Pointbillboard");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CComponent * CVIBuffer_Pointbillboard::Clone(void * pArg)
{
	CVIBuffer_Pointbillboard* pGameInstance = new CVIBuffer_Pointbillboard(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CVIBuffer_Pointbillboard");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CVIBuffer_Pointbillboard::Free()
{
	__super::Free();
}
