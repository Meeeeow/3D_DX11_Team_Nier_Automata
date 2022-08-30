#include "stdafx.h"
#include "EffectBullet.h"
#include "EffectFactory.h"
#include "UI_HUD_Damage.h"

void CEffectBullet::Update_WithMovement()
{
}

CEffectBullet::CEffectBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CMeshEffect(pDevice, pDeviceContext)
{
}

CEffectBullet::CEffectBullet(const CMeshEffect & rhs)
	: CMeshEffect(rhs)
{
}


CEffectBullet * CEffectBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CEffectBullet* pInstance = new CEffectBullet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CEffectBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectBullet::Clone(void * pArg)
{
	CEffectBullet* pInstance = new CEffectBullet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CEffectBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectBullet::Free()
{
	__super::Free();
}

HRESULT CEffectBullet::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEffectBullet::NativeConstruct(void * pArg)
{
	EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
	m_tDesc = *pDesc;
	m_ulID = pDesc->tEffectPacket.ulID;
	m_fMaxLifeTime = pDesc->tEffectPacket.fMaxLifeTime;
	m_tMovement = pDesc->tEffectPacket.tEffectMovement;
	m_bForMiniGame = pDesc->tEffectPacket.bForMiniGame;

	if (FAILED(__super::NativeConstruct(&pDesc->tGroupDesc)))
		return E_FAIL;

	if (FAILED(SetUp_RestComponents()))
		return E_FAIL;

	memcpy(&m_tMovement, &pDesc->tEffectPacket.tEffectMovement, sizeof(EFFECT_MOVEMENT));

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&m_tMovement.vPosition));

	if (m_pNavigation != nullptr)
	{
		_uint iNaviSize = m_pNavigation->Get_Size();

		if (pDesc->tEffectPacket.iNavIndex == -1)
		{
			if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
				return E_FAIL;
			if (m_pNavigation->Get_Index() >= iNaviSize)
				m_eState = OBJSTATE::DEAD;
		}
		else
		{
			if (pDesc->tEffectPacket.iNavIndex >= static_cast<_int>(iNaviSize))
			{
				m_eState = OBJSTATE::DEAD;
			}
			else if (FAILED(m_pNavigation->Init_Index(pDesc->tEffectPacket.iNavIndex)))
				return E_FAIL;
		}
	}

	_vector vLook = XMLoadFloat3(&pDesc->tEffectPacket.tEffectMovement.vDirection);
	vLook = XMVectorSetW(vLook, 0.f);
	if (0 != pDesc->tEffectPacket.tEffectMovement.fSpeed)
	{
		m_pTransform->LookAt_Direction(vLook);
	}

	m_eHitType = HIT::SINGLE;
	m_iTargetAnimation = 900 + rand() % 100;

	m_tObjectStatusDesc.iAtt = pDesc->tEffectPacket.iAtt;

	m_bCollision = true;

	if (pDesc->tEffectPacket.ulID == CHECKSUM_EFFECT_PODBULLET)
		m_bMonsterAttack = false;
	else if (pDesc->tEffectPacket.ulID == CHECKSUM_EFFECT_POD_BULLET)
		m_bMonsterAttack = false;
	else if (pDesc->tEffectPacket.ulID == CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_BULLET)
		m_bMonsterAttack = false;

	return S_OK;
}

_int CEffectBullet::Tick(_double TimeDelta)
{
	if (m_bMiniGamePause && false == m_bForMiniGame)
		return (_int)m_eState;

	//if (m_iIsEnd > 2)
	if (m_eState == OBJSTATE::DEAD)
	{
		if (!m_bCreatedExplosion)
		{
			Create_Explosion();
		}
		return (_int)OBJSTATE::DEAD;
	}

	//if (m_eState == OBJSTATE::DEAD)
	//{
	//	++m_iIsEnd;
	//}


	if (static_cast<_int>(OBJSTATE::DEAD) == __super::Tick(TimeDelta))
	{
		m_eState = OBJSTATE::DEAD;
		//++m_iIsEnd;

		//Create_Explosion();
		//return (_int)OBJSTATE::DEAD;
	}

	if (m_bPause)
		TimeDelta = 0.0;

	m_fCurLifeTime += static_cast<_float>(TimeDelta);
	if (m_fMaxLifeTime > 0 && m_fMaxLifeTime < m_fCurLifeTime)
	{
		m_eState = OBJSTATE::DEAD;
		//++m_iIsEnd;
		//Create_Explosion();
		//return (_int)OBJSTATE::DEAD;
	}

	m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_tMovement.fSpeed);
	if (m_pTransform->Get_TransformDesc().bJump
		&& m_pTransform->Get_TransformDesc().fJumpPower > 0)
	{
		m_pTransform->Jump(TimeDelta);
	}


	if (Is_UnderNavigatoinMesh())
	{
		if (m_tMovement.iJumpCount > 0)
		{
			--m_tMovement.iJumpCount;
			m_tMovement.fJumpPower -= 0.5f;
			CTransform::DESC tTransformDesc = m_pTransform->Get_TransformDesc();
			tTransformDesc.fJumpTime = 0.f;
			tTransformDesc.fJumpPower -= 0.5f;
			m_pTransform->Set_TransformDesc(tTransformDesc);
		}
		else
		{
			m_eState = OBJSTATE::DEAD;
			//++m_iIsEnd;
		}
	}


	m_pAttackBox->Update(m_pTransform->Get_WorldMatrix());


	return 0;
}

_int CEffectBullet::LateTick(_double TimeDelta)
{
	if (m_bMiniGamePause && false == m_bForMiniGame)
		return (_int)m_eState;

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);
	return __super::LateTick(TimeDelta);
}

HRESULT CEffectBullet::Render()
{
	return __super::Render();
}

HRESULT CEffectBullet::SetUp_RestComponents()
{
	// For Collider
	Set_Collider();

	// For Transform Addtional options
	if (m_pTransform)
	{
		CTransform::tagTransformDesc tTransformDesc;

		tTransformDesc.bJump = (0 < m_tMovement.fJumpPower) ? true : false;
		tTransformDesc.fJumpPower = m_tMovement.fJumpPower;
		tTransformDesc.fJumpTime = 0.0f;
		m_pTransform->Set_TransformDesc(tTransformDesc);
	}
	else
		return E_FAIL;	

	// For NavigationMesh
	_uint iCurrentIndex = 0;
	if (m_tDesc.tEffectPacket.iNavIndex != -2)
	{
		switch (m_tDesc.tEffectPacket.eLevel)
		{
		case LEVEL::STAGE1:
			if (FAILED(Add_Components(static_cast<_int>(m_tDesc.tEffectPacket.eLevel), PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
				return E_FAIL;
			break;
		case LEVEL::ZHUANGZISTAGE:
			if (FAILED(Add_Components(static_cast<_int>(m_tDesc.tEffectPacket.eLevel), PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
				return E_FAIL;
			break;
		case LEVEL::AMUSEMENTPARK:
			if (FAILED(Add_Components(static_cast<_int>(m_tDesc.tEffectPacket.eLevel), PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
				return E_FAIL;
			break;
		case LEVEL::OPERABACKSTAGE:
			if (FAILED(Add_Components(static_cast<_int>(m_tDesc.tEffectPacket.eLevel), PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
				return E_FAIL;
			break;
		case LEVEL::ROBOTGENERAL:
			if (FAILED(Add_Components(static_cast<_int>(m_tDesc.tEffectPacket.eLevel), PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
				return E_FAIL;
			break;
		default:
			break;
		}
	}

	return S_OK;
}

HRESULT CEffectBullet::Set_Collider()
{
	Engine::CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = XMVectorGetX(m_vecIndivMeshEffect[0]->pTransformCom->Get_Scale() * 0.035f * m_tDesc.tEffectPacket.fColliderRadius);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &tColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectBullet::Create_Explosion()
{
	m_bCreatedExplosion = true;

	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));


	switch (m_ulID)
	{
	case CHECKSUM_EFFECT_ENEMYBULLET:
		tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLETEXPLOSION;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\EnemyBulletExplosion.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_ENEMYBULLET_BIG:
		tPacket.ulID = CHECKSUM_EFFECT_ENEMYBULLETEXPLOSION_BIG;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\EnemyBulletExplosion_Big.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_PODBULLET:
		tPacket.ulID = CHECKSUM_EFFECT_PODBULLET_EXPLOSION;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\PODBullet_Explosion.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_ELECTRIC_DONUT:
		tPacket.ulID = CHECKSUM_EFFECT_ELECTRIC_DONUT_EXPLOSION;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\ElectricDonut_Explosion.dat", &tPacket);
		break;
	//case CHECKSUM_EFFECT_BEAVOIR_DONUT_UP:
	//	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_DONUT_DOWN;
	//	pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Donut_UpDown1.dat", &tPacket);
	//	break;
	case CHECKSUM_EFFECT_BEAUVOIR_DONUT_DOWN0:
		tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP1;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up1.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_BEAUVOIR_DONUT_DOWN1:
		tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP2;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Up2.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_POD_LASER0:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_POD_LASER1;
		tPacket.tEffectMovement.vDirection = m_tEffectDesc.tEffectPacket.tEffectMovement.vDirection;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser1.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_BULLET:
		tPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_EXPLOSION;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Explosion.dat", &tPacket);
		break;
	default:
		break;
	}




	return S_OK;
}

_bool CEffectBullet::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
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

void CEffectBullet::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		/*
		*/

		if (!m_bMonsterAttack)
		{
			_uint iDamage = 0;
			if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage, m_bMonsterAttack))
			{
				pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

				CUI_HUD_Damage::UIDAMAGEDESC	desc;

				desc.eType = CUI_HUD_Damage::DAMAGETYPE::RANGE;

				desc.iDamageAmount = iDamage;

				XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));
				CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

				RELEASE_INSTANCE(CGameInstance);
			
				m_eState = OBJSTATE::DEAD;
				Create_Explosion();
			}
		}
		else if (m_bMonsterAttack)
		{
			m_eState = OBJSTATE::DEAD;
			Create_Explosion();
		}
	}

	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		if (HIT::NONE != pGameObject->Get_HitType())
		{
			m_eState = OBJSTATE::DEAD;
			Create_Explosion();
		}
	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

_bool CEffectBullet::Is_UnderNavigatoinMesh()
{
	if (m_pNavigation == nullptr)
		return false;

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
		return true;
	}

	return false;
}
