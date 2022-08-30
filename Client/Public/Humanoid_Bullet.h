#pragma once
#ifndef __CLIENT_HUMANOID_BULLET__
#define __CLIENT_HUMANOID_BULLET__

#include "GameObjectModel.h"
#include "Player.h"

BEGIN(Client)
class CHumanoid_Bullet final : public CGameObjectModel
{
public:
	enum class BULLET : _uint
	{
		NORMAL = 1,
		LASER = 2,
		NONE = 999
	};

private:
	CHumanoid_Bullet(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CHumanoid_Bullet(const CHumanoid_Bullet& rhs);
	virtual ~CHumanoid_Bullet() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta);
	virtual _int				LateTick(_double dTimeDelta);
	virtual HRESULT				Render();
	void						Set_Transform(_float4x4 Matrix);
	HRESULT						Set_Collider();
	void						Set_Target();
	void						Change_Transform();

public:
	virtual void					Notify(void* pMessage);

protected:
	virtual _int					VerifyChecksum(void* pMessage);

public:
	static	CHumanoid_Bullet*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;

private:
	class CPlayer*	m_pPlayer;
	CTransform*		m_pPlayerTransformCom;

	BULLET			m_eBulletType = BULLET::NONE;
	_float			m_fSpeed = 10.f;
	_double			m_dContinuingTime = 0.0;	
};
END

#endif // !__CLIENT_HUMANOID_BULLET__
