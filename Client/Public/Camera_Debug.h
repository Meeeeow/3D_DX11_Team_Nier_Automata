#pragma once
#ifndef __CLIENT_CAMERA_DEBUG_H__
#define __CLIENT_CAMERA_DEBUG_H__


#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Debug final : public CCamera
{
private:
	CCamera_Debug(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Debug(const CCamera_Debug& rhs);
	virtual ~CCamera_Debug() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void		Set_CamViewTrue() { m_bCamViewing = true; }

public:
	static CCamera_Debug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CCamera_Debug* Clone(void* pArg);
	virtual void Free();

public:
	_bool		m_bCamViewing = false;
	_bool		m_bOnce = false;

private:
	_float		m_fPower = 0.01f;
};

END

#endif // !
