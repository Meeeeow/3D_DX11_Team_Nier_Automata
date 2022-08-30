#include "stdafx.h"
#include "../public/UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	/* Empty */
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
	, m_ProjMatrix(rhs.m_ProjMatrix)
{
	/* Empty */
}

void CUI::Free()
{
	__super::Free();
}

HRESULT CUI::NativeConstruct_Prototype()
{
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.0f, 1.f));

	return S_OK;
}

HRESULT CUI::NativeConstruct(void * pArg)
{
	ZeroMemory(&m_ScreenPosRect, sizeof(RECT));

	return S_OK;
}

HRESULT CUI::SetUp_Components()
{
	return S_OK;
}

_int CUI::Tick(_double TimeDelta)
{
	switch (m_eUIState)
	{
	case Client::CUI::UISTATE::ACTIVATE:
		Activate(TimeDelta);
		break;
	case Client::CUI::UISTATE::INACTIVATE:
		Inactivate(TimeDelta);
		break;
	case Client::CUI::UISTATE::ENABLE:
		Enable(TimeDelta);
		break;
	case Client::CUI::UISTATE::DISABLE:
		Disable(TimeDelta);
		break;
	case Client::CUI::UISTATE::PRESSED:
		Pressed(TimeDelta);
		break;
	case Client::CUI::UISTATE::RELEASED:
		Released(TimeDelta);
		break;
	case Client::CUI::UISTATE::NONE:
		break;
	default:
		break;
	}
	return (_int)m_eState;
}

_int CUI::LateTick(_double TimeDelta)
{
	if (m_eUIState != UISTATE::DISABLE)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	}
	return 0;
}

HRESULT CUI::Render()
{
	return S_OK;
}

void CUI::SetUp_AtlasUV(_uint iCategory, const _tchar* szElemTag)
{
	if (nullptr == szElemTag || !lstrcmp(TEXT(""), szElemTag))
		return;

	CAtlas_Manager*	pInstance = GET_INSTANCE(CAtlas_Manager);

	m_OrthoDesc.fTexSizeX = (_float)m_pTextureCom->Get_TexMetaData(iCategory).width;
	m_OrthoDesc.fTexSizeY = (_float)m_pTextureCom->Get_TexMetaData(iCategory).height;

	ATLASDESC* atlasDesc = nullptr;

	atlasDesc = pInstance->Get_AtlasDesc(iCategory, szElemTag);

	m_AtlasUV.fOriginLeft = atlasDesc->fLeft;
	m_AtlasUV.fOriginRight = atlasDesc->fRight;
	m_AtlasUV.fOriginTop = atlasDesc->fTop;
	m_AtlasUV.fOriginBottom = atlasDesc->fBottom;

	m_AtlasUV.fLeft = atlasDesc->fLeft / m_OrthoDesc.fTexSizeX;
	m_AtlasUV.fRight = atlasDesc->fRight / m_OrthoDesc.fTexSizeX;
	m_AtlasUV.fTop = atlasDesc->fTop / m_OrthoDesc.fTexSizeY;
	m_AtlasUV.fBottom = atlasDesc->fBottom / m_OrthoDesc.fTexSizeY;

	RELEASE_INSTANCE(CAtlas_Manager);
}

void CUI::SetUp_WorldToScreen(_fvector vWorldPos)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (false == pGameInstance->Culling(vWorldPos))
	{
		m_fWorldToScreenX = -300.f;
		m_fWorldToScreenY = -300.f;
		return;
	}

	_vector			vTargetPos;
	_float4			vViewPort;

	vTargetPos = vWorldPos;
	vTargetPos = XMVector3Transform(vTargetPos, (pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW)));
	vTargetPos = XMVector3TransformCoord(vTargetPos, (pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_PROJ)));
	
	XMStoreFloat4(&vViewPort, vTargetPos);

	m_fWorldToScreenX = (vViewPort.x + 1.f) * (g_iWinCX >> 1);			// (0 ~ 2) * 640
	m_fWorldToScreenY = abs((vViewPort.y - 1.f) * (g_iWinCY >> 1));		// (0 ~ -2) * 360

	if (g_iWinCX < m_fWorldToScreenX || m_fWorldToScreenX < 0 || g_iWinCY < m_fWorldToScreenY || m_fWorldToScreenY < 0)
	{
		m_fWorldToScreenX = -300.f;
		m_fWorldToScreenY = -300.f;
	}
	return;
}

HRESULT CUI::SetUp_Transform(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float fOriginSizeX = m_AtlasUV.fOriginRight - m_AtlasUV.fOriginLeft;
	_float fOriginSizeY = m_AtlasUV.fOriginBottom - m_AtlasUV.fOriginTop;

	m_OrthoDesc.fUIScaleX = fScaleX * m_OrthoDesc.fTexSizeX;
	m_OrthoDesc.fUIScaleY = fScaleY * m_OrthoDesc.fTexSizeY;
	m_OrthoDesc.fOffsetX = (m_OrthoDesc.fUIScaleX) / 2.f - (m_AtlasUV.fOriginLeft * fScaleX) - (fOriginSizeX / 2.f * fScaleX);
	m_OrthoDesc.fOffsetY = (m_OrthoDesc.fUIScaleY) / 2.f - (m_AtlasUV.fOriginTop * fScaleY) - (fOriginSizeY / 2.f * fScaleY);
	m_OrthoDesc.fUIPosX = fPosX + m_OrthoDesc.fOffsetX - _float(g_iWinCX >> 1);
	m_OrthoDesc.fUIPosY = (-fPosY) - m_OrthoDesc.fOffsetY + _float(g_iWinCY >> 1);

	//m_ScreenPosRect.left = LONG(m_OrthoDesc.fUIPosX - (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.right = LONG(m_OrthoDesc.fUIPosX + (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.top = LONG(m_OrthoDesc.fUIPosY - (m_OrthoDesc.fUIScaleY / 2.f));
	//m_ScreenPosRect.bottom = LONG(m_OrthoDesc.fUIPosY + (m_OrthoDesc.fUIScaleY / 2.f));

	m_ScreenPosRect.left = LONG(fPosX - (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.right = LONG(fPosX + (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.top = LONG(fPosY - (fOriginSizeY / 2.f * fScaleY));
	m_ScreenPosRect.bottom = LONG(fPosY + (fOriginSizeY / 2.f * fScaleY));

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_OrthoDesc.fUIPosX, m_OrthoDesc.fUIPosY, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_OrthoDesc.fUIScaleX, m_OrthoDesc.fUIScaleY, 1.f, 0.f));

	return S_OK;
}

HRESULT CUI::SetUp_Transform_AlignLeft(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float fOriginSizeX = m_AtlasUV.fOriginRight - m_AtlasUV.fOriginLeft;
	_float fOriginSizeY = m_AtlasUV.fOriginBottom - m_AtlasUV.fOriginTop;

	m_OrthoDesc.fUIScaleX = fScaleX * m_OrthoDesc.fTexSizeX;
	m_OrthoDesc.fUIScaleY = fScaleY * m_OrthoDesc.fTexSizeY;
	m_OrthoDesc.fOffsetX = (m_OrthoDesc.fUIScaleX) / 2.f - (m_AtlasUV.fOriginLeft * fScaleX);// -(fOriginSizeX / 2.f * fScaleX);
	m_OrthoDesc.fOffsetY = (m_OrthoDesc.fUIScaleY) / 2.f - (m_AtlasUV.fOriginTop * fScaleY) - (fOriginSizeY / 2.f * fScaleY);
	m_OrthoDesc.fUIPosX = fPosX + m_OrthoDesc.fOffsetX - _float(g_iWinCX >> 1);
	m_OrthoDesc.fUIPosY = (-fPosY) - m_OrthoDesc.fOffsetY + _float(g_iWinCY >> 1);

	//m_ScreenPosRect.left = LONG(m_OrthoDesc.fUIPosX - (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.right = LONG(m_OrthoDesc.fUIPosX + (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.top = LONG(m_OrthoDesc.fUIPosY - (m_OrthoDesc.fUIScaleY / 2.f));
	//m_ScreenPosRect.bottom = LONG(m_OrthoDesc.fUIPosY + (m_OrthoDesc.fUIScaleY / 2.f));

	m_ScreenPosRect.left = LONG(fPosX - (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.right = LONG(fPosX + (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.top = LONG(fPosY - (fOriginSizeY / 2.f * fScaleY));
	m_ScreenPosRect.bottom = LONG(fPosY + (fOriginSizeY / 2.f * fScaleY));

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_OrthoDesc.fUIPosX, m_OrthoDesc.fUIPosY, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_OrthoDesc.fUIScaleX, m_OrthoDesc.fUIScaleY, 1.f, 0.f));

	return S_OK;
}

HRESULT CUI::SetUp_Transform_AlignRight(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float fOriginSizeX = m_AtlasUV.fOriginRight - m_AtlasUV.fOriginLeft;
	_float fOriginSizeY = m_AtlasUV.fOriginBottom - m_AtlasUV.fOriginTop;

	m_OrthoDesc.fUIScaleX = fScaleX * m_OrthoDesc.fTexSizeX;
	m_OrthoDesc.fUIScaleY = fScaleY * m_OrthoDesc.fTexSizeY;
	m_OrthoDesc.fOffsetX = (m_OrthoDesc.fUIScaleX) / 2.f - (m_AtlasUV.fOriginLeft * fScaleX) - (fOriginSizeX  * fScaleX);
	m_OrthoDesc.fOffsetY = (m_OrthoDesc.fUIScaleY) / 2.f - (m_AtlasUV.fOriginTop * fScaleY) - (fOriginSizeY / 2.f * fScaleY);
	m_OrthoDesc.fUIPosX = fPosX + m_OrthoDesc.fOffsetX - _float(g_iWinCX >> 1);
	m_OrthoDesc.fUIPosY = (-fPosY) - m_OrthoDesc.fOffsetY + _float(g_iWinCY >> 1);

	//m_ScreenPosRect.left = LONG(m_OrthoDesc.fUIPosX - (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.right = LONG(m_OrthoDesc.fUIPosX + (m_OrthoDesc.fUIScaleX / 2.f));
	//m_ScreenPosRect.top = LONG(m_OrthoDesc.fUIPosY - (m_OrthoDesc.fUIScaleY / 2.f));
	//m_ScreenPosRect.bottom = LONG(m_OrthoDesc.fUIPosY + (m_OrthoDesc.fUIScaleY / 2.f));

	m_ScreenPosRect.left = LONG(fPosX - (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.right = LONG(fPosX + (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.top = LONG(fPosY - (fOriginSizeY / 2.f * fScaleY));
	m_ScreenPosRect.bottom = LONG(fPosY + (fOriginSizeY / 2.f * fScaleY));

	m_pTransformCom->Set_State(CTransform::STATE::POSITION, XMVectorSet(m_OrthoDesc.fUIPosX, m_OrthoDesc.fUIPosY, 0.f, 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(m_OrthoDesc.fUIScaleX, m_OrthoDesc.fUIScaleY, 1.f, 0.f));

	return S_OK;
}

HRESULT CUI::SetUp_ScreenPosRect(_float fPosX, _float fPosY, _float fScaleX, _float fScaleY)
{
	_float fOriginSizeX = m_AtlasUV.fOriginRight - m_AtlasUV.fOriginLeft;
	_float fOriginSizeY = m_AtlasUV.fOriginBottom - m_AtlasUV.fOriginTop;

	m_ScreenPosRect.left = LONG(fPosX - (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.right = LONG(fPosX + (fOriginSizeX / 2.f * fScaleX));
	m_ScreenPosRect.top = LONG(fPosY - (fOriginSizeY / 2.f * fScaleY));
	m_ScreenPosRect.bottom = LONG(fPosY + (fOriginSizeY / 2.f * fScaleY));

	return S_OK;
}

HRESULT	CUI::SetUp_BillBoarding()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_float4x4 ViewMatrix, CamWorldMatrix;

	XMStoreFloat4x4(&ViewMatrix, pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW));
	XMStoreFloat4x4(&CamWorldMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));

	_vector vScale = m_pTransformCom->Get_Scale();
	m_pTransformCom->Set_State(CTransform::STATE::RIGHT, *((_vector*)&CamWorldMatrix.m[0][0]));
	m_pTransformCom->Set_State(CTransform::STATE::UP, *((_vector*)&CamWorldMatrix.m[1][0]));
	m_pTransformCom->Set_State(CTransform::STATE::LOOK, *((_vector*)&CamWorldMatrix.m[2][0]));
	m_pTransformCom->Set_Scale(vScale);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CUI::SetUp_DefaultRawValue()
{
	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_float4x4));
	m_pModelCom->SetUp_RawValue("g_matView", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
	m_pModelCom->SetUp_RawValue("g_matProj", &XMMatrixTranspose(XMLoadFloat4x4(&m_ProjMatrix)), sizeof(_float4x4));

	m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));
	
	return S_OK;
}

HRESULT CUI::SetUp_Transform_ForPP(_fmatrix	matAffine)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_WorldMatrix(matAffine);

	return S_OK;
}

HRESULT CUI::SetUp_DefaultRawvalue_ForPP()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	return S_OK;
}

HRESULT CUI::SetUp_Transform_ForAtlasPP(_fmatrix matAffine)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float	fElemCenterUVPosX = (m_AtlasUV.fLeft + m_AtlasUV.fRight) / 2.f;
	_float	fElemCenterUVPosY = (m_AtlasUV.fTop + m_AtlasUV.fBottom) / 2.f;
	_float	fElemPosOffsetX = 0.5f - fElemCenterUVPosX;
	_float	fElemPosOffsetY = fElemCenterUVPosY - 0.5f;

	m_pTransformCom->Set_WorldMatrix(
		XMMatrixIdentity()
		* XMMatrixTranslationFromVector(XMVectorSet(fElemPosOffsetX, fElemPosOffsetY, 0.f, 1.f))
		* matAffine
	);

	return S_OK;
}

HRESULT CUI::SetUp_Transform_AlignLeft_ForAtlasPP(_fmatrix matAffine)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float	fElemCenterUVPosX = (m_AtlasUV.fLeft + m_AtlasUV.fRight) / 2.f;
	_float	fElemCenterUVPosY = (m_AtlasUV.fTop + m_AtlasUV.fBottom) / 2.f;
	_float	fElemPosOffsetX = (0.5f - fElemCenterUVPosX) + (fElemCenterUVPosX - m_AtlasUV.fLeft);
	_float	fElemPosOffsetY = fElemCenterUVPosY - 0.5f;

	m_pTransformCom->Set_WorldMatrix(
		XMMatrixIdentity()
		* XMMatrixTranslationFromVector(XMVectorSet(fElemPosOffsetX, fElemPosOffsetY, 0.f, 1.f))
		* matAffine
	);

	return S_OK;
}

HRESULT CUI::SetUp_Transform_AlignRight_ForAtlasPP(_fmatrix matAffine)
{
	if (nullptr == m_pTransformCom)
	{
		return E_FAIL;
	}

	_float	fElemCenterUVPosX = (m_AtlasUV.fLeft + m_AtlasUV.fRight) / 2.f;
	_float	fElemCenterUVPosY = (m_AtlasUV.fTop + m_AtlasUV.fBottom) / 2.f;
	_float	fElemPosOffsetX = (0.5f - fElemCenterUVPosX) - (fElemCenterUVPosX - m_AtlasUV.fLeft);
	_float	fElemPosOffsetY = fElemCenterUVPosY - 0.5f;

	m_pTransformCom->Set_WorldMatrix(
		XMMatrixIdentity()
		* XMMatrixTranslationFromVector(XMVectorSet(fElemPosOffsetX, fElemPosOffsetY, 0.f, 1.f))
		* matAffine
	);

	return S_OK;
}

HRESULT CUI::SetUp_DefaultRawvalue_ForAtlasPP()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pModelCom->SetUp_RawValue("g_matWorld", &m_pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	m_pModelCom->SetUp_RawValue("g_fAtlasPosX", &m_AtlasUV.fLeft, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasSizeX", &m_AtlasUV.fRight, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasPosY", &m_AtlasUV.fTop, sizeof(_float));
	m_pModelCom->SetUp_RawValue("g_fAtlasSizeY", &m_AtlasUV.fBottom, sizeof(_float));

	return S_OK;
}

void CUI::Notify(void * pMessage)
{
	return;
}

_int CUI::VerifyChecksum(void * pMessage)
{
	return 0;
}
