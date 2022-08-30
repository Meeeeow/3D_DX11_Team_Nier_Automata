#include "stdafx.h"
#include "..\Public\DecalCube.h"


CDecalCube::CDecalCube(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{
}

CDecalCube::CDecalCube(const CDecalCube & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDecalCube::NativeConstruct_Prototype()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BREAK, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Break/325_%d.dds", 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BURNINGFIELD, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/BurningField/BurningField.png"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_CRACK0, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Crack0/Crack0_%d.png", 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_CRACK1, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Crack1/Crack1_%d.png", 2))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_SPIN, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Spin/Spin%d.png", 3))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BOOM, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Boom/Boom_%d.dds", 4))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_ARCH, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/Arch/Arch_%d.dds", 7))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Component((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_FOGRING, CTexture::Create(m_pGraphicDevice, m_pContextDevice, L"../Bin/Resources/Textures/Decal/FogRing/FogRing%d.png", 2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDecalCube::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	DECALCUBE* pInfo = (DECALCUBE*)pArg;

	if (FAILED(SetUp_Components(pInfo)))
		return E_FAIL;

	m_pTransform->Set_Scale(XMLoadFloat4(&pInfo->vScale));
	m_pTransform->Set_State(CTransform::STATE::POSITION,XMLoadFloat4(&pInfo->vPosition));

	if (pInfo->bContinue == false)
	{
		m_iImageIndex = pInfo->iImageIndex;
	}
	m_bContinue = pInfo->bContinue;

	m_iPassIndex = 1;

	m_dMaxTime = pInfo->dDuration;
	m_dDuration = pInfo->dDuration;
	m_dIndexTime = pInfo->dIndexTime;

	m_dIndexTimer = 0.0;

	m_fAlpha = 0.f;

	m_eState = OBJSTATE::ENABLE;
	m_bAlpha = pInfo->bAlpha;

	m_iGlow = pInfo->iIsGlow;
	m_iColorGlow = pInfo->iIsColorGlow;
	m_vGlowColor = pInfo->vGlowColor;

	m_pTransform->Set_RotationSpeed(pInfo->dRotationSpeed);
	m_pTransform->Set_Speed(pInfo->dMoveSpeed);

	m_vDir = pInfo->vDir;
	m_vRotationAxis = pInfo->vRotationAxis;
	m_fAddScale = pInfo->fAddScale;

	return S_OK;
}

_int CDecalCube::Tick(_double dTimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
		return (_int)m_eState;

	if (m_dDuration <= 0.0)
		m_eState = OBJSTATE::DEAD;

	Move(dTimeDelta);

	Calc_Alpha();
	Calc_Index();

	m_dDuration -= dTimeDelta;
	m_dIndexTimer += dTimeDelta;

	return (_int)m_eState;
}

_int CDecalCube::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);
	return _int();
}

HRESULT CDecalCube::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV(m_iImageIndex));
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

void CDecalCube::Calc_Alpha()
{
	if(m_bAlpha)
		m_fAlpha = 1.f - (_float)(m_dDuration / m_dMaxTime);
}

void CDecalCube::Calc_Index()
{
	if (!m_bContinue || m_iMaxIndex == 0)
		return;

	if (m_dIndexTimer >= m_dIndexTime)
	{
		++m_iImageIndex;
		m_dIndexTimer = 0.0;
		if (m_iImageIndex > m_iMaxIndex)
			m_iImageIndex = m_iMaxIndex;
	}
}

void CDecalCube::Move(_double dTimeDelta)
{
	m_pTransform->Go_Dir(dTimeDelta, XMLoadFloat4(&m_vDir));
	m_pTransform->Rotation_Axis(dTimeDelta, XMLoadFloat4(&m_vRotationAxis));
	m_pTransform->Add_Scale(m_fAddScale);
}

HRESULT CDecalCube::SetUp_Components(DECALCUBE* pInfo)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_VIBUFFER_CUBE, COM_KEY_CUBE, (CComponent**)&m_pVIBuffer)))
		return E_FAIL;


	switch (pInfo->eType)
	{
	case CDecalCube::TYPE::BREAK:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BREAK, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 1;
		break;
	case CDecalCube::TYPE::BURNINGFIELD:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BURNINGFIELD, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 0;
		break;
	case CDecalCube::TYPE::CRACK0:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_CRACK0, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 1;
		break;
	case CDecalCube::TYPE::CRACK1:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_CRACK1, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 1;
		break;
	case CDecalCube::TYPE::SPIN:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_SPIN, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 2;
		break;
	case CDecalCube::TYPE::BOOM:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_BOOM, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 3;
		break;
	case CDecalCube::TYPE::ARCH:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_ARCH, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 5;
		break;
	case CDecalCube::TYPE::FOGRING:
		if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TEXTURE_FOGRING, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
			return E_FAIL;
		m_iMaxIndex = 2;
	default:
		break;
	}
	

	return S_OK;
}

HRESULT CDecalCube::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	bool bAlpha = true;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));
	m_pVIBuffer->SetUp_RawValue("g_fAlpha", &m_fAlpha, sizeof(float));

	m_pVIBuffer->SetUp_RawValue("g_iIsGlow", &m_iGlow, sizeof(int));
	m_pVIBuffer->SetUp_RawValue("g_iIsColorGlow", &m_iColorGlow, sizeof(int));
	m_pVIBuffer->SetUp_RawValue("g_vGlowColor", &m_vGlowColor, sizeof(_float4));

	m_pVIBuffer->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose() , sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW) , sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ) , sizeof(_float4x4));
	
	_matrix			matInvWorld = XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix4x4());
	_matrix			matInvView = pGameInstance->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_VIEW);
	_matrix			matInvProj = pGameInstance->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_PROJ);

	matInvWorld = XMMatrixInverse(nullptr, matInvWorld);
	matInvWorld = XMMatrixTranspose(matInvWorld);

	matInvView = XMMatrixTranspose(matInvView);
	matInvProj = XMMatrixTranspose(matInvProj);

	m_pVIBuffer->SetUp_RawValue("g_matWorldInv", &matInvWorld, sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matViewInv" , &matInvView, sizeof(_float4x4));
	m_pVIBuffer->SetUp_RawValue("g_matProjInv" , &matInvProj, sizeof(_float4x4));
	
	_float			fFar = pGameInstance->Get_Far();
	m_pVIBuffer->SetUp_RawValue("g_fFar", &fFar, sizeof(float));

	m_pVIBuffer->SetUp_Texture("g_texDepth", pGameInstance->Get_SRV(L"RTV_Depth"));

	return S_OK;
}

CDecalCube * CDecalCube::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CDecalCube* pInstance = new CDecalCube(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CDecalCube * CDecalCube::Clone(void * pArg)
{
	CDecalCube* pInstance = new CDecalCube(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDecalCube::Free()
{
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTexture);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);

	__super::Free();
}
