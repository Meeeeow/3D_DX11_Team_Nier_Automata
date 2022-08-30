#include "stdafx.h"
#include "..\Public\Humanoid_Small.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "EffectFactory.h"
#include "SoundMgr.h"
#include "Normal_Attack.h"
#include "MeshEffect.h"
#include "UI_HUD_Damage.h"
#include "Level_Stage1.h"
#include "LevelManager.h"
#include "Player.h"
#include "Inventory.h"
#include "UI_Dialogue_SystemMessage.h"

_uint	CHumanoid_Small::m_iBombSoundChannel = 2; // CHANNELID::BOMB

CHumanoid_Small::CHumanoid_Small(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CHumanoid_Small::CHumanoid_Small(const CHumanoid_Small & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CHumanoid_Small::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHumanoid_Small::NativeConstruct(void * pArg)
{
	MO_INFO* tInfo = (MO_INFO*)pArg;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;


	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Humanoid_Small", COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;
	

	m_pModel->Add_RefNode(L"LeftHand", "bone304");
	//m_pModel->Add_RefNode(L"LeftHand", "Bone306");
	m_pModel->Add_RefNode(L"Rush", "bone016");
	//m_pModel->Add_RefNode(L"RightHand", "Bone562");
	m_pModel->Add_RefNode(L"Head", "bone016");


	m_iPassIndex = 1;
	
	if (tInfo != nullptr)
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo->m_matWorld));

	else
		m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	Engine::CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	_uint iCurrentIndex = 0;

	tColliderDesc.vScale = _float3(0.7f, 0.7f, 0.7f);
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;
	/*if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;*/
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

	m_bCollision = true;

	static _uint iCHumanoid_Small_InstanceID = OriginID_HumanoidSmall;
	m_iObjID = iCHumanoid_Small_InstanceID++;
	
	Gain_Pattern();
	//m_ePattern = M_PATTERN::PATTERN_1;

	m_fHitRecovery = 1.f;

	m_tObjectStatusDesc.fHp = 200.f;
	m_tObjectStatusDesc.fMaxHp = 200.f;
	m_tObjectStatusDesc.iAtt = 5;
	Check_Pause();

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	return S_OK;
}

_int CHumanoid_Small::Tick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (!m_bStart)
		dTimeDelta = 0.0;

	Update_UI(dTimeDelta);

	Update_Effect(dTimeDelta);

	if (OBJSTATE::DEAD == m_eState)
	{
		Create_DeathEffect();

		Drop_RandomItem(ITEMCATEGORY::RESOURCE);

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

	Update_Force(dTimeDelta, m_pNavigation);

	Update_CollisionTime(dTimeDelta);

	Update_FocusTime(dTimeDelta);

	return (_int)m_eState;
}

_int CHumanoid_Small::LateTick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

		CSoundMgr::CHANNELID	eID = (CSoundMgr::CHANNELID)m_iBombSoundChannel;

		pSoundMgr->StopPlaySound(TEXT("Bomb2.mp3"), eID);
		pSoundMgr->PlaySound(TEXT("Bomb2.mp3"), eID);

		++m_iBombSoundChannel;

		if ((_uint)CSoundMgr::CHANNELID::BOMB4 < m_iBombSoundChannel)
		{
			m_iBombSoundChannel = (_uint)CSoundMgr::CHANNELID::BOMB;
		}

		m_eState = OBJSTATE::DEAD;
	}
	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		if(m_bStart)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	
	Check_Pattern_Cool(dTimeDelta);

	return 0;
}

HRESULT CHumanoid_Small::Render()
{
	if (m_bStart)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CHumanoid_Small::Find_Enemy()
{
	if (Ray_Collision())
		m_pModel->Set_Animation(4, true);
	else
		m_pModel->Set_Animation(90, true);

	return S_OK;
}

_bool CHumanoid_Small::Ray_Collision()
{
	Engine::CCollider* pInstance = dynamic_cast<Engine::CCollider*>(CGameInstance::Get_Instance()->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_SPHERE, 0));
	if (pInstance != nullptr)
	{
		_vector vPivot = XMLoadFloat3(&m_pModel->Get_MiddlePoint()) + m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vRay = m_pTransform->Get_State(CTransform::STATE::LOOK);
		_matrix matRotation = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotation));

		vRay = XMVector4Transform(vRay, matRotation);

		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_float	fDistance = 0.f;
		if (pInstance->Collision_Sphere(vPivot, vRay, vPosition, fDistance))
		{
			m_fRotation = 0;
			if (fDistance <= 8.f)
				return true;
		}
		m_fRotation += 1.f;
		if (m_fRotation >= 45.f)
			m_fRotation = -m_fRotation;
	}
	return false;
}

void CHumanoid_Small::Check_Target1(_double TimeDelta)
{

	_vector vTargetPos = Get_TargetPosition();

	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vTargetPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;

	if (m_bJump == false &&	m_eAnimState != M_ANIMSTATE::DROP && m_eAnimState != M_ANIMSTATE::LANDING && m_eAnimState != M_ANIMSTATE::STANDING)
	{
		if (fDist <= 7.f && fDist >= 1.f)
			m_bEngagementRagne = true;
		else
			m_bEngagementRagne = false;

		if ((fDist <= 20.f) && (fDist > 1.f) && m_eAnimState != M_ANIMSTATE::ENGAGE && m_eAnimState != M_ANIMSTATE::BACKSTEP && m_eAnimState != M_ANIMSTATE::HIT)
		{
			if (!m_bPlayOnce)
			{
				int a = rand() % 3;
				
				if ( a == 0)
					CSoundMgr::Get_Instance()->PlaySound(L"Humanoid_Find.wav", CSoundMgr::CHANNELID::MONSTER1);
				else if ( a == 1)
					CSoundMgr::Get_Instance()->PlaySound(L"Humanoid_Find.wav", CSoundMgr::CHANNELID::MONSTER2);
				else if ( a == 2)
					CSoundMgr::Get_Instance()->PlaySound(L"Humanoid_Find.wav", CSoundMgr::CHANNELID::MONSTER3);

				m_bPlayOnce = true;
			}
			m_eAnimState = M_ANIMSTATE::WALK;
		}
		if ((fDist > 20.f) || fDist <= 1.f && !m_bChargeAttackStart && m_eAnimState != M_ANIMSTATE::BACKSTEP && m_eAnimState != M_ANIMSTATE::HIT)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_fRatio = 0.f;
			m_bPlayOnce = false;
		}
		if (fDist < 1.f && m_eAnimState != M_ANIMSTATE::ENGAGE && !m_bBackStepCool && m_eAnimState != M_ANIMSTATE::HIT)
		{
			m_eAnimState = M_ANIMSTATE::BACKSTEP;
		}

		if (m_bEngagementRagne && !m_bAttack1Cool && !m_bAttack2Cool && m_eAnimState != M_ANIMSTATE::HIT)
		{
			m_eAnimState = M_ANIMSTATE::ENGAGE;
			m_fRatio = 0.f;
		}
	}

	else if (m_bJump == true)
	{
		m_eAnimState = M_ANIMSTATE::DROP;
	}
}

void CHumanoid_Small::Check_Times1(_double TimeDelta)
{
	if (m_bAttack1Cool)
	{
		m_fAttack1Time += (_float)TimeDelta;
		if (m_fAttack1Time >= 9.f)
		{
			m_bAttack1Cool = false;
			m_fAttack1Time = 0.f;
		}
	}
	if (m_bAttack2Cool)
	{
		m_fAttack2Time += (_float)TimeDelta;
		if (m_fAttack2Time >= 3.f)
		{
			m_bAttack2Cool = false;
			m_fAttack2Time = 0.f;
		}
	}

	if (m_bBackStepCool)
	{
		m_fBackStepTime += (_float)TimeDelta;
		if (m_fBackStepTime >= 12.f)
		{
			m_bBackStepCool = false;
			m_fBackStepTime = 0.f;
		}
	}
}

void CHumanoid_Small::Check_AnimState1(_double TimeDelta)
{
	int a = 0;
	if (m_eAnimState == M_ANIMSTATE::IDLE)
	{
		m_iAnimIndex = SMALL_IDLE;
	}

	if (m_eAnimState == M_ANIMSTATE::WALK)
	{
		m_iAnimIndex = SMALL_WALK;
	}

	if (m_eAnimState == M_ANIMSTATE::HIT)
	{
		m_iAnimIndex = SMALL_HIT1;

		if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_HIT1)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
		}
	}

	if (m_eAnimState == M_ANIMSTATE::ENGAGE)
	{
		if (!m_bAttack1Cool || m_eAttackState == ATTACK_STATE::RUSH)
		{
			if (!m_bChargeAttackStart)
			{
				if (m_iAnimIndex != SMALL_RUSH_READY && m_bCreateOnce)
				{
					m_bCreateOnce = false;
				}

				m_iAnimIndex = SMALL_RUSH_READY;
				m_eAttackState = ATTACK_STATE::RUSH;

				if (m_bAtkDead)
					m_bAtkDead = false;
			}

			if ((0.9 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_RUSH_READY)
			{
				m_bChargeAttackStart = true;
				m_iAnimIndex = SMALL_RUSHING;
			}
			else if (m_pModel->Get_CurrentAnimation() == SMALL_RUSHING && m_iAnimIndex != SMALL_HIT1)
			{
				m_fChargeTime += (_float)TimeDelta;
				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 2;	// Rush
					tInfo.Matrix = m_RushPos;

					m_AttackMatrix = m_RushPos;
					m_bIsRush = true;
					m_bIsPunch = false;

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

				if (m_fChargeTime >= 2.2f && 0.9 <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimIndex = SMALL_RUSH_END;
					m_fChargeTime = 0.f;

				}
			}
			else if ((0.9 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_RUSH_END)
			{
				m_iAnimIndex = SMALL_IDLE;
				m_bChargeAttackStart = false;
				m_bAttack1Cool = true;
				m_eAttackState = ATTACK_STATE::NONE;
				m_bCreateOnce = false;
				m_bAtkDead = true;
			}
		}
		else if (!m_bAttack2Cool || m_eAttackState == ATTACK_STATE::PUNCH)
		{
			if (m_iAnimIndex != SMALL_PUNCH && m_bCreateOnce)
			{
				m_bCreateOnce = false;
			}

			m_iAnimIndex = SMALL_PUNCH;
			m_eAttackState = ATTACK_STATE::PUNCH;

			if (m_bAtkDead)
				m_bAtkDead = false;

			if (!m_bCreateOnce && m_pModel->Get_CurrentAnimation() == SMALL_PUNCH && (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio()))
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				Humanoid_Atk_Info tInfo;

				tInfo.iNumber = 1;	// Punch
				tInfo.Matrix = m_LArmPos;

				m_AttackMatrix = m_LArmPos;
				m_bIsPunch = true;
				m_bIsRush = false;

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

			if (m_pModel->Get_CurrentAnimation() == SMALL_PUNCH && m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_bAtkDead = true;

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_PUNCH)
			{
				m_iAnimIndex = SMALL_IDLE;
				m_bAttack2Cool = true;
				m_eAttackState = ATTACK_STATE::NONE;
				m_eAnimState = M_ANIMSTATE::IDLE;

				m_bCreateOnce = false;
			}
		}

	}

	if (m_eAnimState == M_ANIMSTATE::BACKSTEP)
	{
		m_iAnimIndex = SMALL_BACKSTEP;

		if (0.9 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == SMALL_BACKSTEP)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
			m_bBackStepCool = true;
		}
	}

	if (m_eAnimState == M_ANIMSTATE::DROP)
	{
		m_iAnimIndex = SMALL_DROP;

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

			m_iAnimIndex = SMALL_LANDING;
			m_eAnimState = M_ANIMSTATE::LANDING;

			m_bJump = false;
		}
	}

	else if (m_eAnimState == M_ANIMSTATE::LANDING)
	{
		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == SMALL_LANDING)
		{
			m_eAnimState = M_ANIMSTATE::STANDING;
			m_iAnimIndex = SMALL_STANDING;
		}
	}

	else if (m_eAnimState == M_ANIMSTATE::STANDING)
	{
		m_iAnimIndex = SMALL_STANDING;

		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == SMALL_STANDING)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
		}
	}

	if (m_iAnimIndex == SMALL_IDLE || m_iAnimIndex == SMALL_WALK || m_iAnimIndex == SMALL_RUSHING || m_iAnimIndex == SMALL_DROP)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);

	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Small::Check_Target2(_double TimeDelta)
{
	_vector vTargetPos = Get_TargetPosition();
	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vTargetPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;

	if (m_bJump == false && m_eAnimState != M_ANIMSTATE::DROP && m_eAnimState != M_ANIMSTATE::LANDING && m_eAnimState != M_ANIMSTATE::STANDING)
	{

		if (fDist <= 3.5f && fDist >= 1.f)
			m_bEngagementRagne = true;
		else
			m_bEngagementRagne = false;

		if ((fDist <= 20.f) && (fDist > 1.f) && m_eAnimState != M_ANIMSTATE::ENGAGE && m_eAnimState != M_ANIMSTATE::SIDESTEP && m_eAnimState != M_ANIMSTATE::HIT)
		{
			if (m_eAnimState != M_ANIMSTATE::F_JUMP)
				m_eAnimState = M_ANIMSTATE::WALK;

			if (fDist <= 15.f && fDist >= 4.f)
				m_eAnimState = M_ANIMSTATE::F_JUMP;
		}
		if ((fDist > 20.f) || fDist <= 1.f && m_eAnimState != M_ANIMSTATE::SIDESTEP && m_eAnimState != M_ANIMSTATE::ENGAGE && m_eAnimState != M_ANIMSTATE::F_JUMP && m_eAnimState != M_ANIMSTATE::HIT)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_fRatio = 0.f;
			//m_bEngagementRagne = false;
		}
		if (fDist < 1.5f && m_eAnimState != M_ANIMSTATE::ENGAGE && !m_bSideStepCool && m_eAnimState != M_ANIMSTATE::HIT)
		{
			m_eAnimState = M_ANIMSTATE::SIDESTEP;
		}

		if (m_bEngagementRagne && (!m_bAttack1Cool || !m_bAttack2Cool) && m_eAnimState != M_ANIMSTATE::HIT) //-> 占쏙옙占쏙옙占쏙옙占쏙옙
		{
			m_eAnimState = M_ANIMSTATE::ENGAGE;
			m_fRatio = 0.f;
		}
	}

	else if (m_bJump == true)
	{
		m_eAnimState = M_ANIMSTATE::DROP;
	}
}

void CHumanoid_Small::Check_AnimState2(_double TimeDelta)
{
	// attack1 = heading
	// attack2 = falchon Punch
	if (m_eAnimState == M_ANIMSTATE::IDLE)
	{
		m_iAnimIndex = SMALL_IDLE;
	}
	if (m_eAnimState == M_ANIMSTATE::WALK)
	{
		m_iAnimIndex = SMALL_WALK;
	}
	if (m_eAnimState == M_ANIMSTATE::F_JUMP)
	{
		m_iAnimIndex = SMALL_JUMP_FOWARD;
	}
	if (m_eAnimState == M_ANIMSTATE::HIT)
	{
		m_iAnimIndex = SMALL_HIT1;

		if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_HIT1)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
		}
	}
	
	if (m_eAnimState == M_ANIMSTATE::ENGAGE)
	{
		if ((!m_bAttack1Cool || m_eAttackState == ATTACK_STATE::HEAD) && m_eAttackState != ATTACK_STATE::PUNCH)
		{
			if (m_iAnimIndex != SMALL_HEADING && m_bCreateOnce)
			{
				m_bCreateOnce = false;
			}
			m_iAnimIndex = SMALL_HEADING;
			m_eAttackState = ATTACK_STATE::HEAD;

			if (m_bAtkDead)
				m_bAtkDead = false;

			if (!m_bCreateOnce && m_pModel->Get_CurrentAnimation() == SMALL_HEADING && (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio()))
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				Humanoid_Atk_Info tInfo;

				tInfo.iNumber = 3;	// Head
				tInfo.Matrix = m_HeadPos;

				m_AttackMatrix = m_HeadPos;
				m_bIsHeading = true;
				m_bIsPunch = false;

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

			if (m_pModel->Get_CurrentAnimation() == SMALL_HEADING && m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_bAtkDead = true;

			if ((0.9 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_HEADING)
			{
				m_iAnimIndex = SMALL_IDLE;
				m_eAnimState = M_ANIMSTATE::IDLE;
				m_bAttack1Cool = true;
				m_eAttackState = ATTACK_STATE::NONE;
				m_bCreateOnce = false;
			}

		}
		else if ((!m_bAttack2Cool || m_eAttackState == ATTACK_STATE::PUNCH) && m_eAttackState != ATTACK_STATE::HEAD)
		{
			if (m_iAnimIndex != SMALL_PUNCH && m_bCreateOnce)
			{
				m_bCreateOnce = false;
			}

			m_iAnimIndex = SMALL_PUNCH;
			m_eAttackState = ATTACK_STATE::PUNCH;

			if (m_bAtkDead)
				m_bAtkDead = false;

			if (!m_bCreateOnce && m_pModel->Get_CurrentAnimation() == SMALL_PUNCH && (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio()))
			{
				CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

				Humanoid_Atk_Info tInfo;

				tInfo.iNumber = 1;	// Punch
				tInfo.Matrix = m_LArmPos;

				m_AttackMatrix = m_LArmPos;
				m_bIsPunch = true;
				m_bIsHeading = false;

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

			if (m_pModel->Get_CurrentAnimation() == SMALL_PUNCH && m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_PUNCH)
			{
				m_iAnimIndex = SMALL_IDLE;
				m_eAnimState = M_ANIMSTATE::IDLE;
				m_bAttack2Cool = true;
				m_eAttackState = ATTACK_STATE::NONE;
				m_bCreateOnce = false;
			}
		}
	}
	if (m_eAnimState == M_ANIMSTATE::SIDESTEP)
	{
		m_eAnimState = M_ANIMSTATE::SIDESTEP;
		m_iAnimIndex = SMALL_SIDESTEP;

		if ((0.9 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == SMALL_SIDESTEP)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
			m_bSideStepCool = true;
		}
	}

	if (m_eAnimState == M_ANIMSTATE::DROP)
	{
		m_iAnimIndex = SMALL_DROP;

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

			m_iAnimIndex = SMALL_LANDING;
			m_eAnimState = M_ANIMSTATE::LANDING;
			m_pTransform->Set_Jump(false);

			m_bJump = false;
		}
	}

	else if (m_eAnimState == M_ANIMSTATE::LANDING)
	{
		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == SMALL_LANDING)
		{
			m_eAnimState = M_ANIMSTATE::STANDING;
			m_iAnimIndex = SMALL_STANDING;
		}
	}

	else if (m_eAnimState == M_ANIMSTATE::STANDING)
	{
		m_iAnimIndex = SMALL_STANDING;

		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == SMALL_STANDING)
		{
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_iAnimIndex = SMALL_IDLE;
		}
	}


	if (m_iAnimIndex == SMALL_IDLE || m_iAnimIndex == SMALL_WALK || m_iAnimIndex == SMALL_RUSHING || m_iAnimIndex == SMALL_JUMP_FOWARD || m_iAnimIndex == SMALL_DROP )
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Small::Check_Times2(_double TimeDelta)
{
	if (m_bAttack1Cool)
	{
		m_fAttack1Time += (_float)TimeDelta;
		if (m_fAttack1Time >= 8.f)
		{
			m_bAttack1Cool = false;
			m_fAttack1Time = 0.f;
		}
	}
	if (m_bAttack2Cool)
	{
		m_fAttack2Time += (_float)TimeDelta;
		if (m_fAttack2Time >= 4.f)
		{
			m_bAttack2Cool = false;
			m_fAttack2Time = 0.f;
		}
	}

	if (m_bSideStepCool)
	{
		m_fSideStepTime += (_float)TimeDelta;
		if (m_fSideStepTime >= 2.f)
		{
			m_bSideStepCool = false;
			m_fSideStepTime = 0.f;
		}
	}
}

void CHumanoid_Small::Check_Target3(_double TimeDelta)
{

}

void CHumanoid_Small::Check_AnimState3(_double TimeDelta)
{
	m_iAnimIndex = SMALL_IDLE;

	if (m_iAnimIndex == SMALL_IDLE || m_iAnimIndex == SMALL_WALK || m_iAnimIndex == SMALL_RUSHING)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Small::Check_Times3(_double TimeDelta)
{

}

void CHumanoid_Small::Update_FocusTime(_double TimeDelta)
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

void CHumanoid_Small::Chase(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::WALK || m_eAnimState == M_ANIMSTATE::ENGAGE || m_eAnimState == M_ANIMSTATE::F_JUMP)
	{
		if (m_fDistance > 0.1f)
		{
			m_fRatio += (_float)(TimeDelta * 0.5f);

			_vector vTargetPos = Get_TargetPosition();
			_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			_float fTargetY = XMVectorGetY(vTargetPos);
			vMonsterPos =XMVectorSetY(vMonsterPos, fTargetY);

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
				m_bChase = true;
			}
		}

		else
			m_fRatio = 0.f;
	}
}

_bool CHumanoid_Small::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CHumanoid_Small::Collision(CGameObject * pGameObject)
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

			if (m_dCreateElectricityTimeDuration < 0)
			{
				m_dCreateElectricityTimeDuration = ROBOT_HIT_EFFECT_DEALY;
				Create_ElectricityEffect();

				if (m_eAnimState == M_ANIMSTATE::ENGAGE)
				{
					if (m_iAnimIndex == SMALL_RUSHING || m_iAnimIndex == SMALL_RUSH_READY || m_iAnimIndex == SMALL_RUSH_END)
					{
						m_bChargeAttackStart = false;
					}
					m_fChargeTime = 0.f;
					m_bAttack1Cool = true;
					m_eAttackState = ATTACK_STATE::NONE;
					m_bCreateOnce = false;
				}
				m_iAnimIndex = SMALL_HIT1;
				m_eAnimState = M_ANIMSTATE::HIT;
			}
		}
		//Create_ElectricityEffect();
	}

	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (HIT::NONE != pGameObject->Get_HitType())
		{
			m_iFlagUI |= FLAG_UI_CALL_HPBAR;
			m_dCallHPBarUITimeDuration = 3.0;

			m_iTargetAnimation = pGameObject->Get_TargetAnimation();
			//Create_Laser_Hit_Effect();

			if (m_dCreateElectricityTimeDuration < 0)
			{
				m_dCreateElectricityTimeDuration = ROBOT_HIT_EFFECT_DEALY;
				Create_ElectricityEffect();
				if (m_eAnimState == M_ANIMSTATE::ENGAGE)
				{
					if (m_iAnimIndex == SMALL_RUSHING || m_iAnimIndex == SMALL_RUSH_READY || m_iAnimIndex == SMALL_RUSH_END)
						m_bChargeAttackStart = false;
					m_fChargeTime = 0.f;
					m_bAttack1Cool = true;
					m_eAttackState = ATTACK_STATE::NONE;
					m_bCreateOnce = false;
				}
				m_iAnimIndex = SMALL_HIT1;
				m_eAnimState = M_ANIMSTATE::HIT;
			}
		}
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CHumanoid_Small::Gain_Pattern()
{

	int iNum = rand() % 2;

	if (iNum == 0)
	{
		m_ePattern = M_PATTERN::PATTERN_1;
	}
	else if (iNum == 1)
	{
		m_ePattern = M_PATTERN::PATTERN_2;
	}
}

void CHumanoid_Small::Play_Pattern(_double TimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Pattern_1(TimeDelta);
	}

	else if (m_ePattern == M_PATTERN::PATTERN_2)
	{
		Pattern_2(TimeDelta);
	}
}

void CHumanoid_Small::Check_Pattern_Cool(_double TimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Check_Times1(TimeDelta);
	}

	else if (m_ePattern == M_PATTERN::PATTERN_2)
	{
		Check_Times2(TimeDelta);
	}
}

void CHumanoid_Small::Pattern_1(_double TimeDelta)
{
	if ((_uint)m_fForce <= 0)
	{
		Check_Target1(TimeDelta);
		Chase(TimeDelta);
	}
		Check_AnimState1(TimeDelta);
}

void CHumanoid_Small::Pattern_2(_double TimeDelta)
{
	if ((_uint)m_fForce <= 0)
	{
		Check_Target2(TimeDelta);
		Chase(TimeDelta);
	}
		Check_AnimState2(TimeDelta);
}

void CHumanoid_Small::Pattern_3(_double TimeDelta)
{
	//Check_Target3(TimeDelta);
	//Chase(TimeDelta);
	Check_AnimState3(TimeDelta);
}

_fvector CHumanoid_Small::Get_TargetPosition()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bFocus)
	{
		CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			MSGBOX("Failed CHumanoid_Small::Get_TargetPosition() - LayerPlayer, Com_Transform");
			return XMVectorZero();
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
			MSGBOX("Failed CHumanoid_Small::Get_TargetPosition() - Android_9S, Com_Transform");
			return XMVectorZero();
		}

		RELEASE_INSTANCE(CGameInstance);

		return pTransform->Get_State(CTransform::STATE::POSITION);
	}

	MSGBOX("Failed CHumanoid_Small::Get_TargetPosition() - else");
	return XMVectorZero();
}

void CHumanoid_Small::Drop_RandomItem(ITEMCATEGORY eCategory)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CPlayer*		pPlayer = nullptr;

	pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	if (nullptr == pPlayer)
		return;

	CInventory*	pPlayerInventory = nullptr;

	pPlayerInventory = dynamic_cast<CInventory*>(pPlayer->Get_Component(COM_KEY_INVENTORY));

	if (nullptr == pPlayerInventory)
		return;

	////////////////////////////////////////////////////////////////////////////////////

	_uint	iItemSlot = 0;
	_uint	iItemCount = 0;

	iItemSlot = (rand() % 10) + ((_int)eCategory * 10);
	iItemCount = (rand() % 15) + 1;

	pPlayerInventory->Open_ItemSlot(iItemSlot)->iItemCount += iItemCount;

	CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(iItemSlot, iItemCount, CUI_Dialogue_SystemMessage::iMsgAchieved);

	pGameInstance->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
}

void CHumanoid_Small::Update_Bone()
{
	Update_LeftArm();
	Update_Rush();
	Update_Head();
}

void CHumanoid_Small::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET* pPacket = (PACKET*)pMessage;
		_uint i = *(_uint*)pPacket->pData;
		Set_State(OBJSTATE::DEAD);
	}
}

_int CHumanoid_Small::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CHumanoid_Small::Update_UI(_double dDeltaTime)
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
	if (m_eAnimState != CHumanoid_Small::M_ANIMSTATE::IDLE)
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

void CHumanoid_Small::Update_Rush()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Rush");
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
	vecPos = vecPos + vNormUp * 0.8f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_RushPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_RushPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_RushPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_RushPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsRush)
		m_AttackMatrix = m_RushPos;
}

void CHumanoid_Small::Update_LeftArm()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"LeftHand");
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

	vecPos = vecPos + vNormRight * -1.4f;
	vecPos = vecPos + vNormUp * 1.f;
	vecPos = vecPos + vNormLook * -0.05f;

	memcpy(&m_LArmPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_LArmPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_LArmPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_LArmPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsPunch)
	{
		m_AttackMatrix = m_LArmPos;
	}
}

void CHumanoid_Small::Update_Head()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Head");
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
	vecPos = vecPos + vNormUp * 1.2f;
	vecPos = vecPos + vNormLook * -0.3f;

	memcpy(&m_HeadPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_HeadPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_HeadPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_HeadPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsHeading)
	{
		m_AttackMatrix = m_HeadPos;
	}
}

void CHumanoid_Small::Update_NormalAttack()
{
	if (false == m_vecNormalAttacks.empty())
	{
		if (true == m_bAtkDead)
		{
			for (auto& pAttack : m_vecNormalAttacks)
			{
				pAttack->Set_State(OBJSTATE::DEAD);
				Safe_Release(pAttack);
			}
			m_vecNormalAttacks.clear();
		}
		else
		{
			for (auto& pAttack : m_vecNormalAttacks)
			{
				CTransform*	pAttackTransform = dynamic_cast<CTransform*>(pAttack->Get_Component(COM_KEY_TRANSFORM));

				if (nullptr != pAttackTransform)
				{
					pAttackTransform->Set_WorldMatrix(XMLoadFloat4x4(&m_AttackMatrix));
				}
			}
		}
	}
	return;
}

CHumanoid_Small * CHumanoid_Small::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CHumanoid_Small* pInstance = new CHumanoid_Small(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CHumanoid_Small::Clone(void * pArg)
{
	CHumanoid_Small* pInstance = new CHumanoid_Small(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHumanoid_Small::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}
	m_vecNormalAttacks.clear();
}

HRESULT CHumanoid_Small::Create_DeathEffect()
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
	pEffectBulletPacket.iNavIndex = m_pNavigation->Get_Index();
	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = 0.f;
	pEffectBulletPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ROBOT_DEATH_EXPLOSION_SPARK4;
	pEffectBulletPacket.fMaxLifeTime = -1.f;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Robot_Death_Explosion_Spark4.dat", &pEffectBulletPacket);

	return S_OK;
}

HRESULT CHumanoid_Small::Create_Laser_Hit_Effect()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		tEffectPacket;


	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	tEffectPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	tEffectPacket.tEffectMovement.fSpeed = 2.f;
	tEffectPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tEffectPacket.tEffectMovement.vPosition = vPos;
	tEffectPacket.ulID = CHECKSUM_EFFECT_LASER_HIT_EFFECT_S;
	tEffectPacket.fMaxLifeTime = 2.f;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Laser_Hit.dat", &tEffectPacket);

	return S_OK;
}

HRESULT CHumanoid_Small::Create_ElectricityEffect()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		tEffectPacket;


	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	tEffectPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	tEffectPacket.tEffectMovement.fSpeed = 0.f;
	tEffectPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tEffectPacket.tEffectMovement.vPosition = vPos;
	tEffectPacket.ulID = CHECKSUM_EFFECT_MONSTER_HIT_ELECTRICITY;
	tEffectPacket.fMaxLifeTime = 1.f;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Robot_Hit_Electricity.dat", &tEffectPacket);

	return S_OK;
}

void CHumanoid_Small::Update_Effect(_double _dTimeDelta)
{
	m_dCreateElectricityTimeDuration -= _dTimeDelta;
}
