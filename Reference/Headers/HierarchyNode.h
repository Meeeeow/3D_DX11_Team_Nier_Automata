#pragma once

#ifndef __ENGINE_HIERARCHYNODE_H__
#define __ENGINE_HIERARCHYNODE_H__


#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;
public:
	_uint Get_Depth() const {
		return m_iDepth;
	}

	const char* Get_Name() const {
		return m_szName;
	}

	const char*	Get_TargetName() const {
		return m_szTargetName;
	}

	_matrix Get_CombinedTransformationMatrix() const {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	const _float4x4* Get_CombinedTransformationMatrixPtr() const {
		return &m_CombinedTransformationMatrix;
	}

	_matrix Get_OffsetMatrix() const {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}
	void	Set_Parent(CHierarchyNode* pParent) {
		m_pParent = pParent;
		Safe_AddRef(m_pParent);
	}

public:
	HRESULT NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	HRESULT NativeConstruct_ByDat(const char* pName, _fmatrix TransformationMatrix, _fmatrix OffsetMatrix, const char* pTargetName, _uint iDepth);
	HRESULT Reserve_Channels(_uint iNumAnimation);
	void Add_Channel(_uint iAnimIndex, class CChannel* pChannel);
	void Update_CombinedTransformationMatrix(_uint iCurrentAnimIndex);
private:
	char			m_szName[MAX_PATH] = "";
	char			m_szTargetName[MAX_PATH] = "";
	_float4x4		m_OffsetMatrix;
	_float4x4		m_TransformationMatrix; /* 부모기준으로 표현된 노드(뼈)의 상태행렬. */
	_float4x4		m_CombinedTransformationMatrix; /* 부모의 행렬상태를 포함한 상태행렬. */
	_uint			m_iDepth = 0;
	CHierarchyNode* m_pParent = nullptr;

private:
	/* 이름이 같은 채널들을 모아놓는다. */
	/* 각 애니메이션 마다 이 뼈를 사용할 수 있기때문에. */
	vector<class CChannel*>			m_AnimChannels;

public:
	static CHierarchyNode* Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	static CHierarchyNode* Create_ByDat(const char* pName, _fmatrix TransformationMatrix, _fmatrix OffsetMatrix, const char* pTargetName, _uint iDepth);
	virtual void Free() override;
};

END
#endif // !__ENGINE_HIERARCHYNODE_H__