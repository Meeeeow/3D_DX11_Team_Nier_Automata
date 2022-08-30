#pragma once
#include "Camera.h"
#include "CamObject.h"

BEGIN(Engine)
class CGameInstance;
END

class CToolCamera final : public CCamera
{
private:
	CToolCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CToolCamera(const CToolCamera& rhs);
	virtual ~CToolCamera() DEFAULT;

public:
	virtual HRESULT				NativeConstruct_Prototype();
	virtual HRESULT				NativeConstruct_Prototype(CCamera::DESC tCameraDesc);
	virtual HRESULT				NativeConstruct(void* pArg);
	virtual _int				Tick(_double dTimeDelta);
	virtual _int				LateTick(_double dTimeDelta);


	_vector						Get_CamPos();

	void						Do_Action() { m_bAction = !m_bAction; }
	void						Move_Free(_double dTimeDelta);

	//for Camera Tab
public:
	void						Set_CamObject(class CCamObject* _pObj) { m_pCamObject = _pObj; }
	void						Set_CamTestTrue() { m_bCamTestOn = true; }
	void						Set_CamTestFalse() { m_bCamTestOn = false; }
	void						Set_CamPlayTrue() { m_bPlayCam = true; }
	void						Set_CamPlayFalse() { m_bPlayCam = false; }
	void						Set_CamResetTrue() { m_bReset = true; }
	void						Play_Cam();
	vector<_float4>*			Get_vecMovePoints() { return &m_vecMovePoints; }
	vector<_float4>*			Get_vecAt() { return &m_vecAt; }
	_float						Get_AccTime() { return m_fAccTime; }
	_float						Get_Speed() { return m_fSpeed; }

	void						Set_vecMovePoints(int iIndex, _float4 vPosition) { m_vecMovePoints[iIndex] = vPosition; }
	void						Set_vecAt(int iIndex, _float4 vPosition) { m_vecAt[iIndex] = vPosition; }
	void						Set_AccTime(_float fTime) { m_fAccTime = fTime; }
	void						Set_Speed(_float fPositionSpeed) { m_fSpeed = fPositionSpeed; }
	void						Play_CamTest();

public:
	void						Update_Positions();

private:
	_bool						m_bAction = false;

	//for Camera Tab
private:
	_bool						m_bCamTestOn = false;
	_bool						m_bPlayCam = false;
	_bool						m_bReset = false;
	_float						m_fAccTime = 0.f;
	_uint						m_iNumEyeCheck = 0;
	_uint						m_iNumAtCheck = 0;

	vector<_float4>				m_vecMovePoints;
	vector<_float4>				m_vecAt;
	_float						m_fSpeed = 0.f;
	CCamObject*					m_pCamObject = nullptr;	

	_bool						m_bFixEye = false;
	_bool						m_bFixAt = false;
	_bool						m_bOnceEye = false;
	_bool						m_bOnceAt = false;

public:
	static	CToolCamera*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	static	CToolCamera*		Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, CCamera::DESC tCameraDesc);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;

};

