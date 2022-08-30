#include "stdafx.h"
#include "..\Public\GameObjectModel.h"
#include "PipeLine.h"



CGameObjectModel::CGameObjectModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{

}

CGameObjectModel::CGameObjectModel(const CGameObjectModel & rhs)
	: CGameObject(rhs)
{

}

void CGameObjectModel::Free()
{
	__super::Free();
}

HRESULT CGameObjectModel::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObjectModel::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

_int CGameObjectModel::Tick(_double dTimeDelta)
{
	return 0;
}

_int CGameObjectModel::LateTick(_double dTimeDelta)
{
	return 0;
}

HRESULT CGameObjectModel::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		m_pModel->Render(i, m_iPassIndex);
	}
	return S_OK;
}

HRESULT CGameObjectModel::Render_ShadowDepth()
{
	if (FAILED(SetUp_DepthConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		if(true == m_pModel->Get_HaveAnim())
			m_pModel->Render(i, static_cast<_int>(MESH_PASS_INDEX::ANIM_SHADOWDEPTH));
		else
			m_pModel->Render(i, static_cast<_int>(MESH_PASS_INDEX::NONANIM_SHADOWDEPTH));
	}

	return S_OK;
}

HRESULT CGameObjectModel::Render_MotionBlur()
{
	if (FAILED(SetUp_BlurConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Render_WithPreAnimation(i, (_uint)MESH_PASS_INDEX::MOTIONBLUR);
	}
	return S_OK;
}

HRESULT CGameObjectModel::Render_RimDodge()
{
	if (FAILED(SetUp_RimDodgeConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Render(i, (_uint)MESH_PASS_INDEX::RIMDODGE);
	}
	return S_OK;
}

const char * CGameObjectModel::Get_PassName(_uint iIndex)
{
	return m_pModel->Get_PassName(iIndex);
}

_uint CGameObjectModel::Get_MaxPassIndex()
{
	return m_pModel->Get_MaxPassIndex();
}

HRESULT CGameObjectModel::SetUp_Components()
{
	return S_OK;
}

HRESULT CGameObjectModel::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();


	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));
	
	Compute_CamDistanceForAlpha(m_pTransform);

	m_pModel->SetUp_RawValue("vDofParams", m_pRenderer->Get_DOF_Parmas(), sizeof(_float4));

	return S_OK;
}

HRESULT CGameObjectModel::SetUp_DepthConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	const LIGHTDEPTHDESC*	pLightDepthDesc = pGameInstance->Get_LightDepthDesc(L"ShadowDepth", 0);
	const LIGHTDESC*		pLightDesc = pGameInstance->Get_LightDesc(L"ShadowDepth");
	if (pLightDepthDesc == nullptr || pLightDesc == nullptr)
		return S_OK;

	_float fFar = pGameInstance->Get_Far();

	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

	_matrix matLightView = XMLoadFloat4x4(&pLightDepthDesc->m_matView);
	m_pModel->SetUp_RawValue("g_matLightView", &XMMatrixTranspose(matLightView), sizeof(_matrix));

	_matrix matLightProj = XMLoadFloat4x4(&pLightDepthDesc->m_matProj);
	m_pModel->SetUp_RawValue("g_matLightProj", &XMMatrixTranspose(matLightProj), sizeof(_matrix));

	_float fLightFar = pLightDepthDesc->fFar;
	m_pModel->SetUp_RawValue("g_fLightFar", &fLightFar, sizeof(_float));

	return S_OK;
}

HRESULT CGameObjectModel::SetUp_BlurConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();

	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	m_pModel->SetUp_RawValue("g_matOldWorld", &m_pTransform->Get_OldWorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matOldView", &pGameInstance->Get_OldTranspose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matOldProj", &pGameInstance->Get_OldTranspose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));

	return S_OK;
}

HRESULT CGameObjectModel::SetUp_RimDodgeConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();
	m_pModel;
	_matrix matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_matHitPosition));
	m_pModel->SetUp_RawValue("g_matWorld", &matWorld, sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));
	_vector vCamPos = pGameInstance->Get_CameraPosition();
	m_pModel->SetUp_RawValue("g_vCamPos", &vCamPos, sizeof(_float4));

	return S_OK;
}

_bool CGameObjectModel::Ray_Collision()
{
	return _bool();
}

_bool CGameObjectModel::Check_Collision(CGameObject * pGameObject, unsigned int iColliderType)
{
	return _bool();
}

void CGameObjectModel::Collision(CGameObject * pGameObject)
{
}

BoundingBox * CGameObjectModel::Get_AABB()
{
	if (nullptr == m_pHitBox)
	{
		return nullptr;
	}
	return m_pHitBox->Get_AABB();
}

BoundingSphere * CGameObjectModel::Get_Sphere()
{
	if (nullptr == m_pAttackBox)
	{
		return nullptr;
	}
	return m_pAttackBox->Get_Sphere();
}

BoundingSphere * CGameObjectModel::Get_MiddlePoint()
{
	if (nullptr == m_pMiddlePoint)
	{
		return nullptr;
	}
	return m_pMiddlePoint->Get_Sphere();
}

CCollider * CGameObjectModel::Get_ColliderAABB()
{
	if (nullptr == m_pHitBox)
	{
		return nullptr;
	}
	return m_pHitBox;
}

CCollider * CGameObjectModel::Get_ColliderSphere()
{
	if (nullptr == m_pAttackBox)
	{
		return nullptr;
	}
	return m_pAttackBox;
}

CCollider * CGameObjectModel::Get_ColliderMiddlePoint()
{
	if (nullptr == m_pMiddlePoint)
	{
		return nullptr;
	}
	return m_pMiddlePoint;
}

void CGameObjectModel::Update_Force(_double TimeDelta, class CNavigation* pNavigation)
{
	if (m_pTransform)
	{
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition += m_fForce * (_float)TimeDelta * XMLoadFloat3(&m_vForceDir);

		if (pNavigation)
		{
			_vector vOut = vPosition;
			CCell* pSlideCell = nullptr;

			if (!m_pTransform->IsJump())
			{
				if (pNavigation->IsOn(vOut, &pSlideCell))
				{
					if (!XMVector4Equal(XMVectorZero(), vOut))
						m_pTransform->Set_State(CTransform::STATE::POSITION, vOut);
				}
			}
		}
		else
			m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		   
		m_fForce -= m_fHitRecovery * (_float)TimeDelta;
		if (m_fForce < 0.0f)
			m_fForce = 0.0f;

		m_HittedTime += TimeDelta;
	}
}

void CGameObjectModel::Update_CollisionTime(_double TimeDelta)
{
	if (nullptr != m_pHitBox)
		m_pHitBox->Update_CollisionTime(TimeDelta);
}

void CGameObjectModel::Set_CurrentMatrix()
{
	XMStoreFloat4x4(&m_matHitPosition, m_pTransform->Get_WorldMatrix());
}

void CGameObjectModel::Notify(void * pMessage)
{

}

void CGameObjectModel::Compute_DOF_Parameters()
{

}

_int CGameObjectModel::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CGameObjectModel::Update_Collider()
{
	if (nullptr == m_pTransform)
	{
		return E_FAIL;
	}
	if (nullptr != m_pHitBox)
	{
		m_pHitBox->Update(m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pAttackBox)
	{
		m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pMiddlePoint)
	{
		m_pMiddlePoint->Update(m_pTransform->Get_WorldMatrix());
	}
	return S_OK;
}

HRESULT CGameObjectModel::Update_UI(_double dDeltaTime)
{
	return S_OK;
}
