#pragma once

#include "Client_Struct.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
class CVIBuffer_Pointbillboard;
class CModel;
END


class CToolEffectMeshObject final : public CGameObject
{
private:
	EFFECTMESHVARIABLES m_tEffectMeshVariables;

public:
	void Play(_float2 _vCreateDeathTime, _bool _bCreateDeathRepeat);
	void Stop();


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



private:
	EFFECTMESHDESC	m_tDesc;
	//EFFECTMESHDESC*	m_pDesc;



	_int			m_iModelPass = (_int)MODEL_PASS::IMAGE;
	_int			m_iImagelessModelPass = (_int)MODEL_PASS::IMAGELESS;
	_int			m_iPointbillboardPass = (_int)POINTBILLBOARD_PASS::BASE;

public:
	const EFFECTMESHDESC& Get_Desc() { return m_tDesc; }
private:
	explicit CToolEffectMeshObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToolEffectMeshObject(const CToolEffectMeshObject& rhs);
	virtual ~CToolEffectMeshObject() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;

	CTexture*					m_pDefaultTextureCom = nullptr;
	CTexture*					m_pAlphaOneTextureCom = nullptr;
	CTexture*					m_pMaskTextureCom = nullptr;

	CTexture*					m_pDissolveTextureCom = nullptr;

	CModel*						m_pModelCom = nullptr;
	CVIBuffer_Pointbillboard*	m_pPointbillboardCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	void	Update_Desc(const EFFECTMESHDESC& _Desc);

private:
	HRESULT Apply_Texture(_int iKind, const _tchar* _szProto);
	//void	Ready_Play();


private:
	void	Init_Datas();
	void	Init_PatternData();
	void	Init_PositionPatternData();
	void	Init_RotationPatternData();
	void	Init_ScalePatternData();
	void	Init_Texture();
	void	Init_ShaderVariables();
	void	Init_FadeData();

	void	Pattern(_float _fTimeDelta);
	void	Pattern_Position(_float _fTimeDelta);
	void	Pattern_Rotation(_float _fTimeDelta);
	void	Pattern_Scale(_float _fTimeDelta);

	void	Update_TextureIndex(_float _fTimeDelta);
	void	Update_ShaderVariables(_float _fTimeDelta);
	void	Update_DissolveShader(_float _fTimeDelta);
	void	Update_Fade(_float _fTimeDelta);
	void	Update_FadeIn(_float fTimeDelta);
	void	Update_FadeOut(_float fTimeDelta);
public:
	_float4 Get_ProjectilePowerToTarget(_float4 vStartPos, _float4 vTargetPos, _float fProjectileMaxTime, _float fDegree);

public:
	static CToolEffectMeshObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

