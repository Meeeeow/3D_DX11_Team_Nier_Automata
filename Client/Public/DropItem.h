#pragma once
#ifndef __CLIENT_DROPITEM_H__
#define __CLIENT_DROPITEM_H__

#include "GameObjectModel.h"

BEGIN(Client)
class CDropItem final : public CGameObjectModel
{
public:
	enum class ANIMSTATE : _uint
	{
		ENABLE		= 0,
		ACTIVATE	= 1,
		DISABLE		= 2,
		INACTIVATE	= 3,
		NONE		= 4
	};

private:
	CDropItem(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CDropItem(const CDropItem& rhs);
	virtual ~CDropItem() DEFAULT;

public:
	static	CDropItem*	Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
	virtual	HRESULT			NativeConstruct_Prototype() override;
	virtual HRESULT			NativeConstruct(void* pArg) override;

private:
	HRESULT					SetUp_Components();
	void					SetUp_TreasureBox();

	void					Fill_Box1();
	void					Fill_Box2();
	void					Fill_Box3();
	void					Fill_Box4();

	void					Release_TreasureBox(class CPlayer* pPlayer);
	_bool					Check_Player();

public:
	virtual _int			Tick(_double dTimeDelta);
	virtual _int			LateTick(_double dTimeDelta);
	virtual HRESULT			Render();

public:
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;

public:
	virtual void			Notify(void* pMessage)			override;

protected:
	virtual _int			VerifyChecksum(void* pMessage)	override;

protected:
	virtual HRESULT			Update_UI(_double dDeltaTime)	override;

private:
	class CInventory*		m_pInventory = nullptr;

	ANIMSTATE				m_eAnimState = ANIMSTATE::NONE;
	_uint					m_iAnimIndex = 1;

	_float					m_fRotation = 0.f;
};

END

#endif // !__CLIENT_DROPITEM_H__