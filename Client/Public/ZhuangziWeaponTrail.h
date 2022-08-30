#pragma once

#ifndef __CLIENT_CZHUANGZIWEAPONTRAIL_H__
#define __CLIENT_CZHUANGZIWEAPONTRAIL_H__

#include "MeshEffect.h"
#include "Normal_Attack.h"

BEGIN(Client)

class CZhuangziWeaponTrail final : public CMeshEffect
{
protected:
	EFFECT_DESC		m_tDesc;
	unsigned long	m_ulID = -1;
	EFFECT_MOVEMENT m_tMovement;
	_float			m_fMaxLifeTime = -1.f;
	_float			m_fCurLifeTime = 0.f;

public:
	explicit CZhuangziWeaponTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CZhuangziWeaponTrail(const CZhuangziWeaponTrail& rhs);
	virtual ~CZhuangziWeaponTrail() = default;
public:
	virtual	HRESULT							NativeConstruct_Prototype() override;
	virtual HRESULT							NativeConstruct(void* pArg) override;
	virtual _int							Tick(_double dTimeDelta);
	virtual _int							LateTick(_double dTimeDelta);
	virtual HRESULT							Render();

public:
	void									Update_NormalAttack();
	HRESULT									Create_NormalAttack();
	_matrix									Get_MatrixForNormalAttack(_uint iCount);

public:
	void									Go_Dir(_double TimeDelta);

public:
	void									Check_DeleteTime(_double TimeDelta);

public:
	virtual _bool							Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void							Collision(CGameObject* pGameObject) override;

private:
	std::vector<CNormal_Attack*>			m_vecNormalAttacks;

private:
	_double									m_DeleteTime = 0.0;
	_double									m_DeleteDelayTime = 5.0;

private:
	HRESULT									SetUp_RestComponents();
	_bool									Is_UnderNavigatoinMesh();

public:
	static	CZhuangziWeaponTrail*			Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*					Clone(void* pArg) override;
	virtual void							Free() override;
};	

END

#endif