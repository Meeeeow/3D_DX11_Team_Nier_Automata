#include "stdafx.h"
#include "..\Public\Pod.h"
#include "MainCamera.h"
#include "PodBullet.h"
#include "UI_HUD_PodCoolTime.h"
#include "EffectFactory.h"
#include "CameraMgr.h"
#include "UI_Dialogue_SystemMessage.h"
#include "SoundMgr.h"

const _float CPod::COUNTERATTACK_RADIUS = 10.f;
CPod::CPod(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CPod::CPod(const CPod & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CPod::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPod::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;
	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(SetUp_PodBulletLight(pGameInstance)))
		return E_FAIL;

	if (FAILED(SetUp_PodLight(pGameInstance, _SNOW, 8.f, 16.f)))
		return E_FAIL;


	XMStoreFloat4x4(&m_ParentFloat4x4, XMMatrixIdentity());

	m_iAnimation = 27;
	m_bContinue = false;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	static _uint iCPod_InstanceID = OriginID_Pod2B;
	m_iObjID = iCPod_InstanceID++;

	//m_iFlagUI |= FLAG_UI_CALL_PODCOOLTIME;
	Check_Pause();

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_tObjectStatusDesc.iAtt = POD_BULLETATTACK_DAMAGE;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

_int CPod::Tick(_double TimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	m_pModel->Synchronize_Animation(m_pTransform);
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_iFlagUI & FLAG_UI_CALL_PODCOOLTIME)
		m_dSkillCoolTimeAcc += TimeDelta;

	if (PODSTATE::HANG != m_ePodState && PODSTATE::COUNTERATTACK != m_ePodState)
	{
		if (!m_bLaserShoot)
			Update(TimeDelta);
	}
	else
	{
		if (PODSTATE::HANG == m_ePodState)
			Hang();
		else if (PODSTATE::COUNTERATTACK == m_ePodState)
		{
			CounterAttack();
		}
	}

	Check_State();
	Create_Bullet(TimeDelta);
	Check_Laser();
	Update_UI(TimeDelta);
	Check_Light(pGameInstance, TimeDelta);

	Update_Effects(TimeDelta);




	return (_int)m_eState;
}

_int CPod::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);


	return _int();
}

HRESULT CPod::Render()
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

HRESULT CPod::SetUp_PodBulletLight(CGameInstance * pGameInstance)
{
	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Pod_Light_A", Shader_Viewport, DefaultTechnique)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Pod_Light_B", Shader_Viewport, DefaultTechnique)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Pod_Light_C", Shader_Viewport, DefaultTechnique)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPod::SetUp_PodLight(CGameInstance* pGameInstance, XMFLOAT4 vColor, _float fInnerDegree, _float fOuterDegree)
{
	LIGHTDESC tLightDesc;
	tLightDesc.eState = LIGHTSTATE::WAIT;
	tLightDesc.eType = LIGHTDESC::TYPE::SPOT;
	m_fDistance = 10.f;

	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVector4Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * m_fDistance;

	XMStoreFloat4(&tLightDesc.vPosition, vPosition);
	XMStoreFloat4(&tLightDesc.vTargetPosition, vPosition + vLook);

	tLightDesc.vDiffuse = CProportionConverter()(vColor, 92.f);
	tLightDesc.vAmbient = CProportionConverter()(vColor, 75.f);
	tLightDesc.vSpecular= CProportionConverter()(vColor, 52.f);

	m_fInnerDegree = fInnerDegree;
	m_fOuterDegree = fOuterDegree;

	if (FAILED(pGameInstance->Add_Light(m_pGraphicDevice, m_pContextDevice, L"Pod_Light", Shader_Viewport, DefaultTechnique
		, tLightDesc)))
		return E_FAIL;

	return S_OK;
}

void CPod::Update_PositionForCoaster(_vector vPos)
{
	m_bOnCoaster = true;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

	if (pPlayer == nullptr)
		return;

	CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	if (pPlayerTransform == nullptr)
		return;

	_vector vPlayerRight, vPlayerUp, vPlayerLook;

	vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE::UP);
	vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vPlayerRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vPlayerUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vPlayerLook);

	XMStoreFloat4(&m_vDestPos, vPos);

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMLoadFloat4(&m_vDestPos));
}

void CPod::Update_Position(_vector vPos)
{
	if (!XMVector4Equal(XMLoadFloat4(&m_vDestPos), vPos))
	{
		m_AccelTime = 0.0;
		m_TimeDelta = 0.0;
	}

	XMStoreFloat4(&m_vDestPos, vPos);

	m_bOnCoaster = false;
}

const _double CPod::Get_SkillCoolTime() const
{
	return m_dSkillCoolTimeAcc;
}

void CPod::Update(_double TimeDelta)
{
	if (m_bOnCoaster)
		return;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix		CamWorldMatrix = pCamera->Get_WorldMatrix();

	if (XMMatrixIsIdentity(CamWorldMatrix))
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_float fAccelPower = 0.0f;
	_vector vAccellPower = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_float fValue = (_float)TimeDelta * 2.f;
	_vector vMaxAccellPower = XMVectorSet(fValue, fValue, fValue, fValue);
	m_AccelTime += TimeDelta;

	if (m_AccelTime > 1.0)
		m_AccelTime = 1.0;

	vMaxAccellPower = XMVectorLerp(vMaxAccellPower, vAccellPower, (_float)m_AccelTime);

	m_TimeDelta += (TimeDelta * 0.01);
	
	if (m_TimeDelta > 1.0)
		m_TimeDelta = 1.0;

	_double Time = m_TimeDelta + XMVectorGetX(vMaxAccellPower);
	if (Time > 1.0)
		Time = 1.0;

	_float4x4	CamFloat4x4;
	XMStoreFloat4x4(&CamFloat4x4, CamWorldMatrix);
	
	_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);		
	_vector vUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	memcpy(&vRight, &CamFloat4x4.m[0], sizeof(_vector));
	memcpy(&vUp, &CamFloat4x4.m[1], sizeof(_vector));
	memcpy(&vLook, &CamFloat4x4.m[2], sizeof(_vector));
	
	_vector vLocalUp = XMVectorZero();

	/* Normal Look */
	//vLocalUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	//vLook = XMVector3Cross(vRight, vLocalUp);
	//vUp = XMVector3Cross(vLook, vRight);

	_vector vDestPos = XMLoadFloat4(&m_vDestPos);
	vDestPos = XMVectorLerp(m_pTransform->Get_State(CTransform::STATE::POSITION), vDestPos, (_float)Time);

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	if ((pCamMgr->Get_MainCam_Option() == CMainCamera::OPTION::TOP || pCamMgr->Get_MainCam_Option() == CMainCamera::OPTION::TOPB) && !m_bTargeting)
	{
		CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

		if (pPlayer == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

		if (pPlayerTransform == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		_vector vPlayerRight, vPlayerUp, vPlayerLook;

		vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE::RIGHT);
		vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE::UP);
		vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE::LOOK);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vPlayerRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vPlayerUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vPlayerLook);
	}

	else if (!m_bTargeting)
	{
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	}

	m_pTransform->Set_State(CTransform::STATE::POSITION, vDestPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CPod::Check_State()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	
	if (PODSTATE::ATTACK == m_ePodState)
	{
		m_iAnimation = 3;
	}
	else if (PODSTATE::IDLE == m_ePodState)
	{
		if (3 == iCurAnimIndex)
			m_iAnimation = 38;

		if (38 == iCurAnimIndex)
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_iAnimation = 27;
		}

		if (11 == iCurAnimIndex)
			m_iAnimation = 27;

		if (1 == iCurAnimIndex)
			m_iAnimation = 27;
	}
	else if (PODSTATE::HANG == m_ePodState)
		m_iAnimation = 11;
	else if (PODSTATE::LASERATTACK == m_ePodState)
	{
		m_iAnimation = 1;
	}
	else if (PODSTATE::COUNTERATTACK == m_ePodState)
		m_iAnimation = 27;
}

void CPod::Create_Bullet(_double TimeDelta)
{
	if (3 == m_pModel->Get_CurrentAnimation())
	{
		m_CreateBulletTime += TimeDelta;

		if (!m_pModel->Get_Blend())
		{
			if (m_CreateBulletTime > 0.15)
			{
				CEffectFactory* pFactory = CEffectFactory::Get_Instance();

				CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
				EFFECT_PACKET		pEffectBulletPacket;


				_vector vecDir = m_pTransform->Get_State(CTransform::STATE::LOOK);
				_float3 vDir;
				XMStoreFloat3(&vDir, vecDir);

				_uint iRandX = rand() % 3;
				_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP));

				_uint iRandY = rand() % 3;
				_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));

				_matrix RotateMatrix = XMMatrixIdentity();

				if(1 == iRandX)
				{
					RotateMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(1.f));
					XMStoreFloat3(&vDir, XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&vDir)), RotateMatrix));
				}
				else if (2 == iRandX)
				{
					RotateMatrix = XMMatrixRotationAxis(vUp, XMConvertToRadians(-1.f));
					XMStoreFloat3(&vDir, XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&vDir)), RotateMatrix));
				}

				if (1 == iRandY)
				{
					RotateMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(1.f));
					XMStoreFloat3(&vDir, XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&vDir)), RotateMatrix));
				}
				else if (2 == iRandY)
				{
					RotateMatrix = XMMatrixRotationAxis(vRight, XMConvertToRadians(-1.f));
					XMStoreFloat3(&vDir, XMVector3TransformNormal(XMVector3Normalize(XMLoadFloat3(&vDir)), RotateMatrix));
				}


				_vector vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
				_float4 vPos;
				XMStoreFloat4(&vPos, vecPos);
				pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
				pEffectBulletPacket.tEffectMovement.fSpeed = POD_BULLET_SPEED;
				pEffectBulletPacket.tEffectMovement.vDirection = vDir;
				pEffectBulletPacket.tEffectMovement.vPosition = vPos;
				pEffectBulletPacket.ulID = CHECKSUM_EFFECT_PODBULLET;
				pEffectBulletPacket.fMaxLifeTime = POD_BULLET_MAXLIFE;
				pEffectBulletPacket.iAtt = m_tObjectStatusDesc.iAtt;

				CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\PODBullet.dat", &pEffectBulletPacket, L"Bullet");
				

				SetUp_PodBulletLight(pGameInstance, vPos);

				CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();

				if (m_iNumBullet % 4 == 0)
				{
					pSoundMgr->StopSound(CSoundMgr::POD);
					pSoundMgr->PlaySound(TEXT("Pod_Bullet.mp3"), CSoundMgr::POD);
				}

				else if (m_iNumBullet % 4 == 1)
				{
					pSoundMgr->StopSound(CSoundMgr::BULLET_1);
					pSoundMgr->PlaySound(TEXT("Pod_Bullet.mp3"), CSoundMgr::BULLET_1);
				}

				else if (m_iNumBullet % 4 == 2)
				{
					pSoundMgr->StopSound(CSoundMgr::BULLET_2);
					pSoundMgr->PlaySound(TEXT("Pod_Bullet.mp3"), CSoundMgr::BULLET_2);
				}

				else if (m_iNumBullet % 4 == 3)
				{
					pSoundMgr->StopSound(CSoundMgr::BULLET_3);
					pSoundMgr->PlaySound(TEXT("Pod_Bullet.mp3"), CSoundMgr::BULLET_3);
				}

				++m_iNumBullet;
				//CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
				//CPodBullet* pBullet = nullptr;
				//_matrix Matrix = m_pTransform->Get_WorldMatrix();
				//if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"Prototype_GameObject_PodBullet", L"PodBullet", &Matrix)))
				//{
				//	RELEASE_INSTANCE(CGameInstance);
				//	return;
				//}
				//RELEASE_INSTANCE(CGameInstance);
				m_CreateBulletTime = 0.0;
			}
		}
	}
}

void CPod::Check_Laser()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);
	if (nullptr == pPlayer)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CModel* pModel = (CModel*)pPlayer->Get_Component(L"Com_Model");
	if (nullptr == pModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (119 == pModel->Get_CurrentAnimation())
	{
		// Laser On
			m_bLaserShoot = true;

		if (pModel->Get_LinkMaxRatio() < pModel->Get_PlayTimeRatio())
		{
			m_bLaserShoot = false;
			m_ePodState = PODSTATE::IDLE;

			m_iFlagUI |= FLAG_UI_CALL_PODCOOLTIME;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPod::Create_Laser()
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;

	_float3 vDir ;
	XMStoreFloat3(&vDir, m_pTransform->Get_State(CTransform::STATE::LOOK));

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.vDirection = vDir;
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.fMaxLifeTime = -1.f;

	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_LASER_READY;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser_Ready.dat", &pEffectBulletPacket);


	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_LASER_READY_0;
	//CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser_Ready0.dat", &pEffectBulletPacket);

	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_LASER_READY_1;
	//pEffectBulletPacket.tEffectMovement.vPosition.x += 1.f;
	//CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser_Ready1.dat", &pEffectBulletPacket);

	//pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_LASER_READY_2;
	//pEffectBulletPacket.tEffectMovement.vPosition.x += 1.f;
	//CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Laser_Ready2.dat", &pEffectBulletPacket);

	return S_OK;
}

void CPod::Check_Light(CGameInstance * pGameInstance, _double dTimeDelta)
{
	CPlayer*	pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	if (true == pPlayer->Is_ProgramChipInstalled(CPlayer::PROGRAM_LIGHT))
	{
		if (pGameInstance->Key_Down(DIK_L))
		{
			m_bLight = !m_bLight;
		}
		if (true == m_bLight)
		{
			Active_Light(pGameInstance, dTimeDelta);
		}
		else
		{
			DeActive_Light(pGameInstance, dTimeDelta);
		}
	}
	else
	{
		if (pGameInstance->Key_Down(DIK_L))
		{
			CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(ITEMCODE_ENFORCECHIP_SKILL_LIGHT, 0, CUI_Dialogue_SystemMessage::iMsgNeedChip);
			pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
		}
		DeActive_Light(pGameInstance, dTimeDelta);
	}
}

void CPod::Active_Light(CGameInstance * pGameInstance, _double dTimeDelta)
{
	LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"Pod_Light");
	if (nullptr != pLightDesc)
	{
		pLightDesc->eState = LIGHTSTATE::ALWAYS;

		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vLook = XMVector4Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * m_fDistance;

		pGameInstance->Trace_SpotLight_EyeToAt(L"Pod_Light", vPosition, vPosition + vLook);
		pGameInstance->Increase_InnerDegree(L"Pod_Light", m_fInnerDegree * (_float)dTimeDelta, m_fInnerDegree);
		pGameInstance->Increase_OuterDegree(L"Pod_Light", m_fOuterDegree * (_float)dTimeDelta, m_fOuterDegree);
	}
}

void CPod::DeActive_Light(CGameInstance * pGameInstance, _double dTimeDelta)
{
	LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(L"Pod_Light");
	if (nullptr != pLightDesc)
	{
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vLook = XMVector4Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * m_fDistance;

		pGameInstance->Trace_SpotLight_EyeToAt(L"Pod_Light", vPosition, vPosition + vLook);
		pGameInstance->Decrease_InnerDegree(L"Pod_Light", m_fInnerDegree * (_float)dTimeDelta);
		pGameInstance->Decrease_OuterDegree(L"Pod_Light", m_fOuterDegree * (_float)dTimeDelta);

		if (pLightDesc->fInnerDegree <= 0.f)
			pLightDesc->eState = LIGHTSTATE::WAIT;
	}
}

void CPod::Set_LookForTarget(_vector vPos)
{
	if (nullptr != m_pTransform)
	{
		_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vDir = XMVector3Normalize(vPos - vMyPos);

		_vector vScale = m_pTransform->Get_Scale();

		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
		vUp = XMVector3Normalize(XMVector3Cross(vDir, vRight));

		vRight *= XMVectorGetX(vScale);
		vUp *= XMVectorGetY(vScale);
		vDir *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vDir);
	}
}

void CPod::Set_MaxCoolTime()
{
	m_iFlagUI |= FLAG_UI_CALL_PODCOOLTIME;
	m_dSkillCoolTimeAcc = m_dSkillCoolTimeAccMax + 1.0;
}

HRESULT CPod::SceneChange_AmusementPark()
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

	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));
	CGameInstance* pGameIstance = CGameInstance::Get_Instance();

	SetUp_PodLight(pGameIstance, _DARKSALMON, 5.f, 15.f);

	return S_OK;
}

HRESULT CPod::SceneChange_OperaBackStage()
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

	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_pNavigation->Update(XMMatrixIdentity());
	CGameInstance* pGameInstnace = CGameInstance::Get_Instance();
	SetUp_PodLight(pGameInstnace, _DARKSALMON, 7.f, 15.f);

	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));

	return S_OK;
}

HRESULT CPod::SceneChange_ZhuangziStage()
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

	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));
	CGameInstance* pGameIstance = CGameInstance::Get_Instance();

	SetUp_PodLight(pGameIstance, _SLATEGRAY, 15.f, 32.f);

	return S_OK;
}

HRESULT CPod::SceneChange_ROBOTGENERAL()
{
	Safe_Release(m_pNavigation);

	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	Safe_Release(iter->second);
	m_mapComponents.erase(iter);

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);

	m_pTransform->Set_State(CTransform::STATE::POSITION, ENGELS_START_POSITION);
	m_pNavigation->Update(XMMatrixIdentity());

	m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION));

	return S_OK;
}

void CPod::Shoot_CounterAttack(_vector vHitTargetPos)
{
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;


	_float3 vDir;
	XMStoreFloat3(&vDir, m_pTransform->Get_State(CTransform::STATE::LOOK));

	_float4 vPos;
	//XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE::POSITION));
	XMStoreFloat4(&vPos, vHitTargetPos);

	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.fSpeed = POD_COUNTERATTACK_SPEED;
	pEffectBulletPacket.tEffectMovement.vDirection = vDir;
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_BULLET;
	pEffectBulletPacket.fMaxLifeTime = POD_COUNTERATTACK_MAXLIFE;
	pEffectBulletPacket.iAtt = rand() % 6 + 50;

	pEffectBulletPacket.fColliderRadius = COUNTERATTACK_RADIUS;
	CEffectFactory::Get_Instance()->Load_EffectBullet(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Bullet.dat", &pEffectBulletPacket, L"Bullet");


	pEffectBulletPacket.fMaxLifeTime = 1.f;
	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_GUNFIRE;

	XMStoreFloat4(&pEffectBulletPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Gunfire.dat", &pEffectBulletPacket);

	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERSHOTGUN_SPARK;
	CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_CounterShotGun_Spark.dat", &pEffectBulletPacket);

	CSoundMgr::Get_Instance()->PlaySound(L"Pod_Counter.wav", CSoundMgr::CHANNELID::POD);

}

void CPod::Set_PodState(PODSTATE eState)
{
	m_ePodState = eState;
}

void CPod::Hang()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Model", 0);
	if (nullptr == pModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix PlayerBoneMatarix = pModel->Get_BoneMatrix(L"bone-1");
	_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	_float4x4 PlayerWorldFloat4x4;
	XMStoreFloat4x4(&PlayerWorldFloat4x4, PlayerWorldMatrix);

	_matrix TempMatrix = PlayerBoneMatarix * PlayerWorldMatrix;
	_float4x4 TempFloat4x4;
	XMStoreFloat4x4(&TempFloat4x4, TempMatrix);

	_vector  vPlayerRight;
	_vector  vPlayerLook;
	_vector	 vPlayerUp;
	_vector  vTempPos;

	memcpy(&vPlayerRight, &PlayerWorldFloat4x4.m[0], sizeof(_vector));
	memcpy(&vPlayerUp, &PlayerWorldFloat4x4.m[1], sizeof(_vector));
	memcpy(&vPlayerLook, &PlayerWorldFloat4x4.m[2], sizeof(_vector));

	memcpy(&vTempPos, &TempFloat4x4.m[3], sizeof(_vector));

	vTempPos += vPlayerRight * 0.059f;
	vTempPos += (vPlayerUp * 1.8f);

	_float4x4 ResultFloat4x4;
	XMStoreFloat4x4(&ResultFloat4x4, PlayerWorldMatrix);

	memcpy(&ResultFloat4x4.m[3], &vTempPos, sizeof(_float4));

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&ResultFloat4x4));

	RELEASE_INSTANCE(CGameInstance);
}

void CPod::CounterAttack()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Model", 0);
	if (nullptr == pModel)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", L"Com_Transform", 0);
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix PlayerBoneMatarix = pModel->Get_BoneMatrix(L"bone-1");
	_matrix PlayerWorldMatrix = pTransform->Get_WorldMatrix();
	_float4x4 PlayerWorldFloat4x4;
	XMStoreFloat4x4(&PlayerWorldFloat4x4, PlayerWorldMatrix);

	_matrix TempMatrix = PlayerBoneMatarix * PlayerWorldMatrix;
	_float4x4 TempFloat4x4;
	XMStoreFloat4x4(&TempFloat4x4, TempMatrix);

	_vector  vPlayerRight;
	_vector  vPlayerLook;
	_vector	 vPlayerUp;
	_vector  vTempPos;

	memcpy(&vPlayerRight, &PlayerWorldFloat4x4.m[0], sizeof(_vector));
	memcpy(&vPlayerUp, &PlayerWorldFloat4x4.m[1], sizeof(_vector));
	memcpy(&vPlayerLook, &PlayerWorldFloat4x4.m[2], sizeof(_vector));

	memcpy(&vTempPos, &TempFloat4x4.m[3], sizeof(_vector));

	vTempPos += (vPlayerUp * 1.0f);
	vTempPos += (vPlayerLook * 0.55f);
	vTempPos += (vPlayerRight * 0.2f);

	_float4x4 ResultFloat4x4;
	XMStoreFloat4x4(&ResultFloat4x4, PlayerWorldMatrix);

	memcpy(&ResultFloat4x4.m[3], &vTempPos, sizeof(_float4));

	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&ResultFloat4x4));

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CPod::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_PODCOOLTIME)
	{
		CUI_HUD_PodCoolTime::UIPODCOOLTIMEDESC	desc;

		desc.iObjID = m_iObjID;
		desc.fCrntCoolTimeAcc = (_float)m_dSkillCoolTimeAcc;
		desc.fMaxCoolTimeAcc = (_float)m_dSkillCoolTimeAccMax;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_PODCOOLTIME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_PODCOOLTIME, &desc));

		RELEASE_INSTANCE(CGameInstance);

		if (m_dSkillCoolTimeAccMax < m_dSkillCoolTimeAcc)
		{
			m_dSkillCoolTimeAcc = 0.0;

			m_iFlagUI ^= FLAG_UI_CALL_PODCOOLTIME;
		}
	}

	return S_OK;
}

HRESULT CPod::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_POD, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	/* For.Com_Navigation */
	_uint iCurIndex = 0;
	if (FAILED(__super::Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurIndex)))
		return E_FAIL;

	return S_OK;
}


void CPod::SetUp_PodBulletLight(CGameInstance* pGameInstance, _float4  vPosition)
{
	LIGHTDESC tDesc;
	tDesc.eType = LIGHTDESC::TYPE::POINT;
	tDesc.eState = LIGHTSTATE::DURATION;
	tDesc.dDuration = 0.12;
	tDesc.vPosition = vPosition;

	

	switch (m_iPodLightCnt)
	{
	case 0:
		tDesc.fRange = 2.5f;
		tDesc.vDiffuse = _float4(0.5f, 0.31f, 0.24f, 1.f);
		tDesc.vAmbient = _float4(0.5f, 0.31f, 0.24f, 1.f);
		tDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
		pGameInstance->Add_Light(L"Pod_Light_A", L"Light_Bullet", tDesc);
		break;
	case 1:
		tDesc.fRange = 2.5f;
		tDesc.vDiffuse = _float4(0.425f, 0.32f, 0.0625f, 1.f);
		tDesc.vAmbient = _float4(0.425f, 0.32f, 0.0625f, 1.f);
		tDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
		pGameInstance->Add_Light(L"Pod_Light_B", L"Light_Bullet", tDesc);
		break;
	case 2:
		tDesc.fRange = 2.5f;
		tDesc.vDiffuse = _float4(0.5f, 0.19f, 0.135f, 1.f);
		tDesc.vAmbient = _float4(0.5f, 0.19f, 0.135f, 1.f);
		tDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
		pGameInstance->Add_Light(L"Pod_Light_C", L"Light_Bullet", tDesc);
		break;
	}
	++m_iPodLightCnt;
	if (m_iPodLightCnt >= 3)
		m_iPodLightCnt = 0;
}

HRESULT CPod::Update_Effects(_double _dTimeDelta)
{
	if (nullptr != m_pChargingEffect)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pChargingEffect->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, m_pTransform->Get_State(CTransform::STATE::POSITION));
	}

	// For CounterAttackEffect
	if (PODSTATE::COUNTERATTACK == m_ePodState)
	{
		m_bIsChargingEffectCreated = false;
	}

	return S_OK;
}

HRESULT CPod::Create_ChargingEffect()
{
	if (m_pChargingEffect)
	{
		return S_OK;
	}


	CEffectFactory* pFactory = CEffectFactory::Get_Instance();

	//Delete_ChargingEffect();

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	EFFECT_PACKET		pEffectBulletPacket;


	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

	pEffectBulletPacket.eLevel = (LEVEL)pGameInstance->Get_LevelID();
	pEffectBulletPacket.tEffectMovement.vPosition = vPos;
	pEffectBulletPacket.fMaxLifeTime = -1.f;

	pEffectBulletPacket.ulID = CHECKSUM_EFFECT_POD_CHARGING;
	m_pChargingEffect = CEffectFactory::Get_Instance()->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\POD_Charging.dat", &pEffectBulletPacket);

	if (nullptr == m_pChargingEffect)
	{
		assert(false);
	}
	Safe_AddRef(m_pChargingEffect);

	return S_OK;
}

HRESULT CPod::Delete_ChargingEffect()
{
	if (nullptr != m_pChargingEffect)
	{
		m_pChargingEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}

	return S_OK;
}


CPod * CPod::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPod* pGameInstance = new CPod(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPod");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CPod::Clone(void * pArg)
{
	CPod* pGameInstance = new CPod(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPod");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPod::Free()
{
	__super::Free();
	Delete_ChargingEffect();
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pNavigation);
}
