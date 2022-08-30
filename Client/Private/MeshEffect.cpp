#include "stdafx.h"
#include "MeshEffect.h"
#include "VIBuffer_Pointbillboard.h"
#include "VIBuffer_PointInstance.h"
#include "GameInstance.h"
#include "EffectFactory.h"
#include "UI_HUD_Damage.h"

const char * CMeshEffect::Get_PassName(_uint iIndex)
{
	return nullptr;
}

_uint CMeshEffect::Get_MaxPassIndex()
{
	return _uint();
}

const char * CMeshEffect::Get_BufferContainerPassName(_uint iIndex, _uint iContainerSize)
{
	auto& iter = m_vecIndivMeshEffect.begin() + iIndex;
	if ((*iter)->pModelCom != nullptr)
		return (*iter)->pModelCom->Get_PassName(iContainerSize);
	else if ((*iter)->pPointbillboardCom != nullptr)
		return (*iter)->pPointbillboardCom->Get_PassName(iContainerSize);

	return nullptr;
}

_uint CMeshEffect::Get_BufferContainerMaxPassIndex(_uint iIndex)
{
	auto& iter = m_vecIndivMeshEffect.begin() + iIndex;
	if ((*iter)->pModelCom != nullptr)
		return (*iter)->pModelCom->Get_MaxPassIndex();
	else if ((*iter)->pPointbillboardCom != nullptr)
		return (*iter)->pPointbillboardCom->Get_MaxPassIndex();

	return 0;
}

_uint CMeshEffect::Get_BufferContainerSize()
{
	return (_uint)m_vecIndivMeshEffect.size();
}

_uint CMeshEffect::Get_BufferContainerPassIndex(_uint iIndex)
{
	auto& iter = m_vecIndivMeshEffect.begin() + iIndex;
	
	if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind== (_int)EFFECTMESHDESC::MESH_KIND::MESH)
		return (*iter)->iModelPass;
	else if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind == (_int)EFFECTMESHDESC::MESH_KIND::IMAGELESS_MESH)
		return (*iter)->iImagelessModelPass;
	else if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind == (_int)EFFECTMESHDESC::MESH_KIND::RECT)
		return (*iter)->iPointbillboardPass;

	return 0;
}

void CMeshEffect::Set_BufferContainerPassIndex(_uint iIndex, _uint iPassIndex)
{
	auto& iter = m_vecIndivMeshEffect.begin() + iIndex;
	
	if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind == (_int)EFFECTMESHDESC::MESH_KIND::MESH)
		(*(*iter)).iModelPass = iPassIndex;
	else if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind == (_int)EFFECTMESHDESC::MESH_KIND::IMAGELESS_MESH)
		(*(*iter)).iImagelessModelPass = iPassIndex;
	else if ((*iter)->tEffectMeshDescription.tInfo.iMeshKind == (_int)EFFECTMESHDESC::MESH_KIND::RECT)
		(*(*iter)).iPointbillboardPass = iPassIndex;
}

CMeshEffect::CMeshEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect & rhs)
	: CGameObjectModel(rhs)
{
}


HRESULT CMeshEffect::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CMeshEffect::NativeConstruct(void * pArg)
{
	EFFECT_DESC* pDesc = (EFFECT_DESC*)pArg;
	m_tEffectDesc = *pDesc;

	m_tGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat = pDesc->tGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat;
	_tcscpy_s(m_tGroupDesc.tEffectMeshGroupDesc.szGroupName, pDesc->tGroupDesc.tEffectMeshGroupDesc.szGroupName);
	memcpy(&m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime, &(pDesc->tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime), sizeof(_float2));

	if (m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime.y < 0.5f)
	{
		int a = 0;
	}
	//m_tGroupDesc.bCreateDeathRepeat = true;
	INDIVIDUALMESHEFFECT* pTempIndiv = nullptr;
	for (auto& pIter : pDesc->tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		pTempIndiv = new INDIVIDUALMESHEFFECT;
		memcpy(&pTempIndiv->tEffectMeshDescription.tInfo, &pIter->tInfo, sizeof(EFFECTMESHDESC::EFFECTMESHINFO));
		memcpy(&pTempIndiv->tEffectMeshDescription.tFade_InOut, &pIter->tFade_InOut, sizeof(EFFECTMESHDESC::FADE_INOUT));
		memcpy(&pTempIndiv->tEffectMeshDescription.tPattern, &pIter->tPattern, sizeof(EFFECTMESHDESC::EFFECTMESHPATTERN));
		memcpy(&pTempIndiv->tEffectMeshDescription.tTexture, &pIter->tTexture, sizeof(EFFECTMESHDESC::EFFECTMESHTEXTURE));
		memcpy(&pTempIndiv->tEffectMeshDescription.tUV_Animation, &pIter->tUV_Animation, sizeof(EFFECTMESHDESC::UV_ANIMATION));
		memcpy(&pTempIndiv->tEffectMeshDescription.tUV_Sprite, &pIter->tUV_Sprite, sizeof(EFFECTMESHDESC::UV_SPRITE));

		SHADERNAME* tShaderName = nullptr;
		for (_int i = 0; i < pIter->tEffectMeshShader.vecShaderName.size(); ++i)
		{
			tShaderName = new SHADERNAME;
			_tcscpy_s(tShaderName->szName, pIter->tEffectMeshShader.vecShaderName[i]->szName);
			pTempIndiv->tEffectMeshDescription.tEffectMeshShader.vecShaderName.emplace_back(tShaderName);
		}

		//ZeroMemory(&pTempIndiv->tEffectMeshVariables, sizeof(EFFECTMESHVARIABLES));

		m_vecIndivMeshEffect.emplace_back(pTempIndiv);

	}


	// Copy Particle Descriptions
	for (auto& pIter : pDesc->tGroupDesc.tParticleGroupDesc.listParticleDesc)
	{
		INDIV_PARTICLE* pParticleIndiv = new INDIV_PARTICLE;
		//memcpy(&pParticleIndiv->tParticleDesc, pIter, sizeof(PARTICLEDESC));
		ZeroMemory(&pParticleIndiv->tParticleVar, sizeof(PARTICLE_VAR));

		//PARTICLEDESC* pParticleDesc = new PARTICLEDESC;


		_tcscpy_s(pParticleIndiv->tParticleDesc.szParticleName, pIter->szParticleName);
		memcpy(&pParticleIndiv->tParticleDesc.vPosition, &pIter->vPosition, sizeof(_float3));
		memcpy(&pParticleIndiv->tParticleDesc.vScale, &pIter->vScale, sizeof(_float3));
		memcpy(&pParticleIndiv->tParticleDesc.vRotation, &pIter->vRotation, sizeof(_float3));

		memcpy(&pParticleIndiv->tParticleDesc.iTextureNumber, &pIter->iTextureNumber, sizeof(_int));
		_tcscpy_s(pParticleIndiv->tParticleDesc.szTextureName, pIter->szTextureName);
		_tcscpy_s(pParticleIndiv->tParticleDesc.szTextureProto, pIter->szTextureProto);
		_tcscpy_s(pParticleIndiv->tParticleDesc.szTexturePath, pIter->szTexturePath);

		memcpy(&pParticleIndiv->tParticleDesc.fMultiTextureInterval, &pIter->fMultiTextureInterval, sizeof(_float));
		memcpy(&pParticleIndiv->tParticleDesc.bMultiTextureRepeat, &pIter->bMultiTextureRepeat, sizeof(_bool));

		if (pParticleIndiv->tParticleDesc.tParticleShaderDesc.vecShaderName.size() > 0)
		{
			for (_int i = 0; i < pParticleIndiv->tParticleDesc.tParticleShaderDesc.vecShaderName.size(); i++)
			{
				Safe_Delete(pParticleIndiv->tParticleDesc.tParticleShaderDesc.vecShaderName[i]);
			}
			pParticleIndiv->tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
		}

		for (_int i = 0; i < pIter->tParticleShaderDesc.vecShaderName.size(); ++i)
		{
			SHADERNAME* pShaderName = new SHADERNAME;
			_tcscpy_s(pShaderName->szName, pIter->tParticleShaderDesc.vecShaderName[i]->szName);
			pParticleIndiv->tParticleDesc.tParticleShaderDesc.vecShaderName.emplace_back(pShaderName);
		}

		memcpy(&pParticleIndiv->tParticleDesc.tPointInst_Desc, &pIter->tPointInst_Desc, sizeof(POINTINST_DESC));
		m_vecIndivParticle.emplace_back(pParticleIndiv);

	}

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	for (auto& pIter   : m_vecIndivMeshEffect)
	{
		pIter->pTransformCom->Set_State(CTransform::STATE::POSITION,
			XMVectorSet(pIter->tEffectMeshDescription.tInfo.vPosition.x
				, pIter->tEffectMeshDescription.tInfo.vPosition.y
				, pIter->tEffectMeshDescription.tInfo.vPosition.z, 1.f));

		pIter->pTransformCom->Set_Scale(XMVectorSet
			(pIter->tEffectMeshDescription.tInfo.vScale.x
			, pIter->tEffectMeshDescription.tInfo.vScale.y
			, pIter->tEffectMeshDescription.tInfo.vScale.z, 0.f));

		pIter->pTransformCom->Rotation_Degree(pIter->tEffectMeshDescription.tInfo.vRotation);

		// Init Data
		Init_MeshEffect_Datas(*pIter);

	}


	for (auto& pIter : m_vecIndivParticle)
	{
		pIter->pTransformCom->Set_State(CTransform::STATE::POSITION
			, XMVectorSet(pIter->tParticleDesc.vPosition.x
						, pIter->tParticleDesc.vPosition.y
						, pIter->tParticleDesc.vPosition.z, 1.f));
		pIter->pTransformCom->Set_Scale(XMVectorSet(pIter->tParticleDesc.vScale.x, pIter->tParticleDesc.vScale.y, pIter->tParticleDesc.vScale.z, 0.f));
		pIter->pTransformCom->Rotation_Degree(pIter->tParticleDesc.vRotation);

	}
	Init_AllParticleVar();

	m_bBuffers = true;


	m_pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&pDesc->tEffectPacket.tEffectMovement.vPosition));

	_vector vLook = XMLoadFloat3(&pDesc->tEffectPacket.tEffectMovement.vDirection);
	
	if (!XMVector3Equal(vLook, XMVectorZero()))
	{
		m_pTransform->LookAt_Direction(vLook);
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&pDesc->tEffectPacket.tEffectMovement.vPosition));

	m_bCollision = true;

	m_ulID = pDesc->tEffectPacket.ulID;
	m_tObjectStatusDesc.iAtt = pDesc->tEffectPacket.iAtt;

	if (CHECKSUM_EFFECT_POD_LASER0 == m_ulID)
	{
		m_eHitType = HIT::SINGLE;
		m_iTargetAnimation = TARGETANIMATION_PODLASER + rand() % 100000;
	}
	
	return S_OK;
}

_int CMeshEffect::Tick(_double TimeDelta)
{
	if (m_bMiniGamePause && false == m_tEffectDesc.tEffectPacket.bForMiniGame)
		return (_int)m_eState;

	if (m_eState == OBJSTATE::DEAD)
	{
		if (!m_bCreatedExplosion)
		{
			Create_AfterExplosion();
		}
		return (_int)OBJSTATE::DEAD;
	}

	_float fTimeDelta = static_cast<_float>(TimeDelta);

	m_fLifeTime += fTimeDelta;
	if (m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime.x < m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime.y)
	{
		if (m_fLifeTime > m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime.y)
		{
			if (m_tGroupDesc.tEffectMeshGroupDesc.bCreateDeathRepeat)
			{
				m_fLifeTime = 0.f;
				Init_AllDatas();
				Init_AllParticleVar();
			}
			else
			{
				Set_State(OBJSTATE::DEAD);
				//return (_int)OBJSTATE::DEAD;
			}
		}
	}
	for (auto& pIter : m_vecIndivMeshEffect)
	{
		Update_TextureIndex(fTimeDelta, *pIter);
		Pattern(fTimeDelta, *pIter);
		Update_Fade(fTimeDelta, *pIter);
		Compute_CamDistanceForAlpha(pIter->pTransformCom, pIter->fCameraDistance);
	}

	for (auto& pIter : m_vecIndivParticle)
	{
		pIter->pModelCom->Update(TimeDelta);

		pIter->tParticleVar.fMultiTextureElapsedTime += fTimeDelta;
		if (pIter->tParticleDesc.fMultiTextureInterval < pIter->tParticleVar.fMultiTextureElapsedTime)
		{
			if (++pIter->tParticleVar.iMultiTextureIndex > pIter->tParticleDesc.iTextureNumber - 1)
			{
				if (pIter->tParticleDesc.bMultiTextureRepeat)
				{
					pIter->tParticleVar.iMultiTextureIndex = 0;
				}
				else
				{
					--pIter->tParticleVar.iMultiTextureIndex;
				}
			}
		}
	}


	for (auto& pIter : m_vecIndivMeshEffect)
	{
		

		if (pIter->tEffectMeshDescription.tUV_Animation.bUVAnimation)
		{
			pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.x += pIter->tEffectMeshDescription.tUV_Animation.vUV_Val.x * fTimeDelta;
			pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.y += pIter->tEffectMeshDescription.tUV_Animation.vUV_Val.y * fTimeDelta;

			if (pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.x > 1.f)
				pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.x = 0;
			if (pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.y > 1.f)
				pIter->tEffectMeshVariables.tUVAnimVar.vAnimValue.y = 0;

		}

	}

	

	//m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), m_tEffectDesc.tEffectPacket.tEffectMovement.fSpeed);

	return 0;
}

_int CMeshEffect::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bMiniGamePause && false == m_tEffectDesc.tEffectPacket.bForMiniGame)
		return (_int)m_eState;

	if(m_eState != OBJSTATE::DISABLE)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHABLEND, this);

	return _int();
}

HRESULT CMeshEffect::Render()
{

	for (auto& pIter : m_vecIndivMeshEffect)
	{

		if (FAILED(SetUp_ConstantTable_MeshEffect(*pIter)))
			return E_FAIL;


		CGameInstance* pGameInstance = CGameInstance::Get_Instance();
		_float fFar = pGameInstance->Get_Far();

		switch (pIter->tEffectMeshDescription.tInfo.iMeshKind)
		{
		case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
		{
			if (nullptr == pIter->pModelCom)
				return E_FAIL;

			_uint		iNumMeshContainers = pIter->pModelCom->Get_NumMeshContainers();

			if (FAILED(pIter->pModelCom->Bind_Buffers()))
				return E_FAIL;

			for (_uint i = 0; i < iNumMeshContainers; ++i)
			{
				pIter->pModelCom->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
				//pIter->pModelCom->Set_MaterialTexture(i, aiTextureType_NORMALS, "g_NormalTexture");
				//pIter->pModelCom->SetUp_Texture(m_pRenderer->Get_DistortionTexture(CRenderer::DISTORTIONTYPE::COUNT));


				pIter->pModelCom->Render(i, pIter->iModelPass);


			}
		}
		break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
			if (nullptr == pIter->pModelCom)
				return E_FAIL;
			{
				_uint		iNumMeshContainers = pIter->pModelCom->Get_NumMeshContainers();

				if (FAILED(pIter->pModelCom->Bind_Buffers()))
					return E_FAIL;

				for (_uint i = 0; i < iNumMeshContainers; ++i)
				{
					pIter->pModelCom->Set_MaterialTexture(i, aiTextureType_DIFFUSE, "g_texDiffuse");
					//m_pModelCom->Set_MaterialTexture(i, aiTextureType_NORMALS, "g_NormalTexture");
					pIter->pModelCom->Render(i, pIter->iImagelessModelPass);
				}
			}
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
			if (pIter->pPointbillboardCom == nullptr)
				return E_FAIL;
			pIter->pPointbillboardCom->Render(pIter->iPointbillboardPass);

			break;
		}
	}

	_bool bIsGlow = false;
	_bool bIsDistortion = false;
	_bool bIsTrail = false;
	for (auto& pIter : m_vecIndivParticle)
	{
		if (FAILED(SetUp_ConstantTable_Particle(*pIter)))
			return E_FAIL;

		bIsGlow = false;
		bIsDistortion = false;
		bIsTrail = false;
		for (_int i = 0; i < pIter->tParticleDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
		{
			if (!_tcscmp(pIter->tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName, L"Glow"))
			{
				bIsGlow = true;
			}
			if (!_tcscmp(pIter->tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName, L"Distortion"))
			{
				bIsDistortion = true;
			}
			if (!_tcscmp(pIter->tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName, L"Trail"))
			{
				bIsTrail = true;
			}
		}

		if (bIsTrail)
		{
			pIter->pModelCom->Render(1);
		}
		else
		{
			pIter->pModelCom->Render(static_cast<_uint>(POINTINSTANCE_PASS_INDEX::DEFAULT_PASS));
		}
	}

	return S_OK;
}

_bool CMeshEffect::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (pGameObject->Get_TargetAnimation() == m_iTargetAnimation)
			return false;

		CCollider* pCollider = (CCollider*)pGameObject->Get_Component(COM_KEY_HITBOX);
		if (nullptr == pCollider)
			return false;

		_float fDist = 0.f;

			if (pCollider->Collision_AABB(Get_RayOrigin(), Get_RayDirection(), fDist))
			{
				m_iFlagCollision |= FLAG_COLLISION_RAY;
				return true;
			}
	}

	return false;
}

void CMeshEffect::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		_uint iDamage = Get_RandomAtt();

		pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

		CUI_HUD_Damage::UIDAMAGEDESC	desc;

		desc.eType = CUI_HUD_Damage::DAMAGETYPE::RANGE;

		desc.iDamageAmount = iDamage;

		CCollider* pCollider = (CCollider*)pGameObject->Get_Component(COM_KEY_HITBOX);
		if (nullptr == pCollider)
		{
			m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
			return;
		}
			
		XMStoreFloat4(&desc.vWorldPos, pCollider->Get_WorldPos(CCollider::TYPE::AABB));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

		RELEASE_INSTANCE(CGameInstance);
	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

_fvector CMeshEffect::Get_RayOrigin()
{
	if (0 >= m_vecIndivMeshEffect.size())
		return XMVectorZero();

	if (nullptr == m_vecIndivMeshEffect.front()->pTransformCom)
		return XMVectorZero();

	_matrix vMyWorldMatrix = m_vecIndivMeshEffect.front()->pTransformCom->Get_WorldMatrix();
	_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	_float4x4 TempFloat4x4;
	XMStoreFloat4x4(&TempFloat4x4, vMyWorldMatrix * WorldMatrix);

	_float4 vResultPos;
	memcpy(&vResultPos, &TempFloat4x4.m[3], sizeof(_float4));

	return XMLoadFloat4(&vResultPos);
}

_fvector CMeshEffect::Get_RayDirection()
{
	if (0 >= m_vecIndivMeshEffect.size())
		return XMVectorZero();

	if (nullptr == m_vecIndivMeshEffect.front()->pTransformCom)
		return XMVectorZero();

	_matrix vMyWorldMatrix = m_vecIndivMeshEffect.front()->pTransformCom->Get_WorldMatrix();
	//_matrix WorldMatrix = m_pTransform->Get_WorldMatrix();

	//_float4x4 TempFloat4x4;
	//XMStoreFloat4x4(&TempFloat4x4, vMyWorldMatrix * WorldMatrix);

	//_float3 vResultLook;
	//memcpy(&vResultLook, &TempFloat4x4.m[2], sizeof(_float3));

	//_matrix RotateMatrix = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	_float4x4 TempWorldFloat4x4;
	XMStoreFloat4x4(&TempWorldFloat4x4, vMyWorldMatrix);

	_vector vLook = XMVectorZero();
	memcpy(&vLook, &TempWorldFloat4x4.m[2], sizeof(_vector));

	_matrix RotateMatrix = XMMatrixRotationAxis(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT)), XMConvertToRadians(-90.f));

	vLook = XMVector3Normalize(vLook);
	vLook = XMVector3TransformNormal(vLook, RotateMatrix);

	return XMVector3Normalize(vLook);
}

_fvector CMeshEffect::Get_RingScale(CTransform::STATE eState)
{
	if (m_vecIndivMeshEffect.empty() == true)
		return XMVectorZero();

	INDIVIDUALMESHEFFECT* pTemp = (m_vecIndivMeshEffect.front());
	_vector vScale = pTemp->pTransformCom->Get_State(eState);

	return vScale;
}


HRESULT CMeshEffect::SetUp_Components()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	/* For.Com_DissolveTexture*/
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		return E_FAIL;

	//
	for (INDIVIDUALMESHEFFECT*& pIter : m_vecIndivMeshEffect)
	{
		pIter->pTransformCom = static_cast<CTransform*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Transform")));

		/* For.Com_Model*/
		switch (pIter->tEffectMeshDescription.tInfo.iMeshKind)
		{
		case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
			pIter->pModelCom = static_cast<CModel*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tEffectMeshDescription.tInfo.szMeshPrototypeName));
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
			pIter->pModelCom = static_cast<CModel*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tEffectMeshDescription.tInfo.szMeshPrototypeName));
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
			pIter->pPointbillboardCom = static_cast<CVIBuffer_Pointbillboard*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_Pointbillboard")));

			break;
		default:
			break;
		}


		/* For.Com_DefaultTexture*/
		if (pIter->tEffectMeshDescription.tTexture.bDefaultTexture)
		{
			pIter->pDefaultTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tEffectMeshDescription.tTexture.szDefaultTexturePrototypeName));
		}

		/* For.Com_AlphaOneTexture*/
		if (pIter->tEffectMeshDescription.tTexture.bAlphaOneTexture)
		{
			pIter->pAlphaOneTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tEffectMeshDescription.tTexture.szAlphaOneTexturePrototypeName));
		}

		/* For.Com_MaskTexture*/
		if (pIter->tEffectMeshDescription.tTexture.bMaskTexture)
		{
			pIter->pMaskTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tEffectMeshDescription.tTexture.szMaskTexturePrototypeName));
		}

	}

	for (INDIV_PARTICLE*& pIter : m_vecIndivParticle)
	{
		pIter->pTransformCom = static_cast<CTransform*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Transform")));

		/* For.Com_Model*/
		pIter->pModelCom = static_cast<CVIBuffer_PointInstance*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"), &pIter->tParticleDesc.tPointInst_Desc));


		/* For.Com_DefaultTexture*/
		pIter->pTextureCom = static_cast<CTexture*>(pGameInstance->Clone_Component((_uint)LEVEL::STATIC, pIter->tParticleDesc.szTextureProto));
	}

	return S_OK;
}

HRESULT CMeshEffect::SetUp_ConstantTable_MeshEffect(const IndividualMeshEffect& _tDesc)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	switch (_tDesc.tEffectMeshDescription.tInfo.iMeshKind)
	{
	case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
	{
		_tDesc.pModelCom->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(_tDesc.pTransformCom->Get_WorldMatrix() * m_pTransform->Get_WorldMatrix()), sizeof(_matrix));
		_tDesc.pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		_tDesc.pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		_tDesc.pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		_float fDissolve = _tDesc.tEffectMeshVariables.tShaderVariables.fDissolve;
		_tDesc.pModelCom->SetUp_RawValue("g_fDissolve", &fDissolve, sizeof(_float));
		_tDesc.pModelCom->SetUp_Texture("g_texDissolve", m_pDissolveTextureCom->Get_SRV(0));

		_int bIsUVAnim = static_cast<_int>(_tDesc.tEffectMeshDescription.tUV_Animation.bUVAnimation);
		_tDesc.pModelCom->SetUp_RawValue("g_iIsUVAnimation", &bIsUVAnim, sizeof(_int));
		_float2 vTempAnimVar = _tDesc.tEffectMeshVariables.tUVAnimVar.vAnimValue;
		_tDesc.pModelCom->SetUp_RawValue("g_vUVAnimation", &vTempAnimVar, sizeof(_float2));


		_int iIsGlow = 0;
		_int iIsDistortion = 0;
		for (_int i = 0; i < _tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName.size(); ++i)
		{
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Glow"))
			{
				iIsGlow = 1;
			}
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Distortion"))
			{
				iIsDistortion = 1;
			}
		}
		_tDesc.pModelCom->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int));
		_tDesc.pModelCom->SetUp_RawValue("g_iIsDistortion", &iIsDistortion, sizeof(_int));
		_float fFar = pGameInstance->Get_Far();
		_tDesc.pModelCom->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

		break;
	}
	case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
	{
		_tDesc.pModelCom->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(_tDesc.pTransformCom->Get_WorldMatrix() * m_pTransform->Get_WorldMatrix()), sizeof(_matrix));
		_tDesc.pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		_tDesc.pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		_tDesc.pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		_float3 fIsTexture = { 1.f, 1.f, 1.f };
		
		if (_tDesc.pDefaultTextureCom && _tDesc.tEffectMeshDescription.tTexture.bDefaultTexture)
			_tDesc.pModelCom->SetUp_Texture("g_texDefault", _tDesc.pDefaultTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex));
		else fIsTexture.x = 0.f;

		if (_tDesc.pAlphaOneTextureCom && _tDesc.tEffectMeshDescription.tTexture.bAlphaOneTexture)
			_tDesc.pModelCom->SetUp_Texture("g_texAlphaOne", _tDesc.pAlphaOneTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex));
		else fIsTexture.y = 0.f;

		if (_tDesc.pMaskTextureCom && _tDesc.tEffectMeshDescription.tTexture.bMaskTexture)
			_tDesc.pModelCom->SetUp_Texture("g_texMask", _tDesc.pMaskTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex));
		else fIsTexture.z = 0.f;

		_tDesc.pModelCom->SetUp_RawValue("g_fIsTexture", &fIsTexture, sizeof(_float3));

		_int bIsUVAnim = static_cast<_int>(_tDesc.tEffectMeshDescription.tUV_Animation.bUVAnimation);
		_tDesc.pModelCom->SetUp_RawValue("g_iIsUVAnimation", &bIsUVAnim, sizeof(_int));
		_float2 vTempAnimVar = _tDesc.tEffectMeshVariables.tUVAnimVar.vAnimValue;
		_tDesc.pModelCom->SetUp_RawValue("g_vUVAnimation", &vTempAnimVar, sizeof(_float2));

		// Shader alert
		_int iIsGlow = 0;
		_int iIsDistortion = 0;
		for (_int i = 0; i < _tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName.size(); ++i)
		{
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Glow"))
			{
				iIsGlow = 1;
			}
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Distortion"))
			{
				iIsDistortion = 1;
			}
		}
		_tDesc.pModelCom->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int));
		_tDesc.pModelCom->SetUp_RawValue("g_iIsDistortion", &iIsDistortion, sizeof(_int));
		_float fFar = pGameInstance->Get_Far();
		_tDesc.pModelCom->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));


		_int iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeIn;
		_tDesc.pModelCom->SetUp_RawValue("g_iIsFadeIn", &iBool, sizeof(_int));
		iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeOut;
		_tDesc.pModelCom->SetUp_RawValue("g_iIsFadeOut", &iBool, sizeof(_int));
		_float fFade = _tDesc.tEffectMeshVariables.tFadeInVariables.fCurrentFade;
		_tDesc.pModelCom->SetUp_RawValue("g_fFadeIn", &fFade, sizeof(_float));
		fFade = _tDesc.tEffectMeshVariables.tFadeOutVariables.fCurrentFade;
		_tDesc.pModelCom->SetUp_RawValue("g_fFadeOut", &fFade, sizeof(_float));

		//_int iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeIn;
		//_tDesc.pModelCom->SetUp_RawValue("g_iIsFadeIn", &iBool, sizeof(_int));
		//iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeOut;
		//_tDesc.pModelCom->SetUp_RawValue("g_iIsFadeOut", &iBool, sizeof(_int));
		//_float fFade = _tDesc.tEffectMeshVariables.tFadeInVariables.fCurrentFade;
		//_tDesc.pModelCom->SetUp_RawValue("g_fFadeIn", &fFade, sizeof(_float));
		//fFade = _tDesc.tEffectMeshVariables.tFadeOutVariables.fCurrentFade;
		//_tDesc.pModelCom->SetUp_RawValue("g_fFadeOut", &fFade, sizeof(_float));
	}

	break;
	case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
	{
		_matrix vWorldMatrix = (_tDesc.pTransformCom->Get_WorldMatrix() * m_pTransform->Get_WorldMatrix());
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(vWorldMatrix), sizeof(_matrix));
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

		_tDesc.pPointbillboardCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));

		_matrix ViewMatrix = pGameInstance->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW);
		_matrix ViewMatrixInverse = XMMatrixInverse(nullptr, ViewMatrix);
		_float4x4 vViewInv4x4 = {};
		XMStoreFloat4x4(&vViewInv4x4, ViewMatrixInverse);
		_vector vCamLook = {};
		memcpy_s(&vCamLook, sizeof(_float4), &vViewInv4x4.m[2][0], sizeof(_float4));
		XMVectorSetW(vCamLook, 0.f);
		vCamLook = XMVector3Normalize(vCamLook);
		HRESULT res = _tDesc.pPointbillboardCom->SetUp_RawValue("g_vCamLook", &vCamLook, sizeof(_float4));

		_float3 fIsTexture = { 1.f, 1.f, 1.f };

		if (_tDesc.pDefaultTextureCom && _tDesc.tEffectMeshDescription.tTexture.bDefaultTexture)
			_tDesc.pPointbillboardCom->SetUp_Texture("g_texDefault", _tDesc.pDefaultTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex));
		else fIsTexture.x = 0.f;

		if (_tDesc.pAlphaOneTextureCom && _tDesc.tEffectMeshDescription.tTexture.bAlphaOneTexture)
			_tDesc.pPointbillboardCom->SetUp_Texture("g_texAlphaOne", _tDesc.pAlphaOneTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex));
		else fIsTexture.y = 0.f;

		if (_tDesc.pMaskTextureCom && _tDesc.tEffectMeshDescription.tTexture.bMaskTexture)
			_tDesc.pPointbillboardCom->SetUp_Texture("g_texMask", _tDesc.pMaskTextureCom->Get_SRV(_tDesc.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex));
		else fIsTexture.z = 0.f;

		_tDesc.pPointbillboardCom->SetUp_RawValue("g_fIsTexture", &fIsTexture, sizeof(_float3));
		_int iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeIn;
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_iIsFadeIn", &iBool, sizeof(_int));
		iBool = _tDesc.tEffectMeshDescription.tFade_InOut.bFadeOut;
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_iIsFadeOut", &iBool, sizeof(_int));
		_float fFade = _tDesc.tEffectMeshVariables.tFadeInVariables.fCurrentFade;
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_fFadeIn", &fFade, sizeof(_float));
		fFade = _tDesc.tEffectMeshVariables.tFadeOutVariables.fCurrentFade;
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_fFadeOut", &fFade, sizeof(_float));


		_int bIsUVAnim = static_cast<_int>(_tDesc.tEffectMeshDescription.tUV_Animation.bUVAnimation);
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_iIsUVAnimation", &bIsUVAnim, sizeof(_int));
		_float2 vTempAnimVar = _tDesc.tEffectMeshVariables.tUVAnimVar.vAnimValue;
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_vUVAnimation", &vTempAnimVar, sizeof(_float2));
		_int iIsGlow = 0;
		_int iIsDistortion = 0;
		for (_int i = 0; i < _tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName.size(); ++i)
		{
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Glow"))
			{
				iIsGlow = 1;
			}
			if (!_tcscmp(_tDesc.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName, L"Distortion"))
			{
				iIsDistortion = 1;
			}
		}
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int));
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_iIsDistortion", &iIsDistortion, sizeof(_int));
		_float fFar = pGameInstance->Get_Far();
		_tDesc.pPointbillboardCom->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));

	}
	break;
	default:
		break;
	}





	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CMeshEffect::SetUp_ConstantTable_Particle(const INDIV_PARTICLE & _tDesc)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	//_vector vOriginPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	//_vector vEffectPos = _tDesc.pTransformCom->Get_State(CTransform::STATE::POSITION);
	//_vector vAddedPos = vOriginPos + vEffectPos;
	//XMVectorSetW(vAddedPos, 1.f);
	//_tDesc.pTransformCom->Set_State(CTransform::STATE::POSITION, vAddedPos);
	//_tDesc.pModelCom->SetUp_RawValue("g_matWorld", &_tDesc.pTransformCom->Get_WorldMatrix4x4Transpose(), sizeof(_matrix));
	//_tDesc.pTransformCom->Set_State(CTransform::STATE::POSITION, vEffectPos);

	_tDesc.pModelCom->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(_tDesc.pTransformCom->Get_WorldMatrix() * m_pTransform->Get_WorldMatrix()), sizeof(_matrix));
	_tDesc.pModelCom->SetUp_RawValue("g_matView", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	_tDesc.pModelCom->SetUp_RawValue("g_matProj", &pGameInstance->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	//const LIGHTDESC*	pLightDesc = pGameInstance->Get_Light(L"LightDir");

	//_tDesc.pModelCom->SetUp_RawValue("g_vLightDir", (void*)&pLightDesc->vDirection, sizeof(_float4));
	//_tDesc.pModelCom->SetUp_RawValue("g_vLightDiffuse", (void*)&pLightDesc->vDiffuse, sizeof(_float4));
	//_tDesc.pModelCom->SetUp_RawValue("g_vLightSpecular", (void*)&pLightDesc->vSpecular, sizeof(_float4));
	//_tDesc.pModelCom->SetUp_RawValue("g_vLightAmbient", (void*)&pLightDesc->vAmbient, sizeof(_float4));
	_tDesc.pModelCom->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));
	_tDesc.pModelCom->SetUp_Texture("g_texDiffuse", _tDesc.pTextureCom->Get_SRV(_tDesc.tParticleVar.iMultiTextureIndex));

	_int iIsGlow = 0;
	_int iIsDistortion = 0;
	for (_int i = 0; i < _tDesc.tParticleDesc.tParticleShaderDesc.vecShaderName.size(); ++i)
	{
		if (!_tcscmp(_tDesc.tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName, L"Glow"))
		{
			iIsGlow = 1;
		}
		if (!_tcscmp(_tDesc.tParticleDesc.tParticleShaderDesc.vecShaderName[i]->szName, L"Distortion"))
		{
			iIsDistortion = 1;
		}
	}
	_tDesc.pModelCom->SetUp_RawValue("g_iIsGlow", &iIsGlow, sizeof(_int));
	_tDesc.pModelCom->SetUp_RawValue("g_iIsDistortion", &iIsDistortion, sizeof(_int));

	_float fFar = pGameInstance->Get_Far();
	_tDesc.pModelCom->SetUp_RawValue("g_fFar", &fFar, sizeof(_float));
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMeshEffect::Init_AllDatas()
{
	for (auto& pIter : m_vecIndivMeshEffect)
		Init_MeshEffect_Datas(*pIter);

	Init_AllParticleVar();
}

void CMeshEffect::Init_MeshEffect_Datas(IndividualMeshEffect & _tDesc)
{
	Init_PatternData(_tDesc);
	Init_Texture(_tDesc);
	Init_ShaderVariables(_tDesc);
	Init_FadeData(_tDesc);
}

void CMeshEffect::Init_PatternData(IndividualMeshEffect& _tDesc)
{
	Init_ScalePatternData(_tDesc);
	Init_RotationPatternData(_tDesc);
	Init_PositionPatternData(_tDesc);
}

void CMeshEffect::Init_AllPatternData()
{
	for (auto& pIter : m_vecIndivMeshEffect)
	{
		Init_PatternData(*pIter);
		//Init_PositionPatternData(*pIter);
		//Init_RotationPatternData(*pIter);
		//Init_ScalePatternData(*pIter);
	}
}

void CMeshEffect::Init_PositionPatternData(IndividualMeshEffect & _tDesc)
{
	_tDesc.tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime = m_fLifeTime;

	// Position
	//_vector vPosition = XMLoadFloat3(&m_tDesc.tPattern.vPattern_Position_Init);
	_vector vPosition = XMLoadFloat3(&_tDesc.tEffectMeshDescription.tInfo.vPosition);
	vPosition = XMVectorSetW(vPosition, 1.f);
	_tDesc.pTransformCom->Set_State(CTransform::STATE::POSITION, vPosition);

	_float fPositionTimeLength = _tDesc.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.y - _tDesc.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.x;
	if (fPositionTimeLength > 0.f)
	{
		_vector vCurPos = _tDesc.pTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vGoalPos = XMLoadFloat3(&_tDesc.tEffectMeshDescription.tPattern.vPattern_Position_Goal);
		XMVectorSetW(vGoalPos, 1.f);

		_float fLength = XMVectorGetX(XMVector3Length(vCurPos - vGoalPos));
		_tDesc.tEffectMeshDescription.tPattern.fPositionSpeed = fLength / fPositionTimeLength;
		_vector vDir = vGoalPos - vCurPos;
		XMStoreFloat3(&_tDesc.tEffectMeshDescription.tPattern.vPositionDirection, vDir);
	}
}

void CMeshEffect::Init_RotationPatternData(IndividualMeshEffect & _tDesc)
{
	_tDesc.tEffectMeshVariables.tPatternVariables.fPattern_Rotation_LifeTime = m_fLifeTime;


	// Rotation
	//_tDesc.pTransformCom->Rotation_Degree(_tDesc.tEffectMeshDescription.tInfo.vRotation);

	ZeroMemory(&_tDesc.tEffectMeshDescription.tPattern.vRotationSpeed, sizeof(_float3));
	//ZeroMemory(&_tDesc.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current, sizeof(_float3));
	_tDesc.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current = _tDesc.tEffectMeshDescription.tInfo.vRotation;
	_float fRotationTimeLength = _tDesc.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.y - _tDesc.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.x;
	if (fRotationTimeLength > 0.f)
	{
		//_vector vCurRot = XMVectorSet(m_tDesc.tInfo.vRotation.x, m_tDesc.tInfo.vRotation.y, m_tDesc.tInfo.vRotation.z, 0.f);
		//_vector vGoalRot = XMLoadFloat3(&m_tDesc.tPattern.vPattern_Rotation_Goal);
		//XMVectorSetW(vGoalRot, 1.f);

		_float fLengthX = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Rotation_Goal.x - _tDesc.tEffectMeshDescription.tInfo.vRotation.x);
		_tDesc.tEffectMeshDescription.tPattern.vRotationSpeed.x = fLengthX / fRotationTimeLength;
		_float fLengthY = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Rotation_Goal.y - _tDesc.tEffectMeshDescription.tInfo.vRotation.y);
		_tDesc.tEffectMeshDescription.tPattern.vRotationSpeed.y = fLengthY / fRotationTimeLength;
		_float fLengthZ = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Rotation_Goal.z - _tDesc.tEffectMeshDescription.tInfo.vRotation.z);
		_tDesc.tEffectMeshDescription.tPattern.vRotationSpeed.z = fLengthZ / fRotationTimeLength;
	}
}

void CMeshEffect::Init_ScalePatternData(IndividualMeshEffect & _tDesc)
{
	_tDesc.tEffectMeshVariables.tPatternVariables.fPattern_Scale_LifeTime = m_fLifeTime;

	
	// Scale
	_vector vScale = XMLoadFloat3(&_tDesc.tEffectMeshDescription.tInfo.vScale);
	vScale = XMVectorSetW(vScale, 0.f);

	_tDesc.pTransformCom->Set_Scale(vScale);

	ZeroMemory(&_tDesc.tEffectMeshDescription.tPattern.vScaleSpeed, sizeof(_float3));
	_tDesc.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current = _tDesc.tEffectMeshDescription.tInfo.vScale;
	_float fScaleTimeLength = _tDesc.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.y - _tDesc.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.x;
	if (fScaleTimeLength > 0.f)
	{
		_float fLengthX = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Scale_Goal.x - _tDesc.tEffectMeshDescription.tInfo.vScale.x);
		_tDesc.tEffectMeshDescription.tPattern.vScaleSpeed.x = fLengthX / fScaleTimeLength;
		_float fLengthY = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Scale_Goal.y - _tDesc.tEffectMeshDescription.tInfo.vScale.y);
		_tDesc.tEffectMeshDescription.tPattern.vScaleSpeed.y = fLengthY / fScaleTimeLength;
		_float fLengthZ = static_cast<_float>(_tDesc.tEffectMeshDescription.tPattern.vPattern_Scale_Goal.z - _tDesc.tEffectMeshDescription.tInfo.vScale.z);
		_tDesc.tEffectMeshDescription.tPattern.vScaleSpeed.z = fLengthZ / fScaleTimeLength;
	}
}

void CMeshEffect::Init_Texture(IndividualMeshEffect & _tDesc)
{
	_tDesc.tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime = 0.f;
	_tDesc.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = 0;
	_tDesc.tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime = 0.f;
	_tDesc.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex = 0;
	_tDesc.tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime = 0.f;
	_tDesc.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex = 0;
}

void CMeshEffect::Init_ShaderVariables(IndividualMeshEffect & _tDesc)
{
	_tDesc.tEffectMeshVariables.tShaderVariables.fDissolve = 0.f;

	_tDesc.tEffectMeshDescription.tEffectMeshShader.fDissolveSpeed = (m_tGroupDesc.tEffectMeshGroupDesc.vCreateDeathTime.y - _tDesc.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.y);
	_tDesc.tEffectMeshDescription.tEffectMeshShader.fDissolveSpeed = 1.f / _tDesc.tEffectMeshDescription.tEffectMeshShader.fDissolveSpeed;

	_tDesc.iModelPass = (_int)MODEL_PASS::IMAGE;
	_tDesc.iImagelessModelPass = (_int)MODEL_PASS::IMAGELESS;
	_tDesc.iPointbillboardPass = (_int)POINTBILLBOARD_PASS::BASE;
}

void CMeshEffect::Init_FadeData(IndividualMeshEffect & _tDesc)
{
	
	_float fFadeInSub = _tDesc.tEffectMeshDescription.tFade_InOut.vFadeIn_StartEndTime.y - _tDesc.tEffectMeshDescription.tFade_InOut.vFadeIn_StartEndTime.x;
	if (fFadeInSub < 0)
		assert(false);
	
	_tDesc.tEffectMeshVariables.tFadeInVariables.fFadeVal = 1.f / fFadeInSub;
	_tDesc.tEffectMeshVariables.tFadeInVariables.fCurrentFade = 0.f;

	_float fFadeOutSub = _tDesc.tEffectMeshDescription.tFade_InOut.vFadeOut_StartEndTime.y - _tDesc.tEffectMeshDescription.tFade_InOut.vFadeOut_StartEndTime.x;
	if (fFadeOutSub < 0)
		assert(false);

	_tDesc.tEffectMeshVariables.tFadeOutVariables.fFadeVal = 1.f / fFadeOutSub;
	_tDesc.tEffectMeshVariables.tFadeOutVariables.fCurrentFade = 1.f;
}

void CMeshEffect::Pattern(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	Pattern_Scale(_dTimeDelta,  _tIndiv);
	Pattern_Rotation(_dTimeDelta, _tIndiv);
	Pattern_Position(_dTimeDelta, _tIndiv);
	Update_ShaderVariables(_dTimeDelta, _tIndiv);

}

//_tIndiv.tEffectMeshVariables.fLifeTime

void CMeshEffect::Pattern_Position(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// CHECK time is right
	if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.x < _tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.y)
	{
		// CHECK it started
		if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.x + _tIndiv.tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime < m_fLifeTime)
		{
			// CHECK it is NOT end
			if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.y + _tIndiv.tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime > m_fLifeTime)
			{
				if (_tIndiv.tEffectMeshDescription.tPattern.bPattern_Position_Projectile)
				{
					_float4 vStartPos = { _tIndiv.tEffectMeshDescription.tInfo.vPosition.x, _tIndiv.tEffectMeshDescription.tInfo.vPosition.y, _tIndiv.tEffectMeshDescription.tInfo.vPosition.z, 1.f };
					_tIndiv.pTransformCom->Projectile(vStartPos, _tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_Projectile_Power
						, m_fLifeTime - _tIndiv.tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime, _tIndiv.tEffectMeshDescription.tPattern.fPattern_Position_Projectile_Angle_Result);
				}
				else
				{
					_vector	vPosition = _tIndiv.pTransformCom->Get_State(CTransform::STATE::POSITION);
					_vector vLook = XMLoadFloat3(&_tIndiv.tEffectMeshDescription.tPattern.vPositionDirection);
					vPosition += XMVector3Normalize(vLook) * _tIndiv.tEffectMeshDescription.tPattern.fPositionSpeed * _fTimeDelta;
					_tIndiv.pTransformCom->Set_State(CTransform::STATE::POSITION, vPosition);
				}




			}
			else if (_tIndiv.tEffectMeshDescription.tPattern.bPattern_Position)
			{
				Init_PositionPatternData(_tIndiv);
			}
		}

	}
}

void CMeshEffect::Pattern_Rotation(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// CHECK time is right
	if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.x < _tIndiv.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.y)
	{
		// CHECK it started
		if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.x < m_fLifeTime)
		{
			// CHECK it is NOT end
			if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Rotation_StartEndTime.y > m_fLifeTime)
			{
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.x += _tIndiv.tEffectMeshDescription.tPattern.vRotationSpeed.x * _float(_dTimeDelta);
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.y += _tIndiv.tEffectMeshDescription.tPattern.vRotationSpeed.y * _float(_dTimeDelta);
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current.z += _tIndiv.tEffectMeshDescription.tPattern.vRotationSpeed.z * _float(_dTimeDelta);
				_tIndiv.pTransformCom->Rotation_Degree(_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Rotation_Current);
			}
			else if (_tIndiv.tEffectMeshDescription.tPattern.bPattern_Rotation)
			{
				Init_RotationPatternData(_tIndiv);
			}
		}
	}
}

void CMeshEffect::Pattern_Scale(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// CHECK time is right
	if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.x < _tIndiv.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.y)
	{
		// CHECK it started
		if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.x < m_fLifeTime)
		{
			// CHECK it is NOT end
			if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Scale_StartEndTime.y > m_fLifeTime)
			{
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.x += _tIndiv.tEffectMeshDescription.tPattern.vScaleSpeed.x * _float(_dTimeDelta);
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.y += _tIndiv.tEffectMeshDescription.tPattern.vScaleSpeed.y * _float(_dTimeDelta);
				_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.z += _tIndiv.tEffectMeshDescription.tPattern.vScaleSpeed.z * _float(_dTimeDelta);
				_tIndiv.pTransformCom->Set_Scale(XMVectorSet(_tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.x, _tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.y, _tIndiv.tEffectMeshVariables.tPatternVariables.vPattern_Scale_Current.z, 0.f));
				
			}
			else if (_tIndiv.tEffectMeshDescription.tPattern.bPattern_Scale)
			{
				Init_ScalePatternData(_tIndiv);
			}
		}
	}
}

void CMeshEffect::Update_TextureIndex(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	if (_tIndiv.tEffectMeshDescription.tTexture.iDefaultTextureNumber > 1
		//		&& m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex != -1
		)
	{
		_tIndiv.tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime += _fTimeDelta;
		if (_tIndiv.tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime > _tIndiv.tEffectMeshDescription.tTexture.fDefaultTextureInterval)
		{
			_tIndiv.tEffectMeshVariables.tTextureVaribles.fDefaultTextureElpasedTime = 0.f;
			if (++_tIndiv.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex > _tIndiv.tEffectMeshDescription.tTexture.iDefaultTextureNumber - 1)
			{
				if (_tIndiv.tEffectMeshDescription.tTexture.bDefaultTextureSpriteRepeat)
				{
					_tIndiv.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = 0;
				}
				else
				{
					//m_tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex = -1;
					--_tIndiv.tEffectMeshVariables.tTextureVaribles.iDefaultTextureIndex;
				}
			}
		}
	}


	if (_tIndiv.tEffectMeshDescription.tTexture.iAlphaOneTextureNumber > 1
		)
	{
		_tIndiv.tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime += static_cast<_float>(_fTimeDelta);
		if (_tIndiv.tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime > _tIndiv.tEffectMeshDescription.tTexture.fAlphaOneTextureInterval)
		{
			_tIndiv.tEffectMeshVariables.tTextureVaribles.fAlphaOneTextureElpasedTime = 0.f;
			if (++_tIndiv.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex > _tIndiv.tEffectMeshDescription.tTexture.iAlphaOneTextureNumber - 1)
			{
				if (_tIndiv.tEffectMeshDescription.tTexture.bAlphaOneTextureSpriteRepeat)
				{
					_tIndiv.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex = 0;
				}
				else
				{
					--_tIndiv.tEffectMeshVariables.tTextureVaribles.iAlphaOneTextureIndex;
				}
			}
		}
	}

	if (_tIndiv.tEffectMeshDescription.tTexture.iMaskTextureNumber > 1
		)
	{
		_tIndiv.tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime += static_cast<_float>(_fTimeDelta);
		if (_tIndiv.tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime > _tIndiv.tEffectMeshDescription.tTexture.fAlphaOneTextureInterval)
		{
			_tIndiv.tEffectMeshVariables.tTextureVaribles.fMaskTextureElpasedTime = 0.f;
			if (++_tIndiv.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex > _tIndiv.tEffectMeshDescription.tTexture.iMaskTextureNumber - 1)
			{
				if (_tIndiv.tEffectMeshDescription.tTexture.bMaskTextureSpriteRepeat)
				{
					_tIndiv.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex = 0;
				}
				else
				{
					--_tIndiv.tEffectMeshVariables.tTextureVaribles.iMaskTextureIndex;
				}
			}
		}
	}
}

void CMeshEffect::Update_ShaderVariables(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	for (size_t i = 0; i < _tIndiv.tEffectMeshDescription.tEffectMeshShader.vecShaderName.size(); i++)
	{
		if (!_tcscmp(L"Dissolve", _tIndiv.tEffectMeshDescription.tEffectMeshShader.vecShaderName[i]->szName))
		{
			Update_DissolveShader(_fTimeDelta, _tIndiv);
		}
	}
}

void CMeshEffect::Update_DissolveShader(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// When Pattern_Position Ended
	if (_tIndiv.tEffectMeshDescription.tPattern.vPattern_Position_StartEndTime.y + _tIndiv.tEffectMeshVariables.tPatternVariables.fPattern_Position_LifeTime + 0.5f < m_fLifeTime)
	{
		switch (_tIndiv.tEffectMeshDescription.tInfo.iMeshKind)
		{
		case (_int)Client::EffectMeshDescription::MESH_KIND::IMAGELESS_MESH:
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::MESH:
			_tIndiv.iModelPass = (_int)MODEL_PASS::IMAGE_DISSOLVE;
			_tIndiv.tEffectMeshVariables.tShaderVariables.fDissolve += _tIndiv.tEffectMeshDescription.tEffectMeshShader.fDissolveSpeed * _fTimeDelta;
			break;
		case (_int)Client::EffectMeshDescription::MESH_KIND::RECT:
			break;
		default:
			break;
		}
	}
}

void CMeshEffect::Update_Fade(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	if (_tIndiv.tEffectMeshDescription.tFade_InOut.bFadeIn)
		Update_FadeIn(_fTimeDelta, _tIndiv);
	if (_tIndiv.tEffectMeshDescription.tFade_InOut.bFadeOut)
		Update_FadeOut(_fTimeDelta, _tIndiv);
}

void CMeshEffect::Update_FadeIn(_float fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// if it already ended
	if (m_fLifeTime > _tIndiv.tEffectMeshDescription.tFade_InOut.vFadeIn_StartEndTime.y) return;
	if (m_fLifeTime > _tIndiv.tEffectMeshDescription.tFade_InOut.vFadeIn_StartEndTime.x)
	{
		_tIndiv.tEffectMeshVariables.tFadeInVariables.fCurrentFade += _tIndiv.tEffectMeshVariables.tFadeInVariables.fFadeVal * fTimeDelta;
	}

}

void CMeshEffect::Update_FadeOut(_float fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv)
{
	// if it already ended
	if (m_fLifeTime > _tIndiv.tEffectMeshDescription.tFade_InOut.vFadeOut_StartEndTime.y) return;
	if (m_fLifeTime > _tIndiv.tEffectMeshDescription.tFade_InOut.vFadeOut_StartEndTime.x)
	{
		_tIndiv.tEffectMeshVariables.tFadeOutVariables.fCurrentFade -= _tIndiv.tEffectMeshVariables.tFadeOutVariables.fFadeVal * fTimeDelta;
	}

}

void CMeshEffect::Init_AllParticleVar()
{
	for (auto& pIter : m_vecIndivParticle)
	{
		Init_ParticleVar(*pIter);
		pIter->pModelCom->Init();
	}
}

void CMeshEffect::Init_ParticleVar(INDIV_PARTICLE & tParticleVar)
{
	tParticleVar.tParticleVar.fMultiTextureElapsedTime = 0.f;
	tParticleVar.tParticleVar.iMultiTextureIndex = 0;
	tParticleVar.tParticleVar.fLifeTime = 0.f;
}

HRESULT CMeshEffect::Create_AfterExplosion()
{

	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(CGameInstance::Get_Instance()->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f;
	XMStoreFloat4(&(tPacket.tEffectMovement.vPosition), m_pTransform->Get_State(CTransform::STATE::POSITION));

	switch (m_tEffectDesc.tEffectPacket.ulID)
	{
	//case CHECKSUM_EFFECT_BEAVOIR_DONUT_UP:
	//	m_bCreatedExplosion = true;
	//	tPacket.ulID = CHECKSUM_EFFECT_BEAVOIR_DONUT_DOWN;
	//	pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beavoir_Donut_UpDown1.dat", &tPacket);
	//	break;
	case CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP0:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_DOWN0;
		pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down0.dat", &tPacket, L"Donut0");
		break;
	case CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP1:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_DOWN1;
		pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down1.dat", &tPacket, L"Donut1");
		break;
	case CHECKSUM_EFFECT_BEAUVOIR_DONUT_UP2:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_BEAUVOIR_DONUT_DOWN2;
		pFactory->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\Beauvoir_Donut_Down2.dat", &tPacket, L"Donut2");
		break;
	case CHECKSUM_EFFECT_POD_LASER_READY:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_POD_LASER0;
		tPacket.tEffectMovement.vDirection = m_tEffectDesc.tEffectPacket.tEffectMovement.vDirection;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		tPacket.iAtt = POD_LASER_DAMAGE;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser0.dat", &tPacket, L"PodRayAttack");
		break;
	case CHECKSUM_EFFECT_POD_LASER0:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_POD_LASER1;
		tPacket.tEffectMovement.vDirection = m_tEffectDesc.tEffectPacket.tEffectMovement.vDirection;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		tPacket.iAtt = POD_LASER_DAMAGE;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser1.dat", &tPacket, L"PodRayAttackEnd");
		break;
	case CHECKSUM_EFFECT_LONGKATANA_CHARGING:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_LONGKATANA_CHARGING_END;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\EnergyCharging_End.dat", &tPacket);
		break;

	case CHECKSUM_EFFECT_FIREWORK_UP_BLUE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_BLUE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Blue.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_BLUE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_BLUE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Blue.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_UP_GREEN:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_GREEN;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Green.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_GREEN:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_GREEN;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Green.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_UP_ORANGE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_ORANGE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Orange.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_ORANGE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_ORANGE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Orange.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_UP_PURPLE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_PURPLE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Purple.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_PURPLE:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_PURPLE;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Purple.dat", &tPacket);
		break;	
	case CHECKSUM_EFFECT_FIREWORK_UP_RED:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_RED;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Red.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_RED:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_RED;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Red.dat", &tPacket);
		break;	
	case CHECKSUM_EFFECT_FIREWORK_UP_YELLOW:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_EXPLOSION_YELLOW;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Explosion_Yellow.dat", &tPacket);
		break;
	case CHECKSUM_EFFECT_FIREWORK_EXPLOSION_YELLOW:
		m_bCreatedExplosion = true;
		tPacket.ulID = CHECKSUM_EFFECT_FIREWORK_UP_YELLOW;
		tPacket.tEffectMovement.vPosition = m_tEffectDesc.tEffectPacket.tEffectMovement.vPosition;
		pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\Firework_Up_Yellow.dat", &tPacket);
		break;
	default:
		return S_OK;
		break;
	}




	return S_OK;
}

CMeshEffect * CMeshEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMeshEffect* pInstance = new CMeshEffect(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeshEffect::Clone(void * pArg)
{
	CMeshEffect* pInstance = new CMeshEffect(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone CMeshEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeshEffect::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pDissolveTextureCom);
	if (m_pAttackBox)
	{
		Safe_Release(m_pAttackBox);
	}
	if (m_pHitBox)
	{
		Safe_Release(m_pHitBox);
	}
	Safe_Release(m_pNavigation);

	_int iCount = (_int)m_vecIndivMeshEffect.size();

	for (size_t i = 0; i < iCount; ++i)
	{
		Safe_Release(m_vecIndivMeshEffect[i]->pTransformCom);
		Safe_Release(m_vecIndivMeshEffect[i]->pDefaultTextureCom);
		Safe_Release(m_vecIndivMeshEffect[i]->pAlphaOneTextureCom);
		Safe_Release(m_vecIndivMeshEffect[i]->pMaskTextureCom);
		Safe_Release(m_vecIndivMeshEffect[i]->pModelCom);
		Safe_Release(m_vecIndivMeshEffect[i]->pPointbillboardCom);


		_int iShaderCount = (_int)m_vecIndivMeshEffect[i]->tEffectMeshDescription.tEffectMeshShader.vecShaderName.size();
		for (_int j = 0; j < iShaderCount; ++j)
		{
			Safe_Delete(m_vecIndivMeshEffect[i]->tEffectMeshDescription.tEffectMeshShader.vecShaderName[j]);
		}
		m_vecIndivMeshEffect[i]->tEffectMeshDescription.tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(m_vecIndivMeshEffect[i]);
	}
	m_vecIndivMeshEffect.clear();


	for (auto& pIter : m_tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc)
	{
		_int iShaderCount = (_int)pIter->tEffectMeshShader.vecShaderName.size();
		for (_int j = 0; j < iShaderCount; ++j)
			Safe_Delete(pIter->tEffectMeshShader.vecShaderName[j]);
		pIter->tEffectMeshShader.vecShaderName.clear();
		Safe_Delete(pIter);
	}

	m_tGroupDesc.tEffectMeshGroupDesc.listEffectMeshDesc.clear();



	// Particle
	//iCount = (_int)m_listIndivParticle.size();

	/////////////////////////////////

	_int iRefCount = 0;

	iCount = static_cast<_int>(m_vecIndivParticle.size());
	for (_int i = 0; i < iCount; ++i)
	{
		iRefCount = Safe_Release(m_vecIndivParticle[i]->pModelCom); /////////////////
		iRefCount = Safe_Release(m_vecIndivParticle[i]->pTransformCom);
		iRefCount = Safe_Release(m_vecIndivParticle[i]->pTextureCom);
		_int iShaderCount = static_cast<_int>(m_vecIndivParticle[i]->tParticleDesc.tParticleShaderDesc.vecShaderName.size());
		for (_int j = 0; j < iShaderCount; ++j)
		{
			Safe_Delete(m_vecIndivParticle[i]->tParticleDesc.tParticleShaderDesc.vecShaderName[j]);
		}
		m_vecIndivParticle[i]->tParticleDesc.tParticleShaderDesc.vecShaderName.clear();
		Safe_Delete(m_vecIndivParticle[i]);
	}
	m_vecIndivParticle.clear();



	/////////////////////////////////////


	for (auto& pIter : m_tGroupDesc.tParticleGroupDesc.listParticleDesc)
		Safe_Delete(pIter);
	m_tGroupDesc.tParticleGroupDesc.listParticleDesc.clear();
}


