#include "stdafx.h"
#include "..\Public\ZhuangziWeaponTrail.h"

CZhuangziWeaponTrail::CZhuangziWeaponTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CMeshEffect(pGraphicDevice, pContextDevice)
{
}

CZhuangziWeaponTrail::CZhuangziWeaponTrail(const CZhuangziWeaponTrail & rhs)
	: CMeshEffect(rhs)
{
}

HRESULT CZhuangziWeaponTrail::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CZhuangziWeaponTrail::NativeConstruct(void * pArg)
{
	EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
	m_tDesc = *pDesc;
	m_ulID = pDesc->tEffectPacket.ulID;
	m_fMaxLifeTime = pDesc->tEffectPacket.fMaxLifeTime;
	m_tMovement = pDesc->tEffectPacket.tEffectMovement;
	if (FAILED(__super::NativeConstruct(&pDesc->tGroupDesc)))
		return E_FAIL;

	if (FAILED(SetUp_RestComponents()))
		return E_FAIL;
	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));

	//if (nullptr != pArg)
	//{
	//	_matrix WorldMatrix;
	//	memcpy(&WorldMatrix, pArg, sizeof(_matrix));

	//	m_pTransform->Set_WorldMatrix(WorldMatrix);
	//}

	m_bCollision = false;


	if (FAILED(Create_NormalAttack()))
		return E_FAIL;

	m_bCollision = false;

	return S_OK;
}

_int CZhuangziWeaponTrail::Tick(_double dTimeDelta)
{
	if (m_eState == OBJSTATE::DEAD)
	{
		//for (auto& pAttack : m_vecNormalAttacks)
		//{
		//	pAttack->Set_State(OBJSTATE::DEAD);
		//	Safe_Release(pAttack);
		//}

		//m_vecNormalAttacks.clear();

		return (_int)OBJSTATE::DEAD;
	}

	if (static_cast<_int>(OBJSTATE::DEAD) == __super::Tick(dTimeDelta))
	{
		m_eState = OBJSTATE::DEAD;
		//++m_iIsEnd;

		//Create_Explosion();
		//return (_int)OBJSTATE::DEAD;
	}

	m_fCurLifeTime += static_cast<_float>(dTimeDelta);
	if (m_fMaxLifeTime > 0 && m_fMaxLifeTime < m_fCurLifeTime)
	{
		m_eState = OBJSTATE::DEAD;
		//++m_iIsEnd;
		//Create_Explosion();
		//return (_int)OBJSTATE::DEAD;
	}

	m_pTransform->Go_DirWithSpeed(dTimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_tMovement.fSpeed);


	if (Is_UnderNavigatoinMesh())
	{
		m_eState = OBJSTATE::DEAD;
	}


	Update_NormalAttack();


	return 0;
}

_int CZhuangziWeaponTrail::LateTick(_double dTimeDelta)
{
	return __super::LateTick(dTimeDelta);
}

HRESULT CZhuangziWeaponTrail::Render()
{
	return __super::Render();
}

void CZhuangziWeaponTrail::Update_NormalAttack()
{
	_uint iCount = 0;

	_float fOffset = 3.f;
	for (auto& pAttack : m_vecNormalAttacks)
	{
		CTransform*	pAttackTransform = dynamic_cast<CTransform*>(pAttack->Get_Component(COM_KEY_TRANSFORM));

		if (nullptr != pAttackTransform)
		{
			pAttackTransform->Set_WorldMatrix(Get_MatrixForNormalAttack(iCount));
			_vector vLook = pAttackTransform->Get_State(CTransform::STATE::LOOK);
			pAttackTransform->Set_State(CTransform::STATE::POSITION
				, pAttackTransform->Get_State(CTransform::STATE::POSITION) + vLook * fOffset);
		}

		++iCount;
	}
}

HRESULT CZhuangziWeaponTrail::Create_NormalAttack()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	for (_uint i = 0; i < 7; ++i)
	{
		Humanoid_Atk_Info tInfo;

		tInfo.iNumber = 13;
		XMStoreFloat4x4(&tInfo.Matrix, Get_MatrixForNormalAttack(i));

		// Offset
		_vector tempPosition;
		memcpy(&tempPosition, &tInfo.Matrix.m[3], sizeof(_vector));
		tempPosition = XMVectorSetY(tempPosition, XMVectorGetY(tempPosition) + 15.f);
		memcpy(&tInfo.Matrix.m[3], &tempPosition, sizeof(_vector));

		CNormal_Attack* pAttack = nullptr;
		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(pGameInstance->Get_LevelID(), L"Prototype_Normal_Attack", L"MonsterAttack"
			, (CGameObject**)&pAttack, &tInfo)))
		{
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

		m_vecNormalAttacks.push_back(pAttack);

		Safe_AddRef(pAttack);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

_matrix CZhuangziWeaponTrail::Get_MatrixForNormalAttack(_uint iCount)
{
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();
	_float4x4 WorldFloat4x4;
	XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vPos = XMVectorZero();
	memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_vector));

	if (0 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);
	}
	else if (1 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);

		vLook *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 3.f;
	}
	else if (2 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 0.5f);

		vLook *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 6.f;
	}
	else if (3 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);

		vLook *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 9.f;
	}
	else if (4 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);

		vLook *= -1.f;
		vRight *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 3.f;
	}
	else if (5 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);

		vLook *= -1.f;
		vRight *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 6.f;
	}
	else if (6 == iCount)
	{
		vUp *= -1.f;
		//vPos += (vUp * 1.5f);

		vLook *= -1.f;
		vRight *= -1.f;
		_vector vDiagonalBackRight = XMVector3Normalize(vLook + vRight);

		vPos += vDiagonalBackRight * 9.f;
	}



	memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

	return XMLoadFloat4x4(&WorldFloat4x4);
}

void CZhuangziWeaponTrail::Go_Dir(_double TimeDelta)
{
	m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), ZHUANGZI_WEAPONTRAIL_SPEED);

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	CCell* pSlideOut = nullptr;

	if (!m_pNavigation->IsOn(vPos, &pSlideOut))
		m_eState = OBJSTATE::DEAD;
}

void CZhuangziWeaponTrail::Check_DeleteTime(_double TimeDelta)
{
	m_DeleteTime += TimeDelta;

	if (m_DeleteTime >= m_DeleteDelayTime)
		m_eState = OBJSTATE::DEAD;
}

_bool CZhuangziWeaponTrail::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	return _bool();
}

void CZhuangziWeaponTrail::Collision(CGameObject * pGameObject)
{
}

HRESULT CZhuangziWeaponTrail::SetUp_RestComponents()
{
	/* For.Com_Navigation*/
	_uint iCurrentIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;		

	return S_OK;
}

_bool CZhuangziWeaponTrail::Is_UnderNavigatoinMesh()
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
		return true;
	}

	return false;
}

CZhuangziWeaponTrail * CZhuangziWeaponTrail::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CZhuangziWeaponTrail* pGameInstance = new CZhuangziWeaponTrail(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziWeaponTrail");
	}

	return pGameInstance;
}

CGameObject * CZhuangziWeaponTrail::Clone(void * pArg)
{
	CZhuangziWeaponTrail* pGameInstance = new CZhuangziWeaponTrail(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		Safe_Release(pGameInstance);
		MSGBOX("Failed To Create CZhuangziWeaponTrail");
	}

	return pGameInstance;
}

void CZhuangziWeaponTrail::Free()
{
	__super::Free();

	//Safe_Release(m_pRenderer);
	//Safe_Release(m_pTransform);
	//Safe_Release(m_pModel);
	Safe_Release(m_pNavigation);

	for (auto& pAttack : m_vecNormalAttacks)
	{
		pAttack->Set_State(OBJSTATE::DEAD);
		Safe_Release(pAttack);
	}

	m_vecNormalAttacks.clear();
}
