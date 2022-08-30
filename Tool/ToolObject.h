#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CModel;
class CNavigation;
END

class CToolObject final : public CGameObject
{
private:
	CToolObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToolObject(const CToolObject& rhs);
	virtual ~CToolObject() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void Set_ProtoModel(const _tchar* pszTag) {
		lstrcpy(m_szProtoModel, pszTag);
	}

	void Set_AnimationIndex(_uint iIndex) {
		m_iAnimationIndex = iIndex;
	}

	void Set_RootMotionValue(_double dValue);

	_int Find_Animation_Initialize(const char* pTag);
	_int Find_Animation(const char* pTag);

	void Set_Stop(_bool bCheck) {
		m_bStop = bCheck;
	}

	void Initialize_Time();

public:
	const _double Get_PlayTime() const;
	const _double Get_Duration() const;
	const _double Get_PlaySpeedPerSec() const;
	const _bool Get_Stop() const {
		return m_bStop;
	}

	const _double Get_LinkMinTime(_uint iIndex) const;
	const _double Get_LinkMaxTime(_uint iIndex) const;

	const _double Get_BlendMaxTime(_uint iIndex) const;

	const _double		Get_BlendLevel(_uint iIndex) const;
	const _bool			Get_NoneUpdatePos(_uint iIndex) const;
	const DIRECTION	    Get_Direction(_uint iIndex) const;

	const _int	  Get_AnimationIndex() const {
		return m_iAnimationIndex;
	}

	const _uint Get_RootBone_CurrentKeyFrameIndex(_uint iIndex) const;

	const _float4 Get_BonePosition(_uint iAnimationIndex, const char* pTag) const;

	const _double Get_RootMotionValue(_uint iAnimationIndex) const;
	const _double Get_RootMotionValue() const;

	const _double Get_CollisionMinRatio() const;
	const _double Get_CollisionMaxRatio() const;

	const _double Get_CollisionMinRatio(_uint iIndex) const;
	const _double Get_CollisionMaxRatio(_uint iIndex) const;

	vector<CAnimation*>*		Get_Animation();
	vector<CHierarchyNode*>*	Get_HierarchyNode();

public:
	void Set_PlayTime(_double Time);
	void Set_Duration(_double Time);

public:
	void Set_PlaySpeedPerSec(_double Speed);
	void Set_RenderStop(_bool bCheck) {
		m_bRenderStop = bCheck;
	}
	void Set_CollRender(_bool bCheck) {
		m_bCollRender = bCheck;
	}

	void Set_CurBoneName(char* pName);

	void Set_LinkMinRatio(_uint iIndex, _double Ratio);
	void Set_LinkMaxRatio(_uint iIndex, _double Ratio);
	void Set_BlendMaxTime(_uint iIndex, _double Time);
	void Set_BlendLevel(_uint iIndex, _uint iLevel);
	void Set_NoneUpdatePos(_uint iIndex, _bool bCheck);
	void Set_Direction(_uint iIndex, DIRECTION eCheck);
	void Set_CollisionMinRatio(_uint iIndex, _double Ratio);
	void Set_CollisionMaxRatio(_uint iIndex, _double Ratio);
	void Set_CollisionMinRatio(_double Ratio);
	void Set_CollisionMaxRatio(_double Ratio);

private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pSphereCom = nullptr;

	//CCollider*			m_pColliderCom = nullptr;
	//CCollider*			m_pOBBCom = nullptr;

private:
	_tchar		m_szProtoModel[MAX_PATH];
	_uint		m_iAnimationIndex = 0;
	_bool		m_bStop = false;
	_bool		m_bRenderStop = false;
	_bool		m_bCollRender = false;

private:
	_tchar		m_szCurBoneName[MAX_PATH];

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CToolObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};
