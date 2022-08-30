#pragma once
#ifndef __ENGINE_VI_BUFFER_CUBE_H__
#define __ENGINE_VI_BUFFER_CUBE_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Cube(const CVIBuffer_Cube& rhs);
	virtual ~CVIBuffer_Cube() DEFAULT;
	
public:
	virtual	HRESULT					NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT					NativeConstruct(void* pArg) override;

public:
	static	CVIBuffer_Cube*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual	CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;

};
END
#endif // !__ENGINE_VI_BUFFER_CUBE_H__