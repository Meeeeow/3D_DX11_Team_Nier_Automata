#pragma once
#ifndef __ENGINE_VI_BUFFER_RECT_H__
#define __ENGINE_VI_BUFFER_RECT_H__

#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT				NativeConstruct(void* pArg) override;

public:
	static	CVIBuffer_Rect*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual	CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;

};
END
#endif // !__ENGINE_VI_BUFFER_RECT_H__