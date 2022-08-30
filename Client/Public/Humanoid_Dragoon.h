#pragma once
#ifndef __CLIENT_HUMANOID_DRAGOON__
#define __CLIENT_HUMANOID_DRAGOON__

#include "GameObjectModel.h"

BEGIN(Client)
class CHumanoid_Dragoon final : public CGameObjectModel
{
public:
	enum class M_ANIMSTATE : _uint
	{
		IDLE = 33,
		WALK = 139,
		ENGAGE = 147,
		SHOOTING = 998,
		BACKJUMP = 89,
		BACKSTEP = 137,
		FRONTRUN = 210,
		RIGHTSTEP = 237,
		LEFTSTEP = 236,
		ANGRY = 72,
		NONE = 999
	};

public:
	enum class M_PATTERN : _uint
	{
		PATTERN_1 = 1,
	};

private:
	CHumanoid_Dragoon(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CHumanoid_Dragoon(const CHumanoid_Dragoon& rhs);
	virtual ~CHumanoid_Dragoon() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype() override;
	virtual HRESULT					NativeConstruct(void* pArg) override;
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	void							Set_Target();
	void							Chase(_double TimeDelta);
	void							Play_Pattern(_double dTimeDelta);
	void							Check_Pattern_Cool(_double TimeDelta);

	//for Pattern_1
	void							Pattern_1(_double TimeDelta);
	void							Check_Target1(_double TimeDelta);
	void							Check_AnimState1(_double TimeDelta);
	void							Check_Times1(_double TimeDelta);

public:
	virtual _bool					Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void					Collision(CGameObject* pGameObject) override;

public:
	virtual void					Notify(void* pMessage)			override;

protected:
	virtual _int					VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT					Update_UI(_double dDeltaTime)	override;

public:
	static	CHumanoid_Dragoon*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;


private:
	class CPlayer*	m_pPlayer = nullptr;
	CTransform*		m_pPlayerTransformCom = nullptr;

private:
	//for Anim
	_uint			m_iAnimIndex = 0;

	//for Enums
	M_PATTERN		m_ePattern;
	M_ANIMSTATE		m_eAnimState;

	//for Chase
	_float			m_fRatio = 0.f;
	_float			m_fDistance = 0.f;

	// For Test
	_uint			m_iMaxHP = 300;
	_int			m_iCrntHP = 300;
	_bool			m_bCallHPBarUI = false;
	_double			m_dCallHPBarUITimeDuration = 3.0;
};

END

#endif
