#pragma once
#include "Client_Defines.h"
#include "MeshEffect.h"
class CEffectBullet : public CMeshEffect
{
//public:
//	enum class BULLET_TYPE : _uint
//	{
//		JUMP = 0,
//		NONE = 999
//	};

protected:
	EFFECT_DESC		m_tDesc;
	unsigned long	m_ulID = -1;
	EFFECT_MOVEMENT m_tMovement;
	_float			m_fMaxLifeTime = -1.f;
	_float			m_fCurLifeTime = 0.f;
	_int			m_iJumpCount = 0;

private:
	_int			m_iIsEnd = 0;
public:
	const _int&		Get_IsEnd() {
		return m_iIsEnd;
	}
public:
	const unsigned long& Get_ID() const {
		return m_ulID;
	}


public:
	void Set_Movement(const EFFECT_MOVEMENT& _tMovement) {
		m_tMovement = _tMovement;
	}
	void Update_WithMovement();


protected:
	explicit CEffectBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffectBullet(const CMeshEffect& rhs);
	virtual ~CEffectBullet() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	HRESULT SetUp_RestComponents();

	HRESULT Set_Collider();

protected:
	HRESULT	Create_Explosion();
	_bool	Is_UnderNavigatoinMesh();

private:
	_bool					m_bMonsterAttack = true;
	_bool					m_bForMiniGame = false;

public:
	// For Collision
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;

public:
	static CEffectBullet* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();

};

