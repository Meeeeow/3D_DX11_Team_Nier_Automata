#include "stdafx.h"
#include "Coaster.h"
#include "Player.h"
#include "Android_9S.h"
#include "Cell.h"
#include "MainCamera.h"
#include "Level_Loading.h"
#include "CameraMgr.h"

CCoaster::CCoaster(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	:CGameObjectModel(pGraphicDevice,pContextDevice)
{
}

CCoaster::CCoaster(const CCoaster & rhs)
	:CGameObjectModel(rhs)
{
}

HRESULT CCoaster::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCoaster::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_eState = OBJSTATE::DISABLE;
	m_iAnimIndex = 0;

	m_pModel->Update_CombinedTransformationMatrix(0.05);
	m_pModel->Synchronize_Animation_For_Coaster(m_pTransform);

	m_bCollision = false;

	if (pArg != nullptr)
	{
		MO_INFO* pInfo = (MO_INFO*)pArg;
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	}

	m_iPassIndex = (_uint)MESH_PASS_INDEX::ANIM;

	return S_OK;
}

_int CCoaster::Tick(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	//if (pGameInstance->Key_Down(DIK_NUMPAD8))
	//{
	//	m_eState = OBJSTATE::ENABLE;
	//	m_iAnimIndex = (_uint)ANIMSTATE::RUNNING;
	//}

	if (m_eState == OBJSTATE::ENABLE)
	{
		Update_Bone();

		if (0.9 <= m_pModel->Get_PlayTimeRatio() && m_pModel->Get_CurrentAnimation() == 0)
		{
			m_iAnimIndex = (_uint)ANIMSTATE::IDLE;
			m_eAnimState = ANIMSTATE::IDLE;

			if (m_bStart)
				Set_PlayablePosition();

			m_bStart = false;
		}

		if (m_iAnimIndex == 2)
			m_pModel->Set_Animation(m_iAnimIndex, true);
		else
			m_pModel->Set_Animation(m_iAnimIndex);

		m_pModel->Synchronize_Animation_For_Coaster(m_pTransform);

		if (m_bStart)
			Update_Navigation(dTimeDelta);
	}

	RELEASE_INSTANCE(CGameInstance);
	return (_int)m_eState;
}

_int CCoaster::LateTick(_double dTimeDelta)
{
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	if (m_bPause)
		dTimeDelta = 0.0;
	
	if (m_eState == OBJSTATE::DISABLE)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);


	_matrix TempMatrix;

	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::NAVIGATION, m_pNavigation);
#ifdef _DEBUG
	/*TempMatrix = XMLoadFloat4x4(&m_CamBone_1_Matrix);
	m_bone_1->Update(TempMatrix);
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_bone_1);

	TempMatrix = XMLoadFloat4x4(&m_CamBone000_Matrix);
	m_bone000->Update(TempMatrix);
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_bone000);*/

	//TempMatrix = XMLoadFloat4x4(&m_CamBone001_Matrix);
	//m_bone001->Update(TempMatrix);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_bone001);

	//TempMatrix = XMLoadFloat4x4(&m_CamBone002_Matrix);
	//m_bone002->Update(TempMatrix);
	//m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_bone002);
#endif

	return 0;
}

HRESULT CCoaster::Render()
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

void CCoaster::Update_Bone()
{
	Update_Bone_1();
	Update_Bone000();
	Update_Bone001();
	Update_Bone002();
}

void CCoaster::Update_Bone_1()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Cam_bone-1");
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

	vecPos = vecPos + vNormRight * 0.f;
	vecPos = vecPos + vNormUp * 0.f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_CamBone_1_Matrix.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_CamBone_1_Matrix.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_CamBone_1_Matrix.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_CamBone_1_Matrix.m[3], &vecPos, sizeof(_vector));
}

void CCoaster::Update_Bone000()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Cam_bone000");
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

	vecPos = vecPos + vNormRight * 0.f;
	vecPos = vecPos + vNormUp * 0.f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_CamBone000_Matrix.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_CamBone000_Matrix.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_CamBone000_Matrix.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_CamBone000_Matrix.m[3], &vecPos, sizeof(_vector));
}

void CCoaster::Update_Bone001()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Cam_bone001");
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

	vecPos = vecPos + vNormRight * 0.f;
	vecPos = vecPos + vNormUp * 0.f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_CamBone001_Matrix.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_CamBone001_Matrix.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_CamBone001_Matrix.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_CamBone001_Matrix.m[3], &vecPos, sizeof(_vector));
}

void CCoaster::Update_Bone002()
{
	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Cam_bone002");
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

	vecPos = vecPos + vNormRight * 0.f; //*-8.f;
	vecPos = vecPos + vNormUp * 0.f;
	vecPos = vecPos + vNormLook * 0.f;

	memcpy(&m_CamBone002_Matrix.m[0], &vNormRight, sizeof(_vector));
	memcpy(&m_CamBone002_Matrix.m[1], &vNormUp, sizeof(_vector));
	memcpy(&m_CamBone002_Matrix.m[2], &vNormLook, sizeof(_vector));
	memcpy(&m_CamBone002_Matrix.m[3], &vecPos, sizeof(_vector));
}

void CCoaster::Update_Navigation(_double TimeDelta)
{
	_matrix WorldMatrix = m_pModel->Get_BoneMatrix(L"Cam_bone-1") * m_pTransform->Get_WorldMatrix();
	_float4x4 WorldFloat4x4;

	XMStoreFloat4x4(&WorldFloat4x4, WorldMatrix);

	_float4 vLook = m_pModel->Get_vecLook();
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vLook.w = 0.f;

	_vector vecLook = XMLoadFloat4(&vLook);
	vecLook = XMVectorSetW(vecLook, 0.f);

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vecLook));

	vUp = XMVector3Normalize(XMVector3Cross(vecLook, vRight));

	memcpy(&WorldFloat4x4.m[0], &vRight, sizeof(_vector));
	memcpy(&WorldFloat4x4.m[1], &vUp, sizeof(_vector));
	memcpy(&WorldFloat4x4.m[2], &vecLook, sizeof(_vector));

	vPos += XMVector3Normalize(vecLook) * -5.8f;

	memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_vector));

	m_pNavigation->Update(XMLoadFloat4x4(&WorldFloat4x4));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_TRANSFORM, 0);
	if (nullptr == pTransform)
		return;

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);
	if (nullptr == pPlayer)
		return;

	_float fY = 0.f;

	_float3 vValue = m_pModel->Get_vecValue();

	pTransform->Set_State(CTransform::STATE::POSITION, pTransform->Get_State(CTransform::STATE::POSITION) + XMLoadFloat3(&vValue));

	CCell* pSlideCell = nullptr;
	_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);

	if ((false == m_pNavigation->IsOn(vPlayerPos, &pSlideCell)) && (false == pTransform->IsJump()))
	{
		// 기존에 기록된 m_iCurrentIndex를 이용해서 셀의 중심점으로 위치를 보정하거나 (o)
		// 셀 외부로 나간 상태에서 모든 셀과의 거리를 비교해서 가까운 셀의 중심점으로 위치를 보정 (x)
		/*	m_pNavigation->Find_NearCell(vPlayerPos);*/
	
		vPlayerPos = m_pNavigation->Find_CenterPos(m_pNavigation->Get_Index());

		if (XMVector4Equal(XMVectorZero(), vPlayerPos))
			return;

		pTransform->Set_State(CTransform::STATE::POSITION, vPlayerPos);
	}

	CModel* pModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_MODEL, 0);
	if (nullptr == pModel)
		return;

	pModel->Synchronize_Animation(pTransform, m_pNavigation, TimeDelta);

	_uint iCell = m_pNavigation->Find_CurrentCell(pTransform->Get_State(CTransform::STATE::POSITION));

	vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);

	if (false == m_pNavigation->IsOn(vPlayerPos, &pSlideCell))
	{
		if (-1 == iCell)
		{
			if (pTransform->IsJump())
			{
				_vector vNavPos = m_pNavigation->Find_CenterPos(m_pNavigation->Get_Index());
				_vector vPlayerPos = pTransform->Get_State(CTransform::STATE::POSITION);

				vNavPos = XMVectorSetY(vNavPos, XMVectorGetY(vPlayerPos));
				pTransform->Set_State(CTransform::STATE::POSITION, vNavPos);
			}
			else
				pTransform->Set_State(CTransform::STATE::POSITION, m_pNavigation->Find_CenterPos(m_pNavigation->Get_Index()));
		}
	}

	pPlayer->ForCoaster();

	// Android
	CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
	if (nullptr == pAndroid)
		return;

	CTransform* pAndroidTransform = (CTransform*)pAndroid->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pAndroidTransform)
		return;

	pAndroidTransform->Set_State(CTransform::STATE::POSITION, m_pNavigation->Find_CenterPos(COASTER_9S_NAVIINDEX));
	pAndroid->Check_WeaponState();
	pAndroid->Update_Collider();
}

void CCoaster::Set_PlayablePosition()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
	if (nullptr == pAndroid)
		return;

	pAndroid->Set_NoneTeleport(true);

	CLevel* pLevel = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, LEVEL::OPERABACKSTAGE, (_uint)LEVEL::LOADING);
	if (pLevel == nullptr)
		return;

	pGameInstance->Get_CurrentLevel()->Set_NextLevel(pLevel);
	CCameraMgr::Get_Instance()->Set_MainCam_On();

	
/*
	CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
	if (nullptr == pTransform)
		return;

	CNavigation* pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_NAVIGATION, 0);
	if (nullptr == pNavigation)
		return;

	pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_COASTER_ENDPOSITION);
	if (FAILED(pNavigation->Init_Index(pTransform->Get_State(CTransform::STATE::POSITION))))
		return;

	pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"Android_9S", L"Com_Transform", 0);
	if (nullptr == pTransform)
		return;

	pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"Android_9S", COM_KEY_NAVIGATION, 0);
	if (nullptr == pNavigation)
		return;

	pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_COASTER_ENDPOSITION);
	if (FAILED(pNavigation->Init_Index(pTransform->Get_State(CTransform::STATE::POSITION))))
		return;*/
}

void CCoaster::Start_Coaster()
{
	m_eState = OBJSTATE::ENABLE;
	m_iAnimIndex = (_uint)ANIMSTATE::RUNNING;
	m_bStart = true;

	m_pRenderer->Set_Fogging(0);
}

HRESULT CCoaster::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, L"Coaster", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"Cam_bone-1", "bone-1");
	m_pModel->Add_RefNode(L"Cam_bone000", "bone000");
	m_pModel->Add_RefNode(L"Cam_bone001", "bone001");
	m_pModel->Add_RefNode(L"Cam_bone002", "bone002");

	CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.fRadius = 2.f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_bone-1"), (CComponent**)&m_bone_1, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_bone000"), (CComponent**)&m_bone000, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_bone001"), (CComponent**)&m_bone001, &tColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, TEXT("Com_bone002"), (CComponent**)&m_bone002, &tColliderDesc)))
		return E_FAIL;

	_uint iCurrentIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_COASTER, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoaster::SetUp_ConstantTable()
{
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	return S_OK;
}

CCoaster * CCoaster::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CCoaster* pInstance = new CCoaster(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CCoaster::Clone(void * pArg)
{
	CCoaster* pInstance = new CCoaster(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCoaster::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pModel);
	Safe_Release(m_bone000);
	Safe_Release(m_bone001);
	Safe_Release(m_bone002);
	Safe_Release(m_bone_1);
}
