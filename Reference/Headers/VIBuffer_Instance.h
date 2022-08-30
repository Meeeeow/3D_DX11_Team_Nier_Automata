#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	CVIBuffer_Instance(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() DEFAULT;

public:
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);
	virtual HRESULT			Render(_uint iPassIndex);

protected:
	_uint					m_iNumInstance = 10;
	ID3D11Buffer*			m_pVBInstance = nullptr;
	D3D11_BUFFER_DESC		m_VBInstanceDesc; /* �����ϰ����ϴ� ������������ ä���. (�޸��� ũ��, cpu, gpu�� �������� */
	D3D11_SUBRESOURCE_DATA	m_VBInstanceSubResourceData;
	_uint					m_iInstanceStride = 0;
	void*					m_pInstanceVertices = nullptr;

protected:
	HRESULT					Create_VertexInstanceBuffer();


public:
	virtual	CComponent*		Clone(void* pArg) PURE;
	virtual void			Free() override;
};

END
