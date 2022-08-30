#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

#define MAXMOVE 10
#define KATANA_TRAIL_NUMBER					10

static const _uint KATANA_IDLE = 0;

static const _uint KATANA_ATTACK1 = 40;
static const _uint KATANA_ATTACK2 = 60;
static const _uint KATANA_ATTACK3 = 4;
static const _uint KATANA_ATTACK4 = 5;
static const _uint KATANA_ATTACK5 = 6;
static const _uint KATANA_ATTACK6 = 7;
static const _uint KATANA_ATTACK7 = 8;
static const _uint KATANA_ATTACK8 = 30;

static const _uint KATANA_POWERATTACK1 = 35;
static const _uint KATANA_POWERATTACK2 = 9;

static const _uint KATANA_JUMPATTACK1 = 28;
static const _uint KATANA_JUMPATTACK2 = 66;
static const _uint KATANA_JUMPATTACK3 = 24;

static const _uint KATANA_JUMPPOWERATTACK = 62;
static const _uint KATANA_JUMPPOWERATTACK_DW = 49;
static const _uint KATANA_JUMPPOWERATTACK_END = 22;

static const _double KATANA_ATTACK3_NONLERPTIME = 0.15;
static const _double KATANA_ATTACK4_NONLERPTIME = 0.128;
static const _double KATANA_ATTACK4_NONLERPTIME2 = 0.24;
static const _double KATANA_ATTACK5_NONLERPTIME = 0.11;
static const _double KATANA_ATTACK5_NONLERPTIME2 = 0.3;
static const _double KATANA_ATTACK6_NONLERPTIME = 0.104;

static const _double KATANA_JUMPATTACK3_NONLERPTIME = 0.2;

static const _double KATANA_DROPSPEED = 10.f;

static const _uint	 KATANA_ATTACK_TARGETANIMATION = 5000;




class CKatana final : public CGameObjectModel
{
public:
	enum DIR { LEFT, RIGHT, NONE };




protected:
	CKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CKatana(const CKatana& rhs);
	virtual ~CKatana() = default;

public:
	virtual HRESULT		NativeConstruct_Prototype();
	virtual HRESULT		NativeConstruct(void* pArg);
	virtual _int		Tick(_double TimeDelta);
	virtual _int		LateTick(_double TimeDelta);
	virtual HRESULT		Render();

public:
	void Set_WeaponState(WEAPONSTATE eState) {
		m_eWeaponState = eState;
	}

	void Set_WeaponEquip(WEAPONEQUIP eEquip);

public:
	WEAPONSTATE Get_WeaponState() const { return m_eWeaponState; }
	WEAPONEQUIP Get_WeaponEquip() const { return m_eWeaponEquip; }

public:
	virtual void			Update_HitType();

public:
	_uint					Get_AnimationIndex() const { return m_iAnimation; }
	_double					Get_PlayTimeRatio() const;
	
public:
	void					Set_Animation(_uint iIndex) { m_iAnimation = iIndex; }

public:
	void					Set_Immediately_Animation(_uint iIndex);

public:
	void					Update_Matrix(_matrix Matrix);

public:
	void					Initialize_WaitTime() { m_WaitTime = 0.0; }

public:
	const _bool				IsNonLerpTime() const;

public:
	const _bool				IsIndependence() const;
	void					Set_Independence(_bool bCheck) { m_bIndependence = bCheck; }
	void					Check_Independence(_double TimeDelta);
	void					Go_Direction(_double TimeDelta);

public:
	// For Collision
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;
	void					Set_Alpha(const _float& fAlpha) { m_fAlpha = fAlpha; }
public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

private:
	HRESULT					Create_Spark();
public:
	HRESULT					Create_Decoration_Effect();
	void					Delete_Decoration_Effect();
	BOOL					Get_IfEffectIs();
	void					Start_Trail();
	void					End_Trail();

private:
	void					Update_Effects(_double _dTimeDelta);
	void					Update_Decoration_Effect_Transform();
	void					Update_Decoration_Effect_Redner();
	void					Update_Trail();

private:
	CGameObject*			m_pDecorationEffect = nullptr;

private:
	_float4					m_vPrevPostion = _float4(0.f, 0.f, 0.f, 1.f);

private:
	_double					m_WaitTime = 0.0;

private:
	WEAPONSTATE				m_eWeaponState = WEAPONSTATE::IDLE;
	WEAPONEQUIP				m_eWeaponEquip = WEAPONEQUIP::EQUIP;

private:
	_uint					m_iAnimation = KATANA_IDLE;

private:
	CGameObject*			m_pTrail = nullptr;

private:
	_bool					m_bIndependence = false;

private:
	_float					m_fDissolveVal = 0.f;
	_float					m_fDissolveSpeed = WEAPON_DISSOLVE_SPEED;
	DISSOLVE_STATE			m_eDissolveState = DISSOLVE_STATE::DISSOLVE_IDLE;
	CTexture*				m_pDissolveTexture = nullptr;


public:
	void					Set_Dissolve() {
		m_eDissolveState = DISSOLVE_STATE::DISSOLVE_UP;
	}
	void					Set_DissolveIdle() {
		m_eDissolveState = DISSOLVE_STATE::DISSOLVE_IDLE;
		m_fDissolveVal = 0.f;
	}
	const DISSOLVE_STATE&			Get_DissolveState() {
		return m_eDissolveState;
	}


private:
	HRESULT					SetUp_Components();

public:
	// For. Scene Change
	HRESULT							SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ZhuangziStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ROBOTGENERAL(const _vector& vPosition, const _uint& iNavigationIndex);

public:
	static CKatana* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END