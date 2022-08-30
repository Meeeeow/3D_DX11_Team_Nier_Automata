#include "stdafx.h"
#include "..\Public\Iron_Humanoid_Small.h"
#include "MeshEffect.h"
#include "MinigameManager.h"
#include "Iron_Android9S.h"
#include "EffectFactory.h"
#include "UI_HUD_Hpbar_MiniGame.h"
#include "SoundMgr.h"

CIron_Humanoid_Small::CIron_Humanoid_Small(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CIron_Humanoid_Small::CIron_Humanoid_Small(const CIron_Humanoid_Small & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CIron_Humanoid_Small::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CIron_Humanoid_Small::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Set_FirstLook();

	m_iAnimation = ANIM_IRONHS_APPEARANCE;
	m_pModel->Set_CurrentAnimation(ANIM_IRONHS_APPEARANCE);
	m_pModel->Set_NoneBlend();

	m_bContinue = true;

	m_pTransform->Set_State(CTransform::STATE::POSITION, IRONFIST_MONSTER_START_POSITION);
	_uint iCurSel = m_pNavigation->Find_CurrentCell(m_pTransform->Get_State(CTransform::STATE::POSITION));
	m_pNavigation->Set_CurrentIndex(iCurSel);

	Update_Collider();

	m_tObjectStatusDesc.fHp = 150;
	m_tObjectStatusDesc.fMaxHp = 150;

	m_eState = OBJSTATE::DISABLE;

	return S_OK;
}

_int CIron_Humanoid_Small::Tick(_double dTimeDelta)
{
	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	if (OBJSTATE::DISABLE == m_eState)
		dTimeDelta *= 0.9;

	if (false == m_bPlayAnimation)
		dTimeDelta = 0.0;

	if (m_bPause)
		dTimeDelta = 0.0;

	Check_Dead();
	Check_Airborne(dTimeDelta);
	Check_GuardState();
	Check_AttackPattern(dTimeDelta);

	if (ANIMSTATE::HIT != m_eAnimState && ANIMSTATE::AIRBORNE != m_eAnimState && false == m_bAirborne && ANIMSTATE::ATTACK != m_eAnimState && ANIMSTATE::DEAD != m_eAnimState)
	{
		if (ANIMSTATE::GUARD != m_eAnimState)
			Check_Distance();

		LerpLooktoTarget(dTimeDelta);
	}

	if (OBJSTATE::DISABLE == m_eState)
		m_pModel->Set_Animation(m_iAnimation, false);
	else
		m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Fix_PositionZ();
	Update_Collider();

	Update_CollisionTime(dTimeDelta);
	Update_UI(dTimeDelta);
	Update_HitType();	

	m_pNavigation->Update(XMMatrixIdentity());

	return _int();
}

_int CIron_Humanoid_Small::LateTick(_double dTimeDelta)
{
	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	if (OBJSTATE::DISABLE == m_eState)
		dTimeDelta *= 0.9;

	if (false == m_bPlayAnimation)
		dTimeDelta = 0.0;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	if (m_pAttackBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	if (m_bAirborne)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		if (ANIM_IRONHS_AIRBORNE_HIT == iCurAnimIndex)
		{
			if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
				dTimeDelta = 0.0;
		}
	}

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);
	
	Fix_PositionZ();
	Update_Collider();

	return _int();
}

HRESULT CIron_Humanoid_Small::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CIron_Humanoid_Small::Set_FirstLook()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, MINIGAME_PLAYER, COM_KEY_TRANSFORM, 0);

	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerLook = XMVector3Normalize(pPlayerTransform->Get_State(CTransform::STATE::LOOK));
	vPlayerLook *= -1.f;

	_vector vScale = m_pTransform->Get_Scale();
	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = vPlayerLook;

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));

	XMStoreFloat3(&m_vRight, vLook);
	XMStoreFloat3(&m_vLeft, vLook * -1.f);
}

void CIron_Humanoid_Small::Check_Distance()
{
	if (OBJSTATE::DISABLE == m_eState)
		return;

	CMinigameManager* pManager = CMinigameManager::Get_Instance();

	CGameObject* pObject = pManager->Find_Player(MINIGAME_IRONFIST);
	if (nullptr == pObject)
		return;

	CTransform* pPlayerTransform = (CTransform*)pObject->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_float fDist = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

	if (fDist >= 3.f)
	{
		m_eAnimState = ANIMSTATE::WALK;
		m_iAnimation = ANIM_IRONHS_WALK;

		m_bContinue = true;
	}
	else
	{
		m_iAnimation = ANIM_IRONHS_IDLE;
		m_eAnimState = ANIMSTATE::IDLE;

		m_bContinue = true;
	}
}

void CIron_Humanoid_Small::Check_Dead()
{
	if (false == m_bAirborne && false == m_pTransform->IsJump())
	{
		if (m_tObjectStatusDesc.fHp <= 0.f)
		{
			if (m_iAnimation != ANIM_IRONHS_DEAD_END)
				m_iAnimation = ANIM_IRONHS_DEAD_START;

			m_bContinue = false;

			m_eAnimState = ANIMSTATE::DEAD;

			CMinigameManager* pManager = CMinigameManager::Get_Instance();

			pManager->Set_End();
		}
	}

	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	
	if (ANIM_IRONHS_DEAD_START == iCurAnimIndex)
	{
		if (0.98 <= m_pModel->Get_PlayTimeRatio())
		{
			m_iAnimation = ANIM_IRONHS_DEAD_END;
			m_bContinue = false;

			m_bCollision = false;
		}
	}
}

void CIron_Humanoid_Small::Check_Airborne(_double TimeDelta)
{
	if (ANIMSTATE::DEAD == m_eAnimState)
		return;

	if (m_bAirborne)
	{
		if (m_bFirstAirborne)
		{
			m_bFirstAirborne = false;
			m_bContinue = false;
		}
		
		if (m_pTransform->Get_JumpTime() >= FIRST_AIRBORNEJUMPTIME && m_pTransform->Get_JumpTime() < m_pTransform->Get_FallTime())
			m_pTransform->Set_FallTime();

			m_pTransform->Jump(TimeDelta * 0.4f);

		_float fY = 0.f;
		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			vPos = XMVectorSetY(vPos, fY);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			m_eAnimState = ANIMSTATE::AIRBORNE;

			m_bAirborne = false;
			m_bContinue = false;
			m_iAnimation = ANIM_IRONHS_AIRBORNE_HIT_LANDING;
			m_pModel->Set_NoneBlend();
			m_pTransform->Set_Jump(false);
			m_pTransform->Initialize_JumpTime();
		}
	}

	if (ANIMSTATE::AIRBORNE == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (ANIM_IRONHS_AIRBORNE_HIT_LANDING == iCurAnimIndex)
		{
			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_IRONHS_AIRBORNE_HIT_STAND;
				m_pModel->Set_NoneBlend();

				m_bContinue = false;
			}
		}

		else if (ANIM_IRONHS_AIRBORNE_HIT_STAND == iCurAnimIndex)
		{
			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_IRONHS_IDLE;
				m_pModel->Set_NoneBlend();
				m_bContinue = false;

				m_eAnimState = ANIMSTATE::IDLE;
			}
		}
	}

	if (ANIMSTATE::HIT == m_eAnimState)
	{
		if (m_pModel->IsFinish_Animation())
		{
			m_iAnimation = ANIM_IRONHS_IDLE;
			m_bContinue = false;

			m_eAnimState = ANIMSTATE::IDLE;
		}
	}
}

void CIron_Humanoid_Small::Activate_Guard()
{
	_uint iRand = rand() % 25;

	if (iRand == 0)
	{
		m_iAnimation = ANIM_IRONHS_GUARD;
		m_eAnimState = ANIMSTATE::GUARD;

		m_bContinue = false;
	}
}

void CIron_Humanoid_Small::Check_GuardState()
{
	if (ANIMSTATE::GUARD == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (ANIM_IRONHS_GUARD == iCurAnimIndex)
		{
			if (m_pModel->Get_PlayTimeRatio() > 0.5)
			{
				m_iAnimation = ANIM_IRONHS_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
			
				m_bContinue = true;
			}
		}
	}
}

void CIron_Humanoid_Small::Check_AttackPattern(_double TimeDelta)
{
	if (OBJSTATE::DISABLE == m_eState)
		return;

	if (ANIMSTATE::DEAD == m_eAnimState)
		return;

	if (ANIMSTATE::AIRBORNE != m_eAnimState && ANIMSTATE::GUARD != m_eAnimState && ANIMSTATE::HIT != m_eAnimState && ANIMSTATE::ATTACK != m_eAnimState)
	{
		m_WaitTime += TimeDelta;

		if (m_WaitTime >= m_WaitDelayTime)
		{
			Set_LooktoTarget();

			if (ANIMSTATE::IDLE == m_eAnimState)
			{
				m_eAnimState = ANIMSTATE::ATTACK;

				_uint iRand = rand() % 3;

				if (0 == iRand)
				{
					m_iAnimation = ANIM_IRONHS_DASHATTACK_START;
					m_bContinue = false;
				}
				else if (1 == iRand)
				{
					m_iAnimation = ANIM_IRONHS_HEADATTACK_START;
					m_bContinue = false;
				}
				else if (2 == iRand)
				{
					m_iAnimation = ANIM_IRONHS_DASH_JUMP;
					m_bContinue = false;
				}

				m_WaitTime = 0.0;
			}
		}
	}

	if (ANIMSTATE::ATTACK == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

		if (ANIM_IRONHS_DASHATTACK_START == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_DASHATTACK_DW;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_DASHATTACK_DW == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_DASHATTACK_END;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_DASHATTACK_END == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_IDLE;
				m_bContinue = false;

				m_eAnimState = ANIMSTATE::IDLE;
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_HEADATTACK_START == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_HEADATTACK_DW;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_HEADATTACK_DW == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_HEADATTACK_END;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_HEADATTACK_END == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_IDLE;
				m_bContinue = false;

				m_eAnimState = ANIMSTATE::IDLE;
				m_WaitTime = 0.0;
			}
		}
		else if (ANIM_IRONHS_DASH_JUMP == iCurAnimIndex)
		{
			if (0.98 <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRONHS_IDLE;
				m_bContinue = false;

				m_eAnimState = ANIMSTATE::IDLE;
				m_WaitTime = 0.0;
			}
		}
	}
}

void CIron_Humanoid_Small::Set_PlayAnimation(_bool bCheck)
{
	m_bPlayAnimation = bCheck;
	CSoundMgr::Get_Instance()->PlaySound(L"HS_IronIngage.wav", CSoundMgr::CHANNELID::VOICE2);
}

void CIron_Humanoid_Small::Set_LooktoTarget()
{
	CMinigameManager* pManager = CMinigameManager::Get_Instance();

	CGameObject* pObject = pManager->Find_Player(MINIGAME_IRONFIST);
	if (nullptr == pObject)
		return;

	CTransform* pTargetTransform = (CTransform*)pObject->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pTargetTransform)
		return;

	_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDir = XMVectorZero();

	if (XMVectorGetX(vTargetPos) > XMVectorGetX(vMyPos))
		vDir = XMLoadFloat3(&m_vRight);
	else
		vDir = XMLoadFloat3(&m_vLeft);

	_vector vScale = m_pTransform->Get_Scale();
	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vDir * XMVectorGetZ(vScale));

	m_LookLerpTime = 1.0;
}

void CIron_Humanoid_Small::Update_HitType()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	m_iTargetAnimation = iCurAnimIndex;

	if (ANIM_IRONHS_DASHATTACK_DW == iCurAnimIndex)
		m_eHitType = HIT::SINGLE;
	else if (ANIM_IRONHS_HEADATTACK_DW == iCurAnimIndex)
		m_eHitType = HIT::SINGLE;
	else if (ANIM_IRONHS_DASH_JUMP == iCurAnimIndex)
	{
		if (0.25 <= m_pModel->Get_PlayTimeRatio())
			m_eHitType = HIT::SINGLE;
		else
			m_eHitType = HIT::NONE;
	}
	else
		m_eHitType = HIT::NONE;
}

void CIron_Humanoid_Small::LerpLooktoTarget(_double TimeDelta)
{
	if (OBJSTATE::DISABLE == m_eState)
		return;

	CMinigameManager* pManager = CMinigameManager::Get_Instance();

	CGameObject* pPlayer = pManager->Find_Player(MINIGAME_IRONFIST);
	if (nullptr == pPlayer)
		return;

	CTransform* pPlayerTransform = (CTransform*)pPlayer->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	if (XMVectorGetX(vPlayerPos) > XMVectorGetX(vMyPos))
	{
		if (DIRECTION::RIGHT != m_eDirection)
		{
			m_LookLerpTime = 0.0;
			m_eDirection = DIRECTION::RIGHT;
		}
	}
	else if (XMVectorGetX(vPlayerPos) < XMVectorGetX(vMyPos))
	{
		if (DIRECTION::LEFT != m_eDirection)
		{
			m_LookLerpTime = 0.0;
			m_eDirection = DIRECTION::LEFT;
		}
	}

	m_LookLerpTime += TimeDelta;
	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	_vector vLerpLook = XMVectorZero();

	if (DIRECTION::LEFT == m_eDirection)
		vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vLeft), _float(m_LookLerpTime)));
	else if (DIRECTION::RIGHT == m_eDirection)
		vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vRight), _float(m_LookLerpTime)));

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetX(vScale));

	return;
}

void CIron_Humanoid_Small::Fix_PositionZ()
{
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vStartPos = IRONFIST_MONSTER_START_POSITION;

	vPos = XMVectorSetZ(vPos, XMVectorGetZ(vStartPos));

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
}

_bool CIron_Humanoid_Small::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CIron_Humanoid_Small::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{


		if (HIT::NONE != pGameObject->Get_HitType() && ANIMSTATE::GUARD != m_eAnimState && ANIMSTATE::ATTACK != m_eAnimState && ANIMSTATE::DEAD != m_eAnimState)
		{
			CIronGauntlet* pGauntlet = static_cast<CIronGauntlet*>(pGameObject);

			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			CModel* pGauntletModel = (CModel*)pGauntlet->Get_Component(COM_KEY_MODEL);
			if (nullptr == pGauntlet)
				return;

			_uint iGauntletAnimIndex = pGauntletModel->Get_CurrentAnimation();
			
			m_eAnimState = ANIMSTATE::HIT;

			switch (rand() % 3)
			{
			case 0:
				Create_PurplishRedFireEffect();
				break;
			case 1:
				Create_BlueFireEffect();
				break;
			case 2:
				Create_ElectricityEffect();
				break;
			}



			if (false == m_bAirborne)
				Activate_Guard();

			if (IRONGAUNTLET_PUNCH3 == iGauntletAnimIndex || ANIM_IRON9S_JUMPATTACK_DW == iGauntletAnimIndex)
			{
				if (!m_pTransform->IsJump())
					m_iTempTargetAnimation = iGauntletAnimIndex;

				m_bAirborne = true;
				m_bFirstAirborne = true;
				m_iAnimation = ANIM_IRONHS_AIRBORNE_HIT;
				m_pModel->Set_NoneBlend();
				m_pTransform->Set_Jump(true);
			}
			else
			{
				if (false == m_bAirborne)
				{
					if (ANIMSTATE::HIT == m_eAnimState)
					{
						if (ANIM_IRONHS_HIT1 == iCurAnimIndex)
						{
							if (0.98 <= m_pModel->Get_PlayTimeRatio())
								m_iAnimation = ANIM_IRONHS_HIT2;
						}
						else if (ANIM_IRONHS_HIT2 == iCurAnimIndex)
						{
							if (0.98 <= m_pModel->Get_PlayTimeRatio())
								m_iAnimation = ANIM_IRONHS_HIT1;
						}
						else
							m_iAnimation = ANIM_IRONHS_HIT1;

						m_bContinue = false;
					}
				}
				else if (m_bAirborne)
				{
					if (m_iTempTargetAnimation != pGameObject->Get_TargetAnimation())
					{
						if (1 != pGameObject->Get_TargetAnimation())
						{
							m_iTempTargetAnimation = pGameObject->Get_TargetAnimation();
							m_pTransform->Initialize_JumpTime();
							m_bFirstAirborne = true;
						}
					}
				}
			}
		}
	}

	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (HIT::NONE != pGameObject->Get_HitType())
		{

		}
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

HRESULT CIron_Humanoid_Small::SetUp_Components()
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


	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRON_HUMANOID_SMALL, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	//m_pTransform->Set_State(CTransform::STATE::POSITION, IRONFIST_MONSTER_START_POSITION);


	m_pModel->Add_RefNode(L"LeftHand", "bone304");
	m_pModel->Add_RefNode(L"Rush", "bone016");
	m_pModel->Add_RefNode(L"Head", "bone016");

	m_iPassIndex = 1;

	Engine::CCollider::DESC tColliderDesc;

	_uint iCurrentIndex = 0;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();

	tColliderDesc.vScale = _float3(0.55f, 0.55f, 0.55f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = _float3(0.f, 0.5f, 0.f);
	tColliderDesc.fRadius = 0.7f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_NAVIGATION_IRONFIST, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	m_bCollision = true;

	static _uint iCIron_Humanoid_Small_InstanceID = OriginID_IronHumanoidSmall;
	m_iObjID = iCIron_Humanoid_Small_InstanceID++;

	m_fHitRecovery = 1.f;

	m_tObjectStatusDesc.fHp = 100.f;
	m_tObjectStatusDesc.fMaxHp = 100.f;
	m_tObjectStatusDesc.iAtt = 10;
	Check_Pause();

	return S_OK;
}

void CIron_Humanoid_Small::Notify(void * pMessage)
{
}

_int CIron_Humanoid_Small::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CIron_Humanoid_Small::Update_UI(_double dDeltaTime)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (true)
	{
		CUI_HUD_Hpbar_MiniGame::HPBARDESC	tDesc;

		tDesc.eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::ENEMY;
		tDesc.fCrntHPUV = m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp;
		tDesc.iObjID = 4;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR, &tDesc));
	}
	return S_OK;
}

HRESULT CIron_Humanoid_Small::Create_PurplishRedFireEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition.y += 1.f;
	tPacket.tEffectMovement.vPosition.z -= 1.f;
	tPacket.ulID = CHECKSUM_EFFECT_PURPLISHREDFIRE;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\PurplishRedFire.dat", &tPacket);

	return S_OK;
}

HRESULT CIron_Humanoid_Small::Create_BlueFireEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition.y += 1.f;
	tPacket.tEffectMovement.vPosition.z -= 1.f;
	tPacket.ulID = CHECKSUM_EFFECT_TEKKEN_BLUEFIRE;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Tekken_BlueFire.dat", &tPacket);

	return S_OK;
}

HRESULT CIron_Humanoid_Small::Create_ElectricityEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition.y += 0.f;
	tPacket.tEffectMovement.vPosition.z -= 1.f;
	tPacket.ulID = CHECKSUM_EFFECT_MONSTER_HIT_ELECTRICITY;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Robot_Hit_Electricity.dat", &tPacket);

	return S_OK;
}

CIron_Humanoid_Small * CIron_Humanoid_Small::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CIron_Humanoid_Small* pGameInstance = new CIron_Humanoid_Small(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CIron_Humanoid_Small");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CIron_Humanoid_Small::Clone(void * pArg)
{
	CIron_Humanoid_Small* pGameInstance = new CIron_Humanoid_Small(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CIron_Humanoid_Small");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CIron_Humanoid_Small::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
}
