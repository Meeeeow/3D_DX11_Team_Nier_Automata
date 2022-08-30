#include "stdafx.h"
#include "LongKatana.h"
#include "UI_HUD_Damage.h"
#include "EffectFactory.h"
#include "AnimationTrail.h"
#include "CameraMgr.h"

const _double CLongKatana::LONGKATANA_POWERATTACK3_NONLERP_RATIO = 0.20693;
const _double CLongKatana::LONGKATANA_POWERATTACK3_RESTART_LERP_RATIO = 0.2509;
CLongKatana::CLongKatana(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CLongKatana::CLongKatana(const CLongKatana & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CLongKatana::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CLongKatana::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
	{
		return E_FAIL;
	}

	m_iAnimation = 0;

	m_pModel->Set_CurrentAnimation(m_iAnimation);

	m_bCollision = true;

	m_fForce = 1.8f;
	m_tObjectStatusDesc.iAtt = 100;
	Check_Pause();


	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CGameObject* pObj = nullptr;
	CAnimationTrail::TRAIL_DESC eTrailDesc;
	eTrailDesc.iNumTrails = LONGKATANA_TRAIL_NUMBER;
	pGameInstance->Add_GameObjectToLayerWithPtr((_uint)LEVEL::STATIC, L"Prototype_GameObject_AnimationTrail", L"Effect_Layer", &pObj, &eTrailDesc);

	if (nullptr == pObj)
		assert(false);

	m_pTrail = pObj;
	Safe_AddRef(m_pTrail);

	m_pTrail->Set_State(OBJSTATE::DISABLE);

	m_pModel->Compute_LongestTwoPoint();
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_FarPoint1(m_pModel->Get_FarPoint1());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_FarPoint2(m_pModel->Get_FarPoint2());

	return S_OK;
}

_int CLongKatana::Tick(_double TimeDelta)
{
	if (nullptr == m_pModel)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Set_Animation(m_iAnimation, false);
	m_pModel->Synchronize_Animation(m_pTransform);

	Check_Independence(TimeDelta);

	Update_HitType();
	Update_Collider();

	Update_Effects(TimeDelta);

	m_pNavigation->Update(XMMatrixIdentity());

	return (_int)m_eState;
}

_int CLongKatana::LateTick(_double TimeDelta)
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

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pAttackBox);

	return _int();
}

HRESULT CLongKatana::Render()
{
	if (nullptr == m_pModel)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint		iNumMeshContainers = m_pModel->Get_NumMeshContainers();

	if (FAILED(m_pModel->Bind_Buffers()))
		return E_FAIL;


	_int iPassIndex = static_cast<_uint>(MESH_PASS_INDEX::ANIM);
	switch (m_eDissolveState)
	{
	case Client::DISSOLVE_STATE::DISSOLVE_UP:
	case Client::DISSOLVE_STATE::DISSOLVE_DOWN:
		m_pModel->SetUp_RawValue("g_fDissolve", &m_fDissolveVal, sizeof(_float));
		if (FAILED(m_pModel->SetUp_Texture("g_texDissolve", m_pDissolveTexture->Get_SRV(0))))
			return E_FAIL;
		iPassIndex = static_cast<_uint>(MESH_PASS_INDEX::WEAPON_DISSOLVE);
		break;
	case Client::DISSOLVE_STATE::DISSOLVE_IDLE:
	case Client::DISSOLVE_STATE::DISSOLVE_COUNT:
	default:
		iPassIndex = static_cast<_uint>(MESH_PASS_INDEX::ANIM);
		break;
	}

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		m_pModel->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");

		m_pModel->Render(i, iPassIndex);
	}

	return S_OK;
}
void CLongKatana::Update_HitType()
{
	if (m_pModel)
	{
		if (!m_pModel->Get_Blend())
		{
			m_iTargetAnimation = m_pModel->Get_CurrentAnimation();

			if (0 == m_iTargetAnimation)
			{
				m_eHitType = HIT::NONE;
				return;
			}

			m_eHitType = HIT::SINGLE;

			if (6 == m_pModel->Get_CurrentAnimation())
			{	
				if (0.2 <= m_pModel->Get_PlayTimeRatio())
					m_iTargetAnimation = 1;
			}
			else if (5 == m_pModel->Get_CurrentAnimation())
			{
				if (0.58 <= m_pModel->Get_PlayTimeRatio())
					m_iTargetAnimation = 1;
			}
			else if (19 == m_pModel->Get_CurrentAnimation())
			{
				if (0.56 <= m_pModel->Get_PlayTimeRatio())
					m_iTargetAnimation = 1;
			}
			else if (28 == m_pModel->Get_CurrentAnimation())
			{
				if (0.13 <= m_pModel->Get_PlayTimeRatio())
					m_iTargetAnimation = 1;
			}
		}
	}
}
HRESULT CLongKatana::Update_Collider()
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
void CLongKatana::Update_Matrix(_matrix Matrix)
{
	m_pTransform->Set_WorldMatrix(Matrix);
}

void CLongKatana::Set_Animation(_uint iIndex)
{
	if (m_bIndependence)
	{
		if (0 != iIndex)
		{
			m_iAnimation = 0;
			m_pModel->Set_NoneBlend();
			
			m_pModel->Set_Animation(0, false);
			m_pModel->Update_CombinedTransformationMatrix(ONETICK_TIME);
		}
	}

	m_iAnimation = iIndex;
}

void CLongKatana::Set_WeaponEquip(WEAPONEQUIP eEquip)
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

const _bool & CLongKatana::IsIndependence() const
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bIndependence;
}

void CLongKatana::Check_Independence(_double TimeDelta)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (6 == iCurAnimIndex)
	{
		_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
		if (LONGKATANA_POWERATTACK3_NONLERP_RATIO <= PlayTimeRatio)
		{
			if (LONGKATANA_POWERATTACK3_RESTART_LERP_RATIO <= PlayTimeRatio)
			{
				m_bPlayerNonLerp = false;
				m_bIndependence = true;
			}
			else
			{
				m_bPlayerNonLerp = true;
				m_bIndependence = false;
			}

			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = 0;
				m_pModel->Set_NoneBlend();

				m_bIndependence = false;
				m_bPlayerNonLerp = false;

				m_eWeaponState = WEAPONSTATE::IDLE;
				Set_Dissolve();
			}
		}
	}
}

_uint CLongKatana::Get_ModelCurrentAnim()
{
	if (nullptr == m_pModel)
		return 0;

	return m_pModel->Get_CurrentAnimation();
}

_double CLongKatana::Get_PlayTimeRatio()
{
	if (nullptr == m_pModel)
		return 0.0;

	return m_pModel->Get_PlayTimeRatio();
}

_bool CLongKatana::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CLongKatana::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		if (m_eWeaponState != WEAPONSTATE::IDLE && m_eWeaponState != WEAPONSTATE::NONE)
		{
			_double PlayRatio = m_pModel->Get_PlayTimeRatio();

			if ((m_pModel->Get_CollisionMinRatio() <= PlayRatio && m_pModel->Get_CollisionMaxRatio() >= PlayRatio) || 1 == m_iTargetAnimation)
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

					CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

					pCamMgr->Set_MainCam_ShakeHitBig_True();

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

void CLongKatana::Notify(void * pMessage)
{
}

_int CLongKatana::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CLongKatana::Create_Spark()
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
	//XMStoreFloat4(&CurrentPos, m_pTransform->Get_State(CTransform::STATE::POSITION));
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

HRESULT CLongKatana::Create_Decoration_Effect()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

	tPacket.ulID = CHECKSUM_EFFECT_KATANA_DECORATION_EFFECT;
	m_pDecorationEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\LongKatana_Decoration_Effect.dat", &tPacket);
	if (!m_pDecorationEffect)
		assert(FALSE);
	Safe_AddRef(m_pDecorationEffect);

	Update_Decoration_Effect_Transform();

	return S_OK;
}

void CLongKatana::Delete_Decoration_Effect()
{
	if (!m_pDecorationEffect) return;
	m_pDecorationEffect->Set_State(OBJSTATE::DEAD);
	Safe_Release(m_pDecorationEffect);
	m_pDecorationEffect = nullptr;
}

BOOL CLongKatana::Get_IfEffectIs()
{
	if (m_pDecorationEffect) return true;
	return false;
}

void CLongKatana::Start_Trail()
{
	if (nullptr == m_pTrail)
	{
		return;
	}

	m_pTrail->Set_State(OBJSTATE::ENABLE);


	// Init AllPosition
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Add_BoneMatrices(m_pModel->Get_BoneMatrices());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MaxBlendIndex(m_pModel->Get_MaxBlendIndex());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MinBlendIndex(m_pModel->Get_MinBlendIndex());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MaxBlendWeight(m_pModel->Get_MaxBlendWeight());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MinBlendWeight(m_pModel->Get_MinBlendWeight());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_WorldMatrix(m_pTransform->Get_WorldMatrix4x4());
	dynamic_cast<CAnimationTrail*>(m_pTrail)->Init_TrailPosition();
}

void CLongKatana::End_Trail()
{
	m_pTrail->Set_State(OBJSTATE::DISABLE);
}

void CLongKatana::Update_Effects(_double _dTimeDelta)
{
	Update_Decoration_Effect_Transform();
	Update_Trail();

	switch (m_eDissolveState)
	{
	case DISSOLVE_STATE::DISSOLVE_UP: // Disappear
		m_fDissolveVal += m_fDissolveSpeed * static_cast<_float>(_dTimeDelta);
		if (m_fDissolveVal > 1.f)
		{
			m_eDissolveState = DISSOLVE_STATE::DISSOLVE_DOWN;
			Set_WeaponState(WEAPONSTATE::IDLE);
			Set_Animation(0);
			Set_Independence(false);
			Set_PlayerNonLerp(false);
		}
		break;
	case DISSOLVE_STATE::DISSOLVE_DOWN: // Appear
		m_fDissolveVal -= m_fDissolveSpeed * static_cast<_float>(_dTimeDelta);
		if (m_fDissolveVal <= 0.f)
		{
			m_eDissolveState = DISSOLVE_STATE::DISSOLVE_IDLE;
		}

		break;
	case DISSOLVE_STATE::DISSOLVE_IDLE:
		m_fDissolveVal = 0.f;
	default:
		break;
	}
}

void CLongKatana::Update_Decoration_Effect_Transform()
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

void CLongKatana::Update_Decoration_Effect_Redner()
{
	if (m_fAlpha != 0.f)
		m_pDecorationEffect->Set_State(OBJSTATE::ENABLE);
	else
		m_pDecorationEffect->Set_State(OBJSTATE::DISABLE);
}

void CLongKatana::Update_Trail()
{
	if (m_pTrail)
	{
		if (m_pTrail->Get_State() == OBJSTATE::ENABLE)
		{
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Add_BoneMatrices(m_pModel->Get_BoneMatrices());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MaxBlendIndex(m_pModel->Get_MaxBlendIndex());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MinBlendIndex(m_pModel->Get_MinBlendIndex());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MaxBlendWeight(m_pModel->Get_MaxBlendWeight());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_MinBlendWeight(m_pModel->Get_MinBlendWeight());
			//dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_WorldMatrixTranspose(m_pTransform->Get_WorldMatrix4x4Transpose());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Set_WorldMatrix(m_pTransform->Get_WorldMatrix4x4());
			dynamic_cast<CAnimationTrail*>(m_pTrail)->Update_Position();
		}
	}
}


HRESULT CLongKatana::SetUp_Components()
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
 	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_LONGKATANA, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Handle", "bone000");			//
	// For Trail
	m_pModel->Set_IsSaveBoneMatrices(true);

	/* For.Com_Navigation*/
	_uint		iCurrentIndex = 0;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	/* For.COM_KEY_ATTACKBOX */
	CCollider::tagColliderDesc		ColliderDesc;

	ColliderDesc.vScale = _float3(1.5f, 1.5f, 1.5f);
	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.vOrientation = _float4(0.0f, 0.0f, 0.0f, 1.f);
	ColliderDesc.fRadius = m_pModel->Get_Radius();

	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_DissolveTexture*/
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;
	return S_OK;
}

HRESULT CLongKatana::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLongKatana::SceneChange_AmusementPark(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Init_Index(iNavigationIndex);

	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

HRESULT CLongKatana::SceneChange_OperaBackStage(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, OPERA_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}
	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Init_Index(iNavigationIndex);

	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

HRESULT CLongKatana::SceneChange_ZhuangziStage(const _vector & vPosition, const _uint & iNavigationIndex)
{
	/*Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		m_mapComponents.erase(iter);
	}*/

	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Init_Index(iNavigationIndex);

	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

HRESULT CLongKatana::SceneChange_ROBOTGENERAL(const _vector & vPosition, const _uint & iNavigationIndex)
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, ENGELS_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release(iter->second);
		m_mapComponents.erase(iter);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Init_Index(iNavigationIndex);

	Delete_Decoration_Effect();
	Create_Decoration_Effect();

	return S_OK;
}

CLongKatana * CLongKatana::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLongKatana* pGameInstance = new CLongKatana(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CLongKatana");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CLongKatana::Clone(void * pArg)
{
	CLongKatana* pGameInstance = new CLongKatana(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CLongKatana");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CLongKatana::Free()
{
	__super::Free();

	Safe_Release(m_pTrail);
	Safe_Release(m_pDecorationEffect);

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);

	Safe_Release(m_pAttackBox);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pDissolveTexture);
}
