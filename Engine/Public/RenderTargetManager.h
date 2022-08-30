#pragma once
#ifndef __ENGINE_RENDER_TARGET_MANAGER_H__
#define __ENGINE_RENDER_TARGET_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
class CRenderTargetManager final : public CBase
{
	DECLARE_SINGLETON(CRenderTargetManager)

private:
	explicit CRenderTargetManager();
	virtual ~CRenderTargetManager() DEFAULT;

public:
	typedef	unordered_map<const _tchar*, class CRenderTarget*>			RENDERTARGETS;
	typedef unordered_map<const _tchar*, list<class CRenderTarget*>>	MULTIRENDERTARGETS;

public:
	HRESULT						NativeConstruct(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	HRESULT						Add_RenderTarget(_pGraphicDevice pGrpahicDevice, _pContextDevice pContextDevice
		, const _tchar* pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex, _float4 vClearColor = _float4(0.f, 0.f, 0.f, 0.f));
	HRESULT						Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

public:
	class CRenderTarget*		Find_RenderTarget(const _tchar* pRenderTargetTag);
	list<class CRenderTarget*>*	Find_MRT(const _tchar* pMRTTag);
	ID3D11ShaderResourceView*	Get_SRV(const _tchar* pRenderTargetTag);

public:
	HRESULT						Set_Buffer(const _tchar* pRenderTargetTag, const _tchar* pShaderFilePath, const char* pTechniqueName, _float4 vScreenPos);
	void						IsRender(_bool bRender) { m_bRender = bRender; }
	
public:
	HRESULT						Begin_MRT(const _tchar* pMRTTag);
	HRESULT						Begin_ShadowRT(const _tchar* pMRTTag);
	HRESULT						End_MRT();

	HRESULT						Begin_DefaultRT();
	HRESULT						End_DefaultRT();

public:
	HRESULT						Render();

public:
	HRESULT						Initialize_ShadowDepthView(_uint iWidth, _uint iHeigth);

private:
	RENDERTARGETS				m_mapRenderTargets;
	MULTIRENDERTARGETS			m_mapMRTs;

private: /* Default Device */
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;
	ID3D11RenderTargetView*		m_pDefaultRTV = nullptr;
	ID3D11DepthStencilView*		m_pDefaultDSV = nullptr;

	ID3D11DepthStencilView*		m_pShadowDepthDSV = nullptr;

private:
	_bool						m_bRender = true;

public:
	virtual	void				Free() override;
};

END
#endif // !__ENGINE_RENDER_TARGET_MANAGER_H__