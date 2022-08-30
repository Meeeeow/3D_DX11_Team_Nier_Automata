#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Texture.h"
#include "RenderTargetManager.h"
#include "LightManager.h"
#include "VIBuffer_Viewport.h"
#include "Collider.h"
#include "Navigation.h"
#include "GraphicDebugger.h"
#include "RenderTarget.h"
#include "GameInstance.h"
#include "PipeLine.h"

CRenderer::CRenderer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CComponent(pGraphicDevice, pContextDevice)
{
}

HRESULT CRenderer::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	if (FAILED(Ready_Default_RenderTarget()))
		return E_FAIL;

	if (FAILED(Ready_Deferred_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_SSAO_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_SSAO_GaussianBlur_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_Shadow_RenderTarget()))
		return E_FAIL;

	if (FAILED(Ready_Light_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_DeferredResult_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_AfterDeferred_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_OutLine_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_RimDodge_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_Dodge_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_PostProcess0_Default_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_PostProcess0_MotionBlur_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_PostProcess0_DownSampling_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_PostProcess0_GaussianBlur_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_PostProcess0_DepthOfField_RenderTargets()))
		return E_FAIL;
	if (FAILED(Ready_PostPrecess2_Distortion_RenderTargets()))
		return E_FAIL;

	if (FAILED(Ready_GlowHorizontal_RenderTarget()))
		return E_FAIL;
	if (FAILED(Ready_GlowVertical_RenderTarget()))
		return E_FAIL;
	if (FAILED(Ready_MotionBlur_RenderTarget()))
		return E_FAIL;
	


	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	_float4 vScreenPos = _float4(0.f, 0.f, tViewportDesc.Width, tViewportDesc.Height);

	m_pVIBuffer = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, vScreenPos);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	//m_pVIBuffer_AfterDeferred = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, vScreenPos);
	//if (m_pVIBuffer_AfterDeferred == nullptr)
	//	return E_FAIL;

	//m_pVIBuffer_PostProcessing = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, vScreenPos);
	//if (m_pVIBuffer_PostProcessing == nullptr)
	//	return E_FAIL;


	//////////////////////////////////////////////////////////////////
	// Deferred
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Diffuse"), pShaderFilePath, pTechniqueName, _float4(0.f, 0.f, 100.f, 100.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Normal"), pShaderFilePath, pTechniqueName, _float4(0.f, 100.f, 100.f, 100.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Depth"), pShaderFilePath, pTechniqueName, _float4(0.f, 200.f, 100.f, 100.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_RimDodge"), pShaderFilePath, pTechniqueName, _float4(0.f, 300.f, 100.f, 100.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Dodge"), pShaderFilePath, pTechniqueName, _float4(0.f, 400.f, 100.f, 100.f))))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// Light Acc
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Shade"), pShaderFilePath, pTechniqueName, _float4(100.f, 0.f, 100.f, 100.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_Specular"), pShaderFilePath, pTechniqueName, _float4(100.f, 100.f, 100.f, 100.f))))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// Shadow
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_ShadowDepth"), pShaderFilePath, pTechniqueName, _float4(100, 200.f, 100.f, 100.f))))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// SSAO
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_SSAO"), pShaderFilePath, pTechniqueName, _float4(300.f, 0.f, 200.f, 200.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_SSAO_GaussianBlur_X"), pShaderFilePath, pTechniqueName, _float4(300.f, 200.f, 200.f, 200.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_SSAO_GaussianBlur"), pShaderFilePath, pTechniqueName, _float4(300.f, 400.f, 200.f, 200.f))))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////
	
	

	//////////////////////////////////////////////////////////////////
	// Down Sampling
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_PostProcess0_DownSampling"), pShaderFilePath, pTechniqueName, _float4(500.f, 0.f, 200.f, 200.f))))
		return E_FAIL;
	// Gaussian Blur
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_PostProcess0_GaussianBlur_X"), pShaderFilePath, pTechniqueName, _float4(500.f, 200.f, 200.f, 200.f))))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_PostProcess0_GaussianBlur"), pShaderFilePath, pTechniqueName, _float4(500.f, 400.f, 200.f, 200.f))))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////
//	if (FAILED(CRenderTargetManager::Get_Instance()->Set_Buffer(TEXT("RTV_PostProcess2_Distortion"), pShaderFilePath, pTechniqueName, _float4(300.f, 0.f, 100.f, 100.f))))
//		return E_FAIL;


	m_pDistortionTexture = CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/PostProcessing/Distortion_AfterUI_%d.png", (_uint)DISTORTIONTYPE::COUNT);
	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject * pGameObject)
{
	if (pGameObject == nullptr || eGroup >= RENDERGROUP::COUNT)
		return E_FAIL;

	if (eGroup == RENDERGROUP::MOTIONBLUR)
		return S_OK;

	m_listRenderObjects[(_uint)eGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Add_DebugGroup(DEBUGGROUP eGroup, CComponent * pComponent)
{
	if (pComponent == nullptr || eGroup >= DEBUGGROUP::COUNT)
		return E_FAIL;

	if (CGameInstance::Get_Instance()->Get_DebugDraw() == false)
	{
		return S_OK;
	}

	if (m_iDebugCnt == 0)
		return S_OK;
	else if (m_iDebugCnt == 1)
	{
		if (eGroup == DEBUGGROUP::NAVIGATION)
			return S_OK;
	}
	else if (m_iDebugCnt == 2)
	{
		if (eGroup == DEBUGGROUP::COLLIDER)
			return S_OK;
	}


	m_listRenderComponents[(_uint)eGroup].push_back(pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Ready_Default_RenderTarget()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Deffered
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Default", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Default", L"RTV_Default")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Deferred_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Deffered
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Diffuse", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
	//	, L"RTV_Normal", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
	//	, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 1.f))))
	//	return E_FAIL;

	//if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
	//	, L"RTV_Depth", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
	//	, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 1.f))))
	//	return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Normal", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Depth", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Glow", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_RimLight", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Deffered", L"RTV_Diffuse")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Deffered", L"RTV_Normal")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Deffered", L"RTV_Depth")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Deffered", L"RTV_Glow")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Deffered", L"RTV_RimLight")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_SSAO_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Deffered
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_SSAO", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_SSAO", L"RTV_SSAO")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_SSAO_GaussianBlur_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_SSAO_GaussianBlur_X", (_uint)(tViewportDesc.Width), (_uint)(tViewportDesc.Height)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_SSAO_GaussianBlur_X", L"RTV_SSAO_GaussianBlur_X")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_SSAO_GaussianBlur", (_uint)(tViewportDesc.Width), (_uint)(tViewportDesc.Height)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_SSAO_GaussianBlur", L"RTV_SSAO_GaussianBlur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_OutLine_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	m_iViewportX = (_uint)tViewportDesc.Width;
	m_iViewportY = (_uint)tViewportDesc.Height;

	// Deffered
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_OutLine", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_OutLine", L"RTV_OutLine")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Shadow_RenderTarget()
{
	_float fShadowWidth = SHADOWMAP_WIDTH, fShadowHeight = SHADOWMAP_HEIGHT;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_ShadowDepth", (_uint)fShadowWidth, (_uint)fShadowHeight
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Initialize_ShadowDepthView((_uint)fShadowWidth, (_uint)fShadowHeight)))
		return E_FAIL;

	// Shadow
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_ShadowDepth", L"RTV_ShadowDepth")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_Light_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Light
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Shade", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Specular", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_LightAcc", L"RTV_Shade")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_LightAcc", L"RTV_Specular")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_AfterDeferred_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// AfterDeferred
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_AfterDeferred_Default", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_AfterDeferred_Glow", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_AfterDeferred_Distortion", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_AfterDeferred_Depth", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_AfterDeferred", L"RTV_AfterDeferred_Default")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_AfterDeferred", L"RTV_AfterDeferred_Glow")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_AfterDeferred", L"RTV_AfterDeferred_Distortion")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_AfterDeferred", L"RTV_AfterDeferred_Depth")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_GlowHorizontal_RenderTarget()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// AfterDeferred
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Glow_Horizontal", (_uint)(tViewportDesc.Width), (_uint)(tViewportDesc.Height)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Glow_Horizontal_9", (_uint)(tViewportDesc.Width), (_uint)(tViewportDesc.Height)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Glow_Horizontal", L"RTV_Glow_Horizontal")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Glow_Horizontal", L"RTV_Glow_Horizontal_9")))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_GlowVertical_RenderTarget()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// AfterDeferred
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Glow_Vertical", (_uint)(tViewportDesc.Width /** 0.5f*/), (_uint)(tViewportDesc.Height /** 0.5f*/)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Glow_Vertical", (_uint)(tViewportDesc.Width /** 0.5f*/), (_uint)(tViewportDesc.Height /** 0.5f*/)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Glow_Vertical", L"RTV_Glow_Vertical")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Glow_Vertical_9", L"RTV_Glow_Vertical")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_MotionBlur_RenderTarget()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Deffered
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_MotionBlur", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_MotionBlur", L"RTV_MotionBlur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_DeferredResult_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Deferred Result
	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_DeferredResult", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_DeferredResult_Glow", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1)))
		return E_FAIL;


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_DeferredResult", L"RTV_DeferredResult")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_DeferredResult", L"RTV_DeferredResult_Glow")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostProcess0_Default_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_Default", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_Default", L"RTV_PostProcess0_Default")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostProcess0_DownSampling_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_DownSampling", (_uint)(tViewportDesc.Width / 4.f), (_uint)(tViewportDesc.Height / 4.f)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0)))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_DownSampling", L"RTV_PostProcess0_DownSampling")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostProcess0_GaussianBlur_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_GaussianBlur_X", (_uint)(tViewportDesc.Width / 4.f), (_uint)(tViewportDesc.Height / 4.f)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_GaussianBlur_X", L"RTV_PostProcess0_GaussianBlur_X")))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_GaussianBlur", (_uint)(tViewportDesc.Width / 4.f), (_uint)(tViewportDesc.Height / 4.f)
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_GaussianBlur", L"RTV_PostProcess0_GaussianBlur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostProcess0_DepthOfField_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_DOF", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_DOF", L"RTV_PostProcess0_DOF")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostProcess0_MotionBlur_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess0_MotionBlur", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostProcess0_MotionBlur", L"RTV_PostProcess0_MotionBlur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_PostPrecess2_Distortion_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_PostProcess2_Distortion", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 1, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_PostPorcess2_Distortion", L"RTV_PostProcess2_Distortion")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Ready_RimDodge_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_RimDodge", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_RimDodge", L"RTV_RimDodge")))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Ready_Dodge_RenderTargets()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);


	if (FAILED(CRenderTargetManager::Get_Instance()->Add_RenderTarget(m_pGraphicDevice, m_pContextDevice
		, L"RTV_Dodge", (_uint)tViewportDesc.Width, (_uint)tViewportDesc.Height
		, DXGI_FORMAT_R16G16B16A16_UNORM, 0, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Add_MRT(L"MRT_Dodge", L"RTV_Dodge")))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	//if (FAILED(CRenderTargetManager::Get_Instance()->Begin_DefaultRT()))
	//	return E_FAIL;
	//if (FAILED(Sequence_Priority()))
	//	return E_FAIL;
	//if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
	//	return E_FAIL;
	D3D11_VIEWPORT tViewport;
	_uint iNum = 1;
	m_pContextDevice->RSGetViewports(&iNum, &tViewport);
	m_vScreenSize = _float2(tViewport.Width, tViewport.Height);

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_Default")))
		return E_FAIL;
	if (FAILED(Sequence_Priority()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_ShadowRT(L"MRT_ShadowDepth")))
		return E_FAIL;
	if (FAILED(Sequence_ShadowDepth()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;


	if(FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_Deffered")))
		return E_FAIL;
	if (FAILED(Sequence_AlphaTest()))
		return E_FAIL;
	if(FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_SSAO")))
		return E_FAIL;
	if (FAILED(Sequence_SSAO()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(Sequence_SSAO_GaussianBlur()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_MotionBlur")))
		return E_FAIL;
	if (FAILED(Sequence_MotionBlur()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_RimDodge")))
		return E_FAIL;
	if (FAILED(Sequence_RimDodge()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_Dodge")))
		return E_FAIL;
	if (FAILED(Sequence_Dodge()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_LightAcc")))
		return E_FAIL;
	if (FAILED(Sequence_LightAcc()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_DeferredResult")))
		return E_FAIL;
	if (FAILED(Sequence_Result()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_AfterDeferred")))
		return E_FAIL;
	if (FAILED(Sequence_NonLight()))
		return E_FAIL;
	if (FAILED(Sequence_AlphaBlend()))
		return E_FAIL;
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_Glow_Horizontal")))
		return E_FAIL;
	//if (FAILED(Sequence_Glow_Horizontal_9()))
	//	return E_FAIL;
	//if (FAILED(Sequence_Glow_Horizontal_19()))
	//	return E_FAIL;
	if (FAILED(Sequence_Glow_Horizontal_41()))
		return E_FAIL;	
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;


	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_Glow_Vertical")))
		return E_FAIL;
	//if (FAILED(Sequence_Glow_Vertical_9()))
	//	return E_FAIL;
	//if (FAILED(Sequence_Glow_Vertical_19()))
	//	return E_FAIL;
	if (FAILED(Sequence_Glow_Vertical_41()))
		return E_FAIL;
	
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	


	if (FAILED(Sequence_PostProcessing_BeforeUI()))
		return E_FAIL;

	if (FAILED(Sequence_UI()))
		return E_FAIL;

	if (m_bRenderTarget)
	{
		if (FAILED(CRenderTargetManager::Get_Instance()->Render()))
			return E_FAIL;
	}

	//Render_Collider(); //Test


	return S_OK;
}

HRESULT CRenderer::Render(RENDERGROUP eRenderSequence)
{
	for (auto& pGameObject : m_listRenderObjects[(_uint)eRenderSequence])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)eRenderSequence].clear();

	return S_OK;
}

HRESULT CRenderer::Sequence_Priority()
{
	return Render(RENDERGROUP::PRIORITY);
}

HRESULT CRenderer::Sequence_AlphaTest()
{
	D3D11_VIEWPORT tViewport;
	ZeroMemory(&tViewport, sizeof(D3D11_VIEWPORT));
	tViewport.MinDepth = 0.f;
	tViewport.MaxDepth = 1.f;
	tViewport.Width = m_vScreenSize.x;
	tViewport.Height = m_vScreenSize.y;

	m_pContextDevice->RSSetViewports(1, &tViewport);

	return Render(RENDERGROUP::ALPHATEST);
}

HRESULT CRenderer::Sequence_SSAO()
{
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texNormal", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Normal"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDepth", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Depth"))))
		return E_FAIL;

	_float fFar = CPipeLine::Get_Instance()->Get_Far();
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::SSAO);

	return S_OK;
}

HRESULT CRenderer::Sequence_SSAO_GaussianBlur()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_SSAO_GaussianBlur_X")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_SSAO"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenWidth", &tViewportDesc.Width, sizeof(float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_GAUSSIANBLUR_X);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_SSAO_GaussianBlur")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_SSAO_GaussianBlur_X"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenHeight", &tViewportDesc.Height, sizeof(float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_GAUSSIANBLUR);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Sequence_ShadowDepth()
{
	D3D11_VIEWPORT tViewport;
	ZeroMemory(&tViewport, sizeof(D3D11_VIEWPORT));
	tViewport.MinDepth = 0.f;
	tViewport.MaxDepth = 1.f;
	tViewport.Width = SHADOWMAP_WIDTH;
	tViewport.Height = SHADOWMAP_HEIGHT;
	
	m_pContextDevice->RSSetViewports(1, &tViewport);

	for (auto& pGameObject : m_listRenderObjects[(_uint)RENDERGROUP::SHADOWDEPTH])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_ShadowDepth()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)RENDERGROUP::SHADOWDEPTH].clear();



	return S_OK;
}

HRESULT CRenderer::Sequence_LightAcc()
{
	CLightManager* pLight = CLightManager::Get_Instance();
	pLight->SSAO(m_bSSAO);
	pLight->Render();
	return S_OK;
}

HRESULT CRenderer::Sequence_AlphaBlend()
{
	m_listRenderObjects[(_uint)RENDERGROUP::ALPHABLEND].sort([](CGameObject* pSour, CGameObject* pDest)->bool
	{
		return pSour->Get_CameraDistance() > pDest->Get_CameraDistance();
	});
	return Render(RENDERGROUP::ALPHABLEND);
}

HRESULT CRenderer::Sequence_OutLine()
{
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_RimDodge"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_iWinCX", &m_iViewportX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_iWinCY", &m_iViewportY, sizeof(_uint))))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Sequence_NonLight()
{
	return Render(RENDERGROUP::NONLIGHT);
}

HRESULT CRenderer::Sequence_Result()
{
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDefault", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Default"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texShade", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Shade"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texSpecular", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Specular"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texRimLight", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_RimLight"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDepth", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Depth"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texShadowDepth", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_ShadowDepth"))))
		return E_FAIL;

	

	if(FAILED(m_pVIBuffer->SetUp_RawValue("fFogBegin", &m_fFogBegin, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("fFogEnd", &m_fFogEnd, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vFogColor", &m_vFogColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("iFogging", &m_iFogging, sizeof(_int))))
		return E_FAIL;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_matViewInv", &XMMatrixTranspose(pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_VIEW)), sizeof(_matrix))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_matProjInv", &XMMatrixTranspose(pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_PROJ)), sizeof(_matrix))))
		return E_FAIL;

	_float fFar = pPipeLine->Get_Far();
	_vector vCamPos = pPipeLine->Get_CameraPosition();
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_vCamPos", &vCamPos, sizeof(_float4))))
		return E_FAIL;

	CLightManager* pLightManager = CLightManager::Get_Instance();
	const LIGHTDEPTHDESC*	pLightDepthDesc = pLightManager->Get_LightDepthDesc(L"ShadowDepth", 0);
	if (nullptr != pLightDepthDesc)
	{
		if (true == pLightDepthDesc->bDepth)
		{
			_matrix matLightView = XMLoadFloat4x4(&pLightDepthDesc->m_matView);
			m_pVIBuffer->SetUp_RawValue("g_matLightView", &XMMatrixTranspose(matLightView), sizeof(_matrix));

			_matrix	matLightProj = XMLoadFloat4x4(&pLightDepthDesc->m_matProj);
			m_pVIBuffer->SetUp_RawValue("g_matLightProj", &XMMatrixTranspose(matLightProj), sizeof(_matrix));

			_float fLightFar = pLightDepthDesc->fFar;
			m_pVIBuffer->SetUp_RawValue("g_fLightFar", &fLightFar, sizeof(_float));
		}
	}

	m_pVIBuffer->Render((_uint)PASS_INDEX::BLEND);

	return S_OK;
}

HRESULT CRenderer::Sequence_Luminance()
{


	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Horizontal_19()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_DeferredResult", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_DeferredResult_Glow"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_AfterDeferred", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Glow"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenWidth", &tViewportDesc.Width, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_HORIZONTAL_19);

	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Vertical_19()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_Horizontal", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow_Horizontal"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenHeight", &tViewportDesc.Height, sizeof(_float))))
		return E_FAIL;


	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_VERTICAL_19);

	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Horizontal_9()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_DeferredResult", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_AfterDeferred", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Glow"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenWidth", &tViewportDesc.Width, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_HORIZONTAL_9);

	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Vertical_9()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_Horizontal", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow_Horizontal_9"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenHeight", &tViewportDesc.Height, sizeof(_float))))
		return E_FAIL;


	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_VERTICAL_9);

	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Horizontal_41()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_DeferredResult", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_AfterDeferred", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Glow"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenWidth", &tViewportDesc.Width, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_HORIZONTAL_41);

	return S_OK;
}

HRESULT CRenderer::Sequence_Glow_Vertical_41()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow_Horizontal", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow_Horizontal"))))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenHeight", &tViewportDesc.Height, sizeof(_float))))
		return E_FAIL;


	m_pVIBuffer->Render((_uint)PASS_INDEX::GLOW_VERTICAL_41);

	return S_OK;
}


HRESULT CRenderer::Sequence_MotionBlur()
{
	m_bBlur = false;

	if (!m_listRenderObjects[(_uint)RENDERGROUP::MOTIONBLUR].empty())
		m_bBlur = true;

	for (auto& pGameObject : m_listRenderObjects[(_uint)RENDERGROUP::MOTIONBLUR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_MotionBlur()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)RENDERGROUP::MOTIONBLUR].clear();

	return S_OK;
}

HRESULT CRenderer::Sequence_RimDodge()
{
	for (auto& pGameObject : m_listRenderObjects[(_uint)RENDERGROUP::RIMDODGE])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_RimDodge()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)RENDERGROUP::RIMDODGE].clear();
	return S_OK;
}

HRESULT CRenderer::Sequence_PostProcessing_BeforeUI()
{
	D3D11_VIEWPORT	tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint			iNumViewport = 1;
	m_pContextDevice->RSGetViewports(&iNumViewport, &tViewportDesc);

	// Default Resources
#pragma region PostProcess0
	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_Default")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_DeferredResult"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texAlphaGroup", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Default"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texGlow", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Glow_Vertical"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDistortionMap", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Distortion"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDepth_AlphaGroup", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_AfterDeferred_Depth"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDepth", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Depth"))))
		return E_FAIL;

	_float fFar = CPipeLine::Get_Instance()->Get_Far();
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;
#pragma endregion Deferred
	
#pragma region PostProcess0
	D3D11_VIEWPORT tViewport;
	ZeroMemory(&tViewport, sizeof(D3D11_VIEWPORT));
	tViewport.MinDepth = 0.f;
	tViewport.MaxDepth = 1.f;
	tViewport.Width = tViewportDesc.Width / 4.f;
	tViewport.Height = tViewportDesc.Height / 4.f;

	m_pContextDevice->RSSetViewports(1, &tViewport);

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_DownSampling")))
		return E_FAIL;


	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_Default"))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_DOWNSAMPLING);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

#pragma endregion DownSampling

#pragma region PostProcess0
	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_GaussianBlur_X")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_DownSampling"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenWidth", &tViewport.Width, sizeof(float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_GAUSSIANBLUR_X);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_GaussianBlur")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_GaussianBlur_X"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fScreenHeight", &tViewport.Height, sizeof(float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_GAUSSIANBLUR);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

#pragma endregion GaussianBlur
	m_pContextDevice->RSSetViewports(1, &tViewportDesc);

#pragma region PostProcess0
	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_DOF")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_Default"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuseLow", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_GaussianBlur"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDepth", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Depth"))))
		return E_FAIL;

	_float2			vPixelSizeHigh = _float2(1.f / tViewportDesc.Width, 1.f / tViewportDesc.Height);
	_float2			vPixelSizeLow = _float2(1.f / tViewport.Width, 1.f / tViewport.Height);

	if (FAILED(m_pVIBuffer->SetUp_RawValue("vPixelSizeHigh", &vPixelSizeHigh, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vPixelSizeLow", &vPixelSizeLow, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("ToneMappingType", &m_iToneMap, sizeof(_uint))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vDofParams", &m_vTargetDOF_Params, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("fDofDistance", &m_fTargetDOF_Distance, sizeof(_float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_DOF);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;
#pragma endregion DOF

#pragma region PostProcess0
	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostProcess0_MotionBlur")))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_DOF"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texMotionBlur", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_MotionBlur"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDodge", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Dodge"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_bMotionBlur", &m_bBlur, sizeof(bool))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_bDodge", &m_bDodge, sizeof(bool))))
		return E_FAIL;
	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_BLUR);

	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;
#pragma endregion MotionBlur

#pragma region PostProcess2
	if (FAILED(CRenderTargetManager::Get_Instance()->Begin_MRT(L"MRT_PostPorcess2_Distortion")))
		return E_FAIL;

	_uint iRandom = rand() % 15;
	_uint iTex = 0; 
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess0_MotionBlur"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_bDistortion", &m_bDistortion, sizeof(bool))))
		return E_FAIL; 
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_bGrayDistortion", &m_bGrayDistortion, sizeof(bool))))
		return E_FAIL;

	if (m_bDistortion)
	{
		iTex = (rand() % (_int)DISTORTIONTYPE::COUNT);

		if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDistortion", m_pDistortionTexture->Get_SRV(iTex))))
			return E_FAIL;
	}
	else if (m_bGrayDistortion)
	{
		iTex = (rand() % (_int)DISTORTIONTYPE::COUNT);

		if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDistortion", m_pDistortionTexture->Get_SRV(iTex))))
			return E_FAIL;
	}

	_float fTimeDelta = CPipeLine::Get_Instance()->Get_fTimeDelta();
	fTimeDelta = fTimeDelta / (_float)iRandom;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_fTimeDelta", &fTimeDelta, sizeof(float))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::POST_PROCESSING_BEFOREUI_DISTORTION);
	if (FAILED(CRenderTargetManager::Get_Instance()->End_MRT()))
		return E_FAIL;

#pragma endregion Distortion

#pragma region LAST
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_PostProcess2_Distortion"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texHalf", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("g_bHalfRender", &m_bHalfRender, sizeof(bool))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::FINALCOLOR);
#pragma endregion COLOR

	return S_OK;
}

HRESULT CRenderer::Sequence_UI()
{

	m_listRenderObjects[(_uint)RENDERGROUP::UI].sort([](CGameObject* pSour, CGameObject* pDest)->bool
	{
		return pSour->Get_DistanceZ() < pDest->Get_DistanceZ();
	});

	for (auto& pGameObject : m_listRenderObjects[(_uint)RENDERGROUP::UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)RENDERGROUP::UI].clear();

	for (auto& pGameObject : m_listRenderObjects[(_uint)RENDERGROUP::CURSOR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_listRenderObjects[(_uint)RENDERGROUP::CURSOR].clear();

	m_bDistortion = false;
	m_bGrayDistortion = false;
	m_bDodge = false;

	return S_OK;
}

HRESULT CRenderer::Sequence_Dodge()
{
	if (FAILED(m_pVIBuffer->SetUp_Texture("g_texDiffuse", CRenderTargetManager::Get_Instance()->Get_SRV(L"RTV_RimDodge"))))
		return E_FAIL;

	_float2		vLT_TexCoord = _float2( 0.f				, -m_fUVDistance);
	_float2		vRT_TexCoord = _float2( m_fUVDistance	,  0.f			);
	_float2		vRB_TexCoord = _float2( 0.f				,  m_fUVDistance);
	_float2		vLB_TexCoord = _float2(-m_fUVDistance	,  0.f			);

	if (FAILED(m_pVIBuffer->SetUp_RawValue("vLT_TexCoord", &vLT_TexCoord, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vRT_TexCoord", &vRT_TexCoord, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vRB_TexCoord", &vRB_TexCoord, sizeof(_float2))))
		return E_FAIL;
	if (FAILED(m_pVIBuffer->SetUp_RawValue("vLB_TexCoord", &vLB_TexCoord, sizeof(_float2))))
		return E_FAIL;

	m_pVIBuffer->Render((_uint)PASS_INDEX::DODGE);

	return S_OK;
}

void CRenderer::Set_UVDistance(const _float & fDistance)
{
	m_fUVDistance += fDistance;
	if (m_fUVDistance >= 0.1f)
		m_fUVDistance = 0.1f;
}

void CRenderer::ReSet_UVDistance()
{
	m_fUVDistance = 0.f;
}

void CRenderer::Set_Fog(const _float & fFogBegin, const _float & fFogEnd, const _float4 & fFogColor)
{
	m_fFogBegin = fFogBegin;
	m_fFogEnd = fFogEnd;
	m_vFogColor = fFogColor;
}

void CRenderer::Set_Fogging(const _int & iFogging)
{
	m_iFogging = iFogging;
}

_float4 * CRenderer::Get_DOF_Parmas()
{
	return &m_vTargetDOF_Params;
}

void CRenderer::Set_DOF_Params(const _float4 & vDOF)
{
	m_vTargetDOF_Params = vDOF;
}

const _uint & CRenderer::Get_HitIndex()
{
	++m_iHit;
	
	if (m_iHit >= 30)
		m_iHit = 0;

	return m_iHit;
}

ID3D11ShaderResourceView * CRenderer::Get_DistortionTexture(DISTORTIONTYPE eType)
{
	return m_pDistortionTexture->Get_SRV((_uint)eType);
}

HRESULT CRenderer::Draw_DebugGroup()
{
	if (FAILED(Render_Navigation()))
		return E_FAIL;

	if (FAILED(Render_Collider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Navigation()
{
	for (auto& pComponent : m_listRenderComponents[(_uint)DEBUGGROUP::NAVIGATION])
	{
		if (nullptr != pComponent)
		{
			CNavigation*	pNavigation = (CNavigation*)pComponent;
			if (FAILED(pNavigation->Render()))
				return E_FAIL;

			Safe_Release(pComponent);
		}
	}
	m_listRenderComponents[(_uint)DEBUGGROUP::NAVIGATION].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Collider()
{
	for (auto& pComponent : m_listRenderComponents[(_uint)DEBUGGROUP::COLLIDER])
	{
		if (nullptr != pComponent)
		{
			CCollider*		pCollider = (CCollider*)pComponent;
			if (FAILED(pCollider->Render()))
				return E_FAIL;

			Safe_Release(pComponent);
		}
	}
	m_listRenderComponents[(_uint)DEBUGGROUP::COLLIDER].clear();

	return S_OK;
}

void CRenderer::Set_DOF_Distance(const _float & fDist)
{
	m_fTargetDOF_Distance = fDist;
}

void CRenderer::SSAO()
{
	m_bSSAO = !m_bSSAO;
}

void CRenderer::Add_RenderDebug()
{
	++m_iDebugCnt;

	if (m_iDebugCnt > 3)
		m_iDebugCnt = 0;
}

void CRenderer::Half_Rendering()
{
	m_bHalfRender = !m_bHalfRender;
}

void CRenderer::RenderTarget_Rendering()
{
	m_bRenderTarget = !m_bRenderTarget;
}

void CRenderer::Next_ToneMap()
{
	++m_iToneMap;
	if (m_iToneMap >= (int)TONE_MAP::COUNT)
		m_iToneMap = 0;
}

void CRenderer::Pre_ToneMap()
{
	--m_iToneMap;
	if (m_iToneMap < 0)
		m_iToneMap = (int)TONE_MAP::COUNT - 1;
}

CRenderer * CRenderer::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pShaderFilePath, const char* pTechniqueName)
{
	CRenderer* pInstance = new CRenderer(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& ObjectList : m_listRenderObjects)
	{
		for (auto& GameObject : ObjectList)
			Safe_Release(GameObject);
		ObjectList.clear();
	}
	
	for (auto& ComponentList : m_listRenderComponents)
	{
		for (auto& Component : ComponentList)
			Safe_Release(Component);
		ComponentList.clear();
	}

	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pDistortionTexture);
	//Safe_Release(m_pVIBuffer_AfterDeferred);
	//Safe_Release(m_pVIBuffer_PostProcessing);
}
