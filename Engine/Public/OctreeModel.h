#pragma once
#ifndef __ENGINE_OCTREE_MODEL_H__
#define __ENGINE_OCTREE_MODEL_H__


#include "VIBuffer.h"
#include "Animation.h"
#include "HierarchyNode.h"

BEGIN(Engine)

class ENGINE_DLL COctreeModel final : public CVIBuffer
{
private:
	COctreeModel(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit COctreeModel(const COctreeModel& rhs);
	virtual ~COctreeModel() DEFAULT;

public:
	virtual	HRESULT				NativeConstruct_Prototype(const _tchar * pDatFilePath
		, const _tchar *		pShaderFilePath
		, const char *			pTechniqueName);
	virtual	HRESULT				NativeConstruct(void* pArg) override;

private:
	// For Import
	HRESULT						Import_Parameter(const _tchar * pDatFilePath);
	HRESULT						Free_Param();

private:
	// For NativeConstruct_Prototype
	HRESULT						Create_MeshContainer();
	HRESULT						Compile_Shader(const _tchar* pShaderFilePath, const char* pTechniqueName);
	HRESULT						Create_Materials();
	HRESULT						Create_VIBuffer();

	// For NativeConstruct
	HRESULT						Create_HierarchyNodes();
	HRESULT						SetUp_HierarchyNodeToMeshContainer();
	HRESULT						Create_Animation();

private:
	CHierarchyNode*				Find_HierarchyNode(const char* pBoneName);



public:
	vector<CAnimation*>*		Get_Animation() { return &m_Animations; }

	vector<CHierarchyNode*>*	Get_HierarchyNode();

	HRESULT						Add_RefNode(const _tchar* pNodeTag, const char* pBoneName);

public:
	HRESULT						Bind_Buffers();
	HRESULT						Set_Animation(_uint iAnimIndex, _bool bContinue = false);
	_uint						Get_NumMeshContainers() const { return (_uint)m_MeshContainers.size(); }
	HRESULT						Set_MaterialTexture(_uint iMeshContainerIndex, aiTextureType eTextureType, char* pContantName);

	void						Update_CombinedTransformationMatrix(_double TimeDelta);
	void						Culling();
	void						Culling_Additional();

	virtual HRESULT				Render(_uint iMeshIndex, _uint iPassIndex);

	void						Reset_Culling();
	void						SetUp_BoundingBox(const _matrix& matWorld);

private:
	// For Create
	IPGeneral					m_IPGeneral;
	vector<IPMeshContainer*>	m_vecIPMeshContainer;
	vector<IPMaterial*>			m_vecIPMaterial;
	vector<IPHNode*>			m_vecIPHNode;
	vector<IPAnim*>				m_vecIPAnim;
	vector<IPChannel*>			m_vecIPChannel;

private:
	typedef vector<class COcMeshContainer*>						MESHCONTAINERS;
	typedef vector<MESHMATERIAL*>								MATERIALS;
	typedef vector<CHierarchyNode*>								HIERARCHYNODE;
	typedef vector<CAnimation*>									ANIMATIONS;
	typedef unordered_map<const _tchar*, CHierarchyNode*>		REFNODES;


	MESHCONTAINERS				m_MeshContainers;
	MATERIALS					m_Materials;
	HIERARCHYNODE				m_HierarchyNodes;
	ANIMATIONS					m_Animations;
	REFNODES					m_RefNodes;


private:
	_float4x4					m_PivotMatrix;
	_uint						m_iCurrentAnimationIndex = 0;
	const _tchar*				m_pAnimationPath = nullptr;
	_float4						m_vPreRootMotion = _float4(0.f, 0.f, 0.f, 1.f);

private:	/* For Animation Blending */
	_uint						m_iNextAnimationIndex = 0;
	_bool						m_bAnimationBlend = false;
	_bool						m_bContinue = false;
	_bool						m_bNoneBlend = false;

private:	/* For Sphere Collider */
	_float3						m_vMiddlePoint = _float3(0.f, 0.f, 0.f);
	_float3						m_vMinPoint = _float3(0.f, 0.f, 0.f);
	_float3						m_vMaxPoint = _float3(0.f, 0.f, 0.f);
	_float						m_fRadius = 0.f;


private:	/* For BoundingBox */
	BoundingBox*				m_pBoundingBox = nullptr;

private:
	_bool						m_bBlendFinishMoment = false;

	void						Compute_BoundingBoxPoint(const _uint& iStartVertexIndex, const _uint& iEndVertexIndex
		, _vector& vMaxPoint, _vector& vMinPoint, const _matrix& matWorld);
	

public:
	_float3						Get_MiddlePoint();
	_float3						Get_MinPoint();
	_float3						Get_MaxPoint();
	_float						Get_Radius();
	BoundingBox*				Get_BoundingBox();

public:
	static	COctreeModel*		Create(_pGraphicDevice	pGraphicDevice
		, _pContextDevice		pContextDevice
		, const _tchar *		pDatFilePath
		, const _tchar *		pShaderFilePath
		, const char *			pTechniqueName);
	virtual	COctreeModel*		Clone(void* pArg) override;
	virtual	void				Free() override;

};

END
#endif // !__ENGINE_OCTREE_MODEL_H__