#include "stdafx.h"
#include "..\Public\Clown_Fly.h"
#include "GameInstance.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "Player.h"
#include "EffectFactory.h"

int CClown_Fly::m_iPatternNumber = 0;

CClown_Fly::CClown_Fly(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	:CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CClown_Fly::CClown_Fly(const CClown_Fly & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CClown_Fly::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CClown_Fly::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;
	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iClown_Fly_InstanceID = OriginID_ClownFly;
	m_iObjID = iClown_Fly_InstanceID++;

	m_tObjectStatusDesc.fHp = 15;
	m_tObjectStatusDesc.fMaxHp = 15;
	m_tObjectStatusDesc.iAtt = 0;

	m_fHitRecovery = 1.f;

	m_eState = OBJSTATE::DISABLE;

	if (m_iPatternNumber % 2 == 0)
	{
		m_eSide = SIDE::LEFT;
		m_fXDistanceL = (_float)(rand() % 3 + 7);
	}
	else
	{
		m_eSide = SIDE::RIGHT;
		m_fXDistanceR = (_float)(rand() % 2 + 2);
	}

	m_dBulletCoolDownTime = (_double)(rand() % 2 + 2);

	m_bCollision = true;

	return S_OK;
}

_int CClown_Fly::Tick(_double dTimeDelta)
{
	Update_UI(dTimeDelta);

	if (m_bPause)
		dTimeDelta = 0.0;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (m_eState == OBJSTATE::DEAD)
	{
		Create_DeathEffect();
		return (_int)m_eState;
	}

	if (m_eState == OBJSTATE::ENABLE)
	{

		if (!m_bOnce)
		{
			Set_Position_From_Coaster_First();
			m_bOnce = true;
		}

		else
		{
			Set_Position_From_Coaster_Floating(dTimeDelta);
			Check_Target(dTimeDelta);
			Check_AnimState1(dTimeDelta);
		}

		Update_Collider();

		Update_CollisionTime(dTimeDelta);

	}

	return (_int)m_eState;
}

_int CClown_Fly::LateTick(_double dTimeDelta)
{
	if (m_tObjectStatusDesc.fHp <= 0.f)
		m_eState = OBJSTATE::DEAD;

	if (!m_bStart)
		dTimeDelta = 0.0;

	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	if (m_bPause)
		dTimeDelta = 0.0;

	if (m_eState == OBJSTATE::ENABLE)
	{
		Check_Times1(dTimeDelta);

	}

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this); 
	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return 0;
}

HRESULT CClown_Fly::Render()
{
	if (m_eState == OBJSTATE::ENABLE)
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

HRESULT CClown_Fly::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, L"Clown_Fly", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	Engine::CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	_uint iCurrentIndex = 0;

	tColliderDesc.vScale = _float3(0.8f, 0.8f, 0.8f);
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;
	/*if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
	return E_FAIL;*/
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;
	/*if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CClown_Fly::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	return S_OK;
}

HRESULT CClown_Fly::Set_Position_From_Coaster_First()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pCoasterModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", TEXT("Com_Model"), 0);

	_float4 vecLook = pCoasterModel->Get_vecLook();	

	_vector vLook = XMVector3Normalize(XMLoadFloat4(&vecLook));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	CTransform* pCoasterTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", COM_KEY_TRANSFORM, 0);

	_vector vCoasterPos = pCoasterTransform->Get_State(CTransform::STATE::POSITION);

	if (m_eSide == SIDE::LEFT)
	{
		vCoasterPos = vCoasterPos + vUp * 6.f;
		m_fDistance = 6.f;
		vCoasterPos = vCoasterPos - vLook * m_fXDistanceL;

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCoasterPos);

		_vector vRight = XMVector3Cross(vUp, vLook);
		vUp = XMVector3Cross(vLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);

	}
	
	else if (m_eSide == SIDE::RIGHT)
	{
		vCoasterPos = vCoasterPos + vUp * 6.f;
		m_fDistance = 6.f;
		vCoasterPos = vCoasterPos + vLook * m_fXDistanceR;

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCoasterPos);

		vLook = vLook * -1.f;

		_vector vRight = XMVector3Cross(vUp, vLook);
		vUp = XMVector3Cross(vLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);

	}

	m_eAnimState = M_ANIMSTATE::IDLE;
	m_iAnimIndex = FLY_IDLE;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CClown_Fly::Set_Position_From_Coaster_Floating(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pCoasterModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", TEXT("Com_Model"), 0);

	_float4 vecLook = pCoasterModel->Get_vecLook();

	_vector vLook = XMVector3Normalize(XMLoadFloat4(&vecLook));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	CTransform* pCoasterTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", COM_KEY_TRANSFORM, 0);

	_vector vCoasterPos = pCoasterTransform->Get_State(CTransform::STATE::POSITION);

	_float fY = XMVectorGetY(vCoasterPos);

	if (m_eSide == SIDE::LEFT)
	{
		//_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		//_float fMonY = XMVectorGetY(vMyPos);
		//m_fDistance = fMonY - fY;
		
		if (m_fDistance >= 4.f)
		{
			m_fDistance -= (_float)dTimeDelta ;
			m_eAnimState = M_ANIMSTATE::FLOAT_DOWN;
		}

		else if (m_fDistance <= 1.f)
		{
			m_fDistance += (_float)dTimeDelta ;
			m_eAnimState = M_ANIMSTATE::FLOAT_UP;
		}

		else if (m_fDistance > 1.f && m_fDistance < 4.f)
		{
			if (m_eAnimState == M_ANIMSTATE::FLOAT_UP)
				m_fDistance += (_float)dTimeDelta;
			else if (m_eAnimState == M_ANIMSTATE::FLOAT_DOWN)
				m_fDistance -= (_float)dTimeDelta;
		}


		vCoasterPos = vCoasterPos - vLook * m_fXDistanceL;
		vCoasterPos = vCoasterPos + vUp * m_fDistance;

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCoasterPos);

		_vector vRight = XMVector3Cross(vUp, vLook);
		vUp = XMVector3Cross(vLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);

	}
	else if (m_eSide == SIDE::RIGHT)
	{
		//_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		//_float fMonY = XMVectorGetY(vMyPos);
		//m_fDistance = fMonY - fY;

		if (m_fDistance >= 4.f)
		{
			m_fDistance -= (_float)dTimeDelta ;
			m_eAnimState = M_ANIMSTATE::FLOAT_DOWN;
		}

		else if (m_fDistance <= 1.f)
		{
			m_fDistance += (_float)dTimeDelta ;
			m_eAnimState = M_ANIMSTATE::FLOAT_UP;
		}

		else if (m_fDistance > 1.f && m_fDistance < 4.f)
		{
			if (m_eAnimState == M_ANIMSTATE::FLOAT_UP)
				m_fDistance += (_float)dTimeDelta;
			else if (m_eAnimState == M_ANIMSTATE::FLOAT_DOWN)
				m_fDistance -= (_float)dTimeDelta;
		}

		vCoasterPos = vCoasterPos + vLook * m_fXDistanceR;
		vCoasterPos = vCoasterPos + vUp * m_fDistance;

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCoasterPos);

		vLook = vLook * -1.f;

		_vector vRight = XMVector3Cross(vUp, vLook);
		vUp = XMVector3Cross(vLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CClown_Fly::Check_Target(_double TimeDelta)
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
	_vector vNewLook = XMVector3Normalize(vPlayerPos - vMonsterPos);

	vDistance = XMVector3Length(vDistance);

	_float fDist = XMVectorGetX(vDistance);

	if (!m_bFireCool && fDist <= 12.f && m_eAnimState != M_ANIMSTATE::ATTACK)
	{
		m_eLastAnimState = m_eAnimState;
		m_iLastAnimIndex = m_iAnimIndex;
		m_eAnimState = M_ANIMSTATE::ATTACK;
		XMStoreFloat4(&m_vecBulletLook, vNewLook);
	}
}

void CClown_Fly::Check_AnimState1(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::FLOAT_UP)
	{
		m_iAnimIndex = FLY_UP;
	}

	else if (m_eAnimState == M_ANIMSTATE::FLOAT_DOWN)
	{
		m_iAnimIndex = FLY_DOWN;
	}

	else if (m_eAnimState == M_ANIMSTATE::ATTACK)
	{
		m_iAnimIndex = FLY_ATTACK;

		if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == FLY_ATTACK && !m_bFireOnce)
		{
			// create bullet here;
			// use m_vecBulletLook;
			Shoot_Bullet();
			m_bFireOnce = true;
		}

		if (m_pModel->Get_PlayTimeRatio() >= 0.90 && m_pModel->Get_CurrentAnimation() == FLY_ATTACK && m_eAnimState != M_ANIMSTATE::FLOAT_DOWN && m_eAnimState != M_ANIMSTATE::FLOAT_UP)
		{
			m_bFireCool = true;
			m_eAnimState = m_eLastAnimState;
			m_iAnimIndex = (_uint)m_eLastAnimState;

		}
	}

	if (m_iAnimIndex == FLY_DOWN || m_iAnimIndex == FLY_UP)
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);

	m_pModel->Synchronize_Animation(m_pTransform);

}

void CClown_Fly::Check_Times1(_double TimeDelta)
{
	if (m_bFireCool)
	{
		m_dBulletCoolTime += TimeDelta;

		if (m_dBulletCoolTime >= m_dBulletCoolDownTime)
		{
			m_bFireCool = false;
			m_bFireOnce = false;
			m_dBulletCoolTime = 0.0;
		}
	}
}

HRESULT CClown_Fly::Shoot_Bullet()
{
	_float4x4 pMatrix;
	XMStoreFloat4x4(&pMatrix, m_pTransform->Get_WorldMatrix());

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;



	_float3 vDir;
	_vector vLook;
	vLook = XMLoadFloat4(&m_vecBulletLook);
	if (m_eSide == SIDE::LEFT)
	{
		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		CModel* pCoasterModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", TEXT("Com_Model"), 0);

		_float4 vecLook = pCoasterModel->Get_vecLook();

		vLook = XMLoadFloat4(&vecLook);
	}
	XMStoreFloat3(&vDir, vLook);
	_vector vecPos;
	vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	memcpy(&vecPos, pMatrix.m[3], sizeof(_float4));
	_float4 vPos;
	XMStoreFloat4(&vPos, vecPos);

	pEffectBulletPacket.iNavIndex = -2;
	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = MONSTER_BULLET_SPEED;
	if (m_eSide == SIDE::LEFT)
		pEffectBulletPacket.tEffectMovement.fSpeed = 60.f;
	pEffectBulletPacket.tEffectMovement.vDirection = vDir;
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLET;
	pEffectBulletPacket.fMaxLifeTime = 5.f;
	CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\EnemyBullet.dat", &pEffectBulletPacket);

	return S_OK;
}

_bool CClown_Fly::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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
	return false;
}

void CClown_Fly::Collision(CGameObject * pGameObject)
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

		//Create_ElectricityEffect();


	}
	else
	{

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CClown_Fly::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET* pPacket = (PACKET*)pMessage;
		_uint i = *(_uint*)pPacket->pData;
		Set_State(OBJSTATE::DEAD);
	}
}

_int CClown_Fly::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CClown_Fly::Update_UI(_double dDeltaTime)
{
	//if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	//{
	//	m_dCallHPBarUITimeDuration -= dDeltaTime;

	//	CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

	//	desc.iObjID = m_iObjID;
	//	desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::COMMON;
	//	desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
	//	desc.dTimeDuration = m_dCallHPBarUITimeDuration;

	//	XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

	//	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//	pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MONSTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MONSTER_HPBAR, &desc));

	//	RELEASE_INSTANCE(CGameInstance);

	//	if (m_dCallHPBarUITimeDuration < 0)
	//	{
	//		m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
	//	}
	//}
	return S_OK;
}

HRESULT CClown_Fly::Create_DeathEffect()
{
	//CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	//EFFECT_PACKET		pEffectBulletPacket;


	//_vector vecDir = m_pTransform->Get_State(CTransform::STATE::LOOK);
	//_float3 vDir;
	//XMStoreFloat3(&vDir, vecDir);
	//_vector vecPos;
	//vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	//_float4 vPos;
	//XMStoreFloat4(&vPos, vecPos);

	//pEffectBulletPacket.iNavIndex = m_pNavigation->Get_Index();
	//pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	//pEffectBulletPacket.tEffectMovement.fSpeed = 0.f;
	//pEffectBulletPacket.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	//pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_ROBOT_DEATH_EXPLOSION_SPARK4;
	//pEffectBulletPacket.fMaxLifeTime = -1.f;
	//CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Robot_Death_Explosion_Spark4.dat", &pEffectBulletPacket);

	return S_OK;
}

CClown_Fly * CClown_Fly::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CClown_Fly* pInstance = new CClown_Fly(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CClown_Fly::Clone(void * pArg)
{
	CClown_Fly* pInstance = new CClown_Fly(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CClown_Fly::Free()
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
