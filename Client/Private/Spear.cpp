#include "stdafx.h"
#include "..\Public\Spear.h"
#include "UI_HUD_Damage.h"
#include "EffectFactory.h"

const _double CSpear::POWERATTACK_NONLERP_RATIO = 0.1031;
const _double CSpear::POWERATTACK_RESTART_LERP_RATIO = 0.1563;
const _double CSpear::ONETICK_TIME = 0.016;

CSpear::CSpear(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CSpear::CSpear(const CSpear & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CSpear::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CSpear::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
	{
		return E_FAIL;
	}

	ZeroMemory(m_szOwnerLayerTag, sizeof(_tchar) * MAX_PATH);

	m_iAnimation = 1;
	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_bCollision = true;

	m_fForce = 1.6f;
	m_tObjectStatusDesc.iAtt = 50;
	Check_Pause();

	return S_OK;
}

_int CSpear::Tick(_double TimeDelta)
{
	if (nullptr == m_pModel)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;
	
	Check_Teleport(TimeDelta);
	Check_Independence();

	Update_Effects();
	
	m_pModel->Set_Animation(m_iAnimation, false);
	m_pModel->Synchronize_Animation(m_pTransform);

	Update_HitType();
	Update_Collider();

	return (_int)m_eState;
}

_int CSpear::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;
	
	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	if (WEAPONEQUIP::EQUIP != m_eWeaponEquip)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);


	if (nullptr != m_pAttackBox)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);


	return _int();
}

HRESULT CSpear::Render()
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
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, 1);
	}

	return S_OK;
}

void CSpear::Update_HitType()
{
	if (m_pModel)
	{
		if (!m_pModel->Get_Blend())
		{
			m_iTargetAnimation = m_pModel->Get_CurrentAnimation();

			if (1 == m_iTargetAnimation)
				m_eHitType = HIT::NONE;
			else
				m_eHitType = HIT::SINGLE;
		}
	}
}

HRESULT CSpear::Update_Collider()
{
	if (nullptr == m_pTransform || nullptr == m_pModel)
	{
		return E_FAIL;
	}
	if (nullptr != m_pHitBox)
	{
		m_pHitBox->Update(m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pAttackBox)
	{
		m_pAttackBox->Update(m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix());
	}
	if (nullptr != m_pMiddlePoint)
	{
		m_pMiddlePoint->Update(m_pTransform->Get_WorldMatrix());
	}
	return S_OK;
}

void CSpear::Set_Position_Immediately(_vector vPos)
{
	if (nullptr != m_pTransform)
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
}

void CSpear::Set_Teleport(_bool bCheck)
{
	m_bTeleport = bCheck;
}

void CSpear::Check_Teleport(_double TimeDelta)
{
	if (m_bTeleport)
	{
		if (!m_bAppearance)
			m_fAlpha -= (_float)TimeDelta;
		else
			m_fAlpha += (_float)TimeDelta;

		if (0 >= m_fAlpha)
			m_bAppearance = true;
		else if (m_fAlpha >= 1.f && m_bAppearance)
		{
			m_fAlpha = 1.f;

			if (m_bAppearance)
			{
				m_bAppearance = false;
				m_bTeleport = false;
			}
		}
	}
}

void CSpear::Check_Independence()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (95 == iCurAnimIndex)
	{
		_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

		if (POWERATTACK_NONLERP_RATIO <= PlayTimeRatio
			&& POWERATTACK_RESTART_LERP_RATIO > PlayTimeRatio)
			m_bPlayerNonLerp = true;
		
		if (m_pModel->IsFinish_Animation())
		{
			m_iAnimation = 1;
			m_pModel->Set_NoneBlend();

			m_bPlayerNonLerp = false;
			m_bIndependence = false;

			m_eWeaponState = WEAPONSTATE::IDLE;
		}
	}
}

void CSpear::Update_Matrix(_matrix Matrix)
{
	m_pTransform->Set_WorldMatrix(Matrix);
}

void CSpear::Set_Animation(_uint iIndex)
{
	if (m_bIndependence)
	{
		if (1 != iIndex)
		{
			m_iAnimation = 1;
			m_pModel->Set_NoneBlend();

			m_pModel->Set_Animation(1, false);
			m_pModel->Update_CombinedTransformationMatrix(ONETICK_TIME);

			m_bPlayerNonLerp = false;
			m_bIndependence = false;
		}
	}

	m_iAnimation = iIndex;
}

void CSpear::Set_Immediately_Animation(_uint iIndex)
{
	m_pModel->Initialize_RootMotion();
	m_pModel->Initialize_Time();
	
	m_iAnimation = iIndex;
	m_pModel->Set_CurrentAnimation(iIndex);
}

void CSpear::Set_NoneBlend(_uint iIndex)
{
	m_iAnimation = iIndex;

	m_pModel->Set_NoneBlend();
	m_pModel->Set_Animation(iIndex);
}

void CSpear::Set_WeaponEquip(WEAPONEQUIP eEquip)
{
	m_eWeaponEquip = eEquip;

	if (WEAPONEQUIP::NONEQUIP == eEquip)
	{
		if (nullptr != m_pDecorationEffect)
			m_pDecorationEffect->Set_State(OBJSTATE::DISABLE);
	}
	else if (WEAPONEQUIP::EQUIP == eEquip)
	{
		if (nullptr != m_pDecorationEffect)
			m_pDecorationEffect->Set_State(OBJSTATE::ENABLE);
	}

	Update_Decoration_Effect_Transform();
}

_bool CSpear::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CSpear::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		if (m_eWeaponState != WEAPONSTATE::IDLE && m_eWeaponState != WEAPONSTATE::NONE)
		{
			_double PlayRatio = m_pModel->Get_PlayTimeRatio();

			if (m_pModel->Get_CollisionMinRatio() <= PlayRatio && m_pModel->Get_CollisionMaxRatio() >= PlayRatio)
			{
				_uint iDamage = 0;
				if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage))
				{
					pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

					CUI_HUD_Damage::UIDAMAGEDESC	desc;

					desc.eType = CUI_HUD_Damage::DAMAGETYPE::MELEE;

					desc.iDamageAmount = iDamage;
					
					XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));

					CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

					pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

					if (!lstrcmp(m_szOwnerLayerTag, L"Android_9S"))
					{
						pGameObject->Set_Focus(true);			// true: Android9S, false: Player
						pGameObject->Initialize_FocusTime();
					}


					Create_Spark();

					RELEASE_INSTANCE(CGameInstance);
				}
			}
		}
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{

	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CSpear::Notify(void * pMessage)
{
}

_int CSpear::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CSpear::Create_Spark()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	_matrix WorldMatrix = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();

	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), WorldMatrix.r[3]);

	tPacket.ulID = CHECKSUM_EFFECT_KATANA_SPARK;
	CGameObject* pOut = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Katana_Spark.dat", &tPacket);

	//_float4 CurrentPos;


	//_matrix ModelWorldMatrix = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();
	//_float4x4 ModelWorld4x4;
	//XMStoreFloat4x4(&ModelWorld4x4, ModelWorldMatrix);
	//memcpy(&CurrentPos, &ModelWorld4x4.m[3], sizeof(_float4));


	//_float3 vSparkLook = _float3(m_vPrevPostion.x - CurrentPos.x, m_vPrevPostion.y - CurrentPos.y, m_vPrevPostion.z - CurrentPos.z);
	//_vector vecLook = XMLoadFloat3(&vSparkLook);
	//vecLook = XMVector3Normalize(vecLook);
	//_vector vecRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vecLook);
	//_vector vecUp = XMVector3Cross(vecLook, vecRight);
	//CTransform* pTransform = dynamic_cast<CTransform*>(pOut->Get_Component(L"Com_Transform"));
	//pTransform->Set_State(CTransform::STATE::RIGHT, vecRight);
	//pTransform->Set_State(CTransform::STATE::UP, vecUp);
	//pTransform->Set_State(CTransform::STATE::LOOK, vecLook);

	//tPacket.tEffectMovement.vPosition.y += 0.05f;
	//pOut = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Katana_Spark.dat", &tPacket);

	//XMStoreFloat4(&CurrentPos, m_pTransform->Get_State(CTransform::STATE::POSITION));
	//vSparkLook = _float3(m_vPrevPostion.x - CurrentPos.x, m_vPrevPostion.y - CurrentPos.y, m_vPrevPostion.z - CurrentPos.z);
	//vecLook = XMLoadFloat3(&vSparkLook);
	//vecLook = XMVector3Normalize(vecLook);
	//vecRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vecLook);
	//vecUp = XMVector3Cross(vecLook, vecRight);
	//pTransform = dynamic_cast<CTransform*>(pOut->Get_Component(L"Com_Transform"));
	//pTransform->Set_State(CTransform::STATE::RIGHT, vecRight);
	//pTransform->Set_State(CTransform::STATE::UP, vecUp);
	//pTransform->Set_State(CTransform::STATE::LOOK, vecLook);




	return S_OK;
}

HRESULT CSpear::Create_Decoration_Effect()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

	tPacket.ulID = CHECKSUM_EFFECT_KATANA_DECORATION_EFFECT;
	m_pDecorationEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Spear_Decoration_Effect.dat", &tPacket);
	if (!m_pDecorationEffect)
		assert(FALSE);
	Safe_AddRef(m_pDecorationEffect);

	Update_Decoration_Effect_Transform();

	return S_OK;
}

void CSpear::Delete_Decoration_Effect()
{
	if (!m_pDecorationEffect) return;
	m_pDecorationEffect->Set_State(OBJSTATE::DEAD);
	Safe_Release(m_pDecorationEffect);
	m_pDecorationEffect = nullptr;
}

BOOL CSpear::Get_IfEffectIs()
{
	if (m_pDecorationEffect) return true;
	return false;
}

void CSpear::Update_Effects()
{
	Update_Decoration_Effect_Transform();
	Update_LongAttackEffect();
}

void CSpear::Update_Decoration_Effect_Transform()
{
	if (!m_pDecorationEffect) return;

	CTransform* pTransform = dynamic_cast<CTransform*>(m_pDecorationEffect->Get_Component(L"Com_Transform"));

	_matrix ModelWorldMatrix = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();
	_float4x4 ModelWorld4x4;
	XMStoreFloat4x4(&ModelWorld4x4, ModelWorldMatrix);

	_vector vRight;
	_vector vUp;
	_vector vLook;
	_vector vecPos;

	memcpy(&vRight, ModelWorld4x4.m[0], sizeof(_float4));
	memcpy(&vUp, ModelWorld4x4.m[1], sizeof(_float4));
	memcpy(&vLook, ModelWorld4x4.m[2], sizeof(_float4));
	memcpy(&vecPos, ModelWorld4x4.m[3], sizeof(_float4));


	pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	pTransform->Set_State(CTransform::STATE::UP, vUp);
	pTransform->Set_State(CTransform::STATE::POSITION, vecPos);

	if (WEAPONEQUIP::EQUIP == m_eWeaponEquip)
		Update_Decoration_Effect_Redner();
	else
		m_pDecorationEffect->Set_State(OBJSTATE::DISABLE);
}

void CSpear::Update_Decoration_Effect_Redner()
{
	if (m_fAlpha != 0.f)
		m_pDecorationEffect->Set_State(OBJSTATE::ENABLE);
	else
		m_pDecorationEffect->Set_State(OBJSTATE::DISABLE);
}

void CSpear::Update_PrevPosition()
{
	_matrix ModelWorldMatrix = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();
	_float4x4 ModelWorld4x4;
	XMStoreFloat4x4(&ModelWorld4x4, ModelWorldMatrix);
	memcpy(&m_vPrevPostion, &ModelWorld4x4.m[3], sizeof(_float4));
}

HRESULT CSpear::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_SPEAR, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::DESC		ColliderDesc;

	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.vOrientation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	ColliderDesc.fRadius = m_pModel->Get_Radius();
	
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &ColliderDesc)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Handle", "bone000");

	return S_OK;
}

HRESULT CSpear::Create_AttackEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.ulID = CHECKSUM_EFFECT_SPEARATTACK;
	_matrix matBoneWorld = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();
	_vector vPosition;

	memcpy(&vPosition, &matBoneWorld.r[3], sizeof(_vector));

	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPosition);
	XMStoreFloat3(&tPacket.tEffectMovement.vDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	//tPacket.tEffectMovement.fSpeed = 9.f;
	//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect.dat", &tPacket, L"Layer_Effect");

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect.dat", &tPacket, L"Layer_Effect");

	return S_OK;
}

HRESULT CSpear::Create_LongAttackEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.ulID = CHECKSUM_EFFECT_SPEARATTACK_LONG;
	_matrix matBoneWorld = m_pModel->Get_BoneMatrix(L"Handle") * m_pTransform->Get_WorldMatrix();
	_vector vPosition;

	memcpy(&vPosition, &matBoneWorld.r[3], sizeof(_vector));

	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, vPosition);
	XMStoreFloat3(&tPacket.tEffectMovement.vDirection, m_pTransform->Get_State(CTransform::STATE::LOOK));
	//tPacket.tEffectMovement.fSpeed = 9.f;
	//pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect.dat", &tPacket, L"Layer_Effect");

	m_pLongAttackEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\SpearAttackEffect_Long.dat", &tPacket, L"Layer_Effect");
	if (nullptr == m_pLongAttackEffect)
	{
		assert(false);
	}

	Safe_AddRef(m_pLongAttackEffect);

	return S_OK;
}

void CSpear::Delete_LongAttackEffect()
{
	if (nullptr !=m_pLongAttackEffect)
	{
		m_pLongAttackEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pLongAttackEffect);
		m_pLongAttackEffect = nullptr;
	}

}

void CSpear::Update_LongAttackEffect()
{
	if (nullptr != m_pLongAttackEffect)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pLongAttackEffect->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, m_pTransform->Get_State(CTransform::STATE::POSITION));
		pTransform->Set_State(CTransform::STATE::LOOK, m_pTransform->Get_State(CTransform::STATE::LOOK));
	}

}

HRESULT CSpear::SceneChange_AmusementPark(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}


HRESULT CSpear::SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex)
{
	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

HRESULT CSpear::SceneChange_ZhuangziStage(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

HRESULT CSpear::SceneChange_ROBOTGENERAL(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

CSpear * CSpear::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSpear* pGameInstance = new CSpear(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CSpear");
	}

	return pGameInstance;
}

CGameObject * CSpear::Clone(void * pArg)
{
	CSpear* pGameInstance = new CSpear(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CSpear");
	}

	return pGameInstance;
}

void CSpear::Free()
{
	__super::Free();

	Safe_Release(m_pDecorationEffect);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pAttackBox);
	Safe_Release(m_pLongAttackEffect);
}
