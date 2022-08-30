#include "stdafx.h"
#include "..\Public\Gauntlet.h"
#include "UI_HUD_Damage.h"
#include "CameraMgr.h"

CGauntlet::CGauntlet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CGauntlet::CGauntlet(const CGauntlet & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CGauntlet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGauntlet::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iAnimation = 1;
	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_bCollision = true;

	m_fForce = 1.3f;
	m_tObjectStatusDesc.iAtt = 30;

	return S_OK;
}

_int CGauntlet::Tick(_double TimeDelta)
{
	if (nullptr == m_pModel)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Set_Animation(m_iAnimation, false);
	m_pModel->Synchronize_Animation(m_pTransform);

	Update_HitType();

	Condition_UpdateCollider();
	Update_Collider();
	Check_Pause();
	return (_int)m_eState;
}

_int CGauntlet::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (WEAPONEQUIP::EQUIP != m_eWeaponEquip)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	return _int();
}

HRESULT CGauntlet::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, 1);
	}

	return S_OK;
}

void CGauntlet::Update_HitType()
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

			m_eHitType = HIT::SINGLE;

			if (13 == m_pModel->Get_CurrentAnimation())
			{
				if (0.016 <= m_pModel->Get_PlayTimeRatio() && 0.033 > m_pModel->Get_PlayTimeRatio())				// 1번째 타격 시간 비율
					m_iTargetAnimation = 1000;
				else if (0.033 <= m_pModel->Get_PlayTimeRatio() && 0.058 > m_pModel->Get_PlayTimeRatio())			// 2번째 타격 시간 비율
					m_iTargetAnimation = 1001;
				else if (0.058 <= m_pModel->Get_PlayTimeRatio() && 0.082 > m_pModel->Get_PlayTimeRatio())			// 3번째 타격 시간 비율
					m_iTargetAnimation = 1002;
				else if (0.082 <= m_pModel->Get_PlayTimeRatio() && 0.125 > m_pModel->Get_PlayTimeRatio())			// 4번째 타격 시간 비율
					m_iTargetAnimation = 1003;
				else if (0.125 <= m_pModel->Get_PlayTimeRatio())													// 5번째 타격 시간 비율
					m_iTargetAnimation = 1004;
			}
			else if (22 == m_pModel->Get_CurrentAnimation())
			{
				if (0.083 <= m_pModel->Get_PlayTimeRatio() && 0.099 > m_pModel->Get_PlayTimeRatio())				// 1번째 타격 시간 비율
					m_iTargetAnimation = 1005;						// 임시 초기화 (안쓰는 애니메이션)
				else if (0.099 <= m_pModel->Get_PlayTimeRatio() && 0.116 > m_pModel->Get_PlayTimeRatio())			// 2번째 타격 시간 비율
					m_iTargetAnimation = 1006;
				else if (0.116 <= m_pModel->Get_PlayTimeRatio() && 0.14 > m_pModel->Get_PlayTimeRatio())			// 3번째 타격 시간 비율
					m_iTargetAnimation = 1007;
				else if (0.14 <= m_pModel->Get_PlayTimeRatio() && 0.15 > m_pModel->Get_PlayTimeRatio())				// 4번째 타격 시간 비율
					m_iTargetAnimation = 1008;
				else if (0.15 <= m_pModel->Get_PlayTimeRatio() && 0.19 > m_pModel->Get_PlayTimeRatio())				// 5번째 타격 시간 비율
					m_iTargetAnimation = 1009;
				else if (0.19 <= m_pModel->Get_PlayTimeRatio() && 0.235 > m_pModel->Get_PlayTimeRatio())			// 6번째 타격 시간 비율
					m_iTargetAnimation = 1010;
				else if (0.235 <= m_pModel->Get_PlayTimeRatio())													// 7번째 타격 시간 비율
					m_iTargetAnimation = 1011;
			}
			else if (33 == m_pModel->Get_CurrentAnimation())
			{
				if (0.135 <= m_pModel->Get_PlayTimeRatio())															// 1번째 타격 시간 비율
					m_iTargetAnimation = 1012;
			}
			else if (34 == m_pModel->Get_CurrentAnimation())
			{
				if (0.01 <= m_pModel->Get_PlayTimeRatio() && 0.035 > m_pModel->Get_PlayTimeRatio())					// 1번째 타격 시간 비율
					m_iTargetAnimation = 1013;					
				else if (0.035 <= m_pModel->Get_PlayTimeRatio() && 0.05 > m_pModel->Get_PlayTimeRatio())			// 2번째 타격 시간 비율	
					m_iTargetAnimation = 1014;
				else if (0.05 <= m_pModel->Get_PlayTimeRatio() && 0.06 > m_pModel->Get_PlayTimeRatio())				// 3번째 타격 시간 비율
					m_iTargetAnimation = 1015;
				else if (0.06 <= m_pModel->Get_PlayTimeRatio() && 0.068 > m_pModel->Get_PlayTimeRatio())			// 4번째 타격 시간 비율
					m_iTargetAnimation = 1016;
				else if (0.068 <= m_pModel->Get_PlayTimeRatio() && 0.075 > m_pModel->Get_PlayTimeRatio())			// 5번째 타격 시간 비율
					m_iTargetAnimation = 1017;
				else if (0.075 <= m_pModel->Get_PlayTimeRatio() && 0.082 > m_pModel->Get_PlayTimeRatio())			// 6번째 타격 시간 비율	
					m_iTargetAnimation = 1018;
				else if (0.082 <= m_pModel->Get_PlayTimeRatio() && 0.104 > m_pModel->Get_PlayTimeRatio())			// 7번째 타격 시간 비율
					m_iTargetAnimation = 1019;
				else if (0.104 <= m_pModel->Get_PlayTimeRatio())													// 8번째 타격 시간 비율
					m_iTargetAnimation = 1020;	
			}
			else if (36 == m_pModel->Get_CurrentAnimation())
			{
				if (0.105 <= m_pModel->Get_PlayTimeRatio())															// 1번째 타격 시간 비율
					m_iTargetAnimation = 1021;
			}
		}
	}
}

HRESULT CGauntlet::Update_Collider()
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

void CGauntlet::Condition_UpdateCollider()
{
	if (1000 <= m_iTargetAnimation)
	{
		if (1013 <= m_iTargetAnimation)
		{
			if (1016 >= m_iTargetAnimation)
			{
				if (1013 == m_iTargetAnimation)
					m_pBoneTag = L"Left";
				else if (1014 == m_iTargetAnimation)
					m_pBoneTag = L"Right";
				else if (1015 == m_iTargetAnimation)
					m_pBoneTag = L"Left";
				else if (1016 == m_iTargetAnimation)
					m_pBoneTag = L"Right";		
			}
			else
			{
				if (1017 == m_iTargetAnimation)
					m_pBoneTag = L"Left";
				else if (1018 == m_iTargetAnimation)
					m_pBoneTag = L"Right";
				else if (1019 == m_iTargetAnimation)
					m_pBoneTag = L"Left";
				else if (1020 == m_iTargetAnimation)
					m_pBoneTag = L"Right";
				else if (1021 == m_iTargetAnimation)
					m_pBoneTag = L"Right";
			}
		}
		else if (1012 == m_iTargetAnimation)
		{
			m_pBoneTag = L"Left";
		}
		else if (1005 <= m_iTargetAnimation)
		{
			if (1005 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
			else if (1006 == m_iTargetAnimation)
				m_pBoneTag = L"Right";
			else if (1007 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
			else if (1008 == m_iTargetAnimation)
				m_pBoneTag = L"Right";
			else if (1009 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
			else if (1010 == m_iTargetAnimation)
				m_pBoneTag = L"Right";
			else if (1011 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
		}
		else if (1004 >= m_iTargetAnimation)
		{
			if (1000 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
			else if (1001 == m_iTargetAnimation)
				m_pBoneTag = L"Right";
			else if (1002 == m_iTargetAnimation)
				m_pBoneTag = L"Left";
			else if (1003 == m_iTargetAnimation)
				m_pBoneTag = L"Right";
			else if (1004 == m_iTargetAnimation)
				m_pBoneTag = L"Left";				// 양손 (임시)
		}
	}
}

void CGauntlet::Update_Matrix(_matrix Matrix)
{
	m_pTransform->Set_WorldMatrix(Matrix);
}

void CGauntlet::Set_Immediately_Animation(_uint iIndex)
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

void CGauntlet::Set_NoneBlend(_uint iIndex)
{
	m_iAnimation = iIndex;

	m_pModel->Set_NoneBlend();
	m_pModel->Set_Animation(iIndex);
}

void CGauntlet::Set_WeaponEquip(WEAPONEQUIP eEquip)
{
	m_eWeaponEquip = eEquip;
}

_bool CGauntlet::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CGauntlet::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		/*
		1. pGameObject의 정보를 받아서 최종 데미지를 계산
		2. 계산한 데미지를 pGameObject에게 반환
		3. 계산한 데미지와 충돌이 발생한 지점을 UI_Damage에 전달
		*/
		_double PlayRatio = m_pModel->Get_PlayTimeRatio();

		if ((m_pModel->Get_CollisionMinRatio() <= PlayRatio && m_pModel->Get_CollisionMaxRatio() >= PlayRatio) || 1000 <= m_iTargetAnimation)
		{
			if (m_eWeaponState != WEAPONSTATE::IDLE && m_eWeaponState != WEAPONSTATE::NONE)
			{
				_uint iDamage = 0;
				if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage))
				{
					pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

					CUI_HUD_Damage::UIDAMAGEDESC	desc;

					desc.eType = CUI_HUD_Damage::DAMAGETYPE::MELEE;

					desc.iDamageAmount = iDamage;

					XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));

					CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

					CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

					pCamMgr->Set_MainCam_ShakeGauntlet_True();

					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		/* 칼이 총알과 부딪혔을 때 총알을 지우는 용도로 쓸 수 있지 않을까? */
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CGauntlet::Notify(void * pMessage)
{
}

_int CGauntlet::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CGauntlet::SetUp_Components()
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

	m_pModel->Add_RefNode(L"Left", "bone032");	// 왼쪽 건틀릿
	m_pModel->Add_RefNode(L"Right", "bone016"); // 오른쪽 건틀릿

	return S_OK;
}

HRESULT CGauntlet::SceneChange_AmusementPark(const _vector & vPosition, const _uint & iNavigationIndex)
{
	return S_OK;
}

HRESULT CGauntlet::SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex)
{
	return S_OK;
}

HRESULT CGauntlet::SceneChange_ZhuangziStage(const _vector & vPosition, const _uint & iNavigationIndex)
{
	return S_OK;
}

HRESULT CGauntlet::SceneChange_ROBOTGENERAL(const _vector & vPosition, const _uint & iNavigationIndex)
{
	return S_OK;
}

CGauntlet * CGauntlet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGauntlet* pGameInstance = new CGauntlet(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CGauntlet");
	}

	return pGameInstance;
}

CGameObject * CGauntlet::Clone(void * pArg)
{
	CGauntlet* pGameInstance = new CGauntlet(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CGauntlet");
	}

	return pGameInstance;
}

void CGauntlet::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pAttackBox);
}
