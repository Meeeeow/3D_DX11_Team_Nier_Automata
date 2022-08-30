#pragma once
#ifndef __ENGINE_PICKING_H__
#define __ENGINE_PICKING_H__

#include "Base.h"

class CPicking final : public CBase
{
	DECLARE_SINGLETON(CPicking)

private:
	explicit CPicking();
	virtual ~CPicking() DEFAULT;

public:
	HRESULT					NativeConstruct(HWND hWnd);

	HRESULT					PreConvert();
	// 피킹 할 대상이 이동 값이 있으면 이동 값 곱해서 넣어주십쇼
	_bool					Picking(_fvector v0, _fvector v1, _fvector v2, _float& fDistance);

public:
	_fvector				Get_Ray() { return XMLoadFloat4(&m_vMouseRay); }
	_fvector				Get_Pivot() { return XMLoadFloat4(&m_vMousePivot); }

private:
	_pContextDevice			m_pContextDevice = nullptr;
	HWND					m_hWnd = NULL;

	_float4					m_vMouseRay;
	_float4					m_vMousePivot;

public:
	virtual void			Free() override;

};


#endif // !__ENGINE_PICKING_H__