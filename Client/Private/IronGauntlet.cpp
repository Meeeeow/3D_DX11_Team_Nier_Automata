#include "stdafx.h"
#include "..\Public\IronGauntlet.h"
#include "UI_HUD_Damage.h"
#include "MinigameManager.h"
#include "Iron_Android9S.h"
#include "Iron_Humanoid_Small.h"

CIronGauntlet::CIronGauntlet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CIronGauntlet::CIronGauntlet(const CIronGauntlet & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CIronGauntlet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CIronGauntlet::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iAnimation = 1;
	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_bCollision = true;

	m_fForce = 1.3f;
	m_tObjectStatusDesc.iAtt = 5;

	CMinigameManager* pManager = CMinigameManager::Get_Instance();

	CIron_Android9S* pObject = nullptr;
	if (nullptr != pArg)
		pObject = (CIron_Android9S*)pArg;

	pObject->Set_Gauntlet(this);

	return S_OK;
}

_int CIronGauntlet::Tick(_double TimeDelta)
{
	if (nullptr == m_pModel)
		return -1;

	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Set_Animation(m_iAnimation, false);
	m_pModel->Synchronize_Animation(m_pTransform);

	Update_HitType();

	Condition_UpdateCollider();
	Update_Collider();
	Check_Pause();

	return (_int)m_eState;
}

_int CIronGauntlet::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CIronGauntlet::Render()
{
	return S_OK;
}

void CIronGauntlet::Update_HitType()
{
	if (m_pModel)
	{
		if (!m_pModel->Get_Blend())
		{
			m_iTargetAnimation = m_pModel->Get_CurrentAnimation();

			if (1 == m_iTargetAnimation)
			{
				m_eHitType = HIT::NONE;
				return;
			}

			_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
			_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

			if (CollMinRatio <= PlayTimeRatio && CollMaxRatio > PlayTimeRatio)
				m_eHitType = HIT::SINGLE;
			else
				m_eHitType = HIT::NONE;
		}
	}
}

HRESULT CIronGauntlet::Update_Collider()
{
	if (nullptr == m_pTransform || nullptr == m_pModel)
	{
		return E_FAIL;
	}
	if (nullptr != m_pHitBox)
	{
		m_pHitBox->Update(m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pAttackBox)
	{
		m_pAttackBox->Update(m_pModel->Get_BoneMatrix(m_pBoneTag) * m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pMiddlePoint)
	{
		m_pMiddlePoint->Update(m_pTransform->Get_WorldMatrix());
	}
	return S_OK;
}

void CIronGauntlet::Condition_UpdateCollider()
{
}

void CIronGauntlet::Update_Matrix(_matrix Matrix)
{
	m_pTransform->Set_WorldMatrix(Matrix);
}

void CIronGauntlet::Set_Immediately_Animation(_uint iIndex)
{
	if (iIndex == m_pModel->Get_CurrentAnimation())
		return;

	m_pModel->Initialize_RootMotion();
	m_pModel->Initialize_Time();
	m_pModel->Initialize_Blend();

	m_iAnimation = iIndex;
	m_pModel->Set_CurrentAnimation(iIndex);
	m_pModel->Set_NextAnimation(iIndex);
}

void CIronGauntlet::Set_NoneBlend(_uint iIndex)
{
	m_iAnimation = iIndex;

	m_pModel->Set_NoneBlend();
	m_pModel->Set_Animation(iIndex);
}

_bool CIronGauntlet::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pAttackBox->Collision_SphereToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::SPHERE)
	{
		if (m_pAttackBox->Collision_SphereToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}
	return false;
}

void CIronGauntlet::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		_double PlayRatio = m_pModel->Get_PlayTimeRatio();

		if ((m_pModel->Get_CollisionMinRatio() <= PlayRatio && m_pModel->Get_CollisionMaxRatio() >= PlayRatio))
		{
			if (m_eWeaponState != WEAPONSTATE::IDLE && m_eWeaponState != WEAPONSTATE::NONE)
			{
				_uint iDamage = 0;
				if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage))
				{
					CIron_Humanoid_Small* pMonster = static_cast<CIron_Humanoid_Small*>(pGameObject);
					if (CIron_Humanoid_Small::ANIMSTATE::GUARD == pMonster->Get_AnimState())
						iDamage = 0;

					pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

					CUI_HUD_Damage::UIDAMAGEDESC	desc;

					desc.eType = CUI_HUD_Damage::DAMAGETYPE::MELEE;

					desc.iDamageAmount = iDamage;

					XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));

					CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CIronGauntlet::Notify(void * pMessage)
{
}

_int CIronGauntlet::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CIronGauntlet::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_GAUNTLET, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::DESC		ColliderDesc;

	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.vOrientation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &ColliderDesc)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Left", "bone032");	// ¿ÞÂÊ °ÇÆ²¸´
	m_pModel->Add_RefNode(L"Right", "bone016"); // ¿À¸¥ÂÊ °ÇÆ²¸´

	return S_OK;
}

CIronGauntlet * CIronGauntlet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CIronGauntlet* pGameInstance = new CIronGauntlet(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CIronGauntlet");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CIronGauntlet::Clone(void * pArg)
{
	CIronGauntlet* pGameInstance = new CIronGauntlet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CIronGauntlet");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CIronGauntlet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pAttackBox);
}
