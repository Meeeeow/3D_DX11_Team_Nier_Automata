#include "stdafx.h"
#include "..\Public\Iron_Android9S.h"
#include "CameraMgr.h"
#include "Camera_IronFist.h"
#include "UI_HUD_Damage.h"
#include "Iron_Humanoid_Small.h"
#include "EffectFactory.h"
#include "UI_HUD_Hpbar_MiniGame.h"
#include "SoundMgr.h"

CIron_Android9S::CIron_Android9S(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{
}

CIron_Android9S::CIron_Android9S(const CIron_Android9S & rhs)
	: CGameObjectModel(rhs)
{
}

HRESULT CIron_Android9S::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CIron_Android9S::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_iAnimation = ANIM_IRON9S_APPEARANCE;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	m_pModel->Update_CombinedTransformationMatrix(ZEROPOINTONE);

	m_eAnimState = ANIMSTATE::IDLE;

	//m_pTransform->Set_State(CTransform::STATE::POSITION, IRONFIST_PLAYER_START_POSITION);

	static _uint iCIAndroid_Iron9S_InstanceID = OriginID_Iron9S;
	m_iObjID = iCIAndroid_Iron9S_InstanceID++;

	m_tObjectStatusDesc.fHp = 150;
	m_tObjectStatusDesc.fMaxHp = m_tObjectStatusDesc.fHp;
	m_tObjectStatusDesc.iAtt = 5;

	m_fHitRecovery = 1.f;

	m_bCollision = true;

	Set_FirstLook();

	m_pTransform->Set_State(CTransform::STATE::POSITION, IRONFIST_PLAYER_START_POSITION);
	_uint iCurSel = m_pNavigation->Find_CurrentCell(m_pTransform->Get_State(CTransform::STATE::POSITION));
	m_pNavigation->Set_CurrentIndex(iCurSel);

	Update_Collider();

	m_eState = OBJSTATE::DISABLE;
	m_fBaseYPosition = XMVectorGetY(m_pTransform->Get_State(CTransform::STATE::POSITION));

	return S_OK;
}

_int CIron_Android9S::Tick(_double TimeDelta)
{
	if (OBJSTATE::DEAD == m_eState)
		return (_int)m_eState;

	if (false == m_bMiniGamePause)
		return (_int)m_eState;
	else
	{
		if (false == m_bAppearanceSound)
		{
			CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
			CSoundMgr::Get_Instance()->PlaySound(L"9S_IronIngage.mp3", CSoundMgr::CHANNELID::VOICE);
			m_bAppearanceSound = true;
		}
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance->Key_Down(DIK_GRAVE))
	{
#ifdef _DEBUG
		GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKPURPLE);
		NOTICE("Request Mode\n\n");
		GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
		REQUEST();
#else
		m_pRenderer->Add_RenderDebug();
#endif // _DEBUG
	}
	if (pGameInstance->Key_Down(DIK_2))
	{
		m_pRenderer->Half_Rendering();
	}
	if (pGameInstance->Key_Down(DIK_3))
	{
		m_pRenderer->RenderTarget_Rendering();
	}

	if (m_bPause)
		TimeDelta = 0.0;

	if (OBJSTATE::DISABLE == m_eState)
		TimeDelta *= 0.9;

	Check_HitState(TimeDelta);
	Check_Dead();

	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	if (ANIMSTATE::HIT != m_eAnimState && ANIM_IRON9S_HIT1 != iCurAnimIndex && ANIM_IRON9S_HIT2 != iCurAnimIndex && ANIMSTATE::DEAD != m_eAnimState)
		Key_Check(TimeDelta);

	if (OBJSTATE::DISABLE == m_eState)
		m_pModel->Set_Animation(m_iAnimation, false);
	else
		m_pModel->Set_Animation(m_iAnimation, m_bContinue);

	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	Update_Force(TimeDelta, m_pNavigation);

	if (nullptr != m_pHitBox)
		m_pHitBox->Update_CollisionTime(TimeDelta);

	Check_Pause();

	Update_Collider();
	Update_UI(TimeDelta);
	Fix_PositionZ();

	Check_WeaponState();
	Update_HitType();

	m_pNavigation->Update(XMMatrixIdentity());


	return (_int)m_eState;
}

_int CIron_Android9S::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bPause)
		TimeDelta = 0.0;

	if (false == m_bMiniGamePause)
		return (_int)m_eState;

	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);

	Update_Collider();
	//Update_UI(TimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::NAVIGATION, m_pNavigation);

	return _int();
}

HRESULT CIron_Android9S::Render()
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

void CIron_Android9S::Set_Gauntlet(CIronGauntlet * pGauntlet)
{
	if (nullptr == pGauntlet)
		return;

	m_pGauntlet = pGauntlet;
}

void CIron_Android9S::Set_Animation(_uint iAnimIndex)
{
	m_iAnimation = iAnimIndex;
}

void CIron_Android9S::Update_HitType()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
	m_iTargetAnimation = iCurAnimIndex;

	if (ANIM_IRON9S_JUMPATTACK_DW == iCurAnimIndex)
		m_eHitType = HIT::SINGLE;
	else
		m_eHitType = HIT::NONE;
}

void CIron_Android9S::Key_Check(_double TimeDelta)
{
	if (OBJSTATE::DISABLE == m_eState)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_bool bMove = false;

	if (pGameInstance->Key_Pressing(DIK_A))
	{
		if (!m_pTransform->IsJump() && !m_pModel->Get_Blend() && m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::JUMPATTACK)
		{
			bMove = true;
			m_eAnimState = ANIMSTATE::RUN;

			m_iAnimation = ANIM_IRON9S_RUN;
			m_bContinue = true;
		}

		if (DIRECTION::LEFT != m_eDirection)
			m_LookLerpTime = 0.0;

		m_eDirection = DIRECTION::LEFT;

		LerpToCameraLeft(TimeDelta);
	}

	else if (pGameInstance->Key_Pressing(DIK_D))
	{
		if (!m_pTransform->IsJump() && !m_pModel->Get_Blend() && m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::JUMPATTACK)
		{
			bMove = true;
			m_eAnimState = ANIMSTATE::RUN;

			m_iAnimation = ANIM_IRON9S_RUN;
			m_bContinue = true;
		}

		if (DIRECTION::RIGHT != m_eDirection)
			m_LookLerpTime = 0.0;

		m_eDirection = DIRECTION::RIGHT;

		LerpToCameraRight(TimeDelta);
	}

	_bool bLkeyDown = false;
	if ((bLkeyDown = pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_LB)) || m_bLbuttonDown)
	{
		bMove = true;
		Check_AttackState(TimeDelta, bLkeyDown);
	}

	_bool bRKeyDown = false;
	if ((bRKeyDown = pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_RB)) || m_bRbuttonDown)
	{
		bMove = true;
		Check_RightAttackState(TimeDelta, bRKeyDown);
	}

	_bool bKeyDown = false;
	if ((bKeyDown = pGameInstance->Key_Down(DIK_SPACE)) || m_pTransform->IsJump())
	{
		bMove = true;
		Check_JumpState(TimeDelta, bKeyDown);
	}

	if (false == bMove)
		Check_Idle();
}

void CIron_Android9S::Check_Idle()
{
	m_iAnimation = ANIM_IRON9S_IDLE;
	m_bContinue = true;

}

void CIron_Android9S::Check_JumpState(_double TimeDelta, _bool& bKeyDown)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (m_eAnimState != ANIMSTATE::ATTACK /*&& m_eAnimState != ANIMSTATE::SLIDING*/)
	{
		if (m_eAnimState != ANIMSTATE::JUMPATTACK)
		{
			if (ANIM_IRON9S_JUMP_START != iCurAnimIndex && ANIM_IRON9S_JUMP_DW != iCurAnimIndex && ANIM_IRON9S_DOUBLEJUMP != iCurAnimIndex)
			{
				m_bContinue = false;
				m_iAnimation = ANIM_IRON9S_JUMP_START;

				m_pTransform->Set_Jump(true);
			}

			m_eAnimState = ANIMSTATE::JUMP;
		}

		if (ANIM_IRON9S_JUMP_START == iCurAnimIndex)
		{
			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_IRON9S_JUMP_DW;
				m_pModel->Set_NoneBlend();
			}

			if (bKeyDown)
			{
				if (ANIM_IRON9S_DOUBLEJUMP != m_iAnimation)
				{
					m_iAnimation = ANIM_IRON9S_DOUBLEJUMP;
					m_pTransform->Initialize_JumpTime();

					m_bContinue = false;
				}
			}
		}

		if (ANIM_IRON9S_JUMP_DW == iCurAnimIndex)
		{
			if (bKeyDown)
			{
				if (ANIM_IRON9S_DOUBLEJUMP != m_iAnimation)
				{
					m_iAnimation = ANIM_IRON9S_DOUBLEJUMP;
					m_pTransform->Initialize_JumpTime();

					m_bContinue = false;
				}
			}
		}

		m_bContinue = false;

		if ((ANIM_IRON9S_JUMP_START == iCurAnimIndex || ANIM_IRON9S_JUMP_DW == iCurAnimIndex || ANIM_IRON9S_DOUBLEJUMP == iCurAnimIndex) && false == m_pTransform->Get_PowerAtt())
		{
			m_pTransform->Jump(TimeDelta, m_pNavigation);

			_float fY = 0.f;
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_pTransform->Set_Jump(false);
				m_pTransform->Initialize_JumpTime();

				_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
				vPos = XMVectorSetY(vPos, fY);
				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			}
		}
	}
}

void CIron_Android9S::Check_RightAttackState(_double TimeDelta, _bool & bRKeyDown)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
	_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();
	_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

	if (!m_pTransform->IsJump())
	{
		if (ANIM_IRON9S_KICK1 != iCurAnimIndex && ANIM_IRON9S_DASH_KICK != iCurAnimIndex)
		{
			if (m_iAnimation != ANIM_IRON9S_KICK1)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
				CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack2.wav", CSoundMgr::CHANNELID::VOICE);
			}

			m_iAnimation = ANIM_IRON9S_KICK1;
			m_eAnimState = ANIMSTATE::ATTACK;

			m_bRbuttonDown = true;
			m_bLbuttonDown = false;

			m_pGauntlet->Set_Animation(IRONGAUNTLET_KICK1);
			m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);
		}

		if (ANIM_IRON9S_KICK1 == iCurAnimIndex)
		{
			if (LinkMinRatio <= PlayTimeRatio && LinkMaxRatio > PlayTimeRatio)
			{
				if (bRKeyDown)
				{
					if (m_iAnimation != ANIM_IRON9S_DASH_KICK)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
						CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack2.wav", CSoundMgr::CHANNELID::VOICE);
					}

					m_iAnimation = ANIM_IRON9S_DASH_KICK;

					m_pGauntlet->Set_Animation(IRONGAUNTLET_DASH_KICK);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);
				}
			}
			else if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_bRbuttonDown = false;

				Set_GauntletIdle();
			}
		}
		else if (ANIM_IRON9S_DASH_KICK == iCurAnimIndex)
		{
			if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_bRbuttonDown = false;

				Set_GauntletIdle();
			}
		}
	}
}

void CIron_Android9S::Check_HitState(_double TimeDelta)
{
	if (ANIMSTATE::HIT == m_eAnimState)
	{
		m_HitLerpTime += TimeDelta;

		if (m_HitLerpTime >= 1.0)
			m_HitLerpTime = 1.0;

		_vector vScale = m_pTransform->Get_Scale();
		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

		_vector vMyPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vTargetPos = XMLoadFloat4(&m_vHitTargetPos);

		vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vMyPos));

		_vector vDir = XMVector3Normalize(vTargetPos - vMyPos);

		_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, _float(m_HitLerpTime)));

		 vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

		 m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
		 m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook * XMVectorGetZ(vScale));

		 _uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		 if (ANIM_IRON9S_HIT1 == iCurAnimIndex || ANIM_IRON9S_HIT2 == iCurAnimIndex)
		 {
			 if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			 {
				 m_iAnimation = ANIM_IRON9S_IDLE;
				 m_eAnimState = ANIMSTATE::IDLE;

				 m_bContinue = true;
			 }
		 }

		 if (m_pTransform->IsJump())
			 m_pTransform->Jump(TimeDelta, m_pNavigation);

		 _float fY = 0.f;
		 if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		 {
			 _vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			 vPos = XMVectorSetY(vPos, fY);

			 m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			 m_pTransform->Set_Jump(false);

			 m_pTransform->Initialize_JumpTime();
		 }
	}
}

void CIron_Android9S::Check_Dead()
{
	if (m_tObjectStatusDesc.fHp <= 0.f)
	{
		if (false == m_pTransform->IsJump())
		{
			m_iAnimation = ANIM_IRON9S_DEAD;
			m_bContinue = false;

			m_eAnimState = ANIMSTATE::DEAD;
			m_bCollision = false;
		}
	}
}

void CIron_Android9S::Check_AttackState(_double TimeDelta, _bool& bLKeyDown)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
	_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

	_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

	if (!m_pTransform->IsJump())
	{
		if (ANIM_IRON9S_PUNCH1 != iCurAnimIndex && ANIM_IRON9S_PUNCH2 != iCurAnimIndex && ANIM_IRON9S_PUNCH3 != iCurAnimIndex)
		{
			if (m_iAnimation != ANIM_IRON9S_PUNCH1)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
				CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack1.wav", CSoundMgr::CHANNELID::VOICE);
			}

			m_iAnimation = ANIM_IRON9S_PUNCH1;
			m_bContinue = false;

			m_eAnimState = ANIMSTATE::ATTACK;

			m_pGauntlet->Set_Animation(IRONGAUNTLET_PUNCH1);
			m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);

			m_bLbuttonDown = true;
			m_bRbuttonDown = false;
		}

		if (ANIM_IRON9S_PUNCH1 == iCurAnimIndex)
		{
			if (LinkMinRatio <= PlayTimeRatio && LinkMaxRatio > PlayTimeRatio)
			{
				if (bLKeyDown)
				{
					if (m_iAnimation != ANIM_IRON9S_PUNCH2)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
						CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack1.wav", CSoundMgr::CHANNELID::VOICE);
					}

					m_iAnimation = ANIM_IRON9S_PUNCH2;

					m_pGauntlet->Set_Animation(IRONGAUNTLET_PUNCH2);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);
				}
			}
			else if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
				m_bLbuttonDown = false;

				Set_GauntletIdle();
			}
		}

		if (ANIM_IRON9S_PUNCH2 == iCurAnimIndex)
		{
			if (LinkMinRatio <= PlayTimeRatio && LinkMaxRatio > PlayTimeRatio)
			{
				if (bLKeyDown)
				{
					if (m_iAnimation != ANIM_IRON9S_PUNCH3)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
						CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack1.wav", CSoundMgr::CHANNELID::VOICE);
					}

					m_iAnimation = ANIM_IRON9S_PUNCH3;

					m_pGauntlet->Set_Animation(IRONGAUNTLET_PUNCH3);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);
				}
			}
			else if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
				m_bLbuttonDown = false;

				Set_GauntletIdle();
			}
		}

		if (ANIM_IRON9S_PUNCH3 == iCurAnimIndex)
		{
			if (LinkMinRatio <= PlayTimeRatio && LinkMaxRatio > PlayTimeRatio)
			{
				if (bLKeyDown)
				{
					if (m_iAnimation != ANIM_IRON9S_PUNCH1)
					{
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
						CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack1.wav", CSoundMgr::CHANNELID::VOICE);
					}

					m_iAnimation = ANIM_IRON9S_PUNCH1;

					m_pGauntlet->Set_Animation(IRONGAUNTLET_PUNCH1);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);
				}
			}
			else if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
				m_bLbuttonDown = false;

				Set_GauntletIdle();
			}
		}

	}
	else
	{
		if (ANIM_IRON9S_JUMPATTACK_START != iCurAnimIndex && ANIM_IRON9S_JUMPATTACK_DW != iCurAnimIndex && ANIM_IRON9S_JUMPATTACK_END != iCurAnimIndex)
		{
			if (m_iAnimation != ANIM_IRON9S_JUMPATTACK_START)
			{
				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
				CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Attack1.wav", CSoundMgr::CHANNELID::VOICE);
			}

			m_iAnimation = ANIM_IRON9S_JUMPATTACK_START;
			m_eAnimState = ANIMSTATE::JUMPATTACK;

			m_pTransform->Set_PowerAtt(true);
			m_bContinue = false;

			m_bLbuttonDown = true;
			Create_JumpAttackEffect();
		}

		if (ANIM_IRON9S_JUMPATTACK_START == iCurAnimIndex)
		{
			if (m_pModel->IsFinish_Animation())
			{
				m_iAnimation = ANIM_IRON9S_JUMPATTACK_DW;
				m_pModel->Set_NoneBlend();
			}
		}

		if (ANIM_IRON9S_JUMPATTACK_START == iCurAnimIndex || ANIM_IRON9S_JUMPATTACK_DW == iCurAnimIndex)
		{
			_float fY = 0.f;
			if (ANIM_IRON9S_JUMPATTACK_DW == iCurAnimIndex)
				m_pTransform->Go_DirWithSpeed(TimeDelta, XMVectorSet(0.f, -1.f, 0.f, 0.f), JUMPATTACKSPEED);

			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				m_iAnimation = ANIM_IRON9S_JUMPATTACK_END;
				
				_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

				vPos = XMVectorSetY(vPos, fY);
				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			}
		}

		if (ANIM_IRON9S_JUMPATTACK_END == iCurAnimIndex)
		{
			if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IRON9S_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;

				m_bContinue = true;
				m_pTransform->Set_Jump(false);
				m_pTransform->Set_PowerAtt(false);
				m_bLbuttonDown = false;

				m_pTransform->Initialize_JumpTime();

				Set_GauntletIdle();
			}
		}
	}
}

void CIron_Android9S::Set_GauntletIdle()
{
	m_pGauntlet->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pGauntlet->Set_NoneBlend(1);
}

void CIron_Android9S::LerpToCameraRight(_double TimeDelta)
{
	m_LookLerpTime += TimeDelta * 3;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	if (m_LookLerpTime <= 0.5)
		LerpToCameraBack(TimeDelta);
	else
	{
		if (XMVector3Equal(XMVectorZero(), XMLoadFloat3(&m_vCameraRight)))
		{
			CCameraMgr* pManager = CCameraMgr::Get_Instance();

			CCamera_IronFist* pCamera = pManager->Get_IronFistCamera();
			if (nullptr == pCamera)
				return;

			CTransform* pCameraTransform = pCamera->Get_Transform();
			if (nullptr == pCameraTransform)
				return;

			XMStoreFloat3(&m_vCameraRight, XMVector3Normalize(pCameraTransform->Get_State(CTransform::STATE::RIGHT)));
		}

		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		_vector vScale = m_pTransform->Get_Scale();

		vLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vCameraRight), _float(m_LookLerpTime)));
		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));
	}

	if (m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::JUMPATTACK)
	{
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED, m_pNavigation);

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fY = 0.f;

		if (false == m_pTransform->IsJump())
		{
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				vPos = XMVectorSetY(vPos, fY);
				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			}
		}
	}
}

void CIron_Android9S::LerpToCameraLook(_double TimeDelta)
{
	if (XMVector3Equal(XMVectorZero(), XMLoadFloat3(&m_vCameraLook)))
	{
		CCameraMgr* pManager = CCameraMgr::Get_Instance();

		CCamera_IronFist* pCamera = pManager->Get_IronFistCamera();
		if (nullptr == pCamera)
			return;

		CTransform* pCameraTransform = pCamera->Get_Transform();
		if (nullptr == pCameraTransform)
			return;

		XMStoreFloat3(&m_vCameraLook, XMVector3Normalize(pCameraTransform->Get_State(CTransform::STATE::LOOK)));
	}

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	vLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vCameraLook), _float(m_LookLerpTime)));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));
}

void CIron_Android9S::LerpToCameraBack(_double TimeDelta)
{
	if (XMVector3Equal(XMVectorZero(), XMLoadFloat3(&m_vCameraBack)))
	{
		CCameraMgr* pManager = CCameraMgr::Get_Instance();

		CCamera_IronFist* pCamera = pManager->Get_IronFistCamera();
		if (nullptr == pCamera)
			return;

		CTransform* pCameraTransform = pCamera->Get_Transform();
		if (nullptr == pCameraTransform)
			return;

		XMStoreFloat3(&m_vCameraBack, XMVector3Normalize(pCameraTransform->Get_State(CTransform::STATE::LOOK)) * -1.f);
	}

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	vLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vCameraBack), _float(m_LookLerpTime)));
	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));
}

void CIron_Android9S::LerpToCameraLeft(_double TimeDelta)
{
	m_LookLerpTime += TimeDelta * 3;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	if (m_LookLerpTime <= 0.5)
		LerpToCameraLook(TimeDelta);
	else
	{
		if (XMVector3Equal(XMVectorZero(), XMLoadFloat3(&m_vCameraLeft)))
		{
			CCameraMgr* pManager = CCameraMgr::Get_Instance();

			CCamera_IronFist* pCamera = pManager->Get_IronFistCamera();
			if (nullptr == pCamera)
				return;

			CTransform* pCameraTransform = pCamera->Get_Transform();
			if (nullptr == pCameraTransform)
				return;

			XMStoreFloat3(&m_vCameraLeft, XMVector3Normalize(pCameraTransform->Get_State(CTransform::STATE::RIGHT)) * -1.f);
		}

		_vector vRight = XMVectorZero();
		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
		_vector vScale = m_pTransform->Get_Scale();

		vLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vCameraLeft), _float(m_LookLerpTime)));
		vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));
	}

	if (m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::JUMPATTACK)
	{
		m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED, m_pNavigation);

		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fY = 0.f;

		if (false == m_pTransform->IsJump())
		{
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				vPos = XMVectorSetY(vPos, fY);
				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
			}
		}
	}
}

_bool CIron_Android9S::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		_bool bCheck = false;
		if ((bCheck = m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB())))
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;

		if (m_eHitType == HIT::SINGLE)
		{
			if(m_pAttackBox->Collision_SphereToAABB(pGameObject->Get_ColliderAABB()))
			{
				m_iFlagCollision |= FLAG_COLLISION_HITBOX;
				return true;
			}
		}

		if (bCheck)
			return true;
	}
	else if (iColliderType == (_uint)CCollider::TYPE::SPHERE)
	{
		if (pGameObject->Get_WholeRangeAttack())
		{
			if (true == m_pTransform->IsJump())
				return false;
		}

		if (m_pHitBox->Collision_AABBToSphere(pGameObject->Get_ColliderSphere()))
		{
			m_iFlagCollision |= FLAG_COLLISION_ATTACKBOX;
			return true;
		}
	}
	return false;
}

void CIron_Android9S::Collision(CGameObject * pGameObject)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));

		if (m_eHitType == HIT::SINGLE)
		{
			_double PlayRatio = m_pModel->Get_PlayTimeRatio();

			_uint iDamage = 0;
			if (true == m_pAttackBox->Result_SphereToAABB(this, pGameObject, pGameObject->Get_ColliderAABB(), &iDamage))
			{
				CIron_Humanoid_Small* pMonster = static_cast<CIron_Humanoid_Small*>(pGameObject);
				if (CIron_Humanoid_Small::ANIMSTATE::GUARD == pMonster->Get_AnimState())
					iDamage = 0;

				pGameObject->Set_Hp(pGameObject->Get_Hp() - iDamage);

				CUI_HUD_Damage::UIDAMAGEDESC	desc;

				desc.eType = CUI_HUD_Damage::DAMAGETYPE::MELEE;

				desc.iDamageAmount = iDamage;

				XMStoreFloat4(&desc.vWorldPos, m_pAttackBox->Get_WorldPos(CCollider::TYPE::SPHERE));

				CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

				pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

				RELEASE_INSTANCE(CGameInstance);
			}
		}
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		if (ANIM_IRON9S_HIT1 != iCurAnimIndex && ANIM_IRON9S_HIT2 != iCurAnimIndex && ANIMSTATE::HIT != m_eAnimState)
		{
			CCollider* pCollider = (CCollider*)pGameObject->Get_Component(COM_KEY_ATTACKBOX);
			if (nullptr == pCollider)
				return;

			_uint iDamage = 0;

			if (HIT::NONE != pGameObject->Get_HitType())
			{
				if (pCollider->Result_SphereToAABB(pGameObject, this, m_pHitBox, &iDamage))
				{
					m_iFlagUI |= FLAG_UI_CALL_HPBAR;
					m_iFlagUI |= FLAG_UI_CALL_DAMAGE;

					m_dCallHPBarUITimeDuration = 3.0;
					m_tObjectStatusDesc.fHp -= iDamage;
					m_iGetDamage = iDamage;

					if (m_tObjectStatusDesc.fHp < 0)
						m_tObjectStatusDesc.fHp = 0;

					_vector vTargetPos = pCollider->Get_WorldPos(CCollider::TYPE::SPHERE);
					vTargetPos = XMVectorSetW(vTargetPos, 1.f);

					_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

					if (m_iPreHitAnim == ANIM_IRON9S_HIT1)
						m_iAnimation = ANIM_IRON9S_HIT2;
					else if (m_iPreHitAnim == ANIM_IRON9S_HIT2)
						m_iAnimation = ANIM_IRON9S_HIT1;

					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::VOICE);
					CSoundMgr::Get_Instance()->PlaySound(L"IF9S_Hit.wav", CSoundMgr::CHANNELID::VOICE);

					m_iPreHitAnim = m_iAnimation;

					m_eAnimState = ANIMSTATE::HIT;
					m_bContinue = false;

					m_bLbuttonDown = false;
					m_bRbuttonDown = false;

					m_pTransform->Set_PowerAtt(false);

					m_HitLerpTime = 0.0;
					XMStoreFloat4(&m_vHitTargetPos, vTargetPos);

					if (m_pTransform->IsJump())
						m_pTransform->Set_FallTime();
				}
			}
		}
	}

	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (ANIM_9S_HIT_FRONT != iCurAnimIndex && ANIMSTATE::HIT != m_eAnimState)
		{
			m_iFlagUI |= FLAG_UI_CALL_HPBAR;
			m_iFlagUI |= FLAG_UI_CALL_DAMAGE;

			m_dCallHPBarUITimeDuration = 3.0;
			m_tObjectStatusDesc.fHp -= pGameObject->Get_RandomAtt();

			if (m_tObjectStatusDesc.fHp < 0)
				m_tObjectStatusDesc.fHp = 0;

			m_iAnimation = ANIM_9S_HIT_FRONT;
			m_eAnimState = ANIMSTATE::HIT;
			m_bContinue = false;

		}
	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CIron_Android9S::Check_WeaponState()
{
	if (nullptr == m_pGauntlet)
		return;

	if (WEAPONSTATE::IDLE == m_pGauntlet->Get_WeaponState())
	{
		_float4x4 WorldFloat4x4;
		XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

		_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
		_float4x4 BoneFloat4x4;
		XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

		_vector vDir;
		memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

		_vector vUp;
		memcpy(&vUp, &BoneFloat4x4.m[1], sizeof(_vector));

		vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));
		vUp = XMVector3Normalize(XMVector3TransformNormal(vUp, XMLoadFloat4x4(&WorldFloat4x4)));

		_float4 vPos;
		memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * 0.54f));
		XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vUp * -0.6f));

		memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

		m_pGauntlet->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
	}
	else
		m_pGauntlet->Update_Matrix(m_pTransform->Get_WorldMatrix());
}

void CIron_Android9S::Fix_PositionZ()
{
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	_vector vStartPos = IRONFIST_PLAYER_START_POSITION;

	vPos = XMVectorSetZ(vPos, XMVectorGetZ(vStartPos));

	m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
}

void CIron_Android9S::Set_FirstLook()
{
	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP));
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	vRight = vLook;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight * XMVectorGetX(vScale));
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook * XMVectorGetZ(vScale));
}

void CIron_Android9S::Notify(void * pMessage)
{
}

_int CIron_Android9S::VerifyChecksum(void * pMessage)
{
	return _int();
}

HRESULT CIron_Android9S::Update_UI(_double dDeltaTime)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (m_iFlagUI & FLAG_UI_CALL_DAMAGE)
	{
		CUI_HUD_Damage::UIDAMAGEDESC	desc;

		desc.eType = CUI_HUD_Damage::DAMAGETYPE::PLAYER;
		desc.iDamageAmount = m_iGetDamage;
		XMStoreFloat4(&desc.vWorldPos, m_pMiddlePoint->Get_WorldPos(CCollider::TYPE::SPHERE));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

		m_iFlagUI ^= FLAG_UI_CALL_DAMAGE;
	}
	if (true)
	{
		CUI_HUD_Hpbar_MiniGame::HPBARDESC	tDesc;

		tDesc.eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::PLAYER;
		tDesc.fCrntHPUV = m_tObjectStatusDesc.fHp / m_tObjectStatusDesc.fMaxHp;
		tDesc.iObjID = 3;

		pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR, &tDesc));
	}


	return S_OK;
}

HRESULT CIron_Android9S::Create_JumpAttackEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
	tPacket.tEffectMovement.vPosition.y = m_fBaseYPosition;
	tPacket.tEffectMovement.vPosition.x += 1.f;

	
	tPacket.ulID = CHECKSUM_EFFECT_9S_SPIRAL;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\9S_Spiral.dat", &tPacket);

	return S_OK;
}

HRESULT CIron_Android9S::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER - 0.3f;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_IRON9S, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"bone-1", "bone-1");
	m_pModel->Add_RefNode(L"Weapon_Idle", "bone3153");
	m_pModel->Add_RefNode(L"Weapon_Play_Left", "bone516");
	m_pModel->Add_RefNode(L"Weapon_Play_Right", "bone260");

	/* For.Com_Navigation*/
	_uint		iCurrentIndex = 0;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_NAVIGATION_IRONFIST, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	Engine::CCollider::DESC ColliderDesc;

	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.fRadius = m_pModel->Get_Radius();
	ColliderDesc.vScale = _float3(PLAYER_COLLIDER_HITBOX);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &ColliderDesc)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.fRadius = 1.1f;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_ATTACKBOX, (CComponent**)&m_pAttackBox, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CIron_Android9S * CIron_Android9S::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CIron_Android9S* pGameInstance = new CIron_Android9S(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CIron_Android9S");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CIron_Android9S::Clone(void * pArg)
{
	CIron_Android9S* pGameInstance = new CIron_Android9S(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CIron_Android9S");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CIron_Android9S::Free()
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
