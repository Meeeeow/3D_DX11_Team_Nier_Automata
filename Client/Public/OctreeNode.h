#pragma once
#ifndef __ENGINE_OCTREE_H__
#define __ENGINE_OCTREE_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL COctreeNode final : public CBase
{
private:
	COctreeNode(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~COctreeNode() DEFAULT; 

public:
	typedef DirectX::PrimitiveBatch<DirectX::VertexPositionColor> BATCH;

public:
	HRESULT						Make_BoundingBox(_float3 vMiddlePoint, _float3 vMaxPoint);
	void						Make_Child(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDeivce
		, _uint iIndex, _float3 vMiddlePoint, _float3 vMaxPoint
		, void* pVertices, void* pIndices, vector<_uint>* pVecIndex
		, _uint iDepth, _uint iMaxDepth);

public:
	BoundingBox*				Get_BoundingBox() { return m_pBoundingBox; }
	vector<_uint>*				Get_IndiciesIndexVector() { return &m_vecIndicesIndex; }
	COctreeNode*				Get_Child(_uint iIndex) { return m_pChildren[iIndex]; }

public:
	_int						LateTick(_double dTimeDelta);
	HRESULT						Render();

public:
	void						Set_Depth(_uint iDepth) { m_iDepth = iDepth; }

private:
	_pGraphicDevice				m_pGraphicDevice = nullptr;
	_pContextDevice				m_pContextDevice = nullptr;
	// 자식 옥트리 노드, 자식 수
	COctreeNode*				m_pChildren[8] = { nullptr };
	_uint						m_iNumChild = 0;
	// 현재 노드의 바운딩 박스, 그 바운딩 박스와 충돌한 Indices 인덱스의 번호
	BoundingBox*				m_pBoundingBox = nullptr;
	vector<_uint>				m_vecIndicesIndex;
		
	_uint						m_iDepth = 0;

	BasicEffect*				m_pEffect = nullptr;
	ID3D11InputLayout*			m_pInputLayout = nullptr;
	BATCH*						m_pBatch = nullptr;

public:
	static	COctreeNode*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, class CModel* pModel, _uint iMaxDepth);
	static	COctreeNode*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, _float3 vMiddlePoint, _float3 vMaxPoint
		, void* pVertices, void* pIndices , vector<_uint>* vecIndex
		, _uint iDepth, _uint iMaxDepth);
	virtual	void				Free() override;
};
END
#endif // !__ENGINE_OCTREE_H__