#include "stdafx.h"
#include "..\Public\Humanoid_Big.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "Humanoid_Bullet.h"
#include "EffectFactory.h"
#include "Normal_Attack.h"
#include "SoundMgr.h"
#include "MeshEffect.h"
#include "Level_Stage1.h"
#include "LevelManager.h"

CHumanoid_Big::CHumanoid_Big(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CHumanoid_Big::CHumanoid_Big(const CHumanoid_Big & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CHumanoid_Big::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHumanoid_Big::NativeConstruct(void * pArg)
{
	//MO_INFO* tInfo = (MO_INFO*)pArg;

	//if (FAILED(Add_Components((_uint)LEVEL::STATIC, tInfo->szModelComName, COM_KEY_MODEL, (CComponent**)&m_pModel)))
	//	return E_FAIL;

	//m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo->m_matWorld));

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_ePattern = M_PATTERN::PATTERN_1;

	m_eState = OBJSTATE::ENABLE;

	m_bCollision = true;
	if (pArg != nullptr)
	{
		MO_INFO* pInfo = (MO_INFO*)pArg;
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	}

	else
		m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	static _uint iCHumanoid_Big_InstanceID = OriginID_HumanoidBig;
	m_iObjID = iCHumanoid_Big_InstanceID++;

	m_tObjectStatusDesc.fHp = 500;
	m_tObjectStatusDesc.fMaxHp = 500;
	m_tObjectStatusDesc.iAtt = 10;

	m_fAlphaOneTextureDuration = 0;
	Check_Pause();

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	return S_OK;
}

_int CHumanoid_Big::Tick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (!m_bStart)
		dTimeDelta = 0.0;

	Update_UI(dTimeDelta);

	if (m_eState == OBJSTATE::DEAD)
	{
		Create_DeathEffect();

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_uint iLevel = pGameInstance->Get_LevelID();

		if (iLevel == (_uint)LEVEL::STAGE1)
		{
			CLevel_Stage1* pLevel_1 = (CLevel_Stage1*)pGameInstance->Get_CurrentLevel();

			if (m_eWave == WAVE::WAVE0)
			{
				pLevel_1->Minus_MonsterCount_W0();
			}

			else if (m_eWave == WAVE::WAVE1)
			{
				pLevel_1->Minus_MonsterCount_W1();
			}

			else if (m_eWave == WAVE::WAVE2)
			{
				pLevel_1->Minus_MonsterCount_W2();
			}
		}

		return (_int)m_eState;
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	Play_Pattern(dTimeDelta);

	Update_Bone();

	Update_Collider();

	Update_NormalAttack();

	Update_CollisionTime(dTimeDelta);

	Update_Effect(static_cast<_float>(dTimeDelta));

	Update_FocusTime(dTimeDelta);

	return (_int)m_eState;
}

_int CHumanoid_Big::LateTick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		m_eState = OBJSTATE::DEAD;
	}
	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		if(m_bStart)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	
	Check_Pattern_Cool(dTimeDelta);

	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

HRESULT CHumanoid_Big::Render()
{
	if (m_bStart)
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

	}

	return S_OK;
}

HRESULT CHumanoid_Big::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Humanoid_Big", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Bullet_Hole", "bone305");

	//_vector vPos = { 8.f , 0.f , 8.f , 1.f };

	//m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);



	CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	LEVEL eLevelID = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	if (eLevelID == LEVEL::LOADING)
		eLevelID = static_cast<LEVEL>(pGameInstance->Get_NextLevelID());

	switch (eLevelID)
	{
	case Client::LEVEL::STAGE1:
		if (FAILED(Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	case Client::LEVEL::ROBOTMILITARY:
		break;
	case Client::LEVEL::AMUSEMENTPARK:
		if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	case Client::LEVEL::OPERABACKSTAGE:
		if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	default:
		break;
	}

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_ElectricityEffect"), L"Com_AlphaOneTexture", (CComponent**)&m_pAlphaOneTexture)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_0_21_255.dds"), L"Com_Color", (CComponent**)&m_pColorTexture)))
		return E_FAIL;


	return S_OK;
}

HRESULT CHumanoid_Big::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();

	if (m_fAlphaOneTextureDuration > 0.f)
	{
		m_pModel->SetUp_Texture("g_texAlphaOne", m_pAlphaOneTexture->Get_SRV(m_iAlphaOneTextureIndex));
		m_pModel->SetUp_Texture("g_texColor", m_pColorTexture->Get_SRV(0));
	}


	return S_OK;
}

void CHumanoid_Big::Chase(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::WALK || m_eAnimState == M_ANIMSTATE::ENGAGE || m_eAnimState == M_ANIMSTATE::SHOOTING 
		|| !m_bLaserOnce)
	{
		if (m_fDistance > 1.f)
		{
			m_fRatio += (_float)(TimeDelta * 0.5f);


			_vector vTargetPos = Get_TargetPosition();
			_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			_float fTargetY = XMVectorGetY(vTargetPos);
			vMonsterPos = XMVectorSetY(vMonsterPos, fTargetY);

			_vector vMonsterRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
			_vector vMonsterUp = { 0.f , 1.f , 0.f , 0.f };
			_vector vScale = m_pTransform->Get_Scale();

			_vector vMonsterLook = vTargetPos - vMonsterPos;

			_vector vCurLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

			vMonsterLook = XMVector3Normalize(vMonsterLook);
			vCurLook = XMVector3Normalize(vCurLook);

			_vector vLerpLook;
			vLerpLook = XMVectorLerp(vCurLook, vMonsterLook, m_fRatio);
			vLerpLook = XMVector3Normalize(vLerpLook);
			vMonsterRight = XMVector3Cross(vMonsterUp, vLerpLook);
			vMonsterUp = XMVector3Cross(vLerpLook, vMonsterRight);

			vLerpLook = vLerpLook * XMVectorGetZ(vScale);
			vMonsterRight = vMonsterRight * XMVectorGetX(vScale);
			vMonsterUp = vMonsterUp * XMVectorGetY(vScale);

			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
			m_pTransform->Set_State(CTransform::STATE::RIGHT, vMonsterRight);
			m_pTransform->Set_State(CTransform::STATE::UP, vMonsterUp);

			if (m_fRatio >= 1.f)
			{
				m_fRatio = 1.f;
			}
		}

		else
			m_fRatio = 0.f;
	}
}

void CHumanoid_Big::Pattern_1(_double TimeDelta)
{
	Check_Target1(TimeDelta);
	Chase(TimeDelta);
	Check_AnimState1(TimeDelta);
}

void CHumanoid_Big::Check_Target1(_double TimeDelta)
{

	_vector vTargetPos = Get_TargetPosition();
	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vTargetPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;
	
	if (m_bJump == false && m_eAnimState != M_ANIMSTATE::DROP && m_eAnimState != M_ANIMSTATE::DROPING && m_eAnimState != M_ANIMSTATE::LANDING)
	{

		if (fDist <= 20.f && fDist >= 6.f)
		{
			m_bRangeAttack = true;
			m_bEngage = false;
		}
		if (fDist < 6.f && fDist >= 2.f)
		{
			if (m_eAnimState != M_ANIMSTATE::SHOOTING)
				m_bRangeAttack = false;

			m_bEngage = true;
		}
		if ((fDist <2.f || fDist > 20.f) && m_eAnimState != M_ANIMSTATE::ENGAGE && m_eAnimState != M_ANIMSTATE::BACKJUMP)
		{
			m_bRangeAttack = false;
			m_bEngage = false;
			m_eAnimState = M_ANIMSTATE::IDLE;
		}

		if ((fDist <= 20.f && fDist >= 4.f && !m_bShootCool && m_eAnimState != M_ANIMSTATE::BACKSTEP && m_eAnimState != M_ANIMSTATE::ENGAGE) || m_eAnimState == M_ANIMSTATE::SHOOTING)
		{
			m_eAnimState = M_ANIMSTATE::SHOOTING;

			if (m_iAnimIndex != BIG_FIRE_MINI && m_iAnimIndex != BIG_FIRE_BIG)
				m_iAnimIndex = BIG_FIRE_READY;

			if ((0.98 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == BIG_FIRE_READY)
			{
				m_bRangeAttack = true;
				m_iAnimIndex = BIG_FIRE_MINI;
			}
			if (m_iAnimIndex == BIG_FIRE_MINI)
			{
				m_dBulletTime += TimeDelta;

				if (m_iNumBullet >= 7)
				{
					m_iNumBullet = 0;
					m_iAnimIndex = BIG_FIRE_BIG;
				}
				if (m_dBulletTime >= 0.3 && m_bRangeAttack)
				{
					Shoot_Bullet();


					//pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_EFFECT_MONSTER_BULLET_SMALL, &desc));
					//CHumanoid_Bullet* pBullet = nullptr;
					//if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_GameObject_Humanoid_Bullet", L"Monster_Bullet"
					//	, (CGameObject**)&pBullet, &tInfo)))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return;
					//}
					//RELEASE_INSTANCE(CGameInstance);

					m_dBulletTime = 0.0;
					++m_iNumBullet;

				}
			}
			if (m_iAnimIndex == BIG_FIRE_BIG)
			{
				if (m_pModel->Get_LinkMaxRatio() < m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_FIRE_BIG && !m_bLaserOnce)
				{
					Shoot_Bullet_Big();

					//_float4x4 pMatrix;
					//XMStoreFloat4x4(&pMatrix, m_pModel->Get_BoneMatrix(L"Bullet_Hole") * m_pTransform->Get_WorldMatrix());
					//_bool bLaser = true;
					//MonsterBulletInfo tInfo;
					//tInfo.Matrix = pMatrix;
					//tInfo.bLaser = bLaser;
					//CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
					//CHumanoid_Bullet* pBullet = nullptr;
					//if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_GameObject_Humanoid_Bullet", L"Monster_Bullet"
					//	, (CGameObject**)&pBullet, &tInfo)))
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return;
					//}
					//RELEASE_INSTANCE(CGameInstance);
					m_bLaserOnce = true;
				}

				if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_FIRE_BIG)
				{
					m_bShootCool = true;
					m_bRangeAttack = false;
					m_eAnimState = M_ANIMSTATE::BACKSTEP;
					m_iAnimIndex = BIG_BACKSTEP;
					m_bLaserOnce = false;

				}
			}

		}

		if ((fDist < 6.f && fDist >= 1.f && !m_bMeleeCool && m_eAnimState != M_ANIMSTATE::BACKJUMP && m_eAnimState != M_ANIMSTATE::SHOOTING)
			|| m_eAnimState == M_ANIMSTATE::ENGAGE)
		{
			if (m_iAnimIndex != BIG_MELEE && m_bCreateOnce)
			{
				m_bCreateOnce = false;
				m_bAtkDead = false;
			}
			m_iAnimIndex = BIG_MELEE;
			m_eAnimState = M_ANIMSTATE::ENGAGE;
			m_iNumBullet = 0;
			m_dBulletTime = 0.0;
			m_bLaserOnce = false;

			if (!m_bCreateOnce && m_pModel->Get_CurrentAnimation() == BIG_MELEE && (m_pModel->Get_LinkMinRatio() + 0.14f <= m_pModel->Get_PlayTimeRatio()))
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				Humanoid_Atk_Info tInfo;

				tInfo.iNumber = 4;
				tInfo.Matrix = m_MeleePos;
				tInfo.pMaker = this;

				m_AttackMatrix = m_MeleePos;

				CNormal_Attack* pAttack = nullptr;
				if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
					, (CGameObject**)&pAttack, &tInfo)))
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}
				m_vecNormalAttacks.push_back(pAttack);

				Safe_AddRef(pAttack);

				RELEASE_INSTANCE(CGameInstance);

				m_bCreateOnce = true;
			}

			if (m_pModel->Get_CurrentAnimation() == BIG_MELEE && m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio() && m_iAnimIndex == BIG_MELEE)
				m_bAtkDead = true;

			if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_MELEE && !m_bBackJumpCool)
			{
				m_iAnimIndex = BIG_BACKJUMP;
				m_eAnimState = M_ANIMSTATE::BACKJUMP;
				m_bMeleeCool = true;
			}
		}

		if (m_eAnimState == M_ANIMSTATE::BACKJUMP)
		{

			m_iAnimIndex = BIG_BACKJUMP;
			if (0.9 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_BACKJUMP)
			{
				m_iAnimIndex = BIG_IDLE;
				m_eAnimState = M_ANIMSTATE::IDLE;
				m_bBackJumpCool = true;
			}
		}

		if (m_eAnimState == M_ANIMSTATE::BACKSTEP)
		{
			m_iAnimIndex = BIG_BACKSTEP;
			m_dBackStepTime += TimeDelta;

			m_iNumBullet = 0;
			m_dBulletTime = 0.0;
			m_bLaserOnce = false;
			if (m_dBackStepTime >= 2.f && 0.9 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_BACKSTEP)
			{
				m_iAnimIndex = BIG_IDLE;
				m_dBackStepTime = 0.f;
				m_eAnimState = M_ANIMSTATE::IDLE;
			}
		}
	}

	if (m_bJump)
	{
		m_eAnimState = M_ANIMSTATE::DROPING;
		m_iAnimIndex = BIG_DROPING;

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fY = 0.f;

		_float fPosY = XMVectorGetY(vPos);
		fPosY -= 20.f * (_float)TimeDelta;
		vPos = XMVectorSetY(vPos, fPosY);
		m_pTransform->Set_Jump(true);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			vPos = XMVectorSetY(vPos, fY);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			m_pTransform->Set_Jump(false);

			m_iAnimIndex = BIG_LANDING;
			m_eAnimState = M_ANIMSTATE::LANDING;

			m_bJump = false;
		}
	}

	else if (m_eAnimState == M_ANIMSTATE::LANDING)
	{
		m_iAnimIndex = BIG_LANDING;

		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == BIG_LANDING)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = BIG_IDLE;
		}
	}

	if (m_eAnimState == M_ANIMSTATE::IDLE)
	{
		m_iAnimIndex = BIG_IDLE;
		m_iNumBullet = 0;
		m_dBulletTime = 0.0;
		m_bLaserOnce = false;
	}

	if (m_iAnimIndex == BIG_IDLE || m_iAnimIndex == BIG_FIRE_MINI || m_iAnimIndex == BIG_FIRE_BIG || m_iAnimIndex == BIG_BACKSTEP || m_iAnimIndex == BIG_DROPING)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Big::Check_AnimState1(_double TimeDelta)
{
}

void CHumanoid_Big::Check_Times1(_double TimeDelta)
{
	if (m_bShootCool)
	{
		m_dShootCoolTime += TimeDelta;

		if (m_dShootCoolTime >= 6.f)
		{
			m_dShootCoolTime = 0.0;
			m_bShootCool = false;
		}
	}

	if (m_bMeleeCool)
	{
		m_dMeleeCoolTime += TimeDelta;

		if (m_dMeleeCoolTime >= 2.f)
		{
			m_dMeleeCoolTime = 0.0;
			m_bMeleeCool = false;
		}
	}
	if (m_bBackJumpCool)
	{
		m_dBackJumpTime += TimeDelta;

		if (m_dBackJumpTime >= 3.0)
		{
			m_dBackJumpTime = 0.0;
			m_bBackJumpCool = false;
		}
	}
}

void CHumanoid_Big::Update_Bone()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Bullet_Hole");
	_matrix WorldMatrix = BoneMatrix * m_pTransform->Get_WorldMatrix();

	_float4x4 TempFloat4x4;
	XMStoreFloat4x4(&TempFloat4x4, WorldMatrix);

	_float4 vPos, vRight, vUp, vLook;
	memcpy(&vPos, &TempFloat4x4.m[3], sizeof(_float4));
	memcpy(&vRight, &TempFloat4x4.m[0], sizeof(_float4));
	memcpy(&vUp, &TempFloat4x4.m[1], sizeof(_float4));
	memcpy(&vLook, &TempFloat4x4.m[2], sizeof(_float4));

	_vector vNormRight, vNormUp, vNormLook, vecPos;
	vecPos = XMLoadFloat4(&vPos);
	vNormRight = XMVector3Normalize(XMLoadFloat4(&vRight));
	vNormUp = XMVector3Normalize(XMLoadFloat4(&vUp));
	vNormLook = XMVector3Normalize(XMLoadFloat4(&vLook));

	vecPos = vecPos + vNormRight * 0.f;
	vecPos = vecPos + vNormUp * 0.f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_MeleePos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_MeleePos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_MeleePos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_MeleePos.m[3], &vecPos, sizeof(_vector));

	m_AttackMatrix = m_MeleePos;
}

void CHumanoid_Big::Update_NormalAttack()
{
	if (true == m_bAtkDead)
	{
		for (auto& pAttack : m_vecNormalAttacks)
		{
			pAttack->Set_State(OBJSTATE::DEAD);
			Safe_Release(pAttack);
		}
		m_vecNormalAttacks.clear();

		return;
	}

	if (false == m_vecNormalAttacks.empty())
	{
		for (auto& pAttack : m_vecNormalAttacks)
		{
			CTransform*	pAttackTransform = (CTransform*)pAttack->Get_Component(COM_KEY_TRANSFORM);

			if (nullptr != pAttackTransform)
			{
				pAttackTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_AttackMatrix));
			}
		}
	}
}

void CHumanoid_Big::Update_FocusTime(_double TimeDelta)
{
	if (m_bFocus)
	{
		m_FocusTime += TimeDelta;

		if (m_FocusTime >= m_FocusDelayTime)
		{
			m_bFocus = false;
			m_FocusTime = 0.0;
		}
	}
}

_fvector CHumanoid_Big::Get_TargetPosition()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_vector vZero = XMVectorZero();

	if (!m_bFocus)
	{
		CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			MSGBOX("Failed CHumanoid_Big::Get_TargetPosition() - LayerPlayer, Com_Transform");
			return vZero;
		}

		RELEASE_INSTANCE(CGameInstance);

		return pTransform->Get_State(CTransform::STATE::POSITION);
	}
	else
	{
		CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"Android_9S", L"Com_Transform", 0);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			MSGBOX("Failed CHumanoid_Big::Get_TargetPosition() - Android_9S, Com_Transform");
			return XMVectorZero();
		}

		RELEASE_INSTANCE(CGameInstance);

		return pTransform->Get_State(CTransform::STATE::POSITION);
	}

	MSGBOX("Failed CHumanoid_Big::Get_TargetPosition() - else");
	return vZero;
}

_bool CHumanoid_Big::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::SPHERE)
	{
		if (m_pHitBox->Collision_AABBToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::RAY)
	{
		if (pGameObject->Get_TargetAnimation() == m_iTargetAnimation)
			return false;

		_float fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		if (m_pHitBox->Collision_AABB(pEffect->Get_RayOrigin(), pEffect->Get_RayDirection(), fDist))
		{
			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	return false;
}

void CHumanoid_Big::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		if (HIT::NONE != pGameObject->Get_HitType())
		{
			m_iFlagUI |= FLAG_UI_CALL_HPBAR;
			m_dCallHPBarUITimeDuration = 3.0;
			//m_fAlphaOneTextureDuration = ROBOT_HIT_EFFECT_DURATION;
		}
	}
	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (HIT::NONE != pGameObject->Get_HitType())
		{
			m_iFlagUI |= FLAG_UI_CALL_HPBAR;
			m_dCallHPBarUITimeDuration = 3.0;

			m_iTargetAnimation = pGameObject->Get_TargetAnimation();
		}
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CHumanoid_Big::Play_Pattern(_double dTimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Pattern_1(dTimeDelta);
	}
}

void CHumanoid_Big::Check_Pattern_Cool(_double TimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Check_Times1(TimeDelta);
	}
}

void CHumanoid_Big::Notify(void * pMessage)
{
}

_int CHumanoid_Big::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CHumanoid_Big::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dDeltaTime;

		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::COMMON;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MONSTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MONSTER_HPBAR, &desc));

		RELEASE_INSTANCE(CGameInstance);

		if (m_dCallHPBarUITimeDuration < 0)
		{
			m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
		}
	}
	if (m_eAnimState != CHumanoid_Big::M_ANIMSTATE::IDLE)
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eObjState = m_eState;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::ENEMY;
		desc.dTimeDuration = CUI_HUD_DirectionIndicator::DURATION_ENEMY;

		XMStoreFloat4(&desc.vTargetWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));

		RELEASE_INSTANCE(CGameInstance);
	}
	return S_OK;
}

HRESULT CHumanoid_Big::Update_Effect(_float fTimeDelta)
{
	m_fAlphaOneTextureDuration -= fTimeDelta;

	if (m_fAlphaOneTextureDuration > 0)
	{
		m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM_ROBOT;
		m_fAlphaOneTextureElapsedTime += fTimeDelta;
		if (m_fAlphaOneTextureElapsedTime > 0.05f)
		{
			m_fAlphaOneTextureElapsedTime = 0.f;
			if (++m_iAlphaOneTextureIndex > 58)
			{
				m_iAlphaOneTextureIndex = 0;
			}
		}
	}
	else
	{
		m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;
	}

	return S_OK;
}

HRESULT CHumanoid_Big::Shoot_Bullet()
{
	_float4x4 pMatrix;
	XMStoreFloat4x4(&pMatrix, m_pModel->Get_BoneMatrix(L"Bullet_Hole") * m_pTransform->Get_WorldMatrix());

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;


	_vector vecDir = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_float3 vDir;
	XMStoreFloat3(&vDir, vecDir);
	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	memcpy(&vecPos, pMatrix.m[3], sizeof(_float4));
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	pEffectBulletPacket.iNavIndex = m_pNavigation->Get_Index();
	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = MONSTER_BULLET_SPEED;
	pEffectBulletPacket.tEffectMovement.vDirection = vDir;
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
	pEffectBulletPacket.fMaxLifeTime = MONSTER_BULLET_MAXLIFE;
	CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &pEffectBulletPacket);

	return S_OK;

}

HRESULT CHumanoid_Big::Shoot_Bullet_Big()
{
	_float4x4 pMatrix;
	XMStoreFloat4x4(&pMatrix, m_pModel->Get_BoneMatrix(L"Bullet_Hole") * m_pTransform->Get_WorldMatrix());

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;


	_vector vecDir = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_float3 vDir;
	XMStoreFloat3(&vDir, vecDir);
	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	memcpy(&vecPos, pMatrix.m[3], sizeof(_float4));
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = MONSTER_BULLET_SPEED;
	pEffectBulletPacket.tEffectMovement.vDirection = vDir;
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET_BIG;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET_BIG;
	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ELECTRIC_DONUT;
	pEffectBulletPacket.fMaxLifeTime = MONSTER_BULLET_MAXLIFE;
	CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet_Big.dat", &pEffectBulletPacket);
	//CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut.dat", &pEffectBulletPacket);


	return S_OK;
}

HRESULT CHumanoid_Big::Create_DeathEffect()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;


	_vector vecDir = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_float3 vDir;
	XMStoreFloat3(&vDir, vecDir);
	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = 0.f;
	pEffectBulletPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ROBOT_DEATH_EXPLOSION_SPARK4;
	pEffectBulletPacket.fMaxLifeTime = -1.f;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Robot_Death_Explosion_Spark4.dat", &pEffectBulletPacket);

	return S_OK;
}

CHumanoid_Big * CHumanoid_Big::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CHumanoid_Big* pInstance = new CHumanoid_Big(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CHumanoid_Big::Clone(void * pArg)
{
	CHumanoid_Big* pInstance = new CHumanoid_Big(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHumanoid_Big::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);

	Safe_Release(m_pColorTexture);
	Safe_Release(m_pAlphaOneTexture);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}
	m_vecNormalAttacks.clear();
}
