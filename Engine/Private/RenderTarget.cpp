#include "..\Public\RenderTarget.h"
#include "VIBuffer_Viewport.h"

CRenderTarget::CRenderTarget(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CRenderTarget::NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex, _float4 vClearColor)
{
	D3D11_TEXTURE2D_DESC		tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tTextureDesc.Width = iWidth;
	tTextureDesc.Height = iHeight;
	tTextureDesc.MipLevels = 1;
	tTextureDesc.ArraySize = 1;
	tTextureDesc.Format = ePixelFormat;

	tTextureDesc.SampleDesc.Quality = 0;
	tTextureDesc.SampleDesc.Count = 1;

	tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	tTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tTextureDesc.CPUAccessFlags = 0;
	tTextureDesc.MiscFlags = 0;

	m_vClearColor = vClearColor;


	if (FAILED(m_pGraphicDevice->CreateTexture2D(&tTextureDesc, nullptr, &m_pTexture2D)))
		FAILMSG("RenderTarget Failed to Create Texture2D - NativeConstruct");

	D3D11_RENDER_TARGET_VIEW_DESC	tRTVDesc;
	ZeroMemory(&tRTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	tRTVDesc.Format = ePixelFormat;
	tRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	tRTVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pGraphicDevice->CreateRenderTargetView(m_pTexture2D, &tRTVDesc, &m_pRTV)))
		FAILMSG("RenderTarget Failed to Create RTV - NativeConstruct");

	D3D11_SHADER_RESOURCE_VIEW_DESC	tSRVDesc;
	ZeroMemory(&tSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	tSRVDesc.Format = ePixelFormat;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tSRVDesc.Texture2D.MipLevels = 1;

	m_iRenderIndex = iRenderIndex;

	if (FAILED(m_pGraphicDevice->CreateShaderResourceView(m_pTexture2D, &tSRVDesc, &m_pSRV)))
		FAILMSG("RenderTarget Failed to Create SRV - NativeConstruct");
	
	return S_OK;


	D3D11_BUFFER_DESC tBufferDesc;

	tBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDesc;

	tUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

}

void CRenderTarget::Clear()
{
	m_pContextDevice->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);
}

HRESULT CRenderTarget::Set_Buffer(const _tchar * pShaderFilePath, const char * pTechniqueName, _float4 vScreenPos)
{
	m_pVIBuffer = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice
		, pShaderFilePath, pTechniqueName, vScreenPos);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;
	return S_OK;
}

HRESULT CRenderTarget::Render()
{
	if (!m_pVIBuffer) return S_OK;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pSRV)))
		return E_FAIL;

	// m_iRenderIndex
	m_pVIBuffer->Render(0);

	return S_OK;
}

CRenderTarget * CRenderTarget::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex, _float4 vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iWidth, iHeight, ePixelFormat, iRenderIndex, vClearColor)))
		Safe_Release(pInstance);

	return pInstance;
}

void CRenderTarget::Free()
{
	m_pContextDevice->ClearState();

	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pTexture2D);
	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);
}
