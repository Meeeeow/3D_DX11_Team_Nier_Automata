#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

#include "Spear.h"
#include "Pod9S.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CAndroid_9S final : public CGameObjectModel
{
	static const _uint			ANIM_9S_HACKING = 47;
public:
	enum class ANIMSTATE  { IDLE, SLIDING, WALK, RUN, SPURT, AVOID, ATTACK, POWERATTACK, JUMP, JUMPLANDING, HIT, NONE };
	enum class ANDROIDSTATE { IDLE, BATTLE, HACKING, NONE };
	enum class NAVISTATE { COASTER, NONE };
private:
	CAndroid_9S(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAndroid_9S(const CAndroid_9S& rhs);
	virtual ~CAndroid_9S() = default;
public:
	virtual HRESULT					NativeConstruct_Prototype();
	virtual HRESULT					NativeConstruct(void* pArg);
	virtual _int					Tick(_double TimeDelta);
	virtual _int					LateTick(_double TimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_AndroidState(ANDROIDSTATE eState);
	void							Set_Idle();

public:
	ANDROIDSTATE					Get_AndroidState() const { return m_eAndroidState; }
	NAVISTATE						Get_NaviState() const { return m_eNaviState; }

public:
	void							Set_PositionForCoaster();
	void							Set_AnimationForCoaster();

public:
	_bool							IsOpenable_MiniGame();

public:
	HRESULT							Open_MiniGame();

public:
	void							Check_Player();
	void							Check_State(_double TimeDelta);
	void							Check_Target();		// Å¸°Ù Ã£±â
	void							Chase_Target(_double TimeDelta);
	void							Find_Target();
	void							Look_Target(_double TimeDelta);
	void							Update_Pod();
	void							Update_Pod_Immediately();
	void							Update_Weapon_Immediately();
	void							Key_Check();
	void							Check_HackingState(_double TimeDelta);
	void							LerpLookToBoss(_double TimeDelta);

public:
	void							Set_Fix(_bool bCheck);

public:
	virtual HRESULT					Update_Collider();

public:
	void							LerpToHitTarget(_double TimeDelta);
	void							Check_JumpState_ForHit(_double TimeDelta);
	void							Check_HitState(_double TimeDelta);
	void							Check_AndroidDataPacket(const ANDROID_DATAPACKET& tDataPacket);

public:
	void							Update_UnbeatableTime(_double TimeDelta);

public:
	void							Set_EquipWeaponIdle();

/////////////////	For. Teleport	///////////////////
public:
	void							Set_Teleport(_bool bCheck);
	void							Set_Teleport_CurrentIndex(const _uint& iIndex);
	void							Set_NoneTeleport(_bool bCheck) { m_bNoneTeleport = bCheck; }
	void							Set_TeleportBool(_bool bCheck) { m_bTeleport = bCheck; }

public:
	const _bool&					Get_Teleport() const { return m_bTeleport; }

//////////////////////////////////////////////////////
public:
	void							Check_WeaponState();
	void							Check_Teleport(_double TimeDelta);
	void							Check_UpdateTime(_double TimeDelta);
	void							Check_JumpState(_double TimeDelta);

public:
	void							Set_Spear(class CSpear*	pSpear);
	void							Set_Pod(class CPod9S* pPod);

public:
	void							Set_NaviState(NAVISTATE eState) { m_eNaviState = eState; }

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

private:
	HRESULT							Create_TeleportEffect();

private:
	HRESULT							SetUp_Components();
	HRESULT							SetUp_Observer();

private:
	_uint							m_iAnimation = 0;
	ANIMSTATE						m_eAnimState = ANIMSTATE::IDLE;
	ANDROIDSTATE					m_eAndroidState = ANDROIDSTATE::IDLE;

private:
	_float4							m_vDestPos;
	_bool							m_bLookLerp = false;
	_double							m_LookLerpTime = 0.0;
	_double							m_HitLerpTime = 0.0;
	_float4							m_vHitTargetPos = _float4(0.f, 0.f, 0.f, 0.f);

private:
	_double							m_BossLerpTime = 0.0;

private:
	ANDROID_DATAPACKET				m_tAndroidDataPacket;
	_double							m_UpdateTime = 0.0;
	_double							m_UpdateDelayTime = 0.4;

private:
	_bool							m_bContinue = false;

private:
	_bool							m_bWalk = false;
	_bool							m_bRun = false;
	_bool							m_bSpurt = false;
	_bool							m_bDoubleJump = false;

private:
	_bool							m_bTeleport = false;
	_bool							m_bAppearance = false;

private:
	_double							m_dCallHPBarUITimeDuration = 0.0;
	_uint							m_iGetDamage = 0;

private:
	_bool							m_bNoneTeleport = false;

private:
	_bool							m_bHackingOnce = false;
	_bool							m_bHackingFirstSound = false;

private:
	_double							m_UnbeatableTime = 0.0;
	_double							m_UnbeatableDelayTime = 0.5;

private:
	_bool							m_bFix = false;

private:
	class CSpear*					m_pSpear = nullptr;
	class CPod9S*					m_pPod = nullptr;

private:
	CGameObject*					m_pCurrentTarget = nullptr;

private:
	NAVISTATE						m_eNaviState = NAVISTATE::NONE;

public:
	// For. Scene Change
	HRESULT							SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ZhuangziStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ROBOTGENERAL(const _vector& vPosition, const _uint& iNavigationIndex);

public:
	static CAndroid_9S* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END