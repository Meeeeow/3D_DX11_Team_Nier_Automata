#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

class CGauntlet final : public CGameObjectModel
{
private:
	CGauntlet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGauntlet(const CGauntlet& rhs);
	virtual ~CGauntlet() = default;

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
	void						Condition_UpdateCollider();

public:
	void						Update_Matrix(_matrix Matrix);

public:
	void						Set_WeaponState(WEAPONSTATE eState) { m_eWeaponState = eState; }
	void						Set_Animation(_uint iIndex) { m_iAnimation = iIndex; }
	void						Set_Immediately_Animation(_uint iIndex);
	void						Set_NoneBlend(_uint iIndex);
	void						Set_StopTime(_double TimeDelta) { m_StopTime += TimeDelta; }
	void						Set_Alpha(const _float& fAlpha) { m_fAlpha = fAlpha; }
	void						Set_WeaponEquip(WEAPONEQUIP eEquip);

public:
	WEAPONSTATE					Get_WeaponState() const { return m_eWeaponState; }
	_uint						Get_JumpAttCount() const { return m_iJumpAttCount; }
	_double						Get_StopTime() const { return m_StopTime; }
	WEAPONEQUIP					Get_WeaponEquip() const { return m_eWeaponEquip; }

public:
	void						Add_JumpAttCount() { ++m_iJumpAttCount; }
	void						Initialize_JumpAttCount() { m_iJumpAttCount = 0; }
	void						Initialize_StopTime() { m_StopTime = 0.0; }

public:
	// For Collision
	virtual _bool				Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void				Collision(CGameObject* pGameObject) override;

public:
	virtual void				Notify(void* pMessage)			override;

protected:
	virtual _int				VerifyChecksum(void* pMessage)	override;

private:
	HRESULT						SetUp_Components();

private:
	_uint						m_iAnimation = 0;
	WEAPONSTATE					m_eWeaponState = WEAPONSTATE::IDLE;
	WEAPONEQUIP					m_eWeaponEquip = WEAPONEQUIP::NONEQUIP;

private:
	_uint						m_iJumpAttCount = 0;
	_double						m_StopTime = 0.0;

private:
	const _tchar*				m_pBoneTag = L"Left";

public:
	// For. Scene Change
	HRESULT						SceneChange_AmusementPark(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT						SceneChange_OperaBackStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT						SceneChange_ZhuangziStage(const _vector& vPosition, const _uint& iNavigationIndex);
	HRESULT						SceneChange_ROBOTGENERAL(const _vector& vPosition, const _uint& iNavigationIndex);

public:
	static CGauntlet*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free();
};

END

