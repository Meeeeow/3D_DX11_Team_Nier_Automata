#pragma once

#include "VIBuffer.h"
#include "Animation.h"
#include "HierarchyNode.h"


/*
FBX to Client ±âº» Èå¸§
1. CExportModel
: fbx -> assimp -> data structure -> binary
2. CImportModel
: binary -> data structure -> Instance
*/

BEGIN(Engine)

class ENGINE_DLL CModel final : public CVIBuffer
{
public:
	typedef struct tagModelDescription
	{
		_bool bSaveMatrices = false;
	} MODELDESC;

private:
	MODELDESC		m_tDesc;
	_int			m_iMinIndex = -1;
	_int			m_iMaxIndex = -1;

public:
	enum class TYPE : _uint
	{
		NONANIM = 0,
		ANIM = 1,
		NONE = 2
	};

private:
	CModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	static CModel*			Create(_pGraphicDevice	pGraphicDevice
							, _pContextDevice		pContextDevice
							, const _tchar *		pDatFilePath
							, const _tchar *		pShaderFilePath
							, const char *			pTechniqueName
							, _bool					bMFC = false
							, const _tchar *		pAnimPath = nullptr
							);
	static CModel*			DynamicCreate(_pGraphicDevice	pGraphicDevice
							, _pContextDevice		pContextDevice
							, const _tchar *		pDatFilePath
							, const _tchar *		pShaderFilePath
							, const char *			pTechniqueName);
	virtual HRESULT			NativeConstruct_Prototype(const _tchar * pDatFilePath
							, const _tchar *		pShaderFilePath
							, const char *			pTechniqueName
							, _bool					bMFC
							, const _tchar *		pAnimPath 
							);
	virtual HRESULT			NativeConstruct_DynamicPrototype(const _tchar * pDatFilePath
							, const _tchar *		pShaderFilePath
							, const char *			pTechniqueName
							);
	virtual CComponent*		Clone(void* pArg) override;
	virtual HRESULT			NativeConstruct(void* pArg) override;
	virtual void			Free() override;

private:
	// For Import
	HRESULT	Import_Parameter(const _tchar * pDatFilePath);
	HRESULT	Free_Param();

private:
	// For NativeConstruct_Prototype
	HRESULT Create_MeshContainer();
	HRESULT Compile_Shader(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT Create_Materials(_bool bMFC);
	HRESULT Create_VIBuffer();
	HRESULT Create_DynamicVIBuffer();

	// For NativeConstruct
	HRESULT Create_HierarchyNodes();
	HRESULT SetUp_HierarchyNodeToMeshContainer();
	HRESULT Create_Animation();
	HRESULT Create_Animation_FromPath();

private:
	CAnimation*		Find_Animation(const char* pAnimName);
	CHierarchyNode* Find_HierarchyNode(const char* pBoneName);

public:
	vector<CAnimation*>*		Get_Animation() {
		return &m_Animations;
	}

	vector<CHierarchyNode*>*	Get_HierarchyNode();

	HRESULT						Add_RefNode(const _tchar* pNodeTag, const char* pBoneName);

public:
	// For Client
	HRESULT			Bind_Buffers();
	HRESULT			Set_Animation(_uint iAnimIndex, _bool bContinue = false);
	_uint			Get_NumMeshContainers() const { return (_uint)m_MeshContainers.size(); }
	HRESULT			Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char* pContantName);
	void			Update_CombinedTransformationMatrix(_double TimeDelta);
	virtual HRESULT	Render(_uint iMeshIndex, _uint iPassIndex);
	virtual HRESULT	Render_WithPreAnimation(_uint iMeshIndex, _uint iPassIndex);
	void			Set_OffsetMatrix(const _tchar* pTag, _matrix OffSetMatrix);
	_matrix			Get_OffsetMatrix(const _tchar* pTag);

	// For Tool
	HRESULT			Set_Animation_Tool(_uint iAnimIndex);
	void			Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta);

	virtual _bool	Picking(_float3* pOut, _matrix matWorld, _float& fDist) override;

private:
	_float4x4		m_tBoneMatrices[128];
	_bool			m_bIsSaveBoneMatrices = false;
public:
	void			Set_IsSaveBoneMatrices(_bool _b)
	{
		m_bIsSaveBoneMatrices = _b;
	}
	_float4x4*		Get_BoneMatrices() {
		return m_tBoneMatrices;
	}


public: // For Animation Control

	_int Find_AnimationIndex(const char* pTag);

	const _double Get_PlayTime() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_PlayTime();
	}

	const _double Get_Duration() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_Duration();
	}

	const _double Get_PlaySpeedPerSec() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_PlaySpeedPerSec();
	}

	const _float4 Get_RootMotion() const;

	const _float4 Get_PreRootMotion() const {
		return m_vPreRootMotion;
	}

	const _bool Get_OriginUpdate() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_OriginUpdate();
	}

	const _float4x4 Get_PivotMatrix() const {
		return m_PivotMatrix;
	}

	const _bool Get_Blend() const {
		return m_bAnimationBlend;
	}

	const _double Get_BlendMaxTime(_uint iIndex) const {
		return m_Animations[iIndex]->Get_BlendMaxTime();
	}

	const _double Get_BlendTime() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_BlendTime();
	}

	const _double Get_BlendMaxTime() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_BlendMaxTime();
	}

	const _uint Get_BlendLevel(_uint iIndex) const {
		return m_Animations[iIndex]->Get_BlendLevel();
	}

	const _uint Get_BlendLevel() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_BlendLevel();
	}

	const _bool Get_NoneUpdatePos() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_NonUpdatePos();
	}

	const _bool Get_NoneUpdatePos(_uint iIndex) const {
		return m_Animations[iIndex]->Get_NonUpdatePos();
	}

	const DIRECTION Get_Direction() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_Direction();
	}

	const DIRECTION Get_Direction(_uint iIndex) const {
		return m_Animations[iIndex]->Get_Direction();
	}

	const _uint Get_CurrentAnimation() const {
		return m_iCurrentAnimationIndex;
	}

	const _double Get_PlayTimeRatio() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_PlayTimeRatio();
	}

	const _uint	Get_NextAnimation() const {
		return m_iNextAnimationIndex;
	}

	const _uint Get_RootBone_CurrentKeyFrameIndex(_uint iIndex) const {
		return m_Animations[iIndex]->Get_RootBone_CurrentKeyFrameIndex();
	}

	const _float4 Get_BonePosition(_uint iAnimationIndex, const char* pTag) const {
		return m_Animations[iAnimationIndex]->Get_BonePosition(pTag);
	}

	const _bool&  Get_NoneBlend() const { return m_bNoneBlend; }

	_matrix Get_BoneMatrix(const _tchar* pNodeTag);

	_matrix Get_BoneMatrixNonPivot(const _tchar* pNodeTag);

	const _float4 Get_BeginRootBone_Position() const;

	const _double Get_LinkMinRatio(_uint iIndex) const {
		return m_Animations[iIndex]->Get_LinkMinRatio();
	}

	const _double Get_LinkMaxRatio(_uint iIndex) const {
		return m_Animations[iIndex]->Get_LinkMaxRatio();
	}

	const _double Get_LinkMinRatio() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_LinkMinRatio();
	}

	const _double Get_LinkMaxRatio() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_LinkMaxRatio();
	}

	const _bool IsEqual_Animation() const {
		return m_iCurrentAnimationIndex == m_iNextAnimationIndex;
	}

	const _double Get_CollisionMinRatio() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_CollisionMinRatio();
	}

	const _double Get_CollisionMaxRatio() const {
		return m_Animations[m_iCurrentAnimationIndex]->Get_CollisionMaxRatio();
	}

	const _double Get_CollisionMinRatio(_uint iIndex) const {
		return m_Animations[iIndex]->Get_CollisionMinRatio();
	}

	const _double Get_CollisionMaxRatio(_uint iIndex) const {
		return m_Animations[iIndex]->Get_CollisionMaxRatio();
	}

	const _double Get_BlendMaxTime_WithIndex(_uint iIndex);

	const _double Get_RootMotionValue() const;
	const _double Get_RootMotionValue(_uint iIndex) const;

	const _bool Check_BlendFinishMoment() const;
	
	const _matrix Get_MeshContainer_Root(const _int& _iMeshIndex, const char* szTag) const;

	const XMUINT4& Get_MaxBlendIndex();
	const _float4& Get_MaxBlendWeight();
	const XMUINT4& Get_MinBlendIndex();
	const _float4& Get_MinBlendWeight();


public:
	void Set_NextAnimationIndex(_uint iIndex) {
		m_iNextAnimationIndex = iIndex;
	}

	void Set_PlaySpeedPerSec(_double Speed) {
		m_Animations[m_iCurrentAnimationIndex]->Set_PlaySpeedPerSec(Speed);
	}

	void Set_PlayTime(_double Time) {
		m_Animations[m_iCurrentAnimationIndex]->Set_PlayTime(Time);
	}

	void Set_Duration(_double Time) {
		m_Animations[m_iCurrentAnimationIndex]->Set_Duration(Time);
	}

	void Set_OriginUpdate(_bool bCheck) {
		m_Animations[m_iCurrentAnimationIndex]->Set_OriginUpdate(bCheck);
	}

	void Set_PreRootMotion(_float4 vPos) {
		m_vPreRootMotion = vPos;
	}

	void Set_CurrentAnimation(_uint iIndex) {
		m_iCurrentAnimationIndex = iIndex;
	}

	void Set_NextAnimation(_uint iIndex) {
		m_iNextAnimationIndex = iIndex;
	}

	void Set_LinkMinRatio(_uint iIndex, _double Ratio) {
		m_Animations[iIndex]->Set_LinkMinRatio(Ratio);
	}

	void Set_LinkMaxRatio(_uint iIndex, _double Ratio) {
		m_Animations[iIndex]->Set_LinkMaxRatio(Ratio);
	}

	void Set_BlendMaxTime(_uint iIndex, _double Time) {
		m_Animations[iIndex]->Set_BlendMaxTime(Time);
	}

	void Set_BlendLevel(_uint iIndex, _uint iLevel) {
		m_Animations[iIndex]->Set_BlendLevel(iLevel);
	}

	void Set_NoneUpdatePos(_uint iIndex, _bool bCheck) {
		m_Animations[iIndex]->Set_NonUpdatePos(bCheck);
	}

	void Set_Direction(_uint iIndex, DIRECTION eCheck) {
		m_Animations[iIndex]->Set_Direction(eCheck);
	}

	void Set_CollisionMinRatio(_uint iIndex, _double Ratio) {
		m_Animations[iIndex]->Set_CollisionMinRatio(Ratio);
	}

	void Set_CollisionMaxRatio(_uint iIndex, _double Ratio) {
		m_Animations[iIndex]->Set_CollisionMaxRatio(Ratio);
	}

	void Set_CollisionMinRatio(_double Ratio) {
		m_Animations[m_iCurrentAnimationIndex]->Set_CollisionMinRatio(Ratio);
	}

	void Set_CollisionMaxRatio(_double Ratio) {
		m_Animations[m_iCurrentAnimationIndex]->Set_CollisionMaxRatio(Ratio);
	}

	void Set_NoneBlend() {
		m_bNoneBlend = true;
	}

	void Set_RootMotionValue(_double dValue);
	void Set_RootMotionValue(_uint iIndex, _double dValue);

public:
	void			Release_RefNode(const _tchar* pTag);

public:
	void			IsOriginUpdate_Initialize();
	void			Initialize_Time();
	void			Initialize_Time(_uint iIndex);
	void			Initialize_RootMotion();
	void			Initialize_OriginUpdate();
	void			Initialize_Blend();

public:
	void			Immediately_Change(_uint iAnimIndex);

public: /* For RootMotion */
	void			Synchronize_Animation(class CTransform* pTransform, class CNavigation* pNavigation = nullptr, _double dTimeDelta = 0.016);
	void			Synchronize_Animation_WithOtherLook(class CTransform* pTransform, class CNavigation* pNavigation, _vector vOtherLook, _double dTimeDelta = 0.016);
	void			Synchronize_Animation_For_Coaster(class CTransform* pTransform, class CNavigation* pNavigation = nullptr, _double dTimeDelta = 0.016);
	const _bool		IsFinish_Animation() const;

private:
	// For Create
	IPGeneral					m_IPGeneral;
	//IPVertex					m_IPVertex;
	vector<IPMeshContainer*>	m_vecIPMeshContainer;
	vector<IPMaterial*>			m_vecIPMaterial;
	vector<IPHNode*>			m_vecIPHNode;
	vector<IPAnim*>				m_vecIPAnim;
	vector<IPChannel*>			m_vecIPChannel;

private:
	typedef vector<class CMeshContainer*>						MESHCONTAINERS;
	typedef vector<MESHMATERIAL*>								MATERIALS;
	typedef vector<CHierarchyNode*>								HIERARCHYNODE;
	typedef vector<CAnimation*>									ANIMATIONS;
	typedef unordered_map<const _tchar*, CHierarchyNode*>		REFNODES;


	MESHCONTAINERS			m_MeshContainers;
	MATERIALS				m_Materials;
	HIERARCHYNODE			m_HierarchyNodes;
	ANIMATIONS				m_Animations;
	REFNODES				m_RefNodes;

			// For Animation
	_bool					m_bHaveAnim = false;

private:
	_float4x4					m_PivotMatrix;
	_uint						m_iCurrentAnimationIndex = 0;
	const _tchar*				m_pAnimationPath = nullptr;
	_float4						m_vPreRootMotion = _float4(0.f, 0.f, 0.f, 1.f);

private:	/* For Animation Blending */
	_uint					m_iNextAnimationIndex = 0;
	_bool					m_bAnimationBlend = false;
	_bool					m_bContinue = false;
	_bool					m_bNoneBlend = false;

private:	/* For Sphere Collider */
	_float3					m_vMiddlePoint = _float3(0.f, 0.f, 0.f);
	_float3					m_vMinPoint = _float3(0.f, 0.f, 0.f);
	_float3					m_vMaxPoint = _float3(0.f, 0.f, 0.f);
	_float					m_fRadius = 0.f;

private:
	_bool					m_bBlendFinishMoment = false;

	HRESULT					Compute_MiddlePoint();

private:
	_float3					m_vFarPoint1 = _float3(0.f, 0.f, 0.f);
	_float3					m_vFarPoint2 = _float3(0.f, 0.f, 0.f);

public:
	HRESULT					Compute_LongestTwoPoint();
	_float3					Get_FarPoint1() { return m_vFarPoint1; }
	_float3					Get_FarPoint2() { return m_vFarPoint2; }


public:
	_float3					Get_MiddlePoint();
	_float3					Get_MinPoint();
	_float3					Get_MaxPoint();
	_float					Get_Radius();

public:
	_bool					Get_HaveAnim() const{ return m_bHaveAnim; }

public:
	_float4					m_vecLook;
	_float4					Get_vecLook() { return m_vecLook; }
	_float3					m_vValue = _float3(0.f, 0.f, 0.f);
	_float3					Get_vecValue() { return m_vValue;}

public:

};

END