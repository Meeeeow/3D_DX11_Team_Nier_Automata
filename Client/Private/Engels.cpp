#include "stdafx.h"
#include "..\Public\Engels.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "EffectFactory.h"
#include "EngelsFireMissile.h"
#include "MeshEffect.h"
#include "VendingMachine.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "DecalCube.h"
#include "CameraMgr.h"
#include "SoundMgr.h"

const _double CEngels::ROTATEPOWER_MAX = 1.0;

CEngels::CEngels(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CEngels::CEngels(const CEngels & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CEngels::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEngels::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));

	m_iAnimation = ANIM_ENGELS_IDLE;
	m_bContinue = true;

	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	m_fHitRecovery = ENGELS_HIT_RECOVERY;

	m_tObjectStatusDesc.fHp = ENGELS_MAX_LIFE;
	m_tObjectStatusDesc.fMaxHp = ENGELS_MAX_LIFE;
	m_tObjectStatusDesc.iAtt = ENGELS_ATTACK_DAMAGE;

	XMStoreFloat3(&m_vFirstLook, XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)));

	static _uint iCPod_InstanceID = OriginID_Engels;
	m_iObjID = iCPod_InstanceID++;

	m_bCollision = true;


	ZeroMemory(m_arrRimLightColor, sizeof(_float3) * static_cast<_int>(MESH_CONTAINER::COUNT));
	ZeroMemory(m_arrGlow, sizeof(_int) * static_cast<_int>(MESH_CONTAINER::COUNT));
	

	return S_OK;
}

_int CEngels::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
	{
		//CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		//CVendingMachine* pInstance = dynamic_cast<CVendingMachine*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ROBOTGENERAL, L"Environment"));
		//if (nullptr == pInstance)
		//	return -1;
		//else
		//	pInstance->Set_AlphaTest();

		//CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

		//tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
		//tDesc.eObjState = OBJSTATE::ENABLE;
		//tDesc.iObjID = pInstance->Get_ObjID();

		//XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pInstance->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

		//pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

		//CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

		//tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::ENGELS;
		//tObjectiveDesc.iEventCount = 1;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));

		return (_int)m_eState;
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	ZeroMemory(&m_arrRimLightColor, sizeof(_float3) * static_cast<_int>(MESH_CONTAINER::COUNT));

	const _float fTimeDelta = static_cast<_float>(dTimeDelta);


	Update_TimeDelta(dTimeDelta);

	LerpLookToFirstLook(dTimeDelta);
	Check_Pattern(dTimeDelta);
	Create_FireMissile(dTimeDelta);
	Create_MainBullet(dTimeDelta);
	Create_RandomBullet(dTimeDelta);

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	m_pModel->Synchronize_Animation(m_pTransform);

	Update_CollisionTime(dTimeDelta);

	Update_UI(dTimeDelta);
	Update_HitType();
	Update_Collider();
	Update_Effects(dTimeDelta);
	Update_NormalAttack(dTimeDelta);

	return (_int)m_eState;
}

_int CEngels::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	Update_TimeDelta(dTimeDelta);

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

HRESULT CEngels::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	_float3 vRimLightColor = _float3(0.f, 0.f, 0.f);
	_int	iIsGlow = 0;
	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		vRimLightColor = _float3(0.f, 0.f, 0.f);
		iIsGlow = 0;

		switch (static_cast<MESH_CONTAINER>(i))
		{
		case MESH_CONTAINER::CHAINSAW:
		case MESH_CONTAINER::HEAD:
		case MESH_CONTAINER::ARM:
			vRimLightColor = m_arrRimLightColor[i];
			iIsGlow = m_arrGlow[i];
			break;
		default:
			break;
		}

		if (m_dHitTextureTime > 0.0)
		{
			_int iIsEffectTexture = 1;
			if (FAILED(m_pModel->SetUp_RawValue("g_iIsEffectTexture", &iIsEffectTexture, sizeof(_int))))
				return E_FAIL;
			if (FAILED(m_pModel->SetUp_Texture("g_texEffect", m_pHitTexture->Get_SRV(m_iHitTextureIndex))))
				return E_FAIL;
		}
		else
		{
			_int iIsEffectTexture = 0;
			if (FAILED(m_pModel->SetUp_RawValue("g_iIsEffectTexture", &iIsEffectTexture, sizeof(_int))))
				return E_FAIL;
		}


		if (FAILED(m_pModel->SetUp_RawValue("g_vRimLightColor", &vRimLightColor, sizeof(_float3))))
			return E_FAIL;

		if (FAILED(m_pModel->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int))))
			return E_FAIL;

		m_pModel->Render(i, m_iPassIndex);

	}

	return S_OK;
}

void CEngels::LerpLookToFirstLook(_double TimeDelta)
{
	if (false == m_bFirstLookLerp)
		return;

	m_FirstLookLerpTime += TimeDelta * 0.5;

	if (m_FirstLookLerpTime >= 1.0)
		m_FirstLookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vFirstLook), (_float)m_FirstLookLerpTime));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetZ(vScale));
}

void CEngels::Check_Pattern(_double TimeDelta)
{
	if (PHASE::PHASE_APPEARANCE == m_eCurPhase)
	{
		if (PATTERN::APPEARNCE == m_eCurPattern)
		{
			if (m_bStart && !m_bIsWaterSplashCreated)
			{
				m_fWaterSplashCreateDelay += (_float)TimeDelta;
				if (m_fWaterSplashCreateDelay > WATER_SPLASH_CREATE_DELAY)
				{
					_float4 vPos = _float4(0.f, 0.f, 0.f, 0.f);
					XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE::POSITION));
					vPos.z -= 33.f;
					vPos.y += 5.f;
					Create_WaterSplash(vPos);
	
					m_bIsWaterSplashCreated = true;
				}



			}

			m_iAnimation = ANIM_ENGELS_APPREANCE;
			m_pModel->Set_NoneBlend();

			m_bContinue = false;
			
		}

		if (m_pModel->IsFinish_Animation())
		{
			m_iAnimation = ANIM_ENGELS_CLAP;
			m_pModel->Set_NoneBlend();

			m_bContinue = false;

			m_eCurPhase = PHASE::PHASE_NORMAL;
			m_eCurPattern = PATTERN::CLAP;
		}
	}
	else if (PHASE::PHASE_NORMAL == m_eCurPhase)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (PATTERN::CLAP == m_eCurPattern)
		{
			Rotate_Wheel(TimeDelta);

			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::CHAINSAW));
			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::ARM));

			if (ANIM_ENGELS_CLAP == iCurAnimIndex)
			{
				_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
				_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();
				_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 14;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(0));

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(1));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(2));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_ClapEffect(vPos);

						CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();
						pCamMgr->Set_MainCam_ShakeHitBig_True();

						CSoundMgr::Get_Instance()->PlaySound(L"Engels_FuryClap.wav", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}

				}
				else if (CollMaxRatio <= PlayTimeRatio)
					m_bAtkDead = true;

				if (ENGELS_CLAP_DELETE_HITPART_RATIO <= PlayTimeRatio)
					Delete_HitPart();

				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = PATTERN::HOWL;
					m_iAnimation = ANIM_ENGELS_HOWL_START;

					m_pModel->Set_NoneBlend();
					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;

					Initialize_RotateAngle();
					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::HOWL == m_eCurPattern)
		{
			if (ANIM_ENGELS_HOWL_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_HOWL_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ENGELS_HOWL_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_HOWL_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_CreateMainBulletTime = 0.0;
					m_CreateTime = 0.0;
				}
			}
			else if (ANIM_ENGELS_HOWL_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = PATTERN::SPIKERIGHTARM;
					m_iAnimation = ANIM_ENGELS_SPIKE_RIGHTARM;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::SPIKERIGHTARM == m_eCurPattern)
		{
			Rotate_Wheel(TimeDelta);

			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::CHAINSAW));
			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::ARM));

			if (ANIM_ENGELS_SPIKE_RIGHTARM == iCurAnimIndex)
			{
				_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
				_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();
				_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 15;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;

						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);
					}
				}
				else if (CollMaxRatio <= PlayTimeRatio)
					m_bAtkDead = true;

				if (ENGELS_SPIKERIGHTARM_DELETE_HITPART_RATIO <= PlayTimeRatio)
					Delete_HitPart();

				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = PATTERN::SHOOTMISSILE;
					m_iAnimation = ANIM_ENGELS_SHOOTMISSILE_START;

					m_pModel->Set_NoneBlend();

					m_bFirstLookLerp = true;
					m_FirstLookLerpTime = 0.0;
					m_bCreateOnce = false;
					m_bAtkDead = false;

					m_CreateTime = 0.0;
					m_bIsCreatedMissileLaunchEffect = false;

					Initialize_RotateAngle();
					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::SHOOTMISSILE == m_eCurPattern)
		{
			if (m_bFirstLookLerp)
			{
				if (m_FirstLookLerpTime >= 1.0)
				{
					m_bFirstLookLerp = false;
					m_FirstLookLerpTime = 0.0;
				}
				else
					return;
			}

			if (ANIM_ENGELS_SHOOTMISSILE_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_SHOOTMISSILE_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_bIsCreatedMissileLaunchEffect = false;

				}
			}
			else if (ANIM_ENGELS_SHOOTMISSILE_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_SHOOTMISSILE_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ENGELS_SHOOTMISSILE_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_CONSECUTIVE_SPIKE_START;
					m_eCurPattern = PATTERN::CONSECUTIVE_SPIKE;

					m_pModel->Set_NoneBlend();

					m_bContinue = false;
					m_CreateTime = 0.0;

					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::CONSECUTIVE_SPIKE == m_eCurPattern)
		{
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
			_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
			_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();

			Rotate_Wheel(TimeDelta);

			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::CHAINSAW));
			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::ARM));


			if (ANIM_ENGELS_CONSECUTIVE_SPIKE_START == iCurAnimIndex)
			{
				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 16;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;

						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));
						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);
					}
					else if (m_bCreateOnce)
						m_bAtkDead = true;

					if (ENGELS_CONSECUTIVE_SPIKE_START_DELETE_HITPART_RATIO <= PlayTimeRatio)
						Delete_HitPart();
				}
				else if (CollMaxRatio <= PlayTimeRatio)
				{
					if (m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 16;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = false;
						m_bAtkDead = false;

						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));
						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);
					}
					else if (false == m_bCreateOnce)
						m_bAtkDead = true;
				}

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_CONSECUTIVE_SPIKE_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
			}
			else if (ANIM_ENGELS_CONSECUTIVE_SPIKE_DW == iCurAnimIndex)
			{
				if (ENGELS_CONSECUTIVE_SPIKE_DW_DELETE_HITPART_RATIO <= PlayTimeRatio
					&& CollMinRatio > PlayTimeRatio)
					Delete_HitPart();

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 16;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;

						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));
						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);
					}
					else if (m_bCreateOnce)
						m_bAtkDead = true;

					if (ENGELS_CONSECUTIVE_SPIKE_DW_SEC_DELETE_HITPART_RATIO <= PlayTimeRatio)
						Delete_HitPart();
				}
				else if (CollMaxRatio <= PlayTimeRatio)
				{
					if (m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 16;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = false;
						m_bAtkDead = false;

						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));
						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);
					}
					else if (false == m_bCreateOnce)
						m_bAtkDead = true;
				}


				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_CONSECUTIVE_SPIKE_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
			}
			else if (ANIM_ENGELS_CONSECUTIVE_SPIKE_END == iCurAnimIndex)
			{

				if (ENGELS_CONSECUTIVE_SPIKE_END_DELETE_HITPART_RATIO <= PlayTimeRatio
					&& CollMinRatio > PlayTimeRatio)
					Delete_HitPart();

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 19;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(1));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						_float4 vTempPos;
						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(2));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						memcpy(&vTempPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_SpikeEffect(vTempPos);

					}
					else if (m_bCreateOnce)
						m_bAtkDead = true;
				}
				else if (CollMaxRatio <= PlayTimeRatio)
					Delete_HitPart();

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_HOWL_START;
					m_eCurPattern = PATTERN::SECOND_HOWL;

					m_pModel->Set_NoneBlend();
					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;

					Initialize_RotateAngle();
					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::SECOND_HOWL == m_eCurPattern)
		{
			if (ANIM_ENGELS_HOWL_START == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_HOWL_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ENGELS_HOWL_DW == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_HOWL_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;
				}
			}
			else if (ANIM_ENGELS_HOWL_END == iCurAnimIndex)
			{
				if (m_pModel->IsFinish_Animation())
				{
					m_eCurPattern = PATTERN::REACHARM;
					m_iAnimation = ANIM_ENGELS_REACHARM_START;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					Check_DeadPattern();
				}
			}
		}
		else if (PATTERN::REACHARM == m_eCurPattern)
		{
			Rotate_Wheel(TimeDelta);

			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
			_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
			_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();

			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::CHAINSAW));
			Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::ARM));

			if (ANIM_ENGELS_REACHARM_START == iCurAnimIndex)
			{

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 17;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;
						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_RightStraight_SparkHoldEffect(vPos);

						CSoundMgr::Get_Instance()->StopPlaySound(L"Engels_RockGrinding.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}

					else if (ENGELS_REACHARM_START_FIRSTATTACK_MAXRATIO <= PlayTimeRatio)
						m_bAtkDead = true;
				}
				else if (CollMaxRatio <= PlayTimeRatio &&
					ENGELS_REACHARM_START_SECONDATTACK_MAXRATIO > PlayTimeRatio)
				{
					if (m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 17;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = false;
						m_bAtkDead = false;
						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_LeftStraight_SparkHoldEffect(vPos);

						CSoundMgr::Get_Instance()->StopPlaySound(L"Engels_RockGrinding.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}
				}

				else if (ENGELS_REACHARM_START_SECONDATTACK_MAXRATIO <= PlayTimeRatio)
					m_bAtkDead = true;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_REACHARM_DW;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
			}
			else if (ANIM_ENGELS_REACHARM_DW == iCurAnimIndex)
			{

				if (CollMinRatio <= PlayTimeRatio &&
					CollMaxRatio > PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 17;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;

						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_RightStraight_SparkHoldEffect(vPos);

						CSoundMgr::Get_Instance()->StopPlaySound(L"Engels_RockGrinding.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}

					else if (ENGELS_REACHARM_DW_FIRSTATTACK_MAXRATIO <= PlayTimeRatio)
						m_bAtkDead = true;
				}

				else if (CollMaxRatio <= PlayTimeRatio &&
					ENGELS_REACHARM_DW_SECONDATTACK_MAXRATIO > PlayTimeRatio)
				{
					if (m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 17;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = false;
						m_bAtkDead = false;

						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_LeftStraight_SparkHoldEffect(vPos);

						CSoundMgr::Get_Instance()->StopPlaySound(L"Engels_RockGrinding.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}
				}

				else if (ENGELS_REACHARM_DW_SECONDATTACK_MAXRATIO <= PlayTimeRatio)
					m_bAtkDead = true;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_REACHARM_END;
					m_pModel->Set_NoneBlend();

					m_bContinue = false;

					m_CreateTime = 0.0;
					m_CreateMainBulletTime = 0.0;

					m_bCreateOnce = false;
					m_bAtkDead = false;
				}
			}
			else if (ANIM_ENGELS_REACHARM_END == iCurAnimIndex)
			{
				if (CollMinRatio <= PlayTimeRatio)
				{
					if (!m_bCreateOnce)
					{
						CGameInstance* pGameInstance = CGameInstance::Get_Instance();
						Humanoid_Atk_Info tInfo;

						tInfo.iNumber = 18;
						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack());

						CNormal_Attack* pAttack = nullptr;
						if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
							, (CGameObject**)&pAttack, &tInfo)))
							return;

						m_vecNormalAttacks.push_back(pAttack);

						Safe_AddRef(pAttack);

						m_bCreateOnce = true;
						m_bAtkDead = false;

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(1));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(2));
						Create_HitPart(XMLoadFloat4x4(&tInfo.Matrix));

						_float4 vPos;
						memcpy(&vPos, &tInfo.Matrix.m[3], sizeof(_float4));
						Create_LeftStraight_SparkHoldEffect(vPos);
						vPos.x -= 7.f;
						Create_RightStraight_SparkHoldEffect(vPos);

						CSoundMgr::Get_Instance()->StopPlaySound(L"Engels_RockGrinding.mp3", CSoundMgr::CHANNELID::BOSS_EFFECT);
					}
				}

				if (ENGELS_REACHARM_END_SEC_DELETE_HITPART_RATIO <= PlayTimeRatio)
					Delete_HitPart();

				if (CollMaxRatio <= PlayTimeRatio)
					m_bAtkDead = true;

				if (m_pModel->IsFinish_Animation())
				{
					m_iAnimation = ANIM_ENGELS_CLAP;
					m_eCurPattern = PATTERN::CLAP;

					m_pModel->Set_NoneBlend();

					m_bAtkDead = false;
					m_bCreateOnce = false;

					Initialize_RotateAngle();
					Check_DeadPattern();

					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::BOSS_EFFECT);
				}
			}
		}
		else if (PATTERN::DEAD == m_eCurPattern)
		{
			m_iAnimation = ANIM_ENGELS_DEAD;
			m_bContinue = false;

			m_bCollision = false;

			if (ANIM_ENGELS_DEAD == iCurAnimIndex)
			{
				if (!m_bIsDeathWaterSplashCreated && m_pModel->Get_PlayTimeRatio() >= 0.6)
				{
					m_bIsDeathWaterSplashCreated = true;

					_float4 vPos = _float4(0.f, 0.f, 0.f, 0.f);
					XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE::POSITION));
					vPos.z -= 33.f;
					vPos.y += 5.f;
					Create_WaterSplash(vPos);
				}

				if (m_pModel->IsFinish_Animation())
				{
					//m_DeadTime += TimeDelta;
					//if (m_DeadTime >= ENGELS_DEAD_DELAYTIME)
					//	m_eState = OBJSTATE::DEAD;
					
					m_eState = OBJSTATE::DEAD;
				}
			}
		}
	}
}

void CEngels::Create_FireMissile(_double TimeDelta)
{
	if (ANIM_ENGELS_SHOOTMISSILE_DW == m_pModel->Get_CurrentAnimation())
	{
		m_CreateTime += TimeDelta;

		if (m_CreateTime >= m_CreateFireMissileDelayTime)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vRight = XMVectorZero();
			_vector vPosition = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vPosition, &WorldFloat4x4.m[3], sizeof(_vector));

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

			vPosition += vUp * 90.f;
			vPosition += vLook * 8.f;

			_vector vDir = XMVector3Normalize(vLook + vUp);
			vDir = XMVectorSetY(vDir, XMVectorGetY(vDir) - 0.6f);

			CEngelsFireMissile* pMissile = nullptr;
			FIRE_MISSILE_DESC tDesc;
			tDesc.vDirection = vDir;

			tDesc.vPosition = vPosition;
			pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Engels_FireMissile", L"MonsterAttack", (CGameObject**)&pMissile, &tDesc);

			if (nullptr == pMissile)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			if (m_iNumMissile % 4 == 0)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::BOSS_EFFECT);
				CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::BOSS_EFFECT);
			}

			else if (m_iNumMissile % 4 == 1)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::MISSILE);
				CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE);
			}

			else if (m_iNumMissile % 4 == 2)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::MISSILE2);
				CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE2);
			}

			else if (m_iNumMissile % 4 == 3)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::MISSILE3);
				CSoundMgr::Get_Instance()->PlaySound(L"Missile.mp3", CSoundMgr::MISSILE3);
			}

			++m_iNumMissile;

			CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
			if (nullptr == pTransform)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			pMissile->Set_TargetPos(pTransform->Get_State(CTransform::STATE::POSITION));
			RELEASE_INSTANCE(CGameInstance);

			m_CreateTime = 0.0;

			_float4 f4Pos;
			XMStoreFloat4(&f4Pos, vPosition);
			if (m_bIsCreatedMissileLaunchEffect == false)
			{
				m_bIsCreatedMissileLaunchEffect = true;
				Create_MissileLaunchEffect(f4Pos);
			}
		}

	}

	return;
}

void CEngels::Create_MainBullet(_double TimeDelta)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	if (ANIM_ENGELS_HOWL_DW == iCurAnimIndex)
	{
		m_CreateMainBulletTime += TimeDelta;

		if (m_CreateMainBulletTime >= m_CreateMainBulletDelayTime)
		{
			CEffectFactory* pFactory = CEffectFactory::Get_Instance();
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			const _matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

			CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM, 0);
			if (nullptr == pPlayerTransform)
				return;

			for (_uint i = 1; i < 9; ++i)
			{
				_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

				_matrix	  WorldMatrix = m_pTransform->Get_WorldMatrix();

				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vRight = XMVectorZero();
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vLook = XMVectorZero();
				_vector vPos = XMVectorZero();

				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				vLook = XMVector3Normalize(vLook);

				vPos += (vUp * 70.f);
				vPos += (vLook * 30.f);

				_int iRandVal = rand() % 2;
				_float fRandUp = (rand() % 2) + ((rand() % 11) * 0.1f);
				if (iRandVal)
					fRandUp *= -1.f;

				iRandVal = rand() % 2;
				_float fRandRight = (rand() % 2) + ((rand() % 11) * 0.2f);

				if (iRandVal)
					fRandRight *= -1.f;

				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + _float(fRandUp));
				vPos = XMVectorSetX(vPos, XMVectorGetX(vPos) + _float(fRandRight));

				_vector vDir = XMVector3Normalize(vPlayerPos - vPos);

				_float3 vEffectDir = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&vEffectDir, vDir);

				EFFECT_PACKET tPacket;
				tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
				tPacket.fMaxLifeTime = ENGELS_RANDOM_BULLET_MAX_LIFETIME;
				tPacket.tEffectMovement.fSpeed = ENGELS_RANDOM_BULLET_SPEED;
				tPacket.tEffectMovement.fJumpPower = 0.f;
				tPacket.tEffectMovement.iJumpCount = 0;
				tPacket.tEffectMovement.vDirection = vEffectDir;
				XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPos);
				tPacket.iNavIndex = m_pNavigation->Get_Index();

				tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
				pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);
			}

			m_CreateMainBulletTime = 0.0;
		}

		Set_RimLightColor(static_cast<_int>(MESH_CONTAINER::HEAD));
		m_arrGlow[static_cast<_int>(MESH_CONTAINER::HEAD)] = 1;
	}
	else
	{
		m_arrRimLightColor[static_cast<_int>(MESH_CONTAINER::HEAD)] = _float3(0.f, 0.f, 0.f);
		m_arrGlow[static_cast<_int>(MESH_CONTAINER::HEAD)] = 0;
	}
}

void CEngels::Create_RandomBullet(_double TimeDelta)
{
	if (ANIM_ENGELS_HOWL_DW == m_pModel->Get_CurrentAnimation())
	{
		m_CreateTime += TimeDelta;

		if (m_CreateTime >= m_CreateBulletDelayTime)
		{
			CEffectFactory* pFactory = CEffectFactory::Get_Instance();
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			const _matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

			CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM, 0);
			if (nullptr == pPlayerTransform)
				return;

			for (_uint i = 1; i < 14; ++i)
			{
				_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

				_matrix	  WorldMatrix = m_pTransform->Get_WorldMatrix();

				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vRight = XMVectorZero();
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vLook = XMVectorZero();
				_vector vPos = XMVectorZero();

				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				vLook = XMVector3Normalize(vLook);

				vPos += (vUp * 80.f);
				vPos += (vLook * 20.f);

				_int iRandVal = rand() % 2;
				_float fRandUp = (rand() % 2) + ((rand() % 11) * 0.3f);
				if (iRandVal)
					fRandUp *= -1.f;

				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + _float(fRandUp));

				vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPos));

				_vector vDir = XMVectorZero();
				if (i < 7)
				{
					_vector vPlayerRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					vPlayerPos += vPlayerRight * _float(i * 3);

					vDir = XMVector3Normalize(vPlayerPos - vPos);
				}
				else
				{
					_int iVal = i - 5;
					_vector vPlayerRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					vPlayerPos += vPlayerRight * (iVal * -3.f);

					vDir = XMVector3Normalize(vPlayerPos - vPos);
				}

				_float3 vEffectDir = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&vEffectDir, vDir);

				vEffectDir.y -= 0.25f;

				EFFECT_PACKET tPacket;
				tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
				tPacket.fMaxLifeTime = ENGELS_RANDOM_BULLET_MAX_LIFETIME;
				tPacket.tEffectMovement.fSpeed = ENGELS_RANDOM_BULLET_SPEED;
				tPacket.tEffectMovement.fJumpPower = 0.f;
				tPacket.tEffectMovement.iJumpCount = 0;
				tPacket.tEffectMovement.vDirection = vEffectDir;
				XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPos);
				tPacket.iNavIndex = m_pNavigation->Get_Index();

				tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
				pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);
			}

			for (_uint i = 1; i < 9; ++i)
			{
				_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

				_matrix	  WorldMatrix = m_pTransform->Get_WorldMatrix();

				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vRight = XMVectorZero();
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vLook = XMVectorZero();
				_vector vPos = XMVectorZero();

				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				vLook = XMVector3Normalize(vLook);

				vPos += (vUp * 65.f);
				vPos += (vLook * 20.f);

				_int iRandVal = rand() % 2;
				_float fRandUp = (rand() % 2) + ((rand() % 11) * 0.5f);
				if (iRandVal)
					fRandUp *= -1.f;

				vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + _float(fRandUp));

				vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPos));

				_vector vDir = XMVectorZero();
				if (i < 5)
				{
					_vector vPlayerRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					vPlayerPos += vPlayerRight * _float(i * 5);

					vDir = XMVector3Normalize(vPlayerPos - vPos);
				}
				else
				{
					_int iVal = i - 5;
					_vector vPlayerRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
					vPlayerPos += vPlayerRight * (iVal * -5.f);

					vDir = XMVector3Normalize(vPlayerPos - vPos);
				}

				_float3 vEffectDir = _float3(0.f, 0.f, 0.f);
				XMStoreFloat3(&vEffectDir, vDir);

				vEffectDir.y -= 0.2f;

				EFFECT_PACKET tPacket;
				tPacket.iAtt = 20;
				tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
				tPacket.fMaxLifeTime = ENGELS_RANDOM_BULLET_MAX_LIFETIME;
				tPacket.tEffectMovement.fSpeed = ENGELS_RANDOM_BULLET_SPEED;
				tPacket.tEffectMovement.fJumpPower = 0.f;
				tPacket.tEffectMovement.iJumpCount = 0;
				tPacket.tEffectMovement.vDirection = vEffectDir;
				XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPos);
				tPacket.iNavIndex = m_pNavigation->Get_Index();

				tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
				pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &tPacket);
			}

			m_CreateTime = 0.0;
		}
	}
}

void CEngels::Create_HitPart(_matrix Matrix)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (PATTERN::CLAP == m_eCurPattern)
	{
		if (ANIM_ENGELS_CLAP == iCurAnimIndex)
		{
			CEngelsHitPart* pPart = nullptr;
			CCollider::tagColliderDesc tDesc{};
			tDesc.vPivot = _float3(0.f, 0.f, 0.f);
			tDesc.vScale = _float3(3.f, 8.f, 5.f);

			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_ENGELS_HITPART, L"BossPart", (CGameObject**)&pPart, &tDesc)))
				return;

			if (nullptr == pPart)
				return;

			pPart->Set_WorldMatrix(Matrix);

			Safe_AddRef(pPart);
			m_vecHitPart.push_back(pPart);
		}
	}
	else if (PATTERN::SPIKERIGHTARM == m_eCurPattern)
	{
		if (ANIM_ENGELS_SPIKE_RIGHTARM == iCurAnimIndex)
		{
			CEngelsHitPart* pPart = nullptr;
			CCollider::tagColliderDesc tDesc{};
			tDesc.vPivot = _float3(0.f, 0.f, 0.f);
			tDesc.vScale = _float3(1.8f, 8.f, 10.f);

			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_ENGELS_HITPART, L"BossPart", (CGameObject**)&pPart, &tDesc)))
				return;

			if (nullptr == pPart)
				return;

			pPart->Set_WorldMatrix(Matrix);

			Safe_AddRef(pPart);
			m_vecHitPart.push_back(pPart);
		}
	}
	else if (PATTERN::CONSECUTIVE_SPIKE == m_eCurPattern)
	{
		_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

		_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
		_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();

		if (ANIM_ENGELS_CONSECUTIVE_SPIKE_START == iCurAnimIndex || ANIM_ENGELS_CONSECUTIVE_SPIKE_DW == iCurAnimIndex)
		{
			CEngelsHitPart* pPart = nullptr;
			CCollider::tagColliderDesc tDesc{};
			tDesc.vPivot = _float3(0.f, 0.f, 0.f);
			tDesc.vScale = _float3(3.f, 8.f, 5.f);

			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_ENGELS_HITPART, L"BossPart", (CGameObject**)&pPart, &tDesc)))
				return;

			if (nullptr == pPart)
				return;

			pPart->Set_WorldMatrix(Matrix);

			Safe_AddRef(pPart);
			m_vecHitPart.push_back(pPart);
		}
		else if (ANIM_ENGELS_CONSECUTIVE_SPIKE_END == iCurAnimIndex)
		{
			CEngelsHitPart* pPart = nullptr;
			CCollider::tagColliderDesc tDesc{};
			tDesc.vPivot = _float3(0.f, 0.f, 0.f);
			tDesc.vScale = _float3(3.f, 8.f, 5.f);

			if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_ENGELS_HITPART, L"BossPart", (CGameObject**)&pPart, &tDesc)))
				return;

			if (nullptr == pPart)
				return;

			pPart->Set_WorldMatrix(Matrix);

			Safe_AddRef(pPart);
			m_vecHitPart.push_back(pPart);
		}
	}
	else if (PATTERN::REACHARM == m_eCurPattern)
	{
		CEngelsHitPart* pPart = nullptr;
		CCollider::tagColliderDesc tDesc{};
		tDesc.vPivot = _float3(0.f, 0.f, 0.f);
		tDesc.vScale = _float3(3.f, 8.f, 5.f);

		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_ENGELS_HITPART, L"BossPart", (CGameObject**)&pPart, &tDesc)))
			return;

		if (nullptr == pPart)
			return;

		pPart->Set_WorldMatrix(Matrix);

		Safe_AddRef(pPart);
		m_vecHitPart.push_back(pPart);
	}
}

void CEngels::Delete_HitPart()
{
	if (0 >= m_vecHitPart.size())
		return;

	for (auto& pPart : m_vecHitPart)
	{
		pPart->Set_State(OBJSTATE::DEAD);
		Safe_Release(pPart);
	}

	m_vecHitPart.clear();
}

void CEngels::Update_TimeDelta(_double & TimeDelta)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

	if (ANIM_ENGELS_CLAP == iCurAnimIndex)
	{
		if (ENGELS_CLAP_UPDATETIMEDELTA_START_RATIO <= PlayTimeRatio
			&& ENGELS_CLAP_UPDATETIMEDELTA_END_RATIO > PlayTimeRatio)
			TimeDelta *= 0.3;
	}
}

void CEngels::Check_DeadPattern()
{
	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		m_eCurPattern = PATTERN::DEAD;
		m_iAnimation = ANIM_ENGELS_DEAD;

		m_bContinue = false;

		m_tObjectStatusDesc.fHp = 0.f;
	}
}

void CEngels::Rotate_Wheel(_double TimeDelta)
{
	m_fRotateAngle += (_float)(TimeDelta * m_RotatePower);

	_matrix RightOffSetMatrix = m_pModel->Get_OffsetMatrix(L"RightWheel");
	_matrix LeftOffSetMatrix = m_pModel->Get_OffsetMatrix(L"LeftWheel");

	_float4x4 RightOffSetFloat4x4;
	XMStoreFloat4x4(&RightOffSetFloat4x4, RightOffSetMatrix);

	_float4x4 LeftOffSetFloat4x4;
	XMStoreFloat4x4(&LeftOffSetFloat4x4, LeftOffSetMatrix);

	_vector vRight = XMVectorZero();
	memcpy(&vRight, &RightOffSetFloat4x4.m[0], sizeof(_vector));

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(vRight), XMConvertToRadians(m_fRotateAngle));
	m_pModel->Set_OffsetMatrix(L"RightWheel", RightOffSetMatrix * RotateMatrix);

	memcpy(&vRight, &LeftOffSetFloat4x4.m[0], sizeof(_vector));

	RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(vRight), XMConvertToRadians(m_fRotateAngle));
	m_pModel->Set_OffsetMatrix(L"LeftWheel", LeftOffSetMatrix * RotateMatrix);
}

void CEngels::Initialize_RotateAngle()
{
	m_fRotateAngle = 0.f;
}

void CEngels::Update_NormalAttack(_double TimeDelta)
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
				if (PATTERN::REACHARM == m_eCurPattern && (ANIM_ENGELS_REACHARM_END != m_pModel->Get_CurrentAnimation()))
					Update_ReachArmPos(TimeDelta);
				else
				{
					CTransform*	pAttackTransform = dynamic_cast<CTransform*>(pAttack->Get_Component(COM_KEY_TRANSFORM));

					if (nullptr != pAttackTransform)
						pAttackTransform->Set_WorldMatrix(Get_MatrixForNormalAttack(iCount));

					++iCount;
				}
			}
		}
	}

	return;
}

void CEngels::Update_ReachArmPos(_double TimeDelta)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	_double	PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

	_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
	_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();

	if (ANIM_ENGELS_REACHARM_START == iCurAnimIndex)
	{
		if (CollMinRatio <= PlayTimeRatio &&
			ENGELS_REACHARM_START_FIRSTATTACK_MAXRATIO > PlayTimeRatio)
		{
			CTransform* pTransform = (CTransform*)m_vecNormalAttacks.front()->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
				return;

			pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), ENGELS_REACHARM_SPEED);
		}
		else if (CollMaxRatio <= PlayTimeRatio &&
			ENGELS_REACHARM_START_SECONDATTACK_MAXRATIO > PlayTimeRatio)
		{
			CTransform* pTransform = (CTransform*)m_vecNormalAttacks.front()->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
				return;

			pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), ENGELS_REACHARM_SPEED);
		}
	}
	else if (ANIM_ENGELS_REACHARM_DW == iCurAnimIndex)
	{
		if (CollMinRatio <= PlayTimeRatio &&
			ENGELS_REACHARM_DW_FIRSTATTACK_MAXRATIO > PlayTimeRatio)
		{
			CTransform* pTransform = (CTransform*)m_vecNormalAttacks.front()->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
				return;

			pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), ENGELS_REACHARM_SPEED);
		}
		else if (CollMaxRatio <= PlayTimeRatio &&
			ENGELS_REACHARM_DW_SECONDATTACK_MAXRATIO > PlayTimeRatio)
		{
			CTransform* pTransform = (CTransform*)m_vecNormalAttacks.front()->Get_Component(COM_KEY_TRANSFORM);
			if (nullptr == pTransform)
				return;

			pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), ENGELS_REACHARM_SPEED);
		}
	}
}

_matrix CEngels::Get_MatrixForNormalAttack(_uint iCount, _vector& _vPos)
{
	if (PATTERN::CLAP == m_eCurPattern)
	{
		_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
		_float4x4 WorldFloat4x4;

		if (0 == iCount)
		{
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vRight = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
		}
		else if (1 == iCount)
		{
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vRight = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;
			vPos += XMVector3Normalize(vRight) * 6.f;

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
		}
		else if (2 == iCount)
		{
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vRight = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;
			vPos += XMVector3Normalize(vRight) * -6.f;

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));
		}

		return XMLoadFloat4x4(&WorldFloat4x4);
	}
	else if (PATTERN::SPIKERIGHTARM == m_eCurPattern)
	{
		_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
		_float4x4 WorldFloat4x4;
		XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVectorZero();
		_vector vRight = XMVectorZero();
		_vector vPos = XMVectorZero();

		memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
		memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
		memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

		vPos += vUp * 60.f;
		vPos += XMVector3Normalize(vLook) * 71.f;
		vPos += XMVector3Normalize(vRight) * 0.5f;

		memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

		return XMLoadFloat4x4(&WorldFloat4x4);
	}
	else if (PATTERN::CONSECUTIVE_SPIKE == m_eCurPattern)
	{
		if (1 == iCount)
		{
			_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vRight = XMVectorZero();
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;
			vPos += XMVector3Normalize(vRight) * 6.f;

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

			return XMLoadFloat4x4(&WorldFloat4x4);
		}
		else if (2 == iCount)
		{
			_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vRight = XMVectorZero();
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;
			vPos += XMVector3Normalize(vRight) * -6.f;

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

			return XMLoadFloat4x4(&WorldFloat4x4);
		}
		else
		{
			_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vRight = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 71.f;
			vPos += XMVector3Normalize(vRight);

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

			return XMLoadFloat4x4(&WorldFloat4x4);
		}
	}
	else if (PATTERN::REACHARM == m_eCurPattern)
	{
		if (ANIM_ENGELS_REACHARM_END == m_pModel->Get_CurrentAnimation())
		{
			if (1 == iCount)
			{
				_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vLook = XMVectorZero();
				_vector vRight = XMVectorZero();
				_vector vPos = XMVectorZero();

				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				vPos += vUp * 60.f;
				vPos += XMVector3Normalize(vLook) * 71.f;
				vPos += XMVector3Normalize(vRight) * 6.f;

				memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

				return XMLoadFloat4x4(&WorldFloat4x4);
			}
			else if (2 == iCount)
			{
				_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				_vector vLook = XMVectorZero();
				_vector vRight = XMVectorZero();
				_vector vPos = XMVectorZero();

				memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
				memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

				vPos += vUp * 60.f;
				vPos += XMVector3Normalize(vLook) * 71.f;
				vPos += XMVector3Normalize(vRight) * -6.f;

				memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

				return XMLoadFloat4x4(&WorldFloat4x4);
			}
		}
		else
		{
			_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			_vector vLook = XMVectorZero();
			_vector vRight = XMVectorZero();
			_vector vPos = XMVectorZero();

			memcpy(&vRight, &WorldFloat4x4.m[0], sizeof(_vector));
			memcpy(&vLook, &WorldFloat4x4.m[2], sizeof(_vector));
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

			vPos += vUp * 60.f;
			vPos += XMVector3Normalize(vLook) * 65.f;

			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
			_double CollMinRatio = m_pModel->Get_CollisionMinRatio();
			_double CollMaxRatio = m_pModel->Get_CollisionMaxRatio();

			if (CollMinRatio <= PlayTimeRatio)
			{
				if (CollMaxRatio <= PlayTimeRatio)
					vPos += XMVector3Normalize(vRight) * -3.f;
				else
					vPos += XMVector3Normalize(vRight) * 3.f;
			}

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

			return XMLoadFloat4x4(&WorldFloat4x4);
		}
	}

	return XMMatrixIdentity();
}

void CEngels::Compute_DOF_Parameters()
{
	m_vDOFParams.y = Get_CameraDistance() + 15.5f;
	m_vDOFParams.z = m_vDOFParams.y * 5.f;
	m_vDOFParams.x = 0.f;
}

void CEngels::Notify(void * pMessage)
{
}

_int CEngels::VerifyChecksum(void * pMessage)
{
	return _int();
}

_bool CEngels::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CEngels::Collision(CGameObject * pGameObject)
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
			m_dHitTextureTime = HIT_TEXTURE_MAX_TIME;
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

HRESULT CEngels::Update_UI(_double dDeltaTime)
{
	//if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	//{
	//m_dCallHPBarUITimeDuration -= dDeltaTime;
	if (m_bShowUI)
	{
		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::BOSS;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;
		desc.eID = CUI_Dialogue_EngageMessage::BOSSID::ENGELS;

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 85.f);

		XMStoreFloat4(&desc.vWorldPos, vPos);

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MONSTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MONSTER_HPBAR, &desc));

		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_dCallHPBarUITimeDuration < 0)
	{
		m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
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
		CUI_Dialogue_EngageMessage::BOSSID	eID = CUI_Dialogue_EngageMessage::BOSSID::ENGELS;

		CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eID));

		m_iFlagUI ^= FLAG_UI_CALL_ENGAGE;

		m_bShowUI = true;
	}
	if (OBJSTATE::DEAD == m_eState)
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		CVendingMachine* pInstance = dynamic_cast<CVendingMachine*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ROBOTGENERAL, L"Environment"));
		if (nullptr == pInstance)
			return -1;
		else
			pInstance->Set_AlphaTest();

		CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

		tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
		tDesc.eObjState = OBJSTATE::ENABLE;
		tDesc.iObjID = pInstance->Get_ObjID();

		XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pInstance->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

		CUI_HUD_StageObjective::UIOBJECTIVEDESC	tObjectiveDesc;

		tObjectiveDesc.eQuest = CUI_HUD_StageObjective::QUEST::ENGELS;
		tObjectiveDesc.iEventCount = 1;

		pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &tObjectiveDesc));
	}
	return S_OK;
}

void CEngels::Set_UI_Engage_Cinematic()
{
	m_iFlagUI |= FLAG_UI_CALL_CINEMATIC;
}

void CEngels::Set_UI_Engage()
{
	m_iFlagUI |= FLAG_UI_CALL_ENGAGE;
}

HRESULT CEngels::SetUp_Components()
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
	if (FAILED(__super::Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_MODEL_ENGELS, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"RightWheel", "bone305");
	m_pModel->Add_RefNode(L"LeftWheel", "bone561");

	m_pModel->Add_RefNode(L"Face", "bone018");
	m_pModel->Add_RefNode(L"Center", "bone006");

	CCollider::tagColliderDesc tColliderDesc;
	tColliderDesc.vPivot = _float3(0.f, 65.f, 0.f);
	tColliderDesc.vScale = _float3(32.f, 65.f, 20.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.vPivot.y += 8.f;
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	_uint iCurIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurIndex)))
		return E_FAIL;

	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));

	// Hit Texture
	if (FAILED(__super::Add_Components((_uint)LEVEL::ROBOTGENERAL, L"Prototype_Compoent_Texture_EngelsHit", TEXT("Com_HitTexture"), (CComponent**)&m_pHitTexture)))
		return E_FAIL;




	return S_OK;
}

CEngels * CEngels::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CEngels* pGameInstance = new CEngels(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CEngels");
	}

	return pGameInstance;
}

CGameObject * CEngels::Clone(void * pArg)
{
	CEngels* pGameInstance = new CEngels(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CEngels");
	}

	return pGameInstance;
}

void CEngels::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pHitTexture);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}
	m_vecNormalAttacks.clear();

	for (auto& pHitPart : m_vecHitPart)
	{
		pHitPart->Set_State(OBJSTATE::DEAD);
		Safe_Release(pHitPart);
	}
	m_vecHitPart.clear();
}

HRESULT CEngels::Create_WaterSplash(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when automaticaly;
	tPacket.tEffectMovement.vPosition = _vPos;

	// Offset
	tPacket.ulID = CHECKSUM_EFFECT_ENGELS_WATERSPLASH;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_WaterSplash.dat", &tPacket);

	CSoundMgr::Get_Instance()->PlaySound(L"EngelsAppear0.wav", CSoundMgr::CHANNELID::BOSS_EFFECT);

	return S_OK;
}

HRESULT CEngels::Create_ClapEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;

	// Offset
	tPacket.tEffectMovement.vPosition.z -= 3.f;
	tPacket.tEffectMovement.vPosition.x -= 7.f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_CLAP;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Clap.dat", &tPacket);

	//// Offset
	//tPacket.tEffectMovement.vPosition.z -= 3.f;
	//tPacket.tEffectMovement.vPosition.x -= 7.f;
	//tPacket.tEffectMovement.vPosition.y -= 7.f;
	//tPacket.ulID = CHECKSUM_EFFECT_ENGELS_HOLD_SMOKE;
	//pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Hold_Smoke.dat", &tPacket);



	return S_OK;
}     

HRESULT CEngels::Create_SpikeEffect(_float4 _vPos)
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();
	pSoundMgr->PlaySound(TEXT("Engels_Crash.mp3"), CSoundMgr::BOSS_EFFECT);

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	CGameObject* pObj = nullptr;

	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.5f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_STAMP;


	// HMH
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 2.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.5f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_CLAP;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Stamp.dat", &tPacket);

	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = 2.f;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.5f;
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_SPARK;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Spark.dat", &tPacket);
	if (!pObj)
		assert(FALSE);

	//tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	//tPacket.fMaxLifeTime = -1.f;
	//tPacket.tEffectMovement.vPosition = _vPos;
	//tPacket.tEffectMovement.vPosition.y -= 4.5f;
	//tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_PLANE;
	//pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Plane.dat", &tPacket);
	//if (!pObj)
	//	assert(FALSE);

	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.5f;
	tPacket.ulID = CHECKSUM_EFFECT_ZHUANGZI_IMPACT_SMOKE;
	pObj = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Zhuangzi_Impact_Smoke.dat", &tPacket);
	if (!pObj)
		assert(FALSE);


	//tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	//tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	//tPacket.tEffectMovement.vPosition = _vPos;
	//tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	//pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);




	CDecalCube::DECALCUBE tCube;
	tCube.bContinue = false;
	tCube.eType = CDecalCube::TYPE::BURNINGFIELD;
	tCube.iImageIndex = 0;
	tCube.dDuration = 1.75;
	tCube.vScale = _float4(32.5f, 1.5f, 32.5f, 0.f);
	tCube.vPosition = _vPos;
	tCube.vPosition.y -= 5.f;

	tCube.iIsColorGlow = 0;
	tCube.vGlowColor = CProportionConverter()(_DARKGOLDENROD, 15.f, false);

	tCube.iIsGlow = 1;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	tCube.eType = CDecalCube::TYPE::FOGRING;
	tCube.iImageIndex = 1;
	tCube.dDuration = 1.25;
	tCube.vScale = _float4(3.5f, 1.5f, 3.5f, 0.f);
	tCube.fAddScale = 7.5f;
	tCube.iIsGlow = 0;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
		return E_FAIL;

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_MainCam_ShakeHitBig_True();

	return S_OK;
}

HRESULT CEngels::Create_RightStraight_SparkHoldEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.f;
	tPacket.tEffectMovement.vPosition.x += 1.f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);

	return S_OK;
}

HRESULT CEngels::Create_LeftStraight_SparkHoldEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y -= 4.f;
	tPacket.tEffectMovement.vPosition.x -= 2.f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);




	return S_OK;
}

HRESULT CEngels::Create_RightSpike_SparkHoldEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);

	return S_OK;
}

HRESULT CEngels::Create_LeftSpike_SparkHoldEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);

	return S_OK;
}

HRESULT CEngels::Create_OnlyRightSpike_SparkHoldEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_SPARK_HOLD;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Spark_Hold.dat", &tPacket);

	return S_OK;
}

HRESULT CEngels::Create_MissileLaunchEffect(_float4 _vPos)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.tEffectMovement.vPosition = _vPos;
	tPacket.tEffectMovement.vPosition.y += 15.f;
	tPacket.ulID = CHECKSUM_EFFECT_ENGLES_MISSILE_LAUNCH;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Engles_Missile_Launch.dat", &tPacket);

	return S_OK;
}

void CEngels::Set_RimLightColor(const _int& i)
{
	switch (static_cast<CEngels::MESH_CONTAINER>(i))
	{
	case Client::CEngels::MESH_CONTAINER::HEAD:
		m_arrRimLightColor[i] = _float3(1.f, 0.f, 1.f);
		break;
	case Client::CEngels::MESH_CONTAINER::CHAINSAW:
		m_arrRimLightColor[i] = _float3(1.f, 0.5f, 0.f);
		break;
	case Client::CEngels::MESH_CONTAINER::ARM:
		m_arrRimLightColor[i] = _float3(1.f, 0.5f, 0.f);
		break;
	default:
		break;
	}
}

void CEngels::Set_RimLightColorClear(const _int & i)
{
	switch (static_cast<CEngels::MESH_CONTAINER>(i))
	{
	case Client::CEngels::MESH_CONTAINER::HEAD:
		m_arrRimLightColor[i] = _float3(0.f, 0.f, 0.f);
		break;
	case Client::CEngels::MESH_CONTAINER::CHAINSAW:
		m_arrRimLightColor[i] = _float3(0.f, 0.f, 0.f);
		break;
	case Client::CEngels::MESH_CONTAINER::ARM:
		m_arrRimLightColor[i] = _float3(0.f, 0.f, 0.f);
		break;
	default:
		break;
	}
}

HRESULT CEngels::Update_Effects(_double _dTimeDelta)
{

	// Hit Texture
	m_dHitTextureTime -= _dTimeDelta;
	m_dHitTextureIntervalTime += _dTimeDelta;
	if (m_dHitTextureIntervalTime > HIT_TEXTURE_INTERVAL)
	{
		m_dHitTextureIntervalTime = 0.0;
		++m_iHitTextureIndex;

		if (m_iHitTextureIndex >= HIT_TEXTURE_NUMBER)
		{
			m_iHitTextureIndex = 0;
		}
	}


	return S_OK;
}
