#include "stdafx.h"
#include "..\Public\Beauvoir_FireMissile.h"
#include "EffectFactory.h"
#include "UI_HUD_Damage.h"
#include "Trail.h"

CBeauvoir_FireMissile::CBeauvoir_FireMissile(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CBeauvoir_FireMissile::CBeauvoir_FireMissile(const CBeauvoir_FireMissile & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CBeauvoir_FireMissile::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CBeauvoir_FireMissile::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		FIRE_MISSILE_DESC* pDesc = (FIRE_MISSILE_DESC*)pArg;
		m_pTransform->Set_State(CTransform::STATE::POSITION, pDesc->vPosition);
		m_pTransform->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 0.f));
		m_pTransform->LookAt_Direction(pDesc->vDirection);
	}

	_uint iRand = rand() % 2;
	m_fRandomY = ((rand() % 10) * 0.1f) + 1.f;

	if (iRand)
		m_fRandomY *= -1.f;
	Check_Pause();

	m_bMonsterAttack = true;

	m_bCollision = true;
	m_iTargetAnimation = 2000 + rand() % 100;

	m_tObjectStatusDesc.iAtt = 20;
	m_tObjectStatusDesc.fHp = 1;
	m_tObjectStatusDesc.fMaxHp = 1;
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CGameObject* pGameObj = nullptr;
	 

	CTrail::TRAIL_DESC tTrailDesc;
	tTrailDesc.iNumTrails = MISSILE_TRAIL_NUMBER;
	pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_GameObject_Trail", L"Layer_Effect", &m_pTrail, &tTrailDesc);
	if (nullptr == m_pTrail)
		assert(false);
	Safe_AddRef(m_pTrail);
	Start_Trail();
	return S_OK;
}

_int CBeauvoir_FireMissile::Tick(_double dTimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
	{
		return static_cast<_int>(m_eState);
	}

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_eState != m_ePrevState)
	{
		if (OBJSTATE::DISABLE == m_eState
			&& OBJSTATE::ENABLE == m_ePrevState)
		{
			Create_Explosion();
			if (nullptr != m_pTrail)
			{
				m_pTrail->Set_State(OBJSTATE::DEAD);
				Safe_Release(m_pTrail);
				m_pTrail = nullptr;
			}
		}
		m_ePrevState = m_eState;
	}

	if (OBJSTATE::DISABLE == m_eState)
	{
		m_bCollision = false;
		return static_cast<_int>(m_eState);
	}

	if (m_bPause)
		dTimeDelta = 0.0;

	Check_DeleteTime(dTimeDelta);
	Go_Direction(dTimeDelta);

	Update_Collider();
	Update_Trail();
	if (Is_UnderNavigationMesh())
	{
		Set_State(OBJSTATE::DISABLE);
	}




	return static_cast<_int>(OBJSTATE::ENABLE);
}

_int CBeauvoir_FireMissile::LateTick(_double dTimeDelta)
{
	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (m_eState == OBJSTATE::DISABLE)
	{
		return static_cast<_int>(OBJSTATE::DISABLE);
		Create_Explosion();
	}
	if (m_bPause)
		dTimeDelta = 0.0;

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	return _int();
}

HRESULT CBeauvoir_FireMissile::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;

	HRESULT hRes = E_FAIL;
	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		hRes = m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		hRes = m_pModel->Render(i, static_cast<_int>(MESH_PASS_INDEX::NONANIM));
	}

	return S_OK;
}

void CBeauvoir_FireMissile::Check_DeleteTime(_double TimeDelta)
{
	m_DeleteTime += TimeDelta;

	if (m_DeleteTime > 5.0)
		m_eState = OBJSTATE::DISABLE;
}

void CBeauvoir_FireMissile::Go_Direction(_double TimeDelta)
{
	if (m_FirstSlowTime > 0.7)
		m_LookLerpTime += TimeDelta * 8;

	m_FirstSlowTime += TimeDelta;

	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if (m_LookLerpTime >= 1.0)
	{
		m_LookLerpTime = 0.0;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
		if (nullptr == pTransform)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);
		vPlayerPos = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fRandomY);
		RELEASE_INSTANCE(CGameInstance);

		XMStoreFloat4(&m_vTargetPos, vPlayerPos);
	}

	_vector vDir = (XMLoadFloat4(&m_vTargetPos) - vPosition);

	_float fDist = XMVectorGetX(XMVector3Length(vDir));
	vDir = XMVector3Normalize(vDir);

	if (fDist <= 1.5f)
		m_bLerpStop = true;

	if (m_bLerpStop)
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_fSpeed);
	else
	{
		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorZero();
		_vector vScale = m_pTransform->Get_Scale();

		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_LookLerpTime)));

		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLerpLook));
		vUp = XMVector3Normalize(XMVector3Cross(vLerpLook, vRight));

		vRight *= XMVectorGetX(vScale);
		vUp *= XMVectorGetY(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);

		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_fSpeed);
	}

	_float fY = 0.f;

	if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
	{
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPos = XMVectorSetY(vPos, fY);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
		m_eState = OBJSTATE::DISABLE;
	}
}

void CBeauvoir_FireMissile::Notify(void * pMessage)
{
}

_int CBeauvoir_FireMissile::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CBeauvoir_FireMissile::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::tagTransformDesc tTransformDesc;
	tTransformDesc.bJump = true;
	tTransformDesc.fJumpPower = JUMPPOWER;
	tTransformDesc.fJumpTime = 0.0f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &tTransformDesc)))
		return E_FAIL;

	Engine::CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = 0.5f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	_uint iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Model_Missile"), TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeauvoir_FireMissile::SetUp_ConstantTable()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	_float fFar = pGameInstance->Get_Far();

	m_pModel->SetUp_RawValue("g_matWorld", &m_pTransform->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	m_pModel->SetUp_RawValue("fAlpha", &m_fAlpha, sizeof(_float));

	_int iIsGlow = 1;
	m_pModel->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int));
	_float3 vRimLightColor = _float3(0.9f, 0.f, 0.f);
	m_pModel->SetUp_RawValue("g_vRimLightColor", &vRimLightColor, sizeof(_float3));

	return S_OK;
}

HRESULT CBeauvoir_FireMissile::Create_Explosion()
{

	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = 2.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

	tPacket.ulID = CHECKSUM_EFFECT_MISSILE_EXPLOSION;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\MissileExplosion.dat", &tPacket);



	return S_OK;
}

_bool CBeauvoir_FireMissile::Is_UnderNavigationMesh()
{
	_float fY = 0.f;
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);

	CCell* pSlideOut = nullptr;
	if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
	{
		vPosition = XMVectorSetY(vPosition, fY);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		return true;
	}
	else if (!m_pNavigation->IsOn(vPosition, &pSlideOut))
	{
		// 네비 셀 안에 없을 때
		return true;
	}

	return false;
}

void CBeauvoir_FireMissile::Update_Trail()
{
	if (m_pTrail)
	{
		if (m_pTrail->Get_State() == OBJSTATE::ENABLE)
		{
			_vector vecTemp;
			vecTemp = XMLoadFloat3(&(m_pModel->Get_FarPoint1()));
			vecTemp = XMVectorSetW(vecTemp, 1.f);

			_float3 vMinPoint;
			vecTemp = XMVector3TransformCoord(vecTemp, m_pTransform->Get_WorldMatrix());
			XMStoreFloat3(&vMinPoint, vecTemp);

			vecTemp = XMLoadFloat3(&(m_pModel->Get_FarPoint2()));
			vecTemp = XMVectorSetW(vecTemp, 1.f);

			_float3 vMaxPoint;
			vecTemp = XMVector3TransformCoord(vecTemp, m_pTransform->Get_WorldMatrix());
			XMStoreFloat3(&vMaxPoint, vecTemp);

			dynamic_cast<CTrail*>(m_pTrail)->Add_Position(vMinPoint, vMaxPoint);
		}
	}
}

void CBeauvoir_FireMissile::Start_Trail()
{
	m_pTrail->Set_State(OBJSTATE::ENABLE);


	// Init AllPosition
	_vector vecTemp;
	vecTemp = XMLoadFloat3(&(m_pModel->Get_FarPoint1()));
	vecTemp = XMVectorSetW(vecTemp, 1.f);

	_float3 vMinPoint;
	vecTemp = XMVector3TransformCoord(vecTemp, m_pTransform->Get_WorldMatrix());
	XMStoreFloat3(&vMinPoint, vecTemp);

	vecTemp = XMLoadFloat3(&(m_pModel->Get_FarPoint2()));
	vecTemp = XMVectorSetW(vecTemp, 1.f);

	_float3 vMaxPoint;
	vecTemp = XMVector3TransformCoord(vecTemp, m_pTransform->Get_WorldMatrix());
	XMStoreFloat3(&vMaxPoint, vecTemp);

	dynamic_cast<CTrail*>(m_pTrail)->Init_TrailPosition(vMinPoint, vMaxPoint);
}

_bool CBeauvoir_FireMissile::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (m_eState == OBJSTATE::DISABLE)
		return false;

	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pAttackBox->Collision_SphereToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::SPHERE)
	{
		if (m_pAttackBox->Collision_SphereToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}
	return false;
}

void CBeauvoir_FireMissile::Collision(CGameObject * pGameObject)
{
	if (m_eState == OBJSTATE::DISABLE)
		return;


	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		//_uint iDamage = 0;
		//if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage, m_bMonsterAttack))
		//{
		//	pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

		//	CUI_HUD_Damage::UIDAMAGEDESC	desc;

		//	desc.eType = CUI_HUD_Damage::DAMAGETYPE::RANGE;

		//	desc.iDamageAmount = iDamage;

		//	XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));
		//	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		//	pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

		//	RELEASE_INSTANCE(CGameInstance);

		m_eState = OBJSTATE::DISABLE;
		//Create_Explosion();
	}
		//	m_eState = OBJSTATE::DEAD;
		//	Create_Explosion();
		//}

	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

CBeauvoir_FireMissile * CBeauvoir_FireMissile::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CBeauvoir_FireMissile* pGameInstance = new CBeauvoir_FireMissile(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_FireMissile");
	}

	return pGameInstance;
}

CGameObject * CBeauvoir_FireMissile::Clone(void * pArg)
{
	CBeauvoir_FireMissile* pGameInstance = new CBeauvoir_FireMissile(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{ 
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CBeauvoir_FireMissile");
	}

	return pGameInstance;
}

void CBeauvoir_FireMissile::Free()
{
	__super::Free();

	if (nullptr != m_pTrail)
	{
		m_pTrail->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pTrail);
		m_pTrail = nullptr;
	}
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pModel);

}
