#pragma once
#ifndef __ENGINE_VI_BUFFER_TRIANGLE_LINE_STRIP_H__
#define __ENGINE_VI_BUFFER_TRIANGLE_LINE_STRIP_H__

#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_TriangleLineStrip final : public CVIBuffer
{
private:
	CVIBuffer_TriangleLineStrip(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_TriangleLineStrip(const CVIBuffer_TriangleLineStrip& rhs);
	virtual ~CVIBuffer_TriangleLineStrip() DEFAULT;
	
public:
	virtual	HRESULT								NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName, _float3* pPoints);
	virtual HRESULT								NativeConstruct(void* pArg) override;

public:
	static	CVIBuffer_TriangleLineStrip*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName, _float3* pPoints);
	virtual	CComponent*							Clone(void* pArg) override;
	virtual void								Free() override;


};
END
#endif // !__ENGINE_VI_BUFFER_TRIANGLE_LINE_STRIP_H__