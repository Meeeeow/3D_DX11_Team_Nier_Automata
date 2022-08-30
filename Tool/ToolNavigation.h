#pragma once

#include "ToolObjectBase.h"

class CToolNavigation final : public CToolObjectBase
{
private:
	CToolNavigation(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	virtual ~CToolNavigation() DEFAULT;

public:
	virtual	HRESULT					NativeConstruct_Prototype(NAVINFO tNavigation);
	virtual _int					Tick(_double dTimeDelta);
	virtual _int					LateTick(_double dTimeDelta);
	virtual HRESULT					Render();

public:
	virtual void					Picking();
	_bool							Picking(_fvector vOrigin, _fvector vRay, _float3& vPos, _float& fDistance);


	void							Set_TargetOn() { m_bTargetOn = true; }
	void							Set_TargetOff() { m_bTargetOn = false; }
	void							Set_TargetType(NAVINFO::OPTION eOption) { m_tNavInfo.eOption = eOption; }

public:
	const NAVINFO&					Get_NavInfo() const { return m_tNavInfo; }


private:
	NAVINFO							m_tNavInfo;
	CVIBuffer_Triangle*				m_pVIBuffer = nullptr;
	CVIBuffer_TriangleLineStrip*	m_pVIBufferLine = nullptr;
	CTexture*						m_pTexture = nullptr;
	_bool							m_bTargetOn = false;
	CCollider*						m_pPointCollider[3];
	_float4							m_vCenterPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float							m_fCenterRadius = 0.f;


public:
	static	CToolNavigation*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, NAVINFO tNavigation);
	virtual void					Free();

};

