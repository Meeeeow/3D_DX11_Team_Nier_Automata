#pragma once
#ifndef __ENGINE_LIGHT_H__
#define __ENGINE_LIGHT_H__

#include "Base.h"

BEGIN(Engine)
class CLight final : public CBase
{
private:
	CLight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CLight(const CLight& rhs);
	virtual ~CLight() DEFAULT;

public:
	enum class COLOR : _uint {
		DIFFUSE			= 0,
		AMBIENT		= 1,
		SPECULAR		= 2,
		ALL				= 3
	};


public:
	HRESULT							NativeConstruct(const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);
	HRESULT							NativeConstruct_ForTool(const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);
	HRESULT							NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT							NativeConstruct(const LIGHTDESC& tLightDesc);

	_int									Tick(_double dTimeDelta);
	_int									LateTick(_double dTimeDelta);
	HRESULT							Render();

public:
	LIGHTDESC*						Get_LightDesc() { return &m_tDesc; }
	const _bool&						Get_LightState() { return m_tDesc.bLightOn; }
	LIGHTDEPTHDESC*				Get_DepthDesc() { return &m_tLightDepthDesc; }

public:
	void									Synchronize(const LIGHTDEPTHDESC& tDepthDesc);
	void									Synchronize_Tracing();

public:
	// Moveable Light
	void									ResetPosition_Shadow_DirectionLight(_fvector vTargetPosition);
	void									ResetPosition_PointLight(_fvector vTargetPosition);

	void									ResetPosition_SpotLight(_fvector vEye);
	void									Trace_SpotLight(_fvector vTargetPosition);

	void									Calcultae_SpotLength();

public:
	void									Change_LightColor(COLOR eColor, _float4 vColor);

	void									Increase_LightColor(_float4 vIncreasePower, _float4 vMaxIncreasePower, COLOR eColor);
	void									Decrease_LightColor(_float4 vDecreasePower, _float4 vMinIncreasePower, COLOR eColor);

	// For Point Light Range
	void									Increase_Range(_float fIncreaseRange, _float fMaxRange);
	void									Decrease_Range(_float fDecreaseRange, _float fMinRange);

	// For Spot Light Degree
	void									Increase_InnerDegree(_float fIncreaseDegree, _float fMaxDegree);
	void									Decrease_InnerDegree(_float fDecreaseDegree);
	void									Increase_OuterDegree(_float fIncreaseDegree, _float fMaxDegree);
	void									Decrease_OuterDegree(_float fDecreaseDegree);

public:
	void									Active() { m_tDesc.bLightOn = true; }
	void									DeActive() { m_tDesc.bLightOn = false; }

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;

	LIGHTDESC							m_tDesc;
	LIGHTDEPTHDESC				m_tLightDepthDesc;

	class CVIBuffer_Viewport*		m_pVIBuffer = nullptr;
	_uint									m_iPassIndex = 0;


public:
	static	CLight*					Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);
	static	CLight*					Create_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);
	static	CLight*					Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	virtual CLight*					Clone(const LIGHTDESC& pArg);
	virtual void					Free() override;

};
END
#endif // !__ENGINE_LIGHT_H__