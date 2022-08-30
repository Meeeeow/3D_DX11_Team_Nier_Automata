 #include "stdafx.h"
#include "..\Public\Humanoid_Clown.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "EffectFactory.h"
#include "Player.h"
#include "MeshEffect.h"
#include "LoadDatFiles.h"

CHumanoid_Clown::CHumanoid_Clown(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	:CGameObjectModel(pGraphicDevice,pContextDevice)
{
}

CHumanoid_Clown::CHumanoid_Clown(const CHumanoid_Clown & rhs)
	:CGameObjectModel(rhs)
{
}

HRESULT CHumanoid_Clown::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHumanoid_Clown::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eState = OBJSTATE::ENABLE;
	
	m_bCollision = true;
	if (pArg != nullptr)
	{
		MO_INFO* pInfo = (MO_INFO*)pArg;
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	}

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;
	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCHumanoid_Big_InstanceID = OriginID_HumanoidBig;
	m_iObjID = iCHumanoid_Big_InstanceID++;

	m_tObjectStatusDesc.fHp = 100;
	m_tObjectStatusDesc.fMaxHp = 100;
	m_tObjectStatusDesc.iAtt = 0;

	Check_Pause();

	Gain_Pattern();

	m_fWalkDistance += rand() % 3;

	m_ePattern = M_PATTERN::PATTERN_2;

	if (CLoadDatFiles::m_iClownCountStatic == 4)
	{
		m_ePattern = M_PATTERN::PATTERN_1;
	}

	m_bCulling = false;

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	m_fHitRecovery = 1.f;

	return S_OK;
}

_int CHumanoid_Clown::Tick(_double dTimeDelta)
{
	Update_UI(dTimeDelta);

	if (m_bPause)
		dTimeDelta = 0.0;

	if (m_eState == OBJSTATE::DEAD)
	{
		Create_DeathEffect();
		return (_int)m_eState;
	}

	Update_LeftArm();

	Play_Pattern(dTimeDelta);

	Update_Collider();

	Update_Force(dTimeDelta, m_pNavigation);

	Update_CollisionTime(dTimeDelta);

	return (_int)m_eState;
}

_int CHumanoid_Clown::LateTick(_double dTimeDelta)
{
	if (m_tObjectStatusDesc.fHp <= 0.f)
		m_eState = OBJSTATE::DEAD;

	m_bCulling = true;
	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
	{
		m_bCulling = false;
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}
	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return 0;
}

HRESULT CHumanoid_Clown::Render()
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



	return S_OK;
}

HRESULT CHumanoid_Clown::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, L"Humanoid_Clown", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Left_Hand", "bone304");
	//m_pModel->Add_RefNode(L"Left_Hand", "bone306");

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
	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHumanoid_Clown::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	

	return S_OK;

}

_bool CHumanoid_Clown::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CHumanoid_Clown::Collision(CGameObject * pGameObject)
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
	else
	{

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CHumanoid_Clown::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET* pPacket = (PACKET*)pMessage;
		_uint i = *(_uint*)pPacket->pData;
		Set_State(OBJSTATE::DEAD);
	}
}

_int CHumanoid_Clown::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CHumanoid_Clown::Update_UI(_double dDeltaTime)
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
	return S_OK;
}

HRESULT CHumanoid_Clown::Create_DeathEffect()
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

void CHumanoid_Clown::Chase(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::WALK)
	{
		if (m_fDistance > 0.1f)
		{
			m_fRatio += (_float)(TimeDelta * 0.5f);

			// 플레이어 방향따라 회전하는 애니
			CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

			CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

			if (pPlayerTransform == nullptr)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}
			RELEASE_INSTANCE(CGameInstance);

			_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

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
				m_bChase = true;
			}
		}

		else
			m_fRatio = 0.f;
	}
}


void CHumanoid_Clown::Check_Target1(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	if (pPlayerTransform == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vPlayerPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;

	if (fDist > 8.f &&  m_eDanceState == DANCE_STATE::NONE && m_iAnimIndex != 9)
	{
		m_eAnimState = M_ANIMSTATE::IDLE;
	}

	if (fDist <= 8.f && m_eDanceState == DANCE_STATE::NONE)
	{
		m_eAnimState = M_ANIMSTATE::WALK;
	}

	if (fDist <= 2.f)
	{
		m_eAnimState = M_ANIMSTATE::START_DANCE;
	}



}

void CHumanoid_Clown::Check_AnimState1(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::IDLE)
	{
		m_iAnimIndex = CLOWN_IDLE;
	}

	if (m_eAnimState == M_ANIMSTATE::START_DANCE)
	{
		if (m_iAnimIndex != CLOWN_DANCE_BEGIN && m_eDanceState == DANCE_STATE::NONE)
		{
			m_iAnimIndex = CLOWN_DANCE_BEGIN;
			m_eDanceState = DANCE_STATE::BEGIN;
		}

		if (0.9 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == CLOWN_DANCE_BEGIN)
		{
			m_iAnimIndex = CLOWN_DANCING;
			m_eDanceState = DANCE_STATE::DOING;
		}

		if (0.95 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == CLOWN_DANCING)
		{
			m_iAnimIndex = CLOWN_DANCE_END;
			m_eAnimState = M_ANIMSTATE::IDLE;
			m_eDanceState = DANCE_STATE::NONE;
			m_bDanceCool = true;
		}
	}
	if (m_eAnimState == M_ANIMSTATE::WALK)
	{
		m_iAnimIndex = CLOWN_WALK;
	}

	if (m_iAnimIndex == CLOWN_IDLE || m_iAnimIndex == CLOWN_WALK)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);

	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Clown::Check_Times1(_double TimeDelta)
{
	if (m_bDanceCool)
	{
		m_fDanceCoolTime += TimeDelta;

		if (m_fDanceCoolTime >= 2.5)
			m_bDanceCool = false;
	}
}

void CHumanoid_Clown::Check_Target2(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	if (pPlayerTransform == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}
	RELEASE_INSTANCE(CGameInstance);

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMonsterPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vDistance = vPlayerPos - vMonsterPos;

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	m_fDistance = fDist;

	m_eAnimState = M_ANIMSTATE::CONFETTI;
}

void CHumanoid_Clown::Check_AnimState2(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::CONFETTI)
	{
		m_iAnimIndex = CLOWN_CONFETTI;
	}

	if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == CLOWN_CONFETTI
		&& m_pModel->Get_LinkMaxRatio() > m_pModel->Get_PlayTimeRatio())
	{
		if (!m_bCreateOnce1)
		{
			//for confetti effect;
			Create_Comfetti();
			m_bCreateOnce1 = true;
			m_bCreateOnce2 = false;
		}
	}

	if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == CLOWN_CONFETTI)
	{
		if (!m_bCreateOnce2)
		{
			//for Confetti Effect
			Create_Comfetti();
			m_bCreateOnce1 = false;
			m_bCreateOnce2 = true;
		}
	}

	if (m_bTurn)
	{
		m_dTurningTime += TimeDelta;
		m_pTransform->Rotation_Axis(TimeDelta * 1.2f);


		if (m_dTurningTime >= 1.0)
		{
			m_dTurningTime = 0.0;
			m_bTurn = false;
		}
	}

	if (m_iAnimIndex == CLOWN_CONFETTI || m_iAnimIndex == CLOWN_WALK)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);

	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Clown::Check_Times2(_double TimeDelta)
{
	if(!m_bTurn)
	m_dTurningCoolTime += TimeDelta;

	if (m_dTurningCoolTime >= 3.0)
	{
		m_bTurn = true;
		m_dTurningCoolTime = 0.0;
	}
}

void CHumanoid_Clown::Update_LeftArm()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Left_Hand");
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
}

void CHumanoid_Clown::Gain_Pattern()
{
	int iNum = rand() % 4;

	if (iNum == 0 || iNum == 1 || iNum ==2)
	{
		m_ePattern = M_PATTERN::PATTERN_1;
	}
	else if (iNum == 3)
	{
		m_ePattern = M_PATTERN::PATTERN_2;
	}
}

void CHumanoid_Clown::Play_Pattern(_double TimeDelta)
{
	if (m_bCulling == false)
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
}

void CHumanoid_Clown::Pattern_1(_double TimeDelta)
{
	if ((_uint)m_fForce <= 0)
	{
		Check_Target1(TimeDelta);
		Chase(TimeDelta);
	}
	Check_AnimState1(TimeDelta);
}

void CHumanoid_Clown::Pattern_2(_double TimeDelta)
{
	if ((_uint)m_fForce <= 0)
	{
		Check_Target2(TimeDelta);
		Check_Times2(TimeDelta);
	}
	Check_AnimState2(TimeDelta);
}

CHumanoid_Clown * CHumanoid_Clown::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CHumanoid_Clown* pInstance = new CHumanoid_Clown(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CHumanoid_Clown::Clone(void * pArg)
{
	CHumanoid_Clown* pInstance = new CHumanoid_Clown(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHumanoid_Clown::Free()
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

HRESULT CHumanoid_Clown::Create_Comfetti()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 3.f; // turn off it when i want;
	tPacket.tEffectMovement.fJumpPower = 0;
	tPacket.tEffectMovement.fSpeed = 0;
	tPacket.tEffectMovement.iJumpCount = 0;
	tPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	memcpy(&tPacket.tEffectMovement.vPosition, &m_LArmPos.m[3], sizeof(_float4));

	//tPacket.tEffectMovement.vPosition = _vPosition;

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Clown_Comfetti.dat", &tPacket);

	return S_OK;
}
