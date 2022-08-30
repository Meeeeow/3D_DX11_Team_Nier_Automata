#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Pointbillboard final : public CVIBuffer
{
//public:
//	typedef struct PointBillboardDescription
//	{
//		_float fSize;
//	} POINTBILLBOARDDESC;
//private:
//	POINTBILLBOARDDESC m_Desc;
private:
	CVIBuffer_Pointbillboard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_Pointbillboard(const CVIBuffer_Pointbillboard& rhs);
	virtual ~CVIBuffer_Pointbillboard() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg);

	virtual HRESULT Render(_uint iPassIndex);

public:
	void Update(_double TimeDelta);


//public:
//	virtual _bool Picking(const _float3* pMouseRay, const _float3* pPivot, _float3* pOut, _uint* pIndex = nullptr) override;

public:
	static CVIBuffer_Pointbillboard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END