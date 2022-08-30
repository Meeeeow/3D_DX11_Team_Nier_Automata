#pragma once
#ifndef __ENGINE_OC_MESH_CONTAINER_H__
#define __ENGINE_OC_MESH_CONTAINER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL COcMeshContainer final : public CBase
{
private:
	COcMeshContainer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit COcMeshContainer(const COcMeshContainer& rhs);
	virtual ~COcMeshContainer() DEFAULT;

public:
	HRESULT							NativeConstruct(_uint iStartVertexIndex, _uint iNumVertices
		, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex);
	void							Culling();
	HRESULT							Render();

	HRESULT							SetUp_CombinedMatrices(_matrix* pMatrices, _fmatrix PivotMatrix);
	void							SetUp_BoundingBoxPoint(const _vector& vMaxPoint, const _vector& vMinPoint);
	void							SetUp_ChildBoundingBox(const _vector& vCenter, const _vector& vHalfExtents);

public:
	void							Add_HierarchyNode(class CHierarchyNode* pNode) { m_Bones.push_back(pNode); }

public:
	_uint							Get_MaterialIndex() const { return m_iMaterialIndex; }
	_uint							Get_StartVertexIndex() const { return m_iStartVertexIndex; }
	const _bool&					Get_Culling() const { return m_bCulling; }

public:
	void							Set_Culling() { m_bCulling = true; }

private:
	_pGraphicDevice					m_pGraphicDevice = nullptr;
	_pContextDevice					m_pContextDevice = nullptr;

	_uint							m_iStartVertexIndex = 0;
	_uint							m_iNumVertices = 0;

	_uint							m_iStartFaceIndex = 0;
	_uint							m_iNumFaces = 0;

	_uint							m_iMaterialIndex = 0;

	_float3							m_vMinPoint = _float3(0.f, 0.f, 0.f);
	_float3							m_vMaxPoint = _float3(0.f, 0.f, 0.f);
	_float3							m_vCenter = _float3(0.f, 0.f, 0.f);
	_float3							m_vExtents = _float3(0.f, 0.f, 0.f);
	_float							m_fRadius = 0.f;

	_bool							m_bCulling = false;

	BoundingBox*					m_pBoundingBox = nullptr;
	BoundingBox*					m_pChildBoundingBox[8] = { nullptr };

	_bool							m_bCloned = false;

private:
	vector<class CHierarchyNode*>	m_Bones;

public:
	static	COcMeshContainer*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, _uint iStartVertexIndex, _uint iNumVertices
		, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex);
	COcMeshContainer*				Clone();
	virtual	void					Free() override;

};

END
#endif // !__ENGINE_OC_MESH_CONTAINER_H__