#include "stdafx.h"
#include "..\Public\Zhuangzi.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "ZhuangziFireMissile.h"
#include "ZhuangziBridge.h"
#include "ZhuangziDoor.h"

#include "EffectFactory.h"
#include "Light.h"
#include "Player.h"
#include "MeshEffect.h"
#include "CameraMgr.h"

#include "Level_ZhuangziStage.h"
#include "DecalCube.h"
#include "SoundMgr.h"

CZhuangzi::CZhuangzi(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CZhuangzi::CZhuangzi(const CZhuangzi & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CZhuangzi::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CZhuangzi::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));



	m_eCurPattern = PATTERN::APPEARANCE_IDLE;
	m_eCurPhase = PHASE::PHASE_APPEARANCE;

	m_iAnimation = ANIM_ZHUANGZI_APPEARANCE_IDLE;
	m_bContinue = false;

	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	m_fHitRecovery = ZHUANGZI_HIT_RECOVERY;

	m_tObjectStatusDesc.fHp = ZHUANGZI_MAX_LIFE;
	m_tObjectStatusDesc.fMaxHp = ZHUANGZI_MAX_LIFE;
	m_tObjectStatusDesc.iAtt = ZHUANGZI_ATTACK_DAMAGE;

	static _uint iCPod_InstanceID = OriginID_Zhuangzi;
	m_iObjID = iCPod_InstanceID++;
	
	m_bCollision = true;

	Set_FirstLook();

	memset(m_arrTempFrontBladeSpark, 0, sizeof(_bool) * 13);
	memset(m_arrLegRimLightColor, 0, sizeof(_float3) * 6);
	return S_OK;
}

_int CZhuangzi::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)					//	DISABLED
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		if(dynamic_cast<CLevel_ZhuangziStage*>(pGameInstance->Get_CurrentLevel()) != nullptr)
			dynamic_cast<CLevel_ZhuangziStage*>(pGameInstance->Get_CurrentLevel())->Electricity_Supply();
		Set_BridgeClose();

		Create_DeathEffect();
		return (_int)m_eState;
	}

	const _float fTimeDelta = static_cast<_float>(dTimeDelta);

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	//Check_PlayerCell();
	Check_Pattern(dTimeDelta);
	Check_Idle(dTimeDelta);
	LerpLookToTarget(dTimeDelta);
	Create_Missile(dTimeDelta);

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);

	if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
		m_pModel->Synchronize_Animation(m_pTransform);
	else
		m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Update_CollisionTime(dTimeDelta);

	Update_UI(dTimeDelta);
	Update_HitType();
	Update_Collider();

	Update_NormalAttack();
	Update_Effects(fTimeDelta);
	BlackOut(dTimeDelta);

	return (_int)OBJSTATE::ENABLE;
}

_int CZhuangzi::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		m_eState = OBJSTATE::DEAD;
	}

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance->Culling(m_pTransform, m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	}

	return (_int)m_eState;
}

HRESULT CZhuangzi::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	_float3 vRimLightColor = _float3(0.f, 0.f, 0.f);
	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		vRimLightColor = _float3(0.f, 0.f, 0.f);
		vRimLightColor = m_vWholeBodyRimLightColor;
		switch (static_cast<MESH_CONTAINER>(i))
		{
		case CZhuangzi::MESH_CONTAINER::INNER_TAIL:
			vRimLightColor = m_vTailRimLightColor;
			break;
		case CZhuangzi::MESH_CONTAINER::MIDDLE_TAIL:
			vRimLightColor = m_vTailRimLightColor;
			break;
		case CZhuangzi::MESH_CONTAINER::OUTTER_TAIL:
			vRimLightColor = m_vTailRimLightColor;
			break;
		case CZhuangzi::MESH_CONTAINER::RIGHT_LEG_0:
			vRimLightColor = m_arrLegRimLightColor[0];
			break;											 
		case CZhuangzi::MESH_CONTAINER::RIGHT_LEG_1:		 
			vRimLightColor = m_arrLegRimLightColor[1];
			break;
		case CZhuangzi::MESH_CONTAINER::RIGHT_LEG_2:		 
			vRimLightColor = m_arrLegRimLightColor[2];
			break;
		case CZhuangzi::MESH_CONTAINER::LEFT_LEG_0:			 
			vRimLightColor = m_arrLegRimLightColor[3];
			break;
		case CZhuangzi::MESH_CONTAINER::LEFT_LEG_1:			 
			vRimLightColor = m_arrLegRimLightColor[4];
			break;
		case CZhuangzi::MESH_CONTAINER::LEFT_LEG_2:			 
			vRimLightColor = m_arrLegRimLightColor[5];
			break;
		default:											 
			break;											  
		}

		if (i == static_cast<_int>(MESH_CONTAINER::FRONT_BLADE))
			vRimLightColor = m_vFrontBladeRimLightColor;

		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		if (FAILED(m_pModel->SetUp_RawValue("g_vRimLightColor", &vRimLightColor, sizeof(_float3))))
			return E_FAIL;
		m_pModel->Render(i, m_iPassIndex);
	}

	return S_OK;
}

void CZhuangzi::Check_Pattern(_double TimeDelta)
{
	if (m_bIdle)
		return;

	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
	{
		if (PATTERN::APPEARANCE_IDLE == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_APPEARANCE_IDLE != iCurAnimIndex)
			{
				m_iAnimation = ANIM_ZHUANGZI_APPEARANCE_IDLE;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();
			}

			else if (ANIM_ZHUANGZI_APPEARANCE_IDLE == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_APPEARANCE_JUMP;
					m_eCurPattern = PATTERN::APPEARANCE_JUMP;
					m_bContinue = false;

					m_bStart = true;

					m_pModel->Set_NoneBlend();
				}
			}
		}
		else if (PATTERN::APPEARANCE_JUMP == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_APPEARANCE_JUMP == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_IDLE;
					m_bContinue = false;

					Set_OppositeRight();

					m_pModel->Set_NoneBlend();

					m_eCurPattern = PATTERN::UNBEATABLE;
					m_eCurPhase = PHASE::PHASE_NORMAL;

					if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
						return;

					Set_BridgeAnimationStart();
					Create_BodyShield();
					Create_LegShield();
					Create_Pile();
				}
			}
		}
	}
	else if (PHASE::PHASE_NORMAL == m_eCurPhase)
	{
		if (PATTERN::UNBEATABLE == m_eCurPattern)
		{
			if (m_bPassNext)
			{
				m_eCurPattern = PATTERN::SWEEPFLOOR;
				m_bPassNext = false;
			}
			else
			{
				if (ANIM_ZHUANGZI_UNBEATABLE_MOTION != iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_UNBEATABLE_MOTION;
					m_bContinue = false;

					m_pModel->Set_NoneBlend();
				}

				if (iCurAnimIndex == m_iAnimation)
				{
					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_IDLE;
						m_bIdle = true;
						m_IdleDelayTime = 3.0;

						m_pModel->Set_NoneBlend();
						m_bContinue = false;

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;

						// I'll remove it later. I put it in for testing. (Origin: I don't get damaged during unbeatable time.)
						Check_90PercentHP();
						//
					}
				}
			}
		}
		else if (PATTERN::SWEEPFLOOR == m_eCurPattern)
		{
			if (m_bPassNext)
			{
				m_iAnimation = ANIM_ZHUANGZI_ROTATE_SWEEPFLOOR;
				m_pModel->Set_NoneBlend();
				m_bContinue = false;

				m_bPassNext = false;

				m_vTailRimLightColor = _float3(1.f, 0.f, 1.f);
			}
			else
			{
				if (m_bPowerFailure && !m_bIdle)
				{
					if (ANIM_ZHUANGZI_IDLE == iCurAnimIndex)
					{
						if (m_LookLerpTime >= 1.0)
						{
							m_bLookLerp = false;
							m_LookLerpTime = 0.0;
						}
						else
							return;
					}
				}

				if (ANIM_ZHUANGZI_CHASE_START != iCurAnimIndex && ANIM_ZHUANGZI_CHASE_DW != iCurAnimIndex && ANIM_ZHUANGZI_CHASE_END != iCurAnimIndex && ANIM_ZHUANGZI_ROTATE_SWEEPFLOOR != iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_START;
					m_bContinue = false;

					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
					CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Dash.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);

					m_pModel->Set_NoneBlend();
				}
				else if (ANIM_ZHUANGZI_CHASE_START == iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_START;

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_CHASE_DW;
						m_bContinue = false;

						m_pModel->Set_NoneBlend();
					}
				}
				else if (ANIM_ZHUANGZI_CHASE_DW == iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_DW;

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_CHASE_END;
						m_bContinue = false;

						m_pModel->Set_NoneBlend();
					}
				}
				else if (ANIM_ZHUANGZI_CHASE_END == iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_END;

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_IDLE;
						m_bIdle = true;
						m_IdleDelayTime = 1.0;

						m_pModel->Set_NoneBlend();
						m_bContinue = true;

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;
						m_bCreateOnce = false;
					}
				}
				else if (ANIM_ZHUANGZI_ROTATE_SWEEPFLOOR == iCurAnimIndex)
				{
					m_iAnimation = ANIM_ZHUANGZI_ROTATE_SWEEPFLOOR;

					_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
					if (PlayTimeRatio >= 0.304 && PlayTimeRatio <= 0.3900)
					{
						m_dSweepSparkElaspedTime += TimeDelta;
						if (m_dSweepSparkElaspedTime > 0.035)
						{
							m_dSweepSparkElaspedTime = 0.0;
							_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Tail") * m_pTransform->Get_WorldMatrix();
							_float4x4 BoneFloat4x4;

							_vector vPos = XMVectorZero();

							XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

							_vector vUp = XMVectorZero();
							_vector vRight = XMVectorZero();

							memcpy(&vRight, &BoneFloat4x4.m[0], sizeof(_vector));
							memcpy(&vPos, &BoneFloat4x4.m[3], sizeof(_vector));

							vUp = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
							vUp *= -1.f;

							vPos += vUp * (6.f);

							Create_TailSweep_Spark(vPos);
						}
					}

					if (!m_bCreateOnce)
					{
						if (m_pModel->Get_CollisionMinRatio() <= m_pModel->Get_PlayTimeRatio() &&
							m_pModel->Get_CollisionMaxRatio() >= m_pModel->Get_PlayTimeRatio())
						{
							m_bAtkDead = false;

							CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

							for (_uint i = 0; i < 3; ++i)
							{
								Humanoid_Atk_Info tInfo;

								tInfo.iNumber = 10;
								XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(i));

								CNormal_Attack* pAttack = nullptr;
								if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
									, (CGameObject**)&pAttack, &tInfo)))
								{
									RELEASE_INSTANCE(CGameInstance);
									return;
								}

								m_vecNormalAttacks.push_back(pAttack);

								Safe_AddRef(pAttack);
							}

							RELEASE_INSTANCE(CGameInstance);

							m_bCreateOnce = true;

							CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
							CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Swish.wav", CSoundMgr::CHANNELID::BOSS_EFFECT);
						}
					}
					else if (m_pModel->Get_CollisionMaxRatio() < m_pModel->Get_PlayTimeRatio())
					{
						if (!m_bAtkDead)
						{
							m_bCreateOnce = false;
							m_bAtkDead = true;
						}
					}

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_SHOOTTRAIL;
						m_eCurPattern = PATTERN::SHOOTTRAIL;

						m_pModel->Set_NoneBlend();
						m_bContinue = false;

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;

						m_bCreateOnce = false;

						Set_DiagonalRight();

						m_vFrontBladeRimLightColor = _float3(1.f, 1.f, 0.f);
						m_vTailRimLightColor = _float3(0.f, 0.f, 0.f);
					}
				}
			}
		}
		else if (PATTERN::SHOOTTRAIL == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_SHOOTTRAIL == iCurAnimIndex)
			{
				if (m_LookLerpTime >= 1.0)
				{
					m_bLookLerp = false;
					m_LookLerpTime = 0.0;
				}

				if (m_pModel->Get_CollisionMinRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (!m_bCreateOnce)
					{
						if (Create_WeaponTrail())
							return;

						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
						CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Slash.mp3", CSoundMgr::BOSS_EFFECT);

						m_bCreateOnce = true;
						m_vFrontBladeRimLightColor = _float3(0.f, 0.f, 0.f);
					}
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_IDLE;
					m_eCurPattern = PATTERN::BLOW_JUMPBLOW;
 
					m_bLookLerp = true;
					m_LookLerpTime = 0.0;

					m_bIdle = true;
					m_IdleDelayTime = 1.0;

					m_bContinue = true;
					m_bCreateOnce = false;

					Check_90PercentHP();
					Check_15PercentHP();
				}
			}
		}
		else if (PATTERN::BLOW_JUMPBLOW == m_eCurPattern)
		{
			if (m_bPassNext)
			{
				m_iAnimation = ANIM_ZHUANGZI_CHARGE_BLOW;
				m_pModel->Set_NoneBlend();

				m_bContinue = false;
				m_bPassNext = false;
				m_bTempRightLeg0 = true;
				m_bTempRightLeg1 = true;
				m_bTempRightLeg2 = true;
				m_bTempLeftLeg0 = true;
				m_bTempLeftLeg1 = true;
				m_bTempLeftLeg2 = true;

				// 6 is Legs num
				for (_int i = 0; i < 6; ++i)
				{
					m_arrLegRimLightColor[i] = _float3(1.f, 1.f, 0.f);
				}
			}
			else
			{
				if (ANIM_ZHUANGZI_CHARGE_BLOW == iCurAnimIndex)
				{
					_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
					_float fLegOffsetY = -2.5f;
					if (0.2695 <= PlayTimeRatio)	//
					{
						if (m_bTempRightLeg0)
						{
							m_bTempRightLeg0 = false;

							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_0), "bone1570");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[0] = _float3(0.f, 0.f, 0.f);
						}
					}
					if (0.3335 <= PlayTimeRatio) // 
					{
						if (m_bTempLeftLeg0)
						{
							m_bTempLeftLeg0 = false;

							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_0), "bone1826");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[3] = _float3(0.f, 0.f, 0.f);

						}

					}

					if (0.3879 <= PlayTimeRatio) // 
					{
						if (m_bTempRightLeg1)
						{
							m_bTempRightLeg1 = false;
							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_1), "bone1586");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[1] = _float3(0.f, 0.f, 0.f);

						}

					}
					if (0.4972 <= PlayTimeRatio) //
					{
						if (m_bTempLeftLeg1)
						{
							m_bTempLeftLeg1 = false;
							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_1), "bone1842");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[4] = _float3(0.f, 0.f, 0.f);

						}

					}
					if (0.5615 <= PlayTimeRatio) //
					{
						if (m_bTempRightLeg2)
						{
							m_bTempRightLeg2 = false;
							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_2), "bone1602");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[2] = _float3(0.f, 0.f, 0.f);

						}

					}
					if (0.6277 <= PlayTimeRatio) //
					{
						if (m_bTempLeftLeg2)
						{
							m_bTempLeftLeg2 = false;
							_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_2), "bone1858");
							_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
							vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);

							Create_Ring(vPos);
							Create_Ring_Spark(vPos);
							m_arrLegRimLightColor[5] = _float3(0.f, 0.f, 0.f);

						}

					}

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_JUMP_BLOW;
						m_pModel->Set_NoneBlend();

						m_bContinue = false;

						Check_90PercentHP();
						Check_15PercentHP();
					}
				}
				else if ((ANIM_ZHUANGZI_JUMP_BLOW == iCurAnimIndex) && (1 >= m_iRepeatCount))
				{
					if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
					{
						if (!m_bCheckIf)
						{
							Check_JumpSpike(true);
							m_bCheckIf = true;
  							Create_ImpactEffects();
						}
						else
							Check_JumpSpike(false);
					}
					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_CHASE_START;
						m_pModel->Set_NoneBlend();

						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
						CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Dash.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;

						m_bContinue = false;
						m_bCheckIf = false;

						Check_90PercentHP();
						Check_15PercentHP();
					}
				}
				else if (ANIM_ZHUANGZI_CHASE_START == iCurAnimIndex)
				{
					if (m_LookLerpTime >= 1.0)
					{
						m_bLookLerp = false;
						m_LookLerpTime = 0.0;
					}

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_CHASE_DW;
						m_pModel->Set_NoneBlend();

						m_bContinue = false;

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;
					}
				}
				else if (ANIM_ZHUANGZI_CHASE_DW == iCurAnimIndex)
				{
					if (m_LookLerpTime >= 1.0)
					{
						m_bLookLerp = false;
						m_LookLerpTime = 0.0;
					}

					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_ZHUANGZI_CHASE_END;
						m_pModel->Set_NoneBlend();

						m_bContinue = false;
					}
				}
				else if (ANIM_ZHUANGZI_CHASE_END == iCurAnimIndex)
				{
					if (m_bLookLerp)
					{
						m_bLookLerp = false;
						m_LookLerpTime = 0.0;
					}

					if (m_pModel->IsFinish_Animation())
					{
						m_iRepeatCount += 1;

						m_iAnimation = ANIM_ZHUANGZI_JUMP_BLOW;
						m_pModel->Set_NoneBlend();

						m_bContinue = false;

						Check_90PercentHP();
						Check_15PercentHP();
					}
				}
				else if ((ANIM_ZHUANGZI_JUMP_BLOW == iCurAnimIndex) && 2 == m_iRepeatCount)
				{
					if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
					{
						if (!m_bCheckIf)
						{
							Check_JumpSpike(true);
							m_bCheckIf = true;
						}
						else
							Check_JumpSpike(false);
					}

					if (m_pModel->IsFinish_Animation())
					{
						//temp
						m_eCurPattern = PATTERN::BEATING;
						m_iAnimation = ANIM_ZHUANGZI_IDLE;
						m_pModel->Set_NoneBlend();

						m_bContinue = false;
						m_iRepeatCount = 0;

						m_bLookLerp = true;
						m_LookLerpTime = 0.0;

						m_bCheckIf = false;
						m_bCreateOnce = false;
						m_bAtkDead = false;

						Check_90PercentHP();
						Check_15PercentHP();
					}
				}
			}
		}
		else if (PATTERN::BEATING == m_eCurPattern)
		{
			if (m_LookLerpTime >= 1.0)
			{
				if (0 >= m_iRepeatCount)
				{
					m_bLookLerp = false;
					m_LookLerpTime = 0.0;
				}
			}

			// Start
			if (ANIM_ZHUANGZI_BEATING_DW != iCurAnimIndex && ANIM_ZHUANGZI_BEATING_END != iCurAnimIndex && (0 == m_iRepeatCount))
			{
				m_iAnimation = ANIM_ZHUANGZI_BEATING_START;
				m_bContinue = false;

				m_pModel->Set_NoneBlend();

			}

			if (ANIM_ZHUANGZI_BEATING_START == iCurAnimIndex)
			{
				memset(m_arrTempFrontBladeSpark, 1, sizeof(_bool) * 13);
				m_vFrontBladeRimLightColor = _float3(1.f, 0.f, 0.5f);

				if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (!m_bCreateOnce)
					{
						CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
						
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 9;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

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


				if (0.884 <= m_pModel->Get_PlayTimeRatio())
				{
					if (m_arrTempFrontBladeSpark[0])
					{
						m_arrTempFrontBladeSpark[0] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 10.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (0.959 <= m_pModel->Get_PlayTimeRatio())
				{
					if (m_arrTempFrontBladeSpark[1])
					{
						m_arrTempFrontBladeSpark[1] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}


				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_BEATING_DW;
					m_bContinue = false;

					m_pModel->Set_NoneBlend();

					CSoundMgr::Get_Instance()->StopPlaySound(L"Zhuangzi_Chopping.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
				}



			}
			else if (ANIM_ZHUANGZI_BEATING_DW == iCurAnimIndex)
			{
				if (m_bLookLerp)
				{
					m_bLookLerp = false;
					m_LookLerpTime = 0.0;
				}




				// End
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_BEATING_END;
					m_bContinue = false;

					m_pModel->Set_NoneBlend();
					m_vFrontBladeRimLightColor = _float3(0.f, 0.f, 0.f);

				}

				if (m_pModel->Get_PlayTimeRatio() >= 0.02) {
					if (m_arrTempFrontBladeSpark[2])
					{
						m_arrTempFrontBladeSpark[2] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.1167) {
					if (m_arrTempFrontBladeSpark[3])
					{
						m_arrTempFrontBladeSpark[3] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.217) {
					if (m_arrTempFrontBladeSpark[4])
					{
						m_arrTempFrontBladeSpark[4] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.305) {
					if (m_arrTempFrontBladeSpark[5])
					{
						m_arrTempFrontBladeSpark[5] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.4138) {
					if (m_arrTempFrontBladeSpark[6])
					{
						m_arrTempFrontBladeSpark[6] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.527) {
					if (m_arrTempFrontBladeSpark[7])
					{
						m_arrTempFrontBladeSpark[7] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.619) {
					if (m_arrTempFrontBladeSpark[8])
					{
						m_arrTempFrontBladeSpark[8] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.722) {
					if (m_arrTempFrontBladeSpark[9])
					{
						m_arrTempFrontBladeSpark[9] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.822) {
					if (m_arrTempFrontBladeSpark[10])
					{
						m_arrTempFrontBladeSpark[10] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.908) {
					if (m_arrTempFrontBladeSpark[11])
					{
						m_arrTempFrontBladeSpark[11] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.978) {
					if (m_arrTempFrontBladeSpark[12])
					{
						m_arrTempFrontBladeSpark[12] = false;
						_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::FRONT_BLADE), "bone000");
						//Create_FrontBlade_Spark(static_cast<_vector>(matBoneMatrix.r[3]));

						_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());

						// Offset
						vPos += m_pTransform->Get_State(CTransform::STATE::LOOK) * 6.f;
						Create_FrontBlade_Spark(vPos);
					}
				}



			}
			else if (ANIM_ZHUANGZI_BEATING_END == iCurAnimIndex)
			{
				m_bAtkDead = true;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_IDLE;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_bLookLerp = true;
					m_LookLerpTime = 0.0;

					++m_iRepeatCount;
					m_bCreateOnce = false;
					m_bAtkDead = false;

					Check_90PercentHP();
					Check_15PercentHP();
				}
			}
			else if (ANIM_ZHUANGZI_IDLE == iCurAnimIndex)
			{
				if (3 == m_iRepeatCount)
				{
					m_bContinue = false;
					m_eCurPattern = PATTERN::POWERFAILURE;

					m_iRepeatCount = 0;
				}
				else
					m_iAnimation = ANIM_ZHUANGZI_BEATING_START;
			}
		}
		else if (PATTERN::POWERFAILURE == m_eCurPattern)
		{
			if (!m_bPowerFailure)
			{
				m_bPowerFailure = true;
			}
			
			if (m_LookLerpTime >= 1.0 && ANIM_ZHUANGZI_IDLE == iCurAnimIndex)
			{
				m_bLookLerp = false;
				m_LookLerpTime = 0.0;

				m_iAnimation = ANIM_ZHUANGZI_ROTATE_BACKWARD;
				m_eCurPattern = PATTERN::STABTAIL;
				m_pModel->Set_NoneBlend();
				m_bContinue = false;
			}
		}
		else if (PATTERN::STABTAIL == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_ROTATE_BACKWARD == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					Set_Back();

					m_iAnimation = ANIM_ZHUANGZI_STABTAIL;
					m_pModel->Set_NoneBlend();

					m_bContinue = true;

					m_bLookLerp = true;
					m_LookLerpTime = 0.0;
					m_vTailRimLightColor = _float3(0.7f, 0.f, 1.f);
				}
			}
			else if (ANIM_ZHUANGZI_STABTAIL == iCurAnimIndex)
			{
				if (ZHUANGZI_STABTAIL_CREATEBOX_RATIO - 0.1 <= m_pModel->Get_PlayTimeRatio()
					&& ZHUANGZI_STABTAIL_CREATEBOX_RATIO > m_pModel->Get_PlayTimeRatio())
				{
					if (false == m_bSoundTiming)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
						CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Stab.mp3", CSoundMgr::BOSS_EFFECT);

						m_bSoundTiming = true;
					}
					
				}
				if (ZHUANGZI_STABTAIL_CREATEBOX_RATIO <= m_pModel->Get_PlayTimeRatio())
				{
					if (!m_bCreateOnce)
					{
						CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 11;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

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
				else if (ZHUANGZI_STABTAIL_CREATEBOX_RATIO > m_pModel->Get_PlayTimeRatio())
				{
					m_bAtkDead = false;
					m_bCreateOnce = false;
				}

				if (m_pModel->Get_LinkMinRatio() >= m_pModel->Get_PlayTimeRatio())
				{
					m_bCheckIf = true;

					if (5 > m_iRepeatCount)
					{
						m_bSoundTiming = false;
						m_bContinue = true;
					}
					else
					{
						m_bSoundTiming = false;
						m_bContinue = false;
					}

					if (false == m_bLookLerp)
					{
						m_bLookLerp = true;
						m_LookLerpTime = 0.0;
					}
				}
				else
				{
					if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
						m_bAtkDead = true;

					if (m_bCheckIf)
					{
						++m_iRepeatCount;
						m_bCheckIf = false;

						m_bLookLerp = false;
						m_LookLerpTime = 0.0;
					}
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_ROTATE_FORWARD;
					m_eCurPattern = PATTERN::SPIKETAIL;

					m_pModel->Set_NoneBlend();
					m_LookLerpTime = 0.0;

					m_iRepeatCount = 0;
					m_bCreateOnce = false;

					Check_90PercentHP();
					Check_15PercentHP();

					m_vTailRimLightColor = _float3(0.f, 0.f, 0.f);
					m_bSoundTiming = false;
				}
			}
		}
		else if (PATTERN::SPIKETAIL == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_ROTATE_FORWARD == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_START;
					m_pModel->Set_NoneBlend();

					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
					CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Dash.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					
					Set_Back();

					m_LookLerpTime = 0.0;
					m_bLookLerp = true;
				}
			}
			else if (ANIM_ZHUANGZI_CHASE_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ZHUANGZI_CHASE_DW == iCurAnimIndex)
			{
				if (m_LookLerpTime >= 1.0)
				{
					m_LookLerpTime = 0.0;
					m_bLookLerp = false;
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHASE_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ZHUANGZI_CHASE_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_IDLE;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_LookLerpTime = 0.0;
					m_bLookLerp = true;

					Check_90PercentHP();
					Check_15PercentHP();
				}
			}
			else if (ANIM_ZHUANGZI_IDLE == iCurAnimIndex)
			{
				if (m_LookLerpTime >= 1.0)
				{
					m_LookLerpTime = 0.0;
					m_bLookLerp = false;

					m_iAnimation = ANIM_ZHUANGZI_SPIKE_TAIL;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_vTailRimLightColor = _float3(0.f, 0.f, 0.5f);

					Check_90PercentHP();
					Check_15PercentHP();

					m_bSoundTiming = false;
				}
			}
			else if (ANIM_ZHUANGZI_SPIKE_TAIL == iCurAnimIndex)
			{
				if (m_pModel->Get_CollisionMinRatio() - 0.05 <= m_pModel->Get_PlayTimeRatio()
					&& m_pModel->Get_CollisionMinRatio() > m_pModel->Get_PlayTimeRatio())
				{
					if (false == m_bSoundTiming)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
						CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Crash.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);

						m_bSoundTiming = true;
					}
				}

				if (m_pModel->Get_CollisionMinRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (!m_bCreateOnce)
					{
						m_bAtkDead = false;

						CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

						for (_uint i = 0; i < 4; ++i)
						{
							Humanoid_Atk_Info tInfo;

							tInfo.iNumber = 12;
							XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(i));

							CNormal_Attack* pAttack = nullptr;
							if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
								, (CGameObject**)&pAttack, &tInfo)))
							{
								RELEASE_INSTANCE(CGameInstance);
								return;
							}

							m_vecNormalAttacks.push_back(pAttack);

							Safe_AddRef(pAttack);
						}

						RELEASE_INSTANCE(CGameInstance);
						
						m_bCreateOnce = true;
						m_bSoundTiming = false;
					}
					else
					{
						if (m_bCreateOnce)
							m_bAtkDead = true;
					}
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = PATTERN::SHOOTMISSILE;

					m_iAnimation = ANIM_ZHUANGZI_MISSILE_START;
					m_pModel->Set_NoneBlend();

					m_LookLerpTime = 0.0;
					m_bLookLerp = true;

					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;

					Check_90PercentHP();
					Check_15PercentHP();
					m_vTailRimLightColor = _float3(0.f, 0.f, 0.f);
				}
			}
		}
		else if (PATTERN::SHOOTMISSILE == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_MISSILE_START == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_bLookLerp = false;
					m_LookLerpTime = 0.0;
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_MISSILE_DW;

					m_pModel->Set_NoneBlend();
					m_bContinue = false;

					m_iRepeatCount = 0;

					m_CreateDelayTime = 0.6;
				}
			}
			else if (ANIM_ZHUANGZI_MISSILE_DW == iCurAnimIndex)
			{
				if (0.5 >= m_pModel->Get_PlayTimeRatio())
				{
					if (3 < m_iRepeatCount)
						m_bContinue = false;
					else
						m_bContinue = true;

					if (false == m_bCheckIf)
						++m_iRepeatCount;

					m_bCheckIf = true;
				}
				else
					m_bCheckIf = false;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_MISSILE_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_bCheckIf = false;
					m_iRepeatCount = 0;
				}
			}
			else if (ANIM_ZHUANGZI_MISSILE_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_IDLE;
					m_pModel->Set_NoneBlend();

					m_eCurPattern = PATTERN::SWEEPFLOOR;

					m_bContinue = false;

					m_bLookLerp = true;
					m_LookLerpTime = 0.0;

					Check_90PercentHP();
					Check_15PercentHP();
				}
			}
		}
		else if (PATTERN::GROGGY == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_GROGGY_START != iCurAnimIndex && ANIM_ZHUANGZI_GROGGY_DW != iCurAnimIndex && ANIM_ZHUANGZI_GROGGY_END != iCurAnimIndex)
			{
				m_iAnimation = ANIM_ZHUANGZI_GROGGY_START;
				m_pModel->Set_NoneBlend();

				m_bIdle = false;
				m_IdleDelayTime = 0.0;

				m_bLookLerp = false;
				m_LookLerpTime = 0.0;

				m_bContinue = false;
			}
			else if (ANIM_ZHUANGZI_GROGGY_START == iCurAnimIndex)
			{
				m_bContinue = false;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_GROGGY_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ZHUANGZI_GROGGY_DW == iCurAnimIndex)
			{
				m_GroggyTime += TimeDelta;

				if (m_GroggyTime >= m_GroggyDelayTime)
				{
					m_iAnimation = ANIM_ZHUANGZI_GROGGY_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_GroggyTime = 0.0;
				}

			}
			else if (ANIM_ZHUANGZI_GROGGY_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPhase = PHASE::PHASE_SPHERE;
					m_eCurPattern = PATTERN::CHANGE_SPHERE;

					m_iAnimation = ANIM_ZHUANGZI_CHANGE_SPHERE;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
		}
		else if (PATTERN::FIRSTGROGGY == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_GROGGY_START != iCurAnimIndex && ANIM_ZHUANGZI_GROGGY_DW != iCurAnimIndex && ANIM_ZHUANGZI_GROGGY_END != iCurAnimIndex)
			{
				m_iAnimation = ANIM_ZHUANGZI_GROGGY_START;
				m_pModel->Set_NoneBlend();

				m_bIdle = false;
				m_IdleDelayTime = 0.0;

				m_bLookLerp = false;
				m_LookLerpTime = 0.0;

				m_bContinue = false;
			}
			else if (ANIM_ZHUANGZI_GROGGY_START == iCurAnimIndex)
			{
				m_bContinue = false;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_GROGGY_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ZHUANGZI_GROGGY_DW == iCurAnimIndex)
			{
				m_GroggyTime += TimeDelta;

				if (m_GroggyTime >= m_FirstGroggyDelayTime)
				{
					m_iAnimation = ANIM_ZHUANGZI_GROGGY_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_GroggyTime = 0.0;
				}

			}
			else if (ANIM_ZHUANGZI_GROGGY_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = m_eMemoryPattern;

					m_iAnimation = m_iMemoryAnimation;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					if (PATTERN::BLOW_JUMPBLOW == m_eCurPattern)
					{
						m_bIdle = true;
						m_IdleDelayTime = 1.0;
					}
					else if (PATTERN::BEATING == m_eCurPattern)
					{
						m_bLookLerp = true;
						m_LookLerpTime = 0.0;
					}
					else if (PATTERN::SHOOTMISSILE == m_eCurPattern)
					{
						m_LookLerpTime = 0.0;
						m_bLookLerp = true;
					}
				}
			}
		}
	}
	else if (PHASE::PHASE_SPHERE == m_eCurPhase)
	{
		if (PATTERN::CHANGE_SPHERE == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_CHANGE_SPHERE == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHANGE_JUMPCHASE;
					m_eCurPattern = PATTERN::CHANGE_JUMPANDCHASE;

					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					if (FAILED(Change_ColliderBox()))
					{
						MSGBOX("Failed - CZhuangzi::Change_ColliderBox()");
						return;
					}
				}
			}
		}
		else if (PATTERN::CHANGE_JUMPANDCHASE == m_eCurPattern)
		{
			if (ANIM_ZHUANGZI_CHANGE_JUMPCHASE == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (false == m_bLookLerp)
					{
						m_LookLerpTime = 0.0;
						m_bLookLerp = true;
					}
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHANGE_CHASE_TARGET;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ZHUANGZI_CHANGE_CHASE_TARGET == iCurAnimIndex)
			{
				if (0.5 >= m_pModel->Get_PlayTimeRatio())
				{
					m_bCheckIf = true;

					if (0 == m_iRepeatChaseCount)
						m_bContinue = true;
					else
						m_bContinue = false;
				}
				else
				{
					if (m_bCheckIf)
					{
						++m_iRepeatChaseCount;
						m_bCheckIf = false;
					}
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ZHUANGZI_CHANGE_WEEKJUMP_CHASE_STOP;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_LookLerpTime = 0.0;
					m_bLookLerp = false;
					m_iRepeatChaseCount = 0;

					m_bLerpStop = false;
				}
			}
			else if (ANIM_ZHUANGZI_CHANGE_WEEKJUMP_CHASE_STOP == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					if (3 >= m_iRepeatCount)
					{
						m_iAnimation = ANIM_ZHUANGZI_CHANGE_JUMPCHASE;
						m_eCurPattern = PATTERN::CHANGE_JUMPANDCHASE;

						m_pModel->Set_NoneBlend();

						m_bContinue = false;
						++m_iRepeatCount;
					}
					else
					{
						m_iAnimation = ANIM_ZHUANGZI_CHANGE_DIZZY;
						m_pModel->Set_NoneBlend();

						m_eCurPattern = PATTERN::CHANGE_DIZZY;

						m_bLookLerp = false;
						m_LookLerpTime = 0.0;

						m_iRepeatCount = 0;
					}
				}
			}
		}
		else if (PATTERN::CHANGE_DIZZY == m_eCurPattern)
		{
			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_ZHUANGZI_CHANGE_JUMPSPIKE;
				m_eCurPattern = PATTERN::CHANGE_JUMPSPIKE;

				m_pModel->Set_NoneBlend();

				m_bContinue = false;
				m_bCreateOnce = false;
			}
		}
		else if (PATTERN::CHANGE_JUMPSPIKE == m_eCurPattern)
		{
			if (m_pModel->Get_CollisionMinRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				if (!m_bCreateOnce)
				{
					Check_JumpSpike(true);
					m_bCreateOnce = true;
				}
				else
					Check_JumpSpike(false);
			}

			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_ZHUANGZI_CHANGE_JUMPCHASE;
				m_eCurPattern = PATTERN::CHANGE_JUMPANDCHASE;

				m_pModel->Set_NoneBlend();

				m_bContinue = false;
				m_bCreateOnce = false;
			}
		}
	}
}

void CZhuangzi::Check_Idle(_double TimeDelta)
{
	if (m_bIdle)
	{
		m_IdleTime += TimeDelta;

		if ((m_IdleTime >= m_IdleDelayTime))
		{
			m_IdleTime = 0.0;
			m_IdleDelayTime = 0.0;

			m_bIdle = false;
			m_bPassNext = true;
			
			m_bLookLerp = false;
			m_LookLerpTime = 0.0;
		}
	}
}

void CZhuangzi::LerpLookToTarget(_double TimeDelta)
{
	if (m_bLookLerp)
	{
		if (PHASE::PHASE_SPHERE == m_eCurPhase)
		{
			m_LookLerpTime += (TimeDelta * 0.1);

			if (PATTERN::CHANGE_JUMPANDCHASE == m_eCurPattern)
			{
				if (m_bLerpStop)
					m_LookLerpTime += (TimeDelta * 0.2);

				if (m_LookLerpTime >= 1.0)
					m_LookLerpTime = 0.5;
			}
			else
			{
				if (m_LookLerpTime >= 1.0)
					m_LookLerpTime = 1.0;
			}
		}
		else
		{
			if (PATTERN::SHOOTTRAIL == m_eCurPattern)
				m_LookLerpTime += (TimeDelta * 0.1);
			else if (ANIM_ZHUANGZI_STABTAIL == m_pModel->Get_CurrentAnimation())
				m_LookLerpTime += TimeDelta * 0.85;
			else
				m_LookLerpTime += (TimeDelta * 0.25);

			if (m_LookLerpTime >= 1.0)
				m_LookLerpTime = 1.0;
		}

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
		if (nullptr == pPlayerTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_vector vScale = m_pTransform->Get_Scale();
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		
		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

		vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPos));

		if (ANIM_ZHUANGZI_CHANGE_CHASE_TARGET == m_pModel->Get_CurrentAnimation())
		{
			m_vWholeBodyRimLightColor = _float3(1.f, 0.3f, 0.f);
			m_dRollinElapsedTime += TimeDelta;

			if (m_dRollinElapsedTime > 0.8)
			{
				m_dRollinElapsedTime = 0.0;
				Create_Rollin_Spark(m_pTransform->Get_State(CTransform::STATE::POSITION));
			}

			_vector vDist = XMVector3Length(vPlayerPos - vPos);

			if (XMVectorGetX(vDist) <= 4.f)
			{
				if (false == m_bLerpStop)
				{
					m_LookLerpTime = 0.0;
					m_bLookLerp = false;

					RELEASE_INSTANCE(CGameInstance);
					return;
				}

				m_bLerpStop = true;
			}
		}
		else
		{
			m_vWholeBodyRimLightColor = _float3(0.f, 0.f, 0.f);
		}

		_vector vDir = XMVectorZero();

		if (ANIM_ZHUANGZI_STABTAIL == m_pModel->Get_CurrentAnimation())
			vDir = XMVector3Normalize(vPos - vPlayerPos);
		else
			vDir = XMVector3Normalize(vPlayerPos - vPos);

		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_LookLerpTime)));

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);

		RELEASE_INSTANCE(CGameInstance);
	}
}

void CZhuangzi::Check_JumpSpike(const _bool& bCheck)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CGameObject* pGameObject = pGameInstance->Get_GameObjectPtr(pGameInstance->Get_LevelID(), L"WholeRange", 0);
	if (nullptr == pGameObject)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	pGameObject->Set_Collision(bCheck);
	RELEASE_INSTANCE(CGameInstance);
}

void CZhuangzi::Set_FirstLook()
{
	//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM, 0);
	//if (nullptr == pTransform)
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return;
	//}

	//_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);
	//_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	//_vector vDir = XMVector3Normalize(vPos - vPlayerPos);
	//_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	//_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

	//_vector vScale = m_pTransform->Get_Scale();

	//vRight *= XMVectorGetX(vScale);
	//vDir *= XMVectorGetZ(vScale);

	//m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	//m_pTransform->Set_State(CTransform::STATE::LOOK, vDir);

	//RELEASE_INSTANCE(CGameInstance);

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	vRight *= -1.f;

	vRight *= XMVectorGetX(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
}

void CZhuangzi::Set_BridgeAnimationStart()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CZhuangziBridge* pBridge = (CZhuangziBridge*)pGameInstance->Get_GameObjectPtr(pGameInstance->Get_LevelID(), L"ZhuangziBridge", EXITBRIDGE_LAYERINDEX);
	if (nullptr == pBridge)
		return;

	pBridge->Set_CellOn(true);
}

void CZhuangzi::Set_BridgeClose()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CZhuangziBridge* pBridge = (CZhuangziBridge*)pGameInstance->Get_GameObjectPtr(pGameInstance->Get_LevelID(), L"ZhuangziBridge", EXITBRIDGE_LAYERINDEX);
	if (nullptr == pBridge)
		return;

	pBridge->Initialize_Animation();
	pBridge->Set_SceneClear(true);

	CZhuangziDoor* pDoor = (CZhuangziDoor*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"ZhuangziDoor", 1);

	CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

	tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
	tDesc.eObjState = OBJSTATE::ENABLE;
	tDesc.iObjID = pDoor->Get_ObjID();

	XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pDoor->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

	CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

	tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::ZHUANGZI;
	tObjectiveDesc.iEventCount = 1;

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));
}

void CZhuangzi::Check_PlayerCell()
{
	if (false == m_bStart)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CNavigation* pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_NAVIGATION, 0);
		if (nullptr == pNavigation)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (ZHUANGZI_SCENE_ONCELL == pNavigation->Get_Index())
			m_bStart = true;

		RELEASE_INSTANCE(CGameInstance);
	}
}

HRESULT CZhuangzi::Change_ColliderBox()
{
	Safe_Release(m_pHitBox);

	auto& iterHitBox = m_mapComponents.find(COM_KEY_HITBOX);
	Safe_Release(iterHitBox->second);
	m_mapComponents.erase(iterHitBox);

	Safe_Release(m_pAttackBox);

	auto& iterAttBox = m_mapComponents.find(COM_KEY_ATTACKBOX);
	Safe_Release(iterAttBox->second);
	m_mapComponents.erase(iterAttBox);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider::tagColliderDesc tColliderDesc;
	tColliderDesc.vPivot = _float3(0.f, 3.5f, 0.f);
	tColliderDesc.vScale = _float3(3.f, 3.5f, 3.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = _float3(0.f, 3.5f, 0.f);
	tColliderDesc.fRadius = 4.f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CZhuangzi::Update_NormalAttack()
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
			_uint iCount = 0;

			for (auto& pAttack : m_vecNormalAttacks)
			{
				CTransform*	pAttackTransform = dynamic_cast<CTransform*>(pAttack->Get_Component(COM_KEY_TRANSFORM));

				if (nullptr != pAttackTransform)
					pAttackTransform->Set_WorldMatrix(Get_MatrixForNormalAttack(iCount));

				++iCount;
			}
		}
	}

	return;
}

_matrix CZhuangzi::Get_MatrixForNormalAttack(_uint iCount)
{
	if (PATTERN::BEATING == m_eCurPattern)
	{
		_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
		_float4x4 WorldFloat4x4;

		_vector vLook = XMVectorZero();
		_vector vPos = XMVectorZero();

		XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

		memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
		memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

		vPos += vLook * 3;
		vPos += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 3;

		memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

		return XMLoadFloat4x4(&WorldFloat4x4);
	}
	else if (PATTERN::SWEEPFLOOR == m_eCurPattern)
	{
		_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Tail") * m_pTransform->Get_WorldMatrix();
		_float4x4 BoneFloat4x4;

		_vector vPos = XMVectorZero();

		XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

		_vector vUp = XMVectorZero();
		_vector vRight = XMVectorZero();

		memcpy(&vRight, &BoneFloat4x4.m[0], sizeof(_vector));
		memcpy(&vPos, &BoneFloat4x4.m[3], sizeof(_vector));

		vUp = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
		vUp *= -1.f;

		vPos += vUp * (6.f * iCount);

		memcpy(&BoneFloat4x4.m[3], &vPos, sizeof(_vector));

		return XMLoadFloat4x4(&BoneFloat4x4);
	}
	else if (PATTERN::STABTAIL == m_eCurPattern)
	{
		_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"EndofTail") * m_pTransform->Get_WorldMatrix();
		_float4x4 BoneFloat4x4;

		_vector vPos = XMVectorZero();

		XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

		_vector vUp = XMVectorZero();
		_vector vRight = XMVectorZero();

		memcpy(&vRight, &BoneFloat4x4.m[0], sizeof(_vector));
		memcpy(&vPos, &BoneFloat4x4.m[3], sizeof(_vector));

		vUp = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
		vUp *= -1.f;

		vPos += vUp * (18.5f);

		memcpy(&BoneFloat4x4.m[3], &vPos, sizeof(_vector));

		return XMLoadFloat4x4(&BoneFloat4x4);
	}
	else if (PATTERN::SPIKETAIL == m_eCurPattern)
	{
		_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Tail") * m_pTransform->Get_WorldMatrix();
		_float4x4 BoneFloat4x4;

		_vector vPos = XMVectorZero();

		XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

		_vector vUp = XMVectorZero();
		_vector vRight = XMVectorZero();

		memcpy(&vRight, &BoneFloat4x4.m[0], sizeof(_vector));
		memcpy(&vPos, &BoneFloat4x4.m[3], sizeof(_vector));

		vUp = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vRight));
		vUp *= -1.f;

		vPos += vUp * (6.5f * iCount);

		memcpy(&BoneFloat4x4.m[3], &vPos, sizeof(_vector));

		return XMLoadFloat4x4(&BoneFloat4x4);
	}

	return XMMatrixIdentity();
}

void CZhuangzi::Check_90PercentHP()
{
	if (true == m_bFirstGroggy)
		return;

	if (0.9 >= m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp)
	{
		if (PHASE::PHASE_NORMAL == m_eCurPhase)
		{
			m_eMemoryPattern = m_eCurPattern;
			m_iMemoryAnimation = m_iAnimation;

			m_eCurPattern = PATTERN::FIRSTGROGGY;
			m_iAnimation = ANIM_ZHUANGZI_GROGGY_START;

			m_pModel->Set_NoneBlend();

			m_bLookLerp = false;
			m_LookLerpTime = 0.0;

			m_bIdle = false;
			m_IdleDelayTime = 0.0;

			m_iRepeatCount = 0;
			m_bCheckIf = false;

			m_bFirstGroggy = true;
			m_bContinue = false;

			m_bSoundTiming = false;
			m_bBlackOut = true;
			Delete_BodyShield();
			Delete_LegShields();
			Delete_Pile();
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();
			pCamMgr->Set_MainCam_Option(CMainCamera::OPTION::CATMULLROM_TOP_TO_3RD);
		}
	}
}

void CZhuangzi::Check_15PercentHP()
{
	if (0.25 >= m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp)
	{
		if (PHASE::PHASE_NORMAL == m_eCurPhase)
		{
			m_eCurPattern = PATTERN::GROGGY;
			m_iAnimation = ANIM_ZHUANGZI_GROGGY_START;

			m_pModel->Set_NoneBlend();

			m_bLookLerp = false;
			m_LookLerpTime = 0.0;

			m_bIdle = false;
			m_IdleDelayTime = 0.0;

			m_iRepeatCount = 0;
			m_bCheckIf = false;

		}
	}
}

HRESULT CZhuangzi::Create_Missile(_double TimeDelta)
{
	if (ANIM_ZHUANGZI_MISSILE_DW == m_pModel->Get_CurrentAnimation())
	{
		m_CreateTime += TimeDelta;

		if (m_CreateTime >= m_CreateDelayTime)
		{
			for (_uint i = 0; i < 6; ++i)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

				_matrix BoneMatrix = XMMatrixIdentity();
				_float4x4 BoneFloat4x4;

				if (0 == (i % 2))
					BoneMatrix = m_pModel->Get_BoneMatrix(L"LeftMissile") * m_pTransform->Get_WorldMatrix();
				else if (1 == (i % 2))
					BoneMatrix = m_pModel->Get_BoneMatrix(L"RightMissile") * m_pTransform->Get_WorldMatrix();

				XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

				_vector vPosition = XMVectorZero();
				memcpy(&vPosition, &BoneFloat4x4.m[3], sizeof(_vector));
				
				_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
				_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));

				_uint iRandRight = rand() % 2;
				_float fRandX = (_float)(rand() % 3) * 0.4f;

				if (iRandRight)
					vRight *= -1.f;

				CZhuangziFireMissile* pMissile = nullptr;
				FIRE_MISSILE_DESC tDesc;
				tDesc.vDirection = vLook;
				vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + 5.f);
				vPosition += vLook * -3.5f;
				vPosition += vRight * fRandX;

				tDesc.vPosition = vPosition;
				pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Zhuangzi_FireMissile", L"MonsterAttack", (CGameObject**)&pMissile, &tDesc);
				if (nullptr == pMissile)
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				if (m_iNumMissile % 4 == 0)
				{
					CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::BOSS_EFFECT);
				}

				else if (m_iNumMissile % 4 == 1)
				{
					CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE);
				}

				else if (m_iNumMissile % 4 == 2)
				{
					CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE2);
				}

				else if (m_iNumMissile % 4 == 3)
				{
					CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE3);
				}

				++m_iNumMissile;

				CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
				if (nullptr == pTransform)
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				pMissile->Set_TargetPos(pTransform->Get_State(CTransform::STATE::POSITION));
				RELEASE_INSTANCE(CGameInstance);
			}

			m_CreateTime = 0.0;
		}
	}

	return S_OK;
}

HRESULT CZhuangzi::Create_WeaponTrail()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	// HMH
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = 5.f;
	tPacket.tEffectMovement.fSpeed = ZHUANGZI_WEAPONTRAIL_SPEED;
	XMStoreFloat3(&tPacket.tEffectMovement.vDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

	// Offset
	//tPacket.tEffectMovement.vPosition.y += 0.2f;

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_WEAPONTRAIL;
	CGameObject* pObj = pFactory->Load_EffectProto(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_WeaponTrail.dat", &tPacket, L"Prototype_Zhuangzi_WeaponTrail", L"MonsterAttack");
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CZhuangzi::Create_BodyShield()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	if (!m_pBodyShield)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_BODY_SHIELD;
		m_pBodyShield = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Body.dat", &tPacket);
		if (!m_pBodyShield)
			assert(FALSE);
		Safe_AddRef(m_pBodyShield);

		CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi Shield_Activate.wav", CSoundMgr::CHANNELID::BOSS);
	}
	return S_OK;
}

HRESULT CZhuangzi::Create_LegShield()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	if (!m_pLeftLegShield0)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pLeftLegShield0 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pLeftLegShield0)
			assert(FALSE);
		Safe_AddRef(m_pLeftLegShield0);
	}
	if (!m_pLeftLegShield1)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pLeftLegShield1 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pLeftLegShield1)
			assert(FALSE);
		Safe_AddRef(m_pLeftLegShield1);
	}
	if (!m_pLeftLegShield2)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pLeftLegShield2 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pLeftLegShield2)
			assert(FALSE);
		Safe_AddRef(m_pLeftLegShield2);
	}
	if (!m_pRightLegShield0)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pRightLegShield0 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pRightLegShield0)
			assert(FALSE);
		Safe_AddRef(m_pRightLegShield0);
	}
	if (!m_pRightLegShield1)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pRightLegShield1 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pRightLegShield1)
			assert(FALSE);
		Safe_AddRef(m_pRightLegShield1);
	}
	if (!m_pRightLegShield2)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_LEG_SHIELD;
		m_pRightLegShield2 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Shield_Leg.dat", &tPacket);
		if (!m_pRightLegShield2)
			assert(FALSE);
		Safe_AddRef(m_pRightLegShield2);
	}

	return S_OK;
}

HRESULT CZhuangzi::Create_Pile()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	if (!m_pPileEffect)
	{
		EFFECT_PACKET tPacket;
		tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
		tPacket.fMaxLifeTime = -1.f;
		XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

		tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_BODY_SHIELD;
		m_pPileEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Pile.dat", &tPacket);
		if (!m_pPileEffect)
			assert(FALSE);
		Safe_AddRef(m_pPileEffect);
	}
	return S_OK;
}

HRESULT CZhuangzi::Create_ImpactEffects()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	CGameObject* pObj = nullptr;


	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_SPARK;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	/*tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_PLANE;
	pObj = pFactory->Load_Ef fect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Plane.dat", &tPacket);
	if (!pObj)
		assert(FALSE);*/

	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_SMOKE;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Smoke.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	CDecalCube::DECALCUBE tCube;
	tCube.eType = CDecalCube::TYPE::ARCH;
	tCube.iImageIndex = 2;
	tCube.dDuration = 1.85;
	tCube.vPosition = tPacket.tEffectMovement.vPosition;
	tCube.vPosition.y -= 0.45f;
	tCube.vScale = _float4(60.5f, 1.5f, 60.5f, 0.f);
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	tCube.eType = CDecalCube::TYPE::FOGRING;
	tCube.iImageIndex = 1;
	tCube.dDuration = 0.75;
	tCube.vPosition = tPacket.tEffectMovement.vPosition;
	tCube.vPosition.y -= 0.45f;
	tCube.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	tCube.fAddScale = 7.5f;
	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
	CSoundMgr::Get_Instance()->PlaySound(L"Zhuangzi_Crash.mp3", CSoundMgr::BOSS_EFFECT);

	return S_OK;
}

HRESULT CZhuangzi::Create_Ring(_vector _vPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = 20.f;
	tPacket.iAtt = 20;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), _vPos);

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_RING;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Ring0.dat", &tPacket, L"Zhuangzi_RingWave");
	if (!pObj)
		assert(FALSE);

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();
	pCamMgr->Set_MainCam_ShakeHitBig_True();

	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
	CSoundMgr::Get_Instance()->PlaySound(L"Zhaungzi_Ring.wav", CSoundMgr::BOSS_EFFECT);

	return S_OK;
}

HRESULT CZhuangzi::Create_Ring_Spark(_vector _vPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), _vPos);

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_RING_SPARK;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_CreateRing_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
	
}

HRESULT CZhuangzi::Create_FrontBlade_Spark(_vector _vPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), _vPos);

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_FRONTBLADE_SPARK;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_FrontBlade_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CZhuangzi::Create_TailSweep_Spark(_vector _vPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), _vPos);

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_TAILSWEEP_SPARK;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Sweep_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CZhuangzi::Create_Rollin_Spark(_vector _vPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), _vPos);
	 
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_TAILSWEEP_SPARK;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Rollin_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CZhuangzi::Create_DeathEffect()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	CGameObject* pObj = nullptr;


	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	tPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_EXPLOSION;

	_float4 vPosition;
	XMStoreFloat4(&(vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition = vPosition;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);


	tPacket.tEffectMovement.vPosition.y += 4;
	tPacket.tEffectMovement.vPosition.x += 4;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	tPacket.tEffectMovement.vPosition.x -= 4;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	tPacket.tEffectMovement.vPosition.y += 3;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	tPacket.tEffectMovement.vPosition.x += 8;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CZhuangzi::Delete_BodyShield()
{
	if (m_pBodyShield)
	{
		m_pBodyShield->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pBodyShield);
		m_pBodyShield = nullptr;
	}

	return S_OK;
}

HRESULT CZhuangzi::Delete_LegShields()
{
	if (m_pLeftLegShield0)
	{
		m_pLeftLegShield0->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLeftLegShield0);
		m_pLeftLegShield0 = nullptr;
	}
	if (m_pLeftLegShield1)
	{
		m_pLeftLegShield1->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLeftLegShield1);
		m_pLeftLegShield1 = nullptr;
	}
	if (m_pLeftLegShield2)
	{
		m_pLeftLegShield2->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLeftLegShield2);
		m_pLeftLegShield2 = nullptr;
	}
	if (m_pRightLegShield0)
	{
		m_pRightLegShield0->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pRightLegShield0);
		m_pRightLegShield0 = nullptr;
	}
	if (m_pRightLegShield1)
	{
		m_pRightLegShield1->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pRightLegShield1);
		m_pRightLegShield1 = nullptr;
	}
	if (m_pRightLegShield2)
	{
		m_pRightLegShield2->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pRightLegShield2);
		m_pRightLegShield2 = nullptr;
	}
	
	
	return S_OK;
}

HRESULT CZhuangzi::Delete_Pile()
{
	if (m_pPileEffect)
	{
		m_pPileEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pPileEffect);
		m_pPileEffect = nullptr;
	}

	return S_OK;
}

void CZhuangzi::Set_DiagonalRight()
{
	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	_vector vDiagonalRight = XMVector3Normalize(vRight + vLook);

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDiagonalRight));

	vRight *= XMVectorGetX(vScale);
	vDiagonalRight *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vDiagonalRight);
}

void CZhuangzi::Set_Back()
{
	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	vLook *= -1.f;

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	vRight *= XMVectorGetX(vScale);
	vLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
}

void CZhuangzi::Set_OppositeRight()
{
	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vScale = m_pTransform->Get_Scale();

	vRight *= -1.f;
	_vector vLook = vRight;

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	vRight *= XMVectorGetX(vScale);
	vLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
}

void CZhuangzi::Set_UI_Engage_Cinematic()
{
	m_iFlagUI |= FLAG_UI_CALL_CINEMATIC;
}

void CZhuangzi::Set_UI_Engage()
{
	m_iFlagUI |= FLAG_UI_CALL_ENGAGE;
}

HRESULT CZhuangzi::Update_Effects(_float fTimeDelta)
{
	//	1825 1826	Left0
	//	1841 1842	Left1
	//	1857 1858 	Left2
	//	1569 1570	Right0
	//	1585 1586	Right1
	//	1601 1602	Right2
	//	003			Center		+y
	//	175			TailEnd		-z -x
	//	560 529		LeftHand		+y -x
	//	288 304		RightHand	+y +x

	if (m_pBodyShield)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pBodyShield->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::BODY_BOTTOM), "bone003");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}

	_float fLegOffsetY = -1.f;
	if (m_pLeftLegShield0)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLeftLegShield0->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_0), "bone1825");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		vLook = XMVectorSetW(vLook, 0.f);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	
	if (m_pLeftLegShield1)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLeftLegShield1->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_1), "bone1841");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		vLook = XMVectorSetW(vLook, 0.f);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	 
	if (m_pLeftLegShield2)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLeftLegShield2->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::LEFT_LEG_2), "bone1857");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	
	if (m_pRightLegShield0)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pRightLegShield0->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_0), "bone1570");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	
	if (m_pRightLegShield1)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pRightLegShield1->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_1), "bone1586");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	if (m_pRightLegShield2)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pRightLegShield2->Get_Component(L"Com_Transform"));

		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::RIGHT_LEG_2), "bone1602");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + fLegOffsetY);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}
	
	

	if (m_pPileEffect)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pPileEffect->Get_Component(L"Com_Transform"));
		
		_matrix matBoneMatrix = m_pModel->Get_MeshContainer_Root(static_cast<_int>(MESH_CONTAINER::OUT_BODY), "bone003");
		_vector vPos = XMVector4Transform(static_cast<_vector>(matBoneMatrix.r[3]), m_pTransform->Get_WorldMatrix());
		_vector vLook = XMVector3Transform(static_cast<_vector>(matBoneMatrix.r[2]), m_pTransform->Get_WorldMatrix());
		vLook = XMVectorSetW(vLook, 0.f);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) - 4.f);
		pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		pTransform->LookAt_Direction(vLook);
	}



	return S_OK;
}

void CZhuangzi::BlackOut(_double dTimeDelta)
{
	if (m_bBlackOut)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		pGameInstance->Decrease_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 8.5f * (_float)dTimeDelta), _NOCOLOR, (_uint)CLight::COLOR::DIFFUSE);
		pGameInstance->Decrease_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 7.8f * (_float)dTimeDelta), _NOCOLOR, (_uint)CLight::COLOR::AMBIENT);
		pGameInstance->Decrease_LightColor(L"ShadowDepth", CProportionConverter()(_SNOW, 0.42f * (_float)dTimeDelta), _NOCOLOR, (_uint)CLight::COLOR::SPECULAR);
		LIGHTDESC* pDirLight = pGameInstance->Get_LightDesc(L"ShadowDepth");
		if (pDirLight->vDiffuse.x == _NOCOLOR.x)
		{
			pDirLight->eState = LIGHTSTATE::WAIT;
		}
		pGameInstance->Increase_Range(L"ZhuanziMagma", 100.f * (_float)dTimeDelta, 250.f);
		pGameInstance->Increase_InnerDegree(L"BlackOutSpotLight", 0.7f * (_float)dTimeDelta, 5.f);
		pGameInstance->Increase_OuterDegree(L"BlackOutSpotLight", 0.98f * (_float)dTimeDelta, 7.f);

		LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"BlackOutSpotLight");
		pLightDesc->eState = LIGHTSTATE::ALWAYS;



		if (!m_bChangeFog)
		{
			Change_FogParams(dTimeDelta);
		}
	}
}

void CZhuangzi::Change_FogParams(_double dTimeDelta)
{
	_float fFogBegin = m_pRenderer->Get_FogBegin();
	_float fFogEnd = m_pRenderer->Get_FogEnd();
	_float4 vFogColor = m_pRenderer->Get_FogColor();

	if (fFogBegin >= m_fFogBegin && fFogEnd >= m_fFogEnd)
	{
		m_bChangeFog = true;
		return;
	}

	_float fBeginParams = (m_fFogBegin - 0) * (dTimeDelta / 2.f);
	_float fEndParams = (m_fFogEnd - 60) * (dTimeDelta / 2.f);

	m_pRenderer->Set_Fog(fBeginParams + fFogBegin, fEndParams + fFogEnd, vFogColor);

}

void CZhuangzi::Update_HitType()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	
	_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
	_double LinkMinRatio = m_pModel->Get_LinkMinRatio();

	if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
		m_eHitType = HIT::NONE;

	else if (PHASE::PHASE_NORMAL == m_eCurPhase)
	{
		if (ANIM_ZHUANGZI_CHASE_DW == iCurAnimIndex)
			m_eHitType = HIT::SINGLE;
		else if (ANIM_ZHUANGZI_CHANGE_WEEKJUMP_CHASE_STOP == iCurAnimIndex)
		{
			if (LinkMinRatio >= PlayTimeRatio)
				m_eHitType = HIT::SINGLE;
			else
				m_eHitType = HIT::NONE;
		}
		else
			m_eHitType = HIT::NONE;
	}
	else if (PHASE::PHASE_SPHERE == m_eCurPhase)
	{
		if (PATTERN::CHANGE_DIZZY == m_eCurPattern)
			m_eHitType = HIT::NONE;
		else
		{
			if (ANIM_ZHUANGZI_CHANGE_CHASE_TARGET == iCurAnimIndex || ANIM_ZHUANGZI_CHANGE_WEEKJUMP_CHASE_STOP == iCurAnimIndex)
			{
				m_eHitType = HIT::MULTI;
				m_HitDelayTime = 1.0;
			}
			else
				m_eHitType = HIT::NONE;
		}
	}
}

void CZhuangzi::Notify(void * pMessage)
{
}

_int CZhuangzi::VerifyChecksum(void * pMessage)
{
	return _int();
}

_bool CZhuangzi::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CZhuangzi::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		if (m_eCurPhase == PHASE::PHASE_SPHERE)
			return;
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
	else
	{

	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

HRESULT CZhuangzi::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dDeltaTime;

		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;
		desc.eID = CUI_Dialogue_EngageMessage::BOSSID::ZHUANGZI;

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

		if (TimeAcc < 7.0)
		{
			CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

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
		CUI_Dialogue_EngageMessage::BOSSID	eID = CUI_Dialogue_EngageMessage::BOSSID::ZHUANGZI;

		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eID));

		m_iFlagUI ^= FLAG_UI_CALL_ENGAGE;
	}

	if (m_eCurPhase != CZhuangzi::PHASE::PHASE_APPEARANCE)
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

HRESULT CZhuangzi::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_MODEL_ZHUANGZI, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;		

	m_pModel->Add_RefNode(L"LeftMissile", "bone3348");
	m_pModel->Add_RefNode(L"RightMissile", "bone3428");
	m_pModel->Add_RefNode(L"Tail", "bone163");
	m_pModel->Add_RefNode(L"EndofTail", "bone175");
	                                                  
	/* For.Com_Navigation*/
	_uint iCurrentIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;		


	CCollider::tagColliderDesc tColliderDesc;
	tColliderDesc.vPivot = _float3(0.f, 5.f, 0.f);
	tColliderDesc.vScale = _float3(4.f, 5.f, 4.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = _float3(0.f, 5.f, 0.f);
	tColliderDesc.fRadius = BEAUVOIR_SPHERE_COLLIDER_RADIUS;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.vPivot.y += 11.5f;
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CZhuangzi * CZhuangzi::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CZhuangzi* pGameInstance = new CZhuangzi(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangzi");
	}

	return pGameInstance;
}

CGameObject * CZhuangzi::Clone(void * pArg)
{
	CZhuangzi* pGameInstance = new CZhuangzi(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangzi");
	}

	return pGameInstance;
}

void CZhuangzi::Free()
{
	__super::Free();


	Delete_BodyShield();
	Delete_Pile();
	Delete_LegShields();

	//Safe_Release(m_pBodyShield);
	//Safe_Release(m_pPileEffect);

	//Safe_Release(m_pLeftLegShield0);
	//Safe_Release(m_pLeftLegShield1);
	//Safe_Release(m_pLeftLegShield2);
	//Safe_Release(m_pRightLegShield0);
	//Safe_Release(m_pRightLegShield1);
	//Safe_Release(m_pRightLegShield2);

	
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}
	m_vecNormalAttacks.clear();
}
