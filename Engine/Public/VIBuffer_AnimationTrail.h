#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_AnimationTrail :
	public CVIBuffer
{
private:
	explicit CVIBuffer_AnimationTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CVIBuffer_AnimationTrail(const CVIBuffer_AnimationTrail& rhs);
	virtual ~CVIBuffer_AnimationTrail() DEFAULT;

public:
	virtual	HRESULT						NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT						NativeConstruct(void* pArg) override;

private:
	HRESULT								Init_Buffer();


private:
	_uint								m_iNumVerticesX = 0;
	const _uint							m_iNumVerticesZ = 2;
	_uint								m_iNumRawPosition = 0;
	_uint								m_iNumRawXPosition = 0;

private:
	_float3*							m_arrRawPosition = nullptr;

public:
	void								Add_LastPosition(_float3 _vMinPoint, _float3 _vMaxPoint);
	void								Add_LastPosition(_vector _vMinPoint, _vector _vMaxPoint);
	void								Init_AllPosition(_float3 _vMinPoint, _float3 _vMaxPoint);
	void								Init_AllPosition(_vector _vMinPoint, _vector _vMaxPoint);
	void								Calculate_Catmullrom();

public:
	static	CVIBuffer_AnimationTrail*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

END