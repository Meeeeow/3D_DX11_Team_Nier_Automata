#include "..\Public\VIBuffer.h"


CVIBuffer::CVIBuffer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs), m_pVB(rhs.m_pVB), m_iNumVertices(rhs.m_iNumVertices), m_iStride(rhs.m_iStride), m_pVertices(rhs.m_pVertices)
	, m_pIB(rhs.m_pIB), m_iFaceIndexSize(rhs.m_iFaceIndexSize), m_iNumPrimitive(rhs.m_iNumPrimitive), m_pIndices(rhs.m_pIndices)
	, m_eIndexFormat(rhs.m_eIndexFormat), m_eRenderType(rhs.m_eRenderType), m_pEffect(rhs.m_pEffect), m_vecPassDesc(rhs.m_vecPassDesc)
	, m_iNumIndicesPerPT(rhs.m_iNumIndicesPerPT), m_iPassMax(rhs.m_iPassMax), m_vecPassName(rhs.m_vecPassName)
{
	for (auto& pPassDesc : m_vecPassDesc)
		Safe_AddRef(pPassDesc->pInputLayout);

	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	
	return S_OK;
}

HRESULT CVIBuffer::SetUp_RawValue(const char * pConstantValueName, void * pData, _uint iByteLength)
{
	ID3DX11EffectVariable*						pVariable = m_pEffect->GetVariableByName(pConstantValueName);
	if (pVariable == nullptr)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iByteLength);
}

HRESULT CVIBuffer::SetUp_Texture(const char * pConstantTextureName, ID3D11ShaderResourceView * pShaderResourceView)
{
	ID3DX11EffectShaderResourceVariable*		pVariable = m_pEffect->GetVariableByName(pConstantTextureName)->AsShaderResource();
	if (pVariable == nullptr)
		return E_FAIL;

	return pVariable->SetResource(pShaderResourceView);
}

HRESULT CVIBuffer::Render(_uint iIndex)
{
	if (m_vecPassDesc.size() <= iIndex)
		FAILMSG("VIBuffer Render - Index is too big");


	_uint	iOffSet = 0;

	m_pContextDevice->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffSet);
	m_pContextDevice->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContextDevice->IASetInputLayout(m_vecPassDesc[iIndex]->pInputLayout);
	m_pContextDevice->IASetPrimitiveTopology(m_eRenderType);

	m_vecPassDesc[iIndex]->pEffectPass->Apply(0, m_pContextDevice);

	m_pContextDevice->DrawIndexed(m_iNumPrimitive * m_iNumIndicesPerPT, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (FAILED(m_pGraphicDevice->CreateBuffer(&m_tVB_Desc, &m_tVB_SubResourceData, &m_pVB)))
		FAILMSG("VIBuffer Failed Create VertexBuffer");


	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (FAILED(m_pGraphicDevice->CreateBuffer(&m_tIB_Desc, &m_tIB_SubResourceData, &m_pIB)))
		FAILMSG("VIBuffer Failed Create IndexBuffer");


	return S_OK;
}

HRESULT CVIBuffer::Compile_Shader(D3D11_INPUT_ELEMENT_DESC * pElement, _uint iNumElements, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif 
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pGraphicDevice, &m_pEffect, nullptr)))
	{
		FAILMSG("VIBuffer Compile_Shader - D3DX11CompileEffectFromFile");
	}

	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByName(pTechniqueName);
	if (pTechnique == nullptr)
		FAILMSG("VIBuffer Compile_Shader - GetTechniqueByName is Failed");

	D3DX11_TECHNIQUE_DESC	tTechniqueDesc;
	ZeroMemory(&tTechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	if (FAILED(pTechnique->GetDesc(&tTechniqueDesc)))
		FAILMSG("VIBuffer Compile_Shader - GetDesc");

	m_vecPassDesc.reserve(tTechniqueDesc.Passes);
	m_iPassMax = tTechniqueDesc.Passes;
	for (_uint i = 0; i < tTechniqueDesc.Passes; ++i)
	{
		PASSDESC*	pPassDesc = new PASSDESC;
		ZeroMemory(pPassDesc, sizeof(PASSDESC));

		pPassDesc->pEffectPass = pTechnique->GetPassByIndex(i);
		if (pPassDesc->pEffectPass == nullptr)
			FAILMSG("VIBuffer Compile_Shader - EffectPass is nullptr");

		D3DX11_PASS_DESC	tLayOutDesc;
		ZeroMemory(&tLayOutDesc, sizeof(D3DX11_PASS_DESC));

		pPassDesc->pEffectPass->GetDesc(&tLayOutDesc);

		if (FAILED(m_pGraphicDevice->CreateInputLayout(pElement, iNumElements
			, tLayOutDesc.pIAInputSignature, tLayOutDesc.IAInputSignatureSize, &pPassDesc->pInputLayout)))
			FAILMSG("VIBuffer Compile_Shader - CreateInputLayout");

		m_vecPassName.push_back(tLayOutDesc.Name);
		m_vecPassDesc.emplace_back(pPassDesc);
	}

	return S_OK;
}

_bool CVIBuffer::Picking(_float3* pOut, _matrix matWorld, _float& fDist)
{
	return false;
}



void CVIBuffer::Free()
{
	unsigned long dwRefCnt = 0;
	__super::Free();

	dwRefCnt = Safe_Release(m_pEffect);

	for (auto& pPassDesc : m_vecPassDesc)
		dwRefCnt = Safe_Release(pPassDesc->pInputLayout);

	if (m_bCloned == false)
	{
		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pIndices);

		for (auto& pPassDesc : m_vecPassDesc)
		{
			Safe_Delete(pPassDesc);
		}
	}

	m_vecPassDesc.clear();
	dwRefCnt = Safe_Release(m_pIB);
	dwRefCnt = Safe_Release(m_pVB);

}
