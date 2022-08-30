#include "stdafx.h"
#include "..\Public\Humanoid_Dragoon.h"
#include "UI_HUD_Hpbar_Monster_Contents.h"


CHumanoid_Dragoon::CHumanoid_Dragoon(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CHumanoid_Dragoon::CHumanoid_Dragoon(const CHumanoid_Dragoon & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CHumanoid_Dragoon::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CHumanoid_Dragoon::NativeConstruct(void * pArg)
{
	//MO_INFO* tInfo = (MO_INFO*)pArg;

	//if (FAILED(Add_Components((_uint)LEVEL::STATIC, tInfo->szModelComName, COM_KEY_MODEL, (CComponent**)&m_pModel)))
	//	return E_FAIL;

	//m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&tInfo->m_matWorld));

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"Dragoon", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	_vector vPos = { 8.f , 0.f , 8.f , 1.f };

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

	m_iPassIndex = 1;

	CCollider::DESC tColliderDesc;

	tColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	tColliderDesc.fRadius = m_pModel->Get_Radius();
	tColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &tColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	m_ePattern = M_PATTERN::PATTERN_1;

	m_eState = OBJSTATE::ENABLE;

	m_bCollision = true;

	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCHumanoid_Dragoon_InstanceID = OriginID_Dragoon;
	m_iObjID = iCHumanoid_Dragoon_InstanceID++;
	Check_Pause();

	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	return S_OK;
}

_int CHumanoid_Dragoon::Tick(_double dTimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
		return (_int)m_eState;

	Update_Collider();

	Update_UI(dTimeDelta);

	Set_Target();

	Play_Pattern(dTimeDelta);


	return (_int)m_eState;
}

_int CHumanoid_Dragoon::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)

		m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);


	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	Check_Pattern_Cool(dTimeDelta);

	RELEASE_INSTANCE(CGameInstance);

	return 0;
}

HRESULT CHumanoid_Dragoon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CHumanoid_Dragoon::Set_Target()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pPlayerTransformCom = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	RELEASE_INSTANCE(CGameInstance);
}

void CHumanoid_Dragoon::Chase(_double TimeDelta)
{
	if (m_eAnimState == M_ANIMSTATE::WALK || m_eAnimState == M_ANIMSTATE::ENGAGE || m_eAnimState == M_ANIMSTATE::SHOOTING)
	{
		if (m_fDistance > 1.f)
		{
			m_fRatio += (_float)(TimeDelta * 0.5f);

			// 플레이어 방향따라 회전하는 애니
			_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
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

		else
			m_fRatio = 0.f;
	}
}

void CHumanoid_Dragoon::Play_Pattern(_double dTimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Pattern_1(dTimeDelta);
	}
}

void CHumanoid_Dragoon::Check_Pattern_Cool(_double TimeDelta)
{
	if (m_ePattern == M_PATTERN::PATTERN_1)
	{
		Check_Times1(TimeDelta);
	}
}

void CHumanoid_Dragoon::Pattern_1(_double TimeDelta)
{
	Check_Target1(TimeDelta);
	Chase(TimeDelta);
	Check_AnimState1(TimeDelta);
}

void CHumanoid_Dragoon::Check_Target1(_double TimeDelta)
{
	m_iAnimIndex = 41;

	if (m_iAnimIndex == 33 || m_iAnimIndex == 0 || m_iAnimIndex == 41 )
		m_pModel->Set_Animation(m_iAnimIndex, true);
	else
		m_pModel->Set_Animation(m_iAnimIndex);
	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);
}

void CHumanoid_Dragoon::Check_AnimState1(_double TimeDelta)
{
		
}

void CHumanoid_Dragoon::Check_Times1(_double TimeDelta)
{
	
}

_bool CHumanoid_Dragoon::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	return _bool();
}

void CHumanoid_Dragoon::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		m_iFlagUI |= FLAG_UI_CALL_HPBAR;
		m_dCallHPBarUITimeDuration = 3.0;

		m_iCrntHP -= 1;
		if (m_iCrntHP < 0)
			m_iCrntHP = 0;

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CHumanoid_Dragoon::Notify(void * pMessage)
{
}

_int CHumanoid_Dragoon::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CHumanoid_Dragoon::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dDeltaTime;

		CUI_HUD_Hpbar_Monster_Contents::UIHPBARMONDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eMonsterType = CUI_HUD_Hpbar_Monster_Contents::MONSTERTYPE::COMMON;
		desc.fCrntHPUV = (_float)m_iCrntHP / (_float)m_iMaxHP;
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

CHumanoid_Dragoon * CHumanoid_Dragoon::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CHumanoid_Dragoon* pInstance = new CHumanoid_Dragoon(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CHumanoid_Dragoon::Clone(void * pArg)
{
	CHumanoid_Dragoon* pInstance = new CHumanoid_Dragoon(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CHumanoid_Dragoon::Free()
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
