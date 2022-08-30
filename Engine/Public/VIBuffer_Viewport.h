#pragma once
#ifndef __ENGINE_VI_BUFFER_VIEWPORT_H__
#define __ENGINE_VI_BUFFER_VIEWPORT_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Viewport final : public CVIBuffer
{
private:
	CVIBuffer_Viewport(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Viewport(const CVIBuffer_Viewport& rhs);
	virtual ~CVIBuffer_Viewport() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName, _float4 vScreenInfo);
	virtual HRESULT					NativeConstruct(void* pArg) override;

public:
	static	CVIBuffer_Viewport*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName, _float4 vScreenInfo);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;

};
END

#endif // !__ENGINE_VI_BUFFER_VIEWPORT_H__