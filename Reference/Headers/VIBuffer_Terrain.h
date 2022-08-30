#pragma once
#ifndef __ENGINE_VI_BUFFER_TERRAIN_H__
#define __ENGINE_VI_BUFFER_TERRAIN_H__

#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	CVIBuffer_Terrain(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() DEFAULT;

public:
	virtual HRESULT					NativeConstruct_Prototype(const _tchar* pHeightMapFilePath, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT					NativeConstruct_Prototype(_float4 vTerrainInfo, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT					NativeConstruct(void* pArg) override;

	virtual _bool					Picking(_float3* pOut, _matrix matWorld, _float& fDist) override;

public:
	_float4							Compute_Height(_fvector vPosition);
	_float4							Compute_Height(_fvector vPosition, _fmatrix matTerrainMatrix);

private:
	_uint							m_iNumVerticesX = 0;
	_uint							m_iNumVerticesZ = 0;

public:
	static	CVIBuffer_Terrain*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pHeightMapFilePath, const _tchar* pShaderFilePath, const char* pTechniqueName);
	static	CVIBuffer_Terrain*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, _float4 vTerrainInfo, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual	CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};
END
#endif // !__ENGINE_VI_BUFFER_TERRAIN_H__