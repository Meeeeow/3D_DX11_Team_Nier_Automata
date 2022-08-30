#include "..\Public\HierarchyNode.h"
#include "Channel.h"

CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char * pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	m_iDepth = iDepth;

	strcpy_s(m_szName, pName);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	m_pParent = pParent;

	Safe_AddRef(m_pParent);

	return S_OK;
}

HRESULT CHierarchyNode::NativeConstruct_ByDat(const char * pName, _fmatrix TransformationMatrix, _fmatrix OffsetMatrix, const char * pTargetName, _uint iDepth)
{
	m_iDepth = iDepth;

	strcpy_s(m_szName, pName);

	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);

	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(TransformationMatrix));

	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

	strcpy_s(m_szTargetName, pTargetName);

	//Safe_AddRef(m_pParent);

	return S_OK;
}

HRESULT CHierarchyNode::Reserve_Channels(_uint iNumAnimation)
{
	m_AnimChannels.resize(iNumAnimation);	

	return S_OK;
}

void CHierarchyNode::Add_Channel(_uint iAnimIndex, CChannel * pChannel)
{
	m_AnimChannels[iAnimIndex] = pChannel;
	Safe_AddRef(pChannel);
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iCurrentAnimIndex)
{
	if (iCurrentAnimIndex >= m_AnimChannels.size())
		return;

	if (nullptr != m_AnimChannels[iCurrentAnimIndex])
	{
		XMStoreFloat4x4(&m_TransformationMatrix, m_AnimChannels[iCurrentAnimIndex]->Get_TransformationMatrix());

		if (nullptr != m_pParent)
			XMStoreFloat4x4(&m_CombinedTransformationMatrix,
				XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
		else
			m_CombinedTransformationMatrix = m_TransformationMatrix;
	}
}

CHierarchyNode * CHierarchyNode::Create(const char * pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode* pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, pParent, iDepth)))
	{
		MSGBOX("Failed To Creating CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CHierarchyNode * CHierarchyNode::Create_ByDat(const char * pName, _fmatrix TransformationMatrix, _fmatrix OffsetMatrix, const char * pTargetName, _uint iDepth)
{
	CHierarchyNode* pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct_ByDat(pName, TransformationMatrix, OffsetMatrix, pTargetName, iDepth)))
	{
		MSGBOX("Failed To Creating CHierarchyNode");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchyNode::Free()
{
	for (auto& pChannel : m_AnimChannels)
		Safe_Release(pChannel);

	m_AnimChannels.clear();


	Safe_Release(m_pParent);
}

