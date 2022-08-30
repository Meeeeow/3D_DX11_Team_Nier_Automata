#pragma once
#ifndef __ENGINE_GRAPHIC_DEVICE_H__
#define __ENGINE_GRAPHIC_DEVICE_H__

#include "Base.h"

BEGIN(Engine)

class CGraphicDevice final : public CBase
{
	DECLARE_SINGLETON(CGraphicDevice)
private:
	explicit CGraphicDevice();
	virtual ~CGraphicDevice() DEFAULT;

public:
	enum class WINMODE : _ulong {
		FULL = 0,
		WIN = 1
	};

public:
	typedef struct tagGraphicDeviceDesc {
		tagGraphicDeviceDesc() {
			hWnd = nullptr;
			iWinCX = 0; iWinCY = 0;
			eWinMode = CGraphicDevice::WINMODE::WIN;
		}
		HWND			hWnd;
		unsigned int	iWinCX, iWinCY;
		WINMODE			eWinMode;
	}DESC;

public:
	HRESULT							Initialize(DESC tGraphicDesc, _pGraphicDevice* ppGraphicDeviceOut, _pContextDevice* ppContextDeviceOut);
	HRESULT							Clear_BackBuffer_View(XMFLOAT4 vClearColor);
	HRESULT							Clear_DepthStencil_View();

	HRESULT							Present();

private:
	HRESULT							Ready_SwapChain(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY);
	HRESULT							Ready_BackBufferRenderTargetView();
	HRESULT							Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

private:
	_pGraphicDevice					m_pGraphicDevice		= nullptr;
	_pContextDevice					m_pContextDevice		= nullptr;

	IDXGISwapChain*					m_pSwapChain			= nullptr;
	ID3D11RenderTargetView*			m_pBackBufferView		= nullptr;
	ID3D11DepthStencilView*			m_pDepthStencilView		= nullptr;

public:
	_pGraphicDevice					Get_GraphicDevice() { return m_pGraphicDevice; }
	_pContextDevice					Get_ContextDevice() { return m_pContextDevice; }

public:
	virtual void					Free() override;
};

END
#endif // !__ENGINE_GRAPHIC_DEVICE_H__