#include "..\Public\OcMeshContainer.h"
#include "GameInstance.h"
#include "Frustum.h"
#include "PipeLine.h"

COcMeshContainer::COcMeshContainer(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice), m_bCloned(false)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

COcMeshContainer::COcMeshContainer(const COcMeshContainer & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice), m_pContextDevice(rhs.m_pContextDevice)
	, m_iNumVertices(rhs.m_iNumVertices), m_iStartVertexIndex(rhs.m_iStartVertexIndex)
	, m_iStartFaceIndex(rhs.m_iStartFaceIndex), m_iNumFaces(rhs.m_iNumFaces), m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_bCloned(true)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT COcMeshContainer::NativeConstruct(_uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex)
{
	m_iNumVertices = iNumVertices;
	m_iStartVertexIndex = iStartVertexIndex;
	m_iStartFaceIndex = iStartFaceIndex;
	m_iNumFaces = iNumFaces;

	m_iMaterialIndex = iMaterialIndex;

	return S_OK;
}

void COcMeshContainer::Culling()
{
	//CFrustum* pFrustum = CFrustum::Get_Instance();
	//
	//

	//if (pFrustum->Culling_BoundingBox(m_pBoundingBox))
	//{
	//	m_bCulling = false;
	//}
	//else
	//{
	//	for (_uint i = 0; i < 8; ++i)
	//	{
	//		if (pFrustum->Culling_BoundingBox(m_pChildBoundingBox[i]))
	//		{
	//			m_bCulling = false;
	//			break;
	//		}
	//	}

	//	if (m_bCulling)
	//	{
	//		if (pFrustum->Culling_PointRadius(XMVectorSetW(XMLoadFloat3(&m_vCenter), 1.f), m_fRadius * 2.f))
	//		{
	//			m_bCulling = false;
	//		}
	//		else
	//		{
	//			m_bCulling = true;
	//		}
	//	}
		/*if(pFrustum->Culling_BoundingBox(m_pChildBoundingBox, 8))
		{
			m_bCulling = false;
		}
		else 
		{
			if (pFrustum->Culling_PointRadius(XMVectorSetW(XMLoadFloat3(&m_vCenter), 1.f), m_fRadius))
			{
				m_bCulling = false;
			}
			else
			{
				m_bCulling = true;
			}
		}*/
	
}

HRESULT COcMeshContainer::Render()
{
	m_pContextDevice->DrawIndexed(m_iNumFaces * 3, m_iStartFaceIndex * 3, m_iStartVertexIndex);

	return S_OK;
}

HRESULT COcMeshContainer::SetUp_CombinedMatrices(_matrix * pMatrices, _fmatrix PivotMatrix)
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

void COcMeshContainer::SetUp_BoundingBoxPoint(const _vector & vMaxPoint, const _vector & vMinPoint)
{
	XMStoreFloat3(&m_vMaxPoint, vMaxPoint);
	XMStoreFloat3(&m_vMinPoint, vMinPoint);


	XMStoreFloat3(&m_vCenter, (vMinPoint + vMaxPoint) * 0.5f);
	XMStoreFloat3(&m_vExtents, (vMaxPoint - vMinPoint) * 0.5f);

	m_fRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_vExtents)));

	m_pBoundingBox = new BoundingBox(m_vCenter, m_vExtents);

	_vector vCenter, vHalfExtents;

	vCenter = XMVectorSetW(XMLoadFloat3(&m_vCenter), 1.f);
	vHalfExtents = XMLoadFloat3(&m_vExtents) / 2.f;

	SetUp_ChildBoundingBox(vCenter, vHalfExtents);

}

void COcMeshContainer::SetUp_ChildBoundingBox(const _vector & vCenter, const _vector & vHalfExtents)
{
	_float3 vChildCenter, vChildExtents;

	_float3 vPreCenter;

	XMStoreFloat3(&vPreCenter, vCenter);
	XMStoreFloat3(&vChildExtents, vHalfExtents);
	
	// Left Front Top
	vChildCenter = _float3(
		vPreCenter.x - vChildExtents.x,
		vPreCenter.y + vChildExtents.y,
		vPreCenter.z + vChildExtents.z
	);
	m_pChildBoundingBox[0] = new BoundingBox(vChildCenter, vChildExtents);
	
	// Right Front Top
	vChildCenter = _float3(
		vPreCenter.x + vChildExtents.x,
		vPreCenter.y + vChildExtents.y,
		vPreCenter.z + vChildExtents.z
	);
	m_pChildBoundingBox[1] = new BoundingBox(vChildCenter, vChildExtents);

	// Right Back Top
	vChildCenter = _float3(
		vPreCenter.x + vChildExtents.x,
		vPreCenter.y + vChildExtents.y,
		vPreCenter.z - vChildExtents.z
	);
	m_pChildBoundingBox[2] = new BoundingBox(vChildCenter, vChildExtents);

	// Left Back Top
	vChildCenter = _float3(
		vPreCenter.x - vChildExtents.x,
		vPreCenter.y + vChildExtents.y,
		vPreCenter.z - vChildExtents.z
	);
	m_pChildBoundingBox[3] = new BoundingBox(vChildCenter, vChildExtents);

	// Left Front Bottom
	vChildCenter = _float3(
		vPreCenter.x - vChildExtents.x,
		vPreCenter.y - vChildExtents.y,
		vPreCenter.z + vChildExtents.z
	);
	m_pChildBoundingBox[4] = new BoundingBox(vChildCenter, vChildExtents);

	// Right Front Bottom
	vChildCenter = _float3(
		vPreCenter.x + vChildExtents.x,
		vPreCenter.y - vChildExtents.y,
		vPreCenter.z + vChildExtents.z
	);
	m_pChildBoundingBox[5] = new BoundingBox(vChildCenter, vChildExtents);

	// Right Back Bottom
	vChildCenter = _float3(
		vPreCenter.x + vChildExtents.x,
		vPreCenter.y - vChildExtents.y,
		vPreCenter.z - vChildExtents.z
	);
	m_pChildBoundingBox[6] = new BoundingBox(vChildCenter, vChildExtents);

	// Left Back Bottom
	vChildCenter = _float3(
		vPreCenter.x - vChildExtents.x,
		vPreCenter.y - vChildExtents.y,
		vPreCenter.z - vChildExtents.z
	);
	m_pChildBoundingBox[7] = new BoundingBox(vChildCenter, vChildExtents);

}

COcMeshContainer * COcMeshContainer::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _uint iStartVertexIndex, _uint iNumVertices, _uint iStartFaceIndex, _uint iNumFaces, _uint iMaterialIndex)
{
	COcMeshContainer* pInstance = new COcMeshContainer(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct(iStartVertexIndex, iNumVertices
		, iStartFaceIndex, iNumFaces, iMaterialIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

COcMeshContainer * COcMeshContainer::Clone()
{
	COcMeshContainer* pInstance = new COcMeshContainer(*this);
	
	return pInstance;
}

void COcMeshContainer::Free()
{
	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);

	if (m_bCloned)
	{
		Safe_Delete(m_pBoundingBox);
		for(_uint i =0; i<8; ++i)
			Safe_Delete(m_pChildBoundingBox[i]);
	}
}
