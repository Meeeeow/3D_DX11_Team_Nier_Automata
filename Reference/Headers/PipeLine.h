#pragma once
#ifndef __ENGINE_PIPE_LINE_H__
#define __ENGINE_PIPE_LINE_H__

#include "Base.h"

BEGIN(Engine)
class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
private:
	explicit CPipeLine();
	virtual ~CPipeLine() DEFAULT;

public:
	enum class TRANSFORM : _uint {
		D3DTS_VIEW = 0,
		D3DTS_PROJ = 1
	};


public:
	HRESULT					Update_Variable();

	void					Set_Transform(TRANSFORM eState, _fmatrix matTransform) {
		eState == TRANSFORM::D3DTS_VIEW ? Set_OldTransform(eState, m_matView) : Set_OldTransform(eState, m_matProj);
		eState == TRANSFORM::D3DTS_VIEW ? XMStoreFloat4x4(&m_matView, matTransform) : XMStoreFloat4x4(&m_matProj, matTransform);
	}
	void					Set_OldTransform(TRANSFORM eState, _float4x4 matPreTransform) {
		eState == TRANSFORM::D3DTS_VIEW ? m_matOldView = matPreTransform : m_matOldProj = matPreTransform;
	}


	void					Set_Far(_float fFar) { m_fFar = fFar; }
	void					Set_Near(_float fNear) { m_fNear = fNear; }
	void					Set_FOV(_float fFOV) { m_fFOV = fFOV; }

	void					Set_WindowSize(const _uint& iWinCX, const _uint& iWinCY) {
		m_iWinCX = iWinCX; m_iWinCY = iWinCY; }

	void					Set_TimeDelta(const _double& dTimeDelta) { m_dTimeDelta = dTimeDelta; }
	void					Add_PlayTime(const _double& dTimeDelta) { m_dPlayTime += dTimeDelta; }

public:
	_fvector				Get_CameraPosition() { return XMLoadFloat4(&m_vCameraPosition); }

	_matrix					Get_Transform(TRANSFORM eState) {
		return eState == TRANSFORM::D3DTS_VIEW ? XMLoadFloat4x4(&m_matView) : XMLoadFloat4x4(&m_matProj); }

	_matrix					Get_OldTransform(TRANSFORM eState) {
		return eState == TRANSFORM::D3DTS_VIEW ? XMLoadFloat4x4(&m_matOldView) : XMLoadFloat4x4(&m_matOldProj);	}

	_matrix					Get_Transpose(TRANSFORM eState) {
		return XMMatrixTranspose(Get_Transform(eState)); }

	_matrix					Get_OldTranspose(TRANSFORM eState) {
		return XMMatrixTranspose(Get_OldTransform(eState));	}

	_matrix					Get_Inverse(TRANSFORM eState) {
		return eState == TRANSFORM::D3DTS_VIEW ? XMLoadFloat4x4(&m_matInverseView) : XMLoadFloat4x4(&m_matInverseProj);	}

	_float					Get_Far() { return m_fFar; }
	_float					Get_Near() { return m_fNear; }
	_float					Get_FOV() { return m_fFOV; }

	const _double&			Get_TimeDelta() const { return m_dTimeDelta; }
	_float					Get_fTimeDelta() const { return (float)m_dTimeDelta; }

private:
	_float4x4				m_matView, m_matInverseView;
	_float4x4				m_matProj, m_matInverseProj;

	_float4x4				m_matOldView, m_matOldInverseView;
	_float4x4				m_matOldProj, m_matOldInverseProj;

	_float4					m_vCameraPosition;
	_float4					m_vOldCameraPosition;

	_float					m_fFar = 0.f;
	_float					m_fNear = 0.f;
	_float					m_fFOV = 0.f;

	_uint					m_iWinCX = 0;
	_uint					m_iWinCY = 0;

	_double					m_dTimeDelta = 0.0;
	_double					m_dPlayTime = 0.0;

public:
	virtual	void			Free() override;

};

END
#endif // !__ENGINE_PIPE_LINE_H__