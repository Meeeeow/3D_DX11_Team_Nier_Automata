#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

class CPod final : public CGameObjectModel
{
	static const _float			COUNTERATTACK_RADIUS;
private:
	CPod(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPod(const CPod& rhs);
	virtual ~CPod() = default;

public:
	virtual HRESULT				NativeConstruct_Prototype();
	virtual HRESULT				NativeConstruct(void* pArg);
	virtual _int				Tick(_double TimeDelta);
	virtual _int				LateTick(_double TimeDelta);
	virtual HRESULT				Render();

private:
	HRESULT						SetUp_PodBulletLight(CGameInstance* pGameInstance);
	HRESULT						SetUp_PodLight(CGameInstance* pGameInstance, XMFLOAT4 vColor, _float fInnerDegree, _float fOuterDegree);

public:
	void						Update_PositionForCoaster(_vector vPos);

public:
	void						Update_Position(_vector vPos);
	void						Set_ParentMatrix(_matrix Matrix) { XMStoreFloat4x4(&m_ParentFloat4x4, Matrix); }

public:
	const _double				Get_SkillCoolTime() const;

public:
	void						Update(_double TimeDelta);
	void						Check_State();
	void						Create_Bullet(_double TimeDelta);
	void						Check_Laser();

	HRESULT						Create_Laser();

	void						Check_Light(CGameInstance* pGameInstance, _double dTimeDelta);
	void						Active_Light(CGameInstance* pGameInstance, _double dTimeDelta);
	void						DeActive_Light(CGameInstance* pGameInstance, _double dTimeDelta);

public:
	void						Set_LookForTarget(_vector vPos);

public:
	void						Set_MaxCoolTime();

public:
	HRESULT						SceneChange_AmusementPark();
	HRESULT						SceneChange_OperaBackStage();
	HRESULT						SceneChange_ZhuangziStage();
	HRESULT						SceneChange_ROBOTGENERAL();

public:
	void						Shoot_CounterAttack(_vector vHitTargetPos);

public:
	void						Set_PodState(PODSTATE eState);
	void						Set_CounterAttack(const _bool& bCheck) { m_bCounterAttack = bCheck; }
	void						Set_Targeting(const _bool& bCheck) { m_bTargeting = bCheck; }

public:
	void						Hang();
	void						CounterAttack();

public:
	PODSTATE					Get_PodState() const { return m_ePodState; }
	const _bool&				Get_CounterAttack() const { return m_bCounterAttack; }

protected:
	virtual HRESULT				Update_UI(_double dDeltaTime)	override;

private:
	HRESULT						SetUp_Components();

private:
	_uint						m_iAnimation = 0;
	_bool						m_bContinue = false;
	_float4x4					m_ParentFloat4x4;

private:
	_float4						m_vDestPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double						m_TimeDelta = 0.0;
	_double						m_AccelTime = 0.0;
	PODSTATE					m_ePodState = PODSTATE::IDLE;
	_double						m_CreateBulletTime = 0.0;
	_float						m_fDistance = 0.f;

	_float						m_fInnerDegree = 0.f;
	_float						m_fOuterDegree = 0.f;

private:
	_bool						m_bLaserShoot = false;
	_bool						m_bCounterAttack = false;
	_bool						m_bLight = false;

private:
	_bool						m_bTargeting = false;			// 플레이어에서 업데이트

private:
	_double						m_dSkillCoolTimeAcc = 0.0;
	_double						m_dSkillCoolTimeAccMax = 16.0;

private:
	_bool						m_bOnCoaster = false;
	_uint						m_iNumBullet = 0;

private:
	// For. Pod Bullet Light
	_uint						m_iPodLightCnt = 0;
	void						SetUp_PodBulletLight(CGameInstance* pGameInstance, _float4 vPosition);

private:
	HRESULT						Update_Effects(_double _dTimeDelta);

private:
	CGameObject*				m_pChargingEffect = nullptr;
	_bool						m_bIsChargingEffectCreated = false;

public:
	HRESULT						Create_ChargingEffect();
	HRESULT						Delete_ChargingEffect();


public:
	static CPod*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free();
};

static const _uint				POD_BULLETATTACK_DAMAGE = 3;

END

