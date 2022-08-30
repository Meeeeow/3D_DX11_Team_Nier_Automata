#pragma once

#include "Client_Defines.h"
#include "GameObjectModel.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CVIBuffer_Pointbillboard;
class CVIBuffer_PointInstance;
class CModel;
END

BEGIN(Client)
class CMeshEffect : public CGameObjectModel
{
public:
	enum class MODEL_PASS : _int
	{
		IMAGE = 0,
		IMAGELESS = 1,
		IMAGELESS_WIREFRAME = 2,
		IMAGE_DISSOLVE = 3,
		COUNT = 4
	};
	enum class POINTBILLBOARD_PASS : _int
	{
		BASE = 0,
		EMPTY = 1,
	};

public:
	typedef struct IndividualMeshEffect
	{
		EFFECTMESHVARIABLES			tEffectMeshVariables;
		EFFECTMESHDESC				tEffectMeshDescription;

		CTransform*					pTransformCom = nullptr;
		CTexture*					pDefaultTextureCom = nullptr;
		CTexture*					pAlphaOneTextureCom = nullptr;
		CTexture*					pMaskTextureCom = nullptr;
		CModel*						pModelCom = nullptr;
		CVIBuffer_Pointbillboard*	pPointbillboardCom = nullptr;
		CVIBuffer_Cube*				pCube = nullptr;

		_int						iMeshKind;

		_int						iModelPass = (_int)MODEL_PASS::IMAGE;
		_int						iImagelessModelPass = (_int)MODEL_PASS::IMAGELESS;
		_int						iPointbillboardPass = (_int)POINTBILLBOARD_PASS::BASE;

		_float						fCameraDistance;

	} INDIVIDUALMESHEFFECT;

	typedef struct IndividualParticle
	{
		PARTICLEDESC				tParticleDesc;
		PARTICLE_VAR				tParticleVar;

		CTransform*					pTransformCom = nullptr;
		CTexture*					pTextureCom = nullptr;
		CVIBuffer_PointInstance*	pModelCom = nullptr;

	} INDIV_PARTICLE;
protected:
	vector<INDIVIDUALMESHEFFECT*>	m_vecIndivMeshEffect;
	vector<INDIV_PARTICLE*>			m_vecIndivParticle;

protected:
	EFFECT_DESC						m_tEffectDesc;
	EffectParticleGroupDescription	m_tGroupDesc;
	_float							m_fLifeTime = 0.f;
public:
	const EffectParticleGroupDescription& Get_Desc() { return m_tGroupDesc; }

	virtual const char*			Get_PassName(_uint iIndex);
	virtual _uint				Get_MaxPassIndex();
	virtual const char*			Get_BufferContainerPassName(_uint iIndex, _uint iContainerSize);
	virtual _uint				Get_BufferContainerMaxPassIndex(_uint iIndex);
	virtual _uint				Get_BufferContainerSize();
	virtual _uint				Get_BufferContainerPassIndex(_uint iIndex);
	_bool						Get_BufferContainer() { return m_bBuffers; }

	virtual void				Set_BufferContainerPassIndex(_uint iIndex, _uint iPassIndex);

protected:
	explicit CMeshEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshEffect(const CMeshEffect& rhs);
	virtual ~CMeshEffect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	virtual _bool			Check_Collision(CGameObject* pGameObject, _uint iColliderType) override;
	virtual void			Collision(CGameObject* pGameObject) override;

public:
	virtual _fvector            Get_RayOrigin()				override;
	virtual _fvector            Get_RayDirection()			override;
	_fvector					Get_RingScale(CTransform::STATE eState);


public:
	const _float				Get_RayDist() { return m_fRayDist; }

private:
	CTexture*					m_pDissolveTextureCom = nullptr;

protected:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable_MeshEffect(const IndividualMeshEffect& _tDesc);
	HRESULT SetUp_ConstantTable_Particle(const INDIV_PARTICLE& _tDesc);


private:
	//void	Ready_Play();
	void	Init_AllDatas();
	void	Init_MeshEffect_Datas(IndividualMeshEffect& _tDesc);
	void	Init_PatternData(IndividualMeshEffect& _tDesc);
	void	Init_AllPatternData();
	void	Init_PositionPatternData(IndividualMeshEffect& _tDesc);
	void	Init_RotationPatternData(IndividualMeshEffect& _tDesc);
	void	Init_ScalePatternData(IndividualMeshEffect& _tDesc);
	void	Init_Texture(IndividualMeshEffect& _tDesc);
	void	Init_ShaderVariables(IndividualMeshEffect& _tDesc);
	void	Init_FadeData(IndividualMeshEffect& _tDesc);


	void	Pattern(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Pattern_Position(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Pattern_Rotation(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Pattern_Scale(_float _dTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);

	void	Update_TextureIndex(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Update_ShaderVariables(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Update_DissolveShader(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Update_Fade(_float _fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Update_FadeIn(_float fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);
	void	Update_FadeOut(_float fTimeDelta, INDIVIDUALMESHEFFECT& _tIndiv);


public:
	const unsigned long& Get_ID() const {
		return m_ulID;
	}

	// Particle 
	void	Init_AllParticleVar();
	void	Init_ParticleVar(INDIV_PARTICLE& tParticleVar);

protected:
	_bool			m_bCreatedExplosion = false;

protected:
	unsigned long	m_ulID = -1;

private:
	_float			m_fRayDist = 10.f;

public:
	HRESULT	Create_AfterExplosion();

public:
	static CMeshEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END