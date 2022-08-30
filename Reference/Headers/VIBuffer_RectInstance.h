#pragma once
#ifndef __ENGINE_VI_BUFFER_RECT_INSTANCE_H__
#define __ENGINE_VI_BUFFER_RECT_INSTANCE_H__


#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectInstance final : public CVIBuffer_Instance
{
private:
	explicit CVIBuffer_RectInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_RectInstance(const CVIBuffer_RectInstance& rhs);
	virtual ~CVIBuffer_RectInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void Update(_double TimeDelta);

private:
	_float*		m_pDropSpeed = nullptr;

public:
	static CVIBuffer_RectInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END

#endif // !__ENGINE_VI_BUFFER_RECT_INSTANCE_H__