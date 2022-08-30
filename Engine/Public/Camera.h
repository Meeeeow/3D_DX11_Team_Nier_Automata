#pragma once
#ifndef __ENGINE_CAMERA_H__
#define __ENGINE_CAMERA_H__

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
protected:
	CCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() DEFAULT;

public:
	typedef struct tagCameraDesc {
		tagCameraDesc() {
			ZeroMemory(this, sizeof(tagCameraDesc));
			vAxis = _float3(0.f, 1.f, 0.f);
			fFOV = XMConvertToRadians(90.f);
		}
		_float3				vEye;
		_float3				vAt;
		_float3				vAxis;
		
		_float				fFOV;
		_float				fAspect;
		_float				fNear;
		_float				fFar;

		CTransform::DESC	tTransformDesc;
	}DESC;

public:
	virtual HRESULT				NativeConstruct_Prototype() override;
	virtual HRESULT				NativeConstruct(void* pArg) override;
	virtual _int				Tick(_double dTimeDelta);
	virtual _int				LateTick(_double dTimeDelta);

public:
	void						Set_CameraDesc(const DESC& tDesc) {
		m_tDesc = tDesc; Syncronize_CameraState(); }

	void						Set_CamTransformDesc(const CTransform::DESC& tDesc) {
		m_tDesc.tTransformDesc = tDesc;	}

public:
	const _matrix				Get_WorldMatrix();
	class CTransform*			Get_Transform();
	DESC						Get_CameraDesc() { return m_tDesc; }

public:
	void						Set_CamState_Disable() { m_eState = OBJSTATE::DISABLE; }
	void						Set_CamState_Enable() { m_eState = OBJSTATE::ENABLE; }

public:
	HRESULT						Syncronize_CameraState();

protected:
	class CTransform*			m_pTransform = nullptr;
	DESC						m_tDesc;

public:
	virtual	CGameObject*		Clone(void* pArg) PURE;
	virtual void				Free() override;

};
END
#endif // !__ENGINE_CAMERA_H__