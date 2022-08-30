#pragma once
#ifndef __CLIENT_CAMERA_CUTSCENE_H__
#define	__CLIENT_CAMERA_CUTSCENE_H__

#include "Client_Defines.h"
#include "Camera.h"
#include "Player.h"
#include "Cam_Object.h"
#include "Level_OperaBackStage.h"


BEGIN(Client)
class CCamera_Cutscene final : public CCamera
{
public:
	enum class EVENT_CUTSCENE : _uint
	{
		ZHUANGZI = 1,
		BEAUVOIR = 2,
		TANK = 3,
		ENGELS = 4,
		CROSS = 5,
		IRON_FIST_9S = 6,
		IRON_FIST_SMALL = 7,
		ENTRANCE = 8,
		IRON_FIST_SMALL_END = 998,
		NONE = 999
	};
private:
	CCamera_Cutscene(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice);
	explicit CCamera_Cutscene(const CCamera_Cutscene& rhs);
	virtual ~CCamera_Cutscene() DEFAULT;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_double TimeDelta);
	virtual _int LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void	Read_File(const _tchar * pDatFilePath , _uint iEventNumber = 0);
	void	Play_Camera(_double TimeDelta);
	void	Update_Positions();
	void	Release_CamObject();
	void	Check_Number(_uint iNumber);
	void	Check_After_Event();

public:
	void	Release_9S_Fix();

public:
	static CCamera_Cutscene* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CCamera_Cutscene* Clone(void* pArg);
	virtual void Free();

private:
	map<const _tchar*, class CCam_Object*>	m_mapCamera;
	vector<_float4>				m_vecMovePoints;
	vector<_float4>				m_vecAt;
	_float						m_fSpeed = 0.f;
	CCam_Object*				m_pCamObject = nullptr;

	_bool						m_bFixEye = false;
	_bool						m_bFixAt = false;
	_bool						m_bOnceEye = false;
	_bool						m_bOnceAt = false;

	_float						m_fAccTime = 0.f;
	_uint						m_iNumEyeCheck = 0;
	_uint						m_iNumAtCheck = 0;

	_bool						m_bCancle = false;

	EVENT_CUTSCENE				m_eEventNum = EVENT_CUTSCENE::NONE;

};
END

#endif


