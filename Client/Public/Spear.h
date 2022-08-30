#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)
class CSpear final : public CGameObjectModel
{
	static const _double POWERATTACK_NONLERP_RATIO;
	static const _double POWERATTACK_RESTART_LERP_RATIO;
	static const _double ONETICK_TIME;

private:
	CSpear(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpear(const CSpear& rhs);
	virtual ~CSpear() = default;

public:
	virtual HRESULT				NativeConstruct_Prototype();
	virtual HRESULT				NativeConstruct(void* pArg);
	virtual _int				Tick(_double TimeDelta);
	virtual _int				LateTick(_double TimeDelta);
	virtual HRESULT				Render();

public:
	virtual void				Update_HitType() override;
	virtual HRESULT				Update_Collider() override;

public:
	void						Set_Position_Immediately(_vector vPos);

	void						Set_Teleport(_bool bCheck);
	void						Set_Appearance(_bool bCheck) { m_bAppearance = bCheck; }
	void						Set_Alpha(const _float& fAlpha) { m_fAlpha = fAlpha; }
public:
	void						Check_Teleport(_double TimeDelta);
	void						Check_Independence();

public:
	void						Update_Matrix(_matrix Matrix);

public:
	void						Set_WeaponState(WEAPONSTATE eState) { m_eWeaponState = eState; }
	void						Set_Animation(_uint iIndex);
	void						Set_Immediately_Animation(_uint iIndex);
	void						Set_NoneBlend(_uint iIndex);
	void						Set_OwnerLayerTag(const _tchar* pTag) { lstrcpy(m_szOwnerLayerTag, pTag); }
	void						Set_WeaponEquip(WEAPONEQUIP eEquip);
	void						Set_WeaponEquipFor9S(WEAPONEQUIP eEquip) { m_eWeaponEquip = eEquip; }

public:
	void						Set_Att(_uint iAtt) { m_tObjectStatusDesc.iAtt = iAtt; };
	void						Set_Independence(_bool bCheck) { m_bIndependence = bCheck; }
	void						Set_PlayerNonLerp(_bool bCheck) { m_bPlayerNonLerp = bCheck; }

public:
	WEAPONSTATE					Get_WeaponState() const { return m_eWeaponState; }
	WEAPONEQUIP					Get_WeaponEquip() const { return m_eWeaponEquip; }

public:
	_bool						IsIndependence() const { return m_bIndependence; }
	_bool						Get_PlayerNonLerp() const { return m_bPlayerNonLerp; }

public:
	// For Collision
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	HRESULT						Create_Spark();
public:
	HRESULT						Create_Decoration_Effect();
	void						Delete_Decoration_Effect();
	BOOL						Get_IfEffectIs();

private:
	void						Update_Effects();

private:
	void						Update_Decoration_Effect_Transform();
	void						Update_Decoration_Effect_Redner();

private:
	CGameObject*				m_pDecorationEffect = nullptr;

public:
	HRESULT						Create_AttackEffect();
	HRESULT						Create_LongAttackEffect();
	void						Delete_LongAttackEffect();

public:
	void						Update_LongAttackEffect();

private:
	void						Update_PrevPosition();

private:
	HRESULT						SetUp_Components();

private:
	_float4						m_vPrevPostion = _float4(0.f, 0.f, 0.f, 1.f);

private:
	_bool						m_bIndependence = false;
	_bool						m_bPlayerNonLerp = false;

private:
	_bool						m_bTeleport = false;
	_bool						m_bAppearance = false;

private:
	_uint						m_iAnimation = 0;
	WEAPONSTATE					m_eWeaponState = WEAPONSTATE::IDLE;
	WEAPONEQUIP					m_eWeaponEquip = WEAPONEQUIP::NONEQUIP;

private:
	_tchar						m_szOwnerLayerTag[MAX_PATH];



private:
	CGameObject*				m_pLongAttackEffect = nullptr;

public:
	// For. Scene Change
	HRESULT							SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ZhuangziStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT							SceneChange_ROBOTGENERAL(const _vector&  vPosition, const _uint& iNavigationIndex);

public:
	static CSpear*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free();
};

END