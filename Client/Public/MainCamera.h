#pragma once
#ifndef __CLIENT_MAIN_CAMERA_H__
#define __CLIENT_MAIN_CAMERA_H__

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"

BEGIN(Client)
class CMainCamera final : public CCamera
{
public:
	enum class OPTION : _uint
	{
		FIXED = 0,
		TOP = 1,
		RSIDE = 2,
		CATMULLROM = 3,
		SWITHLOCK = 4,
		CANCLELOCK = 5,
		LSIDE = 6,
		COASTER = 7,
		WHEEL = 8,
		CATMULLROM_3RD_TO_WHEEL = 9,
		CATMULLROM_WHEEL_TO_3RD = 10,
		CATMULLROM_3RD_TO_TOP = 11,
		CATMULLROM_TOP_TO_3RD = 12,
		CATMULLROM_3RD_TO_COASTER = 13,
		CATMULLROM_COASTER_TO_3RD = 14,
		CATMULLROM_3RD_TO_RSIDE = 15,
		CATMULLROM_RSIDE_TO_3RD = 16,
		CATMULLROM_3RD_TO_LSIDE = 17,
		CATMULLROM_LSIDE_TO_3RD = 18,
		CATMULLROM_RSIDE_TO_COASTER = 19,
		STAIR = 20,
		CATMULLROM_3RD_TO_STAIR = 21,
		CATMULLROM_STAIR_TO_COASTER = 22,
		TELEPORT = 23,
		CATMULLROM_CUTSCENE_TO_3RD = 24,
		CATMULLROM_3RD_TO_UI = 25,
		CATMULLROM_UI_TO_3RD = 26,
		UI = 27,
		SWITCH = 28,
		CATMULLROM_SWITCH_TO_3RD = 29,
		TOPB = 30,
		CATMULLROM_3RD_TO_TOPB = 31,
		CATMULLROM_TOPB_TO_3RD = 32,
		COUNT = 999
	};

private:
	CMainCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta) override;
	virtual _int					LateTick(_double dTimeDelta) override;

public:
	void	Set_Option(OPTION eOption);
	void	Cam_Moving_CatmullRom(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_Moving_CatmullRom_Matrix(_double TimeDelta, _matrix* vMatrixStart = nullptr, _matrix* vMatrixEnd = nullptr);
	
private:
	void	Set_CamView(_double TimeDelta);
	void	Cam_View_3rd(_double TimeDelta);
	void	Cam_View_CatmullRom_Lock(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_CancleLock(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_Set_Shake(_double TimeDelta);
	void	Cam_Set_Shake_Middle(_double TimeDelta);

private:
	void	Set_CamView_Switch(_double TimeDelta);
	void	Get_CamView_Switch(_vector* pOut);

	void	Cam_View_CatmullRom_Switch_To_3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);

	
public:
	void	Get_CamPos_3rd_Temporary(_vector* pOut);
	void	Get_CamPos_LockOn_Temporary(_vector* pOut);
	void	Get_CamLook_LockOn_Temporary(_vector* pOut);

	void	Get_CamMatrix_3rd_Temporary(_matrix* pOut);
	
public:
	const OPTION&	Get_Option() const { return m_eOption; }

public:
	void	Set_Bone1(_float4x4 Pos);
	void	Set_Bone1toVector(_float4x4 bone, _vector* pOut);
	void	Set_ShakeTrue() { m_bShake = true; }
	void	Set_ShakeFalse() { m_bShake = false; }
	void	Set_ShakeMTrue() { m_bShakeMiddle = true; }
	void	Set_ShakeMFalse() { m_bShakeMiddle = false; }
	void	Set_CamViewTrue() { m_bCamViewing = true; }
	void	Set_LastPosition(_float4 vPos) { m_vLastPos = vPos; }
	void	Set_TeleportPosition(_float4 vPos) { m_vTeleportPosition = vPos; }
	void	Set_ShakeEngelsTrue() { m_bShakeEngels = true; }
	void	Set_ShakeEngelsFalse() { m_bShakeEngels = false; }
	void	Set_ShakeHitTrue() { m_bShakeHit = true; }
	void	Set_ShekeHitFalse() { m_bShakeHit = false; }
	void	Set_ShakeHitBigTrue() { m_bShakeHitBig = true; }
	void	Set_ShakeHitBigFalse() { m_bShakeHitBig = false; }
	void	Set_ShakeGauntletTrue() { m_bShakeHitGauntlet = true; }
	void	Set_ShakeGauntletFalse() { m_bShakeHitGauntlet = false; }

public:
	HRESULT Set_Player();
	void	Set_CamFirst_3rd();
	void	Set_CamFirst_RSide();
	void	Set_CamFirst_LSide();
	void	Set_CamFirst_Top();
	void	Update_Position(_double TimeDelta);
	void	Check_MouseState(_double TimeDelta);
	void	LockOn(_double TimeDelta);
	_bool	Check_Position(_vector vPos1, _vector vPos2);

////////// FOR. 9S FIX //////////
public:
	void	Set_9S_Fix();

//////////// for Teleport /////////////
public:
	void	Cam_View_Teleport();

/////////// for Right ///////////////
public:
	void	Get_CamPos_RSide_Temporary(_vector* pOut);
	void	Cam_View_CatmullRom_3rdToRSide(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_RSideTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_RSide(_double TimeDelta);
	void	Get_Parameter_from_Player_for_Right();
/////////////////////////////////////

//////////// for Left ////////////////
public:
	void	Get_CamPos_LSide_Temporary(_vector* pOut);
	void	Cam_View_LSide(_double TimeDelta);
	void	Cam_View_CatmullRom_3rdToLSide(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_LSideTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_Parameter_from_Player_for_Left();

//////////////////////////////////////

//////////// for Wheel //////////////
public:
	void	Cam_View_CatmullRom_3rdToWheel(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_CamPos_Wheel_Temporary(_vector* pOut);
	void	Cam_View_Wheel(_double TimeDelta);
	void	Cam_View_CatmullRom_WheelTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
////////////////////////////////////////////////////////

/////////////// FOR TOP /////////////////////////////
public:
	void	Set_TopCam_Look();
	void	Cam_View_CatmullRom_3rdToTop_Matrix(_double TimeDelta, _matrix* vMatrixStart = nullptr, _matrix* vMatrixEnd = nullptr);
	void	Cam_View_CatmullRom_3rdToTop(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_CamPos_Top_Temporary(_vector* pOut);
	void	Cam_View_Top(_double TimeDelta);
	void	Cam_View_CatmullRom_TopTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
//////////////////////////////////////////////////////////


///////////////// FOR TOP_B /////////////////////////////
public:
	void	Cam_View_CatmullRom_3rdToTopB(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_CamPos_TopB_Temporary(_vector* pOut);
	void	Cam_View_TopB(_double TimeDelta);
	void	Cam_View_CatmullRom_TopBTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
//////////////////////////////////////////////////////////

//////////////////FOR COASTER /////////////////////////////
public:
	void	Cam_View_CatmullRom_3rdToCoaster(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_CamPos_Coaster_Temporary(_vector* pOut);
	void	Cam_View_Coaster(_double TimeDelta);
	void	Cam_View_CatmullRom_CoasterTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_RsideToCoaster(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_Parameter_from_Coaster();
/////////////////////////////////////////////////////////////

///////////////////FOR STAIR ///////////////////////////////////
public:
	void	Cam_View_Stair(_double TimeDelta);
	void	Get_CamPos_Stair_Temporary(_vector* pOut);
	void	Cam_View_CatmullRom_3rdToStair(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_StairToCoaster(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);

	void	Set_MouseMoveCheckToZero();

////////////////////////////////////////////////////////////////////


//////////////// FOR CUTSCENE ///////////////////////////////////
public:
	void	Cam_View_CatmullRom_CutsceneTo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Set_CutscenePos(_vector vPos);
	void	Shake_Cam_Engels(_double TimeDelta);

/////////////////////////////////////////////////////////

////////////////////////////// FOR TOPVIEW /////////////////////////////////////////
public:
	void	Get_Parameter_from_Player_for_Top();
//////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// FOR 3D_UI //////////////////////////////////////////
public:
	void	Cam_View_UI(_double TimeDelta);
	void	Cam_View_CatmullRom_3rdToUI(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Cam_View_CatmullRom_UITo3rd(_double TimeDelta, _vector* vPointStart = nullptr, _vector* vPointEnd = nullptr);
	void	Get_CamPos_UI_Temporary(_vector* pOut);

////////////////////////////////////////////////////////////////////////////////////

public:
	void	Shake_Cam_Hit(_double TimeDelta);
	void	Shake_Cam_HitBig(_double TimeDelta);
	void	Shake_Cam_Gauntlet(_double TimeDelta);
	void	Check_Shake_Cam_State(_double TimeDelta);
	void	Check_FovChange(_double TimeDelta);



// For AfterImage Effects
public:
	const _float4&	Get_GoalPosition() const {
		return m_vGoalPosition; 
	}
	const _float4&	Get_LastPosition() const {
		return m_vLastPos;
	}

/////////////////////////////////////////////////////////
public:
	static	CMainCamera*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

public:
	// for Save Player`s Transform 
	CTransform*		m_pPlayerTransformCom = nullptr;
	CPlayer*		m_pPlayer = nullptr;
	_float			m_fY = 0.f;
	_float			m_fNowCamDistance = 5.f;
	_bool			m_bLockOn = false;
	OPTION			m_eOption = OPTION::FIXED;
	_float			m_fAccTime = 0.f;
	_float			m_fCamSpeed = 1.5f;
	_float4			m_vPosition;
	_bool			m_bOnce = false;
	_float			m_fRatioLock = 0.f;
	_bool			m_bReachEnd = false;
	_float4x4		m_Bone1;
	_bool			m_bShake = false;
	_float			m_fShakeTime = 0.f;
	//_double			m_ShakeTime = 0.f;
	_float			m_fShakeX = 0.f;
	_float			m_fShakeY = 0.f;
	_float			m_fShakeZ = 0.f;
	_float			m_fRatioFix = 0.f;

	_bool			m_bMouseMove = false;
	_bool			m_bCamMoving = false;
	_long			m_MouseMoveStateX = 0;
	_long			m_MouseMoveStateY = 0;
	_long			m_MouseMoveStateCheckY = 0;
	_long			m_MouseMoveStateCheckX = 0;
	_float4			m_vLastPosition = { 0.f,0.f,0.f,0.f };
	_float4			m_vTeleportPosition = { 0.f, 0.f , 0.f ,0.f };

	_float			m_fCamX = 0.f ;
	_float			m_fCamY = 0.f ;
	_float4			m_vFirstCamRight = { 0.f , 0.f , 0.f , 0.f };
	_float4			m_vFirstCamUp = { 0.f , 0.f , 0.f , 0.f };
	_float4			m_vFirstCamLook = { 0.f , 0.f , 0.f , 0.f };
	_float4			m_vFirstCamPos = { 0.f , 0.f , 0.f , 0.f };
	_bool			m_bOnce2 = false;

	_float4			m_vLastPos = { 0.f , 0.f , 0.f , 0.f };
	_float4			m_vLastPlayerPos = { 0.f , 0.f , 0.f , 0.f };
	_bool			m_bFirstCheck = false;

	_bool			m_bMouseLock = true;
	_float			m_fCamDistance = 5.f;
	_float			m_fCamDistance_RSideView = 10.f;
	_float			m_fCamDistance_LSideView = 10.f;
	_float			m_fCamDistance_TopView = 12.f;

	_bool			m_bShakeMiddle = false;
	_float			m_fShakeTimeM = 0.f;
	_float			m_fShakeXM = 0.f;
	_float			m_fShakeYM = 0.f;
	_float			m_fShakeZM = 0.f;

	_bool			m_bShakeEngels = false;
	_float			m_fShakeTimeE = 0.f;
	_float			m_fShakeXE = 0.f;
	_float			m_fShakeYE = 0.f;
	_float			m_fShakeZE = 0.f;

	_double			m_dHoldTime = 0.0;
	_bool			m_bHoldCam = false;
	_bool			m_bMovingCamOnLock = false;

	_bool			m_bCamViewing = true;

	_float4x4		m_TopViewMatrix;
	_float4x4		m_3rdViewMatrix;

	_bool			m_bCheckTopCamLookOnce = false;

	// for Coaster
	_float4x4		m_CoasterMatrix;
	_float4			m_vecCoasterLook;
	_float4			m_vecCoasterPosition;

	_float			m_fCamFar = 0.f;

	// for Right Cam
	_bool			m_bRightCheckOnce = false;
	_float4			m_vSetted_PlayerRight;

	// for Left Cam
	_bool			m_bLeftCheckOnce = false;
	_float4			m_vSetted_PlayerLeft;

	// for Cutscene Cam 
	_float4			m_vCutSceneCamPos;

	// for Top View
	_float4			m_vPlayerLook_for_Top;
	_bool			m_bCheck_Top_Once = false;

	//
	_double			m_dSwitchTime = 0.5;
	_double			m_dSwitchDuration = 0.0;

	// for UI VIEW
	_bool			m_bUIcheck = false;
	_bool			m_b3rdCheck = false;

	// for Shake Hit
	_bool			m_bShakeHit = false;
	_float			m_fShakeTimeH = 0.f;
	_float			m_fShakeXH = 0.f;
	_float			m_fShakeYH = 0.f;
	_float			m_fShakeZH = 0.f;

	// for Shake Hit Big 
	_bool			m_bShakeHitBig = false;
	_float			m_fShakeTimeHB = 0.f;
	_float			m_fShakeXHB = 0.f;
	_float			m_fShakeYHB = 0.f;
	_float			m_fShakeZHB = 0.f;

	// for Shake HIT Gauntlet
	_bool			m_bShakeHitGauntlet = false;
	_float			m_fShakeTimeG = 0.f;
	_float			m_fShakeXG = 0.f;
	_float			m_fShakeYG = 0.f;
	_float			m_fShakeZG = 0.f;

	// for ZoomIn Effect
	_float			m_fFov = 60.f;
	_float			m_fFovKatanaR = 60.f;
	_float			m_fFovLKatanaRush = 60.f;
	_float			m_fThrowSpear = 60.f;

	// for Fov
	_bool			m_bDodgeTime = false;
	

	///////////////////////////////////////////
	// For AfterImage Effects
	_float4			m_vGoalPosition = _float4(0.f, 0.f, 0.f, 1.f);
	/////////////////////////////////////////////


};

END
#endif // !__CLIENT_MAIN_CAMERA_H__