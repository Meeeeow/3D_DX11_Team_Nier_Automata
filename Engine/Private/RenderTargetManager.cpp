#include "..\Public\RenderTargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CRenderTargetManager)

CRenderTargetManager::CRenderTargetManager()
{
}

HRESULT CRenderTargetManager::NativeConstruct(_pGraphicDevice pGraphicDevice ,_pContextDevice pContextDevice)
{
	if (pContextDevice == nullptr)
		return E_FAIL;

	m_pGraphicDevice = pGraphicDevice;
	Safe_AddRef(m_pGraphicDevice);
	m_pContextDevice = pContextDevice;
	Safe_AddRef(m_pContextDevice);

	return S_OK;
}

HRESULT CRenderTargetManager::Add_RenderTarget(_pGraphicDevice pGrpahicDevice, _pContextDevice pContextDevice, const _tchar * pRenderTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, _uint iRenderIndex, _float4 vClearColor)
{
	if (Find_RenderTarget(pRenderTargetTag) != nullptr)
		return S_OK;

	CRenderTarget*	pRenderTarget = CRenderTarget::Create(pGrpahicDevice, pContextDevice
		, iWidth, iHeight, ePixelFormat, iRenderIndex, vClearColor);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	m_mapRenderTargets.emplace(pRenderTargetTag, pRenderTarget);
	return S_OK;
}

HRESULT CRenderTargetManager::Add_MRT(const _tchar * pMRTTag, const _tchar * pRenderTargetTag)
{
	CRenderTarget*			pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	Safe_AddRef(pRenderTarget);
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (pMRTList == nullptr)
	{
		list<CRenderTarget*>	MRTList;
		MRTList.push_back(pRenderTarget);

		m_mapMRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	return S_OK;
}

CRenderTarget * CRenderTargetManager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto	iter = find_if(m_mapRenderTargets.begin(), m_mapRenderTargets.end(), CTagFinder(pRenderTargetTag));

	if (iter == m_mapRenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CRenderTargetManager::Find_MRT(const _tchar * pMRTTag)
{
	auto	iter = find_if(m_mapMRTs.begin(), m_mapMRTs.end(), CTagFinder(pMRTTag));

	if (iter == m_mapMRTs.end())
		return nullptr;

	return &iter->second;
}

ID3D11ShaderResourceView * CRenderTargetManager::Get_SRV(const _tchar * pRenderTargetTag)
{
	CRenderTarget* pInstance = Find_RenderTarget(pRenderTargetTag);
	if (pInstance == nullptr)
		return nullptr;

	return pInstance->Get_SRV();
}

HRESULT CRenderTargetManager::Set_Buffer(const _tchar * pRenderTargetTag, const _tchar * pShaderFilePath, const char * pTechniqueName, _float4 vScreenPos)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (pRenderTarget == nullptr)
		return E_FAIL;

	return pRenderTarget->Set_Buffer(pShaderFilePath, pTechniqueName, vScreenPos);
}

HRESULT CRenderTargetManager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	m_pContextDevice->OMGetRenderTargets(1, &m_pDefaultRTV, &m_pDefaultDSV);

	_uint iNumView = (_uint)pMRTList->size();

	ID3D11RenderTargetView*	pRTVs[8] = { nullptr };

	_uint iNumRTV = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRTV] = pRenderTarget->Get_RTV();

		if (pRTVs[iNumRTV] == nullptr)
			return E_FAIL;
		++iNumRTV;
	}

	m_pContextDevice->OMSetRenderTargets(iNumView, pRTVs, m_pDefaultDSV);
	

	return S_OK;
}

HRESULT CRenderTargetManager::Begin_ShadowRT(const _tchar * pMRTTag)
{
	list<CRenderTarget*>*	pMRTList = Find_MRT(pMRTTag);
	if (pMRTList == nullptr)
		return E_FAIL;

	m_pContextDevice->ClearDepthStencilView(m_pShadowDepthDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	m_pContextDevice->OMGetRenderTargets(1, &m_pDefaultRTV, &m_pDefaultDSV);

	_uint iNumView = (_uint)pMRTList->size();

	ID3D11RenderTargetView*	pRTVs[8] = { nullptr };
	_uint iNumRTV = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRTVs[iNumRTV] = pRenderTarget->Get_RTV();

		if (pRTVs[iNumRTV] == nullptr)
			return E_FAIL;
		++iNumRTV;
	}
	m_pContextDevice->OMSetRenderTargets(iNumView, pRTVs, m_pShadowDepthDSV);

	return S_OK;
}

HRESULT CRenderTargetManager::End_MRT()
{
	m_pContextDevice->OMSetRenderTargets(1, &m_pDefaultRTV, m_pDefaultDSV);

	Safe_Release(m_pDefaultDSV);
	Safe_Release(m_pDefaultRTV);

	return S_OK;
}

HRESULT CRenderTargetManager::Begin_DefaultRT()
{
	CRenderTarget*			pRenderTarget = Find_RenderTarget(L"RTV_Default");
	if (pRenderTarget == nullptr)
		return E_FAIL;

	_uint iNumRTV = 0;
	ID3D11RenderTargetView*	pRTVs[1] = { nullptr };

	pRenderTarget->Clear();
	pRTVs[iNumRTV] = pRenderTarget->Get_RTV();

	if (pRTVs[iNumRTV] == nullptr)
		return E_FAIL;


	m_pContextDevice->OMGetRenderTargets(1, &m_pDefaultRTV, &m_pDefaultDSV);

	m_pContextDevice->OMSetRenderTargets(1, pRTVs, m_pDefaultDSV);


	return S_OK;
}

HRESULT CRenderTargetManager::End_DefaultRT()
{
	m_pContextDevice->OMSetRenderTargets(1, &m_pDefaultRTV, m_pDefaultDSV);

	Safe_Release(m_pDefaultDSV);
	Safe_Release(m_pDefaultRTV);

	return S_OK;
}

HRESULT CRenderTargetManager::Render()
{
	if (!m_bRender)
		return S_OK;

	for (auto& Pair : m_mapMRTs)
	{
		for (auto& RenderTarget : Pair.second)
			RenderTarget->Render();
	}

	return S_OK;
}

HRESULT CRenderTargetManager::Initialize_ShadowDepthView(_uint iWidth, _uint iHeigth)
{
	if (m_pGraphicDevice == nullptr)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	tTextureDesc;
	ZeroMemory(&tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tTextureDesc.Width = iWidth;
	tTextureDesc.Height = iHeigth;
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

	D3D11_DEPTH_STENCIL_VIEW_DESC tDSVDesc;
	tDSVDesc.Flags = 0;
	tDSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	tDSVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pGraphicDevice->CreateDepthStencilView(pDepthStencilTexture, &tDSVDesc, &m_pShadowDepthDSV)))
		FAILMSG("GraphicDevice Ready DepthStencil - CreateDSV");

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CRenderTargetManager::Free()
{

	m_pContextDevice->ClearState();
	
	Safe_Release(m_pGraphicDevice);
	Safe_Release(m_pContextDevice);
	Safe_Release(m_pShadowDepthDSV);


	for (auto& Pair : m_mapMRTs)
	{
		for (auto& List : Pair.second)
			Safe_Release(List);
	}
	m_mapMRTs.clear();

	for (auto& Pair : m_mapRenderTargets)
 		Safe_Release(Pair.second);
	m_mapRenderTargets.clear();
}
