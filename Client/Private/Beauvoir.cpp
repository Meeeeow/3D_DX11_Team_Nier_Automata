#include "stdafx.h"
#include "..\Public\Beauvoir.h"
#include "EffectFactory.h"
#include "EffectBullet.h"
#include "Beauvoir_Bullet.h"
#include "Beauvoir_FireMissile.h"
#include "Beauvoir_NaviBullet.h"
#include "Beauvoir_RisingBullet.h"
#include "BeauvoirCurtain.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "CameraMgr.h"
#include "DecalCube.h"
#include "SoundMgr.h"

CBeauvoir::CBeauvoir(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CBeauvoir::CBeauvoir(const CBeauvoir & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CBeauvoir::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iAnimation = ANIM_SPREADBULLET_START;
	m_eCurPhase = PHASE::PHASE_APPEARANCE;
	m_eCurPattern = PATTERN::JUMP_FLOOR;
	m_bContinue = false;

	MO_INFO* tInfo = (MO_INFO*)pArg;
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo->m_matWorld));

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	static _uint iCPod_InstanceID = OriginID_Beauvoir;
	m_iObjID = iCPod_InstanceID++;

	XMStoreFloat4x4(&m_RisingRotateFloat4x4, XMMatrixIdentity());

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	m_fHitRecovery = BEAUVOIR_HIT_RECOVERAY;

	m_tObjectStatusDesc.fHp = BEAUVOIR_MAX_LIFE;
	m_tObjectStatusDesc.fMaxHp = BEAUVOIR_MAX_LIFE;
	m_tObjectStatusDesc.iAtt = BEAUVOIR_ATTACK_DAMAGE;
	
	m_bCollision = true;

	m_eHitType = HIT::NONE;

	m_iCreateRingWaveTetard = 0;

	Check_Pause();

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vOut = vPos;
	CCell* pSlideCell = nullptr;
	if (m_pNavigation->IsOn(vOut, &pSlideCell))
	{
		if (!XMVector4Equal(XMVectorZero(), vOut))
			m_pTransform->Set_State(CTransform::STATE::POSITION, vOut);
	}

	Set_ReverseLook();

	CrossAndroid_Pulling();

	ZeroMemory(m_arrRimLightColor, sizeof(_float3) * static_cast<_int>(MESH_CONTAINER::COUNT));
	ZeroMemory(m_arrIsGlow, sizeof(_int) * static_cast<_int>(MESH_CONTAINER::COUNT));

	return S_OK;
}

_int CBeauvoir::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
	{
		CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

		tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::BEAUVOIR;
		tObjectiveDesc.iEventCount = 1;

		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));

		Create_DeathEffect();

		return (_int)m_eState;
	}

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_bDeadStop)
		dTimeDelta *= 0.35;

	if (PHASE::PHASE_APPEARANCE == m_eCurPhase && (m_iAnimation == ANIM_SPREADBULLET_START || m_iAnimation == ANIM_SPREADBULLET_DW || m_iAnimation == ANIM_SPREADBULLET_END))
		dTimeDelta *= 0.55f;

	Check_Dead();
	Check_40PercentHP();

	ZeroMemory(&m_arrRimLightColor, sizeof(_float3) * static_cast<_int>(MESH_CONTAINER::COUNT));
	ZeroMemory(m_arrIsGlow, sizeof(_int) * static_cast<_int>(MESH_CONTAINER::COUNT));

	Check_Pattern(dTimeDelta);
	
	Look_Target(dTimeDelta);
	Chase_Target(dTimeDelta);
	Create_JumpBullet(dTimeDelta);
	MoveToStage(dTimeDelta);
	Look_Forward(dTimeDelta);
	Create_FireMissile(dTimeDelta);
	Rotate_Move(dTimeDelta);

	Create_NaviBullet(dTimeDelta);
	Create_RisingBullet(dTimeDelta);
	Check_Distance();
	JumpToPlayerPos(dTimeDelta);

	Check_CrossAndroid(dTimeDelta);

	Check_25PercentHP();
	Check_60PercentHP();

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Update_Parts();
	Update_Effects(static_cast<_float>(dTimeDelta));

	if (PHASE::PHASE_APPEARANCE != m_eCurPhase)
		Update_Force(dTimeDelta, m_pNavigation);

	Update_CollisionTime(dTimeDelta);

	Update_UI(dTimeDelta);
	Update_HitType();
	Update_Collider();

	RimLighting(dTimeDelta);
	return (_int)OBJSTATE::ENABLE;

}

_int CBeauvoir::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_bDeadStop)
		dTimeDelta *= 0.35;

	if (PHASE::PHASE_APPEARANCE == m_eCurPhase && (m_iAnimation == ANIM_SPREADBULLET_START || m_iAnimation == ANIM_SPREADBULLET_DW || m_iAnimation == ANIM_SPREADBULLET_END))
		dTimeDelta *= 0.55f;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance->Culling(m_pTransform, m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);
	return _int();
}

HRESULT CBeauvoir::Render()
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
		_float4 vColor = CProportionConverter()(_INDIANRED, 100.f);
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		_float3 vRimLightColor = _float3(0.f, 0.f, 0.f);
		switch (i)
		{
		case 0:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 1:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 2:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 3:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 4:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 5:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 6:
			vRimLightColor = _float3(m_vRimColor.x, m_vRimColor.y, m_vRimColor.z);
			break;
		case 33:
			vRimLightColor = _float3(vColor.x, vColor.y, vColor.z);
			break;
		default:
			break;
		}
		m_pModel->SetUp_RawValue("g_vRimLightColor", &vRimLightColor, sizeof(_float3));
		m_pModel->Render(i, m_iPassIndex);
	}

	return S_OK;
}

void CBeauvoir::Check_Pattern(_double TimeDelta)
{
	if (PHASE::PHASE1 == m_eCurPhase)
	{
		if (PATTERN::CHASE == m_eCurPattern)
		{
			m_PatternTime += TimeDelta;
			m_PatternDelayTime = 4.0;
			if (m_PatternTime >= m_PatternDelayTime)
			{
				m_eCurPattern = PATTERN::ROTATE_MELEE;
				m_iAnimation = ANIM_BEAUVOIR_IDLE;
				m_PatternTime = 0.0;
			}
		}
		else if (PATTERN::ROTATE_MELEE == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			if (ANIM_MELEE_ROTATE_DW != iCurAnimIndex && ANIM_MELEE_ROTATE_END != iCurAnimIndex)
			{
				m_iAnimation = ANIM_MELEE_ROTATE_START;
				m_bContinue = false;
			}

			if (ANIM_MELEE_ROTATE_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
 					m_iAnimation = ANIM_MELEE_ROTATE_DW;
					m_pModel->Set_NoneBlend();
					Create_LargeSparkEffect();
					m_iRepeatCount = 0;
				}
			}
			else if (ANIM_MELEE_ROTATE_DW == iCurAnimIndex)
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					if (3 > m_iRepeatCount)
					{
						++m_iRepeatCount;
						m_bContinue = true;
					}
					else
						m_bContinue = false;
				}
				if (m_pModel->Get_PlayTimeRatio() >= 0.95)
				{
					if (4 <= m_iRepeatCount)
						Create_EndLargeSparkEffect();
				}


				if (3 == m_iRepeatCount)
				{
					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_MELEE_ROTATE_END;
						m_bContinue = false;
						m_pModel->Set_NoneBlend();

					}
				}
				m_arrRimLightColor[static_cast<_int>(MESH_CONTAINER::BLADE)] = _float3(1.f, 0.f, 0.f);
				m_arrIsGlow[static_cast<_int>(MESH_CONTAINER::BLADE)] = 1;

			}
			else if (ANIM_MELEE_ROTATE_END == iCurAnimIndex)
			{
				m_LookLerpTime = 0.0;

				if (0.97 <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_SPREADBULLET_START;
					m_bContinue = false;
					m_eCurPattern = PATTERN::SPREAD_BULLET;
					m_iRepeatCount = 0;
					//Create_EndLargeSparkEffect();
					Delete_LargeSparkEffect();
					Check_Hp();
				}

			}
		}
		else if (PATTERN::SPREAD_BULLET == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			if (ANIM_SPREADBULLET_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_CreateJumpBulletDelayTime = 0.55;
					m_iAnimation = ANIM_SPREADBULLET_DW;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_DW == iCurAnimIndex)
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					if (3 > m_iRepeatCount)
					{
						++m_iRepeatCount;
						m_bContinue = true;
					}
					else
						m_bContinue = false;
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_SPREADBULLET_END;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_CreateJumpBulletTime = 0.0;
					m_bFastBullet = false;
				}
			}
			else if (ANIM_SPREADBULLET_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_FIRE_MISSILES;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::FIRE_MISSILES;
					m_iRepeatCount = 0;

					Check_Hp();
				}
			}
		}
		else if (PATTERN::FIRE_MISSILES == m_eCurPattern)
		{
			if (ANIM_FIRE_MISSILES == m_pModel->Get_CurrentAnimation())
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					if (7 > m_iRepeatCount)
					{
						++m_iRepeatCount;
						m_bContinue = true;
					}
					else
						m_bContinue = false;
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ROTATE_LASER_START;
					m_LookLerpTime = 0.0;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::ROTATE_LASER;

					Check_Hp();
				}
			}
		}
		else if (PATTERN::ROTATE_LASER == m_eCurPattern)
		{
			if (ANIM_ROTATE_LASER_START == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ROTATE_LASER_DW;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					Create_SemiLasers();
					m_iRepeatCount = 0;

				}
			}
			else if (ANIM_ROTATE_LASER_DW == m_pModel->Get_CurrentAnimation())
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					if (m_iContinueCount < 8)
					{
						m_iContinueCount++;
						m_bContinue = true;
					}
					else if (m_iContinueCount > 7)
					{
						m_iContinueCount = 0;
						m_bContinue = false;
						m_iAnimation = ANIM_ROTATE_LASER_END;
						m_pModel->Set_NoneBlend();
					}
				}
				else if (m_iContinueCount == 1)
				{
					m_bContinue = false;
				}
			}
			else if (ANIM_ROTATE_LASER_END == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_LookLerpTime = 0.0;
					m_iAnimation = ANIM_LASER_RING_START;
					//m_iAnimation = ANIM_ROTATE_LASER_START;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::LASER_RING;
					//m_eCurPattern = PATTERN::ROTATE_LASER;
					Delete_Lasers();

					Check_Hp();
				}
			}
		}
		else if (PATTERN::LASER_RING == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_ROTATE_LASER_DW != iCurAnimIndex && ANIM_LASER_RING_END != iCurAnimIndex)
			{
				m_iAnimation = ANIM_LASER_RING_START;
				m_bContinue = false;
				m_iContinueCount = 0;
			}

			if (ANIM_LASER_RING_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_ROTATE_LASER_DW;
					m_pModel->Set_NoneBlend();
					Create_Donut_Up();
				}
			}
			else if (ANIM_ROTATE_LASER_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_LASER_RING_END;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_LASER_RING_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_MELEE_ROTATE_START;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::FASTROTATE_MELEE;

					Check_Hp();
				}
			}
		}
		else if (PATTERN::FASTROTATE_MELEE == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_MELEE_ROTATE_DW != iCurAnimIndex && ANIM_MELEE_ROTATE_END != iCurAnimIndex)
			{
				m_iAnimation = ANIM_MELEE_ROTATE_START;
				m_bContinue = false;
			}

			if (ANIM_MELEE_ROTATE_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					Create_LargeSparkEffect();
					m_iAnimation = ANIM_MELEE_ROTATE_DW;
					m_bContinue = true;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_MELEE_ROTATE_DW == iCurAnimIndex)
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					if (4 > m_iRepeatCount)
						++m_iRepeatCount;
					else
						m_bContinue = false;
				}

				if (m_pModel->Get_PlayTimeRatio() >= 0.95)
				{
					if (4 <= m_iRepeatCount)
						Create_EndLargeSparkEffect();
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_MELEE_ROTATE_END;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_iRepeatCount = 0;
				}

				m_arrRimLightColor[static_cast<_int>(MESH_CONTAINER::BLADE)] = _float3(1.f, 0.f, 0.f);
				m_arrIsGlow[static_cast<_int>(MESH_CONTAINER::BLADE)] = 1;
			}
			else if (ANIM_MELEE_ROTATE_END == iCurAnimIndex)
			{
				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					Delete_LargeSparkEffect();
					m_iAnimation = ANIM_SPREADBULLET_START;
					m_bContinue = false;
					m_eCurPattern = PATTERN::FASTSPRED_BULLET;
					m_LookLerpTime = 0.0;
					m_pModel->Set_NoneBlend();

					Check_Hp();
				}
			}
		}
		else if (PATTERN::FASTSPRED_BULLET == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_SPREADBULLET_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_DW;
					m_pModel->Set_NoneBlend();
					m_CreateJumpBulletDelayTime = 0.3;
				}
			}
			else if (ANIM_SPREADBULLET_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_END;
					m_pModel->Set_NoneBlend();
					m_CreateJumpBulletTime = 0.0;
				}
			}
			else if (ANIM_SPREADBULLET_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_START;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::NAVI_BULLET;
					m_LookLerpTime = 0.0;

					Check_Hp();
				}
			}
		}
		else if (PATTERN::NAVI_BULLET == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_SPREADBULLET_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_DW;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_SPREADBULLET_END;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_CreateNaviBulletTime = 0.0;
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_START;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::RISING_BULLET;
					m_fGapAngle = 0.f;
					m_bCreateCrossAndroid = false;

					Check_Hp();
				}
			}
		}
		else if (PATTERN::RISING_BULLET == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_SPREADBULLET_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_CreateRisingBulletTime = 0.0;
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_DW;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_DW == iCurAnimIndex)
			{
				if (0.5 >= m_pModel->Get_PlayTimeRatio())
					m_bRepeat = false;

				else if (0.97 <= m_pModel->Get_PlayTimeRatio())
				{
					if (4 == m_iRepeatCount)
					{
						m_bRepeat = true;
						m_bContinue = false;
					}
					
					else if (false == m_bRepeat)
					{
						m_bContinue = true;
						m_iRepeatCount += 1;
						
						m_bRepeat = true;

					}

				}
			
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_SPREADBULLET_END;
					m_pModel->Set_NoneBlend();
					m_iRepeatCount = 0;
				}
			}
			else if (ANIM_SPREADBULLET_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_bContinue = false;
					m_iAnimation = ANIM_MELEE_ROTATE_START;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::ROTATE_MELEE;
					m_CreateRisingBulletTime = 0.0;
					m_LookLerpTime = 0.0;
					m_iRepeatCount = 0;

					m_bRepeat = false;

					Check_Hp();
				}
			}
		}
	/*	else if (PATTERN::MOVE_STAGE == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_BEAUVOIR_IDLE == m_iAnimation)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_BEAUVOIR_JUMP;
					m_bJump = true;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_LookLerpTime = 0.0;
				}
				else if (m_bJump)
				{
					m_iAnimation = ANIM_BEAUVOIR_JUMP;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_BEAUVOIR_JUMP == m_iAnimation)
			{
				_float fY = 0.f;
				if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
				{
					_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
					vPos = XMVectorSetY(vPos, fY);

					m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_BEAUVOIR_IDLE;
					m_bJump = false;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_eCurPattern = PATTERN::LOOK_FOWARD;
					m_LookLerpTime = 0.0;
				}
			}
		}*/
		else if (PATTERN::LOOK_FOWARD == m_eCurPattern)
		{
			if (ANIM_BEAUVOIR_IDLE == m_pModel->Get_CurrentAnimation())
			{
				if (m_bCreateCrossAndroid)
				{
					m_iAnimation = ANIM_BEAUVOIR_IDLE;
					m_bContinue = false;
					m_eCurPattern = PATTERN::CHASE;
					m_LookLerpTime = 0.0;
					m_bCreateCrossAndroid = false;
				}
			}
		}
	}
	else if (PHASE::PHASE2 == m_eCurPhase)
	{
		if (PATTERN::CHANGE_JUMP == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_CHANGE_BEAUVOIR_CREATE_RING != iCurAnimIndex &&
				ANIM_CHANGE_BEAUVOIR_JUMP != iCurAnimIndex
				&& ANIM_CHANGE_IDLE != iCurAnimIndex
				&& ANIM_CHANGE_BEAUVOIR_RUN_DW != iCurAnimIndex
				&& ANIM_CHANGE_BEAUVOIR_RUN_START != iCurAnimIndex)
			{

				m_iAnimation = ANIM_CHANGE_APPEARANCE;
				m_bContinue = false;
				m_LookLerpTime = 0.0;

				if (ANIM_CHANGE_APPEARANCE == iCurAnimIndex)
				{
					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_CHANGE_BEAUVOIR_CREATE_RING;
						m_bContinue = false;
						m_pModel->Set_NoneBlend();
					}

				}
			}
			else if (ANIM_CHANGE_BEAUVOIR_CREATE_RING == iCurAnimIndex)
			{
				if (FAILED(Check_CreateRingWave_Tetrad()))
					return;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_CHANGE_BEAUVOIR_JUMP;
					m_bContinue = true;
					m_pModel->Set_NoneBlend();
					m_LookLerpTime = 0.0;
					m_iRepeatCount = 0;
				}
			}
			else if (ANIM_CHANGE_BEAUVOIR_JUMP == iCurAnimIndex)
			{
				_double TimeRatio = m_pModel->Get_PlayTimeRatio();
				
				if (m_pModel->Get_LinkMinRatio() <= TimeRatio)
				{
					if (!m_bChangeJumpLanding)
					{
						if (FAILED(Create_RingWave()))
							return;

						m_bChangeJumpLanding = true;
						Create_JumpSmoke();
						Create_JumpImpact();

					}
				}
				else
					m_bChangeJumpLanding = false;

				if (0.98 <= TimeRatio)
				{
					if (3 > m_iRepeatCount)
						++m_iRepeatCount;
				}

				if (m_bChangeJumpStop)
				{
					m_bContinue = false;
					if (m_pModel->IsFinish_Animation())
					{
						m_iAnimation = ANIM_CHANGE_IDLE;
						m_bContinue = false;
						m_pModel->Set_NoneBlend();
						m_LookLerpTime = 0.0;
						m_bChangeJumpLanding = false;

						m_bCreateCrossAndroid = false;
					}

				}
			}
			else if (ANIM_CHANGE_IDLE == iCurAnimIndex)
			{
				m_iCreateRingWaveTetard = 0;

				if (m_LookLerpTime >= 1.0)
				{
					if (m_bChangeJumpStop)
					{
						m_iAnimation = ANIM_CHANGE_BEAUVOIR_RUN_START;
						m_bContinue = false;
						m_iRepeatCount = 0;
					}
					else
					{
						m_iAnimation = ANIM_CHANGE_BEAUVOIR_JUMP;
						m_bContinue = true;
						m_LookLerpTime = 0.0;
						m_iRepeatCount = 0;
					}
				}
			}
			else if (ANIM_CHANGE_BEAUVOIR_RUN_START == iCurAnimIndex)
			{
				m_LookLerpTime = 0.0;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_CHANGE_BEAUVOIR_RUN_DW;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
					m_bChangeJumpStop = false;
				}
			}
			else if (ANIM_CHANGE_BEAUVOIR_RUN_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_CHANGE_IDLE;
					m_bContinue = true;
					m_pModel->Set_NoneBlend();
					m_LookLerpTime = 0.0;
				}
			}
		}
	}
	else if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
	{
		if (PATTERN::JUMP_FLOOR == m_eCurPattern)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			if (ANIM_SPREADBULLET_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_SPREADBULLET_DW;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_DW == iCurAnimIndex)
			{
				if (!m_bIsFirstSonicCreated && 0.0989 <= m_pModel->Get_PlayTimeRatio())
				{
					m_bIsFirstSonicCreated = true;
					Create_SonicEffect();
				}
				if (!m_bIsSecondSonicCreated && 0.3067 <= m_pModel->Get_PlayTimeRatio())
				{
					m_bIsSecondSonicCreated = true;
					Create_SonicEffect();
				}
				if (0.5464 <= m_pModel->Get_PlayTimeRatio())
				{
					m_dThirdSonicElapsedTime += TimeDelta;

					if (m_dThirdSonicElapsedTime >= BEAUVOIR_SONIC_COOLTIME)
					{
						m_dThirdSonicElapsedTime = 0.0;
						Create_SonicEffect();
					}
				}
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_SPREADBULLET_END;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_SPREADBULLET_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_BEAUVOIR_IDLE;
					m_bContinue = false;
					m_pModel->Set_NoneBlend();
				}
			}
			else if (ANIM_BEAUVOIR_IDLE == iCurAnimIndex)
			{
				m_pTransform->Set_Jump(true);
				m_pTransform->Jump(TimeDelta * 0.65);

				m_pTransform->Go_DirWithSpeed(TimeDelta * 0.65, m_pTransform->Get_State(CTransform::STATE::LOOK), BEAUVOIR_JUMPRUN);

				_float fY = 0.f;

				if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
				{
					_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
					vPos = XMVectorSetY(vPos, fY);

					m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

					m_eCurPhase = PHASE::PHASE1;
					m_eCurPattern = PATTERN::CHASE;

					m_pTransform->Set_Jump(false);
					m_pTransform->Initialize_JumpTime();

					CGameInstance* pGameInstance = CGameInstance::Get_Instance();
					LIGHTDESC* pLight1 = pGameInstance->Get_LightDesc(L"Spot_Villain");
					LIGHTDESC* pLight2 = pGameInstance->Get_LightDesc(L"Spot_Heroine");
					pLight1->eState = LIGHTSTATE::ALWAYS;
					pLight2->eState = LIGHTSTATE::ALWAYS;
					CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));
					if (pCurtain != nullptr)
					{
						pCurtain->Set_Animation(CBeauvoirCurtain::PATTERN::CLOSING);
					}
				}
			}
		}
	}
}

void CBeauvoir::Look_Target(_double TimeDelta)
{
	if (PATTERN::MOVE_STAGE == m_eCurPattern || PATTERN::MOVE_STAGE_JUMP == m_eCurPattern || PATTERN::STAGE_LOOKAT == m_eCurPattern 
		|| PATTERN::STAGE_IDLE == m_eCurPattern || PATTERN::STAGE_JUMP_FLOOR == m_eCurPattern || PATTERN::DEAD == m_eCurPattern)
		return;

	if (PHASE::PHASE1 == m_eCurPhase)
	{
		if (PATTERN::MOVE_STAGE == m_eCurPattern || PATTERN::LOOK_FOWARD == m_eCurPattern || PATTERN::ROTATE_LASER == m_eCurPattern || PATTERN::FASTROTATE_MELEE == m_eCurPattern
			|| PATTERN::LASER_RING == m_eCurPattern || PATTERN::NAVI_BULLET == m_eCurPattern || PATTERN::RISING_BULLET == m_eCurPattern)
			return;
	}

	else if (PHASE::PHASE2 == m_eCurPhase)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		if (ANIM_CHANGE_IDLE != iCurAnimIndex)
		{
			if (!m_bChangeLookTarget)
				return;
		}
	}

	else if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM, 0);
	if (nullptr == pPlayerTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vMyPos = XMVectorSetY(vMyPos, XMVectorGetY(vPlayerPos));
	_vector vLook = vPlayerPos - vMyPos;

	vLook = XMVector3Normalize(vLook);

	if (XMVector3Equal(vLook, XMLoadFloat3(&m_vLerpLook)))
	{
		if (m_LookLerpTime >= 1.0)
			m_LookLerpTime = 0.0;

		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	XMStoreFloat3(&m_vLerpLook, vLook);

	_vector vLerpLook = XMVectorLerp(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)), vLook, _float(m_LookLerpTime));

	vLerpLook = XMVector3Normalize(vLerpLook);

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vScale = m_pTransform->Get_Scale();

	vRight = XMVector3Cross(vUp, vLerpLook);
	vRight = XMVector3Normalize(vRight);

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);

	RELEASE_INSTANCE(CGameInstance);
}

void CBeauvoir::Chase_Target(_double TimeDelta)
{
	if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
		return;

	if (PATTERN::MOVE_STAGE == m_eCurPattern || PATTERN::MOVE_STAGE_JUMP == m_eCurPattern || PATTERN::STAGE_LOOKAT == m_eCurPattern
		|| PATTERN::STAGE_IDLE == m_eCurPattern || PATTERN::STAGE_JUMP_FLOOR == m_eCurPattern || PATTERN::DEAD == m_eCurPattern)
		return;

	if (PATTERN::MOVE_STAGE == m_eCurPattern || PATTERN::LOOK_FOWARD == m_eCurPattern || PATTERN::FASTROTATE_MELEE == m_eCurPattern || PATTERN::LASER_RING == m_eCurPattern
		|| PATTERN::NAVI_BULLET == m_eCurPattern || PATTERN::RISING_BULLET == m_eCurPattern)
		return;

	if (ANIM_MELEE_ROTATE_DW == m_pModel->Get_CurrentAnimation())
	{
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), WALKSPEED, m_pNavigation);
		XMStoreFloat3(&m_vToPlayerDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	}
}

void CBeauvoir::MoveToStage(_double TimeDelta)
{
	if (PATTERN::MOVE_STAGE == m_eCurPattern)
	{
		m_eState = OBJSTATE::DISABLE;

		m_LookLerpTime += (TimeDelta * 0.5);

		if (m_LookLerpTime >= 1.0)
			m_LookLerpTime = 1.0;

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vStagePos = XMLoadFloat4(&m_vStagePos);

		vStagePos = XMVectorSetY(vStagePos, XMVectorGetY(vPos));

		_vector vDir = XMVector3Normalize(vStagePos - vPos);
		_float fDist = XMVectorGetX(XMVector3Length(vStagePos - vPos));

		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		_vector vScale = m_pTransform->Get_Scale();

		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_LookLerpTime)));

		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorZero();

		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook));

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);

		if (m_LookLerpTime >= 1.0)
		{
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));
			if (nullptr == pCurtain)
				return;

			pCurtain->Set_Animation(CBeauvoirCurtain::PATTERN::OPENING);

			m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED);

			if (0.5f >= fDist)
			{
				m_eCurPattern = PATTERN::MOVE_STAGE_JUMP;
				m_LookLerpTime = 0.0;
			}
		}
	}
	else if (PATTERN::MOVE_STAGE_JUMP == m_eCurPattern)
	{
		if (m_LookLerpTime < 1.0)
		{
			m_LookLerpTime += TimeDelta * 1.2;

			if (m_LookLerpTime >= 1.0)
				m_LookLerpTime = 1.0;

			_vector vScale = m_pTransform->Get_Scale();

			_vector vRight = XMVectorZero();
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

			_vector vDir = XMVectorSet(1.f, 0.f, 0.f, 0.f);

			_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_LookLerpTime)));

			vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetZ(vScale));
		}
		else
		{
			m_iAnimation = ANIM_BEAUVOIR_IDLE;
			m_bContinue = true;

			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			if (ANIM_BEAUVOIR_IDLE == iCurAnimIndex)
			{
				m_pTransform->Set_Jump(true);
				m_pTransform->Jump(TimeDelta * 0.65);

				m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), BEAUVOIR_JUMPRUN);

				_float fY = 0.f;
				if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
				{
					_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
					vPos = XMVectorSetY(vPos, fY);

					m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
					m_eCurPattern = PATTERN::STAGE_LOOKAT;

					m_LookLerpTime = 0.0;
					m_pTransform->Set_Jump(false);
					m_pTransform->Initialize_JumpTime();
				}
			}
		}
	}
	else if (PATTERN::STAGE_LOOKAT == m_eCurPattern)
	{
		m_LookLerpTime += TimeDelta * 0.08;
		
		if (m_LookLerpTime >= 0.15)
			m_LookLerpTime = 1.0;

		_vector vScale = m_pTransform->Get_Scale();
		
		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

		_vector vDir = XMVectorSet(-0.98f, -0.01f, -0.01f, 0.f);
		vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

		vDir = XMVector3Normalize(XMVector3Cross(vRight, vUp));

		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_LookLerpTime)));

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetZ(vScale));

		if (m_LookLerpTime >= 1.0)
		{
			// ���뿡�� ������ �ٶ� ��
			if (m_iAnimation != ANIM_BEAUVOIR_HOWL)
			{
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();

				CTransform* pPlayerTransformCom = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

				pPlayerTransformCom->Set_State(CTransform::STATE::RIGHT, XMVectorSet(0.f, 0.f, 1.f, 0.f));
				pPlayerTransformCom->Set_State(CTransform::STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
				pPlayerTransformCom->Set_State(CTransform::STATE::LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));

				CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

				pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/Cross.dat",5);
			}
			m_LookLerpTime = 1.0;

			m_iAnimation = ANIM_BEAUVOIR_HOWL;
			m_bContinue = false;

			if (ANIM_BEAUVOIR_HOWL == m_pModel->Get_CurrentAnimation())
			{
				if (0.75 <= m_pModel->Get_PlayTimeRatio())
				{
					if (false == m_bCreateCrossAndroid)
						Create_CrossAndroid(TimeDelta);
				}

				if (0.98 <= m_pModel->Get_PlayTimeRatio())
				{
					m_eCurPattern = PATTERN::STAGE_IDLE;
					m_iAnimation = ANIM_BEAUVOIR_IDLE;

					m_pModel->Set_NoneBlend();

				}
			}
		}
	}
}

void CBeauvoir::Look_Forward(_double TimeDelta)
{
	if (PATTERN::LOOK_FOWARD == m_eCurPattern)
	{
		m_LookLerpTime += TimeDelta * 0.5;

		if (m_LookLerpTime >= 1.0)
			m_LookLerpTime = 1.0;

		_vector vForwardLook = XMVectorSet(0.f, 0.f, -1.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

		_vector vScale = m_pTransform->Get_Scale();
		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vForwardLook, _float(m_LookLerpTime)));

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

		vUp = XMVector3Normalize(XMVector3Cross(vLerpLook, vRight));

		vRight *= XMVectorGetX(vScale);
		vUp *= XMVectorGetY(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}
}

void CBeauvoir::Rotate_Move(_double TimeDelta)
{
	if (PATTERN::FASTROTATE_MELEE == m_eCurPattern)
	{
		if (ANIM_MELEE_ROTATE_END == m_pModel->Get_CurrentAnimation() || ANIM_MELEE_ROTATE_START == m_pModel->Get_CurrentAnimation())
			return;

		m_pTransform->Rotation_Axis(TimeDelta, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		
		CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		RELEASE_INSTANCE(CGameInstance);

		vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vMyPos));

		_vector vDir = XMVector3Normalize(vPlayerPos - vMyPos);
		m_pTransform->Go_DirWithSpeed(TimeDelta, vDir, BEAUVOIR_FASTRUN, m_pNavigation);
		XMStoreFloat3(&m_vToPlayerDirection, vDir);
		
	}
}

void CBeauvoir::JumpToPlayerPos(_double TimeDelta)
{
	if (PHASE::PHASE_APPEARANCE != m_eCurPhase)
	{
		if (ANIM_CHANGE_BEAUVOIR_JUMP == m_pModel->Get_CurrentAnimation())
		{
			if (2 == m_iRepeatCount)
			{
				_double TimeRatio = m_pModel->Get_PlayTimeRatio();

				if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
				{
					if (!m_bChangeLookTarget)
						m_LookLerpTime = 0.0;

					m_bChangeLookTarget = true;
				}
			}
			else if (3 == m_iRepeatCount)
			{
				m_bChangeLookTarget = false;
				m_LookLerpTime = 0.0;

				if (0.175 <= m_pModel->Get_PlayTimeRatio() && 0.44 >= m_pModel->Get_PlayTimeRatio())
					m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), BEAUVOIR_JUMPRUNSPEED, m_pNavigation);
			}
		}
	}
}

void CBeauvoir::Check_CrossAndroid(_double TimeDelta)
{
	if (PATTERN::STAGE_IDLE == m_eCurPattern)
	{
		if (0 == m_iNumCrossAndroid)
		{
			m_pTransform->Set_Jump(true);
			m_eCurPattern = PATTERN::STAGE_JUMP_FLOOR;

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));
			if (nullptr == pCurtain)
				return;

			pCurtain->Set_Animation(CBeauvoirCurtain::PATTERN::OPENING);

			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_Option(CMainCamera::OPTION::CATMULLROM_TOPB_TO_3RD);
		}
	}
	else if (PATTERN::STAGE_JUMP_FLOOR == m_eCurPattern)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));

		CModel* pModel = (CModel*)pCurtain->Get_Component(COM_KEY_MODEL);
		if (nullptr == pModel)
			return;

		if (pModel->IsFinish_Animation())
		{
			m_pTransform->Jump(TimeDelta * 0.65);

			m_pTransform->Go_DirWithSpeed(TimeDelta * 0.65, m_pTransform->Get_State(CTransform::STATE::LOOK), BEAUVOIR_JUMPRUN);

			_float fY = 0.f;
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
				vPos = XMVectorSetY(vPos, fY);

				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
				m_pTransform->Set_Jump(false);
				m_pTransform->Initialize_JumpTime();

				m_eCurPattern = PATTERN::ROTATE_MELEE;

				m_iAnimation = ANIM_MELEE_ROTATE_START;
				m_bContinue = false;

				m_b60PerOn = true;
				m_eState = OBJSTATE::ENABLE;
			}
		}
	}
}

void CBeauvoir::Set_ReverseLook()
{
	_vector vScale = m_pTransform->Get_Scale();
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	vLook *= -1.f;

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	vRight *= XMVectorGetX(vScale);
	vLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
}

void CBeauvoir::Check_25PercentHP()
{
	if (0.25 >= m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp)
	{
		if (PHASE::PHASE2 == m_eCurPhase && !m_pSkirt)
			m_bCollision = true;
		else
		{
			m_tObjectStatusDesc.fHp = (m_tObjectStatusDesc.fMaxHp * 0.25f);
			m_bCollision = false;
		}
	}
}

void CBeauvoir::Check_60PercentHP()
{
	if (0.6 >= m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp
		&& 0.25 < m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp)
	{
		if (m_b60PerOn)
			m_bCollision = true;
		else
		{
			m_tObjectStatusDesc.fHp = (m_tObjectStatusDesc.fMaxHp * 0.59f);
			m_bCollision = false;
		}
	}
}

void CBeauvoir::Check_40PercentHP()
{
	if (0.4 >= m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp
		&& 0.25 < m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp)
	{
		if (false == m_bHackOnce)
			m_bHackable = true;
	}
}

void CBeauvoir::Update_Parts()
{
	if (nullptr != m_pSkirt)
	{
		//_float4x4 SkirtFloat4x4;
		//
		//XMStoreFloat4x4(&SkirtFloat4x4, SkirtMatrix);
		//
		//_vector vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
		//_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		//_vector vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);
		//
		//memcpy(&SkirtFloat4x4.m[0], &vRight, sizeof(_vector));
		//memcpy(&SkirtFloat4x4.m[1], &vUp, sizeof(_vector));
		//memcpy(&SkirtFloat4x4.m[2], &vLook, sizeof(_vector));

		_matrix SkirtMatrix = m_pModel->Get_BoneMatrix(L"Skirt");
		m_pSkirt->Set_WorldMatrix(SkirtMatrix * m_pTransform->Get_WorldMatrix());


		if (!m_pSkirt->Get_IsEnd())
		{
			_matrix SkirtMatrix = m_pModel->Get_BoneMatrix(L"Skirt");
			m_pSkirt->Set_WorldMatrix(SkirtMatrix * m_pTransform->Get_WorldMatrix());
		}
		else
		{
			m_pSkirt->Set_State(OBJSTATE::DEAD);
			m_pSkirt = nullptr;
			Set_RimLightColor(_float3(0.2f, 0.f, 0.f));
		}

	}

	if (nullptr != m_pHair)
	{
		_matrix HairMatrix = m_pModel->Get_BoneMatrix(L"Hair") * m_pTransform->Get_WorldMatrix();
		_float4x4 HairFloat4x4;
		XMStoreFloat4x4(&HairFloat4x4, HairMatrix);

		_vector vPos = XMVectorZero();
		_vector vLook = XMVectorZero();
		_vector vRight = XMVectorZero();

		memcpy(&vPos, &HairFloat4x4.m[3], sizeof(_vector));

		memcpy(&vLook, &m_pTransform->Get_State(CTransform::STATE::LOOK), sizeof(_vector));
		vLook = XMVector3Normalize(vLook);

		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 5.f);
		vPos += vLook * -2.f;

		memcpy(&HairFloat4x4.m[3], &vPos, sizeof(_float4));

		m_pHair->Set_WorldMatrix(XMLoadFloat4x4(&HairFloat4x4));
	}
}

void CBeauvoir::Set_Skirt(CBeauvoir_Skirt * pSkirt)
{
	if (nullptr != pSkirt)
		m_pSkirt = pSkirt;
}

void CBeauvoir::Set_Hair(CBeauvoir_Hair * pHair)
{
	if (nullptr != pHair)
		m_pHair = pHair;
}

void CBeauvoir::Check_Distance()
{
	if (ANIM_CHANGE_BEAUVOIR_JUMP == m_pModel->Get_CurrentAnimation())
	{
		if (m_iRepeatCount >= 3)
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

			_float fDist = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

			if (fDist >= 10.f)
				m_bChangeJumpStop = true;

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CBeauvoir::Create_CrossAndroid(_double TimeDelta)
{
	if (false == m_bCreateCrossAndroid)
	{
		for (auto& pAndroid : m_vecCrossAndroid)
			pAndroid->Set_Start(true);

		m_vecCrossAndroid.clear();

		m_bCreateCrossAndroid = true;
	}
}

void CBeauvoir::Update_Effects(_float fTimeDelta)
{
	fSemiLaserDuration -= fTimeDelta;

	if (ANIM_ROTATE_LASER_DW == m_pModel->Get_CurrentAnimation())
	{
		if (m_pLaserEffect0)
		{
			if (CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER == dynamic_cast<CMeshEffect*>(m_pLaserEffect0)->Get_ID())
			{
				if (0 > fSemiLaserDuration)
				{
					Delete_Lasers();
					Create_RealLasers();
				}
			}
		}
		Update_LaserEffectsPosition(fTimeDelta);
	}
	Update_LargeSparkEffectTransform();


}

HRESULT CBeauvoir::Create_JumpBullet(_double TimeDelta)
{
	if (PATTERN::NAVI_BULLET == m_eCurPattern || PATTERN::RISING_BULLET == m_eCurPattern || PATTERN::JUMP_FLOOR == m_eCurPattern)
		return S_OK;

	if (ANIM_SPREADBULLET_DW == m_pModel->Get_CurrentAnimation())
	{
		m_CreateJumpBulletTime += TimeDelta;

		if (m_CreateJumpBulletTime >= m_CreateJumpBulletDelayTime)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_float fAngle = 0.f;

			if (m_bFastBullet)
				fAngle = 30.f;
			
			CEffectFactory* pFactory = CEffectFactory::Get_Instance();

			for (_uint i = 0; i < 20; ++i)
			{
				//CBeauvoir_Bullet::BULLET_TYPE eBullet = CBeauvoir_Bullet::BULLET_TYPE::JUMP;

				EFFECT_PACKET tPacket;
				ZeroMemory(&tPacket, sizeof(EFFECT_PACKET));
				tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
				tPacket.fMaxLifeTime = BEAUVOIR_JUMPBULLET_MAX_LIFETIME;
				tPacket.iNavIndex = m_pNavigation->Get_Index();
				if (false == m_bFastBullet)
				{
					tPacket.tEffectMovement.fSpeed = BEAUVOIR_BULLET_SPEED;
					tPacket.tEffectMovement.fJumpPower = BEAUVOIR_BULLET_JUMP;
					tPacket.tEffectMovement.iJumpCount = BEAUVOIR_BULLET_JUMP_COUNT + 1;
				}
				else
				{
					tPacket.tEffectMovement.fSpeed = BEAUVOIR_BULLET_FASTSPEED;
					tPacket.tEffectMovement.fJumpPower = BEAUVOIR_BULLET_POWERJUMP;
					tPacket.tEffectMovement.iJumpCount = BEAUVOIR_BULLET_POWERJUMP_COUNT + 1;
				}
				_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
				_float4x4 Rotate4x4;

				XMStoreFloat4x4(&Rotate4x4, RotateMatrix);
				_float4 v4Direction = *(_float4*)&Rotate4x4.m[2][0];
				tPacket.tEffectMovement.vDirection = { v4Direction.x, v4Direction.y, v4Direction.z };

				XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
				tPacket.tEffectMovement.vPosition.x += tPacket.tEffectMovement.vDirection.x;
				tPacket.tEffectMovement.vPosition.y += tPacket.tEffectMovement.vDirection.y;
				tPacket.tEffectMovement.vPosition.z += tPacket.tEffectMovement.vDirection.z;


				//tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET_BIG;
				//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet_Big.dat", &tPacket);
				tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
				pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);

				fAngle += 18.f;
			}

			RELEASE_INSTANCE(CGameInstance);
			m_CreateJumpBulletTime = 0.0;

			m_bFastBullet = !m_bFastBullet;
		}
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_FireMissile(_double TimeDelta)
{
	if (ANIM_FIRE_MISSILES == m_pModel->Get_CurrentAnimation()
		&& m_pModel->Get_PlayTimeRatio() <= 0.6f)
	{
		m_CreateFireMissileTime += TimeDelta;
		if (m_CreateFireMissileTime >= m_CreateFireMissileDelayTime)
		{
			_tchar pTag[MAX_PATH]{};

			_float fAngle = -51.f;		
			for (_uint i = 0; i < BEAUVOIR_MISSILE_NUM; ++i)
			{
				CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
				_float4x4 WorldFloat4x4;
				
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vPos;
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				_vector vRight;
				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				vRight = XMVector3Normalize(vRight);

				_vector vUp;
				memcpy(&vUp, &WorldFloat4x4.m[1], sizeof(_vector));
				vUp = XMVector3Normalize(vUp);

				_vector vLook;
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				vLook = XMVector3Normalize(vLook);

				if (2 == i)
				{
					vPos += (XMVector3Normalize(vLook) * 2.f);
					memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

					vLook *= -1.f;

					memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[1], &vLook, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[2], &vUp, sizeof(_vector));

					WorldMatrix = XMLoadFloat4x4(&WorldFloat4x4);
				}
				else if (3 == i)
				{
					vLook *= -1.f;

					memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[1], &vLook, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[2], &vUp, sizeof(_vector));
					vRight *= -1.f;

					vPos += vRight * 2.f;
					memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

					WorldMatrix = XMLoadFloat4x4(&WorldFloat4x4);
				}
				else if (4 == i)
				{
					vLook *= -1.f;

					memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[1], &vLook, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[2], &vUp, sizeof(_vector));

					vRight = (XMVector3Normalize(vRight)) * 2.f;

					vPos += vRight;
					memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

					WorldMatrix = XMLoadFloat4x4(&WorldFloat4x4);
				}
				else
				{
					swprintf_s(pTag, L"FireMissileBone%d", i);
					WorldMatrix = m_pModel->Get_BoneMatrix(pTag) * WorldMatrix;

					XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

					vLook *= -1.f;

					memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[1], &vLook, sizeof(_vector));
					memcpy(&WorldFloat4x4.m[2], &vUp, sizeof(_vector));

					WorldMatrix = XMLoadFloat4x4(&WorldFloat4x4);
				}

				CBeauvoir_FireMissile* pMissile = nullptr;
				FIRE_MISSILE_DESC tDesc;
				tDesc.vDirection = vLook;
				vLook = XMVectorSetY(vLook, 1.f);
				vLook = XMVector3Normalize(vLook);
				tDesc.vDirection = vLook;
				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 2.f);
				tDesc.vPosition = vPos;
				pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Beauvoir_FireMissile", L"MonsterAttack", (CGameObject**)&pMissile, &tDesc);
				
				if (nullptr == pMissile)
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
				if (nullptr == pTransform)
				{
					RELEASE_INSTANCE(CGameInstance);
					return E_FAIL;
				}

				pMissile->Set_TargetPos(pTransform->Get_State(CTransform::STATE::POSITION));
				RELEASE_INSTANCE(CGameInstance);
			}

			m_CreateFireMissileTime = 0;
		}

	}
	return S_OK;
}

HRESULT CBeauvoir::Create_LaserRing(_double TimeDelta)
{
	if (PATTERN::LASER_RING == m_eCurPattern)
	{
		if (ANIM_ROTATE_LASER_DW == m_pModel->Get_CurrentAnimation())
		{
		}
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_RisingBullet(_double TimeDelta)
{
	if (PATTERN::RISING_BULLET == m_eCurPattern)
	{
		if (ANIM_SPREADBULLET_DW == m_pModel->Get_CurrentAnimation())
		{
			// if (2 >= m_iRepeatCount)
			m_pTransform->Rotation_Axis(TimeDelta * -0.25, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		
			m_CreateRisingBulletTime += TimeDelta;

			if (m_CreateRisingBulletTime >= m_CreateRisingBulletDelayTime)
			{
				CEffectFactory* pFactory = CEffectFactory::Get_Instance();
				CGameInstance* pGameInstance = CGameInstance::Get_Instance();
				const _matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

				for (_uint i = 0; i < 8; ++i)
				{
					_float4x4 WorldFloat4x4;
					XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

					_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
					_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP));
					_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
					_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

					_vector vRepeatRight = XMVectorZero();
					_vector vRepeatUp = XMVectorZero();
					_vector vRepeatLook = XMVectorZero();
					_vector vPosition = vPos;

					if (0 == i)
					{
						vPos += vLook;
						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
					}
					else if (1 == i)
					{
						vPos += vRight;

						// add
						vLook *= -1.f;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vRight, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vLook, sizeof(_vector));
					}
					else if (2 == i)
					{
						vPos -= vLook;

						vLook *= -1.f;
						vRight *= -1.f;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vLook, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
					}
					else if (3 == i)
					{
						vPos -= vRight;

						vRight *= -1.f;
						vLook *= -1.f;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vRight, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vLook, sizeof(_vector));
					}
					else if (4 == i)
					{
						_vector vDiagonalLook = XMVector3Normalize(vLook + vRight);
						_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLook));

						vPos += vDiagonalLook;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vDiagonalLook, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vTempRight, sizeof(_vector));
					}
					else if (5 == i)
					{
						vRight *= -1.f;

						_vector vDiagonalLook = XMVector3Normalize(vLook + vRight);
						_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLook));

						vPos += vDiagonalLook;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vDiagonalLook, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vTempRight, sizeof(_vector));
					}
					else if (6 == i)
					{
						vRight *= -1.f;
						vLook *= -1.f;

						_vector vDiagonalLook = XMVector3Normalize(vLook + vRight);
						_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLook));

						vPos += vDiagonalLook;

						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vDiagonalLook, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vTempRight, sizeof(_vector));
					}
					else if (7 == i)
					{
						vLook *= -1.f;

						_vector vDiagonalLook = XMVector3Normalize(vLook + vRight);
						_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLook));

						vPos += vDiagonalLook;
						memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[2], &vDiagonalLook, sizeof(_vector));
						memcpy(&WorldFloat4x4.m[0], &vTempRight, sizeof(_vector));
					}


					vLook = XMVectorSetY(vLook, XMVectorGetY(vLook) + 0.2f);	// 0.1f is Offset to go up

					
					_float3 vEffectDir = _float3(0.f, 0.f, 0.f);

					memcpy(&vEffectDir, &WorldFloat4x4.m[2], sizeof(_float3));
					vEffectDir.y += 0.12f;
					EFFECT_PACKET tPacket;
					tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
					tPacket.fMaxLifeTime = BEAUVOIR_RISINGBULLET_MAX_LIFETIME;
					tPacket.tEffectMovement.fSpeed = BEAUVOIR_RISINGBULLET_SPEED;
					tPacket.tEffectMovement.fJumpPower = 0.f;
					tPacket.tEffectMovement.iJumpCount = 0;
					tPacket.tEffectMovement.vDirection = vEffectDir;
					XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPos);
					tPacket.tEffectMovement.vPosition.x += tPacket.tEffectMovement.vDirection.x;
					tPacket.tEffectMovement.vPosition.y += tPacket.tEffectMovement.vDirection.y;
					tPacket.tEffectMovement.vPosition.z += tPacket.tEffectMovement.vDirection.z;
					tPacket.iNavIndex = m_pNavigation->Get_Index();

					// Offset to prevent delete from collide with Navigation Mesh
					tPacket.tEffectMovement.vPosition.y += 0.5f;

					//tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET_BIG;
					//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet_Big.dat", &tPacket);
					tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
					pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);



					//pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"Prototype_Beauvoir_RisingBullet", L"MonsterAttack", (void*)&WorldFloat4x4);
				}
				
				m_CreateRisingBulletTime = 0.0;
			}
		}
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_NaviBullet(_double TimeDelta)
{
	if (PATTERN::NAVI_BULLET == m_eCurPattern)
	{
		if (ANIM_SPREADBULLET_DW == m_pModel->Get_CurrentAnimation())
		{
			m_CreateNaviBulletTime += TimeDelta;

			_float fAngle = 0.f;
			if (m_CreateNaviBulletTime >= m_CreateNaviBulletDelayTime)
			{
				//for (_uint i = 0; i < 16; ++i)
				for (_uint i = 0; i < 16; ++i)
				{
					CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
					//CBeauvoir_NaviBullet* pBullet = nullptr;
					//pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Beauvoir_NaviBullet", L"MonsterAttack", (CGameObject**)&pBullet);
					//
					//if (nullptr == pBullet)
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return E_FAIL;
					//}
					//
					//CTransform* pTransform = (CTransform*)pBullet->Get_Component(L"Com_Transform");
					//if (nullptr == pTransform)
					//{
					//	RELEASE_INSTANCE(CGameInstance);
					//	return E_FAIL;
					//}
					//
					//_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
					//
					//pTransform->Set_WorldMatrix(RotateMatrix);
					//
					//_vector vLook = XMVector3Normalize(pTransform->Get_State(CTransform::STATE::LOOK));
					//pTransform->Set_State(CTransform::STATE::POSITION, m_pTransform->Get_State(CTransform::STATE::POSITION) + vLook);
					//
					//fAngle += (22.5f + m_fGapAngle);

					CEffectFactory* pFactory = CEffectFactory::Get_Instance();
					EFFECT_PACKET tPacket;
					tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
					tPacket.fMaxLifeTime = BEAUVOIR_NAVIBULLET_MAX_LIFETIME;
					tPacket.tEffectMovement.fSpeed = BEAUVOIR_NAVIBULLET_SPEED;
					tPacket.tEffectMovement.fJumpPower = 0.f;
					tPacket.tEffectMovement.iJumpCount = 0;
					tPacket.iNavIndex = m_pNavigation->Get_Index();
					tPacket.fColliderRadius = 100.f;

					_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
					_float4x4 Rotate4x4;

					XMStoreFloat4x4(&Rotate4x4, RotateMatrix);
					_float4 v4Direction = *(_float4*)&Rotate4x4.m[2][0];
					// adjust direction
					if (i % 2 == 0)
					{
						v4Direction.y += 0.3f;
					}

					tPacket.tEffectMovement.vDirection = { v4Direction.x, v4Direction.y, v4Direction.z };



					XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
					// Offset to lift up bullet
					tPacket.tEffectMovement.vPosition.y += 1.f;

					//tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET_BIG;
					//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet_Big.dat", &tPacket);
					//tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
					//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);
					tPacket.ulID = CHECKSUM_EFFECT_ELECTRIC_DONUT;
					pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut.dat", &tPacket);

					//fAngle += (22.5f + m_fGapAngle);
					fAngle += (45.f + m_fGapAngle);

					RELEASE_INSTANCE(CGameInstance);
				}

				m_CreateNaviBulletTime = 0.0;
				//m_fGapAngle += 25.f;
				m_fGapAngle += 50.f;
			}
		}
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_JumpSmoke()
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
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Jump_Smoke.dat", &tPacket);
	return S_OK;
}

HRESULT CBeauvoir::Create_JumpImpact()
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	pSoundMgr->StopSound(CSoundMgr::BOSS_EFFECT);
	pSoundMgr->PlaySound(TEXT("Beauvoir_Crash.wav"), CSoundMgr::BOSS_EFFECT);

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

	//tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	//tPacket.fMaxLifeTime = -1.f;
	//XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	//tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_PLANE;
	//pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Plane.dat", &tPacket);
	//if (!pObj)
	//	assert(FALSE);

	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_SMOKE;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Smoke.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CDecalCube::DECALCUBE tCube;
	tCube.bContinue = false;
	tCube.dDuration = 1.0;
	XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tCube.vScale = _float4(17.5f, 1.5f, 17.5f, 0.f);
	tCube.iImageIndex = 0;
	tCube.eType = CDecalCube::TYPE::BURNINGFIELD;
	tCube.iIsGlow = 1;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	tCube.vScale = _float4(1.f, 1.f, 1.f, 0.f);
	tCube.eType = CDecalCube::TYPE::FOGRING;
	tCube.dDuration = 0.5;
	tCube.fAddScale = 10.5f;
	tCube.iImageIndex = 1;
	tCube.iIsGlow = 0;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeauvoir::Create_DeathEffect()
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

	tPacket.tEffectMovement.vPosition.x += 4;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

void CBeauvoir::Set_Parts_ChangeState()
{
	if (nullptr != m_pSkirt)
	{
		m_pSkirt->Set_DissolveTrue();
	}
}

void CBeauvoir::Check_Hp()
{
	if (false == m_b60PerOn)
	{
		if (0.6 >= (m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			m_eCurPattern = PATTERN::MOVE_STAGE;
			m_iAnimation = ANIM_BEAUVOIR_IDLE;

			m_LookLerpTime = 0.0;
		}
	}

	if (0.25 >= (m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
	{
		m_eCurPhase = PHASE::PHASE2;
		m_eCurPattern = PATTERN::CHANGE_JUMP;
		m_bContinue = false;
		m_LookLerpTime = 0.0;

		Set_Parts_ChangeState();
	}
}

void CBeauvoir::Check_Dead()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		if (ANIM_CHANGE_IDLE == iCurAnimIndex)
		{
			m_pModel->Initialize_Time();
			m_iAnimation = ANIM_CHANGE_BEAUVOIR_CREATE_RING;
			m_pModel->Set_NoneBlend();

			m_eCurPattern = PATTERN::DEAD;
			m_bContinue = false;
		}
		else if (ANIM_CHANGE_BEAUVOIR_CREATE_RING == iCurAnimIndex)
		{
			m_bDeadStop = true;

			// Add .Dissolve
			if (BEAUVOIR_DEADMAXRATIO <= m_pModel->Get_PlayTimeRatio())
				m_eState = OBJSTATE::DEAD;
		}
	}
}

void CBeauvoir::CrossAndroid_Pulling()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_uint iSize = m_pNavigation->Get_Size();
	for (_uint i = 0; i < 10; ++i)
	{
		_uint iShowUpIndex = rand() % CREATABLE_INDEX;

		_vector vPos = XMLoadFloat4(&m_pNavigation->Get_CenterPosition(iShowUpIndex));

		CCrossAndroid* pAndroid = nullptr;
		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_NextLevelID(), PROTO_KEY_CROSSANDROID, L"Monster", (CGameObject**)&pAndroid, &vPos)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		if (nullptr == pAndroid)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		m_vecCrossAndroid.push_back(pAndroid);
	}

	m_iNumCrossAndroid = CROSSANDROID_MAX;

	RELEASE_INSTANCE(CGameInstance);
}

void CBeauvoir::Update_HitType()
{
	if (m_pModel)
	{
		if (!m_pModel->Get_Blend())
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			if (ANIM_MELEE_ROTATE_DW == iCurAnimIndex || ANIM_CHANGE_BEAUVOIR_JUMP == iCurAnimIndex || ANIM_CHANGE_BEAUVOIR_RUN_DW == iCurAnimIndex)
			{
				m_iTargetAnimation = BEAUVOIR_ATTACKBOX_VALUE + (rand() % 1000);
				m_eHitType = HIT::MULTI;
				m_HitDelayTime = 0.5;
			}
			else
				m_eHitType = HIT::NONE;
		}
	}
}

void CBeauvoir::Compute_DOF_Parameters()
{
	
	m_vDOFParams.y = Get_CameraDistance() + 5.5f;
	m_vDOFParams.z = m_vDOFParams.y * 7.f;
	m_vDOFParams.x = 0.f;
}

HRESULT CBeauvoir::Create_LargeSparkEffect()
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
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));	// 360 / 2
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLargeSparkEffect)
	{
		m_pLargeSparkEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLargeSparkEffect);
		m_pLargeSparkEffect = nullptr;
	}

	m_pLargeSparkEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Large_Spark.dat", &tPacket);
	Safe_AddRef(m_pLargeSparkEffect);

	return S_OK;
}

HRESULT CBeauvoir::Delete_LargeSparkEffect()
{
	if (m_pLargeSparkEffect)
	{
		m_pLargeSparkEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLargeSparkEffect);
		m_pLargeSparkEffect = nullptr;
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_EndLargeSparkEffect()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));	// 360 / 2
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	memcpy(&tPacket.tEffectMovement.vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	memcpy(&tPacket.tEffectMovement.vDirection, &(Spark4x4.m[2]), sizeof(_float3));

	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_TAILSWEEP_SPARK;
	CGameObject* pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Rollin_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	return S_OK;
}

HRESULT CBeauvoir::Update_LargeSparkEffectTransform()
{
	if (m_pLargeSparkEffect)
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
		tPacket.tEffectMovement.vPosition;
		//tPacket.iNavIndex = m_pNavigation->Get_Index();

		_matrix SparkMatrix = m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
		_float4x4 Spark4x4;
		XMStoreFloat4x4(&Spark4x4, SparkMatrix);
		_float4 vPosition;
		memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
		_float4 vLook;
		memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

		tPacket.tEffectMovement.vPosition = vPosition;
		tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLargeSparkEffect->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, m_pTransform->Get_State(CTransform::STATE::POSITION));
		pTransform->LookAt_Direction(-1 * XMLoadFloat3(&m_vToPlayerDirection));
	}
	return S_OK;
}

HRESULT CBeauvoir::Create_Donut_Up()
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
	tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP0;
	_vector vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	tPacket.iNavIndex = m_pNavigation->Get_Index();

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vecPos);
	tPacket.tEffectMovement.vPosition = vPosition;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up0.dat", &tPacket);
	
	return S_OK;
}


HRESULT CBeauvoir::Create_RingWave()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = BEAUVOIR_RINGWAVE_SPEED;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_RING_WAVE;
	tPacket.iAtt = 20;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_vector vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_float4 vPosition;
	XMStoreFloat4(&vPosition, vecPos);
	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vPosition.y += 0.05f;

	//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_RingWave.dat", &tPacket);
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_RingWave.dat", &tPacket, L"Beauvoir_RingWave");

	return S_OK;
}

HRESULT CBeauvoir::Check_CreateRingWave_Tetrad()
{
	_double TimeRatio = m_pModel->Get_PlayTimeRatio();
	if (0 == m_iCreateRingWaveTetard)
	{
		if (BEAUVOIR_CREATE_RINGWAVE_FIRST_RATIO <= TimeRatio)
		{
			if (FAILED(Create_RingWave()))
				return E_FAIL;

			++m_iCreateRingWaveTetard;
		}
	}
	else if (1 == m_iCreateRingWaveTetard)
	{
		if (BEAUVOIR_CREATE_RINGWAVE_SECOND_RATIO <= TimeRatio)
		{
			if (FAILED(Create_RingWave()))
				return E_FAIL;

			++m_iCreateRingWaveTetard;
		}
	}
	else if (2 == m_iCreateRingWaveTetard)
	{
		if (BEAUVOIR_CREATE_RINGWAVE_THIRD_RATIO <= TimeRatio)
		{
			if (FAILED(Create_RingWave()))
				return E_FAIL;

			++m_iCreateRingWaveTetard;
		}
	}
	else if (3 == m_iCreateRingWaveTetard)
	{
		if (BEAUVOIR_CREATE_RINGWAVE_FOURTH_RATIO <= TimeRatio)
		{
			if (FAILED(Create_RingWave()))
				return E_FAIL;

			++m_iCreateRingWaveTetard;
		}
	}

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLasers()
{
	fSemiLaserDuration = BEAUVOIR_SEMI_LASER_DURATION;
	_float fTempAngle = m_fLaserAngle;
	_float fOffsetAngle = 30.f;
	if (FAILED(Create_SemiLaser0(m_fLaserAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser1(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser2(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser3(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser4(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser5(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser6(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser7(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser8(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser9(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser10(fTempAngle))) return E_FAIL;
	fTempAngle += fOffsetAngle;
	if (fTempAngle > 360.f) { fTempAngle -= 360.f; }
	if (FAILED(Create_SemiLaser11(fTempAngle))) return E_FAIL;

	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
	CSoundMgr::Get_Instance()->PlaySound(L"Beauvoir_LaserCharge.wav", CSoundMgr::CHANNELID::BOSS_EFFECT);
	return S_OK;
}

_bool CBeauvoir::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CBeauvoir::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		if (m_eCurPhase == PHASE::PHASE2)
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

HRESULT CBeauvoir::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dDeltaTime;

		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;
		desc.eID = CUI_Dialogue_EngageMessage::BOSSID::BEAUVOIR;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MONSTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MONSTER_HPBAR, &desc));

		RELEASE_INSTANCE(CGameInstance);

		if (m_dCallHPBarUITimeDuration < 0)
		{
			m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
		}
	}
	if (m_eCurPhase != CBeauvoir::PHASE::PHASE_APPEARANCE)
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

void CBeauvoir::RimLighting(_double dTimeDelta)
{
	if (m_eCurPhase != PHASE::PHASE2)
	{
		if (m_bHackable && !m_bHackFinished)
		{
			_float4 vMaxColor = CProportionConverter()(_DARKORCHID, 45.f);

			_float4 vColor = CProportionConverter()(_DARKORCHID, 0.35f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
		else if (m_bHackable && m_bHackFinished)
		{
			_float4 vMaxColor = CProportionConverter()(_NOCOLOR, 100.f);

			_float4 vColor = CProportionConverter()(_DARKORCHID, 0.35f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) - XMLoadFloat4(&vColor));
			if (m_vRimColor.x <= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y <= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z <= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
	}
	else if (m_eCurPhase == PHASE::PHASE2)
	{
		if (0.2 > (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp) && 
			0.15 <= (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			_float4 vMaxColor = CProportionConverter()(_SALMON, 20.f);

			_float4 vColor = CProportionConverter()(_SALMON, 0.5f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
		else if (0.15 > (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp) &&
			0.1 <= (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			_float4 vMaxColor = CProportionConverter()(_SALMON, 40.f);

			_float4 vColor = CProportionConverter()(_SALMON, 0.5f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
		else if (0.1 > (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp) &&
			0.05 <= (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			_float4 vMaxColor = CProportionConverter()(_SALMON, 60.f);

			_float4 vColor = CProportionConverter()(_SALMON, 0.5f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
		else if (0.05 > (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp) &&
			0.00 < (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			_float4 vMaxColor = CProportionConverter()(_SALMON, 80.f);

			_float4 vColor = CProportionConverter()(_SALMON, 0.5f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
		else if (0.0 >= (_double)(m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp))
		{
			_float4 vMaxColor = CProportionConverter()(_SALMON, 100.f);

			_float4 vColor = CProportionConverter()(_SALMON, 0.5f);

			XMStoreFloat4(&m_vRimColor, XMLoadFloat4(&m_vRimColor) + XMLoadFloat4(&vColor));
			if (m_vRimColor.x >= vMaxColor.x)
				m_vRimColor.x = vMaxColor.x;
			if (m_vRimColor.y >= vMaxColor.y)
				m_vRimColor.y = vMaxColor.y;
			if (m_vRimColor.z >= vMaxColor.z)
				m_vRimColor.z = vMaxColor.z;
		}
	}
}

HRESULT CBeauvoir::Create_SemiLaser0(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect0)
	{
		m_pLaserEffect0->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect0);
		m_pLaserEffect0 = nullptr;
	}

	m_pLaserEffect0 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect0);

	

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser1(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect1)
	{
		m_pLaserEffect1->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect1);
		m_pLaserEffect1 = nullptr;
	}

	m_pLaserEffect1 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect1);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser2(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect2)
	{
		m_pLaserEffect2->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect2);
		m_pLaserEffect2 = nullptr;
	}

	m_pLaserEffect2 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect2);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser3(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect3)
	{
		m_pLaserEffect3->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect3);
		m_pLaserEffect3 = nullptr;
	}

	m_pLaserEffect3 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect3);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser4(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect4)
	{
		m_pLaserEffect4->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect4);
		m_pLaserEffect4 = nullptr;
	}

	m_pLaserEffect4 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect4);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser5(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect5)
	{
		m_pLaserEffect5->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect5);
		m_pLaserEffect5 = nullptr;
	}

	m_pLaserEffect5 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect5);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser6(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect6)
	{
		m_pLaserEffect6->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect6);
		m_pLaserEffect6 = nullptr;
	}

	m_pLaserEffect6 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect6);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser7(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect7)
	{
		m_pLaserEffect7->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect7);
		m_pLaserEffect7 = nullptr;
	}

	m_pLaserEffect7 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect7);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser8(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect8)
	{
		m_pLaserEffect8->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect8);
		m_pLaserEffect8 = nullptr;
	}

	m_pLaserEffect8 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect8);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser9(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect9)
	{
		m_pLaserEffect9->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect9);
		m_pLaserEffect9 = nullptr;
	}

	m_pLaserEffect9 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect9);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser10(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect10)
	{
		m_pLaserEffect10->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect10);
		m_pLaserEffect10 = nullptr;
	}

	m_pLaserEffect10 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect10);

	return S_OK;
}

HRESULT CBeauvoir::Create_SemiLaser11(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_SEMI_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect11)
	{
		m_pLaserEffect11->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect11);
		m_pLaserEffect11 = nullptr;
	}

	m_pLaserEffect11 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser0.dat", &tPacket);
	Safe_AddRef(m_pLaserEffect11);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLasers()
{
	_float tempAngle = m_fLaserAngle;
	_float fOffsetAngle = 30.f;
	if (FAILED(Create_RealLaser0(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser1(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser2(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser3(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser4(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser5(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser6(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser7(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser8(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser9(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser10(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Create_RealLaser11(tempAngle))) return E_FAIL;
	
	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
	CSoundMgr::Get_Instance()->PlaySound(L"Beauvoir_Laser.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser0(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect0)
	{
		m_pLaserEffect0->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect0);
		m_pLaserEffect0 = nullptr;
	}

	m_pLaserEffect0 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect0);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser1(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect1)
	{
		m_pLaserEffect1->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect1);
		m_pLaserEffect1 = nullptr;
	}

	m_pLaserEffect1 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect1);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser2(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect2)
	{
		m_pLaserEffect2->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect2);
		m_pLaserEffect2 = nullptr;
	}

	m_pLaserEffect2 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect2);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser3(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect3)
	{
		m_pLaserEffect3->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect3);
		m_pLaserEffect3 = nullptr;
	}

	m_pLaserEffect3 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect3);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser4(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect4)
	{
		m_pLaserEffect4->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect4);
		m_pLaserEffect4 = nullptr;
	}

	m_pLaserEffect4 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect4);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser5(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect5)
	{
		m_pLaserEffect5->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect5);
		m_pLaserEffect5 = nullptr;
	}

	m_pLaserEffect5 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect5);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser6(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect6)
	{
		m_pLaserEffect6->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect6);
		m_pLaserEffect6 = nullptr;
	}

	m_pLaserEffect6 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect6);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser7(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect7)
	{
		m_pLaserEffect7->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect7);
		m_pLaserEffect7 = nullptr;
	}

	m_pLaserEffect7 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect7);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser8(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect8)
	{
		m_pLaserEffect8->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect8);
		m_pLaserEffect8 = nullptr;
	}

	m_pLaserEffect8 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect8);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser9(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect9)
	{
		m_pLaserEffect9->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect9);
		m_pLaserEffect9 = nullptr;
	}

	m_pLaserEffect9 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect9);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser10(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect10)
	{
		m_pLaserEffect10->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect10);
		m_pLaserEffect10 = nullptr;
	}

	m_pLaserEffect10 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect10);

	return S_OK;
}

HRESULT CBeauvoir::Create_RealLaser11(_float fAngle)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_REAL_LASER;

	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	tPacket.tEffectMovement.vPosition;
	tPacket.iNavIndex = m_pNavigation->Get_Index();
	tPacket.iAtt = 20;

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_float4 vPosition;
	memcpy(&vPosition, &(Spark4x4.m[3]), sizeof(_float4));
	_float4 vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_float4));

	tPacket.tEffectMovement.vPosition = vPosition;
	tPacket.tEffectMovement.vDirection = { vLook.x, vLook.y, vLook.z };

	if (m_pLaserEffect11)
	{
		m_pLaserEffect11->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect11);
		m_pLaserEffect11 = nullptr;
	}

	m_pLaserEffect11 = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Laser1.dat", &tPacket, L"MonsterRayAttack");
	Safe_AddRef(m_pLaserEffect11);

	return S_OK;
}

HRESULT CBeauvoir::Delete_Lasers()
{
	if (m_pLaserEffect0)
	{
		m_pLaserEffect0->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect0);
		m_pLaserEffect0 = nullptr;
	}

	if (m_pLaserEffect1)
	{
		m_pLaserEffect1->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect1);
		m_pLaserEffect1 = nullptr;
	}

	if (m_pLaserEffect2)
	{
		m_pLaserEffect2->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect2);
		m_pLaserEffect2 = nullptr;
	}

	if (m_pLaserEffect3)
	{
		m_pLaserEffect3->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect3);
		m_pLaserEffect3 = nullptr;
	}

	if (m_pLaserEffect4)
	{
		m_pLaserEffect4->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect4);
		m_pLaserEffect4 = nullptr;
	}

	if (m_pLaserEffect5)
	{
		m_pLaserEffect5->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect5);
		m_pLaserEffect5 = nullptr;
	}

	if (m_pLaserEffect6)
	{
		m_pLaserEffect6->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect6);
		m_pLaserEffect6 = nullptr;
	}

	if (m_pLaserEffect7)
	{
		m_pLaserEffect7->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect7);
		m_pLaserEffect7 = nullptr;
	}

	if (m_pLaserEffect8)
	{
		m_pLaserEffect8->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect8);
		m_pLaserEffect8 = nullptr;
	}

	if (m_pLaserEffect9)
	{
		m_pLaserEffect9->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect9);
		m_pLaserEffect9 = nullptr;
	}

	if (m_pLaserEffect10)
	{
		m_pLaserEffect10->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect10);
		m_pLaserEffect10 = nullptr;
	}

	if (m_pLaserEffect11)
	{
		m_pLaserEffect11->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLaserEffect11);
		m_pLaserEffect11 = nullptr;
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffectsPosition(_float fTimeDleta)
{
	m_fLaserAngle -= 110 * fTimeDleta;
	if (m_fLaserAngle < 0.f)
		m_fLaserAngle += 360.f;

	_float tempAngle = m_fLaserAngle;
	_float fOffsetAngle = 30.f;
	if (FAILED(Update_LaserEffect0Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) 	{
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect1Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect2Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect3Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect4Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect5Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect6Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect7Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect8Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect9Position(tempAngle))) return E_FAIL;
	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect10Position(tempAngle))) return E_FAIL;

	tempAngle += fOffsetAngle;
	if (tempAngle > 360.f) {
		tempAngle -= 360.f;
	}
	if (FAILED(Update_LaserEffect11Position(tempAngle))) return E_FAIL;
	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect0Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect0)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect0->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect1Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect1)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect1->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect2Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect2)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect2->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect3Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect3)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect3->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect4Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect4)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect4->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect5Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect5)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect5->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect6Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect6)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect6->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect7Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect7)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect7->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect8Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect8)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect8->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect9Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect9)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect9->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect10Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect10)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect10->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

HRESULT CBeauvoir::Update_LaserEffect11Position(_float fAngle)
{
	_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
	_matrix SparkMatrix = RotateMatrix * m_pModel->Get_BoneMatrix(L"bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 Spark4x4;
	XMStoreFloat4x4(&Spark4x4, SparkMatrix);
	_vector vLook;
	memcpy(&vLook, &(Spark4x4.m[2]), sizeof(_vector));


	if (m_pLaserEffect11)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLaserEffect11->Get_Component(L"Com_Transform"));
		pTransform->LookAt_Direction(vLook);
	}

	return S_OK;
}

void CBeauvoir::Notify(void * pMessage)
{
}

_int CBeauvoir::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir::Create_SonicEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 10.f; // turn off it when i want;

	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition.y += 7.f;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Sonic.dat", &tPacket);

	return S_OK;
}

HRESULT CBeauvoir::SetUp_Components()
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
	if (FAILED(__super::Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_MODEL_BEAUVOIR, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Head", "bone1134");
	m_pModel->Add_RefNode(L"Left", "bone1797");
	m_pModel->Add_RefNode(L"Right", "bone1541");

	m_pModel->Add_RefNode(L"Skirt", "bone2305");
	m_pModel->Add_RefNode(L"Hair", "bone001");

	m_pModel->Add_RefNode(L"FireMissileBone0", "bone1121"); 
	m_pModel->Add_RefNode(L"FireMissileBone1", "bone1025"); 
	m_pModel->Add_RefNode(L"FireMissileBone5", "bone1153"); 
	m_pModel->Add_RefNode(L"FireMissileBone6", "bone1058"); 
	m_pModel->Add_RefNode(L"FireMissileBone7", "bone1067"); 

	/* For.Com_Navigation*/
	_int iCurrentIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;


	CCollider::tagColliderDesc tColliderDesc;
	tColliderDesc.vPivot = _float3(0.f, 4.f, 0.f);
	tColliderDesc.vScale = _float3(2.5f, 4.f, 2.5f);
	tColliderDesc.fRadius = 2.5f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = _float3(0.f, 0.f, 0.f);
	tColliderDesc.fRadius = BEAUVOIR_ATTACKBOX_COLLIDER_RADIUS;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CBeauvoir_Skirt* pSkirt = nullptr;
	if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_BEAUVOIR_SKIRT, L"Skirt", (CGameObject**)&pSkirt)))
		return E_FAIL;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	if (nullptr == pSkirt)
		return E_FAIL;





	Set_Skirt(pSkirt);

	return S_OK;
}

HRESULT CBeauvoir::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;


	return S_OK;
}

CBeauvoir * CBeauvoir::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir* pInstance = new CBeauvoir(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CBeauvoir::Clone(void * pArg)
{
	CBeauvoir* pInstance = new CBeauvoir(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBeauvoir::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);


	Safe_Release(m_pLargeSparkEffect);

	Safe_Release(m_pLaserEffect0);
	Safe_Release(m_pLaserEffect1);
	Safe_Release(m_pLaserEffect2);
	Safe_Release(m_pLaserEffect3);
	Safe_Release(m_pLaserEffect4);
	Safe_Release(m_pLaserEffect5);
	Safe_Release(m_pLaserEffect6);
	Safe_Release(m_pLaserEffect7);
	Safe_Release(m_pLaserEffect8);
	Safe_Release(m_pLaserEffect9);
	Safe_Release(m_pLaserEffect10);
	Safe_Release(m_pLaserEffect11);
}
