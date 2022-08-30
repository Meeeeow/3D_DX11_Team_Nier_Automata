#include "..\Public\Light.h"
#include "VIBuffer_Viewport.h"
#include "PipeLine.h"
#include "RenderTargetManager.h"
#include "Frustum.h"
#include "Renderer.h"
#include "LightManager.h"

CLight::CLight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

CLight::CLight(const CLight & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice), m_pContextDevice(rhs.m_pContextDevice), m_pVIBuffer(rhs.m_pVIBuffer)
{
	Safe_AddRef(m_pVIBuffer);

	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CLight::NativeConstruct(const _tchar * pShaderFilePath, const char * pTechniqueName,const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	m_tDesc = tLightDesc;

	switch (m_tDesc.eType)
	{
	case LIGHTDESC::TYPE::DIRECTIONAL:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_DIR;
		break;
	case LIGHTDESC::TYPE::POINT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_POINT;
		break;
	case LIGHTDESC::TYPE::SPOT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_SPOT;
		Calcultae_SpotLength();
		break;
	default:
		break;
	}

	D3D11_VIEWPORT			tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint					iNumViewports = 1;
	m_pContextDevice->RSGetViewports(&iNumViewports, &tViewportDesc);
	m_pVIBuffer = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, _float4(0.f, 0.f, tViewportDesc.Width, tViewportDesc.Height));

	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	if (tDepthDesc.bDepth)
		Synchronize(tDepthDesc);

	return S_OK;
}

HRESULT CLight::NativeConstruct_ForTool(const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	m_tDesc = tLightDesc;
	m_tDesc.bLightOn = true;

	switch (m_tDesc.eType)
	{
	case LIGHTDESC::TYPE::DIRECTIONAL:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_DIR;
		break;
	case LIGHTDESC::TYPE::POINT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_POINT;
		break;
	case LIGHTDESC::TYPE::SPOT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_SPOT;
		Calcultae_SpotLength();
		break;
	default:
		break;
	}

	D3D11_VIEWPORT			tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint					iNumViewports = 1;
	m_pContextDevice->RSGetViewports(&iNumViewports, &tViewportDesc);

	m_pVIBuffer = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, _float4(0.f, 0.f, tViewportDesc.Width, tViewportDesc.Height));
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	D3D11_VIEWPORT			tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));

	_uint					iNumViewports = 1;
	m_pContextDevice->RSGetViewports(&iNumViewports, &tViewportDesc);

	m_pVIBuffer = CVIBuffer_Viewport::Create(m_pGraphicDevice, m_pContextDevice, pShaderFilePath, pTechniqueName, _float4(0.f, 0.f, tViewportDesc.Width, tViewportDesc.Height));
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CLight::NativeConstruct(const LIGHTDESC & tLightDesc)
{
	m_tDesc = tLightDesc;

	switch (m_tDesc.eType)
	{
	case LIGHTDESC::TYPE::DIRECTIONAL:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_DIR;
		break;
	case LIGHTDESC::TYPE::POINT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_POINT;
		break;
	case LIGHTDESC::TYPE::SPOT:
		m_iPassIndex = (_uint)CRenderer::PASS_INDEX::LIGHT_SPOT;
		Calcultae_SpotLength();
		break;
	default:
		break;
	}


	return S_OK;
}

_int CLight::Tick(_double dTimeDelta)
{
	if (m_tDesc.eState == LIGHTSTATE::DISABLE)
		return (_uint)m_tDesc.eState;

	if (m_tDesc.eState == LIGHTSTATE::DURATION && m_tDesc.bLightOn)
	{
		m_tDesc.dDuration -= dTimeDelta;
		if (m_tDesc.dDuration <= 0.0)
		{
			m_tDesc.eState = LIGHTSTATE::DISABLE;
			return (_uint)m_tDesc.eState;
		}
	}

	return (_uint)m_tDesc.eState;
}

_int CLight::LateTick(_double dTimeDelta)
{
	CFrustum* pFrustum = CFrustum::Get_Instance();
	_vector vPosition = XMLoadFloat4(&m_tDesc.vPosition);
	switch (m_tDesc.eType)
	{
	case LIGHTDESC::TYPE::DIRECTIONAL:
		return  0;
	case LIGHTDESC::TYPE::POINT:
		m_tDesc.bLightOn = pFrustum->Culling_PointRadius(vPosition, m_tDesc.fRange);
		break;
	case LIGHTDESC::TYPE::SPOT:
		m_tDesc.bLightOn = pFrustum->Culling_PointRadius(vPosition, m_tDesc.fLength);
		break;
	default:
		break;
	}

	return 0;
}

HRESULT CLight::Render()
{
	if (!m_tDesc.bLightOn || m_tDesc.eState == LIGHTSTATE::DISABLE || m_tDesc.eState == LIGHTSTATE::WAIT)
		return S_OK;

	CLightManager*					pLightManager = CLightManager::Get_Instance();
	CRenderTargetManager*		pRenderTargetManager = CRenderTargetManager::Get_Instance();
	CPipeLine*							pPipeLine = CPipeLine::Get_Instance();

	switch (m_iPassIndex)
	{
	case (_uint)CRenderer::PASS_INDEX::LIGHT_DIR:
		m_pVIBuffer->SetUp_RawValue("g_vLightDir", &m_tDesc.vDirection, sizeof(_float4));
		break;
	case (_uint)CRenderer::PASS_INDEX::LIGHT_POINT:
		m_pVIBuffer->SetUp_RawValue("g_vLightPosition", &m_tDesc.vPosition, sizeof(_float4));
		m_pVIBuffer->SetUp_RawValue("g_fRange", &m_tDesc.fRange, sizeof(_float));
		break;
	case (_uint)CRenderer::PASS_INDEX::LIGHT_SPOT:
		m_pVIBuffer->SetUp_RawValue("g_vLightPosition", &m_tDesc.vPosition, sizeof(_float4));
		m_pVIBuffer->SetUp_RawValue("g_fRange", &m_tDesc.fLength, sizeof(_float));
		m_pVIBuffer->SetUp_RawValue("g_vTargetPosition", &m_tDesc.vTargetPosition, sizeof(_float4));

		_float fInnerCosine = cosf(XMConvertToRadians(m_tDesc.fInnerDegree));
		_float fOuterCosine = cosf(XMConvertToRadians(m_tDesc.fOuterDegree));

		m_pVIBuffer->SetUp_RawValue("g_fInnerCosine", &fInnerCosine, sizeof(_float));
		m_pVIBuffer->SetUp_RawValue("g_fOuterCosine", &fOuterCosine, sizeof(_float));

		break;
	}

	bool bSSAO = pLightManager->Get_SSAO();
	m_pVIBuffer->SetUp_RawValue("g_vLightDiffuse", &m_tDesc.vDiffuse, sizeof(_float4));
	m_pVIBuffer->SetUp_RawValue("g_vLightAmbient", &m_tDesc.vAmbient, sizeof(_float4));
	m_pVIBuffer->SetUp_RawValue("g_vLightSpecular", &m_tDesc.vSpecular, sizeof(_float4));
	m_pVIBuffer->SetUp_RawValue("g_bSSAO", &bSSAO, sizeof(bool));

	_float fFar = pPipeLine->Get_Far();
	m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pVIBuffer->SetUp_Texture("g_texSSAO", pRenderTargetManager->Get_SRV(L"RTV_SSAO_GaussianBlur"));
	m_pVIBuffer->SetUp_Texture("g_texNormal", pRenderTargetManager->Get_SRV(TEXT("RTV_Normal")));
	m_pVIBuffer->SetUp_Texture("g_texDepth", pRenderTargetManager->Get_SRV(TEXT("RTV_Depth")));

	_matrix		matProjInv, matViewInv;

	matProjInv = pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_PROJ);
	matViewInv = pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_VIEW);

	m_pVIBuffer->SetUp_RawValue("g_matProjInv", &XMMatrixTranspose(matProjInv), sizeof(_matrix));
	m_pVIBuffer->SetUp_RawValue("g_matViewInv", &XMMatrixTranspose(matViewInv), sizeof(_matrix));

	m_pVIBuffer->SetUp_RawValue("g_vCamPos", (void*)(&pPipeLine->Get_CameraPosition()), sizeof(_vector));

	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

void CLight::Synchronize(const LIGHTDEPTHDESC& tDepthDesc)
{
	memcpy(&m_tLightDepthDesc, &tDepthDesc, sizeof(LIGHTDEPTHDESC));

	// For. Directional Light's Initialize For Shadow Depth
	m_tLightDepthDesc.vFixedDistance = m_tLightDepthDesc.vEye;

	XMStoreFloat4x4(&m_tLightDepthDesc.m_matView
		, XMMatrixLookAtLH(
			XMLoadFloat4(&m_tLightDepthDesc.vEye)
			, XMLoadFloat4(&m_tLightDepthDesc.vAt)
			, XMLoadFloat4(&m_tLightDepthDesc.vAxis)));

	XMStoreFloat4x4(&m_tLightDepthDesc.m_matProj
		, XMMatrixPerspectiveFovLH(
			tDepthDesc.fFOV
			, tDepthDesc.fWidth / tDepthDesc.fHeigth
			, tDepthDesc.fNear, tDepthDesc.fFar));

}

void CLight::Synchronize_Tracing()
{
	if (!m_tLightDepthDesc.bDepth)
		return;

	// For. Directional Light's Synchronize For Shadow Depth
	XMStoreFloat4x4(&m_tLightDepthDesc.m_matView
		, XMMatrixLookAtLH(
			XMLoadFloat4(&m_tLightDepthDesc.vEye)
			, XMLoadFloat4(&m_tLightDepthDesc.vAt)
			, XMLoadFloat4(&m_tLightDepthDesc.vAxis)));

	XMStoreFloat4x4(&m_tLightDepthDesc.m_matProj
		, XMMatrixPerspectiveFovLH(
			m_tLightDepthDesc.fFOV
			, m_tLightDepthDesc.fWidth / m_tLightDepthDesc.fHeigth
			, m_tLightDepthDesc.fNear, m_tLightDepthDesc.fFar));
}

void CLight::ResetPosition_Shadow_DirectionLight(_fvector vTargetPosition)
{
	if (m_tDesc.eType != LIGHTDESC::TYPE::DIRECTIONAL)
		return;

	if (m_tLightDepthDesc.bDepth)
	{
		XMStoreFloat4(&m_tLightDepthDesc.vAt, vTargetPosition);
		XMStoreFloat4(&m_tLightDepthDesc.vEye, XMLoadFloat4(&m_tLightDepthDesc.vFixedDistance) + vTargetPosition);

		Synchronize_Tracing();
	}
}

void CLight::ResetPosition_PointLight(_fvector vTargetPosition)
{
	if (m_tDesc.eType != LIGHTDESC::TYPE::POINT)
		return;

	XMStoreFloat4(&m_tDesc.vPosition, vTargetPosition);
}

void CLight::ResetPosition_SpotLight(_fvector vEye)
{
	if (m_tDesc.eType != LIGHTDESC::TYPE::SPOT)
		return;

	XMStoreFloat4(&m_tDesc.vPosition, vEye);

	_float fDirX = m_tDesc.vPosition.x - m_tDesc.vTargetPosition.x;
	_float fDirY = m_tDesc.vPosition.y - m_tDesc.vTargetPosition.y;
	_float fDirZ = m_tDesc.vPosition.z - m_tDesc.vTargetPosition.z;

	m_tDesc.vDirection.x = fDirX / fabs(fDirX);
	m_tDesc.vDirection.y = fDirY / fabs(fDirY);
	m_tDesc.vDirection.z = fDirZ / fabs(fDirZ);

	m_tDesc.fLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_tDesc.vPosition) - XMLoadFloat4(&m_tDesc.vTargetPosition)));
}

void CLight::Trace_SpotLight(_fvector vTargetPosition)
{
	if (m_tDesc.eType != LIGHTDESC::TYPE::SPOT)
		return;

	XMStoreFloat4(&m_tDesc.vTargetPosition, vTargetPosition);

	_float fDirX = m_tDesc.vPosition.x - m_tDesc.vTargetPosition.x;
	_float fDirY = m_tDesc.vPosition.y - m_tDesc.vTargetPosition.y;
	_float fDirZ = m_tDesc.vPosition.z - m_tDesc.vTargetPosition.z;

	m_tDesc.vDirection.x = fDirX / fabs(fDirX);
	m_tDesc.vDirection.y = fDirY / fabs(fDirY);
	m_tDesc.vDirection.z = fDirZ / fabs(fDirZ);

	m_tDesc.fLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_tDesc.vPosition) - XMLoadFloat4(&m_tDesc.vTargetPosition)));
}

void CLight::Calcultae_SpotLength()
{
	_float fDirX = m_tDesc.vPosition.x - m_tDesc.vTargetPosition.x;
	_float fDirY = m_tDesc.vPosition.y - m_tDesc.vTargetPosition.y;
	_float fDirZ = m_tDesc.vPosition.z - m_tDesc.vTargetPosition.z;

	m_tDesc.vDirection.x = fDirX / fabs(fDirX);
	m_tDesc.vDirection.y = fDirY / fabs(fDirY);
	m_tDesc.vDirection.z = fDirZ / fabs(fDirZ);

	m_tDesc.fLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_tDesc.vPosition) - XMLoadFloat4(&m_tDesc.vTargetPosition)));
}

void CLight::Change_LightColor(COLOR eColor, _float4 vColor)
{
	switch (eColor)
	{
	case Engine::CLight::COLOR::DIFFUSE:
		m_tDesc.vDiffuse = vColor;
		break;
	case Engine::CLight::COLOR::AMBIENT:
		m_tDesc.vAmbient = vColor;
		break;
	case Engine::CLight::COLOR::SPECULAR:
		m_tDesc.vSpecular = vColor;
		break;
	case Engine::CLight::COLOR::ALL:
		m_tDesc.vDiffuse = vColor;
		m_tDesc.vAmbient = vColor;
		m_tDesc.vSpecular = vColor;
		break;
	default:
		break;
	}
}

void CLight::Increase_LightColor(_float4 vIncreasePower, _float4 vMaxIncreasePower, COLOR eColor)
{
	switch (eColor)
	{
	case Engine::CLight::COLOR::DIFFUSE:
		m_tDesc.vDiffuse.x += vIncreasePower.x;
		m_tDesc.vDiffuse.y += vIncreasePower.y;
		m_tDesc.vDiffuse.z += vIncreasePower.z;
		break;
	case Engine::CLight::COLOR::AMBIENT:
		m_tDesc.vAmbient.x += vIncreasePower.x;
		m_tDesc.vAmbient.y += vIncreasePower.y;
		m_tDesc.vAmbient.z += vIncreasePower.z;
		break;
	case Engine::CLight::COLOR::SPECULAR:
		m_tDesc.vSpecular.x += vIncreasePower.x;
		m_tDesc.vSpecular.y += vIncreasePower.y;
		m_tDesc.vSpecular.z += vIncreasePower.z;
		break;
	case Engine::CLight::COLOR::ALL:
		m_tDesc.vDiffuse.x += vIncreasePower.x;
		m_tDesc.vDiffuse.y += vIncreasePower.y;
		m_tDesc.vDiffuse.z += vIncreasePower.z;
		m_tDesc.vAmbient.x += vIncreasePower.x;
		m_tDesc.vAmbient.y += vIncreasePower.y;
		m_tDesc.vAmbient.z += vIncreasePower.z;
		m_tDesc.vSpecular.x += vIncreasePower.x;
		m_tDesc.vSpecular.y += vIncreasePower.y;
		m_tDesc.vSpecular.z += vIncreasePower.z;
		break;
	default:
		break;
	}

	switch (eColor)
	{
	case Engine::CLight::COLOR::DIFFUSE:
		if (m_tDesc.vDiffuse.x >= vMaxIncreasePower.x)
			m_tDesc.vDiffuse.x = vMaxIncreasePower.x;
		if (m_tDesc.vDiffuse.y >= vMaxIncreasePower.y)
			m_tDesc.vDiffuse.y = vMaxIncreasePower.y;
		if (m_tDesc.vDiffuse.z >= vMaxIncreasePower.z)
			m_tDesc.vDiffuse.z = vMaxIncreasePower.z;
		break;
	case Engine::CLight::COLOR::AMBIENT:
		if (m_tDesc.vAmbient.x >= vMaxIncreasePower.x)
			m_tDesc.vAmbient.x = vMaxIncreasePower.x;
		if (m_tDesc.vAmbient.y >= vMaxIncreasePower.y)
			m_tDesc.vAmbient.y = vMaxIncreasePower.y;
		if (m_tDesc.vAmbient.z >= vMaxIncreasePower.z)
			m_tDesc.vAmbient.z = vMaxIncreasePower.z;
		break;
	case Engine::CLight::COLOR::SPECULAR:
		if (m_tDesc.vSpecular.x >= vMaxIncreasePower.x)
			m_tDesc.vSpecular.x = vMaxIncreasePower.x;
		if (m_tDesc.vSpecular.y >= vMaxIncreasePower.y)
			m_tDesc.vSpecular.y = vMaxIncreasePower.y;
		if (m_tDesc.vSpecular.z >= vMaxIncreasePower.z)
			m_tDesc.vSpecular.z = vMaxIncreasePower.z;
		break;
	case Engine::CLight::COLOR::ALL:
		if (m_tDesc.vDiffuse.x >= vMaxIncreasePower.x)
			m_tDesc.vDiffuse.x = vMaxIncreasePower.x;
		if (m_tDesc.vDiffuse.y >= vMaxIncreasePower.y)
			m_tDesc.vDiffuse.y = vMaxIncreasePower.y;
		if (m_tDesc.vDiffuse.z >= vMaxIncreasePower.z)
			m_tDesc.vDiffuse.z = vMaxIncreasePower.z;
		if (m_tDesc.vAmbient.x >= vMaxIncreasePower.x)
			m_tDesc.vAmbient.x = vMaxIncreasePower.x;
		if (m_tDesc.vAmbient.y >= vMaxIncreasePower.y)
			m_tDesc.vAmbient.y = vMaxIncreasePower.y;
		if (m_tDesc.vAmbient.z >= vMaxIncreasePower.z)
			m_tDesc.vAmbient.z = vMaxIncreasePower.z;
		if (m_tDesc.vSpecular.x >= vMaxIncreasePower.x)
			m_tDesc.vSpecular.x = vMaxIncreasePower.x;
		if (m_tDesc.vSpecular.y >= vMaxIncreasePower.y)
			m_tDesc.vSpecular.y = vMaxIncreasePower.y;
		if (m_tDesc.vSpecular.z >= vMaxIncreasePower.z)
			m_tDesc.vSpecular.z = vMaxIncreasePower.z;
		break;
	default:
		break;
	}
}

void CLight::Decrease_LightColor(_float4 vDecreasePower, _float4 vMinIncreasePower, COLOR eColor)
{
	switch (eColor)
	{
	case Engine::CLight::COLOR::DIFFUSE:
		m_tDesc.vDiffuse.x -= vDecreasePower.x;
		m_tDesc.vDiffuse.y -= vDecreasePower.y;
		m_tDesc.vDiffuse.z -= vDecreasePower.z;
		break;
	case Engine::CLight::COLOR::AMBIENT:
		m_tDesc.vAmbient.x -= vDecreasePower.x;
		m_tDesc.vAmbient.y -= vDecreasePower.y;
		m_tDesc.vAmbient.z -= vDecreasePower.z;
		break;
	case Engine::CLight::COLOR::SPECULAR:
		m_tDesc.vSpecular.x -= vDecreasePower.x;
		m_tDesc.vSpecular.y -= vDecreasePower.y;
		m_tDesc.vSpecular.z -= vDecreasePower.z;
		break;
	case Engine::CLight::COLOR::ALL:
		m_tDesc.vDiffuse.x -= vDecreasePower.x;
		m_tDesc.vDiffuse.y -= vDecreasePower.y;
		m_tDesc.vDiffuse.z -= vDecreasePower.z;
		m_tDesc.vAmbient.x -= vDecreasePower.x;
		m_tDesc.vAmbient.y -= vDecreasePower.y;
		m_tDesc.vAmbient.z -= vDecreasePower.z;
		m_tDesc.vSpecular.x -= vDecreasePower.x;
		m_tDesc.vSpecular.y -= vDecreasePower.y;
		m_tDesc.vSpecular.z -= vDecreasePower.z;
		break;
	default:
		break;
	}

	switch (eColor)
	{
	case Engine::CLight::COLOR::DIFFUSE:
		if (m_tDesc.vDiffuse.x <= vMinIncreasePower.x)
			m_tDesc.vDiffuse.x = vMinIncreasePower.x;
		if (m_tDesc.vDiffuse.y <= vMinIncreasePower.y)
			m_tDesc.vDiffuse.y = vMinIncreasePower.y;
		if (m_tDesc.vDiffuse.z <= vMinIncreasePower.z)
			m_tDesc.vDiffuse.z = vMinIncreasePower.z;
		break;
	case Engine::CLight::COLOR::AMBIENT:
		if (m_tDesc.vAmbient.x <= vMinIncreasePower.x)
			m_tDesc.vAmbient.x = vMinIncreasePower.x;
		if (m_tDesc.vAmbient.y <= vMinIncreasePower.y)
			m_tDesc.vAmbient.y = vMinIncreasePower.y;
		if (m_tDesc.vAmbient.z <= vMinIncreasePower.z)
			m_tDesc.vAmbient.z = vMinIncreasePower.z;
		break;
	case Engine::CLight::COLOR::SPECULAR:
		if (m_tDesc.vSpecular.x <= vMinIncreasePower.x)
			m_tDesc.vSpecular.x = vMinIncreasePower.x;
		if (m_tDesc.vSpecular.y <= vMinIncreasePower.y)
			m_tDesc.vSpecular.y = vMinIncreasePower.y;
		if (m_tDesc.vSpecular.z <= vMinIncreasePower.z)
			m_tDesc.vSpecular.z = vMinIncreasePower.z;
		break;
	case Engine::CLight::COLOR::ALL:
		if (m_tDesc.vDiffuse.x <= vMinIncreasePower.x)
			m_tDesc.vDiffuse.x = vMinIncreasePower.x;
		if (m_tDesc.vDiffuse.y <= vMinIncreasePower.y)
			m_tDesc.vDiffuse.y = vMinIncreasePower.y;
		if (m_tDesc.vDiffuse.z <= vMinIncreasePower.z)
			m_tDesc.vDiffuse.z = vMinIncreasePower.z;
		if (m_tDesc.vAmbient.x <= vMinIncreasePower.x)
			m_tDesc.vAmbient.x = vMinIncreasePower.x;
		if (m_tDesc.vAmbient.y <= vMinIncreasePower.y)
			m_tDesc.vAmbient.y = vMinIncreasePower.y;
		if (m_tDesc.vAmbient.z <= vMinIncreasePower.z)
			m_tDesc.vAmbient.z = vMinIncreasePower.z;
		if (m_tDesc.vSpecular.x <= vMinIncreasePower.x)
			m_tDesc.vSpecular.x = vMinIncreasePower.x;
		if (m_tDesc.vSpecular.y <= vMinIncreasePower.y)
			m_tDesc.vSpecular.y = vMinIncreasePower.y;
		if (m_tDesc.vSpecular.z <= vMinIncreasePower.z)
			m_tDesc.vSpecular.z = vMinIncreasePower.z;
		break;
	default:
		break;
	}
}

void CLight::Increase_Range(_float fIncreaseRange, _float fMaxRange)
{
	m_tDesc.fRange += fIncreaseRange;
	if (m_tDesc.fRange >= fMaxRange)
		m_tDesc.fRange = fMaxRange;
}

void CLight::Decrease_Range(_float fDecreaseRange, _float fMinRange)
{
	m_tDesc.fRange -= fDecreaseRange;
	if (m_tDesc.fRange <= fMinRange)
		m_tDesc.fRange = fMinRange;
}

void CLight::Increase_InnerDegree(_float fIncreaseDegree, _float fMaxDegree)
{
	m_tDesc.fInnerDegree += fIncreaseDegree;
	if (m_tDesc.fInnerDegree >= fMaxDegree)
		m_tDesc.fInnerDegree = fMaxDegree;
}

void CLight::Decrease_InnerDegree(_float fDecreaseDegree)
{
	m_tDesc.fInnerDegree -= fDecreaseDegree;
	if (m_tDesc.fInnerDegree <= 0.f)
		m_tDesc.fInnerDegree = 0.f;
}

void CLight::Increase_OuterDegree(_float fIncreaseDegree, _float fMaxDegree)
{
	m_tDesc.fOuterDegree += fIncreaseDegree;
	if (m_tDesc.fOuterDegree >= fMaxDegree)
		m_tDesc.fOuterDegree = fMaxDegree;
}

void CLight::Decrease_OuterDegree(_float fDecreaseDegree)
{
	m_tDesc.fOuterDegree -= fDecreaseDegree;
	if (m_tDesc.fOuterDegree <= 0.f)
		m_tDesc.fOuterDegree = 0.f;
}

CLight * CLight::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC& tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	CLight* pInstance = new CLight(pGraphicDevice, pContextDevice);

	if (FAILED(pInstance->NativeConstruct(pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

CLight * CLight::Create_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar* pShaderFilePath, const char* pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	CLight* pInstance = new CLight(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_ForTool(pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

CLight * CLight::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	CLight* pInstance = new CLight(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pTechniqueName)))
		Safe_Release(pInstance);

	return pInstance;
}

CLight * CLight::Clone(const LIGHTDESC& pArg)
{
	CLight* pInstance = new CLight(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);
}
