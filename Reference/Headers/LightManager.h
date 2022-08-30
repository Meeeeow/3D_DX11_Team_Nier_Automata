#pragma once
#ifndef __ENGINE_LIGHT_MANAGER_H__
#define __ENGINE_LIGHT_MANAGER_H__

#include "Base.h"



BEGIN(Engine)
class CLightManager final : public CBase
{
	DECLARE_SINGLETON(CLightManager)
	
private:
	explicit CLightManager();
	virtual ~CLightManager() DEFAULT;

public:
	typedef list<class CLight*>											LIGHTS;
	typedef unordered_map<const _tchar*, list<class CLight*>>			LIGHTMAP;
	typedef unordered_map<const _tchar*, CLight*>						PROTOTYPELIGHT;

public:
	// Add
	HRESULT					Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pLightTag
		, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);
	HRESULT					Add_Light_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pLightTag
		, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc);

	// Prototype Pattern
	HRESULT					Add_Light_Prototype(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pPrototypeLightTag
		, const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT					Add_Light(const _tchar* pPrototypeLightTag, const _tchar* pLightTag, const LIGHTDESC& tLightDesc);

	_int					Tick(_double dTimeDelta);
	_int					LateTick(_double dTimeDelta);
	HRESULT				Render();

	// Shadow Trace 
	void					ResetPosition_Shadow_DirectionLight(const _tchar* pLightTag, _fvector vPosition, _uint iLightIndex);

	// Trace Function
	void					Trace_SpotLight(const _tchar* pLightTag, _fvector vTargetPosition);
	void					Trace_SpotLight_EyeToAt(const _tchar* pLightTag, _fvector vEye, _fvector vAt);

	// Get
	CLight*					Get_Light(const _tchar* pLightTag, _uint iIndex);
	CLight*					Get_Light(const _tchar* pPrototypeLightTag, const LIGHTDESC& tLightDesc);

	// For GraphicDebugging
	size_t					Get_LightMapSize();
	const wchar_t*			Get_LightTag(_uint iIndex);
	size_t					Get_LightSize(const _tchar* pLightTag);

	list<class CLight*>*	Find_LightMap(const _tchar* pLightTag);

	HRESULT					LightMap_Active(const _tchar* pLightTag);
	HRESULT					LightMap_DeActive(const _tchar* pLightTag);
	HRESULT					Delete_LightList(const _tchar* pLightTag);
	HRESULT					Delete_LightIndex(const _tchar* pLightTag, _uint iLightIndex);
	HRESULT					DeActive_All();


	// For Light Info
	LIGHTDESC*				Get_LightDesc(const _tchar* pLightTag, _uint iLightIndex);
	LIGHTDEPTHDESC*			Get_LightDepthDesc(const _tchar* pLightTag, _uint iLightIndex);
	

	void					Increase_LightColor(const _tchar* pLightTag, _float4 vIncreasePower, _float4 vMaxIncreasePower, _uint iColor);
	void					Decrease_LightColor(const _tchar* pLightTag, _float4 vDecreasePower, _float4 vMinIncreasePower, _uint iColor);
	void					Increase_InnerDegree(const _tchar* pLightTag, _float fIncreaseDegree, _float fMaxDegree);
	void					Decrease_InnerDegree(const _tchar* pLightTag, _float fDecreaseDegree);
	void					Increase_OuterDegree(const _tchar* pLightTag, _float fIncreaseDegree, _float fMaxDegree);
	void					Decrease_OuterDegree(const _tchar* pLightTag, _float fDecreaseDegree);

	void					Increase_Range(const _tchar* pLightTag, _float fIncreaseRange, _float fMaxRange);
	void					Decrease_Range(const _tchar* pLightTag, _float fDecreaseRange, _float fMinRange);

	HRESULT			Clear();

private:
	LIGHTMAP				m_mapLight;
	PROTOTYPELIGHT			m_mapPrototypeLight;
	LIGHTS					m_listDeadLights;
	bool						m_bSSAO = true;

public:
	void						SSAO(bool SSAO);
	const bool&			Get_SSAO();
public:
	virtual void				Free() override;

};

END
#endif // !__ENGINE_LIGHT_MANAGER_H__