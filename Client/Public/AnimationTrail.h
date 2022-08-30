#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CRenderer;
class CTransform;
class CVIBuffer_AnimationTrail;
class CTexture;
END

BEGIN(Client)

class CAnimationTrail : public CGameObject
{
public:
	typedef struct tagTrailDescription
	{
		_int	iNumTrails;
	} TRAIL_DESC;
private:
	TRAIL_DESC					m_tDesc;

protected:
	CAnimationTrail(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CAnimationTrail(const CAnimationTrail& rhs);
	virtual ~CAnimationTrail() DEFAULT;

public:
	static	CAnimationTrail*				Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual	CGameObject*		Clone(void * pArg);
	virtual void				Free()							override;
	virtual	HRESULT				NativeConstruct_Prototype()		override;
	virtual HRESULT				NativeConstruct(void* pArg)		override;

public:
	virtual _int				Tick(_double dTimeDelta)		override;
	virtual _int				LateTick(_double dTimeDelta)	override;
	virtual HRESULT				Render()						override;

protected:
	virtual HRESULT				SetUp_Components();
	virtual HRESULT				SetUp_ConstantTable();

public:
	// For Visual Debugging
	virtual const char*			Get_PassName(_uint iIndex);
	virtual _uint				Get_MaxPassIndex();

public:
	virtual void				Notify(void* pMessage) override;
protected:
	virtual _int				VerifyChecksum(void* pMessage) override;

public:
	HRESULT						Update_Position();
	HRESULT						Init_TrailPosition();

	void						Set_FarPoint1(_float3 _vPos) {
		m_vFarPoint1 = _vPos;
	}
	void						Set_FarPoint2(_float3 _vPos) {
		m_vFarPoint2 = _vPos;
	}

private:
	_float4x4					m_tBonMatrices[128];
	XMUINT4						m_vMaxBlendIndex;
	XMUINT4						m_vMinBlendIndex;
	_float4						m_vMaxBlendWieght;
	_float4						m_vMinBlendWeight;
	_float4x4					m_matWorldMatrixTranspose4x4;
	_float4x4					m_matWorldMatrix4x4;
	_float3						m_vFarPoint1 = _float3(0.f, 0.f, 0.f);
	_float3						m_vFarPoint2 = _float3(0.f, 0.f, 0.f);
public:
	void						Add_BoneMatrices(_float4x4* _tBoneMatrices);
	void						Set_MaxBlendIndex(const XMUINT4& vMaxBlendIndex) {
		m_vMaxBlendIndex = vMaxBlendIndex;
	}
	void						Set_MinBlendIndex(const XMUINT4& vMinBlendIndex) {
		m_vMinBlendIndex = vMinBlendIndex;
	}
	void						Set_MaxBlendWeight(const _float4& vMaxBlendWieght) {
		m_vMaxBlendWieght = vMaxBlendWieght;
	}
	void						Set_MinBlendWeight(const _float4& vMinBlendWeight) {
		m_vMinBlendWeight = vMinBlendWeight;
	}
	//void						Set_WorldMatrixTranspose(const _float4x4& _vWorldMatrix) {
	//	m_matWorldMatrixTranspose4x4 = _vWorldMatrix;
	//}
	void						Set_WorldMatrix(const _float4x4& _vWorldMatrix) {
		m_matWorldMatrix4x4 = _vWorldMatrix;
	}

protected:
	CRenderer*					m_pRenderer = nullptr;
	CTransform*					m_pTransform = nullptr;
	CVIBuffer_AnimationTrail*			m_pModel = nullptr;
	CTexture*					m_pTexture = nullptr;
};

END