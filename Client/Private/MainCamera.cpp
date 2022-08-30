#include "stdafx.h"
#include "..\Public\MainCamera.h"

#include "GameInstance.h"
#include "Player.h"
#include "Cam_Object.h"
#include "Camera_Debug.h"
#include "Coaster.h"
#include "Android_9S.h"
#include "CameraMgr.h"
#include "Engels.h"

CMainCamera::CMainCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CCamera(pGraphicDevice, pContextDevice)
{

}

CMainCamera::CMainCamera(const CMainCamera & rhs)
	: CCamera(rhs)
{

}

HRESULT CMainCamera::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	pCameraMgr->Set_MainCam_To_Mgr(this);

 	Set_Player();

	Set_CamFirst_3rd(); 
	
	m_eState = OBJSTATE::ENABLE;

	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (m_bPause)
		dTimeDelta = 0.0;

	if (m_eState == OBJSTATE::ENABLE)
	{

		if (m_eOption == OPTION::COASTER || m_eOption == OPTION::CATMULLROM_COASTER_TO_3RD || m_eOption == OPTION::CATMULLROM_COASTER_TO_3RD || m_eOption == OPTION::CATMULLROM_RSIDE_TO_COASTER
			|| m_eOption == OPTION::CATMULLROM_STAIR_TO_COASTER)
		{
			Get_Parameter_from_Coaster();
		}

		if (!m_bLeftCheckOnce && (m_eOption == OPTION::LSIDE || m_eOption == OPTION::CATMULLROM_3RD_TO_LSIDE || m_eOption == OPTION::CATMULLROM_LSIDE_TO_3RD))
		{
			Get_Parameter_from_Player_for_Left();
			m_bLeftCheckOnce = true;
		}

		if (!m_bRightCheckOnce && (m_eOption == OPTION::RSIDE || m_eOption == OPTION::CATMULLROM_RSIDE_TO_3RD || m_eOption == OPTION::CATMULLROM_3RD_TO_RSIDE))
		{
			Get_Parameter_from_Player_for_Right();
			m_bRightCheckOnce = true;
		}

		if (!m_bCheckTopCamLookOnce && (m_eOption == OPTION::TOP || m_eOption == OPTION::CATMULLROM_3RD_TO_TOP || m_eOption == OPTION::CATMULLROM_TOP_TO_3RD 
			|| m_eOption == OPTION::TOPB || m_eOption == OPTION::CATMULLROM_3RD_TO_TOPB || m_eOption == OPTION::CATMULLROM_TOPB_TO_3RD))
		{
			Get_Parameter_from_Player_for_Top();
			m_bCheckTopCamLookOnce = true;
		}

		Set_CamView(dTimeDelta);

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		if (pGameInstance->Key_Down(DIK_BACKSLASH))
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_DebugCam_On();

			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

			pPlayer->Set_Targeting_False();

		}

		_long MouseMove = 0;
		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_W))
		{
			m_fCamDistance -= (_float)(MouseMove * dTimeDelta * 0.04f);

			if (m_fCamDistance <= 2.f)
				m_fCamDistance = 2.f;

			if (m_fCamDistance >= 10.f)
				m_fCamDistance = 10.f;
		}

		RELEASE_INSTANCE(CGameInstance);

		return CCamera::Tick(dTimeDelta);
	}

	return 0;
}

_int CMainCamera::LateTick(_double dTimeDelta)
{
	if (m_bRightCheckOnce && m_eOption != OPTION::RSIDE && m_eOption != OPTION::CATMULLROM_RSIDE_TO_3RD && m_eOption != OPTION::CATMULLROM_3RD_TO_RSIDE)
		m_bRightCheckOnce = false;

	if (m_eOption != OPTION::COASTER)
		m_tDesc.fFar = 250.f;

	if (m_eOption == OPTION::FIXED)
		Check_FovChange(dTimeDelta);

	return _int();
}

void CMainCamera::Set_Option(OPTION eOption)
{
	m_eOption = eOption;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);
	if (nullptr == pPlayer)
		return;

	CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
	if (nullptr == pAndroid)
		return;

	pPlayer->Set_CamOption((_uint)m_eOption);

	if (OPTION::COASTER == m_eOption)
	{
		if (CPlayer::NAVISTATE::COASTER != pPlayer->Get_NaviState())
			pPlayer->Set_PositionForCoaster(0.0);

		pPlayer->Set_NaviState(CPlayer::NAVISTATE::COASTER);
		pAndroid->Set_NaviState(CAndroid_9S::NAVISTATE::COASTER);

		pAndroid->Set_PositionForCoaster();
	}
	else
	{
		pPlayer->Set_NaviState(CPlayer::NAVISTATE::NONE);

		if (OPTION::CATMULLROM_STAIR_TO_COASTER == m_eOption)
		{
			pAndroid->Set_NaviState(CAndroid_9S::NAVISTATE::COASTER);
			pAndroid->Set_AnimationForCoaster();
		}
		else
			pAndroid->Set_NaviState(CAndroid_9S::NAVISTATE::NONE);
	}
}

void CMainCamera::Cam_Moving_CatmullRom(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_eOption == OPTION::CATMULLROM_TOP_TO_3RD)
	{
		Cam_View_CatmullRom_TopTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::SWITHLOCK)
	{
		Cam_View_CatmullRom_Lock(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CANCLELOCK)
	{
		Cam_View_CatmullRom_CancleLock(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_WHEEL)
	{
		Cam_View_CatmullRom_3rdToWheel(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_WHEEL_TO_3RD)
	{
		Cam_View_CatmullRom_WheelTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_RSIDE)
	{
		Cam_View_CatmullRom_3rdToRSide(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_3RD)
	{
		Cam_View_CatmullRom_RSideTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_LSIDE)
	{
		Cam_View_CatmullRom_3rdToLSide(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_LSIDE_TO_3RD)
	{
		Cam_View_CatmullRom_LSideTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_COASTER)
	{
		Cam_View_CatmullRom_RsideToCoaster(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_STAIR)
	{
		Cam_View_CatmullRom_3rdToStair(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_STAIR_TO_COASTER)
	{
		Cam_View_CatmullRom_StairToCoaster(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_CUTSCENE_TO_3RD)
	{
		Cam_View_CatmullRom_CutsceneTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOP)
	{
		Cam_View_CatmullRom_3rdToTop(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_TOP_TO_3RD)
	{
		Cam_View_CatmullRom_TopTo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_SWITCH_TO_3RD)
	{
		Cam_View_CatmullRom_Switch_To_3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_UI)
	{
		Cam_View_CatmullRom_3rdToUI(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_UI_TO_3RD)
	{
		Cam_View_CatmullRom_UITo3rd(TimeDelta, vPointStart, vPointEnd);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOPB)
	{
		Cam_View_CatmullRom_3rdToTopB(TimeDelta, vPointStart, vPointEnd);
	}
	
	else if (m_eOption == OPTION::CATMULLROM_TOPB_TO_3RD)
	{
		Cam_View_CatmullRom_TopBTo3rd(TimeDelta, vPointStart, vPointEnd);
	}
}

void CMainCamera::Cam_Moving_CatmullRom_Matrix(_double TimeDelta, _matrix * vMatrixStart, _matrix * vMatrixEnd)
{
	int a = 0;
}

void CMainCamera::Set_CamView(_double TimeDelta)
{
	_vector vStartViewPos, vGoalViewPos;
	_matrix vStartMatrix, vGoalMatrix;

	if (m_eOption == OPTION::FIXED)
	{
		Cam_View_3rd(TimeDelta);
	}

	else if (m_eOption == OPTION::RSIDE)
	{
		Cam_View_RSide(TimeDelta);
	}

	else if (m_eOption == OPTION::TOP)
	{
		Cam_View_Top(TimeDelta);
		Check_Shake_Cam_State(TimeDelta);
	}

	else if (m_eOption == OPTION::LSIDE)
	{
		Cam_View_LSide(TimeDelta);
	}

	else if (m_eOption == OPTION::WHEEL)
	{
		Cam_View_Wheel(TimeDelta);
	}

	else if (m_eOption == OPTION::COASTER)
	{
		Cam_View_Coaster(TimeDelta);
	}

	else if (m_eOption == OPTION::STAIR)
	{
		Cam_View_Stair(TimeDelta);
	}

	else if (m_eOption == OPTION::TELEPORT)
	{
		Cam_View_Teleport();
	}

	else if (m_eOption == OPTION::CATMULLROM)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_RSide_Temporary(&vGoalViewPos);

	}
	
	else if (m_eOption == OPTION::UI)
	{
		Cam_View_UI(TimeDelta);
	}

	else if (m_eOption == OPTION::TOPB)
	{
		Cam_View_TopB(TimeDelta);
		Check_Shake_Cam_State(TimeDelta);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_WHEEL)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_Wheel_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_WHEEL_TO_3RD)
	{
		_vector vCamPos;

		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}
		
		Get_CamPos_3rd_Temporary(&vGoalViewPos);
		vStartViewPos  = XMLoadFloat4(&m_vPosition);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOP)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_Top_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOPB)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_TopB_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}
	
	else if (m_eOption == OPTION::CATMULLROM_TOP_TO_3RD)
	{
		_vector vCamPos;

		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}

		Get_CamPos_3rd_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_TOPB_TO_3RD)
	{
		_vector vCamPos;

		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}

		Get_CamPos_3rd_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}
	
	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_RSIDE)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_RSide_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_3RD)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}
		Get_CamPos_3rd_Temporary(&vGoalViewPos);

		vStartViewPos = XMLoadFloat4(&m_vPosition);

		m_bOnce2 = false;
		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_COASTER)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}
		Get_CamPos_Coaster_Temporary(&vGoalViewPos);
		
		vStartViewPos = XMLoadFloat4(&m_vPosition);
		m_bOnce2 = false;
		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_LSIDE)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_LSide_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_LSIDE_TO_3RD)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}
		Get_CamPos_3rd_Temporary(&vGoalViewPos);

		vStartViewPos = XMLoadFloat4(&m_vPosition);

		m_bOnce2 = false;
		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::SWITHLOCK)
	{

		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_LockOn_Temporary(&vGoalViewPos);

		m_bOnce2 = false;
		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CANCLELOCK)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}

		Get_CamView_Switch(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);

	}

	else if (m_eOption == OPTION::CATMULLROM_SWITCH_TO_3RD)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}

		Get_CamPos_3rd_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_STAIR)
	{
		Get_CamPos_3rd_Temporary(&vStartViewPos);
		Get_CamPos_Stair_Temporary(&vGoalViewPos);

		m_MouseMoveStateX = (_ulong)0.f;
		m_MouseMoveStateY = (_ulong)0.f;
	}

	else if (m_eOption == OPTION::CATMULLROM_STAIR_TO_COASTER)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bOnce == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bOnce = true;
		}

		Get_CamPos_Coaster_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);
	}

	else if (m_eOption == OPTION::CATMULLROM_3RD_TO_UI)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_b3rdCheck == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_b3rdCheck = true;
			m_bUIcheck = false;
			m_fAccTime = 0.f;
		}

		Get_CamPos_UI_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);
	}

	else if (m_eOption == OPTION::CATMULLROM_UI_TO_3RD)
	{
		_vector vCamPos;
		vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		if (m_bUIcheck == false)
		{
			XMStoreFloat4(&m_vPosition, vCamPos);
			m_bUIcheck = true;
			m_b3rdCheck = false;
			m_fAccTime = 0.f;
		}

		Get_CamPos_3rd_Temporary(&vGoalViewPos);
		vStartViewPos = XMLoadFloat4(&m_vPosition);
	}

	else if (m_eOption == OPTION::CATMULLROM_CUTSCENE_TO_3RD)
	{
		_vector vCamPos;
		vCamPos = XMLoadFloat4(&m_vCutSceneCamPos);

		vStartViewPos = vCamPos;
		Get_CamPos_3rd_Temporary(&vGoalViewPos);
	}

	Cam_Moving_CatmullRom(TimeDelta, &vStartViewPos, &vGoalViewPos);


}

void CMainCamera::Cam_View_3rd(_double TimeDelta)
{
	if (m_pPlayer->Get_Targeting() == false && m_eOption == OPTION::FIXED)
	{
		Check_MouseState(TimeDelta);
		Update_Position(TimeDelta);
		Check_Shake_Cam_State(TimeDelta);
	}
	if (m_pPlayer->Get_Targeting() == true && m_eOption == OPTION::FIXED)
	{
		//Check_MouseState(TimeDelta);
		m_bMovingCamOnLock = true;
		LockOn(TimeDelta);
	}
}

void CMainCamera::Cam_View_RSide(_double TimeDelta)
{
	//Get_Player`s Position & Resetting it.
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vPlayerRight = XMVector3Normalize(XMLoadFloat4(&m_vSetted_PlayerRight));

	_vector vNewCamPos, vNewCamRight, vNewCamUp, vNewCamLook;

	vNewCamPos = vPlayerPos + (vPlayerRight * m_fCamDistance_RSideView);

	vNewCamLook = XMVector3Normalize(vPlayerPos - vNewCamPos);

	vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vNewCamUp, vNewCamLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

	if (m_bShake)
		Cam_Set_Shake(TimeDelta);
	if (m_bShakeMiddle)
		Cam_Set_Shake_Middle(TimeDelta);

}

void CMainCamera::Get_Parameter_from_Player_for_Right()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*  pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	_vector vecPlayerRight = pPlayerTransform->Get_State(CTransform::STATE::RIGHT);

	XMStoreFloat4(&m_vSetted_PlayerRight, vecPlayerRight);

	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::Cam_View_LSide(_double TimeDelta)
{
	//Get_Player`s Position & Resetting it.
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vPlayerLeft = XMVector3Normalize(XMLoadFloat4(&m_vSetted_PlayerLeft));

	_vector vNewCamPos, vNewCamRight, vNewCamUp, vNewCamLook;

	vNewCamPos = vPlayerPos + (vPlayerLeft * m_fCamDistance_LSideView);

	vNewCamLook = XMVector3Normalize(vPlayerPos - vNewCamPos);

	vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vNewCamUp, vNewCamLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

	if (m_bShake)
		Cam_Set_Shake(TimeDelta);
	if (m_bShakeMiddle)
		Cam_Set_Shake_Middle(TimeDelta);

}

void CMainCamera::Cam_View_Top(_double TimeDelta)
{
	//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos;
 	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	_vector vUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vPlayerLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerLook_for_Top));
	_vector vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vNewCamLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_float fY = XMVectorGetY(vPlayerPos);
	fY += 18.f;
	_vector vPlayerPos2 = XMVectorSetY(vPlayerPos, fY);
	vNewCamPos = vPlayerPos2 - vPlayerLook * 2.f;

	vNewCamLook = XMVector3Normalize(vPlayerPos - vNewCamPos);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vNewCamUp, vNewCamLook));

	vNewCamUp = XMVector3Normalize(XMVector3Cross(vNewCamLook, vNewCamRight));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

	if (m_bShake)
		Cam_Set_Shake(TimeDelta);
	if (m_bShakeMiddle)
		Cam_Set_Shake_Middle(TimeDelta);
}

void CMainCamera::Cam_View_CatmullRom_TopTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
		m_bCheckTopCamLookOnce = false;
	}

	if (m_eOption == OPTION::CATMULLROM_TOP_TO_3RD)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
			m_fAccTime = 1.f;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Cam_View_CatmullRom_3rdToTopB(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::TOPB);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOPB)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
		{
			m_fAccTime = 1.f;
		}

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Get_CamPos_TopB_Temporary(_vector * pOut)
{
	//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	_vector vUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vPlayerLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerLook_for_Top));
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_float fY = XMVectorGetY(vPlayerPos);
	fY += 6.f;

	vPlayerPos = XMVectorSetY(vPlayerPos, fY);
	vNewCamPos = vPlayerPos - vPlayerLook * 2.f;

	*pOut = vNewCamPos;
}

void CMainCamera::Cam_View_TopB(_double TimeDelta)
{
	//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	_vector vUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vPlayerLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerLook_for_Top));
	_vector vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vNewCamLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_float fY = XMVectorGetY(vPlayerPos);
	fY += 6.f;
	_vector vPlayerPos2 = XMVectorSetY(vPlayerPos, fY);
	vNewCamPos = vPlayerPos2 - vPlayerLook * 2.f;

	vNewCamLook = XMVector3Normalize(vPlayerPos - vNewCamPos);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vNewCamUp, vNewCamLook));

	vNewCamUp = XMVector3Normalize(XMVector3Cross(vNewCamLook, vNewCamRight));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

	if (m_bShake)
		Cam_Set_Shake(TimeDelta);
	if (m_bShakeMiddle)
		Cam_Set_Shake_Middle(TimeDelta);
}

void CMainCamera::Cam_View_CatmullRom_TopBTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
		m_bCheckTopCamLookOnce = false;
	}

	if (m_eOption == OPTION::CATMULLROM_TOPB_TO_3RD)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
			m_fAccTime = 1.f;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Cam_View_CatmullRom_3rdToCoaster(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
}

void CMainCamera::Get_CamPos_Coaster_Temporary(_vector * pOut)
{
	_vector vCoasterLook, vCoasterUp, vCoasterRight, vCoasterPos, vNewCamPos;

	vCoasterLook = XMLoadFloat4(&m_vecCoasterLook);
	vCoasterLook = XMVector3Normalize(vCoasterLook);

	vCoasterPos = XMLoadFloat4(&m_vecCoasterPosition);

	vCoasterUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vCoasterRight = XMVector3Cross(vCoasterUp, vCoasterLook);

	vCoasterPos += vCoasterUp * 4.95f;
	
	vCoasterPos -= vCoasterLook *6.f;

	vNewCamPos = vCoasterPos + (vCoasterRight * 10.f);

	*pOut = vNewCamPos;
}

void CMainCamera::Cam_View_Coaster(_double TimeDelta)
{
	_vector vCoasterLook , vCoasterUp , vCoasterRight , vCoasterPos ,vNewCamPos;

	vCoasterLook = XMLoadFloat4(&m_vecCoasterLook);
	vCoasterLook = XMVector3Normalize(vCoasterLook);

	vCoasterPos = XMLoadFloat4(&m_vecCoasterPosition);

	vCoasterUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vCoasterRight = XMVector3Cross(vCoasterUp, vCoasterLook);

	vCoasterPos += vCoasterUp * 4.95f;

	vCoasterPos -= vCoasterLook *6.f;

	vNewCamPos = vCoasterPos + (vCoasterRight * 10.f);
	
	_vector vNewCamRight, vNewCamUp, vNewCamLook;

	vNewCamLook = vCoasterPos - vNewCamPos;
	vNewCamLook = XMVector3Normalize(vNewCamLook);

	vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	vNewCamRight = XMVector3Cross(vNewCamUp, vNewCamLook);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

	m_tDesc.fFar = 250.f;

}

void CMainCamera::Cam_View_CatmullRom_CoasterTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
}

void CMainCamera::Cam_View_CatmullRom_RsideToCoaster(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::COASTER);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_COASTER)
	{
		_vector vPosition;
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

	}
}

void CMainCamera::Get_Parameter_from_Coaster()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CModel* pCoasterModel = (CModel*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", TEXT("Com_Model"), 0);

	_float4 vecLook = pCoasterModel->Get_vecLook();

	m_vecCoasterLook = vecLook;

	CTransform* pCoasterTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", COM_KEY_TRANSFORM, 0);

	_vector vCoasterPos = pCoasterTransform->Get_State(CTransform::STATE::POSITION);

	XMStoreFloat4(&m_vecCoasterPosition, vCoasterPos);

	CCoaster* pCoaster = (CCoaster*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", 0);

	if (pCoaster->Get_Coaster_Animstate() == CCoaster::ANIMSTATE::IDLE)
		Set_Option(OPTION::FIXED);

	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::Cam_View_Stair(_double TimeDelta)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vStairEye = XMVectorSet(STAIR_POSITION_X, STAIR_POSITION_Y, STAIR_POSITION_EYE_Z,1.f);
	_vector vStairAt = XMVectorSet(STAIR_POSITION_X, STAIR_POSITION_Y, STAIR_POSITION_AT_Z, 1.f);

	_vector vNewLook = XMVector3Normalize(vStairAt - vStairEye);

	_vector vNewCamPos = vPlayerPos - (vNewLook * 8.f);

	_vector vNewUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

	_vector vNewRight = XMVector3Normalize(XMVector3Cross(vNewUp, vNewLook));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

}

void CMainCamera::Get_CamPos_Stair_Temporary(_vector * pOut)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vStairEye = XMVectorSet(STAIR_POSITION_X, STAIR_POSITION_Y, STAIR_POSITION_EYE_Z, 1.f);
	_vector vStairAt = XMVectorSet(STAIR_POSITION_X, STAIR_POSITION_Y, STAIR_POSITION_AT_Z, 1.f);

	_vector vNewLook = XMVector3Normalize(vStairAt - vStairEye);

	_vector vNewCamPos = vPlayerPos - (vNewLook * 8.f);

	*pOut = vNewCamPos;
}

void CMainCamera::Cam_View_CatmullRom_3rdToStair(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::STAIR);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_STAIR)
	{
		_vector vPosition = XMVectorSet(0.f,0.f,0.f,0.f);
		_vector vPlayerPos = XMVectorSet(0.f,0.f,0.f,0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
	}
}

void CMainCamera::Cam_View_CatmullRom_StairToCoaster(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::COASTER);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CCoaster* pCoaster = (CCoaster*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", 0);

		if (pCoaster == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		pCoaster->Start_Coaster();

		RELEASE_INSTANCE(CGameInstance);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_STAIR_TO_COASTER)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);
		_vector vCoasterPos = XMLoadFloat4(&m_vecCoasterPosition);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vCoasterPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

		///////////////////////////////////////////////////////////////////

		/*_vector vPos = XMVectorSet(128.55f, 40.45f, -20.6117f, 1.f);
		_vector vCoasterPos = XMVectorSet(128.5619f, 35.95f, -10.6116f, 1.f);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, vPos, vPos, m_fAccTime);

		_vector vNewLook = vCoasterPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;*/
	}
}

void CMainCamera::Set_MouseMoveCheckToZero()
{
	m_MouseMoveStateCheckX = 0;
	m_MouseMoveStateCheckY = 0;
}

void CMainCamera::Cam_View_CatmullRom_CutsceneTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_CUTSCENE_TO_3RD)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);
		
		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Set_CutscenePos(_vector vPos)
{
	XMStoreFloat4(&m_vCutSceneCamPos, vPos);
}

void CMainCamera::Shake_Cam_Engels(_double TimeDelta)
{
	if ((m_fShakeTimeE >= 1.5f) && (-0.01f <= m_fShakeYE) && (m_fShakeYE <= 0.01f)
		&& (-0.01f <= m_fShakeXE) && (m_fShakeXE <= 0.01f)
		&& (-0.01f <= m_fShakeZE) && (m_fShakeZE <= 0.01f))
	{
		m_fShakeTimeE = 0.f;
		m_bShakeEngels = false;

		CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

		pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/Engels.dat", 4);

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEngels* pBoss = (CEngels*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ROBOTGENERAL, L"Boss", 0);

		if (pBoss == nullptr)
		{
			RELEASE_INSTANCE(CGameInstance);
			return;
		}

		pBoss->Set_Start_True();
		pBoss->Set_UI_Engage_Cinematic();
		RELEASE_INSTANCE(CGameInstance);

	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTimeE += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeYE = sin(m_fShakeTimeE * 15.f) * powf(0.5f, m_fShakeTimeE * 0.3f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeYE * 0.3f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeYE * 0.3f);

		m_fShakeXE = sin(m_fShakeTimeE * 15.f) * powf(0.5f, m_fShakeTimeE * 0.3f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeXE * 0.3f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeXE * 0.3f);

		m_fShakeZE = sin(m_fShakeTimeE * 15.f) * powf(0.5f, m_fShakeTimeE * 0.3f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZE * 0.3f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZE * 0.3f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Get_Parameter_from_Player_for_Top()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

	if (pPlayer == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_float4 vecPlayerLook = pPlayer->Get_PlayerLook();

	m_vPlayerLook_for_Top = vecPlayerLook;


	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::Cam_View_UI(_double TimeDelta)
{
	_vector vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	_vector vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);

	_vector vUIPos = vPlayerPos + (vPlayerLook * 1.f) + (vPlayerUp * 1.8f);

	_vector vCamPos = vPlayerPos + (vPlayerLook * -1.f) + (vPlayerUp * 1.8f) + (vPlayerRight * 1.25f);

	_vector vNewCamLook = XMVector3Normalize(vUIPos - vCamPos);
	_vector vNewCamUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vNewCamRight = XMVector3Cross(vNewCamUp, vNewCamLook);
	vNewCamUp = XMVector3Cross(vNewCamLook, vNewCamRight);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);

}

void CMainCamera::Cam_View_CatmullRom_3rdToUI(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::UI);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_UI)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
		_vector vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);

		_vector vUIPos = vPlayerPos + (vPlayerLook * 1.f) + (vPlayerUp * 1.8f);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
			m_fAccTime = 1.f;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vUIPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Cam_View_CatmullRom_UITo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_UI_TO_3RD)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
			m_fAccTime = 1.f;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Get_CamPos_UI_Temporary(_vector * pOut)
{
	_vector vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	_vector vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);

	_vector vCamPos = vPlayerPos + (vPlayerLook * -1.f) + (vPlayerUp * 1.8f) + (vPlayerRight * 1.25f);

	*pOut = vCamPos;
}

void CMainCamera::Shake_Cam_Hit(_double TimeDelta)
{
	if ((m_fShakeTimeH >= 0.05f) && (-0.01f <= m_fShakeYH) && (m_fShakeYH <= 0.01f)
		&& (-0.01f <= m_fShakeXH) && (m_fShakeXH <= 0.01f)
		&& (-0.01f <= m_fShakeZH) && (m_fShakeZH <= 0.01f))
	{
		m_fShakeTimeH = 0.f;
		m_bShakeHit = false;
	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTimeH += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeYH = sin(m_fShakeTimeH * 8.f) * powf(0.2f, m_fShakeTimeH * 10.f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeYH * 0.3f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeYH * 0.3f);

		m_fShakeXH = sin(m_fShakeTimeH * 8.f) * powf(0.2f, m_fShakeTimeH * 10.f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeXH * 0.3f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeXH * 0.3f);

		m_fShakeZH = sin(m_fShakeTimeH * 8.f) * powf(0.2f, m_fShakeTimeH * 10.f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZH * 0.3f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ - m_fShakeZH * 0.3f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Shake_Cam_HitBig(_double TimeDelta)
{
	if ((m_fShakeTimeHB >= 0.05f) && (-0.01f <= m_fShakeYHB) && (m_fShakeYHB <= 0.01f)
		&& (-0.01f <= m_fShakeXHB) && (m_fShakeXHB <= 0.01f)
		&& (-0.01f <= m_fShakeZHB) && (m_fShakeZHB <= 0.01f))
	{
		m_fShakeTimeHB = 0.f;
		m_bShakeHitBig = false;
	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTimeHB += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeYHB = sin(m_fShakeTimeHB * 12.f) * powf(0.2f, m_fShakeTimeHB * 10.f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeYHB * 0.3f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeYHB * 0.3f);

		m_fShakeXHB = sin(m_fShakeTimeHB * 12.f) * powf(0.2f, m_fShakeTimeHB * 10.f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeXHB * 0.3f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeXHB * 0.3f);

		m_fShakeZHB = sin(m_fShakeTimeHB * 12.f) * powf(0.2f, m_fShakeTimeHB * 10.f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZHB * 0.3f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ - m_fShakeZHB * 0.3f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Shake_Cam_Gauntlet(_double TimeDelta)
{
	if ((m_fShakeTimeG >= 0.05f) && (-0.01f <= m_fShakeYG) && (m_fShakeYG <= 0.01f)
		&& (-0.01f <= m_fShakeXG) && (m_fShakeXG <= 0.01f)
		&& (-0.01f <= m_fShakeZG) && (m_fShakeZG <= 0.01f))
	{
		m_fShakeTimeG = 0.f;
		m_bShakeHitGauntlet = false;
	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTimeG += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeYG = sin(m_fShakeTimeG * 8.f) * powf(0.2f, m_fShakeTimeG * 10.f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeYG * 0.3f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeYG * 0.3f);

		m_fShakeXG = sin(m_fShakeTimeG * 8.f) * powf(0.2f, m_fShakeTimeG * 10.f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeXG * 0.3f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeXG * 0.3f);

		m_fShakeZG = sin(m_fShakeTimeG * 8.f) * powf(0.2f, m_fShakeTimeG * 10.f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZG * 0.3f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ - m_fShakeZG * 0.3f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Check_Shake_Cam_State(_double TimeDelta)
{
	if (m_bShake)
		Cam_Set_Shake(TimeDelta);
	else if (m_bShakeMiddle)
		Cam_Set_Shake_Middle(TimeDelta);
	else if (m_bShakeEngels)
		Shake_Cam_Engels(TimeDelta);
	else if (m_bShakeHit)
		Shake_Cam_Hit(TimeDelta);
	else if (m_bShakeHitBig)
		Shake_Cam_HitBig(TimeDelta);
	else if (m_bShakeHitGauntlet)
		Shake_Cam_Gauntlet(TimeDelta);
}

void CMainCamera::Check_FovChange(_double TimeDelta)
{
		if (m_pPlayer->Get_AnimState() == (_uint)CPlayer::ANIMSTATE::JUSTAVOID
			|| m_pPlayer->Get_AnimState() == (_uint)CPlayer::ANIMSTATE::JUSTAVOID_COUNTERATTACK || m_bDodgeTime)
		{
			if (m_pPlayer->Get_Dodge())
			{
				m_fFov += (_float)TimeDelta * 60.f;

				if (m_fFov >= 75.f)
					m_fFov = 75.f;

				m_tDesc.fFOV = XMConvertToRadians(m_fFov);
				m_bDodgeTime = true;
				
			}

			else if (m_pPlayer->Get_Dodge() == false)
			{
				m_fFov -= (_float)TimeDelta * 30.f;

				if (m_fFov <= 60.f)
				{
					m_fFov = 60.f;
					m_bDodgeTime = false;
				}

				m_tDesc.fFOV = XMConvertToRadians(m_fFov);
			}
		}

	else if (m_pPlayer->Get_EquipStateMain() == (_uint)CPlayer::EQUIPSTATE::LONGKATANA)
	{
		if (m_pPlayer->Get_PressingR())
		{
			m_fFov -= (_float)TimeDelta * 10.f;

			if (m_fFov <= 40.f)
				m_fFov = 40.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}

		else if (m_pPlayer->Get_LKatanaRush())
		{
			m_fFov -= (_float)TimeDelta * 20.f;

			if (m_fFov <= 40.f)
				m_fFov = 40.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}

		else if (m_pPlayer->Get_PressingR() == false && m_pPlayer->Get_LKatanaRush() == false)
		{
			m_fFov += (_float)TimeDelta * 20.f;

			if (m_fFov >= 60.f)
				m_fFov = 60.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}


	}

	else if (m_pPlayer->Get_EquipStateMain() == (_uint)CPlayer::EQUIPSTATE::KATANA)
	{
		if (m_pPlayer->Get_KatanaR())
		{
			m_fFov -= (_float)TimeDelta * 30.f;

			if (m_fFov <= 40.f)
				m_fFov = 40.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}

		else if (m_pPlayer->Get_KatanaR() == false)
		{
			m_fFov += (_float)TimeDelta * 20.f;

			if (m_fFov >= 60.f)
				m_fFov = 60.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}
	}

	else if (m_pPlayer->Get_EquipStateMain() == (_uint)CPlayer::EQUIPSTATE::SPEAR)
	{
		if (m_pPlayer->Get_ThrowSpear())
		{
			m_fFov -= (_float)TimeDelta * 60.f;

			if (m_fFov <= 40.f)
				m_fFov = 40.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}

		else if (m_pPlayer->Get_ThrowSpear() == false)
		{
			m_fFov += (_float)TimeDelta * 30.f;

			if (m_fFov >= 60.f)
				m_fFov = 60.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}
	}

	else if (m_pPlayer->Get_EquipStateMain() == (_uint)CPlayer::EQUIPSTATE::GAUNTLET)
	{
		if (m_pPlayer->Get_GauntletSpin())
		{
			m_fFov -= (_float)TimeDelta * 60.f;

			if (m_fFov <= 40.f)
				m_fFov = 40.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}

		else if (m_pPlayer->Get_GauntletSpin() == false)
		{
			m_fFov += (_float)TimeDelta * 30.f;

			if (m_fFov >= 60.f)
				m_fFov = 60.f;

			m_tDesc.fFOV = XMConvertToRadians(m_fFov);
		}
	}

}

void CMainCamera::Cam_View_CatmullRom_3rdToWheel(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::WHEEL);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

		Set_9S_Fix();
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_WHEEL)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

	}

}

void CMainCamera::Get_CamPos_Wheel_Temporary(_vector * pOut)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	//vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_float fPlayerY = XMVectorGetY(vPlayerPos);

	_vector vWheelCenterPos = { 0.f , 0.f , 0.f , 1.f };
	vWheelCenterPos = XMVectorSetX(vWheelCenterPos, WHEEL_POSITION_X);
	vWheelCenterPos = XMVectorSetY(vWheelCenterPos, fPlayerY);
	vWheelCenterPos = XMVectorSetZ(vWheelCenterPos, WHEEL_POSITION_Z);

	_vector vNewCamLook;
	vNewCamLook = XMVector3Normalize(vWheelCenterPos - vPlayerPos);
	_vector vNewCamUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vNewCamRight = XMVector3Cross(vNewCamUp, vNewCamLook);
	vNewCamUp = XMVector3Cross(vNewCamLook, vNewCamRight);

	_vector vNewCamPos;
	vNewCamPos = vPlayerPos - (vNewCamLook * 9.f);

	*pOut = vNewCamPos;
}

void CMainCamera::Cam_View_Wheel(_double TimeDelta)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	//vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_float fPlayerY = XMVectorGetY(vPlayerPos);

	_vector vWheelCenterPos = { 0.f , 0.f , 0.f , 1.f };
	vWheelCenterPos = XMVectorSetX(vWheelCenterPos, WHEEL_POSITION_X);
	vWheelCenterPos = XMVectorSetY(vWheelCenterPos, fPlayerY);
	vWheelCenterPos = XMVectorSetZ(vWheelCenterPos, WHEEL_POSITION_Z);

	_vector vNewCamLook;
	vNewCamLook = XMVector3Normalize( vWheelCenterPos - vPlayerPos );
	_vector vNewCamUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vNewCamRight = XMVector3Cross(vNewCamUp, vNewCamLook);
	vNewCamUp = XMVector3Cross(vNewCamLook, vNewCamRight);

	_vector vNewCamPos;
	vNewCamPos = vPlayerPos - (vNewCamLook * 9.f);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);


}

void CMainCamera::Cam_View_CatmullRom_WheelTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_WHEEL_TO_3RD)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)(TimeDelta * m_fCamSpeed * 0.5);

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);


	}
}

void CMainCamera::Cam_View_CatmullRom_3rdToRSide(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		m_eOption = OPTION::RSIDE;

	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_RSIDE)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

	}
}

void CMainCamera::Cam_View_CatmullRom_RSideTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		m_eOption = OPTION::FIXED;
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_RSIDE_TO_3RD)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)(TimeDelta * m_fCamSpeed * 0.5);

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		
	}
}

void CMainCamera::Cam_View_CatmullRom_3rdToLSide(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		m_eOption = OPTION::LSIDE;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_LSIDE)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

	}
}

void CMainCamera::Cam_View_CatmullRom_LSideTo3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		m_eOption = OPTION::FIXED;
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_LSIDE_TO_3RD)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)(TimeDelta * m_fCamSpeed * 0.5);

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

	}
}

void CMainCamera::Get_Parameter_from_Player_for_Left()
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform*  pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	_vector vecPlayerRight = pPlayerTransform->Get_State(CTransform::STATE::RIGHT);

	XMStoreFloat4(&m_vSetted_PlayerLeft, vecPlayerRight *-1.f );

	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::Cam_View_CatmullRom_Lock(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
	}

	if (m_eOption == OPTION::SWITHLOCK)
	{
		_vector vPosition;

		CGameObject* pTarget = m_pPlayer->Get_Target();
		if (pTarget == nullptr)
			return;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		CTransform* pTargetTransformCom = (CTransform*)pTarget->Get_Component(TEXT("Com_Transform"));
		_vector vTargetPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() == (_uint)LEVEL::ROBOTGENERAL)
		{
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			vTargetPos = pTargetTransformCom->Get_State(CTransform::STATE::POSITION);
			vTargetPos = vTargetPos + vUp * 80.f;
		}
		else
		{		
			vTargetPos = pTargetTransformCom->Get_State(CTransform::STATE::POSITION);
		}
		m_fAccTime += (_float)TimeDelta * m_fCamSpeed * 2.f;

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vTargetPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

		if (m_bShake)
			Cam_Set_Shake(TimeDelta);
		if (m_bShakeMiddle)
			Cam_Set_Shake_Middle(TimeDelta);

	}
}

void CMainCamera::Cam_View_CatmullRom_CancleLock(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		//m_eOption = OPTION::CATMULLROM_SWITCH_TO_3RD;
		Set_Option(OPTION::CATMULLROM_SWITCH_TO_3RD);
		m_bOnce = false;
		m_fRatioLock = 0.f;
		//m_MouseMoveStateCheckY = 0;
		//m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CANCLELOCK)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed * 2.f;
		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

		if (m_bShake)
			Cam_Set_Shake(TimeDelta);
		if (m_bShakeMiddle)
			Cam_Set_Shake_Middle(TimeDelta);

	}
}

void CMainCamera::Cam_Set_Shake(_double TimeDelta)
{
	if ((m_fShakeTime >= 1.f) && (-0.01f <= m_fShakeY) && (m_fShakeY <= 0.01f)
		&& (-0.01f <= m_fShakeX) && (m_fShakeX <= 0.01f)
		&& (-0.01f <= m_fShakeZ) && (m_fShakeZ <= 0.01f))
	{
		m_fShakeTime = 0.f;
		m_bShake = false;
	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTime += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeY = sin(m_fShakeTime * 12.f) * powf(0.5f, m_fShakeTime * 0.3f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeY * 0.3f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeY * 0.3f);

		m_fShakeX = sin(m_fShakeTime * 12.f) * powf(0.5f, m_fShakeTime * 0.3f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeX * 0.3f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeX * 0.3f);

		m_fShakeZ = sin(m_fShakeTime * 12.f) * powf(0.5f, m_fShakeTime * 0.3f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZ * 0.3f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZ * 0.3f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Cam_Set_Shake_Middle(_double TimeDelta)
{
	if ((m_fShakeTimeM >= 0.5f) && (-0.01f <= m_fShakeYM) && (m_fShakeYM <= 0.015f)
		&& (-0.015f <= m_fShakeXM) && (m_fShakeXM <= 0.015f)
		&& (-0.015f <= m_fShakeZM) && (m_fShakeZM <= 0.015f))
	{
		m_fShakeTimeM = 0.f;
		m_bShakeMiddle = false;
	}
	else
	{
		_uint a = rand() % 10;
		_uint b = rand() % 10;
		_uint c = rand() % 10;
		m_fShakeTimeM += (_float)TimeDelta;
		_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

		m_fShakeYM = sin(m_fShakeTimeM * 3.f) * powf(0.5f, m_fShakeTimeM * 0.3f);
		_float fCamY = XMVectorGetY(vCamPos);
		if (a >= 5)
			vCamPos = XMVectorSetY(vCamPos, fCamY + m_fShakeYM * 0.2f);
		else
			vCamPos = XMVectorSetY(vCamPos, fCamY - m_fShakeYM * 0.2f);

		m_fShakeXM = sin(m_fShakeTimeM * 3.f) * powf(0.5f, m_fShakeTimeM * 0.3f);
		_float fCamX = XMVectorGetX(vCamPos);
		if (b >= 5)
			vCamPos = XMVectorSetX(vCamPos, fCamX + m_fShakeXM * 0.2f);
		else
			vCamPos = XMVectorSetX(vCamPos, fCamX - m_fShakeXM * 0.2f);

		m_fShakeZM = sin(m_fShakeTimeM * 3.f) * powf(0.5f, m_fShakeTimeM * 0.3f);
		_float fCamZ = XMVectorGetZ(vCamPos);
		if (c >= 5)
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZM * 0.2f);
		else
			vCamPos = XMVectorSetZ(vCamPos, fCamZ + m_fShakeZM * 0.2f);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}
}

void CMainCamera::Set_CamView_Switch(_double TimeDelta)
{
	_vector vPlayerRight, vPlayerUp, vCamLook, vPlayerPos, vPos, vRight, vLook;

	vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	vCamLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	//vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	vCamLook = XMVector3Normalize(vCamLook);
	vPlayerUp = XMVector3Normalize(vPlayerUp);

	vPos = (vPlayerPos - vCamLook * 7.f) + (vPlayerUp * 1.5f);
	vRight = vPlayerRight;
	vLook = vPlayerPos - vPos;
	vLook = XMVector3Normalize(vLook);

	vPos = vPlayerPos - vLook * m_fCamDistance;

	if (m_pPlayer->Get_Spurt() == true)
	{
		vPos = vPlayerPos - vLook * (m_fCamDistance + 1.f);
	}
}

void CMainCamera::Get_CamView_Switch(_vector * pOut)
{
	_vector vPlayerRight, vPlayerUp, vCamLook, vPlayerPos, vPos, vRight, vLook;

	vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	vCamLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	//vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	vCamLook = XMVector3Normalize(vCamLook);
	vPlayerUp = XMVector3Normalize(vPlayerUp);

	vPos = (vPlayerPos - vCamLook * 7.f) /*+ (vPlayerUp * 1.5f)*/;
	vRight = vPlayerRight;
	vLook = vPlayerPos - vPos;
	vLook = XMVector3Normalize(vLook);

	vPos = vPlayerPos - vLook * m_fCamDistance;

	if (m_pPlayer->Get_Spurt() == true)
	{
		vPos = vPlayerPos - vLook * (m_fCamDistance + 1.f);
	}

	*pOut = vPos;

}

void CMainCamera::Cam_View_CatmullRom_Switch_To_3rd(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::FIXED);
		m_bOnce = false;
		m_fRatioLock = 0.f;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_SWITCH_TO_3RD)
	{
		_vector vPosition = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		_vector vPlayerPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed * 1.2f;
		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = { 0.f , 1.f , 0.f , 0.f };

		_vector vRight = XMVector3Cross(vUp, vNewLook);

		vUp = XMVector3Cross(vNewLook, vRight);

		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);
		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);

		if (m_bShake)
			Cam_Set_Shake(TimeDelta);
		if (m_bShakeMiddle)
			Cam_Set_Shake_Middle(TimeDelta);

	}
}

void CMainCamera::Get_CamPos_3rd_Temporary(_vector * pOut)
{
	_vector vPlayerRight, vPlayerUp, vPlayerLook, vPlayerPos, vPos, vRight, vLook;

	vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
	//vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	vPlayerLook = XMVector3Normalize(vPlayerLook);
	vPlayerUp = XMVector3Normalize(vPlayerUp);

	vPos = (vPlayerPos - vPlayerLook * 7.f) + (vPlayerUp * 1.5f);
	vRight = vPlayerRight;
	vLook = vPlayerPos - vPos;
	vLook = XMVector3Normalize(vLook);

	vPos = vPlayerPos - vLook * m_fCamDistance;

	if (m_pPlayer->Get_Spurt() == true)
	{
		vPos = vPlayerPos - vLook * (m_fCamDistance + 1.f);
	}

	*pOut = vPos;

	XMStoreFloat4(&m_vLastPos, vPos);
}

void CMainCamera::Get_CamPos_RSide_Temporary(_vector * pOut)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vPlayerRight = XMVector3Normalize(XMLoadFloat4(&m_vSetted_PlayerRight));

	_vector vNewCamPos;

	vNewCamPos = vPlayerPos + (vPlayerRight * m_fCamDistance_RSideView);

	*pOut = vNewCamPos;
}

void CMainCamera::Get_CamPos_LSide_Temporary(_vector * pOut)
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	_vector vPlayerLeft = XMVector3Normalize(XMLoadFloat4(&m_vSetted_PlayerLeft));

	_vector vNewCamPos;

	vNewCamPos = vPlayerPos + (vPlayerLeft * m_fCamDistance_LSideView);

	*pOut = vNewCamPos;
}

void CMainCamera::Cam_View_CatmullRom_3rdToTop_Matrix(_double TimeDelta, _matrix * vMatrixStart, _matrix * vMatrixEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::TOP);

		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOP)
	{
		//_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);

		//m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		//vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		//_vector vNewLook = vPlayerPos - vPosition;

		//vNewLook = XMVector3Normalize(vNewLook);

		//_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		//_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		//vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		//vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		//m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		//m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		//m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		//m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		//m_MouseMoveStateCheckY = 0;
		//m_MouseMoveStateCheckX = 0;


		///////// Test matrix Lerp ////////////

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed;

		if (m_fAccTime > 1.f)
		{
			m_fAccTime = 1.f;
		}

		_float4x4 StartMatrix; 
		XMStoreFloat4x4(&StartMatrix, *vMatrixStart);
		_float4 vecRight, vecUp, vecLook, vecPosition;
		memcpy( &vecRight, &StartMatrix.m[0], sizeof(_float4));
		memcpy(&vecUp, &StartMatrix.m[1], sizeof(_float4));
		memcpy(&vecLook, &StartMatrix.m[2], sizeof(_float4));
		memcpy(&vecPosition, &StartMatrix.m[3], sizeof(_float4));

		_vector vRight, vUp, vLook, vPos;
		vRight = XMLoadFloat4(&vecRight);
		vUp = XMLoadFloat4(&vecUp);
		vLook = XMLoadFloat4(&vecLook);
		vPos = XMLoadFloat4(&vecPosition);


		// GoalMatrix //
		_vector vRotationRight = XMVector3TransformNormal(vRight, *vMatrixEnd);
		_vector vRotationUp = XMVector3TransformNormal(vUp, *vMatrixEnd);
		_vector vRotationLook = XMVector3TransformNormal(vLook, *vMatrixEnd);
		_vector vRotationPosition = XMVector3TransformCoord(vPos, *vMatrixEnd);

	/*	vRotationRight = XMVectorLerp(vRight, vRotationRight, m_fAccTime);
		vRotationUp = XMVectorLerp(vUp, vRotationUp, m_fAccTime);
		vRotationLook = XMVectorLerp(vLook, vRotationLook, m_fAccTime);
		vRotationPosition = XMVectorLerp(vPos, vRotationPosition, m_fAccTime);*/
		vRotationRight = XMVectorLerp(vRight, vRotationRight, m_fAccTime);
		vRotationUp = XMVectorLerp(vUp, vRotationUp, m_fAccTime);
		vRotationLook = XMVectorLerp(vLook, vRotationLook, m_fAccTime);
		vRotationPosition = XMVectorLerp(vPos, vRotationPosition, m_fAccTime);

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRotationRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vRotationUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vRotationLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vRotationPosition);
	}
}

void CMainCamera::Cam_View_CatmullRom_3rdToTop(_double TimeDelta, _vector * vPointStart, _vector * vPointEnd)
{
	if (m_fAccTime >= 1.f)
	{
		m_fAccTime = 0.f;
		Set_Option(OPTION::TOP);
		m_bOnce = false;
		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;
	}

	if (m_eOption == OPTION::CATMULLROM_3RD_TO_TOP)
	{
		_vector vPosition;
		//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos;
		Set_Bone1toVector(m_Bone1, &vPlayerPos);

		m_fAccTime += (_float)TimeDelta * m_fCamSpeed; /** m_fCamSpeed * 0.5f;*/

		if (m_fAccTime > 1.f)
		{
			m_fAccTime = 1.f;
		}

		vPosition = XMVectorCatmullRom(*vPointStart, *vPointStart, *vPointEnd, *vPointEnd, m_fAccTime);

		_vector vNewLook = vPlayerPos - vPosition;

		vNewLook = XMVector3Normalize(vNewLook);

		_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

		_vector vRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

		vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

		vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

		m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
		m_pTransform->Set_State(CTransform::STATE::UP, vUp);
		m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

		m_MouseMoveStateCheckY = 0;
		m_MouseMoveStateCheckX = 0;

	}
}

void CMainCamera::Get_CamPos_Top_Temporary(_vector * pOut)
{
	//_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	_vector vUp = { 0.f , 1.f ,0.f ,0.f };
	_vector vPlayerLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerLook_for_Top));
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	_float fY = XMVectorGetY(vPlayerPos);
	fY += 18.f;

	vPlayerPos = XMVectorSetY(vPlayerPos, fY);
	vNewCamPos = vPlayerPos - vPlayerLook * 2.f;

	*pOut = vNewCamPos;
}

void CMainCamera::Get_CamPos_LockOn_Temporary(_vector * pOut)
{
	if (m_pPlayer->Get_Targeting())
	{
		CGameObject* pTarget = m_pPlayer->Get_Target();
		if (pTarget == nullptr)
			return;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		CTransform* pTargetTransformCom = (CTransform*)pTarget->Get_Component(TEXT("Com_Transform"));
		_vector vTargetPos = pTargetTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
		_vector vTemporaryTargetPos, vTemporaryLook;

		if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() != (_uint)LEVEL::ROBOTGENERAL)
		{
			_float fPlayerY = XMVectorGetY(vPlayerPos);

			vTemporaryTargetPos = XMVectorSetY(vTargetPos, fPlayerY);

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);
		}

		else
		{
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			vTemporaryTargetPos = vTargetPos + vUp * 80.f;

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);
		}

		_vector vNewCamPos;

		if (m_pPlayer->Get_Spurt() == false)
			vNewCamPos = vPlayerPos - vTemporaryLook * 3.f + (vPlayerUp * 2.f);

		if (m_pPlayer->Get_Spurt() == true)
			vNewCamPos = vPlayerPos - vTemporaryLook * 5.f + (vPlayerUp * 3.f);

		*pOut = vNewCamPos;

		//m_bLockOn = true;
	}
}

void CMainCamera::Get_CamLook_LockOn_Temporary(_vector * pOut)
{
	if (m_pPlayer->Get_Targeting())
	{
		CGameObject* pTarget = m_pPlayer->Get_Target();
		if (pTarget == nullptr)
			return;

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		CTransform* pTargetTransformCom = (CTransform*)pTarget->Get_Component(TEXT("Com_Transform"));
		_vector vTargetPos = pTargetTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
		_vector vTemporaryTargetPos, vTemporaryLook;
		_vector vNewCamPos;
		_vector vNewCamLook;

		if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() != (_uint)LEVEL::ROBOTGENERAL)
		{
			_float fPlayerY = XMVectorGetY(vPlayerPos);

			vTemporaryTargetPos = XMVectorSetY(vTargetPos, fPlayerY);

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);

			if (m_pPlayer->Get_Spurt() == false)
				vNewCamPos = vPlayerPos - vTemporaryLook * 3.f + (vPlayerUp * 2.f);

			if (m_pPlayer->Get_Spurt() == true)
				vNewCamPos = vPlayerPos - vTemporaryLook * 5.f + (vPlayerUp * 3.f);

			vNewCamLook = XMVector3Normalize(vTargetPos - vNewCamPos);

		}

		else
		{
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			vTemporaryTargetPos = vTargetPos + vUp * 80.f;

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);

			if (m_pPlayer->Get_Spurt() == false)
				vNewCamPos = vPlayerPos - vTemporaryLook * 3.f + (vPlayerUp * 2.f);

			if (m_pPlayer->Get_Spurt() == true)
				vNewCamPos = vPlayerPos - vTemporaryLook * 5.f + (vPlayerUp * 3.f);

			vNewCamLook = XMVector3Normalize(vTemporaryTargetPos - vNewCamPos);

		}

		*pOut = vNewCamLook;
		//m_bLockOn = true;
	}
}

void CMainCamera::Get_CamMatrix_3rd_Temporary(_matrix * pOut)
{
	_vector vRight = m_pTransform->Get_State(CTransform::STATE::RIGHT);
	_vector vUp = m_pTransform->Get_State(CTransform::STATE::UP);
	_vector vLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);

	memcpy(&m_3rdViewMatrix.m[0], &vRight, sizeof(_vector));
	memcpy(&m_3rdViewMatrix.m[1], &vUp, sizeof(_vector));
	memcpy(&m_3rdViewMatrix.m[2], &vLook, sizeof(_vector));
	memcpy(&m_3rdViewMatrix.m[3], &vPosition, sizeof(_vector));

	*pOut = XMLoadFloat4x4(&m_3rdViewMatrix);
}

void CMainCamera::Set_Bone1(_float4x4 Pos)
{
	m_Bone1 = Pos;
}

void CMainCamera::Set_Bone1toVector(_float4x4 bone, _vector * pOut)
{
	_float4 vPos{};
	memcpy(&vPos, &m_Bone1.m[3], sizeof(_float4));
	_vector vVecPos;
	vVecPos = XMLoadFloat4(&vPos);
	vVecPos = XMVectorSetY(vVecPos, XMVectorGetY(vVecPos) + 1.5f);
	*pOut = vVecPos;
}

HRESULT CMainCamera::Set_Player()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	m_pPlayerTransformCom = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

	m_pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), 0);

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);

	XMStoreFloat4(&m_vLastPlayerPos, vPlayerPos);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMainCamera::Set_CamFirst_3rd()
{
	_vector vPlayerRight, vPlayerUp, vPlayerLook, vPlayerPos, vPos, vRight, vUp, vLook;

	vPlayerRight = m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT);
	vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
	vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
	vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	vPlayerPos += { 0.f, 1.5f, 0.f, 0.f};

	vPlayerLook = XMVector3Normalize(vPlayerLook);
	vPlayerUp = XMVector3Normalize(vPlayerUp);

	vPos = (vPlayerPos - vPlayerLook * 7.f) + (vPlayerUp * 2.5f);
	vRight = vPlayerRight;
	vLook = vPlayerPos - vPos;
	vLook = XMVector3Normalize(vLook);

	/*_float4x4 HeadFloat4x4 = m_pPlayer->Get_Head();

	memcpy(&vPos, HeadFloat4x4.m[3], sizeof(_vector));

	vPos = vPos - vLook * 2.f;*/

	vPos = vPlayerPos - vLook * m_fCamDistance;

	vUp = XMVector3Cross(vLook, vRight);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vPos);

	XMStoreFloat4(&m_vLastPos, vPos);
}

void CMainCamera::Set_CamFirst_RSide()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerRight = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT));
	_vector vPlayerUp = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::UP));
	_vector vPlayerLook = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK));

	_vector vNewCamLook = vPlayerRight * -1.f;
	_vector vNewCamUp = vPlayerUp;
	_vector vNewCamRight = vPlayerLook;
	_vector vCamPos;

	vCamPos = vPlayerPos - vNewCamLook * m_fCamDistance_RSideView;

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
}

void CMainCamera::Set_CamFirst_LSide()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_vector vPlayerRight = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::RIGHT));
	_vector vPlayerUp = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::UP));
	_vector vPlayerLook = XMVector3Normalize(m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK));

	_vector vNewCamLook = vPlayerRight;
	_vector vNewCamUp = vPlayerUp;
	_vector vNewCamRight = vPlayerLook;
	_vector vCamPos;

	vCamPos = vPlayerPos - vNewCamLook * m_fCamDistance_LSideView;

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
}

void CMainCamera::Set_CamFirst_Top()
{
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
	_float fPlayerY = XMVectorGetY(vPlayerPos);
	fPlayerY += 1.5f;

	XMVectorSetY(vPlayerPos,fPlayerY);

	_vector vUp = XMVectorSet(0.f, 10000.f, 1.f, 0.f);
	_vector vUp2 = { 0.f , 1.f ,0.f ,0.f };
	_vector vCamUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	vUp = XMVector3Normalize(vUp);

	vNewCamPos = vPlayerPos + vUp * 7.f;

	vNewCamLook = vPlayerPos - vNewCamPos;

	vNewCamLook = XMVector3Normalize(vNewCamLook);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vUp2, vNewCamLook));

	vUp2 = XMVector3Normalize(XMVector3Cross(vNewCamLook, vNewCamRight));

	m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vUp2);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);

}

void CMainCamera::Update_Position(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vCamLook = m_pTransform->Get_State(CTransform::STATE::LOOK);
	_vector vPlayerLook = m_pPlayerTransformCom->Get_State(CTransform::STATE::LOOK);
	_vector vPlayerPos;

	_vector vLastPlayerPos = XMLoadFloat4(&m_vLastPlayerPos);

	Set_Bone1toVector(m_Bone1, &vPlayerPos);

	XMStoreFloat4(&m_vLastPlayerPos, vPlayerPos);

	//if (false == m_pPlayer->Get_Spurt() /*&& m_pPlayer->Get_CurPlayerAnimState() != CPlayer::ANIMSTATE::AVOID*/ )
	if (m_pPlayer->Get_CurPlayerAnimState() != CPlayer::ANIMSTATE::JUMPPOWERATTACK)
	{
		if (m_pPlayer->Get_Spurt() == false)
		{
			if (m_fNowCamDistance > m_fCamDistance)
				m_fNowCamDistance -= (_float)TimeDelta * 4.f;
			else
				m_fNowCamDistance = m_fCamDistance;
		}

		else
		{
			if (m_fNowCamDistance < m_fCamDistance + 1.f)
				m_fNowCamDistance += (_float)TimeDelta * 4.f;
			else
				m_fNowCamDistance = m_fCamDistance + 1.f;
		}

		vCamPos = vPlayerPos - (vCamLook * m_fNowCamDistance);
		_vector vLastCamPos;
		vLastCamPos = XMLoadFloat4(&m_vLastPos);
		if (Check_Position(vLastCamPos, vCamPos))
		{
			m_fRatioFix = 0.f;
		}
		else
		{
			m_fRatioFix += (_float)TimeDelta * 0.1f;
			if (m_fRatioFix >= 1.f)
				m_fRatioFix = 1.f;
		}

		XMStoreFloat4(&m_vGoalPosition, vCamPos);


		//_vector vNewCamPosition = XMVectorLerp(vLastCamPos, vCamPos, 0.18f);
		_vector vNewCamPosition = XMVectorLerp(vLastCamPos, vCamPos, (_float)TimeDelta * 5.f);


		_vector vDistance = vNewCamPosition - vLastCamPos;
		_vector vDistance2 = XMVector3Length(vDistance);
		_float fDist = XMVectorGetX(vDistance2);

		//if (fDist > 0.09f)
		if (fDist > 0.12f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.85f );
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 40.f);
		}

		else if (fDist > 0.09f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.81f);
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 36.f);
			int a = 0;
		}

		else if (fDist > 0.07f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.77f);
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 33.f);
		}

		else if (fDist > 0.05f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.73f);
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 30.f);
		}

		else if (fDist > 0.03f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.69f);
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 28.f);
		}

		//if (fDist > 0.01f)
		//{
		//	vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, TimeDelta * 25.f);
		//}

		else if (fDist > 0.005f)
		{
			//vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, 0.65f);
			vNewCamPosition = XMVectorLerp(vLastCamPos, vNewCamPosition, (_float)TimeDelta * 22.f);
		}

		else if (fDist <= 0.002f)
		{
			vNewCamPosition = vCamPos;
		}

		m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPosition);
	}
	if ( /*m_pPlayer->Get_CurPlayerAnimState() == CPlayer::ANIMSTATE::SPURT && m_pPlayer->Get_Spurt() == false*/
		//|| m_pPlayer->Get_CurPlayerAnimState() == CPlayer::ANIMSTATE::AVOID
		m_pPlayer->Get_CurPlayerAnimState() == CPlayer::ANIMSTATE::JUMPPOWERATTACK
		|| m_pPlayer->Get_SceneChange() == true
		/*|| m_pPlayer->Get_CurPlayerAnimIndex() == 63
		|| m_pPlayer->Get_CurPlayerAnimIndex() == 66
		|| m_pPlayer->Get_CurPlayerAnimIndex() == 69
		|| m_pPlayer->Get_CurPlayerAnimIndex() == 72
		|| (m_pPlayer->Get_CurPlayerAnimIndex() == 62 && m_pPlayer->Get_Spurt() == false)
		|| (m_pPlayer->Get_CurPlayerAnimIndex() == 65 && m_pPlayer->Get_Spurt() == false)
		|| (m_pPlayer->Get_CurPlayerAnimIndex() == 68 && m_pPlayer->Get_Spurt() == false)
		|| (m_pPlayer->Get_CurPlayerAnimIndex() == 71 && m_pPlayer->Get_Spurt() == false)*/)
	{
		
		if (m_fNowCamDistance > CAM_JUMPATK_DISTANCE_PLUS)
			m_fNowCamDistance -= (_float)TimeDelta * 4.f;
 		else if (m_fNowCamDistance < CAM_JUMPATK_DISTANCE_MINUS)
			m_fNowCamDistance += (_float)TimeDelta * 4.f;
		else
			m_fNowCamDistance = CAM_JUMPATK_DISTANCE_NORMAL;

		vCamPos = vPlayerPos - (vCamLook * m_fNowCamDistance);
		m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	}

	//if (true == m_pPlayer->Get_Spurt())
	//{

	//	if (m_fDistance > 6.01f)
	//		m_fDistance -= (_float)TimeDelta * 4.f;
	//	else if (m_fDistance < 5.99f)
	//		m_fDistance += (_float)TimeDelta * 4.f;
	//	else
	//		m_fDistance = 6.f;

	//	vCamPos = vPlayerPos - (vCamLook * m_fDistance);
	//	m_pTransform->Set_State(CTransform::STATE::POSITION, vCamPos);
	//}

	_vector vCameraPos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	XMStoreFloat4(&m_vLastPos, vCameraPos);

	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::Check_MouseState(_double TimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_long MouseMove = 0;
	_vector vRight = XMLoadFloat4(&m_vFirstCamRight);
	_vector vUp = XMLoadFloat4(&m_vFirstCamUp);
	_vector vLook = XMLoadFloat4(&m_vFirstCamLook);
	_vector vPos = XMLoadFloat4(&m_vFirstCamPos);

	if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_X))
	{
		if (m_pPlayer->Get_Targeting() == false)
			m_pTransform->Rotation_Axis(MouseMove * TimeDelta * 0.008f);
		else
		{
			m_MouseMoveStateCheckX += MouseMove;

			if (m_MouseMoveStateCheckX > (_long)CAM_PLUS_X_LIMIT)
			{
				MouseMove = MouseMove + (CAM_PLUS_X_LIMIT - m_MouseMoveStateCheckX);
				m_MouseMoveStateCheckX = (_long)CAM_PLUS_X_LIMIT;
			}

			if (m_MouseMoveStateCheckX < (_long)CAM_MINUS_X_LIMIT)
			{
				MouseMove = MouseMove + (CAM_MINUS_X_LIMIT - m_MouseMoveStateCheckX);
				m_MouseMoveStateCheckX = (_long)CAM_MINUS_X_LIMIT;
			}

			m_MouseMoveStateX = MouseMove;

			if(CAM_MINUS_X_LIMIT <= m_MouseMoveStateCheckX && m_MouseMoveStateCheckX <= CAM_PLUS_X_LIMIT)
			{
				m_pTransform->Rotation_Axis(MouseMove * TimeDelta * 0.005f);
			}

		}
	}
	else
		m_MouseMoveStateX = 0;

	if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_Y))
	{
		m_MouseMoveStateCheckY += MouseMove;

		if (m_MouseMoveStateCheckY > (_long)CAM_PLUS_Y_LIMIT) 
		{
			MouseMove = MouseMove + (CAM_PLUS_Y_LIMIT - m_MouseMoveStateCheckY);
   			m_MouseMoveStateCheckY = (_long)CAM_PLUS_Y_LIMIT;
		}

		if (m_MouseMoveStateCheckY < (_long)CAM_MINUS_Y_LIMIT)
		{
			MouseMove = MouseMove + (CAM_MINUS_Y_LIMIT - m_MouseMoveStateCheckY);
			m_MouseMoveStateCheckY = (_long)CAM_MINUS_Y_LIMIT;
		}

		m_MouseMoveStateY = MouseMove;

		if (CAM_MINUS_Y_LIMIT <= m_MouseMoveStateCheckY && m_MouseMoveStateCheckY <= CAM_PLUS_Y_LIMIT)
		{
			m_pTransform->Rotation_Axis(MouseMove * TimeDelta * 0.005f, m_pTransform->Get_State(CTransform::STATE::RIGHT));
		}


	}
	else
		m_MouseMoveStateY = 0;

	if (m_MouseMoveStateX == 0 && m_MouseMoveStateY == 0)
	{
		m_bHoldCam = true;

		m_dHoldTime += TimeDelta;

		if (m_dHoldTime >= 0.6)
			m_bMovingCamOnLock = true;

	}
	else
	{
		m_bHoldCam = false;

		m_dHoldTime = 0;

		m_bMovingCamOnLock = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CMainCamera::LockOn(_double TimeDelta)
{
	if (m_pPlayer->Get_Targeting())
	{
		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		m_fRatioLock += (_float)TimeDelta * 0.1f;

		CGameObject* pTarget = m_pPlayer->Get_Target();
		if (pTarget == nullptr)
			return;

		CTransform* pTargetTransformCom = (CTransform*)pTarget->Get_Component(TEXT("Com_Transform"));
		_vector vTargetPos = pTargetTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE::POSITION);
		_vector vPlayerUp = m_pPlayerTransformCom->Get_State(CTransform::STATE::UP);
		_vector vTemporaryTargetPos, vTemporaryLook;

		if (pGameInstance->Get_CurrentLevel()->Get_CurrentLevelID() != (_uint)LEVEL::ROBOTGENERAL)
		{
			_float fPlayerY = XMVectorGetY(vPlayerPos);

			vTemporaryTargetPos = XMVectorSetY(vTargetPos, fPlayerY);

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);
		}

		else
		{
			_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			vTemporaryTargetPos = vTargetPos + vUp * 80.f;

			vTemporaryLook = XMVector3Normalize(vTemporaryTargetPos - vPlayerPos);
		}

		_vector vNewCamPos;

		if (m_pPlayer->Get_Spurt() == false)
			vNewCamPos = vPlayerPos - vTemporaryLook * 3.f + (vPlayerUp * 2.f);

		if (m_pPlayer->Get_Spurt() == true)
			vNewCamPos = vPlayerPos - vTemporaryLook * 5.f + (vPlayerUp * 3.f);

		if (m_bMovingCamOnLock ) // ���콺�� �������ٰ� ���缭 0.6�ʰ� ��������
		{
			_vector vVirtualCamLook;
			Get_CamLook_LockOn_Temporary(&vVirtualCamLook);
			_vector vNowCamLook;
			vNowCamLook = m_pTransform->Get_State(CTransform::STATE::LOOK);

			vNowCamLook = XMVector3Normalize(vNowCamLook);

			//_vector vNewCamLook = XMVectorLerp(vNowCamLook, vVirtualCamLook, 0.02f);
			_vector vNewCamLook = XMVectorLerp(vNowCamLook, vVirtualCamLook, (_float)TimeDelta * 3.f);

			_vector vUp = { 0.f , 1.f , 0.f , 0.f };

			_vector vNewCamRight = XMVector3Cross(vUp, vNewCamLook);
			_vector vNewCamUp = XMVector3Cross(vNewCamLook, vNewCamRight);
			//vNewCamLook = XMVector3Cross(vNewCamRight, vNewCamUp);

			_vector vPosition = m_pTransform->Get_State(CTransform::STATE::POSITION);

			m_pTransform->Set_State(CTransform::STATE::RIGHT, vNewCamRight);
			m_pTransform->Set_State(CTransform::STATE::UP, vNewCamUp);
			m_pTransform->Set_State(CTransform::STATE::LOOK, vNewCamLook);
			m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);
			m_MouseMoveStateCheckX = 0;
		}
		else 
		{
			m_pTransform->Set_State(CTransform::STATE::POSITION, vNewCamPos);
			m_MouseMoveStateCheckY = (_long)0.0;

		}

		if (m_bShake)
			Cam_Set_Shake(TimeDelta);
		if (m_bShakeMiddle)
			Cam_Set_Shake_Middle(TimeDelta);
	}
}

_bool CMainCamera::Check_Position(_vector vPos1, _vector vPos2)
{
	_float fX1 = XMVectorGetX(vPos1);
	_float fY1 = XMVectorGetY(vPos1);
	_float fZ1 = XMVectorGetZ(vPos1);

	_float fX2 = XMVectorGetX(vPos2);
	_float fY2 = XMVectorGetY(vPos2);
	_float fZ2 = XMVectorGetZ(vPos2);

	_float fX3 = abs(fX1 - fX2);
	_float fY3 = abs(fY1 - fY2);
	_float fZ3 = abs(fZ1 - fZ2);

	//if (fX1 == fX2 && fY1 == fY2 && fZ1 == fZ2)
	//	return true;
	if (fX3 <= CAM_CHECK_POSITION_DISTANCE && fY3 <= CAM_CHECK_POSITION_DISTANCE && fZ3 <= CAM_CHECK_POSITION_DISTANCE)
		return true;
	else
		return false;
}

void CMainCamera::Set_9S_Fix()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
	if (nullptr == pAndroid)
		return;

	pAndroid->Set_Fix(true);
}

void CMainCamera::Cam_View_Teleport()
{
	_vector vNewPos = XMLoadFloat4(&m_vTeleportPosition);

	_float fY = XMVectorGetY(vNewPos);

	fY += 1.5f;

	vNewPos = XMVectorSetY(vNewPos, fY);

	m_pTransform->Set_State(CTransform::STATE::POSITION, vNewPos);
	
	m_vLastPos = m_vTeleportPosition;
	
	m_eOption = OPTION::FIXED;
}

void CMainCamera::Set_TopCam_Look()
{
	_vector vPlayerPos;
	Set_Bone1toVector(m_Bone1, &vPlayerPos);
	_vector vUp = XMVectorSet(0.f, 10000.f, 1.f, 0.f);
	_vector vUp2 = { 0.f , 1.f ,0.f ,0.f };
	_vector vCamUp = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamLook = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamPos = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector vNewCamRight = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	vUp = XMVector3Normalize(vUp);

	vNewCamPos = vPlayerPos + vUp * 7.f;

	vNewCamLook = vPlayerPos - vNewCamPos;

	vNewCamLook = XMVector3Normalize(vNewCamLook);

	vNewCamRight = XMVector3Normalize(XMVector3Cross(vUp2, vNewCamLook));

	vUp2 = XMVector3Normalize(XMVector3Cross(vNewCamLook, vNewCamRight));

}

CMainCamera * CMainCamera::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CMainCamera* pInstance = new CMainCamera(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMainCamera::Clone(void * pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMainCamera::Free()
{
	__super::Free();
}
