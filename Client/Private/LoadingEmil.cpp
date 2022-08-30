#include "stdafx.h"
#include "..\Public\LoadingEmil.h"

CLoadingEmil::CLoadingEmil(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CProjectionBase(pGraphicDevice, pContextDevice)
{

}

CLoadingEmil::CLoadingEmil(const CLoadingEmil & rhs)
	: CProjectionBase(rhs)
{
}

HRESULT CLoadingEmil::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLoadingEmil::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_LOADING_EMIL, COM_KEY_TEXTURE, (CComponent**)&m_pTexture)))
		return E_FAIL;

	XMStoreFloat4x4(&m_matProj, XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f));
	_float fSizeX = 0.f, fSizeY = 0.f, fX = 0.f, fY = 0.f;

	fSizeX = 64.f;
	fSizeY = 64.f;
	fX = -608.f;
	fY = 320.f;

	m_pTransform->Set_Scale(XMVectorSet(fSizeX, fSizeY, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(fX, -fY, 0.f, 1.f));
	m_pTransform->Set_RotationSpeed(3.5);
	m_pTransform->Set_Speed(50.0);
	m_pTransform->Set_JumpPower(5.5f);
	
	m_iDistanceZ = 1;
	m_iPassIndex = 16;

	return S_OK;
}

_int CLoadingEmil::Tick(_double dTimeDelta)
{
	m_pTransform->Rotation_Axis(dTimeDelta * -m_fDirectionX, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	

	Move(dTimeDelta);
	Jump(dTimeDelta);

	return _int();
}

_int CLoadingEmil::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::UI, this);
	return _int();
}

HRESULT CLoadingEmil::Render()
{
	bool bAlpha = false;
	m_pVIBuffer->SetUp_RawValue("g_bAlpha", &bAlpha, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBuffer->SetUp_Texture("g_texDiffuse", m_pTexture->Get_SRV());
	m_pVIBuffer->Render(m_iPassIndex);

	return S_OK;
}

void CLoadingEmil::Jump(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance->Key_Down(DIK_SPACE) && m_bJump == false)
		m_bJump = true;

	if (m_bJump)
	{
		_float fY = m_pTransform->Jump(dTimeDelta);
		if (fY <= -320.f)
		{
			m_pTransform->Set_Jump(false);
			m_pTransform->Initialize_JumpTime();
			m_bJump = false;
			_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
			m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSetY(vPosition, -320.f));
		}
	}
}

void CLoadingEmil::Move(_double dTimeDelta)
{
	_float fPositionX = XMVectorGetX(m_pTransform->Get_State(CTransform::STATE::POSITION));
	if (fPositionX >= m_fMaximumX)
	{
		m_fDirectionX = -1.f;
	}
	else if (fPositionX <= m_fMinimumX)
	{
		m_fDirectionX = 1.f;
	}
	m_pTransform->Go_Dir(dTimeDelta, XMVectorSet(m_fDirectionX, 0.f, 0.f, 0.f));
}

CLoadingEmil * CLoadingEmil::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CLoadingEmil*	pInstance = new CLoadingEmil(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CLoadingEmil * CLoadingEmil::Clone(void * pArg)
{
	CLoadingEmil*	pInstance = new CLoadingEmil(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CLoadingEmil::Free()
{
	Safe_Release(m_pTexture);

	__super::Free();
}
