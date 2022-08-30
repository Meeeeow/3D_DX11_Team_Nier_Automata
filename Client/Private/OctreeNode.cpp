#include "..\Public\OctreeNode.h"
#include "Model.h"
#include "DebugDraw.h"
#include "PipeLine.h"

COctreeNode::COctreeNode(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT COctreeNode::Make_BoundingBox(_float3 vMiddlePoint, _float3 vMaxPoint)
{
	m_pBoundingBox = new BoundingBox(vMiddlePoint, vMaxPoint);
	if (m_pBoundingBox == nullptr)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pGraphicDevice);
	if (m_pEffect == nullptr)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = nullptr;
	size_t			iShaderByteCodeLength = 0;
	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	if (FAILED(m_pGraphicDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;

	m_pBatch = new DirectX::PrimitiveBatch<DirectX::VertexPositionColor>(m_pContextDevice);
	if (m_pBatch == nullptr)
		return E_FAIL;

	return S_OK;
}

void COctreeNode::Make_Child(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDeivce, _uint iIndex, _float3 vMiddlePoint, _float3 vMaxPoint, void* pVertices, void* pIndices, vector<_uint>* pVecIndex, _uint iDepth, _uint iMaxDepth)
{
	if (iDepth == iMaxDepth)
		return;

	m_pChildren[iIndex] = COctreeNode::Create(pGraphicDevice, pContextDeivce, vMiddlePoint, vMaxPoint,pVertices ,pIndices,pVecIndex, ++iDepth, iMaxDepth);
}

_int COctreeNode::LateTick(_double dTimeDelta)
{
	if(!m_vecIndicesIndex.empty())
		size_t iSize = m_vecIndicesIndex.size();

	for (_uint i = 0; i < 8; ++i)
	{
		if(m_pChildren[i] != nullptr)
			m_pChildren[i]->LateTick(dTimeDelta);
	}

	return _int();
}

HRESULT COctreeNode::Render()
{
	CPipeLine*	pPipeLine = CPipeLine::Get_Instance();
	m_pBatch->Begin();

	m_pContextDevice->IASetInputLayout(m_pInputLayout);

	// View, Proj 행렬 세팅
	m_pEffect->SetView(pPipeLine->Get_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_Transform(CPipeLine::TRANSFORM::D3DTS_PROJ));

	m_pEffect->Apply(m_pContextDevice);

	DX::Draw(m_pBatch, *m_pBoundingBox, Colors::LightPink);

	m_pBatch->End();

	for (_uint i = 0; i < 8; ++i)
	{
		if (m_pChildren[i] != nullptr)
			m_pChildren[i]->Render();
	}

	return S_OK;
}

COctreeNode * COctreeNode::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, CModel * pModel, _uint iMaxDepth)
{
	// 최초 부모 옥트리 노드
	COctreeNode* pInstance = new COctreeNode(pGraphicDevice, pContextDevice);

	if (FAILED(pInstance->Make_BoundingBox(pModel->Get_MiddlePoint(), pModel->Get_MaxPoint())))
	{
		Safe_Release(pInstance);
		return pInstance;
	}



	void* pVertices = pModel->Get_Vertices();
	void* pIndices = pModel->Get_Indices();

	_uint iPrimitive = pModel->Get_Primitive();

	for (_uint i = 0; i < iPrimitive; ++i)
	{
		_float3 v1 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[i]._1]).vPosition;
		_float3 v2 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[i]._2]).vPosition;
		_float3 v3 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[i]._3]).vPosition;

		if (pInstance->Get_BoundingBox()->Contains(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3)) == ContainmentType::INTERSECTS
			|| pInstance->Get_BoundingBox()->Contains(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3)) == ContainmentType::CONTAINS)
		{
			vector<_uint>* pVector = pInstance->Get_IndiciesIndexVector();
			pVector->push_back(i);
		}
	}

	_uint iDepth = 0;
	_float3	vMiddlePoint = pModel->Get_MiddlePoint(), vMaxPoint = pModel->Get_MaxPoint();
	_float3 vChildrenMiddlePoint, vChildrenMaxPoint;
	vChildrenMaxPoint = _float3(vMaxPoint.x / 2.f, vMaxPoint.y / 2.f, vMaxPoint.z / 2.f);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x - (vMaxPoint.x / 2.f)
		, vMiddlePoint.y + (vMaxPoint.y / 2.f)
		, vMiddlePoint.z + (vMaxPoint.z / 2.f));
	pInstance->Make_Child(pGraphicDevice, pContextDevice, 0, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x + (vMaxPoint.x / 2.f)
		, vMiddlePoint.y + (vMaxPoint.y / 2.f)
		, vMiddlePoint.z + (vMaxPoint.z / 2.f));
	pInstance->Make_Child(pGraphicDevice, pContextDevice, 1, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x + (vMaxPoint.x / 2.f)
		, vMiddlePoint.y + (vMaxPoint.y / 2.f)
		, vMiddlePoint.z - (vMaxPoint.z / 2.f));
	pInstance->Make_Child(pGraphicDevice, pContextDevice, 2, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x - (vMaxPoint.x / 2.f)
		, vMiddlePoint.y + (vMaxPoint.y / 2.f)
		, vMiddlePoint.z - (vMaxPoint.z / 2.f));
	pInstance->Make_Child(pGraphicDevice, pContextDevice, 3, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x - (vMaxPoint.x / 2.f)
		, vMiddlePoint.y - (vMaxPoint.y / 2.f)
		, vMiddlePoint.z + (vMaxPoint.z / 2.f));
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 4, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x + (vMaxPoint.x / 2.f)
		, vMiddlePoint.y - (vMaxPoint.y / 2.f)
		, vMiddlePoint.z + (vMaxPoint.z / 2.f));
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 5, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x + (vMaxPoint.x / 2.f)
		, vMiddlePoint.y - (vMaxPoint.y / 2.f)
		, vMiddlePoint.z - (vMaxPoint.z / 2.f));
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 6, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	vChildrenMiddlePoint = _float3(
		vMiddlePoint.x - (vMaxPoint.x / 2.f)
		, vMiddlePoint.y - (vMaxPoint.y / 2.f)
		, vMiddlePoint.z - (vMaxPoint.z / 2.f));
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 7, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	return pInstance;
}

COctreeNode * COctreeNode::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _float3 vMiddlePoint, _float3 vMaxPoint, void* pVertices, void* pIndices, vector<_uint>* vecIndex, _uint iDepth, _uint iMaxDepth)
{
	COctreeNode* pInstance = new COctreeNode(pGraphicDevice, pContextDevice);

	if (FAILED(pInstance->Make_BoundingBox(vMiddlePoint, vMaxPoint)))
	{
		Safe_Release(pInstance);
		return pInstance;
	}
	
	pInstance->Set_Depth(iDepth);

	for (auto& iter : *vecIndex)
	{
		_float3 v1 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[iter]._1]).vPosition;
		_float3 v2 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[iter]._2]).vPosition;
		_float3 v3 = (((VTXMESH*)pVertices)[((FACEINDICES32*)pIndices)[iter]._3]).vPosition;

		if (pInstance->Get_BoundingBox()->Contains(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3)) == ContainmentType::INTERSECTS
			|| pInstance->Get_BoundingBox()->Contains(XMLoadFloat3(&v1), XMLoadFloat3(&v2), XMLoadFloat3(&v3)) == ContainmentType::CONTAINS)
		{
			vector<_uint>* pVector = pInstance->Get_IndiciesIndexVector();
			pVector->push_back(iter);
		}
	}
	if (pInstance->Get_IndiciesIndexVector()->size() != 0)
	{
		_float3 vChildrenMiddlePoint;
		_float3 vChildrenMaxPoint = _float3(vMaxPoint.x / 2.f, vMaxPoint.y / 2.f, vMaxPoint.z / 2.f);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x - vChildrenMaxPoint.x
			, vMiddlePoint.y + vChildrenMaxPoint.y
			, vMiddlePoint.z + vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 0, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x + vChildrenMaxPoint.x
			, vMiddlePoint.y + vChildrenMaxPoint.y
			, vMiddlePoint.z + vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 1, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x + vChildrenMaxPoint.x
			, vMiddlePoint.y + vChildrenMaxPoint.y
			, vMiddlePoint.z - vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 2, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x - vChildrenMaxPoint.x
			, vMiddlePoint.y + vChildrenMaxPoint.y
			, vMiddlePoint.z - vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 3, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x - vChildrenMaxPoint.x
			, vMiddlePoint.y - vChildrenMaxPoint.y
			, vMiddlePoint.z + vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 4, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x + vChildrenMaxPoint.x
			, vMiddlePoint.y - vChildrenMaxPoint.y
			, vMiddlePoint.z + vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 5, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x + vChildrenMaxPoint.x
			, vMiddlePoint.y - vChildrenMaxPoint.y
			, vMiddlePoint.z - vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 6, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

		vChildrenMiddlePoint = _float3(
			vMiddlePoint.x - vChildrenMaxPoint.x
			, vMiddlePoint.y - vChildrenMaxPoint.y
			, vMiddlePoint.z - vChildrenMaxPoint.z);
		pInstance->Make_Child(pGraphicDevice, pContextDevice, 7, vChildrenMiddlePoint, vChildrenMaxPoint, pVertices, pIndices, pInstance->Get_IndiciesIndexVector(), iDepth, iMaxDepth);

	}
	return pInstance;
}

void COctreeNode::Free()
{
	if(m_pBoundingBox != nullptr)
		Safe_Delete(m_pBoundingBox);

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);

	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);

	for (_uint i = 0; i < 8; ++i)
		Safe_Release(m_pChildren[i]);
}
