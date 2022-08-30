#pragma once

#include "VIBuffer.h"
#include "HierarchyNode.h"
#include "Animation.h"

BEGIN(Engine)

class ENGINE_DLL CAssimpModel final : public CVIBuffer
{
public:
	enum class TYPE : _uint
	{
		NONANIM		= 0,
		ANIM		= 1,
		NONE		= 2
	};

private:
	CAssimpModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAssimpModel(const CAssimpModel& rhs);
	virtual ~CAssimpModel() = default;

public:
	_uint Get_NumMeshContainers() const {
		return (_uint)m_MeshContainers.size();
	}
	_matrix Get_BoneMatrix(const _tchar* pNodeTag);

	vector<CAnimation*>&	Get_Animation() {
		return m_Animations;
	}

	vector<CHierarchyNode*>&	Get_HierarchyNode() {
		return m_HierarchyNodes;
	}

	_int Find_AnimationIndex(const char* pTag);

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eMeshType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix, const _tchar* pAnimPath);
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual HRESULT NativeConstruct_Prototype_ForTool(TYPE eMeshType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix);
	virtual HRESULT Render(_uint iMeshIndex, _uint iPassIndex);

public:
	HRESULT			Bind_Buffers();
	HRESULT			Compile_Shader(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT			Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char* pContantName);
	HRESULT			Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth);
	HRESULT			Create_SkinnedMesh();
	HRESULT			SetUp_HierarchyNodeToMeshContainer();
	HRESULT			Create_Animation();
	HRESULT			Add_RefNode(const _tchar* pNodeTag, const char* pBoneName);
	HRESULT			Create_Animation_FromPath();

public:
	HRESULT			Set_Animation(_uint iAnimIndex, _bool bContinue = false);
	HRESULT			Set_Animation_Tool(_uint iAnimIndex);
	/* 현재 애니메이션에 맞는 키프레임으로 뼈들의 행렬을 갱신해놓는다. */
	void			Update_CombinedTransformationMatrix(_double TimeDelta);
	void			Update_CombinedTransformationMatrixNonContinue_Tool(_double TimeDelta);

public:
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

	void IsOriginUpdate_Initialize();

	const _float4x4 Get_PivotMatrix() const {
		return m_PivotMatrix;
	}

	const _double Get_BlendMaxTime(_uint iIndex) const {
		return m_Animations[iIndex]->Get_BlendMaxTime();
	}

	const _bool Get_Blend() const {
		return m_bAnimationBlend;
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

	const _float4 Get_BeginRootBone_Position() const;

	const _bool IsEqual_Animation() const {
		return m_iCurrentAnimationIndex == m_iNextAnimationIndex;
	}

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

	void Set_NoneBlend() {
		m_bNoneBlend = true;
	}

	void Release_RefNode(const _tchar* pTag);
public:
	void Initialize_Time();

public:
	const _bool IsFinish_Animation() const;

public:	/* For. RootMotion */
	void Initialize_RootMotion();
	void Synchronize_Animation(class CTransform* pTransform);

public:
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


private:
	/* 파일로부터 읽은 모든 정보를 다 보관한다.  */
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	typedef vector<class CMeshContainer*>						MESHCONTAINERS;
	typedef vector<MESHMATERIAL*>								MATERIALS;
	typedef vector<CHierarchyNode*>								HIERARCHYNODE;
	typedef vector<CAnimation*>									ANIMATIONS;
	typedef unordered_map<const _tchar*, CHierarchyNode*>		REFNODES;



	MESHCONTAINERS												m_MeshContainers;
	MATERIALS													m_Materials;
	HIERARCHYNODE												m_HierarchyNodes;
	ANIMATIONS													m_Animations;
	REFNODES													m_RefNodes;

private:
	_float4x4			m_PivotMatrix;
	_uint				m_iCurrentAnimationIndex = 0;
	const _tchar*		m_pAnimationPath = nullptr;
	_float4				m_vPreRootMotion = _float4(0.f, 0.f, 0.f, 1.f);

	_float4				m_vecMinPos;
	_float4				m_vecMaxPos;
	_bool				m_bAnimation = false;
	_bool				m_bNoneBlend = false;

public:
	_bool				Get_HaveAnim() { return m_bAnimation; }
	_float4				Get_MinPos() { return m_vecMinPos; }
	_float4				Get_MaxPos() { return m_vecMaxPos; }

private:	/* For. Animation Blending */
	_uint				m_iNextAnimationIndex = 0;
	_bool				m_bAnimationBlend = false;
	_bool				m_bContinue = false;

private:
	HRESULT				Create_MeshContainer();
	HRESULT				Create_VIBuffer();
	HRESULT				Create_Materials(const char* pMeshFilePath);
	CHierarchyNode*		Find_HierarchyNode(const char* pBoneName);

public:
	static CAssimpModel*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eMeshType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix = XMMatrixIdentity(), const _tchar* pAnimPath = nullptr);
	static CAssimpModel*		Create_ForTool(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eMeshType, const char* pModelFilePath, const char* pModelFileName, const _tchar* pShaderFilePath, const char* pTechniqueName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

END