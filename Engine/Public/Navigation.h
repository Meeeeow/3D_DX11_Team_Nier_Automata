#pragma once
#ifndef __ENGINE_NAVIGATION_H__
#define __ENGINE_NAVIGATION_H__


#include "Component.h"
#include "Cell.h"

BEGIN(Engine)
class ENGINE_DLL CNavigation final : public CComponent
{
private:
	CNavigation(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() DEFAULT;

public:
	typedef	vector<class CCell*>		CELLS;

public:
	virtual HRESULT				NativeConstruct_Prototype(const _tchar* pNavDataFilePath);
	virtual HRESULT				NativeConstruct(void* pArg);

public:
	HRESULT						Update(_fmatrix matWorld);
	HRESULT						SetUp_Neighbor();
	_bool						IsOn(_vector& vMovePos, CCell** ppSlideOut);
	_bool						IsLowThanCell(_vector & vPos, _vector* pOut);

	_bool						IsJump(_vector& vMovePos, CCell** ppSlideOut);
	
	HRESULT						Render();
	HRESULT						Culling(_double dTimeDelta);

	_uint						Find_CurrentCell(const _vector& vPosition);
	HRESULT						Init_Index(const _vector& vPosition);
	HRESULT						Init_Index(const _uint& iIndex);

public:
	void						Find_NearCell(_vector& vPosition);
	_vector						Find_CenterPos(CCell* pCell);
	_vector						Find_CenterPos(_uint iIndex);

public:
	const _uint&				Get_Index() { return m_iCurrentIndex; }
	const _float4&				Get_CenterPosition(const _uint& iIndex);
	const _uint&				Get_Size() { return m_iSize; }

public:
	_vector						Get_CellPointInWorld(_uint iIndex, CCell::POINT ePoint);

public:
	void						Set_CurrentIndex(_uint iIndex) { m_iCurrentIndex = iIndex; }

private:
	_double						m_dTimeDelta = 0.0;
	_uint						m_iCurrentIndex = 0;
	_uint						m_iSize = 0;
	_float4x4					m_matWorld;
	CELLS						m_vecCells;

private:
	_uint						m_iPreCellIndex = 0;

public:
	static	CNavigation*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice
		, const _tchar* pNavDataFilePath);
	virtual	CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;

};
END
#endif // !__ENGINE_NAVIGATION_H__