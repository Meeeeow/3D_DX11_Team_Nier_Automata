#include "..\Public\Cell.h"
#include "PipeLine.h"
#include "VIBuffer_TriangleLineStrip.h"
#include "Frustum.h"
#include "PipeLine.h"

CCell::CCell(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pGraphicDevice);
	Safe_AddRef(m_pContextDevice);
}

HRESULT CCell::NativeConstruct_Prototype(_float3 * pPoints, NAVINFO::OPTION eOption, _uint iIndex)
{
	m_iIndex = iIndex;
	memcpy(m_vPoints, pPoints, sizeof(_float3) * (_uint)POINT::COUNT);
	m_eOption = eOption;

	m_vLineCenterPos[(_uint)LINE::AB].x = (m_vPoints[(_uint)POINT::A].x + m_vPoints[(_uint)POINT::B].x) / 2.f;
	m_vLineCenterPos[(_uint)LINE::AB].y = (m_vPoints[(_uint)POINT::A].y + m_vPoints[(_uint)POINT::B].y) / 2.f;
	m_vLineCenterPos[(_uint)LINE::AB].z = (m_vPoints[(_uint)POINT::A].z + m_vPoints[(_uint)POINT::B].z) / 2.f;

	m_vLineCenterPos[(_uint)LINE::BC].x = (m_vPoints[(_uint)POINT::B].x + m_vPoints[(_uint)POINT::C].x) / 2.f;
	m_vLineCenterPos[(_uint)LINE::BC].y = (m_vPoints[(_uint)POINT::B].y + m_vPoints[(_uint)POINT::C].y) / 2.f;
	m_vLineCenterPos[(_uint)LINE::BC].z = (m_vPoints[(_uint)POINT::B].z + m_vPoints[(_uint)POINT::C].z) / 2.f;

	m_vLineCenterPos[(_uint)LINE::CA].x = (m_vPoints[(_uint)POINT::C].x + m_vPoints[(_uint)POINT::A].x) / 2.f;
	m_vLineCenterPos[(_uint)LINE::CA].y = (m_vPoints[(_uint)POINT::C].y + m_vPoints[(_uint)POINT::A].y) / 2.f;
	m_vLineCenterPos[(_uint)LINE::CA].z = (m_vPoints[(_uint)POINT::C].z + m_vPoints[(_uint)POINT::A].z) / 2.f;

	m_vCenter.x = (m_vPoints[(_uint)POINT::A].x + m_vPoints[(_uint)POINT::B].x + m_vPoints[(_uint)POINT::C].x) / 3.f;
	m_vCenter.y = (m_vPoints[(_uint)POINT::A].y + m_vPoints[(_uint)POINT::B].y + m_vPoints[(_uint)POINT::C].y) / 3.f;
	m_vCenter.z = (m_vPoints[(_uint)POINT::A].z + m_vPoints[(_uint)POINT::B].z + m_vPoints[(_uint)POINT::C].z) / 3.f;
	m_vCenter.w = 1.f;

	m_fRadius = max(
		XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vCenter) - XMLoadFloat3(&m_vPoints[(_uint)POINT::A])))
		, XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vCenter) - XMLoadFloat3(&m_vPoints[(_uint)POINT::B]))));
	m_fRadius = max(
		m_fRadius
		, XMVectorGetX(XMVector4Length(XMLoadFloat4(&m_vCenter) - XMLoadFloat3(&m_vPoints[(_uint)POINT::C]))));

	ZeroMemory(m_pNeighbor, sizeof(CCell*) * (_uint)LINE::COUNT);
	if (FAILED(Ready_Buffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCell::Update(_fmatrix matWorld)
{
	for (_uint i = 0; i < (_uint)POINT::COUNT; ++i)
		XMStoreFloat3(&m_vPointsInWorld[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), matWorld));

	XMStoreFloat3(&m_vLine[(_uint)LINE::AB], XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]));
	XMStoreFloat3(&m_vLine[(_uint)LINE::BC], XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]));
	XMStoreFloat3(&m_vLine[(_uint)LINE::CA], XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]));

	return S_OK;
}

HRESULT CCell::IsCulling()
{
	m_bCulling = false;
	if (m_eOption == NAVINFO::OPTION::FALL)
		m_bCulling = true;

	if (false == m_bCulling)
	{
		_float fDistance = XMVectorGetX(XMVector4Length(CPipeLine::Get_Instance()->Get_CameraPosition() - XMLoadFloat4(&m_vCenter)));
		if (fDistance >= 45.f)
			m_bCulling = true;

		if (CFrustum::Get_Instance()->Culling_PointRadius(XMLoadFloat4(&m_vCenter), m_fRadius) == false && m_bCulling == false)
			m_bCulling = true;
	}
	

	return S_OK;
}

_bool CCell::Compare(_fvector vSourPoint, _fvector vDestPoint)
{
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::A]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::B]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::C]), vDestPoint))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::B]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::C]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::A]), vDestPoint))
			return true;
	}
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::C]), vSourPoint))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::A]), vDestPoint))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[(_uint)POINT::B]), vDestPoint))
			return true;
	}

	return false;
}

_bool CCell::IsIn(_vector & vOutPosition, CCell ** ppOut, CCell** ppSlideOut)
{
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector vDir = XMVector3Normalize(vOutPosition - XMLoadFloat3(&m_vPointsInWorld[i]));

		_vector vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));

		if (0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*ppOut = m_pNeighbor[i];
			*ppSlideOut = this;
			return false;
		}
	}
	
	if (m_eOption != NAVINFO::OPTION::FALL)
		vOutPosition = Get_CorrectPosition(vOutPosition);

	return true;
}

_bool CCell::IsInSetPosition(_vector & vOutPosition, CCell ** ppOut, CCell ** ppSlideOut)
{
	return _bool();
}

_bool CCell::IsOver(_vector & vOutPosition)
{
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector vDir = XMVector3Normalize(vOutPosition - XMLoadFloat3(&m_vPointsInWorld[i]));
		_vector vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));
		_float fHeightDistance = m_vCenter.y - XMVectorGetY(vOutPosition);
		if ((0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal))))
		{
			return false;
		}
	}
	//vOutPosition = Get_CorrectPosition(vOutPosition);
	return true;
}

_bool CCell::IsInForCompare(_vector & vPosition, _vector * pOut, CCell ** ppOut)
{
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));

		_vector vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));

		if (0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
		{
			*ppOut = m_pNeighbor[i];
			return false;
		}
	}

	*pOut = Get_CorrectPosition(vPosition);

	if (XMVector4Equal(XMVectorZero(), *pOut))
		return false;

	return true;
}

_bool CCell::IsHigher(_vector vPosition, _uint & iIndex, const _uint iCurrentIndex)
{
	if (m_eOption == NAVINFO::OPTION::FALL || m_bCulling)
		return false;

	if (iCurrentIndex == m_iIndex)
		return false;

	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));
		_vector vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));
		_float fHeightDistance = m_vCenter.y - XMVectorGetY(vPosition);

		if (0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal)) && fHeightDistance < 2.f)
		{
			return false;
		}
	}
	iIndex = m_iIndex;
	return true;
}

_vector CCell::IsSlide(_fvector & vPosition)
{
	_float fAngle[(_uint)LINE::COUNT] = { 0.f };
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));
		_vector		vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.0f, m_vLine[i].x, 0.f));
		fAngle[i] = XMVectorGetX(XMVector3Dot(vDir, vNormal));
	}

	_float fMax = max(fAngle[(_uint)LINE::AB], fAngle[(_uint)LINE::BC]);
	fMax = max(fMax, fAngle[(_uint)LINE::CA]);


	if (fMax == fAngle[(_uint)LINE::AB])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::AB].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::AB].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::AB].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::AB].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}
	else if (fMax == fAngle[(_uint)LINE::BC])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::BC].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::BC].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::BC].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::BC].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}
	else if(fMax == fAngle[(_uint)LINE::CA])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::CA].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::CA].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::CA].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::CA].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}

	return XMVectorZero();
}

_bool CCell::IsJump(_fvector & vPosition, CCell ** ppOut, CCell ** ppSlideOut)
{
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));

		_vector vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.f, m_vLine[i].x, 0.f));

		if ((0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal))))
		{
			*ppOut = m_pNeighbor[i];
			*ppSlideOut = this;
			return false;
		}
	}

	return true;
}

_vector CCell::IsJumpSlide(_fvector & vPosition)
{
	_float fAngle[(_uint)LINE::COUNT] = { 0.f };
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));
		_vector		vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0.0f, m_vLine[i].x, 0.f));
		fAngle[i] = XMVectorGetX(XMVector3Dot(vDir, vNormal));
	}

	_float fMax = max(fAngle[(_uint)LINE::AB], fAngle[(_uint)LINE::BC]);
	fMax = max(fMax, fAngle[(_uint)LINE::CA]);


	if (fMax == fAngle[(_uint)LINE::AB])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::AB].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::AB].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::AB].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::AB].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}
	else if (fMax == fAngle[(_uint)LINE::BC])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::BC].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::BC].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::BC].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::BC].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::B]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}
	else if (fMax == fAngle[(_uint)LINE::CA])
	{
		_float fDiffX = m_vLineCenterPos[(_uint)LINE::CA].x - XMVectorGetX(vPosition);
		_float fDiffZ = m_vLineCenterPos[(_uint)LINE::CA].z - XMVectorGetZ(vPosition);
		if (fabs(fDiffX) > fabs(fDiffZ))
		{
			if (m_vLineCenterPos[(_uint)LINE::CA].x > XMVectorGetX(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
		else
		{
			if (m_vLineCenterPos[(_uint)LINE::CA].z > XMVectorGetZ(vPosition))
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
			else
			{
				_vector vDir = XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::A]) - XMLoadFloat3(&m_vPointsInWorld[(_uint)POINT::C]);
				_vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
				return XMVector3Normalize(XMVector3Cross(vDir, vUp));
			}
		}
	}

	return XMVectorZero();
}

_bool CCell::Check_CurrentCell(_vector vPosition)
{
	for (_uint i = 0; i < (_uint)LINE::COUNT; ++i)
	{
		_vector		vDir = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPointsInWorld[i]));
		_vector		vNormal = XMVector3Normalize(XMVectorSet(m_vLine[i].z * -1.f, 0, m_vLine[i].x, 0.f));

		if ((0.f < XMVectorGetX(XMVector3Dot(vDir, vNormal))))
		{
			return false;
		}
	}
	return true;
}

_vector CCell::Get_CorrectPosition(_fvector vPosition)
{
	_fvector vDirDownPos = XMVectorSet(XMVectorGetX(vPosition), XMVectorGetY(vPosition) - 1.f, XMVectorGetZ(vPosition), 1.f);
	_fvector vDirUpPos = XMVectorSet(XMVectorGetX(vPosition), XMVectorGetY(vPosition) + 1.f, XMVectorGetZ(vPosition), 1.f);

	_vector vDownRay = vDirDownPos - vPosition;
	_vector vUpRay = vDirUpPos - vPosition;

	vDownRay = XMVectorSetW(vDownRay, 0.f);
	vUpRay = XMVectorSetW(vUpRay, 0.f);

	_float fDist = 0.0f;

	FXMVECTOR vPointA = XMVectorSet(m_vPointsInWorld[(_uint)POINT::A].x, m_vPointsInWorld[(_uint)POINT::A].y, m_vPointsInWorld[(_uint)POINT::A].z, 1.f);
	GXMVECTOR vPointB = XMVectorSet(m_vPointsInWorld[(_uint)POINT::B].x, m_vPointsInWorld[(_uint)POINT::B].y, m_vPointsInWorld[(_uint)POINT::B].z, 1.f);
	HXMVECTOR vPointC = XMVectorSet(m_vPointsInWorld[(_uint)POINT::C].x, m_vPointsInWorld[(_uint)POINT::C].y, m_vPointsInWorld[(_uint)POINT::C].z, 1.f);

	if (TriangleTests::Intersects(vPosition, vDownRay, vPointA, vPointB, vPointC, fDist))
		return vPosition + XMVector3Normalize(vDownRay) * fDist;
	else if (TriangleTests::Intersects(vPosition, vUpRay, vPointA, vPointB, vPointC, fDist))
		return vPosition + XMVector3Normalize(vUpRay) * fDist;

	_vector vTemp = XMVectorZero();
	return vTemp;
}

const _float4 & CCell::Get_CenterPosition()
{
	return m_vCenter;
}

HRESULT CCell::Ready_Buffer()
{
	m_pVIBuffer = CVIBuffer_TriangleLineStrip::Create(m_pGraphicDevice, m_pContextDevice, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), "DefaultTechnique", m_vPoints);
	if (m_pVIBuffer == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CCell::Render(_fmatrix matWorld)
{
	if (m_bCulling)
		return S_OK;

	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	m_pVIBuffer->SetUp_RawValue("g_matWorld", &XMMatrixTranspose(matWorld), sizeof(_matrix));
	m_pVIBuffer->SetUp_RawValue("g_matView", &pPipeLine->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_VIEW), sizeof(_matrix));
	m_pVIBuffer->SetUp_RawValue("g_matProj", &pPipeLine->Get_Transpose(CPipeLine::TRANSFORM::D3DTS_PROJ), sizeof(_matrix));

	m_pVIBuffer->Render();

	return S_OK;
}

CCell * CCell::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _float3 * pPoints, NAVINFO::OPTION eOption, _uint iIndex)
{
	CCell* pInstance = new CCell(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(pPoints, eOption, iIndex)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCell::Free()
{
	unsigned long dwRefCnt = 0;
	dwRefCnt = Safe_Release(m_pVIBuffer);
	dwRefCnt = Safe_Release(m_pContextDevice);
	dwRefCnt = Safe_Release(m_pGraphicDevice);
}
