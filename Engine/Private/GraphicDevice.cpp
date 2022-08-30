#include "..\Public\GraphicDevice.h"
#include "GraphicDebugger.h"

IMPLEMENT_SINGLETON(CGraphicDevice)

CGraphicDevice::CGraphicDevice()
	: m_pGraphicDevice(nullptr), m_pContextDevice(nullptr)
{
}


HRESULT CGraphicDevice::Initialize(DESC tGraphicDesc, _pGraphicDevice * ppGraphicDeviceOut, _pContextDevice * ppContextDeviceOut)
{
	_uint	iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG
	D3D_FEATURE_LEVEL		tFreatueLevel;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pGraphicDevice, &tFreatueLevel, &m_pContextDevice)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(tGraphicDesc.hWnd, tGraphicDesc.eWinMode, tGraphicDesc.iWinCX, tGraphicDesc.iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(tGraphicDesc.iWinCX, tGraphicDesc.iWinCY)))
		return E_FAIL;

	m_pContextDevice->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView);

	D3D11_VIEWPORT			tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));
	tViewportDesc.TopLeftX = 0;
	tViewportDesc.TopLeftY = 0;
	tViewportDesc.Width = (FLOAT)tGraphicDesc.iWinCX;
	tViewportDesc.Height = (FLOAT)tGraphicDesc.iWinCY;
	tViewportDesc.MinDepth = 0.f;
	tViewportDesc.MaxDepth = 1.f;

	m_pContextDevice->RSSetViewports(1, &tViewportDesc);

	*ppGraphicDeviceOut = m_pGraphicDevice;
	*ppContextDeviceOut = m_pContextDevice;

#ifdef _DEBUG
	NOTICEDBG("GraphicDevice Create");
#endif // _DEBUG

	return S_OK;
}

HRESULT CGraphicDevice::Clear_BackBuffer_View(XMFLOAT4 vClearColor)
{
	if (m_pContextDevice == nullptr)
		return E_FAIL;

	m_pContextDevice->ClearRenderTargetView(m_pBackBufferView, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphicDevice::Clear_DepthStencil_View()
{
	if (m_pContextDevice == nullptr)
		return E_FAIL;

	m_pContextDevice->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphicDevice::Present()
{
	if (m_pSwapChain == nullptr)
		return E_FAIL;

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphicDevice::Ready_SwapChain(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice*		pDevice = nullptr;
	m_pGraphicDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*		pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*		pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC	tSwapChainDesc;
	ZeroMemory(&tSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// SwapChain Size
	tSwapChainDesc.BufferDesc.Width = iWinCX;
	tSwapChainDesc.BufferDesc.Height = iWinCY;

	// SwapChain RefreshRate Count
	tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	tSwapChainDesc.SampleDesc.Quality = 0;
	tSwapChainDesc.SampleDesc.Count = 1;
	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.OutputWindow = hWnd;
	tSwapChainDesc.Windowed = (BOOL)eWinMode;
	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pGraphicDevice, &tSwapChainDesc, &m_pSwapChain)))
		FAILMSG("GraphicDevice Ready SwapChain - Create SwapChain");

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphicDevice::Ready_BackBufferRenderTargetView()
{
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	ID3D11Texture2D*		pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		FAILMSG("GraphicDevice Ready BackBuffer - GetBuffer");

	if (FAILED(m_pGraphicDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferView)))
		FAILMSG("GraphicDevice Ready BackBuffer - CreateRTV");

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphicDevice::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tTextureDesc.Width = iWinCX;
	tTextureDesc.Height = iWinCY;
	tTextureDesc.MipLevels = 1;
	tTextureDesc.ArraySize = 1;
	tTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Multi Sampling
	tTextureDesc.SampleDesc.Quality = 0;
	tTextureDesc.SampleDesc.Count = 1;

	tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tTextureDesc.CPUAccessFlags = 0;
	tTextureDesc.MiscFlags = 0;

	// Texture Desc
	// D3D11_SUBRESOURCE_DATA;

	if (FAILED(m_pGraphicDevice->CreateTexture2D(&tTextureDesc, nullptr, &pDepthStencilTexture)))
		FAILMSG("GraphicDevice Ready DepthStencil - CreateTexture");

	if (FAILED(m_pGraphicDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		FAILMSG("GraphicDevice Ready DepthStencil - CreateDSV");

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CGraphicDevice::Free()
{
	unsigned long dwRefCnt = 0;

	dwRefCnt = Safe_Release(m_pSwapChain);
	dwRefCnt = Safe_Release(m_pDepthStencilView);
	dwRefCnt = Safe_Release(m_pBackBufferView);
	dwRefCnt = Safe_Release(m_pContextDevice);
	dwRefCnt = Safe_Release(m_pGraphicDevice);
}
