#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Client)

class CPod9S final : public CGameObjectModel
{
private:
	CPod9S(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPod9S(const CPod9S& rhs);
	virtual ~CPod9S() = default;

public:
	virtual HRESULT				NativeConstruct_Prototype();
	virtual HRESULT				NativeConstruct(void* pArg);
	virtual _int				Tick(_double TimeDelta);
	virtual _int				LateTick(_double TimeDelta);
	virtual HRESULT				Render();

public:
	void						Update_Position(_vector vPos);
	void						Set_ParentMatrix(_matrix Matrix) { XMStoreFloat4x4(&m_ParentFloat4x4, Matrix); }
	void						Update_PositionForCoaster(_vector vPos);

public:
	void						Update(_double TimeDelta);
	void						Check_State();
	void						Check_Idle();

public:
	HRESULT						SceneChange_AmusementPark();
	HRESULT						SceneChange_OperaBackStage();
	HRESULT						SceneChange_ZhuangziStage();
	HRESULT						SceneChange_ROBOTGENERAL();

public:
	void						Set_Position_Immediately(_vector vPos);
	void						Set_Teleport(_bool bCheck);
	void						Set_Appearance(_bool bCheck) { m_bAppearance = bCheck; }

public:
	void						Check_Teleport(_double TimeDelta);

public:
	void						Set_PodState(PODSTATE eState);

public:
	PODSTATE					Get_PodState() const { return m_ePodState; }

private:
	HRESULT						SetUp_Components();

private:
	_uint						m_iAnimation = 0;
	_bool						m_bContinue = false;
	_float4x4					m_ParentFloat4x4;

private:
	_bool						m_bTeleport = false;
	_bool						m_bAppearance = false;

private:
	_bool						m_bOnCoaster = false;

private:
	_float4						m_vDestPos = _float4(0.f, 0.f, 0.f, 1.f);
	_double						m_TimeDelta = 0.0;
	_double						m_AccelTime = 0.0;
	PODSTATE					m_ePodState = PODSTATE::IDLE;

public:

public:
	static CPod9S*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free();
};

END

