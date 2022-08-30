#pragma once
#ifndef __ENGINE_VI_BUFFER_H__
#define __ENGINE_VI_BUFFER_H__

#include "Component.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() DEFAULT; 

public:
	typedef struct tagVIBufferPassDesc {
		ID3DX11EffectPass*			pEffectPass = nullptr;
		ID3D11InputLayout*			pInputLayout = nullptr;
	}PASSDESC;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	
public:
	HRESULT							SetUp_RawValue(const char* pConstantValueName, void* pData, _uint iByteLength);
	HRESULT							SetUp_Texture(const char* pConstantTextureName, ID3D11ShaderResourceView* pShaderResourceView);

public:
	HRESULT							Render(_uint iIndex = 0);

protected:
	HRESULT							Create_VertexBuffer();
	HRESULT							Create_IndexBuffer();
	HRESULT							Compile_Shader(D3D11_INPUT_ELEMENT_DESC* pElement, _uint iNumElements, const _tchar* pShaderFilePath, const char* pTechniqueName);

public:
	virtual _bool					Picking(_float3* pOut, _matrix matWorld, _float& fDist);

public:
	_uint							Get_MaxPassIndex() { return m_iPassMax; }
	const char*						Get_PassName(_uint iIndex) { return *(m_vecPassName.begin() + iIndex); }
	void*							Get_Vertices() { return m_pVertices; }
	void*							Get_Indices() { return m_pIndices; }
	_uint							Get_Primitive() { return m_iNumPrimitive; }
	const _uint&					Get_VerticesNum() const { 
		return m_iNumVertices;
	}
protected:
	_uint							m_iStride = 0;			//
	_uint							m_iNumVertices = 0;

	_uint							m_iNumPrimitive = 0;
	_uint							m_iNumIndicesPerPT = 0;
	_uint							m_iFaceIndexSize = 0;

	DXGI_FORMAT						m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY		m_eRenderType;


	ID3D11Buffer*					m_pVB = nullptr;
	D3D11_BUFFER_DESC				m_tVB_Desc;
	D3D11_SUBRESOURCE_DATA			m_tVB_SubResourceData;
	void*							m_pVertices = nullptr;

	ID3D11Buffer*					m_pIB = nullptr;
	D3D11_BUFFER_DESC				m_tIB_Desc;
	D3D11_SUBRESOURCE_DATA			m_tIB_SubResourceData;
	void*							m_pIndices = nullptr;

	ID3DX11Effect*					m_pEffect = nullptr;
	vector<PASSDESC*>				m_vecPassDesc;

	_uint							m_iPassMax = 0;
	vector<const char*>				m_vecPassName;

public:
	virtual	CComponent*				Clone(void* pArg) PURE;
	virtual void					Free() override;

};


END
#endif // !__ENGINE_VI_BUFFER_H__