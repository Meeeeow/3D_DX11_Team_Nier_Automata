#pragma once

#include "VIBuffer_Instance.h"
#include "Engine_Struct.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_PointInstance final : public CVIBuffer_Instance
{
//public:
//	typedef struct PointInstanceDescription
//	{
//		_float3		vDirection;
//		_float		fDensity;
//		_float		fLifeTime;
//		_int		iLifeTimeDensity;
//		_float		fDirectionDensity;
//		_int		iNumber;
//		_float		fSpeed;
//		_float		fSpeedDensity;
//		_int		iWidth;
//		_int		iHeight;
//		_float		fSpriteInterval;
//	} POINTINSTANCEDESC;
//
//private:
//	POINTINSTANCEDESC m_Desc;

private:
	POINTINST_DESC	m_tDesc;

private:
	HRESULT Init_Buffer();


private:
	explicit CVIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVIBuffer_PointInstance(const CVIBuffer_PointInstance& rhs);
	virtual ~CVIBuffer_PointInstance() = default;

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:
	void Update(_double TimeDelta);
	void Init();
	void Set_Desc(const POINTINST_DESC& _tDesc);
	void Update_ByDesc();



private:
	_float3*	m_pDirection			= nullptr;
	_float3*	m_pPosition				= nullptr;
	_float*		m_pLifeTime				= nullptr;
	_float*		m_pSpeed				= nullptr;
	_int*		m_pCurrentIndex			= nullptr;
	_float*		m_pSpriteElapsedTime	= nullptr;

private:
	_float*	m_pSavedLifeTime			= nullptr;

public:
	static CVIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END