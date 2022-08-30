#include "stdafx.h"
#include "AfterImage.h"
#include "VIBuffer_Pointbillboard.h"


HRESULT CAfterImage::SetUp_Components()
{
	//* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	/* For.Com_Pointbillboard */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Pointbillboard"), TEXT("Com_Pointbillboard"), (CComponent**)&m_pPointbillboard)))
		return E_FAIL;

	/* For.Com_AlphaOneTexture */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_AfterImage"), TEXT("Com_AlphaOneTexture"), (CComponent**)&m_pAlphaOneTexture)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CAfterImage::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	
	_matrix ViewMatrix = pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW);
	_matrix ViewMatrixInverse = XMMatrixInverse(nullptr, ViewMatrix);
	_float4x4 vViewInv4x4 = {};
	XMStoreFloat4x4(&vViewInv4x4, ViewMatrixInverse);
	_vector vCamLook = {};
	memcpy_s(&vCamLook, sizeof(_float4), &vViewInv4x4.m[2][0], sizeof(_float4));
	XMVectorSetW(vCamLook, 0.f);
	vCamLook = XMVector3Normalize(vCamLook);
	m_pPointbillboard->SetUp_RawValue("g_vCamLook", &vCamLook, sizeof(_float4));

	m_pPointbillboard->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(m_pTransform->Get_WorldMatrix()), sizeof(_matrix));
	m_pPointbillboard->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pPointbillboard->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pPointbillboard->SetUp_Texture("g_texDefault", m_pSRV);

	_float fWidth = static_cast<_float>(g_iWinCX) * 0.007f;
	_float fHeight = static_cast<_float>(g_iWinCY) * 0.007f;

	m_pPointbillboard->SetUp_RawValue("g_fWidth", &fWidth, sizeof(_float));
	m_pPointbillboard->SetUp_RawValue("g_fHeight", &fHeight, sizeof(_float));

	m_pPointbillboard->SetUp_Texture("g_texAlphaOne", m_pAlphaOneTexture->Get_SRV(m_iAlphaOneTextureIndex));



	return S_OK;
}

CAfterImage::CAfterImage(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CAfterImage::CAfterImage(const CAfterImage & rhs)
	: CGameObject(rhs)
{
}


CAfterImage * CAfterImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAfterImage* pInstance = new CAfterImage(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CAfterImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAfterImage::Clone(void * pArg)
{
	CAfterImage* pInstance = new CAfterImage(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CAfterImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAfterImage::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pPointbillboard);
	Safe_Release(m_pAlphaOneTexture);
	Safe_Release(m_pSRV);
	Safe_Release(m_pTexture2D);
	__super::Free();
}

HRESULT CAfterImage::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAfterImage::NativeConstruct(void * pArg)
{
	AFTERIMAGE_DESC* pDesc = (AFTERIMAGE_DESC*)pArg;

	m_fMaxLifeTime = pDesc->fMaxLifeTime;

	memcpy(&m_tMovement, &pDesc->tEffectMovement, sizeof(EFFECT_MOVEMENT));

	m_pSRV = pDesc->pSRV;
	if (!m_pSRV)
		assert(false);
	Safe_AddRef(m_pSRV);

	m_iAlphaOneTextureIndex = rand() % 2;

	//D3D11_TEXTURE2D_DESC		tTextureDesc;
	//ZeroMemory(&tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	//tTextureDesc.Width = g_iWinCX;
	//tTextureDesc.Height = g_iWinCY;
	//tTextureDesc.MipLevels = 1;
	//tTextureDesc.ArraySize = 1;
	//tTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	//tTextureDesc.SampleDesc.Quality = 0;
	//tTextureDesc.SampleDesc.Count = 1;

	//tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	//tTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//tTextureDesc.CPUAccessFlags = 0;
	//tTextureDesc.MiscFlags = 0;


	//if (FAILED(m_pGraphicDevice->CreateTexture2D(&tTextureDesc, nullptr, &m_pTexture2D)))
	//	FAILMSG("AfterImage Failed to Create Texture2D - NativeConstruct");


	//D3D11_SHADER_RESOURCE_VIEW_DESC	tSRVDesc;
	//ZeroMemory(&tSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	//tSRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	//tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//tSRVDesc.Texture2D.MipLevels = 1;

	//if (FAILED(m_pGraphicDevice->CreateShaderResourceView(m_pTexture2D, &tSRVDesc, &m_pSRV)))
	//	FAILMSG("AfterImage Failed to Create SRV - NativeConstruct");

	//*m_pSRV = *pDesc->pSRV;


	if (FAILED(SetUp_Components()))
		assert(false);

	_vector vecPos = XMLoadFloat4(&m_tMovement.vPosition);
	vecPos = XMVectorSetY(vecPos, XMVectorGetY(vecPos) + 1.45f);
	
	m_pTransform->Set_State(CTransform::STATE::POSITION, vecPos);

	return S_OK;
}

_int CAfterImage::Tick(_double TimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
	{
		return (_int)OBJSTATE::DEAD;
	}

	const _float fTimeDelta = static_cast<_float>(TimeDelta);


	if (static_cast<_int>(OBJSTATE::DEAD) == __super::Tick(TimeDelta))
	{
		m_eState = OBJSTATE::DEAD;
	}

	m_fCurLifeTime += fTimeDelta;
	if (m_fMaxLifeTime > 0 && m_fMaxLifeTime < m_fCurLifeTime)
	{
		m_eState = OBJSTATE::DEAD;
	}

	m_pTransform->Go_DirWithSpeed(TimeDelta, XMLoadFloat3(&m_tMovement.vDirection), m_tMovement.fSpeed);

	// Update AlphaOneTexture Index;
	m_fAlphaOneTextureElapsedTime += fTimeDelta;
	if (m_fAlphaOneTextureElapsedTime > 0.2f)
	{
		m_fAlphaOneTextureElapsedTime = 0.f;
		if (++m_iAlphaOneTextureIndex > 1)
			m_iAlphaOneTextureIndex = 0;
	}

	return static_cast<_int>(OBJSTATE::ENABLE);
	//return static_cast<_int>(m_eState);
}

_int CAfterImage::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);

	return 0;
}

HRESULT CAfterImage::Render()
{
	if (!m_pPointbillboard) return E_FAIL;
	SetUp_ConstantTable();
	m_pPointbillboard->Render(static_cast<_uint>(POINTBILLBOARD_PASS_INDEX::AFTERIMAGE_PASS));

	return S_OK;
}