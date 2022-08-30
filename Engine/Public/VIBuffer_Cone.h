#pragma once
#ifndef __ENGINE_VI_BUFFER_CONE_H__
#define __ENGINE_VI_BUFFER_CONE_H__

#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Cone final : public CVIBuffer
{
private:
	CVIBuffer_Cone(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Cone(const CVIBuffer_Cone& rhs);
	virtual ~CVIBuffer_Cone() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iResoultion, _float fRadius, _float fHeight);
	virtual HRESULT					NativeConstruct(void* pArg);

public:
	static	CVIBuffer_Cone*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CVIBuffer_Cone*			Clone(void* pArg) override;
	virtual void					Free() override;

};

END
#endif // !__ENGINE_VI_BUFFER_CONE_H__