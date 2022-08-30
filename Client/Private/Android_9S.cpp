#include "stdafx.h"
#include "..\Public\Android_9S.h"
#include "Player.h"
#include "MainCamera.h"
#include "UI_HUD_Damage.h"
#include "Coaster.h"
#include "MinigameManager.h"
#include "Beauvoir.h"
#include "MeshEffect.h"
#include "EffectFactory.h"
#include "SoundMgr.h"


CAndroid_9S::CAndroid_9S(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CAndroid_9S::CAndroid_9S(const CAndroid_9S & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CAndroid_9S::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CAndroid_9S::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (FAILED(SetUp_Observer()))
		return E_FAIL;

	m_iAnimation = ANIM_9S_IDLE;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	m_eAnimState = ANIMSTATE::IDLE;

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	static _uint iCAndroid_9S_InstanceID = OriginID_9S;
	m_iObjID = iCAndroid_9S_InstanceID++;

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	m_tObjectStatusDesc.fHp = 100000;					// �ʸ�
	m_tObjectStatusDesc.fMaxHp = m_tObjectStatusDesc.fHp;

	m_fHitRecovery = 1.f;

	m_bCollision = true;

	ZeroMemory(&m_tAndroidDataPacket, sizeof(ANDROID_DATAPACKET));

	return S_OK;
}

_int CAndroid_9S::Tick(_double TimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bLookLerp)
		m_LookLerpTime += (TimeDelta * 2);

	if (NAVISTATE::COASTER != m_eNaviState)
	{
		Check_Teleport(TimeDelta);
		Check_Player();
	}
		Check_State(TimeDelta);
	if (NAVISTATE::COASTER != m_eNaviState)
	{
		Check_Target();
		Chase_Target(TimeDelta);
	}
		Check_WeaponState();
	if (NAVISTATE::COASTER != m_eNaviState)
	{
		Check_UpdateTime(TimeDelta);
		Check_JumpState(TimeDelta);
	}

	// Test
	if (NAVISTATE::COASTER != m_eNaviState)
	{
		Key_Check();
		Find_Target();
	}

	Check_HackingState(TimeDelta);

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Update_Force(TimeDelta, m_pNavigation);

	if (nullptr != m_pHitBox)
		m_pHitBox->Update_CollisionTime(TimeDelta);

	Update_Pod();

	Update_Collider();
	Update_UI(TimeDelta);
	Update_UnbeatableTime(TimeDelta);

	Check_Pause();

	return (_int)m_eState;
}

_int CAndroid_9S::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bPause)
		TimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);


	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return _int();
}

HRESULT CAndroid_9S::Render()
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

void CAndroid_9S::Set_AndroidState(ANDROIDSTATE eState)
{
	if (ANDROIDSTATE::BATTLE == eState)
	{
		if (ANDROIDSTATE::BATTLE != m_eAndroidState)
		{
			m_bLookLerp = true;
			m_LookLerpTime = 0.0;

			m_pCurrentTarget = nullptr;
		}
	}
	else if (ANDROIDSTATE::IDLE == eState)
	{
		if (ANDROIDSTATE::IDLE != m_eAndroidState)
		{
			m_bLookLerp = true;
			m_LookLerpTime = 0.0;
		}
	}

	m_eAndroidState = eState;
}

void CAndroid_9S::Set_Idle()
{
	m_pSpear->Set_NoneBlend(1);
	m_pSpear->Set_WeaponState(WEAPONSTATE::IDLE);
}

void CAndroid_9S::Set_PositionForCoaster()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCoaster* pCoaster = (CCoaster*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", 0);
	if (nullptr == pCoaster)
		return;

	CNavigation* pCoasterNavigation = (CNavigation*)pCoaster->Get_Component(COM_KEY_NAVIGATION);
	if (nullptr == pCoasterNavigation)
		return;

	m_pTransform->Set_State(CTransform::STATE::POSITION, pCoasterNavigation->Find_CenterPos(COASTER_9S_NAVIINDEX));
}

void CAndroid_9S::Set_AnimationForCoaster()
{
	m_iAnimation = ANIM_9S_IDLE2;
	m_pModel->Set_NoneBlend();

	m_bContinue = true;
}

_bool CAndroid_9S::IsOpenable_MiniGame()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CBeauvoir* pBeauvoir = (CBeauvoir*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Boss", 0);
	if (nullptr == pBeauvoir)
		return false;

	return pBeauvoir->Get_Hackable();
}

HRESULT CAndroid_9S::Open_MiniGame()
{
	CMinigameManager* pManager = CMinigameManager::Get_Instance();

	CMiniGame* pGame = pManager->Get_CurrentMiniGame();
	if (nullptr == pGame)
		pManager->Set_MiniGame(MINIGAME_IRONFIST);

	return S_OK;
}

void CAndroid_9S::Check_Player()
{
	if (m_bTeleport || m_bNoneTeleport)
		return;

	if (ANDROIDSTATE::BATTLE == m_eAndroidState || ANDROIDSTATE::HACKING == m_eAndroidState)
		return;

	if (ANIMSTATE::SLIDING == m_eAnimState || ANIMSTATE::HIT == m_eAnimState)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pPlayerTransform = (CTransform*)pPlayer->Get_Component(L"Com_Transform");
	if (nullptr == pPlayerTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CModel*		pPlayerModel = (CModel*)pPlayer->Get_Component(L"Com_Model");
	if (nullptr == pPlayerModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPos));

	_vector vDir = vPlayerPos - vPos;
	_float fDist = XMVectorGetX(XMVector3Length(vDir));

	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	if (fDist > 3.f || m_pTransform->IsJump() || ANIM_9S_JUMPLANDING == iCurAnimIndex || ANIM_9S_JUMPLANDINGRUN == iCurAnimIndex)
	{
		if (m_eAnimState != ANIMSTATE::JUMP && m_eAnimState != ANIMSTATE::JUMPLANDING)
		{
			if (false == m_tAndroidDataPacket.bJump && false == m_tAndroidDataPacket.bDoubleJump)
			{
				if (fDist > 3.f && false == m_bFix)
				{
					if (ANIM_9S_JUMP == m_pModel->Get_CurrentAnimation() || ANIM_9S_JUMPLANDING == m_pModel->Get_CurrentAnimation())
					{
						m_iAnimation = ANIM_9S_IDLE;
						m_eAnimState = ANIMSTATE::IDLE;
						m_bDoubleJump = false;
					}
					else
					{
						m_eAnimState = ANIMSTATE::RUN;
						m_iAnimation = ANIM_RUN;
						m_bDoubleJump = false;
					}
				}
				else
				{
					if (ANIM_9S_JUMPLANDING == iCurAnimIndex)
					{
						if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
						{
							m_iAnimation = ANIM_9S_IDLE;
							m_eAnimState = ANIMSTATE::IDLE;
							m_bDoubleJump = false;
						}
						else
						{
							m_iAnimation = ANIM_9S_JUMPLANDING;
							m_eAnimState = ANIMSTATE::JUMPLANDING;
							m_bDoubleJump = false;
						}
					}
				}

				if (ANIMSTATE::IDLE == m_eAnimState)
				{
					m_bLookLerp = true;
					m_LookLerpTime = 0.0;
				}
			}
		}

		if (true != m_tAndroidDataPacket.bJump && true != m_tAndroidDataPacket.bDoubleJump)
		{
			if (m_eAnimState != ANIMSTATE::JUMP)
			{
				m_eAnimState = ANIMSTATE::RUN;
				m_bContinue = true;
			}
		}

		_vector vScale = m_pTransform->Get_Scale();

		_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
		_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

		vRight = XMVector3Normalize(vRight);
		vUp = XMVector3Normalize(vUp);
		vLook = XMVector3Normalize(vLook);

		if (m_bLookLerp)
		{
			if (m_LookLerpTime > 1.0)
			{
				m_LookLerpTime = 1.0;
				m_bLookLerp = false;
			}

			vDir = XMVectorLerp(vLook, vDir, (_float)m_LookLerpTime);
			vDir = XMVector3Normalize(vDir);

			if (false == m_bLookLerp)
				m_LookLerpTime = 0.0;
		}
		else
			vDir = XMVector3Normalize(vDir);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

		vRight *= XMVectorGetX(vScale);
		vUp *= XMVectorGetY(vScale);
		vDir *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vDir);
	}
	else
	{
		if (ANIMSTATE::RUN == m_eAnimState)
		{
			if (ANIM_RUN == m_pModel->Get_CurrentAnimation())
			{
				m_eAnimState = ANIMSTATE::SLIDING;
				m_bContinue = false;

				if (0.4 > m_pModel->Get_PlayTimeRatio())
     				m_iAnimation = ANIM_RUNSLIDING_RIGHT;
				else
					m_iAnimation = ANIM_RUNSLIDING_LEFT;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CAndroid_9S::Check_State(_double TimeDelta)
{
	if (m_bTeleport)
		return;

	if (ANIMSTATE::SLIDING == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		if (ANIM_RUNSLIDING_LEFT == iCurAnimIndex || ANIM_RUNSLIDING_RIGHT == iCurAnimIndex)
		{
			if (m_pModel->Get_LinkMaxRatio() < m_pModel->Get_PlayTimeRatio())
			{
				if (ANDROIDSTATE::BATTLE == m_eAndroidState)
				{
					m_eAnimState = ANIMSTATE::ATTACK;
					m_iAnimation = ANIM_9S_ATTACK1;
					m_pSpear->Set_WeaponState(WEAPONSTATE::ATTACK);
					m_pSpear->Set_Animation(38);
					m_bDoubleJump = false; // Temp
				}
				else if (ANDROIDSTATE::IDLE == m_eAndroidState)
				{
					m_iAnimation = ANIM_9S_IDLE;
					m_eAnimState = ANIMSTATE::IDLE;
					Set_EquipWeaponIdle();

					m_bLookLerp = true;
					m_LookLerpTime = 0.0;
				}
			}
		}
		else if (ANIM_9S_SPURTSLIDING == iCurAnimIndex)
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_iAnimation = ANIM_9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_bContinue = true;
				m_bLookLerp = true;
				m_bDoubleJump = false;
			}
		}
	}
	else if (ANIMSTATE::ATTACK == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (WEAPONSTATE::ATTACK != m_pSpear->Get_WeaponState())
		{
			m_iAnimation = ANIM_9S_ATTACK1;
			m_pSpear->Set_Animation(38);
		}

		m_pSpear->Set_WeaponState(WEAPONSTATE::ATTACK);

		if (ANIM_9S_ATTACK1 <= m_iAnimation && ANIM_9S_ATTACK6 >= m_iAnimation)
		{
			if (m_iAnimation == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (ANIM_9S_ATTACK1 == iCurAnimIndex)
						m_pSpear->Set_Animation(3);
					else if (ANIM_9S_ATTACK2 == iCurAnimIndex)
						m_pSpear->Set_Animation(62);
					else if (ANIM_9S_ATTACK3 == iCurAnimIndex)
						m_pSpear->Set_Animation(69);
					else if (ANIM_9S_ATTACK4 == iCurAnimIndex)
						m_pSpear->Set_Animation(39);
					else if (ANIM_9S_ATTACK5 == iCurAnimIndex)
						m_pSpear->Set_Animation(60);

					if (ANIM_9S_ATTACK6 != iCurAnimIndex)
						m_iAnimation = iCurAnimIndex + 1;
					else if (ANIM_9S_ATTACK6 == iCurAnimIndex)
					{
						m_bLookLerp = true;
						m_LookLerpTime = 0.0;
						m_pSpear->Set_Immediately_Animation(1);
						m_pSpear->Set_WeaponState(WEAPONSTATE::IDLE);
					}
				}
			}
		}
	}
	else if (ANIMSTATE::HIT == m_eAnimState)
	{
		LerpToHitTarget(TimeDelta);
		Check_JumpState_ForHit(TimeDelta);

		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (ANIM_9S_HIT_FRONT == iCurAnimIndex)
		{
			_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

			if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
				m_bDoubleJump = false;
				m_UnbeatableTime = 0.0;
			}
		}
	}
	else if (ANIMSTATE::JUMPLANDING == m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
		{
			if (ANIM_9S_JUMPLANDING == iCurAnimIndex)
			{
				m_iAnimation = ANIM_9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_bContinue = true;
				m_bDoubleJump = false;
			}
			else if (ANIM_9S_JUMPLANDINGRUN == iCurAnimIndex)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
				if (nullptr == pPlayerTransform)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
				_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

				vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vMyPos));

				_vector vDist = XMVector3Length(vPlayerPos - vMyPos);

				if (3 < XMVectorGetX(vDist) && m_bFix)
				{
					if (false == m_tAndroidDataPacket.bJump && false == m_tAndroidDataPacket.bDoubleJump)
					{
						m_iAnimation = ANIM_RUN;
						m_eAnimState = ANIMSTATE::RUN;

						m_pModel->Set_NoneBlend();
						m_bContinue = true;
						m_bDoubleJump = false;
					}
				}
				else
				{
						m_iAnimation = ANIM_9S_JUMPLANDING;
						m_eAnimState = ANIMSTATE::JUMPLANDING;

						m_bContinue = false;
						m_bDoubleJump = false;
				}

				RELEASE_INSTANCE(CGameInstance);
			}
		}
		else 
		{
			if (ANIM_JUMPLANDING_RUN == iCurAnimIndex)
			{
				_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
		
				if (m_pModel->Get_LinkMinRatio() <= PlayTimeRatio)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

					CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
					if (nullptr == pPlayerTransform)
					{
						RELEASE_INSTANCE(CGameInstance);
						return;
					}

					_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
					_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

					vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vMyPos));

					_vector vDist = XMVector3Length(vPlayerPos - vMyPos);

					if (false == m_tAndroidDataPacket.bJump && false == m_tAndroidDataPacket.bDoubleJump)
					{
						if (3 < XMVectorGetX(vDist))
						{
							m_iAnimation = ANIM_RUN;
							m_eAnimState = ANIMSTATE::RUN;

							m_pModel->Set_NoneBlend();
							m_bContinue = true;
							m_bDoubleJump = false;
						}
					}

					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}
}

void CAndroid_9S::Check_Target()
{

}

void CAndroid_9S::Chase_Target(_double TimeDelta)
{
	if (m_bTeleport)
		return;

	if (ANIMSTATE::HIT == m_eAnimState || ANIMSTATE::JUMPLANDING == m_eAnimState)
		return;

	if (ANDROIDSTATE::BATTLE == m_eAndroidState)
	{
		if (nullptr != m_pCurrentTarget)
		{
			OBJSTATE eState = m_pCurrentTarget->Get_State();
			if (OBJSTATE::DEAD == eState)
			{
				m_LookLerpTime = 0.0;
				m_bLookLerp = false;

				Safe_Release(m_pCurrentTarget);
				m_pCurrentTarget = nullptr;

				m_iAnimation = ANIM_RUN;
				m_eAnimState = ANIMSTATE::RUN;

				Set_EquipWeaponIdle();

				return;
			}

			if (m_bLookLerp)
			{
				Look_Target(TimeDelta);
			}

			CTransform* pTransform = (CTransform*)m_pCurrentTarget->Get_Component(L"Com_Transform");
			if (nullptr == pTransform)
			{
				m_eAndroidState = ANDROIDSTATE::IDLE;
				m_pCurrentTarget = nullptr;
				return;
			}

			_vector vTargetPos = pTransform->Get_State(CTransform::STATE::POSITION);
			_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

			vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPos));

			_float fDist = XMVectorGetX(XMVector3Length(vTargetPos - vPos));
			if (fDist > 6.f)
			{
				if (m_bLookLerp)
				{
					m_iAnimation = ANIM_RUN;
					m_eAnimState = ANIMSTATE::RUN;
					m_bContinue = true;

					Set_EquipWeaponIdle();
				}
			}
			else
			{
				if (ANIMSTATE::RUN == m_eAnimState)
				{
					if (ANIM_RUN == m_pModel->Get_CurrentAnimation())
					{
						m_eAnimState = ANIMSTATE::SLIDING;
						m_bContinue = false;

						if (0.4 > m_pModel->Get_PlayTimeRatio())
							m_iAnimation = ANIM_RUNSLIDING_RIGHT;
						else
							m_iAnimation = ANIM_RUNSLIDING_LEFT;

						m_LookLerpTime = 0.0;
						m_bLookLerp = true;
					}
				}
				else if (ANIMSTATE::IDLE == m_eAnimState || ANIMSTATE::SLIDING == m_eAnimState)
				{
					m_eAnimState = ANIMSTATE::ATTACK;
					m_bContinue = false;

					m_bLookLerp = true;
					m_LookLerpTime = 0.0;
				}
			}
		}
	}
}

void CAndroid_9S::Find_Target()
{
	if (ANIMSTATE::HIT == m_eAnimState || ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPLANDING == m_eAnimState)
		return;

	if (nullptr == m_pCurrentTarget && ANDROIDSTATE::BATTLE == m_eAndroidState)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		_vector vPlayerPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_bool bCheck = true;

		list<CGameObject*>* MonsterList = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelID(), L"Monster");

		list<LENGTHINFO> TargetList;

		if (MonsterList == nullptr)
		{
			bCheck = false;
		}

		if (MonsterList != nullptr)
		{
			if (0 >= MonsterList->size())
			{
				m_pCurrentTarget = nullptr;
				bCheck = false;
			}
		}

		if (bCheck)
		{
			for (auto& Monster : *MonsterList)
			{
				if (OBJSTATE::DISABLE == Monster->Get_State())
					continue;

				CTransform* pMonsterTransformCom = (CTransform*)Monster->Get_Component(TEXT("Com_Transform"));
				_vector vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE::POSITION);
				_float fDistance = 0.f;

				_vector vDistance = vMonsterPos - vPlayerPos;
				fDistance = XMVectorGetX(XMVector3Length(vDistance));

				if (fDistance >= 50.f)
					continue;
				else
				{
					LENGTHINFO tInfo;
					tInfo.pGameObject = Monster;
					tInfo.fLength = fDistance;
					TargetList.emplace_back(tInfo);
				}
			}
		}

		bCheck = true;

		list<CGameObject*>* BossList = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelID(), L"Boss");

		if (BossList == nullptr)
		{
			bCheck = false;
		}

		if (BossList != nullptr && 0 >= BossList->size())
		{
			bCheck = false;
		}

		if (bCheck)
		{
			for (auto& Boss : *BossList)
			{
				if (OBJSTATE::DISABLE == Boss->Get_State())
					continue;

				CTransform* pMonsterTransformCom = (CTransform*)Boss->Get_Component(TEXT("Com_Transform"));
				_vector vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE::POSITION);
				_float fDistance = 0.f;

				_vector vDistance = vMonsterPos - vPlayerPos;
				fDistance = XMVectorGetX(XMVector3Length(vDistance));

				if (fDistance >= 50.f)
					continue;
				else
				{
					LENGTHINFO tInfo;
					tInfo.pGameObject = Boss;
					tInfo.fLength = fDistance;
					TargetList.emplace_back(tInfo);
				}
			}
		}

		if (0 >= TargetList.size())
		{
			RELEASE_INSTANCE(CGameInstance);
			m_pCurrentTarget = nullptr;

			m_eAndroidState = ANDROIDSTATE::IDLE;
			m_eAnimState = ANIMSTATE::IDLE;

			m_iAnimation = ANIM_9S_IDLE;

			m_bLookLerp = true;
			m_LookLerpTime = 0.0;
			m_bContinue = false;

			Set_EquipWeaponIdle();
			
			return;
		}

		TargetList.sort(CompareLength<LENGTHINFO>);

		m_pCurrentTarget = TargetList.front().pGameObject;
		Safe_AddRef(m_pCurrentTarget);

		TargetList.clear();


		if (m_pCurrentTarget != nullptr && m_pCurrentTarget->Get_State() == OBJSTATE::DEAD)
		{
			Safe_Release(m_pCurrentTarget);
			m_pCurrentTarget = nullptr;

			m_bLookLerp = true;
			m_LookLerpTime = 0.0;

			m_iAnimation = ANIM_RUN;
			m_eAnimState = ANIMSTATE::RUN;

			Set_EquipWeaponIdle();
		}

		RELEASE_INSTANCE(CGameInstance);
	}
	else if (ANDROIDSTATE::IDLE == m_eAndroidState && nullptr == m_pCurrentTarget)
		Set_EquipWeaponIdle();
}

void CAndroid_9S::Look_Target(_double TimeDelta)
{
	if (ANDROIDSTATE::BATTLE == m_eAndroidState)
	{
		CTransform* pTargetTransform = (CTransform*)m_pCurrentTarget->Get_Component(L"Com_Transform");
		if (nullptr == pTargetTransform)
		{
			m_LookLerpTime = 0.0;
			m_bLookLerp = false;

			return;
		}

		m_LookLerpTime += (TimeDelta * 2);
		if (m_LookLerpTime > 1.0)
			m_LookLerpTime = 1.0;

		_vector vTargetPos = pTargetTransform->Get_State(CTransform::STATE::POSITION);
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPos));

		_vector vRight = XMVectorZero();
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		_vector vScale = m_pTransform->Get_Scale();

		_vector vDir = XMVector3Normalize(vTargetPos - vPos);
		vDir = XMVectorLerp(vLook, vDir, (_float)m_LookLerpTime);
		vDir = XMVector3Normalize(vDir);

		vRight = XMVector3Cross(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP)), vDir);

		vRight *= XMVectorGetX(vScale);
		vDir *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vDir);

		if (false == m_bLookLerp)
			m_LookLerpTime = 0.0;
	}
}

void CAndroid_9S::Update_Pod()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"bone-1");

	_matrix WorldMatrix = BoneMatrix * m_pTransform->Get_WorldMatrix();
	_float4x4 TempFloat4x4;

	XMStoreFloat4x4(&TempFloat4x4, WorldMatrix);

	_float4 vPos;
	memcpy(&vPos, &TempFloat4x4.m[3], sizeof(_float4));

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCameraTransform = pCamera->Get_Transform();
	if (nullptr == pCameraTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix CamMatrix = pCamera->Get_WorldMatrix();
	_float4x4 CamFloat4x4;

	XMStoreFloat4x4(&CamFloat4x4, CamMatrix);

	_float4	vCamPosFloat4;
	memcpy(&vCamPosFloat4, &CamFloat4x4.m[3], sizeof(_float4));
	_vector vCamPos = XMLoadFloat4(&vCamPosFloat4);

	vPos.y += 1.2f;

	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	vRight = XMVector3Normalize(vRight);

	XMStoreFloat4(&vPos, (XMLoadFloat4(&vPos) + vRight * 1.1f));

	if (CMainCamera::OPTION::COASTER != pCamera->Get_Option())
		m_pPod->Update_Position(XMLoadFloat4(&vPos));
	else if (CMainCamera::OPTION::COASTER == pCamera->Get_Option())
		m_pPod->Update_PositionForCoaster(XMLoadFloat4(&vPos));

	RELEASE_INSTANCE(CGameInstance);
}

void CAndroid_9S::Update_Pod_Immediately()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"bone-1");

	_matrix WorldMatrix = BoneMatrix * m_pTransform->Get_WorldMatrix();
	_float4x4 TempFloat4x4;

	XMStoreFloat4x4(&TempFloat4x4, WorldMatrix);

	_float4 vPos;
	memcpy(&vPos, &TempFloat4x4.m[3], sizeof(_float4));

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCameraTransform = pCamera->Get_Transform();
	if (nullptr == pCameraTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix CamMatrix = pCamera->Get_WorldMatrix();
	_float4x4 CamFloat4x4;

	XMStoreFloat4x4(&CamFloat4x4, CamMatrix);

	_float4	vCamPosFloat4;
	memcpy(&vCamPosFloat4, &CamFloat4x4.m[3], sizeof(_float4));
	_vector vCamPos = XMLoadFloat4(&vCamPosFloat4);

	vPos.y += 1.2f;

	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	vRight = XMVector3Normalize(vRight);

	XMStoreFloat4(&vPos, (XMLoadFloat4(&vPos) + vRight * 1.1f));

	m_pPod->Update_Position(XMLoadFloat4(&vPos));
	m_pPod->Set_Position_Immediately(XMLoadFloat4(&vPos));

	RELEASE_INSTANCE(CGameInstance);
}

void CAndroid_9S::Update_Weapon_Immediately()
{
	_float4x4 WorldFloat4x4;
	XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
	_float4x4 BoneFloat4x4;
	XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

	_vector vDir;
	memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

	_vector vUp;
	memcpy(&vUp, &BoneFloat4x4.m[1], sizeof(_vector));

	vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));
	vUp = XMVector3Normalize(XMVector3TransformNormal(vUp, XMLoadFloat4x4(&WorldFloat4x4)));

	_float4 vPos;
	memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * 0.54f));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vUp * -0.6f));

	memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

	m_pSpear->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
}

void CAndroid_9S::Key_Check()
{
	if (ANDROIDSTATE::HACKING == m_eAndroidState)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (pGameInstance->Key_Down(DIK_P))
	{
		if (ANDROIDSTATE::BATTLE != m_eAndroidState)
		{
			m_eAndroidState = ANDROIDSTATE::BATTLE;
			CSoundMgr::Get_Instance()->PlaySound(L"9S_Patrol.mp3", CSoundMgr::CHANNELID::VOICE);
		}
		else
		{
			if (nullptr != m_pCurrentTarget)
			{
				Safe_Release(m_pCurrentTarget);
				m_pCurrentTarget = nullptr;
			}
				
			m_eAndroidState = ANDROIDSTATE::IDLE;
		}
	}

	if (false == m_pTransform->IsJump() && ANIMSTATE::HIT != m_eAnimState)
	{
		if (pGameInstance->Key_Down(DIK_H) && (_uint)LEVEL::OPERABACKSTAGE == pGameInstance->Get_LevelID())
		{
			if (true == IsOpenable_MiniGame() && false == m_bHackingOnce)
			{
				CSoundMgr::Get_Instance()->PlaySound(L"2B_CallHacking.mp3", CSoundMgr::CHANNELID::VOICE);

				m_eAndroidState = ANDROIDSTATE::HACKING;
				m_iAnimation = ANIM_9S_HACKING;

				m_bContinue = false;
				m_bHackingOnce = true;
			}
		}
	}
}

void CAndroid_9S::Check_HackingState(_double TimeDelta)
{
	if (ANDROIDSTATE::HACKING == m_eAndroidState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (ANIM_9S_HACKING == iCurAnimIndex && m_iAnimation == iCurAnimIndex)
		{
			_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

			m_LookLerpTime = 0.0;

			Set_EquipWeaponIdle();

			LerpLookToBoss(TimeDelta);

			if (LinkMaxRatio <= PlayTimeRatio && m_BossLerpTime >= 1.0)
			{
				Open_MiniGame();

				m_eAndroidState = ANDROIDSTATE::IDLE;

				m_eAnimState = ANIMSTATE::IDLE;
				m_iFlagUI = 0x00000000;
			}
		}
	}
}

void CAndroid_9S::LerpLookToBoss(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CTransform* pBossTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::OPERABACKSTAGE, L"Boss", COM_KEY_TRANSFORM, 0);
	if (nullptr == pBossTransform)
		return;

	if (m_BossLerpTime >= 0.4 && false == m_bHackingFirstSound)
	{
		CSoundMgr::Get_Instance()->PlaySound(L"9S_HackingStart.mp3", CSoundMgr::CHANNELID::VOICE2);
		m_bHackingFirstSound = true;
	}

	m_BossLerpTime += TimeDelta;

	if (m_BossLerpTime >= 1.0)
		m_BossLerpTime = 1.0;

	_vector vBossPos = pBossTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vBossPos = XMVectorSetY(vBossPos, XMVectorGetY(vMyPos));

	_vector vDir = XMVector3Normalize(vBossPos - vMyPos);

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_BossLerpTime)));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetZ(vScale));
}

void CAndroid_9S::Set_Fix(_bool bCheck)
{
	if (bCheck)
	{
		m_iAnimation = ANIM_9S_IDLE;
		m_eAndroidState = ANDROIDSTATE::IDLE;

		m_bFix = true;
		m_bContinue = true;
	}
	else
		m_bFix = false;
}

HRESULT CAndroid_9S::Update_Collider()
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

void CAndroid_9S::LerpToHitTarget(_double TimeDelta)
{
	if (m_HitLerpTime >= 1.0)
	{
		m_HitLerpTime = 1.0;
		return;
	}

	m_HitLerpTime += TimeDelta * 4.0;

	if (m_HitLerpTime >= 1.0)
		m_HitLerpTime = 1.0;

	_vector vTargetPos = XMLoadFloat4(&m_vHitTargetPos);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPos));

	_vector vLook = XMVector3Normalize(vTargetPos - vPos);

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)), vLook, _float(m_HitLerpTime)));

	_vector vScale = m_pTransform->Get_Scale();
	_vector vRight = XMVectorZero();

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CAndroid_9S::Check_JumpState_ForHit(_double TimeDelta)
{
	if (m_pTransform->IsJump())
	{
		if (nullptr != m_pPod)
		{
			if (PODSTATE::IDLE != m_pPod->Get_PodState())
				m_pPod->Set_PodState(PODSTATE::IDLE);
		}

		m_pTransform->Jump(TimeDelta, m_pNavigation);

		_float fY = 0.f;
		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			vPos = XMVectorSetY(vPos, fY);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

			m_pTransform->Initialize_JumpTime();
			m_pTransform->Set_Jump(false);
		}
	}
}

void CAndroid_9S::Check_HitState(_double TimeDelta)
{
}

void CAndroid_9S::Check_AndroidDataPacket(const ANDROID_DATAPACKET & tDataPacket)
{
	if (ANIMSTATE::JUMPLANDING == m_eAnimState)
		return;

	if (m_bFix)
		return;

	if (tDataPacket.bJump)
	{
		if (m_tAndroidDataPacket.bJump || m_tAndroidDataPacket.bDoubleJump || m_bDoubleJump)
			return;

		m_tAndroidDataPacket.bJump = true;
		m_tAndroidDataPacket.bDoubleJump = false;

		m_bContinue = false;

		m_UpdateTime = 0.0;

		m_LookLerpTime = 0.0;
		m_pModel->Initialize_Time(ANIM_9S_JUMP);
	}
	else if (tDataPacket.bDoubleJump)
	{
		if (!m_pTransform->IsJump() || m_bDoubleJump)
			return;

		m_tAndroidDataPacket.bDoubleJump = true;
		m_tAndroidDataPacket.bJump = false;
		m_bDoubleJump = true;

		m_bContinue = false;

		m_UpdateTime = 0.0;

		m_LookLerpTime = 0.0;
	}
}

void CAndroid_9S::Update_UnbeatableTime(_double TimeDelta)
{
	m_UnbeatableTime += TimeDelta;
}

void CAndroid_9S::Set_EquipWeaponIdle()
{
	if (nullptr == m_pSpear)
		return;

	m_pSpear->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pSpear->Set_NoneBlend(1);
}

void CAndroid_9S::Set_Teleport(_bool bCheck)
{
	if (m_bFix)
		return;

	m_bTeleport = bCheck;

	if (nullptr != m_pPod && nullptr != m_pSpear)
	{
		m_pPod->Set_Teleport(bCheck);
		m_pSpear->Set_Teleport(bCheck);
	}
}

void CAndroid_9S::Set_Teleport_CurrentIndex(const _uint & iIndex)
{
	if (m_pNavigation == nullptr ||
		iIndex >= m_pNavigation->Get_Size())
		return;

	m_pNavigation->Init_Index(iIndex);
}

void CAndroid_9S::Check_WeaponState()
{
	if (m_bTeleport)
		return;

	if (m_pSpear)
	{
		if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState())
			m_pSpear->Update_Matrix(m_pTransform->Get_WorldMatrix());
		else if (WEAPONSTATE::IDLE == m_pSpear->Get_WeaponState())
		{
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

			_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
			_float4x4 BoneFloat4x4;
			XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

			_vector vDir;
			memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

			_vector vUp;
			memcpy(&vUp, &BoneFloat4x4.m[1], sizeof(_vector));

			vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));
			vUp = XMVector3Normalize(XMVector3TransformNormal(vUp, XMLoadFloat4x4(&WorldFloat4x4)));

			_float4 vPos;
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

			XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * 0.54f));
			XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vUp * -0.6f));

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

			m_pSpear->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
		}
	}
}

void CAndroid_9S::Check_Teleport(_double TimeDelta)
{
	if (true == m_bNoneTeleport)
		return;

	if (m_bTeleport)
	{
		if (ANIM_9S_IDLE != m_pModel->Get_CurrentAnimation())
		{
			m_iAnimation = ANIM_9S_IDLE;
			m_pModel->Immediately_Change(ANIM_9S_IDLE);
			m_eAnimState = ANIMSTATE::IDLE;
		}

		if (!m_bAppearance)
			m_fAlpha -= _float(TimeDelta) * 10; // Teleport Delay
		else
			m_fAlpha += _float(TimeDelta) * 10; // Teleport Delay

		if (0 >= m_fAlpha)
		{
			m_fAlpha = 0.f;

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
			if (nullptr == pTransform)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);
			_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

			_vector vRealPlayerPos = vPlayerPos;
			vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vMyPos));

			_vector vDir = XMVector3Normalize(vMyPos - vPlayerPos);

			vRealPlayerPos += vDir * 2.f;

			m_pTransform->Set_State(CTransform::STATE::POSITION, vRealPlayerPos);

			RELEASE_INSTANCE(CGameInstance);

			m_bAppearance = true;

			m_pPod->Set_Appearance(true);
			m_pSpear->Set_Appearance(true);

			_vector vLook = XMVector3Normalize(vPlayerPos - vMyPos);
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vScale = m_pTransform->Get_Scale();

			_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

			vRight *= XMVectorGetX(vScale);
			vLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);

			Update_Pod_Immediately();
			Update_Weapon_Immediately();
			Create_TeleportEffect();
		}
		else if (m_fAlpha >= 1.f && m_bAppearance)
		{
			m_fAlpha = 1.f;

			if (m_bAppearance)
			{
				m_bAppearance = false;
				m_bTeleport = false;
			}
		}
	}
}

void CAndroid_9S::Check_UpdateTime(_double TimeDelta)
{
	if (ANDROIDSTATE::BATTLE == m_eAndroidState || ANDROIDSTATE::HACKING == m_eAndroidState)
	{
		ZeroMemory(&m_tAndroidDataPacket, sizeof(ANDROID_DATAPACKET));
		return;
	}

	if (m_tAndroidDataPacket.bJump || m_tAndroidDataPacket.bDoubleJump)
	{
		m_UpdateTime += TimeDelta;

		if (m_UpdateTime >= m_UpdateDelayTime)
		{
			if (m_tAndroidDataPacket.bJump)
			{
				if (m_tAndroidDataPacket.bImmediately)
				{

					if (ANIMSTATE::JUMPLANDING == m_eAnimState)
						m_pModel->Immediately_Change(ANIM_9S_JUMP);
					else
						return;
				}

				m_eAnimState = ANIMSTATE::JUMP;

				m_pTransform->Set_Jump(true);
				m_pTransform->Initialize_JumpTime();

 				m_iAnimation = ANIM_9S_JUMP;

				m_bContinue = false;
				m_bLookLerp = true;

			}
			else if (m_tAndroidDataPacket.bDoubleJump)
			{
				m_eAnimState = ANIMSTATE::JUMP;

				m_pTransform->Set_Jump(true);
				m_pTransform->Initialize_JumpTime();

				m_iAnimation = ANIM_9S_DOUBLEJUMP;

				if (m_tAndroidDataPacket.bImmediately)
					m_pModel->Immediately_Change(ANIM_9S_DOUBLEJUMP);

				m_bContinue = false;
				m_bLookLerp = false;
			}

			ZeroMemory(&m_tAndroidDataPacket, sizeof(ANDROID_DATAPACKET));
			m_UpdateTime = 0.0;
		}
	}
}

void CAndroid_9S::Check_JumpState(_double TimeDelta)
{
	if (ANIMSTATE::JUMP == m_eAnimState || m_pTransform->IsJump())
	{
		m_pTransform->Jump(TimeDelta);
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);
		if (nullptr == pPlayer)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (pPlayer->Get_WASDKey())
			m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED, m_pNavigation);
		
		RELEASE_INSTANCE(CGameInstance);
		_float fY = 0.f;
		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			vPos = XMVectorSetY(vPos, fY);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			m_pTransform->Set_Jump(false);
			m_pTransform->Initialize_JumpTime();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
			if (nullptr == pPlayerTransform)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
			_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

			vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vMyPos));

			_vector vDist = XMVector3Length(vPlayerPos - vMyPos);

			if (false == m_tAndroidDataPacket.bJump && false == m_tAndroidDataPacket.bDoubleJump)
  				m_iAnimation = ANIM_9S_JUMPLANDING;

			m_bContinue = false;
			m_eAnimState = ANIMSTATE::JUMPLANDING;

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CAndroid_9S::Set_Spear(CSpear * pSpear)
{
	if (nullptr != pSpear)
	{
		m_pSpear = pSpear;
		m_pSpear->Set_WeaponEquipFor9S(WEAPONEQUIP::EQUIP);
	}
}

void CAndroid_9S::Set_Pod(CPod9S * pPod)
{
	if (nullptr != pPod)
		m_pPod = pPod;
}

_bool CAndroid_9S::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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
		if (pGameObject->Get_WholeRangeAttack())
		{
			if (true == m_pTransform->IsJump())
				return false;
		}

		if (m_pHitBox->Collision_AABBToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::RAY)
	{
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
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT0)
	{
		// 10 ~ 11
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);

		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 12.f || fDist < 9.f)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT1)
	{
		// 15 ~ 16.5
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);

		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 17.25f || fDist < 14.25f)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT2)
	{
		// 20 ~ 22
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);

		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 23.f || fDist < 19.f)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::BEAUVOIR_RING)
	{
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vRingOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vRingOrigin));

		_vector vScale = pEffect->Get_RingScale(CTransform::STATE::RIGHT);
		_float fScale = XMVectorGetX(XMVector3Length(vScale));

		_vector vDir = XMVector4Normalize(vPosition - vRingOrigin);
		if (m_pHitBox->Collision_AABB(vRingOrigin, vDir, fDist))
		{
			if (fDist > 44.f * fScale || fDist < 36.f * fScale)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::ZHUANGZI_RING)
	{
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vRingOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vRingOrigin));

		_vector vScale = pEffect->Get_RingScale(CTransform::STATE::RIGHT);
		_float fScale = XMVectorGetX(XMVector3Length(vScale));

		_vector vDir = XMVector4Normalize(vPosition - vRingOrigin);
		if (m_pHitBox->Collision_AABB(vRingOrigin, vDir, fDist))
		{

			if (fDist > 0.61f * fScale || fDist < 0.59f * fScale)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	return false;
}

void CAndroid_9S::Collision(CGameObject * pGameObject)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		if (m_UnbeatableTime >= m_UnbeatableDelayTime)
		{
			if (ANIM_9S_HIT_FRONT != iCurAnimIndex && ANIMSTATE::HIT != m_eAnimState && ANDROIDSTATE::HACKING != m_eAndroidState)
			{
				CCollider* pCollider = (CCollider*)pGameObject->Get_Component(COM_KEY_ATTACKBOX);
				if (nullptr == pCollider)
					return;

				_uint iDamage = 0;

				if (HIT::NONE != pGameObject->Get_HitType())
				{
					if (pCollider->Result_SphereToAABB(pGameObject, this, m_pHitBox, &iDamage))
					{
						m_iFlagUI |= FLAG_UI_CALL_HPBAR;
						m_iFlagUI |= FLAG_UI_CALL_DAMAGE;

						m_dCallHPBarUITimeDuration = 3.0;
						m_tObjectStatusDesc.fHp -= iDamage;
						m_iGetDamage = iDamage;

						if (m_tObjectStatusDesc.fHp < 0)
							m_tObjectStatusDesc.fHp = 0;

						_vector vTargetPos = pCollider->Get_WorldPos(CCollider::TYPE::SPHERE);
						vTargetPos = XMVectorSetW(vTargetPos, 1.f);

						m_iAnimation = ANIM_9S_HIT_FRONT;
						m_eAnimState = ANIMSTATE::HIT;
						m_bContinue = false;

						m_HitLerpTime = 0.0;
						XMStoreFloat4(&m_vHitTargetPos, vTargetPos);

						Set_EquipWeaponIdle();

						if (m_pTransform->IsJump())
							m_pTransform->Set_FallTime();
					}
				}
			}
		}
	}

	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (m_UnbeatableTime >= m_UnbeatableDelayTime)
		{
			if (ANIM_9S_HIT_FRONT != iCurAnimIndex && ANIMSTATE::HIT != m_eAnimState && ANDROIDSTATE::HACKING != m_eAndroidState)
			{
				m_iFlagUI |= FLAG_UI_CALL_HPBAR;
				m_iFlagUI |= FLAG_UI_CALL_DAMAGE;

				m_dCallHPBarUITimeDuration = 3.0;
				m_tObjectStatusDesc.fHp -= pGameObject->Get_RandomAtt();

				if (m_tObjectStatusDesc.fHp < 0)
					m_tObjectStatusDesc.fHp = 0;

				m_iAnimation = ANIM_9S_HIT_FRONT;
				m_eAnimState = ANIMSTATE::HIT;
				m_bContinue = false;

				Set_EquipWeaponIdle();
			}
		}
	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CAndroid_9S::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET* pPacket = (PACKET*)pMessage;
		
		ANDROID_DATAPACKET tDataPacket;
		ZeroMemory(&tDataPacket, sizeof(ANDROID_DATAPACKET));

		memcpy(&tDataPacket, pPacket->pData, sizeof(ANDROID_DATAPACKET));

		Check_AndroidDataPacket(tDataPacket);
	}
}

_int CAndroid_9S::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
		return FALSE;
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check != CHECKSUM_9S_ANDROID_DATAPACKET)
			return FALSE;
	}

	return TRUE;
}

HRESULT CAndroid_9S::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		//
	}
	if (m_iFlagUI & FLAG_UI_CALL_DAMAGE)
	{
		CUI_HUD_Damage::UIDAMAGEDESC	desc;

		desc.eType = CUI_HUD_Damage::DAMAGETYPE::PLAYER;
		desc.iDamageAmount = m_iGetDamage;
		XMStoreFloat4(&desc.vWorldPos, m_pMiddlePoint->Get_WorldPos(CCollider::TYPE::SPHERE));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

		RELEASE_INSTANCE(CGameInstance);

		m_iFlagUI ^= FLAG_UI_CALL_DAMAGE;
	}

	return S_OK;
}

HRESULT CAndroid_9S::Create_TeleportEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));

	tPacket.ulID = CHECKSUM_EFFECT_9S_APPEAR;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\9S_Appear.dat", &tPacket);

	return S_OK;
}

HRESULT CAndroid_9S::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_9S, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"bone-1", "bone-1");
	m_pModel->Add_RefNode(L"Weapon_Idle", "bone3153");
	m_pModel->Add_RefNode(L"Weapon_Play_Left", "bone516");
	m_pModel->Add_RefNode(L"Weapon_Play_Right", "bone260");

	/* For.Com_Navigation*/
	_uint		iCurrentIndex = 0;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	Engine::CCollider::DESC ColliderDesc;

	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.fRadius = m_pModel->Get_Radius();
	ColliderDesc.vScale = _float3(PLAYER_COLLIDER_HITBOX);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CAndroid_9S::SetUp_Observer()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CObserver* pObserver = nullptr;
	pGameInstance->Create_Observer(L"OBSERVER_9S", this);

	pGameInstance->Get_Subject(L"SUBJECT_PLAYER")->Add_Observer(pGameInstance->Get_Observer(L"OBSERVER_9S"));

	return S_OK;
}

HRESULT CAndroid_9S::SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex)
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex)))
	{
		return E_FAIL;
	}
	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_AmusementPark()))
			return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	m_pNavigation->Init_Index(iNavigationIndex);

	return S_OK;
}

HRESULT CAndroid_9S::SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex)
{
	Safe_Release(m_pNavigation);

	_vector vPos = OPERA_START_POSITION;
	vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + 2.f);

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_OperaBackStage()))
			return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	m_pNavigation->Init_Index(iNavigationIndex);

	//m_bNoneTeleport = false;
	//m_bTeleport = false;
	m_bAppearance = false;
	m_fAlpha = 1.f;

	return S_OK;
}

HRESULT CAndroid_9S::SceneChange_ZhuangziStage(const _vector & vPosition, const _uint & iNavigationIndex)
{
	/*Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		m_mapComponents.erase(iter);
	}*/
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_ZhuangziStage()))
			return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	m_pNavigation->Init_Index(iNavigationIndex);
	m_pNavigation->Init_Index(vPosition);

	return S_OK;
}

HRESULT CAndroid_9S::SceneChange_ROBOTGENERAL(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	Safe_Release(iter->second);
	m_mapComponents.erase(iter);

	_uint iIndex = 0;
	if(FAILED(Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex)))
		return E_FAIL;

	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_ROBOTGENERAL()))
			return E_FAIL;
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
	m_pNavigation->Init_Index(iNavigationIndex);

	return S_OK;
}

CAndroid_9S * CAndroid_9S::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CAndroid_9S* pGameInstance = new CAndroid_9S(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CAndroid_9S");
	}

	return pGameInstance;
}

CGameObject * CAndroid_9S::Clone(void * pArg)
{
	CAndroid_9S* pGameInstance = new CAndroid_9S(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CAndroid_9S");
	}

	return pGameInstance;
}

void CAndroid_9S::Free()
{
	if (m_pCurrentTarget)
		Safe_Release(m_pCurrentTarget);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pModel);

	__super::Free();

}
