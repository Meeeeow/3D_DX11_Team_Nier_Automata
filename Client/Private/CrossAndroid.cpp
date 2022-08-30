#include "stdafx.h"
#include "..\Public\CrossAndroid.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"
#include "UI_HUD_DirectionIndicator.h"
#include "EffectFactory.h"
#include "MeshEffect.h"
#include "Beauvoir.h"

CCrossAndroid::CCrossAndroid(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CCrossAndroid::CCrossAndroid(const CCrossAndroid & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CCrossAndroid::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCrossAndroid::NativeConstruct(void * pArg)
{
	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_vector vPos = (*(_vector*)pArg);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
	}

	m_bCollision = false;

	m_eState = OBJSTATE::DISABLE;
	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCCrossAndroid_InstanceID = OriginID_CrossAndroid;
	m_iObjID = iCCrossAndroid_InstanceID++;

	m_tObjectStatusDesc.fHp = 200;
	m_tObjectStatusDesc.fMaxHp = 200;
	m_tObjectStatusDesc.iAtt = 1;

	m_fHitRecovery = 100.f;

	m_iAnimation = 6;
	m_bContinue = false;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	Check_Pause();

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	m_CreateDelayTime = 8.0 + rand() % 7;

	return S_OK;
}

_int CCrossAndroid::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DISABLE == m_eState)
		return (_int)m_eState;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (OBJSTATE::DEAD == m_eState)
	{
		Check_Beauvoir();
		return (_int)m_eState;
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	Check_Landing();

	m_pModel->Set_Animation(m_iAnimation);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Update_Collider();

	Update_Force(dTimeDelta);
	Update_CollisionTime(dTimeDelta);
	Update_UI(dTimeDelta);

	Check_Create(dTimeDelta);

	if (m_bAppear)
	{
		m_dAppear -= dTimeDelta;
		if (m_dAppear <= 0.0)
		{
			m_bAppear = false;
			m_dAppear = 0.0;
		}
	}

	return (_int)m_eState;
}

_int CCrossAndroid::LateTick(_double dTimeDelta)
{
	if (OBJSTATE::DISABLE == m_eState)
		return (_int)m_eState;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_tObjectStatusDesc.fHp <= 0.f)
		m_eState = OBJSTATE::DEAD;

	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);
	if (CGameInstance::Get_Instance()->Culling(m_pTransform->Get_State(CTransform::STATE::POSITION), m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return _int();
}

HRESULT CCrossAndroid::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
		_float3 vRimColor = _float3(0.f, 0.f, 0.f);

		if (m_bAppear)
		{
			if (i == 0) {
				_float4 vColor = CProportionConverter()(_GOLDENROD, 75.f);
				vRimColor = _float3(vColor.x, vColor.y, vColor.z);
			}
			else {
				_float4 vColor = CProportionConverter()(_DARKORCHID, 80.f);
				vRimColor = _float3(vColor.x, vColor.y, vColor.z);
			}
		}
		m_pModel->SetUp_RawValue("g_vRimLightColor", &vRimColor, sizeof(_float3));
		m_pModel->Render(i, m_iPassIndex);
	}

	return S_OK;
}

void CCrossAndroid::Check_Create(_double TimeDelta)
{
	m_CreateTime += TimeDelta;

	if (m_CreateTime >= m_CreateDelayTime)
	{
		_float fAngle = 0.0f;
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		m_fAngleRight += 10.f;

		for (_uint i = 0; i < 16; ++i)
		{
			CEffectFactory* pFactory = CEffectFactory::Get_Instance();
			EFFECT_PACKET tPacket;
			tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
			tPacket.fMaxLifeTime = BEAUVOIR_NAVIBULLET_MAX_LIFETIME;
			tPacket.tEffectMovement.fSpeed = CROSSANDROID_NAVIBULLET_SPEED;
			tPacket.tEffectMovement.fJumpPower = 0.f;
			tPacket.tEffectMovement.iJumpCount = 0;

			_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fAngle));
			_float4x4 Rotate4x4;

			XMStoreFloat4x4(&Rotate4x4, RotateMatrix);
			_float4 v4Direction = *(_float4*)&Rotate4x4.m[2][0];

			RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT)), XMConvertToRadians(m_fAngleRight));
			_vector vDirection = XMLoadFloat4(&v4Direction);

			vDirection = XMVector3Normalize(XMVector3TransformNormal(vDirection, RotateMatrix));

			tPacket.tEffectMovement.vDirection = { XMVectorGetX(vDirection), XMVectorGetY(vDirection), XMVectorGetZ(vDirection) };

			XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
			// Offset to lift up bullet
			tPacket.tEffectMovement.vPosition.y += 2.f;

			tPacket.ulID = CHECKSUM_EFFECT_ELECTRIC_DONUT;
			pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut.dat", &tPacket);

			fAngle += (45.f + m_fGapAngle);
		}

		RELEASE_INSTANCE(CGameInstance);

		if (m_fAngleRight >= 10.f)
			m_fAngleRight = -20.f;

		m_CreateTime = 0.0;
	}
}

void CCrossAndroid::Check_Landing()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (6 == iCurAnimIndex)
	{
		if (m_pModel->IsFinish_Animation())
			m_bCollision = true;
	}
}

void CCrossAndroid::Set_Start(_bool bCheck)
{
	if (bCheck)
	{
		m_eState = OBJSTATE::ENABLE;
		m_bAppear = true;
	}

	Look_Target();
}

HRESULT CCrossAndroid::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_MODEL_CROSSANDROID, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;


	CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(0.5f, 1.5f, 0.5f);

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrossAndroid::SetUp_ConstantTable()
{


	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	

	return S_OK;
}

_bool CCrossAndroid::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CCrossAndroid::Collision(CGameObject * pGameObject)
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

void CCrossAndroid::Check_Beauvoir()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CBeauvoir* pBoss = (CBeauvoir*)pGameInstance->Get_GameObjectPtr(pGameInstance->Get_LevelID(), L"Boss", 0);
	if (nullptr == pBoss)
		return;

	pBoss->Set_CrossAndroidNum(pBoss->Get_CrossAndroidNum() - 1);
}

void CCrossAndroid::Look_Target()
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

	_vector vLook = XMVector3Normalize(vPlayerPos - vMyPos);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);

	RELEASE_INSTANCE(CGameInstance);
}

void CCrossAndroid::Notify(void * pMessage)
{
}

_int CCrossAndroid::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CCrossAndroid::Update_UI(_double dDeltaTime)
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
	//if (m_eAnimState != CCrossAndroid::M_ANIMSTATE::IDLE)
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

HRESULT CCrossAndroid::Update_Effect(_float fTimeDelta)
{
	return S_OK;
}

CCrossAndroid * CCrossAndroid::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CCrossAndroid* pGameInstance = new CCrossAndroid(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CCrossAndroid");
	}

	return pGameInstance;
}

CGameObject * CCrossAndroid::Clone(void * pArg)
{
	CCrossAndroid* pGameInstance = new CCrossAndroid(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CCrossAndroid");
	}

	return pGameInstance;
}

void CCrossAndroid::Free()
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
