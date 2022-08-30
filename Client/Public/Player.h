#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"




#define SPEAR_ATTACK_NUM	6
#define WEAPON_EFFECT_CREATE_DELAY	0.7

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)
class CPlayer final : public CGameObjectModel
{
public:
	// For m_iFlagProgramChipState
	static const _uint	PROGRAM_INITIALIZE	= 0x00000000;
	static const _uint	PROGRAM_OS			= 0x00000001;
	static const _uint	PROGRAM_MACHINEGUN	= 0x00000002;
	static const _uint	PROGRAM_HANG		= 0x00000004;
	static const _uint	PROGRAM_LASER		= 0x00000008;
	static const _uint	PROGRAM_LIGHT		= 0x00000010;
	static const _uint	PROGRAM_ALL			= 0x0000001E;
	static const _uint	PROGRAM_PROTOTYPE	= 0x00000007;

public:
	enum class INPUTSTATE	{ INGAME, UI, NONE };
	
	enum class ANIMSTATE	{ IDLE, SLIDING, WALK, RUN, SPURT, AVOID, JUMP, JUMPLANDINGRUN, JUMPATTACK, JUMPPOWERATTACK,
							  ATTACK, POWERATTACK, LASERATTACK, HIT, JUSTAVOID, JUSTAVOID_COUNTERATTACK, NONE };

	enum class EQUIPSTATE	{ KATANA, LONGKATANA, SPEAR, GAUNTLET, NONE };

	enum class NAVISTATE	{ COASTER, NONE };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT					NativeConstruct_Prototype();
	virtual HRESULT					NativeConstruct(void* pArg);
	virtual _int					Tick(_double TimeDelta);
	virtual _int					LateTick(_double TimeDelta);
	virtual HRESULT					Render();

public:
	// Old Method
	void							Key_Check(_double TimeDelta);

	// New Method
	void							Key_Check_Common(_double TimeDelta);
	void							Key_Check_Ingame(_double TimeDelta);
	void							Key_Check_UI(_double TimeDelta);

	void							Check_Target();

public:
	void							Initialize_EquipState();

public:
	void							IdleCheck_State();
	void							Check_RunState(_double TimeDelta);
	void							Check_JumpState(_double TimeDelta, _bool& bJumpKeyDown, _bool& bPlayAnim);
	void							Check_AvoidState(_bool& bPlayAnim);

	void							Check_MoveRun(_bool& bPlayAnim, _double TimeDelta);
	void							Check_JumpAttack(_bool& bDown);
	void							Check_JumpPowerAttack(_bool& bDown);
	void							Check_LongKatana_JumpAttack(_bool& bDown);
	void							Check_WeaponState();
	void							Check_RestTime(_double TimeDelta);
	void							Check_AndroidDistance();

public:
	void							Set_Target_ForPod(_vector vPos);

public:
	void							Set_BulletTime(_bool bBulletTime, _double BulletLevel, _double BulletTime);

public:
	void							Update_Pod();
	void							Update_DataFor9S(const ANDROID_DATAPACKET& tDataPacket);

public:
	void							IsLButton_SetFalse();
	void							IsRButton_SetFalse();

public:
	void							LerpToHitTarget(_double TimeDelta);
	void							LerpToTargetLook(_double TimeDelta);

public:
	void							LerpToLaserLook(_double TimeDelta);
	void							LerpToCameraLook(_double TimeDelta);
	void							Check_LerpTime(_bool& bMove);
	void							Rotate_Camera_Left(_double TimeDelta);
	void							Rotate_Camera_Right(_double TimeDelta);
	void							Rotate_Camera_DiagonalLeft(_double TimeDelta);
	void							Rotate_Camera_DiagonalRight(_double TimeDelta);
	void							Rotate_Camera_DiagonalBackLeft(_double TimeDelta);
	void							Rotate_Camera_DiagonalBackRight(_double TimeDelta);
	void							LerpToLeftofCenter(_double TimeDelta);
	void							LerpToRightofCenter(_double TimeDelta);
	void							LerpToLeftofCamera(_double TimeDelta);
	void							LerpToRightofCamera(_double TimeDelta);

public:
	void							JumpMoveForTick(_double TimeDelta, _bool& bPlayAnim, _bool& bCheck);

public:		
	void							Set_Katana(class CKatana* pKatana);
	void							Set_LongKatana(class CLongKatana* pLongKatana);
	void							Set_Spear(class CSpear* pSpear);
	void							Set_Gauntlet(class CGauntlet* pGauntlet);
	void							Set_Pod(class CPod* pPod);
	void							Set_LaserAttack(_bool bCheck) { m_bLaserAttack = bCheck; }
	void							Set_Android9S(class CAndroid_9S* pAndroid);
	void							Set_Targeting_False() { m_bTargeting = false; }
	void							Set_CamOption(_uint iOption) { m_iCamOption = iOption; }
	void							Set_iAnimation(_uint iIndex) { m_iAnimation = iIndex; }

public:
	void							Set_StartCamLeft();
	void							Set_StartCamRight();
	void							Set_StartCamDiagonalLeft();
	void							Set_StartCamDiagonalRight();
	void							Set_StartCamDiagonalBackLeft();
	void							Set_StartCamDiagonalBackRight();

private:
	void							Compute_DOF_Parameters() override;

public:
	void							Set_FixCamRight(_vector vFixRight) { XMStoreFloat3(&m_vFixCamRight, vFixRight); }
	DIRECTION						Result_FixRightAndLook();

public:
	_bool							Get_Spurt() { return m_bSpurt;}
	CGameObject*					Get_Target() { return m_pTarget; }
	_bool							Get_Targeting() { return m_bTargeting; }
	_bool							Get_LaserAttack() const { return m_bLaserAttack; }
	_bool							Get_Jump() { return m_bJump; }
	ANIMSTATE						Get_CurPlayerAnimState() const { return m_eAnimState; }
	_uint							Get_CurPlayerAnimIndex() const { return m_pModel->Get_CurrentAnimation(); }
	_vector							Get_CameraLook();
	_bool							Get_WASDKey() const { return m_bWASDKey; }
	_uint							Get_EquipStateMain() { return (_uint)m_eEquipState; }
	_uint							Get_EquipStateSub() { return (_uint)m_eEquipStateSub; }
	_uint							Get_AnimState() { return (_uint)m_eAnimState; }

public:
	void							Set_EquipWeaponState(_uint iSlot, EQUIPSTATE eEquipState);

	void							Set_KatanaIdle();
	void							Set_LongKatanaIdle();
	void							Set_SpearIdle();
	void							Set_GauntletIdle();

public:
	void							Set_EquipWeaponIdle();
	void							Check_JumpState_ForHit(_double TimeDelta);
	void							Check_AvoidAnimation();

public:
	void							Initialize_HangOn();

	/*For. NaviState */
public:
	void							Set_NaviState(NAVISTATE eState) { m_eNaviState = eState; }

public:
	void							ForCoaster();

public:
	NAVISTATE						Get_NaviState() { return m_eNaviState; }

public:
	void							Set_PositionForCoaster(_double TimeDelta);

	// For. Unbeatable Time
public:		
	void							Update_UnbeatableTime(_double TimeDelta);

public:
	void							Initialize_WASD_Key() { m_bWASDKey = false; }


public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

	HRESULT							SetUp_Observer();

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

/////////////////////////////////////////////////////////////////////
/////////// For Effects /////////////////////////////////////////////

private:
	void							Update_Effects(_float _fTimeDelta);
	void							Update_Weapon_Decoration_Effects(_double _dTimeDelta);
	void							Update_PlayerSRV();
	void							Create_SpurtAfterImage();
	void							Create_FlipAfterImage();

private:
	ID3D11ShaderResourceView*		m_pPlayerSRV = nullptr;
private:
	_float							m_fAfterImageGenTimes = 0.f;
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

private: 
	void							Update_Alpha(_double dTimeDelta);
	
private:
	_uint							m_iAnimation = 0;
	_bool							m_bJump = false;
	_bool							m_bContinue = false;
	ANIMSTATE						m_eAnimState;
	DIRECTION						m_eDirection = DIRECTION::FORWARD;

private:
	_bool							m_bWASDKey = false;
	_bool							m_bHangOn = false;

private:
	_float3							m_vStartLeft = _float3(0.f, 0.f, 0.f);
	_float3							m_vStartRight = _float3(0.f, 0.f, 0.f);
	_float3							m_vStartDiagonalLeft = _float3(0.f, 0.f, 0.f);
	_float3							m_vStartDiagonalRight = _float3(0.f, 0.f, 0.f);
	_float3							m_vStartDiagonalBackLeft = _float3(0.f, 0.f, 0.f);
	_float3							m_vStartDiagonalBackRight = _float3(0.f, 0.f, 0.f);

private: /* For. Key */	
	_bool							m_bLButtonDown = false;
	_bool							m_bRButtonDown = false;

private:
	_bool							m_bSpearJumpAttackFinal = false;
	_bool							m_bGauntletJumpAttackFinal = false;

private:
	_bool							m_bSpurt = false;
	_bool							m_bRun = false;
	_bool							m_bWalk = false;

	// For. TargeLock
	_bool							m_bTargeting = false;
	CGameObject*					m_pTarget = nullptr;

private:
	class CKatana*					m_pKatana = nullptr;
	class CLongKatana*				m_pLongKatana = nullptr;
	class CPod*						m_pPod = nullptr;
	class CSpear*					m_pSpear = nullptr;
	class CGauntlet*				m_pGauntlet = nullptr;
	EQUIPSTATE						m_eEquipState = EQUIPSTATE::KATANA;
	EQUIPSTATE						m_eEquipStateSub = EQUIPSTATE::LONGKATANA;
	_uint							m_iFlagProgramChipState = CPlayer::PROGRAM_PROTOTYPE;
	//_uint							m_iFlagProgramChipState = CPlayer::PROGRAM_OS;

	_bool							m_dIsSwitchingReady = true;
	_double							m_dSwitchingCoolTimeAcc = 0.0;
	const _double					m_dSwitchingCoolTimeAccMax = 2.3;

public:
	const _bool						Switch_EquipState();
	const _uint&					Get_iFlagProgramChipState() const { return m_iFlagProgramChipState; }
	const _bool						Is_ProgramChipInstalled(_uint iChipFlag);
	const _bool						Is_ValidProgramChip(_uint iChipFlag);
	void							Install_ProgramChip(_uint iChipFlag);
	void							Uninstall_ProgramChip(_uint iChipFlag);

private:
	_bool							m_bIsSysMsgSendOut = false;


private:
	class CAndroid_9S*				m_pAndroid9S = nullptr;


private:
	CGameObject*					m_pChargingEffect = nullptr;

private:
	INPUTSTATE						m_eCrntInputState = INPUTSTATE::INGAME;
	INPUTSTATE						m_ePrevInputState = INPUTSTATE::INGAME;

private:
	_double							m_RestTime = 0.0;
	_double							m_RestDelayTime = 3.0;

private:
	_double							m_SpacePressingTime = 0.0;

private:	// For. Charging Attack
	_bool							m_bCharge = false;

	// For Test
	//_uint							m_iMaxHP = 200;
	//_int							m_iCrntHP = 200;

	// For Collision()
	_uint							m_iGetDamage = 0;

private:
	// For UI
	_double							m_dCallHPBarUITimeDuration = 3.0;
	_double							m_dCallTargetUITimeDuration = 3.0;

private:	// For. Laser Attack
	_bool							m_bLaserAttack = false;
	_bool							m_bLaserShootStart = false;

private:
	_double							m_LookLerpTime = 0.0;
	_float3							m_vPreCamLook = _float3(0.f, 0.f, 0.f);
	_float3							m_vPreCamLeft = _float3(0.f, 0.f, 0.f);
	_float3							m_vPreCamRight = _float3(0.f, 0.f, 0.f);

private:
	_float4							m_vTargetPos = _float4(0.f, 0.f, 0.f, 0.f);
	_double							m_TargetLookLerpTime = 0.0;

private:
	_double							m_LaserLookLerpTime = 0.0;
	_float3							m_vPreLaserLook = _float3(0.f, 0.f, 0.f);

	// For Debug Debug Debug Debug Debug Debug Debug Debug Debug
	_bool							m_bDebugNavigation = false;
	// For Debug Debug Debug Debug Debug Debug Debug Debug Debug

	class CInventory*				m_pInventory = nullptr;


	// For LongKatana Trail delay
private:
	_float							m_fLongKatanaTrail_Dealy = 0.f;
	_bool							m_bLongKatanaTrail_Create = false;

private: 
	// For. Hit
	_float4							m_vHitTargetPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float4							m_vCounterAttPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double							m_HitLerpTime = 0.0;

private:
	_bool							m_bBulletTime = false;

private:
	// For. Unbeatable 작업 예정
	_double							m_UnbeatableTime = 0.0;
	_double							m_UnbeatableDelayTime = 1.0;

private:
	_uint							m_iCamOption = 0;

private:
	_float3							m_vFixCamRight = _float3(0.f, 0.f, 0.f);

private:
	NAVISTATE						m_eNaviState = NAVISTATE::NONE;

private:
	_float3							m_vRimLightColor = _float3(0.f, 0.f, 0.f);
	void							Set_RimLightColor(const _float3& _vColor) {
		m_vRimLightColor = _vColor;
	}

private:
	_bool							m_arrIsSpearAttackEffectCreated[SPEAR_ATTACK_NUM];

private:
	_double							m_dKatanaEffectCreateDelay = 0.0;
	_double							m_dLongKatanaEffectCreateDelay = 0.0;
	_double							m_dSpearEffectCreateDelay = 0.0;



public:
	//for Camera
	_bool							m_bSceneChange = false;
	void							Set_SceneChangeTrue() { m_bSceneChange = true; }
	_bool							Get_SceneChange() { return m_bSceneChange; }
	_bool							m_bPressingTab = false;
	_bool							Get_PressingTab() { return m_bPressingTab; }

public:
	// For. Scene Change
	HRESULT							SceneChange_AmusementPark();
	HRESULT							SceneChange_OperaBackStage();
	HRESULT							SceneChange_ZhuangziStage();
	HRESULT							SceneChange_ROBOTGENERAL();

private:
	// For. Shadow Light
	void							Shadow_BeTraced();

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_ConstantTable();
	HRESULT							SetUp_Bone1();

public:
	// For. TopView
	_float4							m_vPlayerLook;
	_float4							Get_PlayerLook() { return m_vPlayerLook; }
	void							SetUp_PlayerLook();
	
private:
	HRESULT							Create_ChargingEffect();
	void							Delete_ChargingEffect();
	HRESULT							Create_CounterAttackEffect();


private:
	_float4							m_vPlayerDOF = _float4(0.f, 0.f, 0.f, 1.f);
	_float							m_fDOFNear = 0.0f;
	_float							m_fDOFFar = 0.0f;
	_float							m_fDOFFocus = 0.0f;

	void							Calculate_Target_DOF(CGameInstance* pGameInstance);
	void							Calculate_Player_DOF(CGameInstance* pGameInstance);
	void							Calculate_CutScene_DOF(CGameInstance* pGameInstance);
	void							Calculate_DepthOfField(CGameInstance* pGameInstance, _double dTimeDelta);

private:
	int								m_iScale = 1;
	_bool							m_bPressingR = false;
	_bool							m_bKatanaR = false;
	_bool							m_bThrowSpear = false;
	_bool							m_bLKatanaRush = false;
	_bool							m_bGauntletSpin = false;
	_bool							m_bDodge = false;

public:
	_bool							Get_PressingR() { return m_bPressingR; } // LongKatana Charge
	_bool							Get_KatanaR() { return m_bKatanaR; } // Katana RB click
	_bool							Get_ThrowSpear() { return m_bThrowSpear; } // 
	_bool							Get_LKatanaRush() { return m_bLKatanaRush; }
	_bool							Get_GauntletSpin() { return m_bGauntletSpin; }
	_bool							Get_Dodge() { return m_bDodge; }

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END