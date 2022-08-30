#pragma once
#ifndef __ENGINE_FRUSTUM_H__
#define __ENGINE_FRUSTUM_H__

#include "Base.h"
BEGIN(Engine)
class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)

private:
	CFrustum();
	virtual ~CFrustum() DEFAULT;

	enum class FRUSTUM_PLANE : _uint {
		NEARPLANE		= 0,
		FARPLANE		= 1,
		RIGHTPLANE		= 2,
		LEFTPLANE		= 3,
		TOPPLANE		= 4,
		BOTTOMPLANE		= 5,
		COUNT			= 6
	};

	enum class FRUSTUM_POINT : _uint {
		NEAR_LEFT_TOP		= 0,
		NEAR_RIGHT_TOP		= 1,
		NEAR_RIGHT_BOTTOM	= 2,
		NEAR_LEFT_BOTTOM	= 3,
		FAR_LEFT_TOP		= 4,
		FAR_RIGHT_TOP		= 5,
		FAR_RIGHT_BOTTOM	= 6,
		FAR_LEFT_BOTTOM		= 7,
		COUNT				= 8
	};

public:
	HRESULT					Initialize();
	void					Make_Frustum();

public:
	_bool					Culling_Radius(class CTransform* pTransform, _float fRadius);
	// Moveable
	_bool					Culling_PointRadius(_fvector vPos, _matrix matWorld, _float fRadius);
	// Non Moveable
	_bool					Culling_PointRadius(_fvector vPos, _float fRadius);

	// Bounding Box
	_bool					Culling_BoundingBox(BoundingBox* pBoundingBox);
	_bool					Culling_BoundingBox(BoundingBox* pBoundingBox[], _uint iChildNum);

private:
	_float3					m_vOriginPoint[(_uint)FRUSTUM_POINT::COUNT];
	_float3					m_vPoint[(_uint)FRUSTUM_POINT::COUNT];


	_float4					m_vPlane[(_uint)FRUSTUM_PLANE::COUNT];

public:
	virtual void			Free() override;

};
END
#endif // !__ENGINE_FRUSTUM_H__