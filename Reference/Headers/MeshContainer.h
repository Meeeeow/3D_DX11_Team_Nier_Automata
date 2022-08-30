#pragma once
#ifndef __ENGINE_MESH_CONTAINER_H__
#define __ENGINE_MESH_CONTAINER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CMeshContainer final : public CBase
{
public:
	CMeshContainer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

	_uint Get_StartVertexIndex() const {
		return m_iStartVertexIndex;
	}

public:
	HRESULT					NativeConstruct(_uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex);
	HRESULT					Render();
	void					Add_HierarchyNode(class CHierarchyNode* pNode) {
		m_Bones.push_back(pNode);
	}
	HRESULT SetUp_CombinedMatrices(_matrix* pMatrices, _fmatrix PivotMatrix);
	const CHierarchyNode*	Get_HierarchyNode(const char * pNodeTag);

	HRESULT	SetUp_CombineBlurMatrices(_matrix* pMatrices, _matrix* pPreMatrices, _fmatrix matPivot);

private:
	_pGraphicDevice			m_pGraphicDevice= nullptr;
	_pContextDevice			m_pContextDevice = nullptr;
private:
	_uint					m_iStartVertexIndex = 0;
	_uint					m_iStartFaceIndex;
	_uint					m_iNumVertices = 0;
	_uint					m_iNumFaces = 0;
	_uint					m_iMaterialIndex = 0;
	
	vector<class CHierarchyNode*>		m_Bones;	



public:
	static CMeshContainer*	Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex);
	CMeshContainer*			Clone();
	virtual void Free()		override;
};

END

#endif // !__ENGINE_MESH_CONTAINER_H__