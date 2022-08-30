#pragma once
#ifndef __ENGINE_CELL_H__
#define __ENGINE_CELL_H__

#include "Base.h"
BEGIN(Engine)
class CCell final : public CBase
{
private:
	CCell(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CCell() DEFAULT;

public:
	enum class POINT : _uint {
		A = 0,
		B = 1,
		C = 2,
		COUNT = 3
	};
	enum class LINE : _uint {
		AB = 0,
		BC = 1,
		CA = 2,
		COUNT = 3
	};

public:
	HRESULT									NativeConstruct_Prototype(_float3* pPoints, NAVINFO::OPTION eOption, _uint iIndex);
	HRESULT									Update(_fmatrix matWorld);
	HRESULT									IsCulling();
	_bool									Compare(_fvector vSourPoint, _fvector vDestPoint);
	_bool									IsIn(_vector& vOutPosition, CCell** ppOut, CCell** ppSlideOut);
	_bool									IsInSetPosition(_vector& vOutPosition, CCell** ppOut, CCell** ppSlideOut);
	_bool									IsOver(_vector& vOutPosition);
	_bool									IsInForCompare(_vector& vPosition, _vector* pOut, CCell** ppOut);
	_bool									IsHigher(_vector vPosition, _uint& iIndex, const _uint iCurrentIndex);
	_vector									IsSlide(_fvector& vPosition);

	_bool									IsJump(_fvector& vPosition, CCell** ppOut, CCell** ppSlideOut);
	_vector									IsJumpSlide(_fvector& vPosition);

	_bool									Check_CurrentCell(_vector vPosition);

public:
	_vector									Get_CorrectPosition(_fvector vPosition);
	const _float4&							Get_CenterPosition();

public:
	_vector									Get_PointInLocal(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[(_uint)ePoint]); }
	_vector									Get_PointInWorld(POINT ePoint) {
		return XMLoadFloat3(&m_vPointsInWorld[(_uint)ePoint]); }
	_uint									Get_Index() const { return m_iIndex; }
	_bool									Get_Culling() const { return m_bCulling; }

public:
	void									Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_pNeighbor[(_uint)eLine] = pNeighbor; }
	void									Set_Culling(_bool bCulling) { m_bCulling = bCulling; }


private:
	_float3									m_vPoints[(_uint)POINT::COUNT];
	_float3									m_vPointsInWorld[(_uint)POINT::COUNT];
	_float3									m_vLine[(_uint)LINE::COUNT];
	_float3									m_vLineCenterPos[(_uint)LINE::COUNT];

	_float4									m_vCenter = _float4(0.f, 0.f, 0.f, 0.f);
	_float									m_fRadius = 0.f;
	_bool									m_bCulling = false;
	_double									m_dTimeDelta = 0.0;

	_uint									m_iIndex = 0;
	_uint									m_iParentIndex = 0;
	CCell*									m_pNeighbor[(_uint)LINE::COUNT];
	NAVINFO::OPTION							m_eOption = NAVINFO::OPTION::COUNT;
	_pGraphicDevice							m_pGraphicDevice = nullptr;
	_pContextDevice							m_pContextDevice = nullptr;

	class CVIBuffer_TriangleLineStrip*		m_pVIBuffer = nullptr;

private:
	HRESULT									Ready_Buffer();

public:
	HRESULT									Render(_fmatrix matWorld);


public:
	static	CCell*							Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, _float3* pPoints, NAVINFO::OPTION eOption, _uint iIndex);
	virtual void							Free() override;
};
END
#endif // !__ENGINE_CELL_H__