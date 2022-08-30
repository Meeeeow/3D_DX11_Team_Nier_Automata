#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail :
	public CVIBuffer
{
private:
	explicit CVIBuffer_Trail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT						NativeConstruct(void* pArg) override;

private:
	HRESULT								Init_Buffer();


private:
	_uint								m_iNumVerticesX = 0;
	const _uint							m_iNumVerticesZ = 2;

public:
	void								Add_LastPosition(_float3 _vMinPoint, _float3 _vMaxPoint);
	void								Init_AllPosition(_float3 _vMinPoint, _float3 _vMaxPoint);

public:
	static	CVIBuffer_Trail*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

END