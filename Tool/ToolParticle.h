#pragma once

#include "GameObject.h"
#include "Engine_Struct.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CVIBuffer_PointInstance;
END


class CToolParticle final : public CGameObject
{
private:
	PARTICLEDESC	m_tParticleDesc;
	PARTICLE_VAR	m_tParticleVar;


private:
	explicit CToolParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToolParticle(const CToolParticle& rhs);
	virtual ~CToolParticle() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_PointInstance*	m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void	Update_ByDesc(const PARTICLEDESC& _tDesc);
	void	Play();
	void	Stop();
	
	void	Init_Var();


public:
	const PARTICLEDESC& Get_Desc() {
		return m_tParticleDesc; 
	}

public:
	static CToolParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

