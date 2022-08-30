#pragma once

#ifndef __CLIENT_IRONGAUNTLET_H__
#define __CLIENT_IRONGAUNTLET_H__

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

static const _uint IRONGAUNTLET_PUNCH1 = 9;
static const _uint IRONGAUNTLET_PUNCH2 = 37;
static const _uint IRONGAUNTLET_PUNCH3 = 43;

static const _uint IRONGAUNTLET_KICK1 = 10;
static const _uint IRONGAUNTLET_DASH_KICK = 26;

class CIronGauntlet final : public CGameObjectModel
{
public:
	CIronGauntlet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CIronGauntlet(const CIronGauntlet& rhs);
	virtual ~CIronGauntlet() DEFAULT;

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
	void						Set_Alpha(const _float& fAlpha) { m_fAlpha = fAlpha; }

public:
	WEAPONSTATE					Get_WeaponState() const { return m_eWeaponState; }
	_uint						Get_JumpAttCount() const { return m_iJumpAttCount; }

public:
	void						Add_JumpAttCount() { ++m_iJumpAttCount; }
	void						Initialize_JumpAttCount() { m_iJumpAttCount = 0; }

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

private:
	_uint						m_iJumpAttCount = 0;

private:
	const _tchar*				m_pBoneTag = L"Left";

public:
	static CIronGauntlet*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free();
};

END

#endif