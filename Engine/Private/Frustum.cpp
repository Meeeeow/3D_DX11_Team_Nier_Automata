#include "..\Public\Frustum.h"
#include "PipeLine.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize()
{
	m_vOriginPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_TOP]		= _float3(-1.f,  1.f, 0.f);	// Near Left Top
	m_vOriginPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_TOP]	= _float3( 1.f,  1.f, 0.f);	// Near Right Top
	m_vOriginPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_BOTTOM]	= _float3( 1.f, -1.f, 0.f);	// Near Right Bottom
	m_vOriginPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_BOTTOM]	= _float3(-1.f, -1.f, 0.f);	// Near Left Bottom

	m_vOriginPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_TOP]		= _float3(-1.f,  1.f, 1.f);	// Far Left Top
	m_vOriginPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_TOP]		= _float3( 1.f,  1.f, 1.f);	// Far Right Top
	m_vOriginPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_BOTTOM]	= _float3( 1.f, -1.f, 1.f);	// Far Right Bottom
	m_vOriginPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_BOTTOM]	= _float3(-1.f, -1.f, 1.f);	// Far Left Bottom

	return S_OK;
}

void CFrustum::Make_Frustum()
{
	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	_matrix		matViewInverse = pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_VIEW);
	_matrix		matProjInverse = pPipeLine->Get_Inverse(CPipeLine::TRANSFORM::D3DTS_PROJ);

	for (_uint i = 0; i < (_uint)FRUSTUM_POINT::COUNT; ++i)
	{
		XMStoreFloat3(&m_vPoint[i], XMVector3TransformCoord(XMLoadFloat3(&m_vOriginPoint[i]), matProjInverse));
		XMStoreFloat3(&m_vPoint[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), matViewInverse));
	}
	
	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::NEARPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_BOTTOM])));
	
	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::FARPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_BOTTOM])));

	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::RIGHTPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_BOTTOM])));

	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::LEFTPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_BOTTOM])));

	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::TOPPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_LEFT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_TOP]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_TOP])));

	XMStoreFloat4(&m_vPlane[(_uint)FRUSTUM_PLANE::BOTTOMPLANE],
		XMPlaneFromPoints(XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_LEFT_BOTTOM]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::NEAR_RIGHT_BOTTOM]),
			XMLoadFloat3(&m_vPoint[(_uint)FRUSTUM_POINT::FAR_RIGHT_BOTTOM])));

}

_bool CFrustum::Culling_Radius(CTransform * pTransform, _float fRadius)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE::POSITION);
	for (_uint i = 0; i < 6; ++i)
	{
		_vector vPlane = XMLoadFloat4(&m_vPlane[i]);
		if (XMVectorGetX(XMPlaneDotCoord(vPlane, vPos)) > fRadius)
			return false;
	}

	return true;
}

_bool CFrustum::Culling_PointRadius(_fvector vPos, _matrix matWorld, _float fRadius)
{
	_vector vWorldPos = XMVector4Transform(vPos, matWorld);

	for (_uint i = 0; i < 6; ++i)
	{
		_vector vPlane = XMLoadFloat4(&m_vPlane[i]);
		if (XMVectorGetX(XMPlaneDotCoord(vPlane, vWorldPos)) > fRadius)
			return false;
	}
	return true;
}

_bool CFrustum::Culling_PointRadius(_fvector vPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		_vector vPlane = XMLoadFloat4(&m_vPlane[i]);
		if (XMVectorGetX(XMPlaneDotCoord(vPlane, vPos)) > fRadius)
			return false;
	}
	return true;
}

_bool CFrustum::Culling_BoundingBox(BoundingBox * pBoundingBox)
{
	_vector vPlane[(_uint)FRUSTUM_PLANE::COUNT];
	for (_uint i = 0; i < (_uint)FRUSTUM_PLANE::COUNT; ++i)
	{
		vPlane[i] = XMLoadFloat4(&m_vPlane[i]);
	}

	ContainmentType eType = pBoundingBox->ContainedBy(
		vPlane[(_uint)FRUSTUM_PLANE::NEARPLANE]
		, vPlane[(_uint)FRUSTUM_PLANE::FARPLANE]
		, vPlane[(_uint)FRUSTUM_PLANE::RIGHTPLANE]
		, vPlane[(_uint)FRUSTUM_PLANE::LEFTPLANE]
		, vPlane[(_uint)FRUSTUM_PLANE::TOPPLANE]
		, vPlane[(_uint)FRUSTUM_PLANE::BOTTOMPLANE]);

	if (eType == ContainmentType::DISJOINT)
		return false;

	return true;
}

_bool CFrustum::Culling_BoundingBox(BoundingBox * pBoundingBox[], _uint iChildNum)
{
	_vector vPlane[(_uint)FRUSTUM_PLANE::COUNT];
	for (_uint i = 0; i < (_uint)FRUSTUM_PLANE::COUNT; ++i)
	{
		vPlane[i] = XMLoadFloat4(&m_vPlane[i]);
	}

	for (_uint j = 0; j < iChildNum; ++j)
	{
		ContainmentType eType = pBoundingBox[j]->ContainedBy(
			vPlane[(_uint)FRUSTUM_PLANE::NEARPLANE]
			, vPlane[(_uint)FRUSTUM_PLANE::FARPLANE]
			, vPlane[(_uint)FRUSTUM_PLANE::RIGHTPLANE]
			, vPlane[(_uint)FRUSTUM_PLANE::LEFTPLANE]
			, vPlane[(_uint)FRUSTUM_PLANE::TOPPLANE]
			, vPlane[(_uint)FRUSTUM_PLANE::BOTTOMPLANE]);

		if (eType != ContainmentType::DISJOINT)
			return true;
	}

	return false;
}

void CFrustum::Free()
{

}
