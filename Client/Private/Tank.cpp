#include "stdafx.h"
#include "..\Public\Tank.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "MainCamera.h"
#include "Player.h"
#include "UI_Dialogue_EngageMessage.h"
#include "Normal_Attack.h"
#include "SoundMgr.h"
#include "EffectFactory.h"
#include "MeshEffect.h"
#include "DecalCube.h"

CTank::CTank(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CTank::CTank(const CTank & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CTank::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTank::NativeConstruct(void * pArg)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, L"Tank", COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Right_Arm", "bone560");
	m_pModel->Add_RefNode(L"Left_Arm", "bone304");
	m_pModel->Add_RefNode(L"Turret", "bone020");
	m_pModel->Add_RefNode(L"MoningStar", "bone173");
	m_pModel->Add_RefNode(L"Arm_Both", "bone304");


	CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = _float3(0.f, 1.f, 0.f);
	tColliderDesc.vPivot.y += 0.4f;
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;
	//tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.vPivot = _float3(0.f, 1.f, 0.f);
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(4.5f, 4.f, 4.5f);

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;


	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = 1.f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_MStar"), (CComponent**)&m_MoningStar, &tColliderDesc)))
		return E_FAIL;
	tColliderDesc.fRadius = 1.5f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_LArm"), (CComponent**)&m_LeftArm, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.fRadius = 1.5f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_RArm"), (CComponent**)&m_RightArm, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.fRadius = 12.f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_Turret"), (CComponent**)&m_Turret, &tColliderDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MO_INFO* pInfo = (MO_INFO*)pArg;
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	}


	m_iPassIndex = static_cast<_int>(MESH_PASS_INDEX::ANIM);

	m_iAnimIndex = TANK_FOWARD;

	m_bCollision = true;

	static _uint iCTank_InstanceID = OriginID_Tank;
	m_iObjID = iCTank_InstanceID++;

	m_tObjectStatusDesc.fHp = TANK_MAX_HP;
	m_tObjectStatusDesc.fMaxHp = TANK_MAX_HP;
	m_tObjectStatusDesc.iAtt = TANK_ATTACK_DAMAGE;
	Check_Pause();


	Set_State(OBJSTATE::DISABLE);

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	m_fDissolveVal = 0.f;
	m_fDissolveSpeed = TANK_DISSOLVE_SPEED;

	/* For.Com_DissolveTexture*/
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	return S_OK;
}

_int CTank::Tick(_double dTimeDelta)
{
	if (m_bPause)
		dTimeDelta = 0.0;

	if (m_eState == OBJSTATE::DISABLE)
	{
		Compute_CamDistanceForAlpha(m_pTransform);
		if (CGameInstance::Get_Instance()->Culling(m_pTransform, m_pModel->Get_Radius())
			&& m_fCameraDistance < 75.f)
		{
			//CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

			//pSoundMgr->StopSound(CSoundMgr::BGM);
			//pSoundMgr->Set_Volume(CSoundMgr::BGM2, 0.5f);
			//pSoundMgr->PlayLoopSound(TEXT("Chariot_Battle_BGM.mp3"), CSoundMgr::CHANNELID::BGM2);
			//pSoundMgr->Set_Volume(CSoundMgr::BGM2, 0.5f);
			Set_State(OBJSTATE::ENABLE);
		}
		return (_int)m_eState;
	}
	
	if (m_eState == OBJSTATE::DEAD)
	{
		//CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

		//tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::CHARIOT;
		//tObjectiveDesc.iEventCount = 1;

		//CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));

		//CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

		//pSoundMgr->StopSound(CSoundMgr::BGM2);
		//pSoundMgr->FadeIn_Volume(CSoundMgr::BGM);

		return (_int)m_eState;
	}


	_float fTimeDelta = static_cast<_float>(dTimeDelta);

	Update_Bone();
	Check_Target(dTimeDelta);
	Chase(dTimeDelta);
	Check_AnimState(dTimeDelta);

	if (m_iAnimIndex == TANK_IDLE || m_iAnimIndex == TANK_FOWARD)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_0))
	{
		m_iPatternNum = 3;
	}

	Update_Collider();

	Update_UI(dTimeDelta);

	Update_Effects(fTimeDelta);

	Update_NormalAttack();

	Update_CollisionTime(dTimeDelta);

	RELEASE_INSTANCE(CGameInstance);
	



	return (_uint)m_eState;
}

_int CTank::LateTick(_double dTimeDelta)
{
	if (m_bPause)
		dTimeDelta = 0.0;

	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		m_iAnimIndex = TANK_DESTROY;
		m_eAnimState = M_ANIMSTATE::DESTROYED;
		m_ePattern =  M_PATTERN::DESTROYED;      
		m_iPatternNum = 4;
		//m_fDissolveVal = 0.f;
		m_bTraceFast = false;
		//m_eState = OBJSTATE::DEAD;
	}

	if (m_eState == OBJSTATE::DISABLE)
		return (_int)m_eState;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);


	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	_matrix vecRPos;
	vecRPos =XMLoadFloat4x4(&m_BothPos);

	//m_RightArm->Update(vecRPos);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_RightArm);
	//m_LeftArm->Update(vecLPos);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_LeftArm);
	//m_MoningStar->Update(vecMStarPos);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_MoningStar);
	//m_Turret->Update(vecTurret);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_Turret);
	if (m_eState == OBJSTATE::DISABLE)
		dTimeDelta = 0.0;

	Check_Times(dTimeDelta);

	return 0;
}

HRESULT CTank::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (m_ePattern == M_PATTERN::DESTROYED)
	{
		m_pModel->SetUp_RawValue("g_fDissolve", &m_fDissolveVal, sizeof(_float));
		m_pModel->SetUp_Texture("g_texDissolve", m_pDissolveTexture->Get_SRV(0));
	}


	return S_OK;
}

void CTank::Chase(_double TimeDelta)
{
	if ((m_fDistance > 0.1f && m_eAnimState != M_ANIMSTATE::RUSH && !m_bTraceFast && m_iAnimIndex != TANK_RUSH
		&& m_eAnimState != M_ANIMSTATE::SHOOTING && m_eAnimState != M_ANIMSTATE::MONINGSTAR && !m_bCamHold && m_eAnimState != M_ANIMSTATE::DESTROYED))
	{
		//if ((m_pModel->Get_CurrentAnimation() == 12 && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		//	|| (m_pModel->Get_CurrentAnimation() == 60 && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		//	|| (m_pModel->Get_CurrentAnimation() == 34 && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		//	|| (m_pModel->Get_CurrentAnimation() == 61 && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		//	|| (m_pModel->Get_CurrentAnimation() == 22 && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio()))
		//{
		//	_float fTime = m_pModel->Get_LinkMinRatio();
		//	_float fTime2 = m_pModel->Get_PlayTimeRatio();
		//	return;
		//}
		m_fRatio += (_float)(TimeDelta * 0.3f);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_vector vPlayerPos = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(
			(_uint)LEVEL::STATIC, TEXT("LayerPlayer"), COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION);
		_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fPlayerY = XMVectorGetY(vPlayerPos);
		vMonsterPos = XMVectorSetY(vMonsterPos, fPlayerY);

		_vector vMonsterRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
		_vector vMonsterUp = { 0.f , 1.f , 0.f , 0.f };
		_vector vScale = m_pTransform->Get_Scale();

		_vector vMonsterLook = vPlayerPos - vMonsterPos;

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

	if (m_bTraceFast)
	{
		_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
		vLook = XMVector3Normalize(vLook);

		vLook *= -1.f;
		
		_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP));
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

		_vector vScale = m_pTransform->Get_Scale();

		vLook *= XMVectorGetZ(vScale);
		vRight *= XMVectorGetX(vScale);

		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);

		if (m_bTraceFast)
		{
			m_bTraceFast = false;
			m_pModel->Set_NoneBlend();
			m_iAnimIndex = TANK_TURN_IDLE;
		}

	}
	
}

void CTank::Check_Target(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_vector vPlayerPos = dynamic_cast<CTransform*>(pGameInstance->Get_ComponentPtr(
		(_uint)LEVEL::STATIC, TEXT("LayerPlayer"), COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION);
	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vPlayerPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;

	if (fDist < 30.f && fDist > 4.f)
	{
		if (	m_eAnimState != M_ANIMSTATE::SWEEP_L && m_eAnimState != M_ANIMSTATE::SWEEP_R
			&& m_eAnimState != M_ANIMSTATE::STAMP_L && m_eAnimState != M_ANIMSTATE::STAMP_R && m_eAnimState != M_ANIMSTATE::STAMP_B
			&& m_eAnimState != M_ANIMSTATE::RUSH
			&& m_eAnimState != M_ANIMSTATE::SHOOTING && m_eAnimState != M_ANIMSTATE::MONINGSTAR 
			&& m_eAnimState != M_ANIMSTATE::DESTROYED)

		m_iAnimIndex = TANK_FOWARD; // foward
	}

	else if (fDist <= 10.f && m_eAnimState != M_ANIMSTATE::DESTROYED)
	{
		m_bPlayPattern = true;
	}

	if (fDist <= 4.f &&	m_eAnimState != M_ANIMSTATE::SWEEP_L && m_eAnimState != M_ANIMSTATE::SWEEP_R
		&& m_eAnimState != M_ANIMSTATE::STAMP_L && m_eAnimState != M_ANIMSTATE::STAMP_R && m_eAnimState != M_ANIMSTATE::STAMP_B
		&& m_eAnimState != M_ANIMSTATE::RUSH
		&& m_eAnimState != M_ANIMSTATE::SHOOTING && m_eAnimState != M_ANIMSTATE::MONINGSTAR 
		&& m_eAnimState != M_ANIMSTATE::DESTROYED)
	{
		m_bPlayPattern = false;
		m_iAnimIndex = TANK_IDLE;
	}
}

void CTank::Check_AnimState(_double TimeDelta)
{
	if (m_bPlayPattern)
	{
		if (m_ePattern == M_PATTERN::PATTERN_1) // 
		{
			if (m_eAnimState != M_ANIMSTATE::SWEEP_L && m_eAnimState != M_ANIMSTATE::SWEEP_R)
			{
				if (m_iAnimIndex != TANK_LSWEEP && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
				m_eAnimState = M_ANIMSTATE::SWEEP_L;
				m_iAnimIndex = TANK_LSWEEP;
				m_bPlusOnce = false;


				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 6;
					tInfo.Matrix = m_LArmPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_LArmPos;
					m_bIsLArm = true;
					m_bIsRArm = false;
					m_bIsMstar = false;
					m_bIsBoth = false;


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
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_LSWEEP && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_LSWEEP
				&& (m_pModel->Get_LinkMinRatio() > m_pModel->Get_PlayTimeRatio() || m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio()))
			{
				m_bCamHold = false;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_LSWEEP && m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio() && m_iAnimIndex != TANK_RSWEEP)
			{
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_LSWEEP)
			{
				if (m_iAnimIndex != TANK_RSWEEP && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
				m_iAnimIndex = TANK_RSWEEP;
				m_bCamHold = false;
				m_eAnimState = M_ANIMSTATE::SWEEP_R;


				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 7;
					tInfo.Matrix = m_RArmPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_RArmPos;
					m_bIsLArm = false;
					m_bIsRArm = true;
					m_bIsMstar = false;
					m_bIsBoth = false;


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
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_RSWEEP && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_RSWEEP
				&& (m_pModel->Get_LinkMinRatio() > m_pModel->Get_PlayTimeRatio() || m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio()))
			{
				m_bCamHold = false;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_RSWEEP && m_pModel->Get_LinkMinRatio() + 0.4 <= m_pModel->Get_PlayTimeRatio())
			{
				_double dTime = m_pModel->Get_LinkMinRatio();
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_RSWEEP)
			{
				m_iAnimIndex = TANK_FOWARD;
				m_dPatternTime = 0.0;
				m_bCamHold = false;
				if (!m_bPlusOnce)
				{
					m_bPlusOnce = true;
					++m_iPatternNum;
				}
			}


		}
		if (m_ePattern == M_PATTERN::PATTERN_2)
		{
			if (m_eAnimState != M_ANIMSTATE::STAMP_L && m_eAnimState != M_ANIMSTATE::STAMP_R && m_eAnimState != M_ANIMSTATE::STAMP_B)
			{
				if (m_iAnimIndex != TANK_LSTAMP && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
				m_eAnimState = M_ANIMSTATE::STAMP_L;
				m_iAnimIndex = TANK_LSTAMP;
				m_bPlusOnce = false;
				m_bShakeBigOnce = false;
				m_bShakeMidOnce1 = false;
				m_bShakeMidOnce2 = false;


				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 6;
					tInfo.Matrix = m_LArmPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_LArmPos;
					m_bIsLArm = true;
					m_bIsRArm = false;
					m_bIsMstar = false;
					m_bIsBoth = false;

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
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_LSTAMP && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_LSTAMP
				&& (m_pModel->Get_LinkMinRatio() > m_pModel->Get_PlayTimeRatio() || m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio()))

			{
				m_bCamHold = false;

			}

			if ((m_pModel->Get_LinkMaxRatio()) <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_LSTAMP && !m_bShakeMidOnce1)
			{
				CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

				CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
				if (pCamera == nullptr)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				pCamera->Set_ShakeMTrue();
				m_bShakeMidOnce1 = true;

				Set_LCrashPos();
				Create_Smoke(m_LArmPos);

				RELEASE_INSTANCE(CGameInstance);
			}

			if (m_pModel->Get_LinkMaxRatio() + 0.3f <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_LSTAMP && m_iAnimIndex != TANK_RSTAMP)
			{
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_LSTAMP)
			{
				if (m_iAnimIndex != TANK_RSTAMP && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
				m_iAnimIndex = TANK_RSTAMP;
				m_eAnimState = M_ANIMSTATE::STAMP_R;
				m_bCamHold = false;


				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 7;
					tInfo.Matrix = m_RArmPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_RArmPos;

					m_bIsLArm = false;
					m_bIsRArm = true;
					m_bIsMstar = false;
					m_bIsBoth = false;


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
			}

			if ((m_pModel->Get_LinkMaxRatio()) <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_RSTAMP && !m_bShakeMidOnce2)
			{
				CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

				CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
				if (pCamera == nullptr)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				pCamera->Set_ShakeMTrue();
				m_bShakeMidOnce2 = true;

				Set_RCrashPos();
				//Create_Smoke(m_RCrashPos);
				Create_Smoke(m_RArmPos);


				RELEASE_INSTANCE(CGameInstance);

			}


			if (m_pModel->Get_CurrentAnimation() == TANK_RSTAMP && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_RSTAMP
				&& (m_pModel->Get_LinkMinRatio() > m_pModel->Get_PlayTimeRatio() || m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio()))

			{
				m_bCamHold = false;
			}

			if (m_pModel->Get_LinkMinRatio() + 0.3f <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_RSTAMP && m_iAnimIndex != TANK_BCRASH)
			{
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_RSTAMP)
			{
				if (m_iAnimIndex != TANK_BCRASH && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
				m_iAnimIndex = TANK_BCRASH;
				m_eAnimState = M_ANIMSTATE::STAMP_B;
				m_bCamHold = false;
			}

			if ((m_pModel->Get_LinkMaxRatio()) <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_BCRASH && !m_bShakeBigOnce)
			{
				CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

				CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
				if (pCamera == nullptr)
				{
					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				pCamera->Set_ShakeTrue();
				m_bShakeBigOnce = true;

				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 8;
					tInfo.Matrix = m_BothPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_BothPos;
					m_bIsLArm = false;
					m_bIsRArm = false;
					m_bIsMstar = false;
					m_bIsBoth = true;


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

				// DUAL CHECK //
				Set_LCrashPos();
				Set_RCrashPos();
				// Quake_Effect//
				Create_LeftArmSpark();
				Create_RightArmSpark();
				RELEASE_INSTANCE(CGameInstance);
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_BCRASH && m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if (m_pModel->Get_CurrentAnimation() == TANK_BCRASH
				&& (m_pModel->Get_LinkMinRatio() > m_pModel->Get_PlayTimeRatio() || m_pModel->Get_LinkMinRatio() + 0.3 <= m_pModel->Get_PlayTimeRatio()))
			{
				m_bCamHold = false;
				Delete_LeftArmSpark();
				Delete_RightArmSpark();
			}

			if (m_pModel->Get_LinkMinRatio() + 0.3f <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_BCRASH)
			{
				m_bAtkDead = true;
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_BCRASH)
			{
				m_iAnimIndex = TANK_FOWARD;
				m_dPatternTime = 0.0;
				m_bCamHold = false;
				if (!m_bPlusOnce)
				{
					m_bPlusOnce = true;
					++m_iPatternNum;
				}
			}
		}

		if (m_ePattern == M_PATTERN::PATTERN_3)
		{
			if (m_eAnimState != M_ANIMSTATE::RUSH && m_eAnimState != M_ANIMSTATE::IDLE)
			{
				m_eAnimState = M_ANIMSTATE::RUSH;
				m_iAnimIndex = TANK_RUSH;
				m_bPlusOnce = false;
				m_bCamHold = true;
				m_fRatio = 0.f;
			}

			if ((0.97 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_RUSH)
			{
				m_dPatternTime = 0.0;
				if (!m_bPlusOnce)
				{
					m_bPlusOnce = true;
					++m_iPatternNum;
					m_bTraceFast = true;
					m_bCamHold = false;
					m_eAnimState = M_ANIMSTATE::IDLE;
				}
			}
		}

		if (m_ePattern == M_PATTERN::PATTERN_4)
		{
			if (m_eAnimState != M_ANIMSTATE::SHOOTING && m_eAnimState != M_ANIMSTATE::MONINGSTAR)
			{
				if (m_iAnimIndex != TANK_MONINGSTAR && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
					m_bTraceFast = false;
				}

				m_bCamHold = false;
				m_eAnimState = M_ANIMSTATE::SHOOTING;
				m_iAnimIndex = TANK_LASER;
				m_bPlusOnce = false;

			}

			if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_LASER)
			{
				if (!m_bCreateOnce)
				{
					// Laser Effect Create
					// use m_TurretMatrix
					Create_Laser();
					m_bCreateOnce = true;
				}
			}

			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_LASER)
			{
				Delete_Laser();
				// laser dead timing
				// use m_TurretMatrix
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_LASER)
			{
				if (m_iAnimIndex != TANK_MONINGSTAR && m_bCreateOnce)
				{
					m_bCreateOnce = false;
					m_bAtkDead = false;
				}

				m_eAnimState = M_ANIMSTATE::MONINGSTAR;
				m_iAnimIndex = TANK_MONINGSTAR;

				if (!m_bCreateOnce)
				{
					CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

					Humanoid_Atk_Info tInfo;

					tInfo.iNumber = 5;
					tInfo.Matrix = m_MStarPos;
					tInfo.pMaker = this;

					m_AttackMatrix = m_MStarPos;
					m_bIsLArm = false;
					m_bIsRArm = false;
					m_bIsMstar = true;
					m_bIsBoth = false;


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
			}

			if ((0.95 <= m_pModel->Get_PlayTimeRatio()) && m_pModel->Get_CurrentAnimation() == TANK_MONINGSTAR)
			{
				m_bAtkDead = true;
				m_iAnimIndex = TANK_FOWARD;
				m_dPatternTime = 0.0;
				if (!m_bPlusOnce)
				{
					m_bPlusOnce = true;
					m_iPatternNum = 0;
				}
			}
		}

		if (m_ePattern == M_PATTERN::DESTROYED)
		{
   			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == TANK_DESTROY)
			{
				if (m_fDissolveVal > 1.f)
				{
					m_eState = OBJSTATE::DEAD;
				}

			}
		}
	}
}

void CTank::Check_Times(_double TimeDelta)
{ 
	if (m_iPatternNum == 0) 
	{
		m_dPatternTime += TimeDelta;

		if (m_dPatternTime >= 15.0)
		{
			m_ePattern = M_PATTERN::PATTERN_1;
		}
	}

	if (m_iPatternNum == 1) 
	{
		m_dPatternTime += TimeDelta;

		if (m_dPatternTime >= 3.0)
		{
			m_ePattern = M_PATTERN::PATTERN_2;
		}
	}

	if (m_iPatternNum == 2) 
	{
		m_dPatternTime += TimeDelta;

		if (m_dPatternTime >= 4.0)
		{
			m_ePattern = M_PATTERN::PATTERN_3;
		}
	}

	if (m_iPatternNum == 3) 
	{
		m_dPatternTime += TimeDelta;

		if (m_dPatternTime >= 4.0)
		{
			m_ePattern = M_PATTERN::PATTERN_4;
		}
	}

	if (m_iPatternNum == 4)
	{
		m_ePattern = M_PATTERN::DESTROYED;
	}
}

void CTank::Update_Bone()
{
	Update_LeftArm();
	Update_RightArm();
	Update_Turret();
	Update_MoningStar();
	Update_Arms();
}

void CTank::Update_RightArm()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Right_Arm");
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

	vecPos = vecPos + vNormRight * 3.5f;
	vecPos = vecPos + vNormUp * 2.f;
	vecPos = vecPos + vNormLook * -5.f;

	memcpy(&m_RArmPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_RArmPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_RArmPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_RArmPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsRArm)
		m_AttackMatrix = m_RArmPos;

}

void CTank::Update_LeftArm()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Left_Arm");
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

	vecPos = vecPos + vNormRight *-3.5f;
	vecPos = vecPos + vNormUp * 2.f;
	vecPos = vecPos + vNormLook * -5.f;

	memcpy(&m_LArmPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_LArmPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_LArmPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_LArmPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsLArm)
		m_AttackMatrix = m_LArmPos;
}

void CTank::Update_MoningStar()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"MoningStar");
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

	vecPos = vecPos + vNormRight * -1.1f;
	vecPos = vecPos + vNormUp * 3.98f;
	vecPos = vecPos + vNormLook * 15.2f;

	memcpy(&m_MStarPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_MStarPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_MStarPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_MStarPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsMstar)
		m_AttackMatrix = m_MStarPos;
}

void CTank::Update_Turret()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Turret");
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
	vecPos = vecPos + vNormUp * 5.f;
	vecPos = vecPos + vNormLook * -4.5f;

	memcpy(&m_TurretMatrix.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_TurretMatrix.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_TurretMatrix.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_TurretMatrix.m[3], &vecPos, sizeof(_vector));
}

void CTank::Update_Arms()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Arm_Both");
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
	vecPos = vecPos + vNormUp * 2.f;
	vecPos = vecPos + vNormLook * -5.f;

	memcpy(&m_BothPos.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_BothPos.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_BothPos.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_BothPos.m[3], &vecPos, sizeof(_vector));

	if (m_bIsBoth)
		m_AttackMatrix = m_BothPos;
}

void CTank::Set_UI_Engage_Cinematic()
{
	m_iFlagUI |= FLAG_UI_CALL_CINEMATIC;
}

void CTank::Set_UI_Engage()
{
	m_iFlagUI |= FLAG_UI_CALL_ENGAGE;
}

void CTank::Update_NormalAttack()
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

void CTank::Set_LCrashPos()
{
	_float4 vPos;
	memcpy(&vPos, &m_LArmPos.m[3], sizeof(_float4));
	_vector vecPos;
	vecPos = XMLoadFloat4(&vPos);
	_float y = XMVectorGetY(vecPos);
	y -= 1.5f;
	vecPos = XMVectorSetY(vecPos, y);
	XMStoreFloat4(&m_LCrashPos, vecPos);
}

void CTank::Set_RCrashPos()
{
	_float4 vPos;
	memcpy(&vPos, &m_RArmPos.m[3], sizeof(_float4));
	_vector vecPos;
	vecPos = XMLoadFloat4(&vPos);
	_float y = XMVectorGetY(vecPos);
	y -= 1.5f;
	vecPos = XMVectorSetY(vecPos, y);
	XMStoreFloat4(&m_RCrashPos, vecPos);
}


_bool CTank::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CTank::Collision(CGameObject * pGameObject)
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

void CTank::Notify(void * pMessage)
{
}

_int CTank::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CTank::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dDeltaTime;

		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;
		desc.eID = CUI_Dialogue_EngageMessage::BOSSID::CHARIOT;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MONSTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MONSTER_HPBAR, &desc));

		RELEASE_INSTANCE(CGameInstance);

		if (m_dCallHPBarUITimeDuration < 0)
		{
			m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
		}
	}
	if (m_iFlagUI & FLAG_UI_CALL_CINEMATIC)
	{
		static _double	TimeAcc = 0.0;

		if (TimeAcc < 3.5)
		{
			CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

			//CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

			//pSoundMgr->FadeOut_Volume(CSoundMgr::BGM);

			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_CORE_CINEMATIC"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CINEMATIC, &eState));

			TimeAcc += dDeltaTime;
		}
		else
		{
			CUI::UISTATE	eState = CUI::UISTATE::INACTIVATE;

			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_CORE_CINEMATIC"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CINEMATIC, &eState));

			m_iFlagUI ^= FLAG_UI_CALL_CINEMATIC;
		}
	}
	if (m_iFlagUI & FLAG_UI_CALL_ENGAGE)
	{
		CUI_Dialogue_EngageMessage::BOSSID	eID = CUI_Dialogue_EngageMessage::BOSSID::CHARIOT;
				
		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eID));

		m_iFlagUI ^= FLAG_UI_CALL_ENGAGE;
	}
	if (m_eAnimState != CTank::M_ANIMSTATE::IDLE)
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
	if (OBJSTATE::DEAD == m_eState)
	{
		CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

		tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::CHARIOT;
		tObjectiveDesc.iEventCount = 1;

		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));

		/*CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

		pSoundMgr->StopSound(CSoundMgr::BGM2);
		pSoundMgr->PlayLoopSound(L"AmuseParkBGM.mp3", CSoundMgr::BGM);*/
	}

	return S_OK;
}

CTank * CTank::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CTank* pInstance = new CTank(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTank::Clone(void * pArg)
{
	CTank* pInstance = new CTank(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTank::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_RightArm);
	Safe_Release(m_LeftArm);
	Safe_Release(m_MoningStar);
	Safe_Release(m_Turret);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pDissolveTexture);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}
	m_vecNormalAttacks.clear();

	//Safe_Release(m_pLeftArmSpark);
	//Safe_Release(m_pRightArmSpark);
	//Safe_Release(m_pLaser);
	Delete_LeftArmSpark();
	Delete_RightArmSpark();
	Delete_Laser();
}

HRESULT CTank::Update_Effects(_float _fTimeDelta)
{
	Update_Laser();

	// Update Spark

	if (m_pLeftArmSpark)
	{
		_float4 vPos;
		memcpy(&vPos, &m_LArmPos.m[3], sizeof(_float4));
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLeftArmSpark->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&vPos));
		//pTransform->LookAt_Direction(m_pTransform->Get_State(CTransform::STATE::LOOK));
	}
	if (m_pRightArmSpark)
	{
		_float4 vPos;
		memcpy(&vPos, &m_RArmPos.m[3], sizeof(_float4));
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pRightArmSpark->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&vPos));
		//pTransform->LookAt_Direction(m_pTransform->Get_State(CTransform::STATE::LOOK));
	}

	if (M_PATTERN::DESTROYED == m_ePattern)
	{
		if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == 5)
		{
			m_iPassIndex = static_cast<_int>(MESH_PASS_INDEX::ANIM_DISSOLVE);
			m_fDissolveVal += m_fDissolveSpeed * _fTimeDelta;
		}
	}


	return S_OK;
}

HRESULT CTank::Create_Smoke(_float4x4 _vMatrix)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	memcpy(&tPacket.tEffectMovement.vPosition, &_vMatrix.m[3], sizeof(_float4));
	tPacket.tEffectMovement.vPosition.y -= 1.f;
	//tPacket.tEffectMovement.vPosition = _vPosition;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Jump_Smoke.dat", &tPacket);
	return S_OK;
}

HRESULT CTank::Create_LeftArmSpark()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	XMStoreFloat3(&tPacket.tEffectMovement.vDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	memcpy(&tPacket.tEffectMovement.vPosition, &m_LArmPos.m[3], sizeof(_float4));
	tPacket.tEffectMovement.vPosition.y -= 5.f;

	if (m_pLeftArmSpark)
	{
		m_pLeftArmSpark->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLeftArmSpark);
		m_pLeftArmSpark = nullptr;
	}
	m_pLeftArmSpark = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Tank_Spark.dat", &tPacket);
	if (!m_pLeftArmSpark)
		assert(false);

	Safe_AddRef(m_pLeftArmSpark);

	CDecalCube::DECALCUBE tCube;
	tCube.eType = CDecalCube::TYPE::BOOM;
	tCube.iImageIndex = 2;
	tCube.vPosition = tPacket.tEffectMovement.vPosition;
	tCube.vScale = _float4(10.f, 1.5f, 10.f, 0.f);
	tCube.dDuration = 2.5;
	tCube.iIsGlow = 1;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTank::Create_RightArmSpark()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	XMStoreFloat3(&tPacket.tEffectMovement.vDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	memcpy(&tPacket.tEffectMovement.vPosition, &m_RArmPos.m[3], sizeof(_float4));
	tPacket.tEffectMovement.vPosition.y -= 3.f;

	if (m_pRightArmSpark)
	{
		m_pRightArmSpark->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pRightArmSpark);
		m_pRightArmSpark = nullptr;
	}

	m_pRightArmSpark = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Tank_Spark.dat", &tPacket);
	if (!m_pRightArmSpark)
		assert(false);

	Safe_AddRef(m_pRightArmSpark);

	CDecalCube::DECALCUBE tCube;
	tCube.eType = CDecalCube::TYPE::BOOM;
	tCube.iImageIndex = 2;
	tCube.vPosition = tPacket.tEffectMovement.vPosition;
	tCube.vScale = _float4(10.f, 1.5f, 10.f, 0.f);
	tCube.dDuration = 2.5;
	tCube.iIsGlow = 1;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTank::Create_Comfetti()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	memcpy(&tPacket.tEffectMovement.vPosition, m_TurretMatrix.m[3], sizeof(_float4));
	memcpy(&tPacket.tEffectMovement.vDirection, m_TurretMatrix.m[2], sizeof(_float3));

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Tank_Comfetti.dat", &tPacket);
	return S_OK;
}

HRESULT CTank::Create_Laser()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	memcpy(&tPacket.tEffectMovement.vDirection, m_TurretMatrix.m[2], sizeof(_float3));
	memcpy(&tPacket.tEffectMovement.vPosition, m_TurretMatrix.m[3], sizeof(_float4));

	m_pLaser = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Tank_Laser.dat", &tPacket);
	if (!m_pLaser)
		assert(false);
	Safe_AddRef(m_pLaser);
	return S_OK;
}

HRESULT CTank::Update_Laser()
{
	if (!m_pLaser) return E_FAIL;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaser->Get_Component(L"Com_Transform"));
	_vector vPos;
	memcpy(&vPos, &m_TurretMatrix.m[3], sizeof(_vector));
	pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	_vector vLook;
	memcpy(&vLook, &m_TurretMatrix.m[2], sizeof(_vector));
	pTransform->Set_State(CTransform::STATE::LOOK, vLook);


	return S_OK;
}

HRESULT CTank::Delete_LeftArmSpark()
{
	if (m_pLeftArmSpark)
	{
		m_pLeftArmSpark->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLeftArmSpark);
		m_pLeftArmSpark = nullptr;
	}
	return S_OK;
}

HRESULT CTank::Delete_RightArmSpark()
{
	if (m_pRightArmSpark)
	{
		m_pRightArmSpark->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pRightArmSpark);
		m_pRightArmSpark = nullptr;
	}
	return S_OK;
}

HRESULT CTank::Delete_Laser()
{
	if (m_pLaser)
	{
		m_pLaser->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaser);
		m_pLaser = nullptr;
	}
	return S_OK;
}
