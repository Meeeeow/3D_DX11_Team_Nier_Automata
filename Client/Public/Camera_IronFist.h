#pragma once

#ifndef __CLIENT_CAMERA_IRONFIST_H__
#define __CLIENT_CAMERA_IRONFIST_H__

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_IronFist final : public CCamera
{
public:
	CCamera_IronFist(_pGraphicDevice pDevice, _pContextDevice pDeviceContext);
	CCamera_IronFist(const CCamera_IronFist& rhs);
	virtual ~CCamera_IronFist()DEFAULT;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void				Update_Position(_double TimeDelta);
	void				Set_LookAt();

private:
	_double				m_LerpPositionTime = 0.0;
	_float4				m_vPrePos = _float4(0.f, 0.f, 0.f, 1.f);
	_float4				m_vGoalPos = _float4(0.f, 0.f, 0.f, 1.f);

public:
	static CCamera_IronFist* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CCamera_IronFist* Clone(void* pArg);
	virtual void Free();
};

END

#endif