#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "MainCamera.h"
#include "Katana.h"
#include "GraphicDebugger.h"
#include "UI_HUD_Hpbar_Player_Contents.h"
#include "UI_HUD_Hpbar_Player_Frame.h"
#include "UI_HUD_Hpbar_MiniGame.h"
#include "UI_HUD_Target.h"
#include "UI_HUD_Damage.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_Dialogue_EngageMessage.h"
#include "UI_Dialogue_HighlightMessage.h"
#include "UI_Core_Controller.h"
#include "UI_Core_WeaponSlot.h"
#include "UI_Core_Status.h"

#include "AfterImage.h"
#include "SoundMgr.h"
#include "MeshEffect.h"
#include "Coaster.h"
#include "Inventory.h"
#include "CameraMgr.h"
#include "EffectFactory.h"
#include "SoundMgr.h"

#include "Katana.h"
#include "LongKatana.h"
#include "Spear.h"
#include "Gauntlet.h"
#include "Pod.h"

#include "Android_9S.h"

// Temp
#include "DecalCube.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObjectModel(pDevice, pDeviceContext)
{

}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObjectModel(rhs)
{

}


HRESULT CPlayer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// For Debug Debug Debug Debug Debug Debug Debug Debug Debug
	m_bDebugNavigation = false;

	m_iAnimation = ANIM_IDLE;

	m_pModel->Set_CurrentAnimation(m_iAnimation);
	m_pModel->Set_Animation(m_iAnimation);

	m_eAnimState = ANIMSTATE::IDLE;
	m_bRun = true;

	m_iPassIndex = static_cast<_uint>(MESH_PASS_INDEX::ANIM);
	m_bCollision = true;
	
	static _uint iCPlayer_InstanceID = OriginID_2B;
	m_iObjID = iCPlayer_InstanceID++;

	m_tObjectStatusDesc.fHp = PLAYER_MAX_HEALTH;
	m_tObjectStatusDesc.fMaxHp = PLAYER_MAX_HEALTH;
	m_tObjectStatusDesc.iDef = PLAYER_DFL_DEFENCE;

	m_pTransform->Set_State(CTransform::STATE::POSITION, TUTORIAL_START_POSITION);
	if (FAILED(m_pNavigation->Init_Index(m_pTransform->Get_State(CTransform::STATE::POSITION))))
		return E_FAIL;

	m_fHitRecovery = PLAYER_HIT_RECOVERY;
	Set_RimLightColor(_float3(0.f, 0.f, 0.f));

	ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);
	m_pRenderer->Set_DOF_Distance(50.f);
	return S_OK;
}

_int CPlayer::Tick(_double TimeDelta)
{
	Key_Check_Common(TimeDelta);

	switch (m_ePrevInputState)
	{
	case Client::CPlayer::INPUTSTATE::INGAME:
		Key_Check_Ingame(TimeDelta);
		break;
	case Client::CPlayer::INPUTSTATE::UI:
		Key_Check_UI(TimeDelta);
		break;
	case Client::CPlayer::INPUTSTATE::NONE:
		break;
	default:
		break;
	}

	if (m_bPause)
		TimeDelta = 0.0;

	if (m_bMiniGamePause)
	{
		m_dCallHPBarUITimeDuration = 0.0;

		Update_UI(TimeDelta);
		return (_int)m_eState;
	}

	Update_UI(TimeDelta);
	Update_Collider();

	// Animation Set()
	m_pModel->Set_Animation(m_iAnimation, m_bContinue);
	// For Debug Debug Debug Debug Debug Debug Debug Debug Debug
	if (m_bDebugNavigation)
		m_pModel->Synchronize_Animation(m_pTransform);
	else
	{
		if (NAVISTATE::COASTER != m_eNaviState)
			m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation, TimeDelta);
	}
	// For Debug Debug Debug Debug Debug Debug Debug Debug Debug
	Check_Target();
	Check_WeaponState();
	Update_Pod();
	Check_AndroidDistance();
	SetUp_Bone1();

	Update_Force(TimeDelta, m_pNavigation);

	Update_Effects(static_cast<_float>(TimeDelta));
	Update_UnbeatableTime(TimeDelta);

	if (nullptr != m_pHitBox)
		m_pHitBox->Update_CollisionTime(TimeDelta);

	Shadow_BeTraced();
	SetUp_PlayerLook();
	Update_Alpha(TimeDelta);
	return (_int)m_eState;
}

_int CPlayer::LateTick(_double TimeDelta)
{
	if (nullptr == m_pRenderer)
		return -1;

	if (m_bMiniGamePause)
		return (_int)m_eState;

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	if (m_eAnimState == ANIMSTATE::AVOID || m_eAnimState == ANIMSTATE::SPURT)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::MOTIONBLUR, this);
	if (m_bPause)
		TimeDelta = 0.0;

	m_pNavigation->Culling(TimeDelta);
	m_pModel->Update_CombinedTransformationMatrix(TimeDelta);


	if (m_bSceneChange)
		m_bSceneChange = false;


	if (nullptr != m_pHitBox)
	{
		if (ANIMSTATE::AVOID != m_eAnimState)
			m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);
	}
	if (nullptr != m_pNavigation)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::NAVIGATION, m_pNavigation);


	return _int();
}

HRESULT CPlayer::Render()
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
		m_pModel->Set_MaterialTexture(i, aiTextureType_NORMALS, "g_texNormal");
		
		m_pModel->Render(i, m_iPassIndex);
	}
#ifdef _DEBUG
	/*TCHAR		szPosition[64];
	_vector		vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
	swprintf_s(szPosition, L"X:%.2f,Y:%.2f,Z:%.2f", XMVectorGetX(vPosition), XMVectorGetY(vPosition), XMVectorGetZ(vPosition));
	CGameInstance::Get_Instance()->Render_Font(L"Font_Arial", szPosition, _float2(g_iWinCX / 2.f, g_iWinCY / 2.f), XMVectorSet(1.f, 0.f, 0.f, 1.f));*/
#endif // _DEBUG

	return S_OK;
}



void CPlayer::Key_Check(_double TimeDelta)
{
	
}

void CPlayer::Key_Check_Common(_double TimeDelta)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_F1))
	{
		CUI_Core_Controller::COMMANDDESC	tDesc;

		if (CPlayer::INPUTSTATE::INGAME == m_ePrevInputState)
		{
			tDesc.iCommandFlag |= COMMAND_CORE_OPEN;
			pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))
				->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));
		}
		if (CPlayer::INPUTSTATE::UI == m_ePrevInputState)
		{
			tDesc.iCommandFlag |= COMMAND_CORE_CLOSE;
			pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))
				->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));
		}
	}

	// Test Code
	if (pGameInstance->Key_Down(DIK_F2))
	{
		//CUI_Dialogue_SystemMessage::UISYSMSGDESC	desc(rand() % 10 + 20, rand() % 300, rand() % 3);
		//pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &desc));

		//pGameInstance->Get_Observer(TEXT("OBSERVER_MINIGAMESCREEN"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAMESCREEN, nullptr));

		//EQUIPSTATE	eState = CPlayer::EQUIPSTATE::KATANA;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_QUICKSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_QUICKSLOT, &eState));
	}
	if (pGameInstance->Key_Down(DIK_F3))
	{

		//EQUIPSTATE	eState = CPlayer::EQUIPSTATE::LONGKATANA;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_QUICKSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_QUICKSLOT, &eState));

		//CUI_HUD_Hpbar_MiniGame::HPBARDESC	tDesc;

		//static _float	CrntHP = 1.f;

		//tDesc.eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::ENEMY;
		//tDesc.fCrntHPUV = CrntHP;
		//tDesc.fPrevHPUV = 1.f;
		//tDesc.iObjID = 3;

		//CrntHP -= 0.1f;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR, &tDesc));

		//CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(0, 0, CUI_Dialogue_SystemMessage::iMsgOnQuest);
		//pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));

		//CUI_Dialogue_HighlightMessage::EVENTID	eID = CUI_Dialogue_HighlightMessage::EVENTID::BANNED;
		//pGameInstance->Get_Observer(TEXT("OBSERVER_HIGHLIGHT_MSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HIGHLIGHTMSG, &eID));

		
	}
	if (pGameInstance->Key_Down(DIK_F4))
	{
		EQUIPSTATE	eState = CPlayer::EQUIPSTATE::SPEAR;

		pGameInstance->Get_Observer(TEXT("OBSERVER_QUICKSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_QUICKSLOT, &eState));

		//CUI_HUD_Hpbar_MiniGame::HPBARDESC	tDesc;

		//static _float	CrntHPb = 1.f;

		//tDesc.eTarget = CUI_HUD_Hpbar_MiniGame::TARGET::PLAYER;
		//tDesc.fCrntHPUV = CrntHPb;
		//tDesc.fPrevHPUV = 1.f;
		//tDesc.iObjID = 4;

		//CrntHPb -= 0.1f;

		//pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_MINIGAME"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_MINIGAME_HPBAR, &tDesc));

		//CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(rand() % 5 + 80, 0, CUI_Dialogue_SystemMessage::iMsgNeedChip);
		//pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));

		//CUI_Dialogue_HighlightMessage::EVENTID	eID = CUI_Dialogue_HighlightMessage::EVENTID::MISSION_CLEAR;
		//pGameInstance->Get_Observer(TEXT("OBSERVER_HIGHLIGHT_MSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HIGHLIGHTMSG, &eID));
	}

	if (pGameInstance->Key_Pressing(DIK_F3))
	{
		//CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::BGM);
		//CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::HACKING);
		//pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_HACKING"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HACKINGMSG, nullptr));
	}
	if (pGameInstance->Key_Pressing(DIK_F4))
	{
		//CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::BGM);
		//CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::HACKING);
		//CUI_Dialogue_EngageMessage::BOSSID	eID = (CUI_Dialogue_EngageMessage::BOSSID)(rand() % 4);
		//pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_ENGAGEMSG, &eID));
		//pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_HACKING_SSC"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HACKINGSSCMSG, nullptr));
	}
	// Test Code End

	if (pGameInstance->Key_Down(DIK_LEFTARROW))
	{
		m_pRenderer->Pre_ToneMap();
#ifdef _DEBUG
		int iIndex = m_pRenderer->Get_ToneMap();
		switch (iIndex)
		{
		case (_uint)CRenderer::TONE_MAP::LUMINANCEBASEDREINHARDTONE:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "LuminanceBase Reinhard ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::DIRECTXBASED:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "DirectX Based ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::UNCHARTED2V2:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "Uncharted2 ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::FILMIC:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "Filmic ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ROMBINDAHOUSETONEMAP:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "RombindaHouse ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ACES:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "ACES ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ACES2:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "ACES2 ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		default:
			string strNum = to_string(iIndex);
			GRAPHICDEBUGGER()->Notice("Mix Tone ", DEBUGCOLOR()::WHITE, strNum.c_str(), DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		}
#endif // _DEBUG
	}
	if (pGameInstance->Key_Down(DIK_RIGHTARROW))
	{
		m_pRenderer->Next_ToneMap();
#ifdef _DEBUG
		int iIndex = m_pRenderer->Get_ToneMap();
		switch (iIndex)
		{
		case (_uint)CRenderer::TONE_MAP::LUMINANCEBASEDREINHARDTONE:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "LuminanceBase Reinhard ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::DIRECTXBASED:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "DirectX Based ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::UNCHARTED2V2:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "Uncharted2 ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::FILMIC:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "Filmic ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ROMBINDAHOUSETONEMAP:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "RombindaHouse ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ACES:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "ACES ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		case (_uint)CRenderer::TONE_MAP::ACES2:
			GRAPHICDEBUGGER()->Notice("Tone ", DEBUGCOLOR()::WHITE, "ACES2 ", DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		default:
			string strNum = to_string(iIndex);
			GRAPHICDEBUGGER()->Notice("Mix Tone ", DEBUGCOLOR()::WHITE, strNum.c_str(), DEBUGCOLOR()::DARKGRAY, "Render", DEBUGCOLOR()::WHITE);
			break;
		}
#endif // _DEBUG
	}
	if (pGameInstance->Key_Down(DIK_DOWNARROW))
	{
		CDecalCube::DECALCUBE tCube;
		tCube.bContinue = false;
		tCube.eType = CDecalCube::TYPE::ARCH;
		tCube.iImageIndex = 6;
		tCube.dDuration = 2.5;
		tCube.vScale = _float4(15.f, 1.f, 15.f, 0.f);
		XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
		tCube.vPosition.y -= 0.25f;

		tCube.iIsColorGlow = 0;
		tCube.vGlowColor = CProportionConverter()(_DARKGOLDENROD, 33.f, false);

		tCube.iIsGlow = 0;

		if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
			return;
	}
	if (pGameInstance->Key_Down(DIK_UPARROW))
	{
		CDecalCube::DECALCUBE tCube;

		tCube.bContinue = false;

		tCube.eType = CDecalCube::TYPE::FOGRING;

		tCube.dDuration = 2.5;
		tCube.dIndexTime = 0.0;

		tCube.iImageIndex = 1;

		tCube.vRotationAxis = _float4(0.f, 1.f, 0.f, 0.f);
		tCube.dRotationSpeed = 0.0;

		XMStoreFloat4(&tCube.vDir, m_pTransform->Get_State(CTransform::STATE::LOOK));
		tCube.dMoveSpeed = 0.0;

		tCube.vScale = _float4(1.f, 1.f, 1.f, 0.f);
		tCube.fAddScale = 0.15f;
		

		XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));
		tCube.vPosition.y -= 0.125f;

		tCube.iIsColorGlow = 0;
		tCube.vGlowColor = CProportionConverter()(_DARKGOLDENROD, 33.f, false);

		tCube.iIsGlow = 0;

		if (FAILED(pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube)))
			return;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Key_Check_Ingame(_double TimeDelta)
{
	if (m_bMiniGamePause)
		return;

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();

	CMainCamera* pMainCam = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pMainCam == nullptr)
	{
		return;
	}

	if (pMainCam->Get_State() == OBJSTATE::DISABLE)
	{
		m_iAnimation = ANIM_IDLE;
		m_eAnimState = ANIMSTATE::IDLE;

		m_bContinue = true;
		m_pModel->Set_NoneBlend();

		m_pModel->Set_Animation(m_iAnimation, m_bContinue);
		m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation, TimeDelta);
		return;
	}

	if (ANIMSTATE::HIT == m_eAnimState)
	{
		LerpToHitTarget(TimeDelta);
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		if (m_tObjectStatusDesc.fHp >= m_tObjectStatusDesc.fMaxHp / 2.f)
		{
			m_pRenderer->Set_PostProcessing_Distortion();
		}
		else
		{
			m_pRenderer->Set_PostProcessing_GrayDistortion();
		}
		Check_JumpState_ForHit(TimeDelta);

		

		if (ANIM_HIT_FRONT == iCurAnimIndex || ANIM_HIT_BACK == iCurAnimIndex)
		{
			_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();

			if (LinkMaxRatio <= PlayTimeRatio)
			{
				m_iAnimation = ANIM_IDLE;
				m_eAnimState = ANIMSTATE::IDLE;
				m_UnbeatableTime = 0.0;
			}
			else
				return;
		}
		else
			return;
	}
	else if (ANIMSTATE::JUSTAVOID == m_eAnimState)
	{
		Check_AvoidAnimation();
		if (ANIMSTATE::IDLE != m_eAnimState)
			return;
	}
	else if (ANIMSTATE::JUSTAVOID_COUNTERATTACK == m_eAnimState)
	{
		LerpToHitTarget(TimeDelta);

		if (m_HitLerpTime >= 1.0)
		{
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			_double PlayTimeRatio = m_pModel->Get_PlayTimeRatio();
			if (ANIM_JUSTAVOID_COUNTERATTACK == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMinRatio() <= PlayTimeRatio)
				{
					m_pPod->Set_PodState(PODSTATE::COUNTERATTACK);
				}

				if (m_pModel->Get_CollisionMinRatio() <= PlayTimeRatio)
				{
					if ((false == m_pPod->Get_CounterAttack()) && PODSTATE::IDLE != m_pPod->Get_PodState())
					{
						m_pPod->Shoot_CounterAttack(XMLoadFloat4(&m_vCounterAttPos));
						
						m_pPod->Set_CounterAttack(true);
					}
				}

				if (m_pModel->Get_LinkMaxRatio() <= PlayTimeRatio)
				{
					m_iAnimation = ANIM_IDLE;
					m_eAnimState = ANIMSTATE::IDLE;
					m_bContinue = true;
					m_pPod->Set_CounterAttack(false);
					m_pPod->Set_PodState(PODSTATE::IDLE);

					m_bBulletTime = false;
				}
				else
					return;
			}
			else
				return;
		}
		else
			return;
	}

	_bool bPlayAnim = false;
	_bool bDown = false;		//LB
	_bool bRightDown = false;	//RB
	_bool bJumpKeyDown = false;

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pGameInstance->Key_Down(DIK_9)) //test
	{
		if (nullptr != m_pAndroid9S)
		{
			if (CAndroid_9S::ANDROIDSTATE::IDLE == m_pAndroid9S->Get_AndroidState())
				m_pAndroid9S->Set_AndroidState(CAndroid_9S::ANDROIDSTATE::BATTLE);
			else
			{
				m_pAndroid9S->Set_AndroidState(CAndroid_9S::ANDROIDSTATE::IDLE);
				m_pAndroid9S->Set_Idle();
			}
		}
	}

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
	if (pGameInstance->Key_Down(DIK_4))
	{
		m_pRenderer->SSAO();
	}

	if (pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		m_bRun = false;
		m_bSpurt = true;
		m_bWalk = false;
	}
	else
	{
		m_bRun = true;
		m_bSpurt = false;
		m_bWalk = false;
	}


	//if (pGameInstance->Key_Pressing(DIK_LCONTROL))		
	//{
	//	m_bRun = true;
	//	m_bSpurt = false;
	//	m_bWalk = false;
	//}


	_bool bGKeyDown = false;

	if ((bGKeyDown = pGameInstance->Key_Pressing(DIK_G)) || m_bLaserAttack)
	{
		if (true == Is_ProgramChipInstalled(CPlayer::PROGRAM_LASER))
		{
			if (nullptr != m_pPod)
			{
				if (m_pPod->Get_SkillCoolTime() <= 0.0)
				{
					if (bGKeyDown)
					{
						if (!m_bLaserAttack)
						{
							_vector vCamLook = Get_CameraLook();
							if (XMVector3Equal(XMVectorZero(), vCamLook))
								return;

							XMStoreFloat3(&m_vPreLaserLook, XMVector3Normalize(vCamLook));
						}
						m_pPod->Set_PodState(PODSTATE::LASERATTACK);
						m_pPod->Create_ChargingEffect();
						pSoundMgr->PlaySound(TEXT("PodLaser_Ing.mp3"), CSoundMgr::POD);
						m_bLaserAttack = true;
					}
					else
					{
						if (m_bLaserAttack)
						{
							if (m_bLaserAttack)
							{
								m_pPod->Set_PodState(PODSTATE::LASERATTACK);

								bPlayAnim = true;
								m_bContinue = false;

								m_eAnimState = ANIMSTATE::LASERATTACK;
								if (m_iAnimation != ANIM_POD_LASERDW)
								{
									LerpToLaserLook(TimeDelta);

									m_iAnimation = ANIM_POD_LASERSTART;
									if (ANIM_POD_LASERSTART == m_pModel->Get_CurrentAnimation())
									{
										if (m_pModel->Get_LinkMaxRatio() < m_pModel->Get_PlayTimeRatio())
										{
											m_iAnimation = ANIM_POD_LASERDW;
											m_bLaserShootStart = true;
										}
									}
								}
								else
								{
									bPlayAnim = true;
									m_bContinue = false;
									m_eAnimState = ANIMSTATE::LASERATTACK;
									m_iAnimation = ANIM_POD_LASERDW;

									if (ANIM_POD_LASERDW == m_pModel->Get_CurrentAnimation())
									{
										if (m_bLaserShootStart)
										{
											m_pPod->Delete_ChargingEffect();
											m_pPod->Create_Laser();
											pSoundMgr->StopSound(CSoundMgr::POD);
											pSoundMgr->PlaySound(TEXT("PodLaser_Fire.mp3"), CSoundMgr::POD);

											m_bLaserShootStart = false;
										}
										if (m_pModel->Get_LinkMaxRatio() < m_pModel->Get_PlayTimeRatio())
										{
											bPlayAnim = false;
											m_bContinue = false;
											m_bLaserAttack = false;

											m_eAnimState = ANIMSTATE::IDLE;
										}
									}
								}
							}
						}
					}
				}
			}
		}/* if (true == Is_ProgramChipInstalled(CPlayer::PROGRAM_LASER)) */
		else
		{
			if (false == m_bIsSysMsgSendOut)
			{
				CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(ITEMCODE_ENFORCECHIP_SKILL_LASER, 0, CUI_Dialogue_SystemMessage::iMsgNeedChip);
				pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
				m_bIsSysMsgSendOut = true;
			}
		}
	}
	if (pGameInstance->Key_Up(DIK_G))
	{
		m_bIsSysMsgSendOut = false;
	}


	if (pGameInstance->Key_Pressing(DIK_LCONTROL))
	{
		if (true == Is_ProgramChipInstalled(CPlayer::PROGRAM_MACHINEGUN))
		{
			if (PODSTATE::HANG != m_pPod->Get_PodState() && PODSTATE::LASERATTACK != m_pPod->Get_PodState())
				m_pPod->Set_PodState(PODSTATE::ATTACK);
		}
		else
		{
			if (false == m_bIsSysMsgSendOut)
			{
				CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(ITEMCODE_ENFORCECHIP_SKILL_MACHINEGUN, 0, CUI_Dialogue_SystemMessage::iMsgNeedChip);
				pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
				m_bIsSysMsgSendOut = true;
			}
		}
	}
	else
	{
		if (PODSTATE::HANG != m_pPod->Get_PodState() && PODSTATE::LASERATTACK != m_pPod->Get_PodState())
			m_pPod->Set_PodState(PODSTATE::IDLE);
	}
	if (pGameInstance->Key_Up(DIK_LCONTROL))
	{
		m_bIsSysMsgSendOut = false;
	}

	if (pGameInstance->Key_Down(DIK_E))
	{
		if (EQUIPSTATE::LONGKATANA == m_eEquipState)
			m_bCharge = !m_bCharge;
	}

	if (pGameInstance->Key_Down(DIK_F))
	{
		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_COMMON_INTERACTION, this));
	}

	if (pGameInstance->Key_Down(DIK_Q))
	{
		m_bTargeting = !m_bTargeting;
		if (nullptr != m_pPod)
			m_pPod->Set_Targeting(m_bTargeting);
	}

	if ((pGameInstance->Key_Down(DIK_C) || ANIM_FRONT_AVOID == m_iAnimation || ANIM_BACK_AVOID == m_iAnimation || ANIM_RIGHT_AVOID == m_iAnimation || ANIM_LEFT_AVOID == m_iAnimation) && ANIMSTATE::LASERATTACK != m_eAnimState)
		Check_AvoidState(bPlayAnim);


	if (((bDown = pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_LB)) || m_bLButtonDown))
	{
		if (m_eAnimState != ANIMSTATE::JUMP && m_eAnimState != ANIMSTATE::JUMPATTACK && m_eAnimState != ANIMSTATE::JUMPPOWERATTACK)
		{
			_bool bCheck = false;
			m_bContinue = false;

			if (m_eAnimState != ANIMSTATE::ATTACK)
				bCheck = true;

			if (ANIMSTATE::POWERATTACK == m_eAnimState)
				m_bRButtonDown = false;

			m_eAnimState = ANIMSTATE::ATTACK;

			if (EQUIPSTATE::KATANA == m_eEquipState)																				// Katana
			{
				m_pKatana->Set_WeaponState(WEAPONSTATE::ATTACK);

				bPlayAnim = true;
				m_bLButtonDown = true;

				_double Time = m_pModel->Get_PlayTimeRatio();

				_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
				_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

				if (ANIM_KATANA_ATTACK1 <= m_iAnimation && ANIM_KATANA_ATTACK7 >= m_iAnimation)
				{
					//if (m_pKatana->Get_DissolveState() == DISSOLVE_STATE::DISSOLVE_UP
					//	&& ANIM_KATANA_ATTACK1 == m_iAnimation
					//	)
					//{
					//	Set_KatanaIdle();
					//}
					m_pKatana->Set_DissolveIdle();


					if (m_iAnimation == m_pModel->Get_CurrentAnimation())
					{
						if (m_pModel->Check_BlendFinishMoment())
						{
							//pGameInstance->Notice("BlendFinish");
							//Set_KatanaIdle();
							m_pKatana->Start_Trail();
						}



						if (LinkMinRatio <= Time && LinkMaxRatio > Time)
						{
							if (bDown)
							{
								if (ANIM_KATANA_ATTACK1 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK2 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK2;
									m_pKatana->Set_Animation(KATANA_ATTACK2);
								}

								else if (ANIM_KATANA_ATTACK2 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK3 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK3;
									m_pKatana->Set_Animation(KATANA_ATTACK3);
								}

								else if (ANIM_KATANA_ATTACK3 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK4 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L4.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK4;
									m_pKatana->Set_Animation(KATANA_ATTACK4);
								}

								else if (ANIM_KATANA_ATTACK4 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK5 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L5.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK5;
									m_pKatana->Set_Animation(KATANA_ATTACK5);
								}

								else if (ANIM_KATANA_ATTACK5 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK6 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L6.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK6;
									m_pKatana->Set_Animation(KATANA_ATTACK6);
								}

								else if (ANIM_KATANA_ATTACK6 == m_iAnimation)
								{
									if (ANIM_KATANA_ATTACK7 != m_iAnimation)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Katana_L7.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_KATANA_ATTACK7;
									m_pKatana->Set_Animation(KATANA_ATTACK7);
								}

								if (ANIM_KATANA_ATTACK7 == m_iAnimation)
									m_pKatana->Set_Independence(true);
							}
						}

						if (Time > LinkMaxRatio)
						{
							if (ANIM_KATANA_ATTACK6 == m_iAnimation)
							{
								//if (0.32 <= m_pModel->Get_PlayTimeRatio())
								if (0.52 <= m_pModel->Get_PlayTimeRatio())
								{
									bPlayAnim = false;
									m_eAnimState = ANIMSTATE::IDLE;
									m_bLButtonDown = false;
									m_pKatana->End_Trail();

									//Set_KatanaIdle();
									// Katana Dissolve
									m_pKatana->Set_Dissolve();

								}
							}
							else
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;
								m_pKatana->End_Trail();

								_uint iKatanaAnim = m_pKatana->Get_AnimationIndex();
								if (KATANA_ATTACK3 == iKatanaAnim || KATANA_ATTACK4 == iKatanaAnim || KATANA_ATTACK5 == iKatanaAnim)
									m_pKatana->Set_Independence(true);
								else {
									if (m_iAnimation == ANIM_KATANA_ATTACK1 ||
										m_iAnimation == ANIM_KATANA_ATTACK2)
									{
										Set_KatanaIdle();
									}
									else
									{
										m_pKatana->Set_Dissolve();
									}
								}

							}
						}
					}
				}

				if (bCheck)
				{
					if (ANIM_KATANA_ATTACK1 != m_iAnimation)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Katana_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
						m_bKatanaR = false;
					}
					m_iAnimation = ANIM_KATANA_ATTACK1;
					m_pKatana->Set_Animation(KATANA_ATTACK1);
					m_pKatana->Set_Independence(false);
				}
			}

			else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
			{
				if (!m_bLButtonDown)
				{
					m_iAnimation = ANIM_LONGKATANA_ATTACK1;
					m_pLongKatana->Set_Animation(3);
					m_bLButtonDown = true;

					m_pLongKatana->Set_PlayerNonLerp(false);
					m_pLongKatana->Set_Independence(false);
				}

				else if (3 == m_pLongKatana->Get_ModelCurrentAnim())
				{
					if (0.04 >= m_pLongKatana->Get_PlayTimeRatio())
					{
						m_pLongKatana->Set_DissolveIdle();
						m_pLongKatana->Start_Trail();
					}
					
					//
					pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
					pSoundMgr->PlaySound(TEXT("LKatana_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
				}

				bPlayAnim = true;
				m_bLButtonDown = true;
				m_pLongKatana->Set_WeaponState(WEAPONSTATE::ATTACK);

				if (ANIM_LONGKATANA_ATTACK1 <= m_iAnimation && ANIM_LONGKATANA_ATTACK4 >= m_iAnimation)
				{
					//if (DISSOLVE_STATE::DISSOLVE_UP == m_pLongKatana->Get_DissolveState())
					//{
					//	Set_LongKatanaIdle();
					//}
					m_pLongKatana->Set_DissolveIdle();


					_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
					_double TimeRatio = m_pModel->Get_PlayTimeRatio();

					_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
					_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

					if (m_iAnimation == iCurAnimIndex)
					{
						if (m_pModel->Check_BlendFinishMoment())
						{
							//pGameInstance->Notice("BlendFinish");
							//m_pLongKatana->Create_SwordTrail(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)));
							m_bLongKatanaTrail_Create = true;
						}

						if (ANIM_LONGKATANA_ATTACK1 == m_iAnimation)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_LONGKATANA_ATTACK2)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("LKatana_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
									}
									m_iAnimation = ANIM_LONGKATANA_ATTACK2;
									m_pLongKatana->Set_Animation(41);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_LongKatanaIdle();
								m_pLongKatana->End_Trail();
								//m_pLongKatana->Set_Dissolve();
							}
						}
						else if (ANIM_LONGKATANA_ATTACK2 == m_iAnimation)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_LONGKATANA_ATTACK3)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("LKatana_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_LONGKATANA_ATTACK3;
									m_pLongKatana->Set_Animation(28);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;
								m_pLongKatana->End_Trail();
								Set_LongKatanaIdle();
								//m_pLongKatana->Set_Dissolve();
							}
						}
						else if (ANIM_LONGKATANA_ATTACK3 == m_iAnimation)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_LONGKATANA_ATTACK4)
									{
										m_bLKatanaRush = true;
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("LKatana_L4.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_LONGKATANA_ATTACK4;
									m_pLongKatana->Set_Animation(4);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;
								m_pLongKatana->End_Trail();
								//Set_LongKatanaIdle();
								m_pLongKatana->Set_Dissolve();
								m_bLKatanaRush = false;
							}
						}
						else if (ANIM_LONGKATANA_ATTACK4 == m_iAnimation)
						{
							if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;
								m_pLongKatana->End_Trail();

								//Set_LongKatanaIdle();
								m_pLongKatana->Set_Dissolve();
								m_bLKatanaRush = false;
							}
						}
					}
				}
			}
			else if (EQUIPSTATE::SPEAR == m_eEquipState)
			{
				if (!m_bLButtonDown)
				{
					m_iAnimation = ANIM_SPEAR_ATTACK1;
					m_pSpear->Set_Animation(96);

					pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
					pSoundMgr->PlaySound(TEXT("Spear_L1.mp3"), CSoundMgr::P_WEAPONSOUND);

					m_bLButtonDown = true;
					m_bThrowSpear = false;

					m_pSpear->Set_PlayerNonLerp(false);
					m_pSpear->Set_Independence(false);
				}

				bPlayAnim = true;
				m_bLButtonDown = true;
				m_pSpear->Set_WeaponState(WEAPONSTATE::ATTACK);

				if (m_iAnimation >= ANIM_SPEAR_ATTACK1 && m_iAnimation <= ANIM_SPEAR_ATTACK6)
				{
					_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
					_double TimeRatio = m_pModel->Get_PlayTimeRatio();

					_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
					_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

					if (m_iAnimation == iCurAnimIndex)
					{
						if (ANIM_SPEAR_ATTACK1 == iCurAnimIndex)
						{
							// SpearAttack
							if (0.10797 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[0])
								{
									m_arrIsSpearAttackEffectCreated[0] = true;
									m_pSpear->Create_AttackEffect();
								}
							}

							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_SPEAR_ATTACK2)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Spear_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
									}
									m_iAnimation = ANIM_SPEAR_ATTACK2;
									m_pSpear->Set_Animation(54);


								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);
							}
						}
						else if (ANIM_SPEAR_ATTACK2 == iCurAnimIndex)
						{
							if (0.0921 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[1])
								{
									m_arrIsSpearAttackEffectCreated[1] = true;
									m_pSpear->Create_AttackEffect();
								}
							}

							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_SPEAR_ATTACK3)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Spear_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_SPEAR_ATTACK3;
									m_pSpear->Set_Animation(51);

								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);

							}
						}
						else if (ANIM_SPEAR_ATTACK3 == iCurAnimIndex)
						{
							if (0.0800 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[2])
								{
									m_arrIsSpearAttackEffectCreated[2] = true;
									m_pSpear->Create_AttackEffect();
								}
							}


							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_SPEAR_ATTACK4)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Spear_L4.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_SPEAR_ATTACK4;
									m_pSpear->Set_Animation(4);
									// SpearAttack
									if (0.1400 <= m_pModel->Get_PlayTimeRatio())
										m_pSpear->Create_AttackEffect();

								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);

							}
						}
						else if (ANIM_SPEAR_ATTACK4 == iCurAnimIndex)
						{
							if (0.1200 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[3])
								{
									m_arrIsSpearAttackEffectCreated[3] = true;
									m_pSpear->Create_AttackEffect();
								}
							}

							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_SPEAR_ATTACK5)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Spear_L5.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_SPEAR_ATTACK5;
									m_pSpear->Set_Animation(5);
									// SpearAttack
									if (0.1500 <= m_pModel->Get_PlayTimeRatio())
										m_pSpear->Create_AttackEffect();

								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);

							}
						}
						else if (ANIM_SPEAR_ATTACK5 == iCurAnimIndex)
						{
							if (0.1300 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[4])
								{
									m_arrIsSpearAttackEffectCreated[4] = true;
									m_pSpear->Create_AttackEffect();
								}
							}

							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown)
								{
									if (m_iAnimation != ANIM_SPEAR_ATTACK6)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Spear_L6.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_SPEAR_ATTACK6;
									m_pSpear->Set_Animation(92);


								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);

							}
						}
						else if (ANIM_SPEAR_ATTACK6 == iCurAnimIndex)
						{
							if (0.1100 <= m_pModel->Get_PlayTimeRatio())
							{
								if (!m_arrIsSpearAttackEffectCreated[5])
								{
									m_arrIsSpearAttackEffectCreated[5] = true;
									m_pSpear->Create_AttackEffect();
								}
							}

							if (LinkMaxRatio <= TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_SpearIdle();
								ZeroMemory(&m_arrIsSpearAttackEffectCreated, sizeof(_bool) * SPEAR_ATTACK_NUM);

							}
						}
					}
				}
			}
			else if (EQUIPSTATE::GAUNTLET == m_eEquipState)
			{
				if (!m_bLButtonDown)
				{
					m_iAnimation = ANIM_GAUNTLET_ATTACK1;
					m_pGauntlet->Set_Animation(9);

					pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
					pSoundMgr->PlaySound(TEXT("Gauntlet_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
					m_bLButtonDown = true;
					m_bGauntletSpin = false;
				}

				bPlayAnim = true;
				m_bLButtonDown = true;
				m_pGauntlet->Set_WeaponState(WEAPONSTATE::ATTACK);

				if (m_iAnimation >= ANIM_GAUNTLET_ATTACK1 && m_iAnimation <= ANIM_GAUNTLET_ATTACK8)
				{
					_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
					_double TimeRatio = m_pModel->Get_PlayTimeRatio();

					_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
					_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

					if (m_iAnimation == iCurAnimIndex)
					{
						if (ANIM_GAUNTLET_ATTACK1 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK2)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK2;
									m_pGauntlet->Set_Animation(37);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK2 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK3)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK3;
									m_pGauntlet->Set_Animation(43);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK3 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK4)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L4.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK4;
									m_pGauntlet->Set_Animation(10);
								}
							}
							else if (0.325 < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK4 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK5)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L5.mp3"), CSoundMgr::P_WEAPONSOUND);
										m_bGauntletSpin = true;
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK5;
									m_pGauntlet->Set_Animation(22);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK5 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK6)
									{
 										m_bGauntletSpin = false;
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L6.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK6;
									m_pGauntlet->Set_Animation(24);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK6 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK7)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L7.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK7;
									m_pGauntlet->Set_Animation(34);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK7 == iCurAnimIndex)
						{
							if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
							{
								if (bDown && !m_pModel->Get_Blend())
								{
									if (m_iAnimation != ANIM_GAUNTLET_ATTACK8)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("Gauntlet_L8.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_GAUNTLET_ATTACK8;
									m_pGauntlet->Set_Animation(26);
								}
							}
							else if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
						else if (ANIM_GAUNTLET_ATTACK8 == iCurAnimIndex)
						{
							if (LinkMaxRatio < TimeRatio)
							{
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_bLButtonDown = false;

								Set_GauntletIdle();
							}
						}
					}
				}
			}
		}

		if (m_bTargeting)
			LerpToTargetLook(TimeDelta);
	}
	else if (((bRightDown = pGameInstance->Mouse_Pressing(CInputDevice::DIMB::DIMB_RB)) || m_bRButtonDown))
	{
		if (!m_bJump)
		{
			bPlayAnim = true;
			m_bContinue = false;
			m_bRButtonDown = true;

			if (ANIMSTATE::ATTACK == m_eAnimState)
				m_bLButtonDown = false;

			m_eAnimState = ANIMSTATE::POWERATTACK;

			if (EQUIPSTATE::KATANA == m_eEquipState)
			{
				m_pKatana->Set_WeaponState(WEAPONSTATE::ATTACK);

				_double TimeRatio = m_pModel->Get_PlayTimeRatio();

				if (ANIM_KATANA_POWERATTACK2 != m_pModel->Get_CurrentAnimation())
				{
					if (m_iAnimation != ANIM_KATANA_POWERATTACK1)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Katana_R1.mp3"), CSoundMgr::P_WEAPONSOUND);
					}

					m_iAnimation = ANIM_KATANA_POWERATTACK1;
					m_pKatana->Set_Animation(KATANA_POWERATTACK1);
					m_pKatana->Set_Independence(false);
					m_pKatana->Set_DissolveIdle();

					if (ANIM_KATANA_POWERATTACK1 == m_pModel->Get_CurrentAnimation())
					{
						if (m_pModel->Get_LinkMinRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() > TimeRatio)
						{
							if (!bRightDown)
							{
								m_iAnimation = ANIM_IDLE;
								m_bContinue = false;
								m_eAnimState = ANIMSTATE::IDLE;
								bPlayAnim = false;
								m_bRButtonDown = false;

								//Set_KatanaIdle();
								m_pKatana->Set_Dissolve();

							}
						}
						else if (m_pModel->Get_LinkMaxRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() + SECPERFRAME >= TimeRatio)
						{
							if (bRightDown)
							{
								if (m_iAnimation != ANIM_KATANA_POWERATTACK2)
								{
									m_bKatanaR = true;
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("Katana_R2.mp3"), CSoundMgr::P_WEAPONSOUND);
								}

								m_iAnimation = ANIM_KATANA_POWERATTACK2;
								m_pKatana->Set_Animation(KATANA_POWERATTACK2);
							}
						}

					}
				}
				else if (ANIM_KATANA_POWERATTACK2 == m_pModel->Get_CurrentAnimation())
				{
					if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
					{
						m_eAnimState = ANIMSTATE::IDLE;
						bPlayAnim = false;
						m_bContinue = true;
						m_iAnimation = ANIM_IDLE;
						m_bRButtonDown = false;

						//Set_KatanaIdle();
						m_pKatana->Set_Dissolve();
						m_bKatanaR = false;

					}
				}
			}
			else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
			{
				_double TimeRatio = m_pModel->Get_PlayTimeRatio();

				if (false == m_bCharge)
				{
					m_pLongKatana->Set_WeaponState(WEAPONSTATE::LONGPOWERATTACK);

					if (ANIM_LONGKATANA_POWERATTACK2 != m_iAnimation && ANIM_LONGKATANA_POWERATTACK3 != m_iAnimation)
					{
						if (ANIM_LONGKATANA_POWERATTACK1 != m_iAnimation)
						{
							//
							pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
							pSoundMgr->PlaySound(TEXT("LKatana_R1.mp3"), CSoundMgr::P_WEAPONSOUND);
						}

						m_iAnimation = ANIM_LONGKATANA_POWERATTACK1;

						if (19 != m_pLongKatana->Get_AnimationIndex())
						{
							m_pLongKatana->Set_Animation(19);

							m_pLongKatana->Set_PlayerNonLerp(false);
							m_pLongKatana->Set_Independence(false);
						}

						else if (19 == m_pLongKatana->Get_ModelCurrentAnim())
						{
							if (0.04 >= m_pLongKatana->Get_PlayTimeRatio())
							{
								//Set_LongKatanaIdle();
								if (DISSOLVE_STATE::DISSOLVE_IDLE != m_pLongKatana->Get_DissolveState())
								{
									Set_LongKatanaIdle();
									m_pLongKatana->Set_DissolveIdle();
								}
								m_pLongKatana->Start_Trail();
							}
						}

						if (ANIM_LONGKATANA_POWERATTACK1 == m_pModel->Get_CurrentAnimation())
						{
							if (m_pModel->Get_LinkMinRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() >= TimeRatio)
							{
								if (bRightDown)
								{
									if (m_iAnimation != ANIM_LONGKATANA_POWERATTACK2)
									{
										pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
										pSoundMgr->PlaySound(TEXT("LKatana_R2.mp3"), CSoundMgr::P_WEAPONSOUND);
									}

									m_iAnimation = ANIM_LONGKATANA_POWERATTACK2;
									m_pLongKatana->Set_Animation(5);
								}
							}
							else if (m_pModel->Get_LinkMaxRatio() + ZEROPOINTONE < TimeRatio)
							{
								m_eAnimState = ANIMSTATE::IDLE;
								bPlayAnim = false;
								m_bContinue = true;
								m_iAnimation = ANIM_IDLE;
								m_bRButtonDown = false;
								m_pLongKatana->End_Trail();

								//Set_LongKatanaIdle();
								m_pLongKatana->Set_Dissolve();
							}
						}
					}
					else
					{
						if (ANIM_LONGKATANA_POWERATTACK2 == m_iAnimation)
						{
							if (ANIM_LONGKATANA_POWERATTACK2 == m_pModel->Get_CurrentAnimation())
							{
								if (m_pModel->Get_LinkMinRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() >= TimeRatio)
								{
									if (bRightDown)
									{
										m_pLongKatana->Set_Animation(6);

										if (m_iAnimation != ANIM_LONGKATANA_POWERATTACK3)
										{
											pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
											pSoundMgr->PlaySound(TEXT("LKatana_R3.mp3"), CSoundMgr::P_WEAPONSOUND);
										}

										m_iAnimation = ANIM_LONGKATANA_POWERATTACK3;
									}
								}

								if (m_pModel->Get_LinkMaxRatio() + ZEROPOINTONE < TimeRatio)
								{
									m_bRButtonDown = false;
									m_iAnimation = ANIM_IDLE;
									bPlayAnim = false;
									m_eAnimState = ANIMSTATE::IDLE;
									m_pLongKatana->End_Trail();

									//Set_LongKatanaIdle();
									m_pLongKatana->Set_Dissolve();
								}
							}
						}
						else if (ANIM_LONGKATANA_POWERATTACK3 == m_pModel->Get_CurrentAnimation())
						{
							if (ANIM_LONGKATANA_POWERATTACK3 == m_pModel->Get_CurrentAnimation())
							{
								if (m_pModel->Get_LinkMaxRatio() /*+ ZEROPOINTONE*/ < TimeRatio)
								{
									m_bRButtonDown = false;
									m_iAnimation = ANIM_IDLE;
									bPlayAnim = false;
									m_eAnimState = ANIMSTATE::IDLE;
									m_pLongKatana->End_Trail();

									//Set_LongKatanaIdle();
									m_pLongKatana->Set_Dissolve();
								}
							}
						}
					}
				}
				else
				{
					m_pLongKatana->Set_WeaponState(WEAPONSTATE::LONGPOWERATTACK);
					if (ANIM_LONGKATANA_CHARGEDW != m_iAnimation && ANIM_LONGKATANA_CHARGEATTACK != m_iAnimation)
					{
						if (ANIM_LONGKATANA_STARTCHARGE != m_iAnimation)
						{
							Create_ChargingEffect();
							if (DISSOLVE_STATE::DISSOLVE_IDLE != m_pLongKatana->Get_DissolveState())
							{
								Set_LongKatanaIdle();
								m_pLongKatana->Set_DissolveIdle();
							}
							m_pLongKatana->Start_Trail();
							m_bPressingR = true;

							pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
							pSoundMgr->PlaySound(TEXT("LKatana_ChargeStart.mp3"), CSoundMgr::P_WEAPONSOUND);
						}

						m_iAnimation = ANIM_LONGKATANA_STARTCHARGE;
						m_pLongKatana->Set_Animation(31);

						if (m_iAnimation == m_pModel->Get_CurrentAnimation())
						{
							if (m_pModel->Get_LinkMinRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() >= TimeRatio)
							{
								if (bRightDown)
								{
									m_iAnimation = ANIM_LONGKATANA_CHARGEDW;
									m_pLongKatana->Set_Animation(35);

									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("LKatana_Charging.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								else
								{
									m_bRButtonDown = false;
									m_iAnimation = ANIM_IDLE;
									bPlayAnim = false;
									m_eAnimState = ANIMSTATE::IDLE;

									//Set_LongKatanaIdle();
									m_pLongKatana->Set_Dissolve();
								}
							}
						}
					}
					else if (ANIM_LONGKATANA_CHARGEDW == m_iAnimation)
					{
						m_iAnimation = ANIM_LONGKATANA_CHARGEDW;
						m_pLongKatana->Set_Animation(35);
						pSoundMgr->PlaySound(TEXT("LKatana_Charging.mp3"), CSoundMgr::P_WEAPONSOUND);

						if (m_pModel->Get_LinkMinRatio() <= TimeRatio && m_pModel->Get_LinkMaxRatio() >= TimeRatio)
						{
							if (!bRightDown)
							{
								if (m_iAnimation != ANIM_LONGKATANA_CHARGEATTACK)
								{
									m_bPressingR = false;
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("LKatana_ChargeEnd.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								m_iAnimation = ANIM_LONGKATANA_CHARGEATTACK;
								m_pLongKatana->Set_Animation(42);
								Delete_ChargingEffect();
							}
						}
						else if (m_pModel->Get_LinkMaxRatio() < TimeRatio)
						{
							if (m_iAnimation != ANIM_LONGKATANA_CHARGEATTACK)
								{
									m_bPressingR = false;
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("LKatana_ChargeEnd.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
							m_iAnimation = ANIM_LONGKATANA_CHARGEATTACK;
							m_pLongKatana->Set_Animation(42);
							Delete_ChargingEffect();
						}
					}
					else if (ANIM_LONGKATANA_CHARGEATTACK == m_iAnimation)
					{
						m_iAnimation = ANIM_LONGKATANA_CHARGEATTACK;
						m_pLongKatana->Set_Animation(42);
						Delete_ChargingEffect();

						if (ANIM_LONGKATANA_CHARGEATTACK == m_pModel->Get_CurrentAnimation())
						{
							if (m_pModel->Get_LinkMaxRatio() < TimeRatio)
							{
								m_bPressingR = false;
								m_bRButtonDown = false;
								m_iAnimation = ANIM_IDLE;
								bPlayAnim = false;
								m_eAnimState = ANIMSTATE::IDLE;
								m_pLongKatana->End_Trail();

								//Set_LongKatanaIdle();
								m_pLongKatana->Set_Dissolve();
							}
						}
					}
				}
			}
			else if (m_eEquipState == EQUIPSTATE::SPEAR)
			{
				_double TimeRatio = m_pModel->Get_PlayTimeRatio();
				_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();
				_double LinkMinRatio = m_pModel->Get_LinkMinRatio();

				if (bRightDown)
				{
					if (ANIM_IDLE == m_iAnimation)
					{
						Set_SpearIdle();
					}
				}

				//
				if (m_iAnimation != ANIM_SPEAR_POWERATTACK)
				{
					pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
					pSoundMgr->PlaySound(TEXT("Spear_Throw.mp3"), CSoundMgr::P_WEAPONSOUND);
				}

				m_iAnimation = ANIM_SPEAR_POWERATTACK;
				m_pSpear->Set_Animation(95);
				m_pSpear->Set_WeaponState(WEAPONSTATE::SPEARPOWERATTACK);

				if (ANIM_SPEAR_POWERATTACK == m_pModel->Get_CurrentAnimation())
				{
					if (LinkMinRatio >= TimeRatio)
					{
						m_bThrowSpear = true;
					}
					
					if (LinkMaxRatio + 0.01 <= TimeRatio)
					{
						m_bRButtonDown = false;
						m_iAnimation = ANIM_IDLE;
						bPlayAnim = false;
						m_eAnimState = ANIMSTATE::IDLE;

						m_pSpear->Set_Independence(true);
						m_pSpear->Set_PlayerNonLerp(false);
						m_bThrowSpear = false;
						//Set_SpearIdle();
					}
				}
			}
			else if (m_eEquipState == EQUIPSTATE::GAUNTLET)
			{
				_double TimeRatio = m_pModel->Get_PlayTimeRatio();
				_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
				_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

				if (ANIM_GAUNTLET_POWERATTACK2 != m_iAnimation && ANIM_GAUNTLET_POWERATTACK3 != m_iAnimation)
				{
					if (m_iAnimation != ANIM_GAUNTLET_POWERATTACK1)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Gauntlet_R1.mp3"), CSoundMgr::P_WEAPONSOUND);
						m_bGauntletSpin = false;
					}

					m_iAnimation = ANIM_GAUNTLET_POWERATTACK1;
					m_pGauntlet->Set_Animation(36);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETPOWERATTACK);

					if (ANIM_GAUNTLET_POWERATTACK1 == m_pModel->Get_CurrentAnimation())
					{
						if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
						{
							if (bRightDown)
							{
								if (m_iAnimation != ANIM_GAUNTLET_POWERATTACK2)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("Gauntlet_R2.mp3"), CSoundMgr::P_WEAPONSOUND);
								}

								m_iAnimation = ANIM_GAUNTLET_POWERATTACK2;
								m_pGauntlet->Set_Animation(33);
								m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETPOWERATTACK);
							}
						}
						else if (LinkMaxRatio < TimeRatio)
						{
							m_bRButtonDown = false;
							m_iAnimation = ANIM_IDLE;
							bPlayAnim = false;
							m_eAnimState = ANIMSTATE::IDLE;

							Set_GauntletIdle();
						}
					}
				}
				else if (ANIM_GAUNTLET_POWERATTACK2 == m_iAnimation)
				{
					m_iAnimation = ANIM_GAUNTLET_POWERATTACK2;
					m_pGauntlet->Set_Animation(33);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETPOWERATTACK);

					if (ANIM_GAUNTLET_POWERATTACK2 == m_pModel->Get_CurrentAnimation())
					{
						if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
						{
							if (bRightDown)
							{
								if (m_iAnimation != ANIM_GAUNTLET_POWERATTACK3)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("Gauntlet_R3.mp3"), CSoundMgr::P_WEAPONSOUND);
								}

								m_iAnimation = ANIM_GAUNTLET_POWERATTACK3;
								m_pGauntlet->Set_Animation(13);
								m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETPOWERATTACK);
							}
						}
						else if (LinkMaxRatio < TimeRatio)
						{
							m_bRButtonDown = false;
							m_iAnimation = ANIM_IDLE;
							bPlayAnim = false;
							m_eAnimState = ANIMSTATE::IDLE;

							Set_GauntletIdle();
						}
					}
				}
				else if (ANIM_GAUNTLET_POWERATTACK3 == m_iAnimation)
				{
					m_iAnimation = ANIM_GAUNTLET_POWERATTACK3;
					m_pGauntlet->Set_Animation(13);
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETPOWERATTACK);

					if (ANIM_GAUNTLET_POWERATTACK3 == m_pModel->Get_CurrentAnimation())
					{
						if (LinkMaxRatio < TimeRatio)
						{
							m_bRButtonDown = false;
							m_iAnimation = ANIM_IDLE;
							bPlayAnim = false;
							m_eAnimState = ANIMSTATE::IDLE;

							Set_GauntletIdle();
						}
					}
				}
			}
		}

		if (m_bTargeting)
			LerpToTargetLook(TimeDelta);
	}

	if (((bJumpKeyDown = pGameInstance->Key_Down(DIK_SPACE)) || m_bJump))
	{
		if (pGameInstance->Key_Pressing(DIK_SPACE))
		{
			m_SpacePressingTime += TimeDelta;

			if ((m_pTransform->Get_JumpTime() >= m_pTransform->Get_TopJumpTime()) || m_bHangOn)
			{
				if (m_pTransform->Get_TopJumpTime() <= m_SpacePressingTime)
				{
					if (ANIM_JUMPLANDING != m_iAnimation)
					{
						if (ANIMSTATE::JUMP == m_eAnimState)
						{
							if (true == Is_ProgramChipInstalled(CPlayer::PROGRAM_HANG))
							{
								m_iAnimation = 26;
								m_pPod->Set_PodState(PODSTATE::HANG);
								m_pTransform->Set_FallTime();

								m_bHangOn = true;
							}
							else
							{
								if (pGameInstance->Key_Pressing(DIK_SPACE))
								{
									if (false == m_bIsSysMsgSendOut)
									{
										CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(ITEMCODE_ENFORCECHIP_SKILL_HANG, 0, CUI_Dialogue_SystemMessage::iMsgNeedChip);
										pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
										m_bIsSysMsgSendOut = true;
									}
								}
								m_bIsSysMsgSendOut = false;
								m_bHangOn = false;
								m_SpacePressingTime = 0.0;
							}
						}
					}
				}
			}
		}
		else
		{
			m_bHangOn = false;
			m_SpacePressingTime = 0.0;
		}
		
		if (m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::SLIDING)
		{
			if (m_eAnimState != ANIMSTATE::JUMPATTACK && m_eAnimState != ANIMSTATE::JUMPPOWERATTACK)
				m_eAnimState = ANIMSTATE::JUMP;

			bPlayAnim = true;
			m_bContinue = false;
			Check_JumpState(TimeDelta, bJumpKeyDown, bPlayAnim);

			if (bDown || m_eAnimState == ANIMSTATE::JUMPATTACK)
			{
				if (EQUIPSTATE::KATANA == m_eEquipState || EQUIPSTATE::SPEAR == m_eEquipState
					|| EQUIPSTATE::GAUNTLET == m_eEquipState)
					Check_JumpAttack(bDown);
			}
			else if (bRightDown || m_eAnimState == ANIMSTATE::JUMPPOWERATTACK)
			{
				if (EQUIPSTATE::KATANA == m_eEquipState || EQUIPSTATE::GAUNTLET == m_eEquipState)
					Check_JumpPowerAttack(bRightDown);
				else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
					Check_LongKatana_JumpAttack(bRightDown);
			}

			if ((ANIM_JUMP == m_pModel->Get_CurrentAnimation() || ANIM_HANG == m_pModel->Get_CurrentAnimation() || ANIM_JUMP_DW == m_pModel->Get_CurrentAnimation() || ANIM_DOUBLEJUMP == m_pModel->Get_CurrentAnimation() || true == m_pTransform->Get_PowerAtt() || m_pModel->Get_NoneBlend()))
			{
				m_pTransform->Set_Jump(true);
				m_pTransform->Jump(TimeDelta, m_pNavigation);
			}

		}
	}

	Check_MoveRun(bPlayAnim, TimeDelta);

	if (false == bPlayAnim)
		IdleCheck_State();

	Check_RestTime(TimeDelta);

	if (pGameInstance->Key_Down(DIK_NUMPAD1) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::FIXED);
		m_bTargeting = false;

		if (nullptr != m_pPod)
			m_pPod->Set_Targeting(m_bTargeting);
	}

	/*if (pGameInstance->Key_Down(DIK_NUMPAD2) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_RSIDE);
		m_bTargeting = false;
	}*/

	if (pGameInstance->Key_Down(DIK_NUMPAD3) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_RSIDE_TO_3RD);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD0) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::TOP);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD4) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_TOP);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD5) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_TOP_TO_3RD);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD6) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_WHEEL);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD7) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_WHEEL_TO_3RD);
		m_bTargeting = false;
	}

	if (pGameInstance->Key_Down(DIK_NUMPAD9) && pCamera != nullptr)
	{
		pCamera->Set_Option(CMainCamera::OPTION::COASTER);
		m_bTargeting = false;
	}


	if (pGameInstance->Key_Pressing(DIK_TAB) && pCamera != nullptr && pCamera->Get_Option() != CMainCamera::OPTION::COASTER)
	{
		CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE_STATE, &eState));

		if (pCamera->Get_Option() != CMainCamera::OPTION::UI)
		{
			pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_UI);
		}

		m_bPressingTab = true;
	}
	if (pGameInstance->Key_Up(DIK_TAB) && pCamera != nullptr && pCamera->Get_Option() != CMainCamera::OPTION::COASTER)
	{
		CUI::UISTATE	eState = CUI::UISTATE::INACTIVATE;

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE_STATE, &eState));

		pCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_UI_TO_3RD);

		m_bPressingTab = false;
	}

	if (false == m_dIsSwitchingReady)
	{
		m_dSwitchingCoolTimeAcc += TimeDelta;
		if (m_dSwitchingCoolTimeAccMax < m_dSwitchingCoolTimeAcc)
		{
			m_dSwitchingCoolTimeAcc = 0.0;
			m_dIsSwitchingReady = true;
		}
	}

	if (pGameInstance->Key_Down(DIK_1))
	{
		if (m_dIsSwitchingReady == true)
		{
			Switch_EquipState();
			m_dIsSwitchingReady = false;
		}
	}
}

void CPlayer::Key_Check_UI(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(pGameInstance->Get_hWnd(), &ptMouse);

	// POINT Culling
	_bool	bCullResult = false;

	if (ptMouse.y < 90.f)	// DecoLine RACE_TOP posY
		bCullResult = true;

	// UI Search
	//if (true == bCullResult)
	{
		Engine::CLayer::OBJECTS*				pObjects = pGameInstance->Get_ObjectList((_uint)LEVEL::STATIC, TEXT("Layer_UI"));
		
		CUI_Core_Controller::INPUTLIMITLEVEL	eInputLimitLevel = dynamic_cast<CUI_Core_Controller*>(*(pObjects->rbegin()))->Get_CurrentInputLimitLevel();

		std::priority_queue<UIDISTANCEINFO>		pqPickedUI;

		_bool	bIsKeyDown = pGameInstance->Mouse_Down(CInputDevice::DIMB::DIMB_LB);

		// Z Sorting
		for (auto pObj : *pObjects)
		{
			CUI*			pUI = dynamic_cast<CUI*>(pObj);
			const RECT&		objPos = pUI->Get_ScreenPosRect();
			const _bool&	bCollision = pUI->Get_Collision();

			if (pUI->Get_DistanceZ() < (_uint)eInputLimitLevel)
			{
				continue;
			}			
			if (true == bCollision)
			{
				if (TRUE == PtInRect(&objPos, ptMouse))
				{
					UIDISTANCEINFO	tInfo;
					tInfo.pUI = pUI;
					tInfo.iDistance = pUI->Get_DistanceZ();

					pqPickedUI.emplace(tInfo);
				}
				else
				{
					if (true == bIsKeyDown)
					{
						if (pUI->Get_UISTATE() == CUI::UISTATE::PRESSED)
						{
							CUI::UISTATE eState = CUI::UISTATE::RELEASED;

							if (true == bCullResult)
							{
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eState));
							}
							else
							{
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eState));
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &eState));
							}
						}
					}
					else
					{
						if (pUI->Get_UISTATE() != CUI::UISTATE::PRESSED)
						{
							CUI::UISTATE eState = CUI::UISTATE::INACTIVATE;

							if (true == bCullResult)
							{
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eState));
							}
							else
							{
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eState));
								pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &eState));
							}
						}
					}
				}
			}
		}

		// Result PtInRect
		if (false == pqPickedUI.empty())
		{
			if (true == bIsKeyDown)
			{
				CUI::UISTATE	eState = CUI::UISTATE::PRESSED;

				if (true == bCullResult)
				{
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eState));
				}
				else
				{
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eState));
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &eState));
				}
			}
			else
			{
				CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

				if (true == bCullResult)
				{
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_HEADBUTTON, &eState));
				}
				else
				{
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_LISTLINE_STATE, &eState));
					pqPickedUI.top().pUI->Notify((void*)&PACKET(CHECKSUM_UI_CORE_SELECTLINE_STATE, &eState));
				}
			}
		}
	}

	// Wheel Check
	_long	iMouseWheelMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_W);
	
	CUI_Core_Controller::COMMANDDESC	tDesc;

	if (0 < iMouseWheelMove)
	{
		//CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::BGM);
		//CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::HACKING);

		tDesc.iCommandFlag |= COMMAND_CORE_SCROLL_UP;
		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));

	}
	else if (iMouseWheelMove < 0)
	{
		//CSoundMgr::Get_Instance()->FadeOut_Volume(CSoundMgr::BGM);
		//CSoundMgr::Get_Instance()->FadeIn_Volume(CSoundMgr::HACKING);

		tDesc.iCommandFlag |= COMMAND_CORE_SCROLL_DOWN;
		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CONTROLLER, &tDesc));
	}

	// Cursor Activation
	if (true == pGameInstance->Mouse_Pressing(CInputDevice::DIMB::DIMB_LB))
	{
		CUI::UISTATE	eState = CUI::UISTATE::ACTIVATE;

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CURSOR, (void*)&eState));
	}
	if (true == pGameInstance->Mouse_Up(CInputDevice::DIMB::DIMB_LB))
	{
		CUI::UISTATE	eState = CUI::UISTATE::ENABLE;

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_CURSOR, (void*)&eState));
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Check_Target()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (m_eAnimState == ANIMSTATE::HIT)
	{
		m_bTargeting = false;
		if (m_pTarget != nullptr)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
			m_iFlagUI ^= FLAG_UI_CALL_TARGET;

			if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
				&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
			{
				pCamera->Set_Option(CMainCamera::OPTION::CANCLELOCK);
			}

			if (nullptr != m_pPod)
				m_pPod->Set_Targeting(m_bTargeting);
		}

	}

	if (m_bTargeting == false)
	{
		if (m_pTarget != nullptr)
		{
			Safe_Release(m_pTarget);
			m_pTarget = nullptr;
			m_iFlagUI ^= FLAG_UI_CALL_TARGET;

			if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
				&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
			{
				pCamera->Set_Option(CMainCamera::OPTION::CANCLELOCK);
			}

			if (nullptr != m_pPod)
				m_pPod->Set_Targeting(m_bTargeting);
		}
	}

	if (m_bTargeting && m_pTarget)
	{
		CTransform* pMonsterTransformCom = (CTransform*)m_pTarget->Get_Component(TEXT("Com_Transform"));

		_vector vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE::POSITION);

		_vector vPlayerPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		_float fDistance = 0.f;

		_vector vDistance = vMonsterPos - vPlayerPos;
		fDistance = XMVectorGetX(XMVector3Length(vDistance));

		if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() == (_uint)LEVEL::ROBOTGENERAL)
		{
			if (fDistance >= 200.f)
			{
				m_bTargeting = false;
				Safe_Release(m_pTarget);
				m_pTarget = nullptr;
				m_iFlagUI ^= FLAG_UI_CALL_TARGET;

				if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
					&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
				{
					pCamera->Set_Option(CMainCamera::OPTION::CANCLELOCK);
				}
				if (nullptr != m_pPod)
					m_pPod->Set_Targeting(m_bTargeting);
			}
		}

		else
		{
			if (fDistance >= 50.f)
			{
				m_bTargeting = false;
				Safe_Release(m_pTarget);
				m_pTarget = nullptr;
				m_iFlagUI ^= FLAG_UI_CALL_TARGET;

				if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
					&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
				{
					pCamera->Set_Option(CMainCamera::OPTION::CANCLELOCK);
				}
				if (nullptr != m_pPod)
					m_pPod->Set_Targeting(m_bTargeting);
			}
		}

	}

	if (m_bTargeting == true && m_pTarget == nullptr) // m_bTargeting == true
	{
		_vector vPlayerPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_bool bCheck = true;

		list<CGameObject*>* MonsterList = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelID(), L"Monster");

		list<LENGTHINFO> TargetList;

		if (MonsterList == nullptr)
		{
			bCheck = false;
		}

		if (MonsterList != nullptr)
		{
			if (0 >= MonsterList->size())
			{
				//m_bTargeting = false;
				//m_pTarget = nullptr;
				//m_iFlagUI ^= FLAG_UI_CALL_TARGET;
				bCheck = false;
			}
		}

		if (bCheck)
		{
			for (auto& Monster : *MonsterList)
			{
				if (Monster->Get_State() == OBJSTATE::DISABLE)
				{
					continue;
				}

				CTransform* pMonsterTransformCom = (CTransform*)Monster->Get_Component(TEXT("Com_Transform"));
				_vector vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE::POSITION);
				_float fDistance = 0.f;

				_vector vDistance = vMonsterPos - vPlayerPos;
				fDistance = XMVectorGetX(XMVector3Length(vDistance));

				if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() == (_uint)LEVEL::ROBOTGENERAL)
				{
					if (fDistance >= 200.f)
						continue;

					else
					{
						LENGTHINFO tInfo;
						tInfo.pGameObject = Monster;
						tInfo.fLength = fDistance;
						TargetList.emplace_back(tInfo);
					}
				}
				else
				{
					if (fDistance >= 50.f)
						continue;
					else
					{
						LENGTHINFO tInfo;
						tInfo.pGameObject = Monster;
						tInfo.fLength = fDistance;
						TargetList.emplace_back(tInfo);
					}
				}
			}
		}

		bCheck = true;

		list<CGameObject*>* BossList = pGameInstance->Get_ObjectList(pGameInstance->Get_LevelID(), L"Boss");

		if (BossList == nullptr)
		{
			bCheck = false;
		}

		if (BossList != nullptr && 0 >= BossList->size())
		{
			bCheck = false;
		}

		if (bCheck)
		{
			for (auto& Boss : *BossList)
			{
				CTransform* pMonsterTransformCom = (CTransform*)Boss->Get_Component(TEXT("Com_Transform"));
				_vector vMonsterPos = pMonsterTransformCom->Get_State(CTransform::STATE::POSITION);
				_float fDistance = 0.f;

				_vector vDistance = vMonsterPos - vPlayerPos;
				fDistance = XMVectorGetX(XMVector3Length(vDistance));

				if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() == (_uint)LEVEL::ROBOTGENERAL)
				{
					if (fDistance >= 200.f)
						continue;

					else
					{
						LENGTHINFO tInfo;
						tInfo.pGameObject = Boss;
						tInfo.fLength = fDistance;
						TargetList.emplace_back(tInfo);
					}
				}

				else
				{
					if (fDistance >= 50.f)
						continue;
					else
					{
						LENGTHINFO tInfo;
						tInfo.pGameObject = Boss;
						tInfo.fLength = fDistance;
						TargetList.emplace_back(tInfo);
					}
				}
			}
		}

		if (0 >= TargetList.size())
		{
			RELEASE_INSTANCE(CGameInstance);
			m_bTargeting = false;
			m_pTarget = nullptr;
			m_iFlagUI ^= FLAG_UI_CALL_TARGET;

			if (nullptr != m_pPod)
				m_pPod->Set_Targeting(m_bTargeting);

			return;
		}

		TargetList.sort(CompareLength<LENGTHINFO>);

		m_pTarget = TargetList.front().pGameObject;
		Safe_AddRef(m_pTarget);
		m_iFlagUI |= FLAG_UI_CALL_TARGET;

		//MonsterList->clear();

		TargetList.clear();

		if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
			&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
		{
			pCamera->Set_Option(CMainCamera::OPTION::SWITHLOCK);
		}
	}

	if (m_bTargeting == true && m_pTarget != nullptr && m_pTarget->Get_State() == OBJSTATE::DEAD)
	{
		m_bTargeting = false;
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
		m_iFlagUI ^= FLAG_UI_CALL_TARGET;
		
		if (pCamera->Get_Option() != CMainCamera::OPTION::COASTER && pCamera->Get_Option() != CMainCamera::OPTION::TOP
			&& pCamera->Get_Option() != CMainCamera::OPTION::TOPB)
		{
			pCamera->Set_Option(CMainCamera::OPTION::CANCLELOCK);
		}

		if (nullptr != m_pPod)
			m_pPod->Set_Targeting(m_bTargeting);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Initialize_EquipState()
{
	m_pKatana->Set_WeaponEquip(WEAPONEQUIP::NONEQUIP);
	m_pLongKatana->Set_WeaponEquip(WEAPONEQUIP::NONEQUIP);
	m_pSpear->Set_WeaponEquip(WEAPONEQUIP::NONEQUIP);
	m_pGauntlet->Set_WeaponEquip(WEAPONEQUIP::NONEQUIP);
}

void CPlayer::IdleCheck_State()
{
	if (m_eAnimState == ANIMSTATE::ATTACK)
	{
		m_eAnimState = ANIMSTATE::IDLE;
		m_bLButtonDown = false;
	}

	else if (m_eAnimState == ANIMSTATE::IDLE)
	{
		m_iAnimation = ANIM_IDLE;
		m_bContinue = true;
		m_LookLerpTime = 0.0;
		m_eDirection = DIRECTION::FORWARD;
	}

	else if (m_eAnimState == ANIMSTATE::WALK)
	{
		m_iAnimation = ANIM_IDLE;
		m_bContinue = true;
		m_eAnimState = ANIMSTATE::WALK;
	}

	else if (m_eAnimState == ANIMSTATE::RUN)
	{
		if (ANIM_RUN == m_iAnimation)
		{
			m_bContinue = false;
			m_eAnimState = ANIMSTATE::SLIDING;
			m_eDirection = DIRECTION::FORWARD;

			if (0.4 > m_pModel->Get_PlayTimeRatio())
				m_iAnimation = ANIM_RUNSLIDING_RIGHT;
			else
				m_iAnimation = ANIM_RUNSLIDING_LEFT;
		}
		else if (ANIM_JUMPLANDING_RUN == m_iAnimation /*|| ANIM_SPURTTORUN == m_pModel->Get_CurrentAnimation()*/)
		{
			m_iAnimation = ANIM_SPURTSLIDING_RIGHT;
			m_bContinue = false;
			m_eAnimState = ANIMSTATE::SLIDING;
		}
	}

	else if (m_eAnimState == ANIMSTATE::SPURT)
	{
		if (ANIM_SPURT == m_pModel->Get_CurrentAnimation() || ANIM_SPURT == m_iAnimation)
		{
			m_iAnimation = ANIM_SPURTSLIDING_RIGHT;
			m_bContinue = false;
			m_eAnimState = ANIMSTATE::SLIDING;
		}
		else if (ANIM_FRONT_AVOID == m_iAnimation)
		{
			m_bContinue = false;
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_iAnimation = ANIM_SPURTSLIDING_RIGHT;
				m_eAnimState = ANIMSTATE::SLIDING;
			}
		}
	}

	else if (m_eAnimState == ANIMSTATE::SLIDING)
	{
		if (ANIM_RUNSLIDING_RIGHT == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_iAnimation = ANIM_IDLE;
				m_eDirection = DIRECTION::FORWARD;
			}
		}
		else if (ANIM_RUNSLIDING_LEFT == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_iAnimation = ANIM_IDLE;
				m_eDirection = DIRECTION::FORWARD;
			}
		}
		else if (ANIM_SPURTSLIDING_RIGHT == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_iAnimation = ANIM_IDLE;
				m_eDirection = DIRECTION::FORWARD;
			}
		}
		else if (ANIM_FRONT_AVOID_SLIDING == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_iAnimation = ANIM_IDLE;
		}
		else if (ANIM_BACK_AVOID_SLIDING == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_iAnimation = ANIM_IDLE;
		}
		else if (ANIM_RIGHT_AVOID_SLIDING == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_iAnimation = ANIM_IDLE;
		}
		else if (ANIM_LEFT_AVOID_SLIDING == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				m_iAnimation = ANIM_IDLE;
		}

		if (ANIM_IDLE == m_iAnimation && ANIM_IDLE == m_pModel->Get_CurrentAnimation())
		{
			m_eAnimState = ANIMSTATE::IDLE;
			m_eDirection = DIRECTION::FORWARD;
		}


	}
	else if (m_eAnimState == ANIMSTATE::JUMP)
	{
		if (ANIM_JUMPLANDING == m_pModel->Get_CurrentAnimation())
		{
			m_eAnimState = ANIMSTATE::IDLE;
			
			m_pGauntlet->Initialize_JumpAttCount();
			Set_GauntletIdle();

			m_bGauntletJumpAttackFinal = false;
			m_bSpearJumpAttackFinal = false;

		}
	}
	else if (m_eAnimState == ANIMSTATE::JUMPLANDINGRUN)
	{
		m_bContinue = false;
		m_eAnimState = ANIMSTATE::SLIDING;

		m_iAnimation = ANIM_RUNSLIDING_RIGHT;

		m_bJump = false;
		m_pTransform->Initialize_JumpTime();
		m_pTransform->Set_Jump(false);
		m_pGauntlet->Initialize_JumpAttCount();
		Set_GauntletIdle();

		m_bGauntletJumpAttackFinal = false;
		m_bSpearJumpAttackFinal = false;
	}
}

void CPlayer::Check_RunState(_double TimeDelta)
{
	if (ANIMSTATE::ATTACK != m_eAnimState && ANIMSTATE::JUMP != m_eAnimState && ANIMSTATE::JUMPATTACK != m_eAnimState && ANIMSTATE::JUMPPOWERATTACK != m_eAnimState)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		if (m_bSpurt)
		{
			if (ANIM_FRONT_AVOID == iCurAnimIndex || ANIM_LEFT_AVOID == iCurAnimIndex || ANIM_RIGHT_AVOID == iCurAnimIndex || ANIM_BACK_AVOID == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_SPURT;
				}
			}
			else if (m_eAnimState != ANIMSTATE::SPURT)
			{
				if (DIRECTION::LEFT == m_eDirection)
					m_iAnimation = ANIM_LEFT_AVOID;
				else if (DIRECTION::RIGHT == m_eDirection)
					m_iAnimation = ANIM_RIGHT_AVOID;
				else if (DIRECTION::BACKWARD == m_eDirection)
					m_iAnimation = ANIM_BACK_AVOID;
				else
					m_iAnimation = ANIM_FRONT_AVOID;
			}

			m_eAnimState = ANIMSTATE::SPURT;
			CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 1.f);
			CSoundMgr::Get_Instance()->PlaySound(L"2B_Run.mp3", CSoundMgr::CHANNELID::PLAYER);
			CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 1.f);
			m_bContinue = true;
		}
		else if (m_bRun)
		{
			if (ANIM_SPURT == iCurAnimIndex)
			{
				m_bContinue = false;
				m_iAnimation = ANIM_JUMPLANDING_RUN;

				m_pModel->Set_NoneBlend();
			}
			else if (ANIM_JUMPLANDING_RUN == iCurAnimIndex)
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					if (m_iAnimation != ANIM_RUN)
						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER);
					CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 0.5f);
					CSoundMgr::Get_Instance()->PlaySound(L"2B_Walk.mp3", CSoundMgr::CHANNELID::PLAYER);
					CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 0.5f);
					m_bContinue = false;
					m_iAnimation = ANIM_RUN;

					m_pModel->Set_NoneBlend();
				}
			}
			else
			{
				if (m_iAnimation != ANIM_RUN)
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER);
				CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 0.5f);
				CSoundMgr::Get_Instance()->PlaySound(L"2B_Walk.mp3", CSoundMgr::CHANNELID::PLAYER);
				CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::PLAYER, 0.5f);
				m_bContinue = true;
				m_iAnimation = ANIM_RUN;
			}

			m_eAnimState = ANIMSTATE::RUN;
		}
		else
		{
			m_iAnimation = ANIM_WALK;
			m_eAnimState = ANIMSTATE::WALK;


			m_bContinue = true;
		}
	}
}

void CPlayer::Check_JumpState(_double TimeDelta, _bool & bJumpKeyDown, _bool & bPlayAnim)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if ((m_bJump && ((m_eAnimState == ANIMSTATE::JUMP || m_eAnimState == ANIMSTATE::JUMPPOWERATTACK))) || ANIM_JUMPLANDING_RUN == m_pModel->Get_CurrentAnimation())
	{
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_float fY = 0.f;
		
		if ((_uint)CMainCamera::OPTION::COASTER == m_iCamOption)
		{

			CNavigation* pNavigation = (CNavigation*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", COM_KEY_NAVIGATION, 0);
			if (nullptr != pNavigation)
			{
				if (m_pTransform->IsLowThanCell(pNavigation, &fY))
				{
					vPos = XMVectorSetY(vPos, fY);

					m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
					m_pTransform->Initialize_AttCount();
					m_pTransform->Set_PowerAtt(false);

					m_bHangOn = false;
					m_SpacePressingTime = 0.0;

					_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
					if (m_eAnimState == ANIMSTATE::JUMP || ANIM_GAUNTLET_JUMPPOWERATTACK_DW == iCurAnimIndex)
					{
						if (ANIM_GAUNTLET_JUMPPOWERATTACK_DW == m_iAnimation)
						{
							m_bContinue = true;
							m_pGauntlet->Set_StopTime(TimeDelta);

							if (0.01 < m_pGauntlet->Get_StopTime())
							{
								m_eAnimState = ANIMSTATE::JUMP;

								m_iAnimation = ANIM_JUMPLANDING;

								m_pGauntlet->Initialize_JumpAttCount();
								m_pGauntlet->Initialize_StopTime();

								m_bGauntletJumpAttackFinal = false;
								m_bSpearJumpAttackFinal = false;

								Set_GauntletIdle();
							}
						}
						else
						{
							if (m_bWASDKey)
								m_iAnimation = ANIM_JUMPLANDING_RUN;
							else
							{
								if (ANIM_JUMPLANDING_RUN != m_iAnimation)
									m_iAnimation = ANIM_JUMPLANDING;

								m_pGauntlet->Initialize_JumpAttCount();

								m_bGauntletJumpAttackFinal = false;
								m_bSpearJumpAttackFinal = false;
							}
						}

						if (PODSTATE::HANG == m_pPod->Get_PodState())
							m_pPod->Set_PodState(PODSTATE::IDLE);

					}
					if (ANIMSTATE::JUMPPOWERATTACK == m_eAnimState)
					{
						if (EQUIPSTATE::KATANA == m_eEquipState)
						{
							if (m_iAnimation != ANIM_KATANA_JUMPDW_POWERATTACK_END)
							{
								CDecalCube::DECALCUBE tCube;
								tCube.bContinue = false;
								tCube.dDuration = 1.25;
								tCube.iImageIndex = 1;
								XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION) + m_pTransform->Get_State(CTransform::STATE::LOOK) * 2.f);
								tCube.vPosition.y -= 0.25f;
								
								tCube.dRotationSpeed = 0.15;

								tCube.vScale = _float4(1.f, 1.f, 1.f, 1.f);
								tCube.fAddScale = 0.275f;
								tCube.eType = CDecalCube::TYPE::FOGRING;

								pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube);
							}
								m_iAnimation = ANIM_KATANA_JUMPDW_POWERATTACK_END;
								m_pKatana->Set_Animation(KATANA_JUMPPOWERATTACK_END);
						}

						else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
						{
							if (m_iAnimation != ANIM_LONGKATANA_JUMPDW_ATTACK_END)
							{
								CDecalCube::DECALCUBE tCube;
								tCube.bContinue = false;
								tCube.dDuration = 1.25;
								tCube.iImageIndex = 1;
								XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION) + m_pTransform->Get_State(CTransform::STATE::LOOK) * 2.f);
								tCube.vPosition.y -= 0.25f;

								tCube.dRotationSpeed = 0.15;

								tCube.vScale = _float4(1.f, 1.f, 1.f, 1.f);
								tCube.fAddScale = 0.275f;
								tCube.eType = CDecalCube::TYPE::FOGRING;

								pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube);
							}
							m_iAnimation = ANIM_LONGKATANA_JUMPDW_ATTACK_END;
							m_pLongKatana->Set_Animation(25);
						}
					}
				}
			}
		}
		else
		{
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				vPos = XMVectorSetY(vPos, fY);

				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
				m_pTransform->Initialize_AttCount();
				m_pTransform->Set_PowerAtt(false);

				m_bHangOn = false;
				m_SpacePressingTime = 0.0;

				_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
				if (m_eAnimState == ANIMSTATE::JUMP || ANIM_GAUNTLET_JUMPPOWERATTACK_DW == iCurAnimIndex)
				{
					if (ANIM_GAUNTLET_JUMPPOWERATTACK_DW == m_iAnimation)
					{
						m_bContinue = true;
						m_pGauntlet->Set_StopTime(TimeDelta);

						if (0.01 < m_pGauntlet->Get_StopTime())
						{
							m_eAnimState = ANIMSTATE::JUMP;

							m_iAnimation = ANIM_JUMPLANDING;

							m_pTransform->Initialize_JumpTime(); //
							m_pGauntlet->Initialize_JumpAttCount();
							m_pGauntlet->Initialize_StopTime();

							m_bGauntletJumpAttackFinal = false;
							m_bSpearJumpAttackFinal = false;

							Set_GauntletIdle();
						}
					}
					else
					{
						if (m_bWASDKey)
							m_iAnimation = ANIM_JUMPLANDING_RUN;
						else
						{
							if (ANIM_JUMPLANDING_RUN != m_iAnimation)
								m_iAnimation = ANIM_JUMPLANDING;

							m_pTransform->Initialize_JumpTime(); //
							m_pGauntlet->Initialize_JumpAttCount();

							m_bGauntletJumpAttackFinal = false;
							m_bSpearJumpAttackFinal = false;
						}
					}

					if (PODSTATE::HANG == m_pPod->Get_PodState())
						m_pPod->Set_PodState(PODSTATE::IDLE);

				}
				if (ANIMSTATE::JUMPPOWERATTACK == m_eAnimState)
				{
					if (EQUIPSTATE::KATANA == m_eEquipState)
					{
						if (m_iAnimation != ANIM_KATANA_JUMPDW_POWERATTACK_END)
						{
							CDecalCube::DECALCUBE tCube;
							tCube.bContinue = false;
							tCube.dDuration = 0.5;
							tCube.iImageIndex = 1;
							XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION) + m_pTransform->Get_State(CTransform::STATE::LOOK) * 2.f);
							tCube.vPosition.y -= 0.35f;

							tCube.dRotationSpeed = 0.35;

							tCube.vScale = _float4(1.f, 1.f, 1.f, 1.f);
							tCube.fAddScale = 0.5f;
							tCube.eType = CDecalCube::TYPE::FOGRING;

							pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube);
						}

						m_iAnimation = ANIM_KATANA_JUMPDW_POWERATTACK_END;
						m_pKatana->Set_Animation(KATANA_JUMPPOWERATTACK_END);
						
						m_pTransform->Initialize_JumpTime(); //
						m_pTransform->Set_PowerAtt(false);
					}

					else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
					{
						if (m_iAnimation != ANIM_LONGKATANA_JUMPDW_ATTACK_END)
						{
							CDecalCube::DECALCUBE tCube;
							tCube.bContinue = false;
							tCube.dDuration = 1.25;
							tCube.iImageIndex = 1;
							XMStoreFloat4(&tCube.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION) + m_pTransform->Get_State(CTransform::STATE::LOOK) * 2.f);
							tCube.vPosition.y -= 0.25f;

							tCube.dRotationSpeed = 0.15;

							tCube.vScale = _float4(1.f, 1.f, 1.f, 1.f);
							tCube.fAddScale = 0.275f;
							tCube.eType = CDecalCube::TYPE::FOGRING;

							pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), L"DecalCube", L"DecalEffect", &tCube);
						}
						m_iAnimation = ANIM_LONGKATANA_JUMPDW_ATTACK_END;
						m_pLongKatana->Set_Animation(25);
					
						m_pTransform->Initialize_JumpTime(); //
						m_pTransform->Set_PowerAtt(false);
					}
				}
			}
		}

		if (ANIM_JUMP == m_pModel->Get_CurrentAnimation())
		{
			if (bJumpKeyDown)
			{
				if (ANIM_DOUBLEJUMP != m_iAnimation)
				{
					if (!m_bHangOn)
					{
						if (m_iAnimation != ANIM_DOUBLEJUMP)
						{
							CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_EFFECT);
							CSoundMgr::Get_Instance()->PlaySound(L"Player_Jump.wav", CSoundMgr::PLAYER_EFFECT);
						}

						m_iAnimation = ANIM_DOUBLEJUMP;
						m_pTransform->Initialize_JumpTime();

						// For. 9S
						ANDROID_DATAPACKET tPacket{};
						tPacket.bDoubleJump = true;

						Update_DataFor9S(tPacket);
						//
					}
				}
			}
		}

		else if (ANIM_JUMPLANDING == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_eAnimState = ANIMSTATE::IDLE;
				m_bJump = false;
				bPlayAnim = false;
				m_pTransform->Initialize_JumpTime();
				m_pTransform->Set_Jump(false);

				m_pGauntlet->Initialize_JumpAttCount();

				m_bGauntletJumpAttackFinal = false;
				m_bSpearJumpAttackFinal = false;

				Set_GauntletIdle();
		
			}

			if (bJumpKeyDown)
			{
				m_eAnimState = ANIMSTATE::JUMP;

				if (ANIM_JUMP != m_iAnimation)
				{
					m_pTransform->Initialize_AttCount();
					m_pTransform->Initialize_JumpTime();
					m_SpacePressingTime = 0.0;
					m_bHangOn = false;

					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_EFFECT);
					CSoundMgr::Get_Instance()->PlaySound(L"Player_Jump.wav", CSoundMgr::PLAYER_EFFECT);

				}
				m_iAnimation = ANIM_JUMP;

				m_bPressingR = false;
				m_bKatanaR = false;
				m_bThrowSpear = false;
				m_bLKatanaRush = false;
				m_bGauntletSpin = false;

				m_bJump = true;
				m_pTransform->Set_Jump(true);
				bPlayAnim = true;

				m_pModel->Immediately_Change(ANIM_JUMP);
				//CSoundMgr::Get_Instance()->PlaySound(L"Jump.mp3", CSoundMgr::PLAYER);

				// For. 9S
				ANDROID_DATAPACKET tPacket{};
				tPacket.bJump = true;
				tPacket.bImmediately = true;

				Update_DataFor9S(tPacket);
				//
			}
		}
		else if (ANIM_DOUBLEJUMP == m_pModel->Get_CurrentAnimation())
		{
			if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
			{
				vPos = XMVectorSetY(vPos, fY);

				m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);
				m_iAnimation = ANIM_JUMPLANDING;
			}
		}
		else if (ANIM_JUMPLANDING_RUN == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_eAnimState = ANIMSTATE::RUN;
				m_iAnimation = ANIM_RUN;

				m_bJump = false;
				bPlayAnim = true;

				m_pTransform->Initialize_JumpTime();
				m_pTransform->Set_Jump(false);

				m_pGauntlet->Initialize_JumpAttCount();

				m_bGauntletJumpAttackFinal = false;
				m_bSpearJumpAttackFinal = false;

				Set_GauntletIdle();
			}
			else
			{
				if (bJumpKeyDown)
				{
					m_eAnimState = ANIMSTATE::JUMP;

					if (ANIM_JUMP != m_iAnimation)
					{
						m_pTransform->Initialize_AttCount();
						m_pTransform->Initialize_JumpTime();

						CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_EFFECT);
						CSoundMgr::Get_Instance()->PlaySound(L"Player_Jump.wav", CSoundMgr::PLAYER_EFFECT);
					}

					m_iAnimation = ANIM_JUMP;

					m_bPressingR = false;
					m_bKatanaR = false;
					m_bThrowSpear = false;
					m_bLKatanaRush = false;
					m_bGauntletSpin = false;

					m_bJump = true;
					m_pTransform->Set_Jump(true);
					bPlayAnim = true;

					m_pModel->Immediately_Change(ANIM_JUMP);

					// For. 9S
					ANDROID_DATAPACKET tPacket{};
					tPacket.bJump = true;

					Update_DataFor9S(tPacket);
					//
				}
			}
		}

		else if (ANIM_KATANA_JUMPDW_POWERATTACK_END == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_eAnimState = ANIMSTATE::IDLE;
				m_iAnimation = ANIM_IDLE;
				bPlayAnim = false;
				m_pTransform->Initialize_JumpTime();
				m_pTransform->Initialize_AttCount();
				m_pTransform->Set_Jump(false);
				m_bRButtonDown = false;
				m_bJump = false;

				//Set_KatanaIdle();
				m_pKatana->Set_Dissolve();

			}
		}
		else if (ANIM_LONGKATANA_JUMPDW_ATTACK_END == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
			{
				m_eAnimState = ANIMSTATE::IDLE;
				m_iAnimation = ANIM_IDLE;
				m_pTransform->Initialize_JumpTime();
				m_pTransform->Set_Jump(false);
				m_bRButtonDown = false;
				bPlayAnim = false;
				m_bJump = false;

				//Set_LongKatanaIdle();
				m_pLongKatana->Set_Dissolve();
			}
		}

	}
	else
	{
		if (EQUIPSTATE::SPEAR == m_eEquipState || EQUIPSTATE::GAUNTLET == m_eEquipState)
		{
			if (ANIMSTATE::JUMP == m_eAnimState)
			{
				m_bJump = true;
				if (m_iAnimation != ANIM_JUMP)
				{
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_EFFECT);
					CSoundMgr::Get_Instance()->PlaySound(L"Player_Jump.wav", CSoundMgr::PLAYER_EFFECT);
				}
				m_iAnimation = ANIM_JUMP;

				m_bPressingR = false;
				m_bKatanaR = false;
				m_bThrowSpear = false;
				m_bLKatanaRush = false;
				m_bGauntletSpin = false;

				// For. 9S
				ANDROID_DATAPACKET tPacket{};
				tPacket.bJump = true;

				Update_DataFor9S(tPacket);
				//
			}
		}
		else
		{
			if (ANIM_JUMP != m_iAnimation)
			{
				m_pTransform->Set_PowerAtt(false);
				m_pTransform->Initialize_JumpTime();

				CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER_EFFECT);
				CSoundMgr::Get_Instance()->PlaySound(L"Player_Jump.wav", CSoundMgr::PLAYER_EFFECT);
			}

			if (ANIMSTATE::JUMP == m_eAnimState)
			{
				m_bJump = true;
				m_iAnimation = ANIM_JUMP;
				
				m_bPressingR = false;
				m_bKatanaR = false;
				m_bThrowSpear = false;
				m_bLKatanaRush = false;
				m_bGauntletSpin = false;

				bPlayAnim = true;

				// For. 9S
				ANDROID_DATAPACKET tPacket{};
				tPacket.bJump = true;

				Update_DataFor9S(tPacket);
				//
			}
		}
	}
}

void CPlayer::Check_AvoidState(_bool& bPlayAnim)
{
	if (m_eAnimState != ANIMSTATE::JUMP && ANIMSTATE::JUMPATTACK != m_eAnimState && ANIMSTATE::JUMPPOWERATTACK != m_eAnimState)
	{
		bPlayAnim = true;
		m_bContinue = false;

		m_bLButtonDown = false;
		m_bRButtonDown = false;

		if (EQUIPSTATE::KATANA == m_eEquipState)
		{
			if (m_pKatana)
			{
				if (!m_bSpurt)
				{
					m_pKatana->Set_WeaponState(WEAPONSTATE::IDLE);
					//Set_KatanaIdle();
					m_pKatana->Set_Dissolve();
					m_bKatanaR = false;

					m_pKatana->End_Trail();
				}

			}
		}
		if (EQUIPSTATE::LONGKATANA == m_eEquipState)
		{
			if (m_pLongKatana)
			{
				m_pLongKatana->Set_WeaponState(WEAPONSTATE::IDLE);
				Set_LongKatanaIdle();
				m_bLKatanaRush = false;
				m_bPressingR = false;

				m_pLongKatana->End_Trail();
			}
		}
		else if (EQUIPSTATE::SPEAR == m_eEquipState)
		{
			if (m_pSpear)
			{
				m_pSpear->Set_WeaponState(WEAPONSTATE::IDLE);
				Set_SpearIdle();
				m_bThrowSpear = false;
			}

		}
		else if (EQUIPSTATE::GAUNTLET == m_eEquipState)
		{
			if (m_pGauntlet)
			{
				m_pGauntlet->Set_WeaponState(WEAPONSTATE::IDLE);
				Set_GauntletIdle();
				m_bGauntletSpin = false;
			}
		}

		m_eAnimState = ANIMSTATE::AVOID;

		if ((_uint)CMainCamera::OPTION::STAIR == m_iCamOption || (_uint)CMainCamera::OPTION::COASTER == m_iCamOption)
			m_eDirection = DIRECTION::FORWARD;

		if (DIRECTION::BACKWARD == m_eDirection)
		{
			m_iAnimation = ANIM_BACK_AVOID;
			if (ANIM_BACK_AVOID == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_BACK_AVOID_SLIDING;
					m_eAnimState = ANIMSTATE::SLIDING;
				}
			}
		}
		else if (DIRECTION::LEFT == m_eDirection)
		{
			m_iAnimation = ANIM_LEFT_AVOID;
			if (ANIM_LEFT_AVOID == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_LEFT_AVOID_SLIDING;
					m_eAnimState = ANIMSTATE::SLIDING;
				}
			}
		}
		else if (DIRECTION::RIGHT == m_eDirection)
		{
			m_iAnimation = ANIM_RIGHT_AVOID;
			if (ANIM_RIGHT_AVOID == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_RIGHT_AVOID_SLIDING;
					m_eAnimState = ANIMSTATE::SLIDING;
				}
			}
		}
		else
		{
			m_iAnimation = ANIM_FRONT_AVOID;

			if (ANIM_FRONT_AVOID == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
				{
					m_iAnimation = ANIM_FRONT_AVOID_SLIDING;
					m_eAnimState = ANIMSTATE::SLIDING;
				}
			}
		}
	}
}

void CPlayer::Check_MoveRun(_bool& bPlayAnim, _double TimeDelta)
{
	if (ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::LASERATTACK == m_eAnimState)
		return;

	Initialize_WASD_Key();

	if (ANIM_JUMPLANDING == m_pModel->Get_CurrentAnimation())
		return;

	if (m_bTargeting && (m_bLButtonDown || m_bRButtonDown))
		return;
	else
		m_TargetLookLerpTime = 0.0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_bool bMove = false;
	_bool bJumpMove = false;
	_bool bTempWASDKey = false;

	if (m_eAnimState != ANIMSTATE::AVOID && m_eAnimState != ANIMSTATE::LASERATTACK)
	{
		if ((_uint)CMainCamera::OPTION::CATMULLROM_3RD_TO_WHEEL == m_iCamOption)
		{
			if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_A)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (DIRECTION::FORWARD != m_eDirection)
					m_LookLerpTime = 0.0;

				m_eDirection = DIRECTION::FORWARD;

				if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
					Check_RunState(TimeDelta);

				if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
					|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
					LerpToLeftofCenter(TimeDelta);
			}
			else if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_D)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (DIRECTION::BACKWARD != m_eDirection)
					m_LookLerpTime = 0.0;

				m_eDirection = DIRECTION::BACKWARD;

				if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
					Check_RunState(TimeDelta);

				if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
					|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
					LerpToRightofCenter(TimeDelta);
			}
		}
		else
		{
			if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_S)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (pGameInstance->Key_Pressing(DIK_A))
				{
					if (DIRECTION::DIAGONAL_BACKLEFT != m_eDirection)
						Set_StartCamDiagonalBackLeft();

					m_eDirection = DIRECTION::DIAGONAL_BACKLEFT;

					if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						Rotate_Camera_DiagonalBackLeft(TimeDelta);
				}
				else if (pGameInstance->Key_Pressing(DIK_D))
				{
					if (DIRECTION::DIAGONAL_BACKRIGHT != m_eDirection)
						Set_StartCamDiagonalBackRight();

					m_eDirection = DIRECTION::DIAGONAL_BACKRIGHT;

					if (!m_pModel->Get_Blend() && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						Rotate_Camera_DiagonalBackRight(TimeDelta);
				}
				else
				{
					if (DIRECTION::BACKWARD != m_eDirection)
						m_LookLerpTime = 0.0;

					m_eDirection = DIRECTION::BACKWARD;

					if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						LerpToCameraLook(TimeDelta);
				}
			}

			else if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_W)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (pGameInstance->Key_Pressing(DIK_A))
				{
					if (DIRECTION::DIAGONAL_LEFT != m_eDirection)
						Set_StartCamDiagonalLeft();

					m_eDirection = DIRECTION::DIAGONAL_LEFT;

					if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						Rotate_Camera_DiagonalLeft(TimeDelta);

				}
				else if (pGameInstance->Key_Pressing(DIK_D))
				{
					if (DIRECTION::DIAGONAL_RIGHT != m_eDirection)
						Set_StartCamDiagonalRight();

					m_eDirection = DIRECTION::DIAGONAL_RIGHT;

					if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						Rotate_Camera_DiagonalRight(TimeDelta);
				}
				else
				{
					if (DIRECTION::FORWARD != m_eDirection)
						m_LookLerpTime = 0.0;

					m_eDirection = DIRECTION::FORWARD;

					if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
						Check_RunState(TimeDelta);

					if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
						|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
						LerpToCameraLook(TimeDelta);
				}
			}

			else if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_A)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (DIRECTION::LEFT != m_eDirection)
					Set_StartCamLeft();

				m_eDirection = DIRECTION::LEFT;

				_uint iAnimIndex = m_pModel->Get_CurrentAnimation();
				if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
					Check_RunState(TimeDelta);


				if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
					|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
					Rotate_Camera_Left(TimeDelta);
			}

			else if ((bTempWASDKey = pGameInstance->Key_Pressing(DIK_D)))
			{
				m_bWASDKey = bTempWASDKey;

				bMove = true;
				bPlayAnim = true;

				if (false == bJumpMove)
					JumpMoveForTick(TimeDelta, bPlayAnim, bJumpMove);

				if (DIRECTION::RIGHT != m_eDirection)
					Set_StartCamRight();

				m_eDirection = DIRECTION::RIGHT;

				_uint iAnimIndex = m_pModel->Get_CurrentAnimation();
				if ((!m_pModel->Get_Blend()) && (ANIM_JUMPLANDING_RUN != m_pModel->Get_CurrentAnimation() || false == m_pTransform->IsJump()) && ANIMSTATE::POWERATTACK != m_eAnimState)
					Check_RunState(TimeDelta); 



				if (ANIMSTATE::RUN == m_eAnimState || ANIMSTATE::SPURT == m_eAnimState || ANIMSTATE::WALK == m_eAnimState || ANIMSTATE::ATTACK == m_eAnimState || ANIMSTATE::POWERATTACK == m_eAnimState
					|| ANIMSTATE::JUMP == m_eAnimState || ANIMSTATE::JUMPATTACK == m_eAnimState || ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMPLANDINGRUN == m_eAnimState)
					Rotate_Camera_Right(TimeDelta);
			}
		}
	}

	//CSoundMgr*	pSoundMgr = CSoundMgr::Get_Instance();

	//static _double	SoundTimeAcc = 0.0;

	//SoundTimeAcc += TimeDelta;

	//if (0.5 < SoundTimeAcc)
	//{
	//	if (true == m_bWalk)
	//	{
	//		TimeDelta

	//			pSoundMgr->StopPlaySound(TEXT("WalkingA.mp3"), CSoundMgr::WALK);

	//		pSoundMgr->PlaySound(TEXT("WalkingA.mp3"), CSoundMgr::CHANNELID::WALK);
	//	}
	//	SoundTimeAcc = 0.0;
	//}

	//if (true == m_bWalk)
	//{
	//	TimeDelta

	//	pSoundMgr->StopPlaySound(TEXT("WalkingA.mp3"), CSoundMgr::WALK);

	//	pSoundMgr->PlaySound(TEXT("WalkingA.mp3"), CSoundMgr::CHANNELID::WALK);
	//}
	//else
	//{
	//	pSoundMgr->StopSound(CSoundMgr::WALK);
	//}

	Check_LerpTime(bMove);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Check_JumpAttack(_bool& bDown)
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();

	if (m_eAnimState == ANIMSTATE::JUMP || m_eAnimState == ANIMSTATE::JUMPATTACK)
	{
		if (ANIM_JUMPLANDING == m_pModel->Get_CurrentAnimation() || ANIM_JUMPLANDING_RUN == m_pModel->Get_CurrentAnimation())
			return;

		if (ANIM_JUMP_DW == m_pModel->Get_CurrentAnimation() && (ANIM_JUMPLANDING == m_pModel->Get_NextAnimation() || ANIM_JUMPLANDING_RUN == m_pModel->Get_NextAnimation()))
			return;

		_double TimeRatio = m_pModel->Get_PlayTimeRatio();
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		_uint iCount = m_pTransform->Get_AttCount();

		if (EQUIPSTATE::KATANA == m_eEquipState)
		{
			if (5 > iCount)
			{
				if (0 == iCount)
				{
					if (m_iAnimation != ANIM_KATANA_JUMPATTACK)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("KatanaJump_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
					}
					m_iAnimation = ANIM_KATANA_JUMPATTACK;
					m_pKatana->Set_Animation(KATANA_JUMPATTACK1);
					m_pKatana->Set_Independence(false);
					
					if (bDown)
					{
						if (ANIM_KATANA_JUMPATTACK == iCurAnimIndex)
						{
							if ((m_pModel->Get_LinkMinRatio() <= TimeRatio) && (m_pModel->Get_LinkMaxRatio() >= TimeRatio))
							{
								m_pTransform->Add_AttCount();
								if (m_iAnimation != ANIM_KATANA_JUMPATTACK2)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("KatanaJump_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								m_iAnimation = ANIM_KATANA_JUMPATTACK2;

								m_pKatana->Set_Animation(KATANA_JUMPATTACK2);
							}
						}
					}
				}
				else if (1 == iCount)
				{
					if (ANIM_JUMP_DW == iCurAnimIndex)
						return;

					m_iAnimation = ANIM_KATANA_JUMPATTACK2;
					m_pKatana->Set_Animation(KATANA_JUMPATTACK2);

					if (bDown)
					{
						if (ANIM_KATANA_JUMPATTACK2 == iCurAnimIndex)
						{
							if ((m_pModel->Get_LinkMinRatio() <= TimeRatio) && (m_pModel->Get_LinkMaxRatio() >= TimeRatio))
							{
								m_pTransform->Add_AttCount();
								if (m_iAnimation != ANIM_KATANA_JUMPATTACK)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("KatanaJump_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								m_iAnimation = ANIM_KATANA_JUMPATTACK;

								m_pKatana->Set_Animation(KATANA_JUMPATTACK1);
							}
						}
					}
				}
				else if (2 == iCount)
				{
					if (ANIM_JUMP_DW == iCurAnimIndex)
						return;

					m_iAnimation = ANIM_KATANA_JUMPATTACK;
					m_pKatana->Set_Animation(KATANA_JUMPATTACK1);

					if (bDown)
					{
						if (ANIM_KATANA_JUMPATTACK == iCurAnimIndex)
						{
							if ((m_pModel->Get_LinkMinRatio() <= TimeRatio) && (m_pModel->Get_LinkMaxRatio() >= TimeRatio))
							{
								m_pTransform->Add_AttCount();
								if (m_iAnimation != ANIM_KATANA_JUMPATTACK2)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("KatanaJump_L4.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								m_iAnimation = ANIM_KATANA_JUMPATTACK2;

								m_pKatana->Set_Animation(KATANA_JUMPATTACK2);
							}
						}
					}
				}
				else if (3 == iCount)
				{
					if (ANIM_JUMP_DW == iCurAnimIndex)
						return;

					m_iAnimation = ANIM_KATANA_JUMPATTACK2;
					m_pKatana->Set_Animation(KATANA_JUMPATTACK2);

					if (bDown)
					{
						if (ANIM_KATANA_JUMPATTACK2 == iCurAnimIndex)
						{
							if ((m_pModel->Get_LinkMinRatio() <= TimeRatio) && (m_pModel->Get_LinkMaxRatio() >= TimeRatio))
							{
								m_pTransform->Add_AttCount();
								if (m_iAnimation != ANIM_KATANA_JUMPATTACK3)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("KatanaJump_L5.mp3"), CSoundMgr::P_WEAPONSOUND);
								}
								m_iAnimation = ANIM_KATANA_JUMPATTACK3;

								m_pKatana->Set_Animation(KATANA_JUMPATTACK3);
							}
						}
					}
				}
				else if (4 == iCount)
				{
					if (ANIM_JUMP_DW == iCurAnimIndex)
						return;

					m_iAnimation = ANIM_KATANA_JUMPATTACK3;
					m_pKatana->Set_Animation(KATANA_JUMPATTACK3);
				}
			}

			if (ANIM_KATANA_JUMPATTACK == iCurAnimIndex || ANIM_KATANA_JUMPATTACK2 == iCurAnimIndex || ANIM_KATANA_JUMPATTACK3 == iCurAnimIndex)
			{
				m_pKatana->Set_WeaponState(WEAPONSTATE::KATANAJUMPATTACK);

				if (m_pModel->Get_LinkMaxRatio() < TimeRatio)
				{
					m_eAnimState = ANIMSTATE::JUMP;
					m_iAnimation = ANIM_JUMP_DW;
					m_pTransform->Set_FallTime();

					if (KATANA_JUMPATTACK3 == m_pKatana->Get_AnimationIndex())
						m_pKatana->Set_Independence(true);
					else 
					{
						//Set_KatanaIdle();
						m_pKatana->Set_Dissolve();
					}
				}
				else
					m_eAnimState = ANIMSTATE::JUMPATTACK;
			}
			else
				m_eAnimState = ANIMSTATE::JUMPATTACK;
		}
		else if (EQUIPSTATE::SPEAR == m_eEquipState)
		{
			if (m_bSpearJumpAttackFinal)
				return;

			_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
			_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

			if (ANIM_SPEAR_JUMPATTACK2 != m_iAnimation && ANIM_SPEAR_JUMPATTACK3 != m_iAnimation)
			{
				if (0 == iCount)
				{
					m_pSpear->Set_WeaponState(WEAPONSTATE::SPEARJUMPATTACK);

					if (m_iAnimation != ANIM_SPEAR_JUMPATTACK1)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Spear_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
					}

					m_iAnimation = ANIM_SPEAR_JUMPATTACK1;
					m_pSpear->Set_Animation(99);

					m_pTransform->Add_AttCount();
					m_eAnimState = ANIMSTATE::JUMPATTACK;
				}
				
				if (ANIM_SPEAR_JUMPATTACK1 == iCurAnimIndex)
				{
					if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
					{
						if (bDown)
						{
							if (m_iAnimation != ANIM_SPEAR_JUMPATTACK2)
							{
								pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
								pSoundMgr->PlaySound(TEXT("Spear_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
							}

							m_iAnimation = ANIM_SPEAR_JUMPATTACK2;
							m_pSpear->Set_Animation(98);
						}
					}

					if (LinkMaxRatio < TimeRatio)
					{
						if (m_iAnimation != ANIM_SPEAR_JUMPATTACK2)
						{
							m_eAnimState = ANIMSTATE::JUMP;
							m_iAnimation = ANIM_JUMP_DW;
							m_pTransform->Set_FallTime();

							Set_SpearIdle();
						}
					}
				}
			}
			else if (ANIM_SPEAR_JUMPATTACK2 == m_iAnimation)
			{
				if (ANIM_JUMP_DW == iCurAnimIndex)
					return;

				if (1 == iCount)
				{
					m_iAnimation = ANIM_SPEAR_JUMPATTACK2;
					m_pSpear->Set_Animation(98);

					m_pTransform->Add_AttCount();
				}

				if (ANIM_SPEAR_JUMPATTACK2 == iCurAnimIndex)
				{
					if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
					{
						if (bDown)
						{
							m_iAnimation = ANIM_SPEAR_JUMPATTACK3;
							m_pSpear->Set_Animation(10);
						}
					}

					if (LinkMaxRatio < TimeRatio)
					{
						if (m_iAnimation != ANIM_SPEAR_JUMPATTACK3)
						{
							m_eAnimState = ANIMSTATE::JUMP;
							m_iAnimation = ANIM_JUMP_DW;
							m_pTransform->Set_FallTime();

							Set_SpearIdle();
						}
					}
				}
			}
			else if (ANIM_SPEAR_JUMPATTACK3 == m_iAnimation)
			{
				if (2 == iCount)
				{
					m_iAnimation = ANIM_SPEAR_JUMPATTACK3;
					m_pSpear->Set_Animation(10);

					m_pTransform->Add_AttCount();
				}

				if (ANIM_SPEAR_JUMPATTACK3 == iCurAnimIndex)
				{
					if (LinkMaxRatio < TimeRatio)
					{
						m_eAnimState = ANIMSTATE::JUMP;
						m_iAnimation = ANIM_JUMP_DW;
						m_pTransform->Set_FallTime();

						Set_SpearIdle();

						m_bSpearJumpAttackFinal = true;
					}
					else if (0.29 <= TimeRatio && 0.30 >= TimeRatio)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Spear_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
					}
				}
			}
		}
		else if (EQUIPSTATE::GAUNTLET == m_eEquipState)
		{
			if (m_bGauntletJumpAttackFinal)
				return;

			_double LinkMinRatio = m_pModel->Get_LinkMinRatio();
			_double LinkMaxRatio = m_pModel->Get_LinkMaxRatio();

			if (ANIM_GAUNTLET_JUMPATTACK2 != m_iAnimation && ANIM_GAUNTLET_JUMPATTACK3 != m_iAnimation)
			{
				if (0 == iCount)
				{
					m_eAnimState = ANIMSTATE::JUMPATTACK;
					m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETJUMPATTACK);

					if (m_iAnimation != ANIM_GAUNTLET_JUMPATTACK1)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("Gauntlet_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
					}

					m_iAnimation = ANIM_GAUNTLET_JUMPATTACK1;
					m_pGauntlet->Set_Animation(17);

					m_pTransform->Add_AttCount();
				}

				if (ANIM_GAUNTLET_JUMPATTACK1 == iCurAnimIndex)
				{
					if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
					{
						if (bDown)
						{
							if (m_iAnimation != ANIM_GAUNTLET_JUMPATTACK2)
							{
								pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
								pSoundMgr->PlaySound(TEXT("Gauntlet_L2.mp3"), CSoundMgr::P_WEAPONSOUND);
							}

							m_iAnimation = ANIM_GAUNTLET_JUMPATTACK2;
							m_pGauntlet->Set_Animation(65);
						}
					}

					if (LinkMaxRatio < TimeRatio)
					{
						if (m_iAnimation != ANIM_GAUNTLET_JUMPATTACK2)
						{
							m_eAnimState = ANIMSTATE::JUMP;
							m_iAnimation = ANIM_JUMP_DW;
							m_pTransform->Set_FallTime();

							Set_GauntletIdle();
						}
					}
				}
			}
			else if (ANIM_GAUNTLET_JUMPATTACK2 == m_iAnimation)
			{
				if (1 == iCount)
				{
					m_iAnimation = ANIM_GAUNTLET_JUMPATTACK2;
					m_pGauntlet->Set_Animation(65);
		
					m_pTransform->Add_AttCount();
				}

				if (ANIM_GAUNTLET_JUMPATTACK2 == iCurAnimIndex)
				{
					if (LinkMinRatio <= TimeRatio && LinkMaxRatio >= TimeRatio)
					{
						if (bDown)
						{
							if (2 == m_pGauntlet->Get_JumpAttCount())
							{
								if (m_iAnimation != ANIM_GAUNTLET_JUMPATTACK3)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("Gauntlet_L3.mp3"), CSoundMgr::P_WEAPONSOUND);
								}

								m_iAnimation = ANIM_GAUNTLET_JUMPATTACK3;
								m_pGauntlet->Set_Animation(64);
							}
							else
							{
								if (m_iAnimation != ANIM_GAUNTLET_JUMPATTACK1)
								{
									pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
									pSoundMgr->PlaySound(TEXT("Gauntlet_L1.mp3"), CSoundMgr::P_WEAPONSOUND);
								}

								m_iAnimation = ANIM_GAUNTLET_JUMPATTACK1;
								m_pGauntlet->Set_Animation(17);

								m_pGauntlet->Add_JumpAttCount();
							}

						}
					}
					else if(LinkMaxRatio < TimeRatio)
					{
						m_eAnimState = ANIMSTATE::JUMP;
						m_iAnimation = ANIM_JUMP_DW;
						m_pTransform->Set_FallTime();

						Set_GauntletIdle();
					}
				}
			}
			else if (ANIM_GAUNTLET_JUMPATTACK3 == m_iAnimation)
			{
				if (2 == iCount)
				{
					m_iAnimation = ANIM_GAUNTLET_JUMPATTACK3;
					m_pGauntlet->Set_Animation(64);

					m_pTransform->Add_AttCount();
				}

				if (ANIM_GAUNTLET_JUMPATTACK3 == iCurAnimIndex)
				{
					if (LinkMaxRatio < TimeRatio)
					{
						m_eAnimState = ANIMSTATE::JUMP;
						m_iAnimation = ANIM_JUMP_DW;
						m_pTransform->Set_FallTime();

						Set_GauntletIdle();

						m_bGauntletJumpAttackFinal = true;
					}
				}
			}
		}
	}
}

void CPlayer::Check_JumpPowerAttack(_bool & bDown)
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();

	if (ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMP == m_eAnimState)
	{
		if (EQUIPSTATE::KATANA == m_eEquipState)
		{
			if (m_pKatana)
				m_pKatana->Set_WeaponState(WEAPONSTATE::ATTACK);

			_double TimeRatio = m_pModel->Get_PlayTimeRatio();
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
			_uint iCount = m_pTransform->Get_AttCount();

			m_eAnimState = ANIMSTATE::JUMPPOWERATTACK;
			m_bJump = true;

			if (false == m_pModel->Get_Blend())
			{
				if (ANIM_KATANA_JUMPDW_POWERATTACK_SPIKE != iCurAnimIndex && ANIM_KATANA_JUMPDW_POWERATTACK_END != iCurAnimIndex)
				{
					if (m_iAnimation != ANIM_KATANA_JUMPDW_POWERATTACK)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("KatanaJump_R1.mp3"), CSoundMgr::P_WEAPONSOUND);
					}
					m_iAnimation = ANIM_KATANA_JUMPDW_POWERATTACK;
					m_pKatana->Set_Animation(KATANA_JUMPPOWERATTACK);
					m_pKatana->Set_Independence(false);

					Initialize_HangOn();
				}
			}

			if (ANIM_KATANA_JUMPDW_POWERATTACK == iCurAnimIndex)
			{
				m_eAnimState = ANIMSTATE::JUMPPOWERATTACK;

				if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
				{
					if (m_iAnimation != ANIM_KATANA_JUMPDW_POWERATTACK_SPIKE)
					{
						pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
						pSoundMgr->PlaySound(TEXT("KatanaJump_R3.mp3"), CSoundMgr::P_WEAPONSOUND);
					}

					m_iAnimation = ANIM_KATANA_JUMPDW_POWERATTACK_SPIKE;
					m_pKatana->Set_Animation(KATANA_JUMPPOWERATTACK_DW);

					m_pTransform->Set_FallTime();
					m_pTransform->Set_PowerAtt(true);
				}

			}

			else if (ANIM_KATANA_JUMPDW_POWERATTACK_END == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
				{
					m_pTransform->Set_PowerAtt(false);
					m_bRButtonDown = false;

					m_pTransform->Initialize_AttCount();

					//Set_KatanaIdle();
					m_pKatana->Set_Dissolve();
				}
			}
		}
		/*else if (EQUIPSTATE::GAUNTLET == m_eEquipState)			
		{
			if (ANIM_JUMPLANDING == m_iAnimation)
				return;

			_double TimeRatio = m_pModel->Get_PlayTimeRatio();
			_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

			m_pGauntlet->Set_WeaponState(WEAPONSTATE::GAUNTLETJUMPPOWERATTACK);

			m_eAnimState = ANIMSTATE::JUMPPOWERATTACK;
			m_bJump = true;

			if (ANIM_GAUNTLET_JUMPPOWERATTACK_DW != iCurAnimIndex)
			{
				m_iAnimation = ANIM_GAUNTLET_JUMPPOWERATTACK;
				m_pGauntlet->Set_Animation(66);

				if (ANIM_GAUNTLET_JUMPPOWERATTACK == iCurAnimIndex)
				{
					if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
					{
						m_iAnimation = ANIM_GAUNTLET_JUMPPOWERATTACK_DW;
						m_pGauntlet->Set_Animation(56);
					}
				}
			}
			else if (ANIM_GAUNTLET_JUMPPOWERATTACK_DW == iCurAnimIndex)
			{
				m_iAnimation = ANIM_GAUNTLET_JUMPPOWERATTACK_DW;
				m_pGauntlet->Set_Animation(56);

				m_pTransform->Set_FallTime();
				m_pTransform->Set_PowerAtt(true);
			}
		}*/
	}
}

void CPlayer::Check_LongKatana_JumpAttack(_bool & bDown)
{
	CSoundMgr* pSoundMgr = CSoundMgr::Get_Instance();

	if (ANIMSTATE::JUMPPOWERATTACK == m_eAnimState || ANIMSTATE::JUMP == m_eAnimState)
	{
		if (m_pLongKatana)
			m_pLongKatana->Set_WeaponState(WEAPONSTATE::ATTACK);

		_double TimeRatio = m_pModel->Get_PlayTimeRatio();
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

		m_eAnimState = ANIMSTATE::JUMPPOWERATTACK;
		m_bJump = true;

		if (ANIM_LONGKATANA_JUMPDW_ATTACK_SPIKE != m_pModel->Get_CurrentAnimation() && ANIM_LONGKATANA_JUMPDW_ATTACK_END != m_pModel->Get_CurrentAnimation())
		{
			m_iAnimation = ANIM_LONGKATANA_JUMPDW_ATTACK;
			m_pLongKatana->Set_Animation(23);

			m_pLongKatana->Set_PlayerNonLerp(false);
			m_pLongKatana->Set_Independence(false);

			Check_WeaponState();
			Initialize_HangOn();
		}

		if (ANIM_LONGKATANA_JUMPDW_ATTACK == m_pModel->Get_CurrentAnimation())
		{
			m_eAnimState = ANIMSTATE::JUMPPOWERATTACK;

			if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
			{
				if (m_iAnimation != ANIM_LONGKATANA_JUMPDW_ATTACK_SPIKE)
				{
					pSoundMgr->StopSound(CSoundMgr::P_WEAPONSOUND);
					pSoundMgr->PlaySound(TEXT("LKatanaJump_R2.mp3"), CSoundMgr::P_WEAPONSOUND);
				}
				m_iAnimation = ANIM_LONGKATANA_JUMPDW_ATTACK_SPIKE;
				m_pTransform->Set_FallTime();
				m_pTransform->Set_PowerAtt(true);
			}
		}
		else if (ANIM_LONGKATANA_JUMPDW_ATTACK_END == m_pModel->Get_CurrentAnimation())
		{
			if (m_pModel->Get_LinkMaxRatio() <= TimeRatio)
			{
				m_pTransform->Set_PowerAtt(false);
				m_bRButtonDown = false;
			}
		}
	}
}

void CPlayer::Check_WeaponState()
{
	if (nullptr != m_pKatana)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::IDLE == m_pKatana->Get_WeaponState())
			{
				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

				_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
				_float4x4 BoneFloat4x4;
				XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

				_vector vDir;
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

				vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));

				_float4 vPos;
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

				if (m_eEquipState == EQUIPSTATE::KATANA)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * EQUIP_OFFSET));
				else if (m_eEquipStateSub == EQUIPSTATE::KATANA)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * SUBEQUIP_OFFSET));

				memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

				m_pKatana->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
			}
			else
				m_pKatana->Update_Matrix(m_pTransform->Get_WorldMatrix());
		}
	}

	if (nullptr != m_pLongKatana)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::IDLE == m_pLongKatana->Get_WeaponState())
			{
				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

				_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
				_float4x4 BoneFloat4x4;
				XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

				_vector vDir;
				memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

				vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));

				_float4 vPos;
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

				if (m_eEquipState == EQUIPSTATE::LONGKATANA)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * (EQUIP_OFFSET - 0.1f)));
				else if (m_eEquipStateSub == EQUIPSTATE::LONGKATANA)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * SUBEQUIP_OFFSET));

				memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

				m_pLongKatana->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
			}
			else
				m_pLongKatana->Update_Matrix(m_pTransform->Get_WorldMatrix());
		}
	}

	if (nullptr != m_pSpear)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::IDLE == m_pSpear->Get_WeaponState())
			{
				_float4x4 WorldFloat4x4;
				XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

				_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
				_float4x4 BoneFloat4x4;
				XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

				_vector vDir;
				memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

				vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));

				_float4 vPos;
				memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

				if (m_eEquipState == EQUIPSTATE::SPEAR)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * EQUIP_OFFSET));
				else if (m_eEquipStateSub == EQUIPSTATE::SPEAR)
					XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * (SUBEQUIP_OFFSET + 0.1f)));

				memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

				m_pSpear->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
			}
			else
				m_pSpear->Update_Matrix(m_pTransform->Get_WorldMatrix());
		}
	}

	if (nullptr != m_pGauntlet)
	{
		if (WEAPONSTATE::IDLE == m_pGauntlet->Get_WeaponState())
		{
			_float4x4 WorldFloat4x4;
			XMStoreFloat4x4(&WorldFloat4x4, m_pTransform->Get_WorldMatrix());

			_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"Weapon_Idle");
			_float4x4 BoneFloat4x4;
			XMStoreFloat4x4(&BoneFloat4x4, BoneMatrix);

			_vector vDir;
			memcpy(&vDir, &BoneFloat4x4.m[2], sizeof(_vector));

			_vector vTempDir = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::RIGHT)) * -1.f;

			vDir = XMVector3Normalize(XMVector3TransformNormal(vDir, XMLoadFloat4x4(&WorldFloat4x4)));

			_float4 vPos;
			memcpy(&vPos, &WorldFloat4x4.m[3], sizeof(_float4));

			if (m_eEquipState == EQUIPSTATE::GAUNTLET)
				XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * (EQUIP_OFFSET + 0.12f)));
			else if (m_eEquipStateSub == EQUIPSTATE::GAUNTLET)
				XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vDir * (SUBEQUIP_OFFSET + 0.35f)));

			XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + (vTempDir) * 0.1f);

			memcpy(&WorldFloat4x4.m[3], &vPos, sizeof(_float4));

			m_pGauntlet->Update_Matrix(BoneMatrix * XMLoadFloat4x4(&WorldFloat4x4));
		}
		else 
			m_pGauntlet->Update_Matrix(m_pTransform->Get_WorldMatrix());
	}
}

void CPlayer::IsLButton_SetFalse()
{
	if (m_bLButtonDown)
		m_bLButtonDown = false;
}

void CPlayer::IsRButton_SetFalse()
{
	if (m_bRButtonDown)
		m_bRButtonDown = false;
}

void CPlayer::LerpToHitTarget(_double TimeDelta)
{
	if (m_HitLerpTime >= 1.0)
	{
		m_HitLerpTime = 1.0;
		return;
	}

	m_HitLerpTime += TimeDelta * 4.0;

	if (m_HitLerpTime >= 1.0)
		m_HitLerpTime = 1.0;

	_vector vTargetPos = XMLoadFloat4(&m_vHitTargetPos);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPos));

	_vector vLook = XMVector3Normalize(vTargetPos - vPos);

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)), vLook, _float(m_HitLerpTime)));

	_vector vScale = m_pTransform->Get_Scale();
	_vector vRight = XMVectorZero();

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::LerpToTargetLook(_double TimeDelta)
{
	m_TargetLookLerpTime += TimeDelta * 2;

	if (m_TargetLookLerpTime >= 1.0)
		m_TargetLookLerpTime = 1.0;

	_vector vTargetPos = XMLoadFloat4(&m_vTargetPos);
	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vPos));
	_vector vDir = XMVector3Normalize(vTargetPos - vPos);

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLerpLook = XMVectorZero();

	vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vDir, (_float)m_TargetLookLerpTime));

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::LerpToLaserLook(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);

	CTransform* pCamTransform = pCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
	_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime > 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vCamLook = XMVector3Cross(vCamRight, vUp);
	vCamLook = XMVector3Normalize(vCamLook);

	vCamLook = XMVectorSetW(vCamLook, 0.f);

	XMStoreFloat3(&m_vPreLaserLook, vCamLook);

	_vector vLerpLook = XMVectorLerp(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)), XMLoadFloat3(&m_vPreLaserLook), _float(m_LookLerpTime));

	vLerpLook = XMVector3Normalize(vLerpLook);

	vRight = XMVector3Cross(vUp, vLerpLook);
	vRight = XMVector3Normalize(vRight);

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	vRight = XMVectorSetW(vRight, 0.f);
	vLerpLook = XMVectorSetW(vLerpLook, 0.f);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);

}

void CPlayer::LerpToCameraLook(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	RELEASE_INSTANCE(CGameInstance);

	CTransform* pCamTransform = pCamera->Get_Transform();
	if (nullptr == pCamTransform)
		return;

	_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
	_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime > 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	if (m_eDirection == DIRECTION::BACKWARD)
		vCamRight *= -1.f;

	vCamLook = XMVector3Cross(vCamRight, vUp);
	vCamLook = XMVector3Normalize(vCamLook);

	if (XMVector3Equal(XMLoadFloat3(&m_vPreCamLook), vCamLook))
	{
		if ((_uint)m_LookLerpTime >= 1)
		{
			m_LookLerpTime = 0.0;
			return;
		}
	}

	XMStoreFloat3(&m_vPreCamLook, vCamLook);

	_vector vLerpLook = XMVectorLerp(XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)), vCamLook, _float(m_LookLerpTime));

	vLerpLook = XMVector3Normalize(vLerpLook);

	vRight = XMVector3Cross(vUp, vLerpLook);
	vRight = XMVector3Normalize(vRight);
	
	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::Check_LerpTime(_bool& bMove)
{
	if (ANIMSTATE::LASERATTACK == m_eAnimState)
		return;

	if (false == bMove)
		m_LookLerpTime = 0.0;
}

void CPlayer::Rotate_Camera_Right(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
	_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));
	_vector vCamUp = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if (!m_bTargeting || ((_uint)CMainCamera::OPTION::CATMULLROM_3RD_TO_WHEEL != m_iCamOption && (_uint)CMainCamera::OPTION::CATMULLROM_3RD_TO_STAIR != m_iCamOption
			&& (_uint)CMainCamera::OPTION::CATMULLROM_WHEEL_TO_3RD != m_iCamOption))
		{
			pCamTransform->Rotation_Axis(TimeDelta * 0.1f, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			XMStoreFloat3(&m_vPreCamRight, vCamRight);

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vCamRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vLerpLook = XMVectorLerp(vPlayerLook, XMLoadFloat3(&m_vStartRight), _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Rotate_Camera_DiagonalLeft(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if ((!m_bTargeting || (_uint)CMainCamera::OPTION::STAIR == m_iCamOption) && (_uint)CMainCamera::OPTION::TOP != m_iCamOption && (_uint)CMainCamera::OPTION::TOPB != m_iCamOption)
		{
			_vector vDiagonalLeft = XMVectorZero();

			pCamTransform->Rotation_Axis(TimeDelta * -0.025f, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamRight *= -1.f;

			vDiagonalLeft = XMVector3Normalize(vCamLook + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalLeft, vDiagonalLeft);

			_vector vTempLeft = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLeft));
			vDiagonalLeft = XMVector3Normalize(XMVector3Cross(vTempLeft, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalLeft, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
		else if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
		{
			_vector vDiagonalLeft = XMVectorZero();

			_vector vCamUp = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamRight *= -1.f;

			vDiagonalLeft = XMVector3Normalize(vCamUp + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalLeft, vDiagonalLeft);

			_vector vTempLeft = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLeft));
			vDiagonalLeft = XMVector3Normalize(XMVector3Cross(vTempLeft, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalLeft, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vDiagonalLeft = XMLoadFloat3(&m_vStartDiagonalLeft);
		_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalLeft));
		vDiagonalLeft = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

		_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalLeft, _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Rotate_Camera_DiagonalRight(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if ((!m_bTargeting || (_uint)CMainCamera::OPTION::STAIR == m_iCamOption) && (_uint)CMainCamera::OPTION::TOP != m_iCamOption && (_uint)CMainCamera::OPTION::TOPB != m_iCamOption)
		{
			_vector vDiagonalRight = XMVectorZero();

			pCamTransform->Rotation_Axis(TimeDelta * 0.025f, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vDiagonalRight = XMVector3Normalize(vCamLook + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalRight, vDiagonalRight);

			_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalRight));
			vDiagonalRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
		else if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
		{
			_vector vDiagonalRight = XMVectorZero();

			_vector vCamUp = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vDiagonalRight = XMVector3Normalize(vCamUp + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalRight, vDiagonalRight);

			_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalRight));
			vDiagonalRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vDiagonalRight = XMLoadFloat3(&m_vStartDiagonalRight);
		_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalRight));
		vDiagonalRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

		_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalRight, _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Rotate_Camera_DiagonalBackLeft(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if ((!m_bTargeting || (_uint)CMainCamera::OPTION::STAIR == m_iCamOption) && (_uint)CMainCamera::OPTION::TOP != m_iCamOption && (_uint)CMainCamera::OPTION::TOPB != m_iCamOption)
		{
			_vector vDiagonalBackLeft = XMVectorZero();
			
			pCamTransform->Rotation_Axis(TimeDelta * -0.06, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamLook *= -1.f;
			vCamRight *= -1.f;

			vDiagonalBackLeft = XMVector3Normalize(vCamLook + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalLeft, vDiagonalBackLeft);

			_vector vTempLeft = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackLeft));
			vDiagonalBackLeft = XMVector3Normalize(XMVector3Cross(vTempLeft, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackLeft, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
		else if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
		{
			_vector vDiagonalBackLeft = XMVectorZero();

			_vector vCamUp = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamUp *= -1.f;
			vCamRight *= -1.f;

			vDiagonalBackLeft = XMVector3Normalize(vCamUp + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalLeft, vDiagonalBackLeft);

			_vector vTempLeft = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackLeft));
			vDiagonalBackLeft = XMVector3Normalize(XMVector3Cross(vTempLeft, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackLeft, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vDiagonalBackLeft = XMLoadFloat3(&m_vStartDiagonalBackLeft);
		_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackLeft));
		vDiagonalBackLeft = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

		_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackLeft, _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Rotate_Camera_DiagonalBackRight(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if (((!m_bTargeting) || (_uint)CMainCamera::OPTION::STAIR == m_iCamOption) && (_uint)CMainCamera::OPTION::TOP != m_iCamOption && (_uint)CMainCamera::OPTION::TOPB != m_iCamOption)
		{
			_vector vDiagonalBackRight = XMVectorZero();

			pCamTransform->Rotation_Axis(TimeDelta * 0.06f, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamLook *= -1.f;

			vDiagonalBackRight = XMVector3Normalize(vCamLook + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalBackRight, vDiagonalBackRight);

			_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackRight));
			vDiagonalBackRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
		else if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
		{
			_vector vDiagonalBackRight = XMVectorZero();

			_vector vCamUp = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
			_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

			vCamUp *= -1.f;

			vDiagonalBackRight = XMVector3Normalize(vCamUp + vCamRight);

			XMStoreFloat3(&m_vStartDiagonalBackRight, vDiagonalBackRight);

			_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackRight));
			vDiagonalBackRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vDiagonalBackRight = XMLoadFloat3(&m_vStartDiagonalBackRight);
		_vector vTempRight = XMVector3Normalize(XMVector3Cross(vUp, vDiagonalBackRight));
		vDiagonalBackRight = XMVector3Normalize(XMVector3Cross(vTempRight, vUp));

		_vector vLerpLook = XMVectorLerp(vPlayerLook, vDiagonalBackRight, _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::LerpToLeftofCenter(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime >= 0.5)
		m_LookLerpTime = 0.5;

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vCenterPos = XMVectorSet(WHEEL_POSITION_X, XMVectorGetY(vPos), WHEEL_POSITION_Z, 1.f);

	_vector vDir = XMVector3Normalize(vCenterPos - vPos);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));
	vRight *= -1.f;

	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vRight, _float(m_LookLerpTime)));

	_vector vScale = m_pTransform->Get_Scale();

	_vector vPlayerRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vPlayerRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vPlayerRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::LerpToRightofCenter(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime >= 0.5)
		m_LookLerpTime = 0.5;

	_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vCenterPos = XMVectorSet(WHEEL_POSITION_X, XMVectorGetY(vPos), WHEEL_POSITION_Z, 1.f);

	_vector vDir = XMVector3Normalize(vCenterPos - vPos);
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDir));

	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vRight, _float(m_LookLerpTime)));

	_vector vScale = m_pTransform->Get_Scale();

	_vector vPlayerRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vPlayerRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vPlayerRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::LerpToLeftofCamera(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vRight = XMVectorZero();
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vCamRight = XMVectorZero();
	vCamRight = XMLoadFloat3(&m_vFixCamRight);

	vCamRight *= -1.f;

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, vCamRight, _float(m_LookLerpTime)));

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::LerpToRightofCamera(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	m_LookLerpTime += TimeDelta;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;

	_vector vRight = XMVectorZero();
	_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));
	_vector vScale = m_pTransform->Get_Scale();

	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vLerpLook = XMVector3Normalize(XMVectorLerp(vLook, XMLoadFloat3(&m_vFixCamRight), _float(m_LookLerpTime)));

	vRight = XMVector3Normalize(XMVector3Cross(vUp, vLerpLook));

	vRight *= XMVectorGetX(vScale);
	vLerpLook *= XMVectorGetZ(vScale);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
}

void CPlayer::JumpMoveForTick(_double TimeDelta, _bool& bPlayAnim, _bool & bCheck)
{
	if (m_eAnimState == ANIMSTATE::JUMP || m_eAnimState == ANIMSTATE::JUMPLANDINGRUN)
	{
		_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();
		if (ANIM_JUMPLANDING != iCurAnimIndex && ANIM_JUMPLANDING_RUN != iCurAnimIndex)
		{
			if (ANIM_JUMP == iCurAnimIndex)
			//if (ANIM_JUMP == iCurAnimIndex || ANIM_JUMP_DW == iCurAnimIndex)
			{
				if (0.15 <= m_pModel->Get_PlayTimeRatio())
				{
					if (m_bSpurt)
						m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), SPURTSPEED, m_pNavigation);
					else if (m_bRun)
						m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED, m_pNavigation);
					else
						m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), WALKSPEED, m_pNavigation);

					bCheck = true;
				}
			}
			else
			{
				if (m_bSpurt)
					m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), SPURTSPEED, m_pNavigation);
				else if (m_bRun)
					m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), RUNSPEED, m_pNavigation);
				else
					m_pTransform->Go_DirWithSpeed(TimeDelta, m_pTransform->Get_State(CTransform::STATE::LOOK), WALKSPEED, m_pNavigation);

				bCheck = true;
			}
		}
		else
		{
			if (ANIM_JUMPLANDING_RUN == m_pModel->Get_CurrentAnimation())
			{
				if (m_pModel->Get_PlayTimeRatio() >= m_pModel->Get_LinkMaxRatio())
				{
					m_eAnimState = ANIMSTATE::RUN;
					m_iAnimation = ANIM_RUN;

					m_pModel->Set_NoneBlend();

					m_bJump = false;
					bPlayAnim = true;
					m_pTransform->Initialize_JumpTime();
					m_pTransform->Set_Jump(false);

					m_pGauntlet->Initialize_JumpAttCount();

					m_bGauntletJumpAttackFinal = false;
					m_bSpearJumpAttackFinal = false;

					Set_GauntletIdle();
				}
				else
				{
					m_eAnimState = ANIMSTATE::JUMPLANDINGRUN;

					m_pGauntlet->Initialize_JumpAttCount();
					m_pTransform->Initialize_JumpTime();
					m_pTransform->Set_Jump(false);
					m_bJump = false;
					bPlayAnim = true;
				}
			}
		}
	}
}

void CPlayer::Rotate_Camera_Left(_double TimeDelta)
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
	{
		if (false == m_pKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pKatana->Get_WeaponState() || WEAPONSTATE::KATANAJUMPATTACK == m_pKatana->Get_WeaponState())
			{
				if (m_pKatana->IsNonLerpTime())
					return;
			}
		}
	}
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
	{
		if (false == m_pLongKatana->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pLongKatana->Get_WeaponState() || WEAPONSTATE::LONGPOWERATTACK == m_pLongKatana->Get_WeaponState())
			{
				if (m_pLongKatana->IsPlayerNonLerp())
					return;
			}
		}
	}
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
	{
		if (false == m_pSpear->IsIndependence())
		{
			if (WEAPONSTATE::ATTACK == m_pSpear->Get_WeaponState() || WEAPONSTATE::SPEARPOWERATTACK == m_pSpear->Get_WeaponState())
			{
				if (m_pSpear->Get_PlayerNonLerp())
					return;
			}
		}
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if (m_bJump)
		m_LookLerpTime += TimeDelta * 3;
	else
		m_LookLerpTime += TimeDelta * 2;

	if (m_LookLerpTime >= 1.0)
		m_LookLerpTime = 1.0;
	
	_vector vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVectorZero();
	_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vCamLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
	_vector vCamRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

	_vector vPlayerLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if ((_uint)m_LookLerpTime >= 1)
	{
		if (!m_bTargeting || ((_uint)CMainCamera::OPTION::CATMULLROM_3RD_TO_WHEEL != m_iCamOption && (_uint)CMainCamera::OPTION::CATMULLROM_3RD_TO_STAIR != m_iCamOption
			&& (_uint)CMainCamera::OPTION::CATMULLROM_WHEEL_TO_3RD != m_iCamOption))
		{
			pCamTransform->Rotation_Axis(TimeDelta * -0.1f, XMVectorSet(0.f, 1.f, 0.f, 0.f));

			vCamRight *= -1.f;
			XMStoreFloat3(&m_vPreCamLeft, vCamRight);

			_vector vLerpLook = XMVectorLerp(vPlayerLook, vCamRight, _float(m_LookLerpTime));

			vLerpLook = XMVector3Normalize(vLerpLook);

			vRight = XMVector3Cross(vUp, vLerpLook);
			vRight = XMVector3Normalize(vRight);

			vRight *= XMVectorGetX(vScale);
			vLerpLook *= XMVectorGetZ(vScale);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
		}
	}
	else
	{
		_vector vLerpLook = XMVectorLerp(vPlayerLook, XMLoadFloat3(&m_vStartLeft), _float(m_LookLerpTime));

		vLerpLook = XMVector3Normalize(vLerpLook);

		vRight = XMVector3Cross(vUp, vLerpLook);
		vRight = XMVector3Normalize(vRight);

		vRight *= XMVectorGetX(vScale);
		vLerpLook *= XMVectorGetZ(vScale);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vLerpLook);
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_Katana(CKatana * pKatana)
{
	if (nullptr == pKatana)
		return;

	m_pKatana = pKatana;
}

void CPlayer::Set_LongKatana(CLongKatana * pLongKatana)
{
	if (nullptr == pLongKatana)
		return;

	m_pLongKatana = pLongKatana;
}

void CPlayer::Set_Spear(CSpear * pSpear)
{
	if (nullptr == pSpear)
		return;

	m_pSpear = pSpear;
}

void CPlayer::Set_Gauntlet(CGauntlet * pGauntlet)
{
	if (nullptr == pGauntlet)
		return;

	m_pGauntlet = pGauntlet;
}

void CPlayer::Set_Pod(CPod * pPod)
{
	if (nullptr == pPod)
		return;

	m_pPod = pPod;
}

void CPlayer::Set_Android9S(CAndroid_9S * pAndroid)
{
	m_pAndroid9S = pAndroid;
}

void CPlayer::Set_StartCamLeft()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));
	vRight *= -1.f;

	XMStoreFloat3(&m_vStartLeft, vRight);

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_StartCamRight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));
	XMStoreFloat3(&m_vStartRight, vRight);

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_StartCamDiagonalLeft()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
	{
		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vRight *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalLeft, XMVector3Normalize(vLook + vRight));
	}
	else
	{

		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vRight *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalLeft, XMVector3Normalize(vLook + vRight));
	}

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_StartCamDiagonalRight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
	{
		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		XMStoreFloat3(&m_vStartDiagonalRight, XMVector3Normalize(vLook + vRight));
	}
	else
	{
		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		XMStoreFloat3(&m_vStartDiagonalRight, XMVector3Normalize(vLook + vRight));
	}

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_StartCamDiagonalBackLeft()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
	{
		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vLook *= -1.f;
		vRight *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalBackLeft, XMVector3Normalize(vLook + vRight));
	}
	else
	{

		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vLook *= -1.f;
		vRight *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalBackLeft, XMVector3Normalize(vLook + vRight));
	}

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Set_StartCamDiagonalBackRight()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCamTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pCamTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	if ((_uint)CMainCamera::OPTION::TOP == m_iCamOption || (_uint)CMainCamera::OPTION::TOPB == m_iCamOption)
	{
		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::UP));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vLook *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalBackRight, XMVector3Normalize(vLook + vRight));
	}
	else
	{

		_vector vLook = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::LOOK));
		_vector vRight = XMVector3Normalize(pCamTransform->Get_State(CTransform::STATE::RIGHT));

		vLook *= -1.f;

		XMStoreFloat3(&m_vStartDiagonalBackRight, XMVector3Normalize(vLook + vRight));
	}

	m_LookLerpTime = 0.0;

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Compute_DOF_Parameters()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CMainCamera::OPTION eOption = CCameraMgr::Get_Instance()->Get_MainCam_Option();
	if (CCameraMgr::Get_Instance()->Get_CutScene_State() == OBJSTATE::DISABLE 
		&& CCameraMgr::Get_Instance()->Get_DebugCam_State() == OBJSTATE::DISABLE)
	{
		if (m_bTargeting == false && eOption != CMainCamera::OPTION::COASTER)
		{
			Calculate_Player_DOF(pGameInstance);
		}
		else if (m_bTargeting == true && eOption != CMainCamera::OPTION::COASTER)
		{
			Calculate_Target_DOF(pGameInstance);
		}
		else if (eOption == CMainCamera::OPTION::COASTER)
		{
			m_fDOFFar = 300.f;
			m_fDOFFocus = Get_CameraDistance();
			m_fDOFNear = 0.0f;
			m_vPlayerDOF.w = 0.75f;
		}
	}
	else if (CCameraMgr::Get_Instance()->Get_CutScene_State() == OBJSTATE::DISABLE 
		&& CCameraMgr::Get_Instance()->Get_DebugCam_State() == OBJSTATE::ENABLE)
	{
		m_fDOFFar = 175.f;
		m_fDOFFocus = 8.f;
		m_fDOFNear = 0.0f;
		m_vPlayerDOF.w = 0.75f;
	}
	else if (CCameraMgr::Get_Instance()->Get_CutScene_State() == OBJSTATE::ENABLE 
		&& CCameraMgr::Get_Instance()->Get_DebugCam_State() == OBJSTATE::DISABLE)
	{
		if (eOption != CMainCamera::OPTION::COASTER)
		{
			Calculate_CutScene_DOF(pGameInstance);
		}
	}

	Calculate_DepthOfField(pGameInstance, pGameInstance->Get_TimeDelta());

	m_pRenderer->Set_DOF_Params(m_vPlayerDOF);
}

DIRECTION CPlayer::Result_FixRightAndLook()
{
	_vector vCamFixRight = XMVector3Normalize(XMLoadFloat3(&m_vFixCamRight));
	_vector vMyLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK));

	if (XMVector3Equal(vMyLook, vCamFixRight))
		return DIRECTION::FORWARD;
	else
		return DIRECTION::BACKWARD;
}

void CPlayer::Check_RestTime(_double TimeDelta)
{
	//if (m_eAnimState != ANIMSTATE::ATTACK && m_eAnimState != ANIMSTATE::JUMPATTACK && m_eAnimState != ANIMSTATE::POWERATTACK && m_eAnimState != ANIMSTATE::JUMPPOWERATTACK)
	//{
	//	if (m_pKatana)
	//	{
	//		if (WEAPONSTATE::IDLE != m_pKatana->Get_WeaponState())
	//			m_pKatana->Set_WeaponState(WEAPONSTATE::IDLE);
	//	}
	//}

	//	if (WEAPONSTATE::ATTACKIDLE == m_pKatana->Get_WeaponState())
	//	{
	//		m_RestTime += TimeDelta;
	//		if (m_RestTime >= m_RestDelayTime)
	//		{
	//			m_pKatana->Set_WeaponState(WEAPONSTATE::IDLE);
	//			m_RestTime = 0.0;
	//		}
	//	}
	//}
	//else
	//	m_RestTime = 0.0;
}

void CPlayer::Check_AndroidDistance()
{
	if (nullptr != m_pAndroid9S)
	{
		CTransform* pAndroidTransform = (CTransform*)m_pAndroid9S->Get_Component(L"Com_Transform");
		if (nullptr == pAndroidTransform)
			return;

		_vector vAndroidPos = pAndroidTransform->Get_State(CTransform::STATE::POSITION);
		_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		vAndroidPos = XMVectorSetY(vAndroidPos, XMVectorGetY(vPos));

		_vector vDir = vAndroidPos - vPos;
		_vector vDist = XMVector3Length(vDir);

		if (XMVectorGetX(vDist) >= 20.f)
		{
			if (CAndroid_9S::ANDROIDSTATE::BATTLE != m_pAndroid9S->Get_AndroidState())
			{
				m_pAndroid9S->Set_Teleport(true);
				m_pAndroid9S->Set_Teleport_CurrentIndex(m_pNavigation->Get_Index());
			}
		}
	}
}

void CPlayer::Set_Target_ForPod(_vector vPos)
{
	if (nullptr != m_pPod)
	{
		vPos = XMVectorSetW(vPos, 1.f);
		m_pPod->Set_LookForTarget(vPos);
	}
}

void CPlayer::Set_BulletTime(_bool bBulletTime, _double BulletLevel, _double BulletTime)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	pGameInstance->Set_BulletTime(true);
	pGameInstance->Set_BulletLevel(BulletLevel);
	pGameInstance->Set_BulletRemaining(BulletTime);

	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Update_Pod()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix BoneMatrix = m_pModel->Get_BoneMatrix(L"bone-1");

	_matrix WorldMatrix = BoneMatrix * m_pTransform->Get_WorldMatrix();
	_float4x4 TempFloat4x4;

	XMStoreFloat4x4(&TempFloat4x4, WorldMatrix);

	_float4 vPos;
	memcpy(&vPos, &TempFloat4x4.m[3], sizeof(_float4));

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	CTransform* pCameraTransform = pCamera->Get_Transform();
	if (nullptr == pCameraTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_matrix CamMatrix = pCamera->Get_WorldMatrix();
	_float4x4 CamFloat4x4;

	XMStoreFloat4x4(&CamFloat4x4, CamMatrix);

	_float4	vCamPosFloat4;
	memcpy(&vCamPosFloat4, &CamFloat4x4.m[3], sizeof(_float4));
	_vector vCamPos = XMLoadFloat4(&vCamPosFloat4);

	vPos.y += 1.2f;

	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	vRight = XMVector3Normalize(vRight);

	XMStoreFloat4(&vPos, (XMLoadFloat4(&vPos) + vRight * 1.1f));

	if ((_uint)CMainCamera::OPTION::COASTER != m_iCamOption)
		m_pPod->Update_Position(XMLoadFloat4(&vPos));
	else if ((_uint)CMainCamera::OPTION::COASTER == m_iCamOption)
		m_pPod->Update_PositionForCoaster(XMLoadFloat4(&vPos));


	RELEASE_INSTANCE(CGameInstance);
}

void CPlayer::Update_DataFor9S(const ANDROID_DATAPACKET& tDataPacket)
{
	/*if (nullptr != m_pAndroid9S)
		m_pAndroid9S->Check_AndroidDataPacket(tDataPacket);*/

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	ANDROID_DATAPACKET tPacket = tDataPacket;

	pGameInstance->Get_Subject(L"SUBJECT_PLAYER")->Notify((void*)&PACKET(CHECKSUM_9S_ANDROID_DATAPACKET, &tPacket));
}

_vector CPlayer::Get_CameraLook()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (nullptr == pCamera)
	{
		RELEASE_INSTANCE(CGameInstance);
		return XMVectorZero();
	}

	CTransform* pTransform = (CTransform*)pCamera->Get_Transform();
	if (nullptr == pTransform)
	{
		RELEASE_INSTANCE(CGameInstance);
		return XMVectorZero();
	}

	RELEASE_INSTANCE(CGameInstance);

	return pTransform->Get_State(CTransform::STATE::LOOK);
}

void CPlayer::Set_EquipWeaponState(_uint iSlot, EQUIPSTATE eNextEquipState)
{
	if (1 == iSlot)
	{
		if (m_eEquipState == eNextEquipState)
		{
			return;
		}
		if (m_eEquipStateSub == eNextEquipState)
		{
			EQUIPSTATE	eEquipStateBuffer;

			eEquipStateBuffer = m_eEquipState;
			m_eEquipState = m_eEquipStateSub;
			m_eEquipStateSub = eEquipStateBuffer;
		}
		else
		{
			m_eEquipState = eNextEquipState;
		}
	}
	else
	{
		if (m_eEquipStateSub == eNextEquipState)
		{
			return;
		}
		if (m_eEquipState == eNextEquipState)
		{
			EQUIPSTATE	eEquipStateBuffer;

			eEquipStateBuffer = m_eEquipStateSub;
			m_eEquipStateSub = m_eEquipState;
			m_eEquipState = eEquipStateBuffer;
		}
		else
		{
			m_eEquipStateSub = eNextEquipState;
		}
	}

	Initialize_EquipState();
	// 0: Katana, 1: LongKatana, 2: Spear, 3: Gauntlet
	switch ((_uint)m_eEquipState)
	{
	case 0:
		m_pKatana->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 1:
		m_pLongKatana->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 2:
		m_pSpear->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 3:
		m_pGauntlet->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	default:
		break;
	}

	switch ((_uint)m_eEquipStateSub)
	{
	case 0:
		m_pKatana->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 1:
		m_pLongKatana->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 2:
		m_pSpear->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	case 3:
		m_pGauntlet->Set_WeaponEquip(WEAPONEQUIP::EQUIP);
		break;
	default:
		break;
	}

	Set_KatanaIdle();
	Set_LongKatanaIdle();
	Set_SpearIdle();
	Set_GauntletIdle();

	Check_WeaponState();
}

void CPlayer::Set_KatanaIdle()
{
	m_pKatana->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pKatana->Set_Immediately_Animation(0);
	m_pKatana->Set_Independence(false);
	m_pKatana->Initialize_WaitTime();
}

void CPlayer::Set_LongKatanaIdle()
{
	m_pLongKatana->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pLongKatana->Set_Animation(0);
	m_pLongKatana->Set_Independence(false);
	m_pLongKatana->Set_PlayerNonLerp(false);
}

_bool CPlayer::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
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
	else if (iColliderType == (_uint)CCollider::TYPE::RAY)
	{
		_float fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		if (m_pHitBox->Collision_AABB(pEffect->Get_RayOrigin(), pEffect->Get_RayDirection(), fDist))
		{
			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT0)
	{
		// 10 ~ 11
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);
		
		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 12.f || fDist < 9.f)
				return false; 

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT1)
	{
		// 15 ~ 16.5
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);

		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 17.25f || fDist < 14.25f)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::DONUT2)
	{
		// 20 ~ 22
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vDonutOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vDonutOrigin));
		_vector vDir = XMVector4Normalize(vPosition - vDonutOrigin);

		if (m_pHitBox->Collision_AABB(vDonutOrigin, vDir, fDist))
		{
			if (fDist > 23.f || fDist < 19.f)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::BEAUVOIR_RING)
	{
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vRingOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vRingOrigin));

		_vector vScale = pEffect->Get_RingScale(CTransform::STATE::RIGHT);
		_float fScale = XMVectorGetX(XMVector3Length(vScale));

		_vector vDir = XMVector4Normalize(vPosition - vRingOrigin);
		if (m_pHitBox->Collision_AABB(vRingOrigin, vDir, fDist))
		{
			if (fDist > 44.f * fScale || fDist < 36.f * fScale)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	else if (iColliderType == (_uint)CCollider::TYPE::ZHUANGZI_RING)
	{
		float	fDist = 0;
		CMeshEffect* pEffect = dynamic_cast<CMeshEffect*>(pGameObject);
		if (nullptr == pEffect)
			return false;

		_vector vRingOrigin = pEffect->Get_RayOrigin();
		_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vRingOrigin));

		_vector vScale = pEffect->Get_RingScale(CTransform::STATE::RIGHT);
		_float fScale = XMVectorGetX(XMVector3Length(vScale));

		_vector vDir = XMVector4Normalize(vPosition - vRingOrigin);
		if (m_pHitBox->Collision_AABB(vRingOrigin, vDir, fDist))
		{

			if (fDist > 0.61f * fScale || fDist < 0.59f * fScale)
				return false;

			m_iFlagCollision |= FLAG_COLLISION_RAY;
			return true;
		}
	}
	return false;
}

void CPlayer::Collision(CGameObject * pGameObject)
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	

	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		if (m_eAnimState != CPlayer::ANIMSTATE::JUSTAVOID)
			m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	if (m_iFlagCollision & FLAG_COLLISION_ATTACKBOX)
	{
		if (ANIM_HIT_FRONT != iCurAnimIndex)
		{
			if (m_UnbeatableTime >= m_UnbeatableDelayTime)
			{
				CTransform* pTransform = (CTransform*)pGameObject->Get_Component(COM_KEY_TRANSFORM);
				if (nullptr == pTransform)
					return;

				_vector vCounterPos = pTransform->Get_State(CTransform::STATE::POSITION);
				XMStoreFloat4(&m_vCounterAttPos, vCounterPos);

				if (m_eAnimState != CPlayer::ANIMSTATE::AVOID && m_eAnimState != CPlayer::ANIMSTATE::HIT && m_eAnimState != CPlayer::ANIMSTATE::JUSTAVOID && m_eAnimState != CPlayer::ANIMSTATE::JUSTAVOID_COUNTERATTACK)
				{
					CCollider* pCollider = (CCollider*)pGameObject->Get_Component(COM_KEY_ATTACKBOX);
					if (nullptr == pCollider)
						return;

					_uint iDamage = 0;
					m_bBulletTime = false;

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

							XMStoreFloat4(&m_vHitTargetPos, vTargetPos);

							_bool bCheck = false;
							if (EQUIPSTATE::KATANA == m_eEquipState)
							{
								CModel* pKatanaModel = (CModel*)m_pKatana->Get_Component(COM_KEY_MODEL);
								if (nullptr == pKatanaModel)
									return;

								_uint iKatanaIndex = pKatanaModel->Get_CurrentAnimation();

								if (KATANA_POWERATTACK1 == iKatanaIndex || KATANA_POWERATTACK2 == iKatanaIndex || KATANA_JUMPPOWERATTACK == iKatanaIndex
									|| KATANA_JUMPPOWERATTACK_DW == iKatanaIndex || KATANA_JUMPPOWERATTACK_END == iKatanaIndex)
									bCheck = true;
							}
							else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
							{
								WEAPONSTATE eWeaponState = m_pLongKatana->Get_WeaponState();

								CModel* pLongKatanaModel = (CModel*)m_pLongKatana->Get_Component(COM_KEY_MODEL);
								if (nullptr == pLongKatanaModel)
									return;

								_uint iLKatanaIndex = pLongKatanaModel->Get_CurrentAnimation();

								if (WEAPONSTATE::LONGPOWERATTACK == eWeaponState || 23 == iLKatanaIndex || 25 == iLKatanaIndex
									|| 31 == iLKatanaIndex || 35 == iLKatanaIndex || 42 == iLKatanaIndex)
									bCheck = true;
							}
							else if (EQUIPSTATE::SPEAR == m_eEquipState)
							{
								WEAPONSTATE	eWeaponState = m_pSpear->Get_WeaponState();
								if (WEAPONSTATE::SPEARPOWERATTACK == eWeaponState)
									bCheck = true;
							}
							else if (EQUIPSTATE::GAUNTLET == m_eEquipState)
							{
								WEAPONSTATE eWeaponState = m_pGauntlet->Get_WeaponState();
								if (WEAPONSTATE::GAUNTLETPOWERATTACK == eWeaponState)
									bCheck = true;
							}

							if (!m_bLaserAttack && !bCheck)
							{
								if (m_iAnimation != ANIM_HIT_FRONT)
								{
									CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER);
									int a = rand() % 2;
									if (a == 0)
										CSoundMgr::Get_Instance()->PlaySound(L"Player_Hit1.wav", CSoundMgr::PLAYER);
									else
										CSoundMgr::Get_Instance()->PlaySound(L"Player_Hit2.wav", CSoundMgr::PLAYER);
								}

								m_iAnimation = ANIM_HIT_FRONT;
								m_eAnimState = ANIMSTATE::HIT;
								m_bContinue = false;
								m_bPressingR = false;
								m_bKatanaR = false;
								m_bThrowSpear = false;
								m_bLKatanaRush = false;
								m_bGauntletSpin = false;

								m_HitLerpTime = 0.0;
							}
							else
								m_UnbeatableTime = 0.0;

							if (!bCheck)
							{
								Set_EquipWeaponIdle();
								m_pKatana->End_Trail();
								m_pLongKatana->End_Trail();
							}

							if (m_pTransform->IsJump())
								m_pTransform->Set_FallTime();
						}
					}
				}
				else if (ANIMSTATE::AVOID == m_eAnimState)
				{
					if (HIT::NONE != pGameObject->Get_HitType())
					{
						CTransform* pTransform = (CTransform*)pGameObject->Get_Component(L"Com_Transform");
						if (nullptr == pTransform)
							return;

						m_HitLerpTime = 0.0;

						_vector vTargetPos = pTransform->Get_State(CTransform::STATE::POSITION);
						Set_CurrentMatrix();
						m_pKatana->Set_CurrentMatrix();
						m_pLongKatana->Set_CurrentMatrix();
						m_pGauntlet->Set_CurrentMatrix();
						m_pSpear->Set_CurrentMatrix();
						XMStoreFloat4(&m_vHitTargetPos, vTargetPos);
						m_eAnimState = ANIMSTATE::JUSTAVOID;
					}
				}
			}
		}
	}

	if (m_iFlagCollision & FLAG_COLLISION_RAY)
	{
		if (ANIM_HIT_FRONT != iCurAnimIndex)
		{
			if (m_UnbeatableTime >= m_UnbeatableDelayTime)
			{
				if (m_eAnimState != CPlayer::ANIMSTATE::AVOID && m_eAnimState != CPlayer::ANIMSTATE::HIT && m_eAnimState != CPlayer::ANIMSTATE::JUSTAVOID && m_eAnimState != CPlayer::ANIMSTATE::JUSTAVOID_COUNTERATTACK)
				{
					m_iFlagUI |= FLAG_UI_CALL_HPBAR;
					m_iFlagUI |= FLAG_UI_CALL_DAMAGE;

					m_dCallHPBarUITimeDuration = 3.0;
					m_tObjectStatusDesc.fHp -= pGameObject->Get_RandomAtt();

					if (m_tObjectStatusDesc.fHp < 0)
						m_tObjectStatusDesc.fHp = 0;

					if (!m_bLaserAttack)
					{
						if (m_iAnimation != ANIM_HIT_FRONT)
						{
							CSoundMgr::Get_Instance()->StopSound(CSoundMgr::PLAYER);
							int a = rand() % 2;
							if ( a == 0)
								CSoundMgr::Get_Instance()->PlaySound(L"Player_Hit1.wav", CSoundMgr::PLAYER);
							else 
								CSoundMgr::Get_Instance()->PlaySound(L"Player_Hit2.wav", CSoundMgr::PLAYER);
						}
						m_iAnimation = ANIM_HIT_FRONT;
						m_eAnimState = ANIMSTATE::HIT;
						m_bContinue = false;
						m_bPressingR = false;
						m_bKatanaR = false;
						m_bThrowSpear = false;
						m_bLKatanaRush = false;
						m_bGauntletSpin = false;
					}
					else
						m_UnbeatableTime = 0.0;

					Set_EquipWeaponIdle();
				}
				else if (ANIMSTATE::AVOID == m_eAnimState)
				{
					CTransform* pTransform = (CTransform*)pGameObject->Get_Component(L"Com_Transform");
					if (nullptr == pTransform)
						return;

					m_HitLerpTime = 0.0;

					_vector vTargetPos = pTransform->Get_State(CTransform::STATE::POSITION);

					XMStoreFloat4(&m_vHitTargetPos, vTargetPos);
					Set_CurrentMatrix();
					m_pKatana->Set_CurrentMatrix();
					m_pLongKatana->Set_CurrentMatrix();
					m_pGauntlet->Set_CurrentMatrix();
					m_pSpear->Set_CurrentMatrix();

					m_eAnimState = ANIMSTATE::JUSTAVOID;
				}
			}
		}
	}

	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CPlayer::Set_SpearIdle()
{
	m_pSpear->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pSpear->Set_Immediately_Animation(1);
	m_pSpear->Set_Independence(false);
	m_pSpear->Set_PlayerNonLerp(false);
}

void CPlayer::Set_GauntletIdle()
{
	m_pGauntlet->Set_WeaponState(WEAPONSTATE::IDLE);
	m_pGauntlet->Set_NoneBlend(1);
}

void CPlayer::Set_EquipWeaponIdle()
{
	if (EQUIPSTATE::KATANA == m_eEquipState)
		Set_KatanaIdle();
	else if (EQUIPSTATE::LONGKATANA == m_eEquipState)
		Set_LongKatanaIdle();
	else if (EQUIPSTATE::SPEAR == m_eEquipState)
		Set_SpearIdle();
	else if (EQUIPSTATE::GAUNTLET == m_eEquipState)
		Set_GauntletIdle();

	m_bRButtonDown = false;
	m_bLButtonDown = false;

	m_pTransform->Initialize_AttCount();
	m_pTransform->Set_PowerAtt(false);
	m_bGauntletJumpAttackFinal = false;
	m_bSpearJumpAttackFinal = false;
}

void CPlayer::Check_JumpState_ForHit(_double TimeDelta)
{
	if (m_bJump || m_pTransform->IsJump())
	{
		if (PODSTATE::IDLE != m_pPod->Get_PodState())
			m_pPod->Set_PodState(PODSTATE::IDLE);

		m_pTransform->Jump(TimeDelta, m_pNavigation);

		_float fY = 0.f;
		if (m_pTransform->IsLowThanCell(m_pNavigation, &fY))
		{
			_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
			vPos = XMVectorSetY(vPos, fY);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

			m_pGauntlet->Initialize_JumpAttCount();

			m_pTransform->Initialize_JumpTime();
			m_pTransform->Set_Jump(false);
			m_bJump = false;

			m_bHangOn = false;
			m_SpacePressingTime = 0.0;
		}
	}
}

void CPlayer::Check_AvoidAnimation()
{
	_uint iCurAnimIndex = m_pModel->Get_CurrentAnimation();

	if (ANIM_FRONT_AVOID == m_iAnimation || ANIM_FRONT_AVOID_SLIDING == iCurAnimIndex)
		m_iAnimation = ANIM_JUSTAVOID_BACK;

	else if (ANIM_BACK_AVOID == m_iAnimation || ANIM_BACK_AVOID_SLIDING == iCurAnimIndex)
		m_iAnimation = ANIM_JUSTAVOID_BACK;

	else if (ANIM_LEFT_AVOID == m_iAnimation || ANIM_LEFT_AVOID_SLIDING == iCurAnimIndex)
		m_iAnimation = ANIM_JUSTAVOID_LEFT;

	else if (ANIM_RIGHT_AVOID == m_iAnimation || ANIM_RIGHT_AVOID_SLIDING == iCurAnimIndex)
		m_iAnimation = ANIM_JUSTAVOID_RIGHT;

	if (m_iAnimation == iCurAnimIndex)
	{
		m_bCollision = false;
		m_bDodge = true;

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		if (false == m_bBulletTime)
		{
			m_bBulletTime = true;
			Set_BulletTime(true, 2.0, 1.0);
		}

		RELEASE_INSTANCE(CGameInstance);

		if (m_pModel->Get_LinkMaxRatio() <= m_pModel->Get_PlayTimeRatio())
		{
			m_iAnimation = ANIM_IDLE;
			m_eAnimState = ANIMSTATE::IDLE;
			m_bContinue = true;

			m_bBulletTime = false;
			m_bDodge = false;
			m_bCollision = true;
		}
		else if (m_pModel->Get_LinkMinRatio() <= m_pModel->Get_PlayTimeRatio())
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (pGameInstance->Mouse_Pressing(CInputDevice::DIMB::DIMB_LB))
			{
				m_eAnimState = ANIMSTATE::JUSTAVOID_COUNTERATTACK;
				m_iAnimation = ANIM_JUSTAVOID_COUNTERATTACK;

				m_bContinue = false;
				m_bDodge = false;
				m_bCollision = true;
			}

			RELEASE_INSTANCE(CGameInstance);
		}
	}
}

void CPlayer::Initialize_HangOn()
{
	m_SpacePressingTime = 0.0;
	m_bHangOn = false;

	m_pPod->Set_PodState(PODSTATE::IDLE);
}

void CPlayer::ForCoaster()
{
	Check_Target();
	Check_WeaponState();
	Update_Pod();
	Check_AndroidDistance();
	SetUp_Bone1();
	Update_Collider();
}

void CPlayer::Set_PositionForCoaster(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CCoaster* pCoaster = (CCoaster*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", 0);
	if (nullptr == pCoaster)
		return;

	CTransform* pCoasterTransform = (CTransform*)pCoaster->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pCoasterTransform)
		return;

	CNavigation* pCoasterNavigation = (CNavigation*)pCoaster->Get_Component(COM_KEY_NAVIGATION);
	if (nullptr == pCoasterNavigation)
		return;

	pCoaster->Update_Navigation(TimeDelta);

	//m_pTransform->Set_State(CTransform::STATE::POSITION, pCoasterTransform->Get_State(CTransform::STATE::POSITION));
	//_vector vPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
}

void CPlayer::Update_UnbeatableTime(_double TimeDelta)
{
	m_UnbeatableTime += TimeDelta;
}

void CPlayer::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		PACKET*	pPacket = (PACKET*)pMessage;

		CPlayer::INPUTSTATE	eInputState = *(CPlayer::INPUTSTATE*)pPacket->pData;

		m_ePrevInputState = eInputState;
	}
}

_int CPlayer::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_CONTROLLER_REQUEST)
		{
			return TRUE;
		}
	}
	return FALSE;
}

HRESULT CPlayer::SetUp_Observer()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(pGameInstance->Create_Observer(TEXT("OBSERVER_PLAYER"), this)))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	CSubject*	pPlayerSubject = nullptr;

	pPlayerSubject = pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"));

	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_PLAYER")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_HPBAR_PLAYER_FRAME")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_TARGET")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_BACKGROUND")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CURSOR")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_SYSMSG")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_ENGAGE")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE")))))
		return E_FAIL;	
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_STATUS")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_WEAPONSLOT")))))
		return E_FAIL;
	if (FAILED(pPlayerSubject->Add_Observer(pGameInstance->Get_Observer(TEXT("OBSERVER_CORE_CONTROLLER")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::Update_UI(_double dTimeDelta)
{
	if (CPlayer::INPUTSTATE::UI == m_ePrevInputState)
	{
		CUI_Core_Status::UISTATUSDESC		tDesc;
		CUI_Core_WeaponSlot::WEAPONSLOTDESC	tWeaponDesc;

		tDesc.iAmountHp = (_uint)m_tObjectStatusDesc.fHp;
		tDesc.iAmountMaxHp = (_uint)m_tObjectStatusDesc.fMaxHp;
		tDesc.iAmountMoney = m_pInventory->Get_CurrentMoney();
		tDesc.iAmountAttShoot = m_pPod->Get_ObjectStatusDesc().iAtt;
		tDesc.iAmountDef = (_uint)m_tObjectStatusDesc.iDef;

		tWeaponDesc.iMainWeapon = (_uint)m_eEquipState;
		tWeaponDesc.iSubWeapon = (_uint)m_eEquipStateSub;

		switch (m_eEquipState)
		{
		case Client::CPlayer::EQUIPSTATE::KATANA:
			tDesc.iAmountAtt = m_pKatana->Get_Att();
			break;
		case Client::CPlayer::EQUIPSTATE::LONGKATANA:
			tDesc.iAmountAtt = m_pLongKatana->Get_Att();
			break;
		case Client::CPlayer::EQUIPSTATE::SPEAR:
			tDesc.iAmountAtt = m_pSpear->Get_Att();
			break;
		case Client::CPlayer::EQUIPSTATE::GAUNTLET:
			tDesc.iAmountAtt = m_pGauntlet->Get_Att();
			break;
		case Client::CPlayer::EQUIPSTATE::NONE:
			break;
		default:
			break;
		}
		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_STATUS_UPDATE, &tDesc));
		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_CORE_WEAPONSLOT_UPDATE, &tWeaponDesc));
	}

	if (m_iFlagUI & FLAG_UI_CALL_HPBAR)
	{
		m_dCallHPBarUITimeDuration -= dTimeDelta;

		CUI_HUD_Hpbar_Player_Contents::UIHPBARPLAYERDESC	desc;

		desc.iObjID = m_iObjID;
		desc.ePlayerState = CUI_HUD_Hpbar_Player_Contents::PLAYERSTATE::IDLE;
		desc.fCrntHPUV = (_float)m_tObjectStatusDesc.fHp / (_float)m_tObjectStatusDesc.fMaxHp;
		desc.dTimeDuration = m_dCallHPBarUITimeDuration;

		CUI_HUD_Hpbar_Player_Frame::UIHPBARPLAYERFRAMEDESC	desc2;

		desc2.iObjID = m_iObjID;
		desc2.ePlayerState = CUI_HUD_Hpbar_Player_Frame::PLAYERSTATE::IDLE;
		desc2.dTimeDuration = m_dCallHPBarUITimeDuration;

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_PLAYER_HPBAR, &desc));
		pGameInstance->Get_Subject(TEXT("SUBJECT_PLAYER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_PLAYER_HPBAR_FRAME, &desc2));

		RELEASE_INSTANCE(CGameInstance);

		if (m_dCallHPBarUITimeDuration < 0) 
		{
			m_iFlagUI ^= FLAG_UI_CALL_HPBAR;
		}
	}
	if (m_iFlagUI & FLAG_UI_CALL_DAMAGE)
	{
		CUI_HUD_Damage::UIDAMAGEDESC	desc;

		desc.eType = CUI_HUD_Damage::DAMAGETYPE::PLAYER;
		desc.iDamageAmount = m_iGetDamage;
		XMStoreFloat4(&desc.vWorldPos, m_pMiddlePoint->Get_WorldPos(CCollider::TYPE::SPHERE));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_DAMAGE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DAMAGE, &desc));

		RELEASE_INSTANCE(CGameInstance);

		m_iFlagUI ^= FLAG_UI_CALL_DAMAGE;
	}
	if (m_iFlagUI & FLAG_UI_CALL_TARGET)
	{
		CUI_HUD_Target::UITARGETDESC	desc;

		if (nullptr != m_pTarget)
		{
			desc.iObjID = 0;
			desc.eTargetType = CUI_HUD_Target::TARGETTYPE::MONSTER;
			desc.dTimeDuration = m_dCallTargetUITimeDuration;

			XMStoreFloat4(&desc.vWorldPos, dynamic_cast<CCollider*>(m_pTarget->Get_Component(COM_KEY_MIDDLEPOINT))->Get_WorldPos(CCollider::TYPE::SPHERE));

			m_vTargetPos = desc.vWorldPos;
			m_vTargetPos.w = 1.f;

			Set_Target_ForPod(XMLoadFloat4(&desc.vWorldPos));

			CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Get_Observer(TEXT("OBSERVER_TARGET"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_TARGET, &desc));

			RELEASE_INSTANCE(CGameInstance);
		}
		else
		{
			desc.iObjID = 1;
			desc.eTargetType = CUI_HUD_Target::TARGETTYPE::MONSTER;
			desc.dTimeDuration = 0.0;

			XMStoreFloat4(&desc.vWorldPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));

			CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Get_Observer(TEXT("OBSERVER_TARGET"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_TARGET, &desc));

			RELEASE_INSTANCE(CGameInstance);
		}
	}
	else
	{
		CUI_HUD_Target::UITARGETDESC	desc;

		desc.iObjID = 1;
		desc.eTargetType = CUI_HUD_Target::TARGETTYPE::MONSTER;
		desc.dTimeDuration = 0.0;

		XMStoreFloat4(&desc.vWorldPos, XMVectorSet(0.f, 0.f, 0.f, 1.f));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Get_Observer(TEXT("OBSERVER_TARGET"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_TARGET, &desc));

		RELEASE_INSTANCE(CGameInstance);
	}
	return S_OK;
}

void CPlayer::Update_Effects(_float _fTimeDelta)
{
	Update_Weapon_Decoration_Effects(static_cast<_double>(_fTimeDelta));
	//Update_PlayerSRV();
	//if (m_bLongKatanaTrail_Create)
	//{
	//	m_fLongKatanaTrail_Dealy += _fTimeDelta;
	//	if (m_fLongKatanaTrail_Dealy > PLAYER_LONGKATANA_TRAIL_DEALY)
	//	{
	//		m_fLongKatanaTrail_Dealy = 0.f;
	//		m_bLongKatanaTrail_Create = false;
	//	}
	//}

	if (m_pChargingEffect)
	{
		CTransform* pTransform = dynamic_cast<CTransform*>(m_pChargingEffect->Get_Component(L"Com_Transform"));
		pTransform->Set_State(CTransform::STATE::POSITION, m_pTransform->Get_State(CTransform::STATE::POSITION));
	}

	
	// Dissolve Up, Go back to Player's back, Dissolve Down
	//if (m_pKatana->Get_DissolveState() == DISSOLVE_STATE::DISSOLVE_UP)
	//if (m_pKatana->Get_DissolveState() == DISSOLVE_STATE::DISSOLVE_DOWN)
	//{
	//	if (WEAPONSTATE::IDLE == m_pKatana->Get_WeaponState())
	//	{
	//		//m_pKatana->Set_DissolveIdle();
	//		Set_KatanaIdle();
	//	}
	//}
	//if (m_pLongKatana->Get_DissolveState() == DISSOLVE_STATE::DISSOLVE_DOWN)
	////if (m_pLongKatana->Get_DissolveState() != DISSOLVE_STATE::DISSOLVE_IDLE)
	//{
	//	if (WEAPONSTATE::IDLE != m_pLongKatana->Get_WeaponState())
	//	{
	//		//m_pLongKatana->Set_DissolveIdle();
	//		Set_LongKatanaIdle();
	//	}
	//}




	//if (ANIMSTATE::SPURT == m_eAnimState
	//	|| ANIMSTATE::AVOID == m_eAnimState
	//	)
	//{
	//	//Create_SpurtAfterImage();
	//	m_fAfterImageGenTimes += _fTimeDelta;
	//	if (m_fAfterImageGenTimes > 0.03f)
	//	{
	//		m_fAfterImageGenTimes = 0.f;
	//		Create_SpurtAfterImage();
	//	}
	//}
}

void CPlayer::Update_Weapon_Decoration_Effects(_double _dTimeDelta)
{
	if (WEAPONSTATE::IDLE == m_pKatana->Get_WeaponState())
	{
		if (!m_pKatana->Get_IfEffectIs())
		{
			m_dKatanaEffectCreateDelay += _dTimeDelta;
			if (m_dKatanaEffectCreateDelay > WEAPON_EFFECT_CREATE_DELAY)
			{
				m_dKatanaEffectCreateDelay = 0.0;
				m_pKatana->Create_Decoration_Effect();
			}
		}
	}
	else
	{
		if (m_pKatana->Get_IfEffectIs())
			m_pKatana->Delete_Decoration_Effect();
	}


	if (WEAPONSTATE::IDLE == m_pLongKatana->Get_WeaponState())
	{
		if (!m_pLongKatana->Get_IfEffectIs())
		{
			m_dLongKatanaEffectCreateDelay += _dTimeDelta;
			if (m_dLongKatanaEffectCreateDelay > WEAPON_EFFECT_CREATE_DELAY)
			{
				m_dLongKatanaEffectCreateDelay = 0.0;
				m_pLongKatana->Create_Decoration_Effect();
			}
		}
	}
	else
	{
		if (m_pLongKatana->Get_IfEffectIs())
			m_pLongKatana->Delete_Decoration_Effect();
	}

	if (WEAPONSTATE::IDLE == m_pSpear->Get_WeaponState())
	{
		if (!m_pSpear->Get_IfEffectIs())
		{
			m_dSpearEffectCreateDelay += _dTimeDelta;
			if (m_dSpearEffectCreateDelay > WEAPON_EFFECT_CREATE_DELAY)
			{
				m_dSpearEffectCreateDelay = 0.0;
				m_pSpear->Create_Decoration_Effect();
			}
		}
	}
	else
	{
		if (m_pSpear->Get_IfEffectIs())
			m_pSpear->Delete_Decoration_Effect();
	}
}

void CPlayer::Update_PlayerSRV()
{
	if (m_pPlayerSRV)
	{
		Safe_Release(m_pPlayerSRV);
		m_pPlayerSRV = nullptr;
	}

	m_pPlayerSRV = CGameInstance::Get_Instance()->Get_SRV(L"RTV_Player");
	if (!m_pPlayerSRV)
		assert(false);
	Safe_AddRef(m_pPlayerSRV);
}

void CPlayer::Create_SpurtAfterImage()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_vector vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	//CGameObject* pCam = pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	//_float4 vCameraGoalPos = dynamic_cast<CMainCamera*>(pCam)->Get_GoalPosition();
	//_float4 vCameraLastPos = dynamic_cast<CMainCamera*>(pCam)->Get_LastPosition();

	//_vector vecCameraGoalPos = XMLoadFloat4(&vCameraGoalPos);
	//_vector vecCameraLastPos = XMLoadFloat4(&vCameraLastPos);
	//_vector vecCameraSub = vecCameraLastPos - vecCameraGoalPos;
	////_vector vecCameraSub = vecCameraGoalPos - vecCameraLastPos;
	//vecCameraSub = XMVectorSetW(vecCameraSub, 1.f);
	//vecCameraSub = XMVectorSetY(vecCameraSub, 0.f);

	//vecPos += vecCameraSub;

	CAfterImage::AFTERIMAGE_DESC tAfterImageDesc;
	tAfterImageDesc.fMaxLifeTime = 0.7f;
	tAfterImageDesc.pSRV = m_pPlayerSRV;
	tAfterImageDesc.tEffectMovement.fSpeed = 0.f;
	tAfterImageDesc.tEffectMovement.vDirection = { 0.f, 0.f, 0.f };
	XMStoreFloat4(&tAfterImageDesc.tEffectMovement.vPosition, vecPos);
	pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_AfterImage"), TEXT("Layer_Effect"), &tAfterImageDesc);
}

void CPlayer::Create_FlipAfterImage()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_vector vecPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

	CAfterImage::AFTERIMAGE_DESC tAfterImageDesc;
	tAfterImageDesc.fMaxLifeTime = 0.7f;
	tAfterImageDesc.pSRV = m_pPlayerSRV;
	tAfterImageDesc.tEffectMovement.fSpeed = 2.f;
	tAfterImageDesc.tEffectMovement.vDirection = { 1.f, 0.f, 0.f };
	XMStoreFloat4(&tAfterImageDesc.tEffectMovement.vPosition, vecPos);
	pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_AfterImage"), TEXT("Layer_Effect"), &tAfterImageDesc);

	tAfterImageDesc.fMaxLifeTime = 0.7f;
	tAfterImageDesc.pSRV = m_pPlayerSRV;
	tAfterImageDesc.tEffectMovement.fSpeed = 2.f;
	tAfterImageDesc.tEffectMovement.vDirection = { -1.f, 0.f, 0.f };
	XMStoreFloat4(&tAfterImageDesc.tEffectMovement.vPosition, vecPos);
	pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_AfterImage"), TEXT("Layer_Effect"), &tAfterImageDesc);

	tAfterImageDesc.fMaxLifeTime = 0.7f;
	tAfterImageDesc.pSRV = m_pPlayerSRV;
	tAfterImageDesc.tEffectMovement.fSpeed = 2.f;
	tAfterImageDesc.tEffectMovement.vDirection = { 0.f, 0.f, 1.f };
	XMStoreFloat4(&tAfterImageDesc.tEffectMovement.vPosition, vecPos);
	pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_AfterImage"), TEXT("Layer_Effect"), &tAfterImageDesc);

	tAfterImageDesc.fMaxLifeTime = 0.7f;
	tAfterImageDesc.pSRV = m_pPlayerSRV;
	tAfterImageDesc.tEffectMovement.fSpeed = 2.f;
	tAfterImageDesc.tEffectMovement.vDirection = { 0.f, 0.f, -1.f };
	XMStoreFloat4(&tAfterImageDesc.tEffectMovement.vPosition, vecPos);
	pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelID(), TEXT("Prototype_GameObject_AfterImage"), TEXT("Layer_Effect"), &tAfterImageDesc);

}

void CPlayer::Update_Alpha(_double dTimeDelta)
{
	if (m_eAnimState == ANIMSTATE::JUSTAVOID)
	{
		_float fTimeDelta = (_float)dTimeDelta;
		m_fAlpha -= fTimeDelta * 3.f;
		m_pRenderer->Set_PostProcessing_Dodge();

		fTimeDelta /= 12.f;
		m_pRenderer->Set_UVDistance(fTimeDelta);

		m_pKatana->Set_Alpha(m_fAlpha);
		m_pSpear->Set_Alpha(m_fAlpha);
		m_pLongKatana->Set_Alpha(m_fAlpha);
		m_pGauntlet->Set_Alpha(m_fAlpha);

		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::RIMDODGE, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::RIMDODGE, m_pGauntlet);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::RIMDODGE, m_pLongKatana);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::RIMDODGE, m_pSpear);

		if (m_fAlpha <= 0.15f)
			m_fAlpha = 0.15f;
	}
	else
	{
		m_fAlpha += (_float)dTimeDelta * 1.5f;
		m_pRenderer->ReSet_UVDistance();

		m_pKatana->Set_Alpha(m_fAlpha);
		m_pSpear->Set_Alpha(m_fAlpha);
		m_pLongKatana->Set_Alpha(m_fAlpha);
		m_pGauntlet->Set_Alpha(m_fAlpha);

		if (m_fAlpha >= 1.f)
			m_fAlpha = 1.f;
	}
}

const _bool CPlayer::Switch_EquipState()
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	Set_EquipWeaponState(1, m_eEquipStateSub);
		
	pGameInstance->Get_Observer(TEXT("OBSERVER_QUICKSLOT"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_QUICKSLOT, &m_eEquipState));

	return true;
}

const _bool CPlayer::Is_ProgramChipInstalled(_uint iChipFlag)
{
	return (m_iFlagProgramChipState & iChipFlag) ? true : false;
}

const _bool CPlayer::Is_ValidProgramChip(_uint iChipFlag)
{
	_bool	bResult = false;

	if (PROGRAM_OS == iChipFlag)
		bResult = true;
	else if (PROGRAM_MACHINEGUN == iChipFlag)
		bResult = true;
	else if (PROGRAM_HANG == iChipFlag)
		bResult = true;
	else if (PROGRAM_LASER == iChipFlag)
		bResult = true;
	else if (PROGRAM_LIGHT == iChipFlag)
		bResult = true;

	if (false == bResult)
	{
		MSGBOX("Unvalid ProgramChip");
	}
	return bResult;
}

void CPlayer::Install_ProgramChip(_uint iChipFlag)
{
	if (false == Is_ValidProgramChip(iChipFlag))
	{
		return;
	}
	if (false == Is_ProgramChipInstalled(iChipFlag))
	{
		m_iFlagProgramChipState |= iChipFlag;
	}
}

void CPlayer::Uninstall_ProgramChip(_uint iChipFlag)
{
	if (false == Is_ValidProgramChip(iChipFlag))
	{
		return;
	}
	if (true == Is_ProgramChipInstalled(iChipFlag))
	{
		m_iFlagProgramChipState ^= iChipFlag;
	}
}

HRESULT CPlayer::SceneChange_AmusementPark()
{
	
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, AMUSEMENT_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	m_pRenderer->Set_Fogging(1);

	m_pRenderer->Set_Fog(100.f, 190.f, CProportionConverter()(_DARKSALMON, 93.f, true));

	m_pRenderer->Set_DOF_Distance(100.f);
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(CGameInstance::Get_Instance()->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera"));
	if (nullptr != pCamera)
	{
		_float4 vecPos;
		//XMStoreFloat4(&vecPos, AMUSEMENT_COASTER_ENDPOSITION);
		XMStoreFloat4(&vecPos, AMUSEMENT_START_POSITION);
		pCamera->Set_TeleportPosition(vecPos);
		pCamera->Set_Option(CMainCamera::OPTION::TELEPORT);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_AmusementPark()))
			return E_FAIL;
	}
	
	if (FAILED(m_pNavigation->Init_Index(AMUSEMENT_START_POSITION)))
		return E_FAIL;

	if (FAILED(m_pAndroid9S->SceneChange_AmusementPark(AMUSEMENT_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pKatana->SceneChange_AmusementPark(AMUSEMENT_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pLongKatana->SceneChange_AmusementPark(AMUSEMENT_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pGauntlet->SceneChange_AmusementPark(AMUSEMENT_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pSpear->SceneChange_AmusementPark(AMUSEMENT_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	m_pPod->Set_MaxCoolTime();

	return S_OK;
}

HRESULT CPlayer::SceneChange_OperaBackStage()
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, OPERA_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	m_pRenderer->Set_Fogging(0);
	m_pRenderer->Set_DOF_Distance(100.f);

	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(CGameInstance::Get_Instance()->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera"));
	if( nullptr != pCamera )
	{
		_float4 vecPos;
		XMStoreFloat4(&vecPos, OPERA_START_POSITION);
		pCamera->Set_TeleportPosition(vecPos);
		pCamera->Set_Option(CMainCamera::OPTION::TELEPORT);
	}
	
	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_OperaBackStage()))
			return E_FAIL;
	}

	if (FAILED(m_pNavigation->Init_Index(OPERA_START_POSITION)))
		return E_FAIL;

	if (FAILED(m_pAndroid9S->SceneChange_OperaBackStage(OPERA_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pKatana->SceneChange_OperaBackStage(OPERA_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pLongKatana->SceneChange_OperaBackStage(OPERA_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pGauntlet->SceneChange_OperaBackStage(OPERA_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pSpear->SceneChange_OperaBackStage(OPERA_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	m_pPod->Set_MaxCoolTime();

	m_pTransform->Set_State(CTransform::STATE::RIGHT, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::LOOK, XMVectorSet(1.f, 0.f, 0.f, 0.f));

	return S_OK;
}

void CPlayer::Shadow_BeTraced()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameInstance->ResetPosition_Shadow_DirectionLight(L"ShadowDepth"
		, m_pTransform->Get_State(CTransform::STATE::POSITION));

	RELEASE_INSTANCE(CGameInstance);
}
HRESULT CPlayer::SceneChange_ZhuangziStage()
{
	Safe_Release(m_pNavigation);
	m_pTransform->Set_State(CTransform::STATE::POSITION, ZHUANGZI_START_POSITION);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	if (iter != m_mapComponents.end())
	{
		Safe_Release((iter->second));
		m_mapComponents.erase(iter);
	}

	m_pRenderer->Set_Fogging(1);
	m_pRenderer->Set_Fog(0.f, 60.f, CProportionConverter()(_DARKSALMON, 100.f,true));
	m_pRenderer->Set_DOF_Distance(50.f);
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(CGameInstance::Get_Instance()->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera"));
	if (nullptr != pCamera)
	{
		_float4 vecPos;
		XMStoreFloat4(&vecPos, ZHUANGZI_START_POSITION);
		pCamera->Set_TeleportPosition(vecPos);
		pCamera->Set_Option(CMainCamera::OPTION::TELEPORT);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ZHUANGZISTAGE, PROTO_KEY_NAVIGATION_ZHUANGZI, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_ZhuangziStage()))
			return E_FAIL;
	}

	if (FAILED(m_pNavigation->Init_Index(ZHUANGZI_START_POSITION)))
		return E_FAIL;

	if (FAILED(m_pAndroid9S->SceneChange_ZhuangziStage(ZHUANGZI_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pKatana->SceneChange_ZhuangziStage(ZHUANGZI_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pLongKatana->SceneChange_ZhuangziStage(ZHUANGZI_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pGauntlet->SceneChange_ZhuangziStage(ZHUANGZI_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pSpear->SceneChange_ZhuangziStage(ZHUANGZI_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	m_pPod->Set_MaxCoolTime();

	return S_OK;
}

HRESULT CPlayer::SceneChange_ROBOTGENERAL()
{
	m_pTransform->Set_State(CTransform::STATE::POSITION, ENGELS_START_POSITION);

	Safe_Release(m_pNavigation);
	auto& iter = m_mapComponents.find(COM_KEY_NAVIGATION);
	Safe_Release(iter->second);
	m_mapComponents.erase(iter);

	m_pRenderer->Set_Fogging(1);
	m_pRenderer->Set_Fog(45.f, 145.f, CProportionConverter()(_LAVENDERBLUSH, 85.f, true));
	m_pRenderer->Set_DOF_Distance(100.f);
	CMainCamera* pCamera = dynamic_cast<CMainCamera*>(CGameInstance::Get_Instance()->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera"));
	if (nullptr != pCamera)
	{
		_float4 vecPos;
		XMStoreFloat4(&vecPos, ENGELS_START_POSITION);
		pCamera->Set_TeleportPosition(vecPos);
		pCamera->Set_Option(CMainCamera::OPTION::TELEPORT);
	}

	_uint iIndex = 0;
	Add_Components((_uint)LEVEL::ROBOTGENERAL, PROTO_KEY_NAVIGATION_ENGELS, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iIndex);
	m_pNavigation->Update(XMMatrixIdentity());

	if (nullptr != m_pPod)
	{
		if (FAILED(m_pPod->SceneChange_ROBOTGENERAL()))
			return E_FAIL;
	}

	if (FAILED(m_pNavigation->Init_Index(ENGELS_START_POSITION)))
		return E_FAIL;

	if (FAILED(m_pAndroid9S->SceneChange_ROBOTGENERAL(ENGELS_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pKatana->SceneChange_ROBOTGENERAL(ENGELS_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pLongKatana->SceneChange_ROBOTGENERAL(ENGELS_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pGauntlet->SceneChange_ROBOTGENERAL(ENGELS_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	if (FAILED(m_pSpear->SceneChange_ROBOTGENERAL(ENGELS_START_POSITION, m_pNavigation->Get_Index())))
		return E_FAIL;

	m_pPod->Set_MaxCoolTime();

	return S_OK;
}

HRESULT CPlayer::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::tagTransformDesc		TransformDesc;
	TransformDesc.dSpeedPerSec = 5.f;
	TransformDesc.dRotationPerSec = XMConvertToRadians(90.0f);
	TransformDesc.fJumpPower = JUMPPOWER;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_PLAYER, TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	m_pModel->Add_RefNode(L"bone-1", "bone-1");
 	m_pModel->Add_RefNode(L"Weapon_Idle", "bone2656");
	m_pModel->Add_RefNode(L"Weapon_Play_Left", "bone516");
	m_pModel->Add_RefNode(L"Weapon_Play_Right", "bone260");
	m_pModel->Add_RefNode(L"FaceLight_Bone", "bone2656");

	/* For.Com_Navigation*/
	_uint	iCurrentIndex = 0;

	if (FAILED(Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
		return E_FAIL;

	/* For.Com_AABB */
	Engine::CCollider::DESC		ColliderDesc;

	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.fRadius = m_pModel->Get_Radius();
	ColliderDesc.vScale = _float3(PLAYER_COLLIDER_HITBOX);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &ColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &ColliderDesc)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Create_Subject(TEXT("SUBJECT_PLAYER"))))
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(SetUp_Observer()))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_INVENTORY, COM_KEY_INVENTORY, (CComponent**)&m_pInventory)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ConstantTable()
{
	Compute_CamDistanceForAlpha(m_pTransform);
	Compute_DOF_Parameters();
	if (FAILED(__super::SetUp_ConstantTable()))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pModel->SetUp_RawValue("g_matViewInv", &XMMatrixTranspose(pGameInstance->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_VIEW)), sizeof(_matrix));
	m_pModel->SetUp_RawValue("g_matProjInv", &XMMatrixTranspose(pGameInstance->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_PROJ)), sizeof(_matrix));

	m_pModel->SetUp_RawValue("g_vCamPos", &pGameInstance->Get_CameraPosition(), sizeof(_float4));
	m_pModel->SetUp_RawValue("g_vCamLook", &Get_CameraLook(), sizeof(_float4));

	m_pModel->SetUp_RawValue("g_vRimLightColor", &m_vRimLightColor, sizeof(_float3));

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CPlayer::SetUp_Bone1()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);
	if (pCamera == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return 0;
	}

	_float4x4 pMatrix;
	XMStoreFloat4x4(&pMatrix, m_pTransform->Get_WorldMatrix());
	pCamera->Set_Bone1(pMatrix);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CPlayer::SetUp_PlayerLook()
{
	_vector vPlayerLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

	XMStoreFloat4(&m_vPlayerLook, vPlayerLook);
}

HRESULT CPlayer::Create_ChargingEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.ulID = CHECKSUM_EFFECT_LONGKATANA_CHARGING;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));

	if (m_pChargingEffect)
	{
		m_pChargingEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}

	m_pChargingEffect = pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\EnergyCharging.dat", &tPacket);
	Safe_AddRef(m_pChargingEffect);

	return S_OK;
}

void CPlayer::Delete_ChargingEffect()
{
	if (nullptr != m_pChargingEffect)
	{
		m_pChargingEffect->Set_State(OBJSTATE::DEAD);
		Safe_Release(m_pChargingEffect);
		m_pChargingEffect = nullptr;
	}
}

HRESULT CPlayer::Create_CounterAttackEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = -1.f; // turn off it when i want;
	tPacket.ulID = CHECKSUM_EFFECT_POD_COUNTERATTACK;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));

	//if (m_pChargingEffect)
	//{
	//	m_pChargingEffect->Set_State(OBJSTATE::DEAD);
	//	Safe_Release(m_pChargingEffect);
	//	m_pChargingEffect = nullptr;
	//}

	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\CounterAttackEffect.dat", &tPacket);
	
	return S_OK;
}

void CPlayer::Calculate_Target_DOF(CGameInstance* pGameInstance)
{
	if (m_pTarget == nullptr)
		return;

	m_fDOFFar = m_pTarget->Get_CameraDistance() + 15.f;
	m_fDOFFocus = m_pTarget->Get_CameraDistance();
	m_fDOFNear = Get_CameraDistance() * -1.f;
	m_vPlayerDOF.w = 0.75f;
}

void CPlayer::Calculate_Player_DOF(CGameInstance* pGameInstance)
{
	m_fDOFFar = 150.f;
	m_fDOFFocus = Get_CameraDistance() + 1.f;
	m_fDOFNear = 0.0f;
	m_vPlayerDOF.w = 0.75f;
}

void CPlayer::Calculate_CutScene_DOF(CGameInstance* pGameInstance)
{
	m_fDOFFar = 75.f;
	m_fDOFFocus = 5.f;
	m_fDOFNear = 0.0f;
	m_vPlayerDOF.w = 1.f;
}

void CPlayer::Calculate_DepthOfField(CGameInstance * pGameInstance, _double dTimeDelta)
{
	_float fNear = m_vPlayerDOF.x;
	_float fFocus = m_vPlayerDOF.y;
	_float fFar = m_vPlayerDOF.z;

	// Near
	if (fNear >= m_fDOFNear)
	{
		fNear += m_fDOFNear * dTimeDelta;
		if (fNear < m_fDOFNear)
			fNear = m_fDOFNear;

		m_vPlayerDOF.x = fNear;
	}
	else if (fNear <= m_fDOFNear)
	{
		fNear += 1 * dTimeDelta;
		if (fNear > m_fDOFNear)
			fNear = m_fDOFNear;

		m_vPlayerDOF.x = fNear;
	}

	// Focus
	if (fFocus >= m_fDOFFocus)
	{
		fFocus -= m_fDOFFocus * dTimeDelta;
		if (fFocus < m_fDOFFocus)
			fFocus = m_fDOFFocus;

		m_vPlayerDOF.y = fFocus;
	}
	else if (fFocus <= m_fDOFFocus)
	{
		fFocus += m_fDOFFocus * dTimeDelta;
		if (fFocus > m_fDOFFocus)
			fFocus = m_fDOFFocus;

		m_vPlayerDOF.y = fFocus;
	}

	// Far
	if (fFar >= m_fDOFFar)
	{
		fFar -= m_fDOFFar * dTimeDelta;
		if (fFar < m_fDOFFar)
			fFar = m_fDOFFar;

		m_vPlayerDOF.z = fFar;
	}
	else if (fFar <= m_fDOFFar)
	{
		fFar += m_fDOFFar * dTimeDelta;
		if (fFar > m_fDOFFar)
			fFar = m_fDOFFar;

		m_vPlayerDOF.z = fFar;
	}
}


CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer* pGameInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pGameInstance = new CPlayer(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPlayer");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pInventory);
	
	Delete_ChargingEffect();

	if (nullptr != m_pTarget)
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}	

	if (nullptr != m_pPlayerSRV)
	{
		Safe_Release(m_pPlayerSRV);
		m_pPlayerSRV = nullptr;
	}
}
