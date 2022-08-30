#pragma once
#ifndef __ENGINE_VI_BUFFER_SPHERE_H__
#define __ENGINE_VI_BUFFER_SPHERE_H__

#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
private:
	CVIBuffer_Sphere(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
	virtual ~CVIBuffer_Sphere() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName, _uint iNumVertical, _uint iNumHorizontal, _float fRadius, _float4 vColor);
	virtual HRESULT						NativeConstruct(void* pArg) override;



private:
	_uint								m_iNumVertical = 0;
	_uint								m_iNumHorizontal = 0;
	_float								m_fRadius = 1.f;

public:
	static	CVIBuffer_Sphere*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		,const _tchar* pShaderFilePath, const char* pTechniqueName , _uint iNumVertical, _uint iNumHorizontal, _float fRadius = 1.f, _float4 vColor = _float4(0.f, 0.f, 0.f, 1.f));
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;

};
END
#endif // !__ENGINE_VI_BUFFER_SPHERE_H__