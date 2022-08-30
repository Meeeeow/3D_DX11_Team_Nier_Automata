#include "..\Public\MeshContainer.h"
#include "HierarchyNode.h"


CMeshContainer::CMeshContainer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(pGraphicDevice);
	Safe_AddRef(pContextDevice);
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice), m_pContextDevice(rhs.m_pContextDevice)
	, m_iNumVertices(rhs.m_iNumVertices), m_iStartVertexIndex(rhs.m_iStartVertexIndex)
	, m_iStartFaceIndex(rhs.m_iStartFaceIndex), m_iNumFaces(rhs.m_iNumFaces), m_iMaterialIndex(rhs.m_iMaterialIndex)
{
	Safe_AddRef(m_pContextDevice);
	Safe_AddRef(m_pGraphicDevice);
}

HRESULT CMeshContainer::NativeConstruct(_uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex)
{
	m_iNumVertices = iNumVertices;
	m_iStartVertexIndex = iStartVertexIndex;
	m_iStartFaceIndex = iStartFaceIndex;
	m_iNumFaces = iNumFaces;

	m_iMaterialIndex = iMaterialIndex;

	return S_OK;
}

HRESULT CMeshContainer::Render()
{
	m_pContextDevice->DrawIndexed(m_iNumFaces * 3, m_iStartFaceIndex * 3, m_iStartVertexIndex);
	
	return S_OK;
}

HRESULT CMeshContainer::SetUp_CombinedMatrices(_matrix* pMatrices, _fmatrix PivotMatrix)
{
	_uint		iIndex = 0;

	_matrix		OffsetMatrix, CombinedTransformationMatrix;

	for (auto& pHierarchyNode : m_Bones)
	{
		OffsetMatrix = pHierarchyNode->Get_OffsetMatrix();
		CombinedTransformationMatrix = pHierarchyNode->Get_CombinedTransformationMatrix();

		pMatrices[iIndex++] = XMMatrixTranspose(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix);
	}

	return S_OK;
}

const CHierarchyNode * CMeshContainer::Get_HierarchyNode(const char* pNodeTag)
{
	for (auto& pIter : m_Bones)
	{
		if (!strcmp(pIter->Get_Name(), pNodeTag))
		{
			return pIter;
		}
	}
	return nullptr;
}
HRESULT CMeshContainer::SetUp_CombineBlurMatrices(_matrix * pMatrices, _matrix * pPreMatrices, _fmatrix matPivot)
{
	_uint			iIndex = 0;
	_matrix			matOffset, matCombinedTransformation;

	_uint			iBoneSize = (_uint)m_Bones.size();
	for (_uint i = 0; i < iBoneSize; ++i)
	{
		auto& iter = m_Bones[i];

		matOffset = iter->Get_OffsetMatrix();

		matCombinedTransformation = iter->Get_CombinedTransformationMatrix();

		pMatrices[i] = XMMatrixTranspose(matOffset * matCombinedTransformation * matPivot);
		if (i == 0)
		{
			pPreMatrices[0] = pMatrices[0];
		}
		else
		{
			pPreMatrices[i] = pMatrices[i-1];
		}
	}

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex)
{
	CMeshContainer* pInstance = new CMeshContainer(pGraphicDevice, pContextDevice);

	if (FAILED(pInstance->NativeConstruct(iStartVertexIndex, iNumVertices, iStartFaceIndex, iNumFaces, iMaterialIndex)))
	{
		MSGBOX("Failed To Creating CMeshContainer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeshContainer * CMeshContainer::Clone()
{
	CMeshContainer*		pMeshContainer = new CMeshContainer(*this);


	return pMeshContainer;
}

void CMeshContainer::Free()
{
	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);	
}
