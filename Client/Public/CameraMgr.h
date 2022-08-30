#pragma once
#ifndef __CLIENT_CAMERA_MGR_H__
#define	__CLIENT_CAMERA_MGR_H__

#include "Client_Defines.h"
#include "MainCamera.h"
#include "Camera_Debug.h"
#include "Camera_Cutscene.h"
#include "Camera_IronFist.h"

BEGIN(Client)
class CCameraMgr final : public CBase
{
	DECLARE_SINGLETON(CCameraMgr)

private:
	explicit CCameraMgr();
	virtual ~CCameraMgr() DEFAULT;

public:
	void				Set_MainCam_To_Mgr(CMainCamera* pCamera);
	void				Set_DebugCam_To_Mgr(CCamera_Debug* pDebugCamera);
	void				Set_CutsceneCam_To_Mgr(CCamera_Cutscene* pCutsceneCmera);
	void				Set_IronFistCam_To_Mgr(CCamera_IronFist* pIronFistCamera);

public:
	void				Set_MainCam_On();
	void				Set_DebugCam_On();
	void				Set_CutSceneCam_On(const _tchar * pDatFilePath , _uint iNumber = 0);
	void				Set_IronFistCam_On();

public:
	void				Set_MainCam_CutsceneTo3rd_On(_vector vPos);
	void				Set_MainCam_Shake_Engels_True();
	void				Set_MainCam_Option(CMainCamera::OPTION eOption);
	void				Set_MainCam_ShakeHit_True();
	void				Set_MainCam_ShakeHitBig_True();
	void				Set_MainCam_ShakeGauntlet_True();
	CMainCamera::OPTION				Get_MainCam_Option();
	void				Set_MiniCam_On();
	void				Set_MainCam_Fov(_float fFov);

public:
	const OBJSTATE&		Get_CutScene_State() const;
	const OBJSTATE&		Get_MainCam_State() const;
	const OBJSTATE&		Get_DebugCam_State() const;

public:
	CCamera_IronFist*	Get_IronFistCamera() { return m_pIronFistCamera; }

public:
	virtual	void				Free() override;

private:
	CMainCamera*		m_pMainCamera = nullptr;
	CCamera_Debug*		m_pDebugCamera = nullptr;
	CCamera_IronFist*	m_pIronFistCamera = nullptr;
	CCamera_Cutscene*	m_pCutsceneCamera = nullptr;
};
END

#endif