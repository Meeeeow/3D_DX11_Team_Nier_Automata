#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Engine)
class CNavigation;
END

#define LONGKATANA_TRAIL_NUMBER				10


BEGIN(Client)

static const _double ONETICK_TIME = 0.016;

class CLongKatana final : public CGameObjectModel
{
	static const _double		LONGKATANA_POWERATTACK3_NONLERP_RATIO;
	static const _double		LONGKATANA_POWERATTACK3_RESTART_LERP_RATIO;
public:
	enum DIR { LEFT, RIGHT, NONE };
private:
	CLongKatana(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLongKatana(const CLongKatana& rhs);
	virtual ~CLongKatana() = default;

public:
	virtual HRESULT			NativeConstruct_Prototype();
	virtual HRESULT			NativeConstruct(void* pArg);
	virtual _int			Tick(_double TimeDelta);
	virtual _int			LateTick(_double TimeDelta);
	virtual HRESULT			Render();

public:
	virtual void			Update_HitType() override;
	virtual HRESULT			Update_Collider() override;

public:
	void					Update_Matrix(_matrix Matrix);

public:
	_uint					Get_AnimationIndex() const { return m_iAnimation; }
	WEAPONSTATE				Get_WeaponState() const { return m_eWeaponState; }
	WEAPONEQUIP				Get_WeaponEquip() const { return m_eWeaponEquip; }
	const _bool&			Get_Independence() const { return m_bIndependence; }

public:
	void					Set_Animation(_uint iIndex);
	void					Set_Alpha(const _float& fAlpha) { m_fAlpha = fAlpha; }
public:
	void					Set_WeaponState(WEAPONSTATE eState) { m_eWeaponState = eState; }	
	void					Set_WeaponEquip(WEAPONEQUIP eEquip);
	void					Set_Independence(_bool bCheck) { m_bIndependence = bCheck; }
	void					Set_PlayerNonLerp(_bool bCheck) { m_bPlayerNonLerp = bCheck; }

public:
	const _bool&			IsIndependence() const;
	const _bool&			IsPlayerNonLerp() const { return m_bPlayerNonLerp; }
	void					Check_Independence(_double TimeDelta);

public:
	_uint					Get_ModelCurrentAnim();
	_double					Get_PlayTimeRatio();

public:
	// For Collision
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;

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

private:
	CGameObject*			m_pTrail = nullptr;

public:
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
	_bool					m_bIndependence = false;
	_bool					m_bPlayerNonLerp = false;

private:
	HRESULT					SetUp_Components();
	HRESULT					SetUp_ConstantTable();

private:
	CCollider*				m_pSphere = nullptr;
	CNavigation*			m_pNavigation = nullptr;

private:
	_uint					m_iAnimation = 0;
	WEAPONSTATE				m_eWeaponState = WEAPONSTATE::IDLE;
	WEAPONEQUIP				m_eWeaponEquip = WEAPONEQUIP::EQUIP;

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
	const DISSOLVE_STATE&	Get_DissolveState() {
		return m_eDissolveState;
	}


public:
	// For. Scene Change
	HRESULT					SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT					SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT					SceneChange_ZhuangziStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT					SceneChange_ROBOTGENERAL(const _vector& vPosition, const _uint& iNavigationIndex);

public:
	static CLongKatana*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free();
};

END

