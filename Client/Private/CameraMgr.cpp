#include "stdafx.h"
#include "..\Public\CameraMgr.h"

IMPLEMENT_SINGLETON(CCameraMgr)

CCameraMgr::CCameraMgr()
{
}


void CCameraMgr::Set_MainCam_To_Mgr(CMainCamera * pCamera)
{
	m_pMainCamera = pCamera;
	Safe_AddRef(m_pMainCamera);
}

void CCameraMgr::Set_DebugCam_To_Mgr(CCamera_Debug * pDebugCamera)
{
	m_pDebugCamera = pDebugCamera;
	Safe_AddRef(m_pDebugCamera);
}

void CCameraMgr::Set_CutsceneCam_To_Mgr(CCamera_Cutscene * pCutsceneCmera)
{
	m_pCutsceneCamera = pCutsceneCmera;
	Safe_AddRef(m_pCutsceneCamera);
}

void CCameraMgr::Set_IronFistCam_To_Mgr(CCamera_IronFist * pIronFistCamera)
{
	m_pIronFistCamera = pIronFistCamera;
	Safe_AddRef(m_pIronFistCamera);
}

void CCameraMgr::Set_MainCam_On()
{
	m_pMainCamera->Set_State(OBJSTATE::ENABLE);
	m_pMainCamera->Set_Option(CMainCamera::OPTION::FIXED);

	m_pDebugCamera->Set_State(OBJSTATE::DISABLE);

	m_pCutsceneCamera->Set_State(OBJSTATE::DISABLE);
	m_pIronFistCamera->Set_State(OBJSTATE::DISABLE);

}

void CCameraMgr::Set_DebugCam_On()
{
	m_pMainCamera->Set_State(OBJSTATE::DISABLE);
	m_pDebugCamera->Set_State(OBJSTATE::ENABLE);
	m_pCutsceneCamera->Set_State(OBJSTATE::DISABLE);
	m_pIronFistCamera->Set_State(OBJSTATE::DISABLE);
}

void CCameraMgr::Set_CutSceneCam_On(const _tchar * pDatFilePath , _uint iNumber)
{
	m_pIronFistCamera->Set_State(OBJSTATE::DISABLE);
	m_pMainCamera->Set_State(OBJSTATE::DISABLE);
	m_pDebugCamera->Set_State(OBJSTATE::DISABLE);
	m_pCutsceneCamera->Read_File(pDatFilePath , iNumber);
	m_pCutsceneCamera->Set_State(OBJSTATE::ENABLE);
}

void CCameraMgr::Set_IronFistCam_On()
{
	m_pIronFistCamera->Set_State(OBJSTATE::ENABLE);
	m_pIronFistCamera->Set_LookAt();
	m_pMainCamera->Set_State(OBJSTATE::DISABLE);
	m_pDebugCamera->Set_State(OBJSTATE::DISABLE);
	m_pCutsceneCamera->Set_State(OBJSTATE::DISABLE);
}

void CCameraMgr::Set_MainCam_CutsceneTo3rd_On(_vector vPos)
{
	m_pMainCamera->Set_State(OBJSTATE::ENABLE);
	m_pMainCamera->Set_CutscenePos(vPos);
	m_pMainCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_CUTSCENE_TO_3RD);

	m_pDebugCamera->Set_State(OBJSTATE::DISABLE);
	m_pCutsceneCamera->Set_State(OBJSTATE::DISABLE);
}

void CCameraMgr::Set_MainCam_Shake_Engels_True()
{
	if (Get_MainCam_State() == OBJSTATE::ENABLE)
	{
		m_pMainCamera->Set_ShakeEngelsTrue();
	}
}

void CCameraMgr::Set_MainCam_Option(CMainCamera::OPTION eOption)
{
	m_pMainCamera->Set_State(OBJSTATE::ENABLE);
	m_pMainCamera->Set_Option(eOption);

	m_pDebugCamera->Set_State(OBJSTATE::DISABLE);
	m_pCutsceneCamera->Set_State(OBJSTATE::DISABLE);
}

void CCameraMgr::Set_MainCam_ShakeHit_True()
{
	if (Get_MainCam_State() == OBJSTATE::ENABLE)
	{
		m_pMainCamera->Set_ShakeHitTrue();
	}
}

void CCameraMgr::Set_MainCam_ShakeHitBig_True()
{
	if (Get_MainCam_State() == OBJSTATE::ENABLE)
	{
		m_pMainCamera->Set_ShakeHitBigTrue();
	}
}

void CCameraMgr::Set_MainCam_ShakeGauntlet_True()
{
	if (Get_MainCam_State() == OBJSTATE::ENABLE)
	{
		m_pMainCamera->Set_ShakeGauntletTrue();
	}
}

CMainCamera::OPTION CCameraMgr::Get_MainCam_Option()
{
	CMainCamera::OPTION eOption = CMainCamera::OPTION::COUNT;

	if (OBJSTATE::ENABLE == Get_MainCam_State())
	{
		eOption = m_pMainCamera->Get_Option();
	}
	return eOption;
}

void CCameraMgr::Set_MiniCam_On()
{
	m_pIronFistCamera->Set_LookAt();
}

void CCameraMgr::Set_MainCam_Fov(_float fFov)
{
}

const OBJSTATE & CCameraMgr::Get_CutScene_State() const
{
	return m_pCutsceneCamera->Get_State();
}

const OBJSTATE & CCameraMgr::Get_MainCam_State() const
{
	return m_pMainCamera->Get_State();
}

const OBJSTATE & CCameraMgr::Get_DebugCam_State() const
{
	return m_pDebugCamera->Get_State();
}

void CCameraMgr::Free()
{
	Safe_Release(m_pDebugCamera);
	Safe_Release(m_pCutsceneCamera);
	Safe_Release(m_pIronFistCamera);
	Safe_Release(m_pMainCamera);
}
