#include "stdafx.h"
#include "..\Public\Camera_Cutscene.h"
#include "UI_Dialogue_SystemMessage.h"
#include "GameInstance.h"
#include "MainCamera.h"
#include "CameraMgr.h"
#include "Tank.h"
#include "Level_OperaBackStage.h"
#include "Zhuangzi.h"
#include "Engels.h"
#include "BeauvoirCurtain.h"
#include "MinigameManager.h"
#include "Iron_Humanoid_Small.h"
#include "Iron_Android9S.h"
#include "MiniGame_IronFist.h"
#include "Android_9S.h"

CCamera_Cutscene::CCamera_Cutscene(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CCamera(pGraphicDevice, pContextDevice)
{
}

CCamera_Cutscene::CCamera_Cutscene(const CCamera_Cutscene & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Cutscene::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCamera_Cutscene::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	pCameraMgr->Set_CutsceneCam_To_Mgr(this);

	m_eState = OBJSTATE::DISABLE;

	return S_OK;
}

_int CCamera_Cutscene::Tick(_double TimeDelta)
{
	if (m_eState == OBJSTATE::ENABLE)
	{

		m_fAccTime += (_float)TimeDelta* m_fSpeed;

		if (m_vecAt.size() == 1)
		{
			m_bFixAt = true;
		}

		if (m_vecAt.size() != 1 && !m_bOnceAt)
		{
			m_bFixAt = false;
			m_bOnceAt = true;
		}

		if (m_vecMovePoints.size() == 1)
		{
			m_bFixEye = true;
		}

		if (m_vecMovePoints.size() != 1 && !m_bOnceEye)
		{
			m_bFixEye = false;
			m_bOnceEye = true;
		}

		if (m_fAccTime >= 1.f)
		{
			m_fAccTime = 0.f;
			if (m_iNumEyeCheck + 1 >= m_vecMovePoints.size())
			{
				m_bFixEye = true;
			}
			else
			{
				++m_iNumEyeCheck;
			}

			if (m_iNumAtCheck + 1 >= m_vecAt.size())
			{
				m_bFixAt = true;
			}
			else
			{
				++m_iNumAtCheck;
			}

			if (m_iNumAtCheck + 1 >= m_vecAt.size() && m_iNumEyeCheck + 1 >= m_vecMovePoints.size()) // reset all . this is the whole circle end time.
			{
				// write the code Stop & Disable Cam  || Start another Cam?
				m_eState = OBJSTATE::DISABLE;

				Check_After_Event();

				Release_CamObject();
			}

		}
		Play_Camera(TimeDelta);

		CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		if (pGameInstance->Key_Down(DIK_BACKSPACE))
		{
			m_bCancle = true;

			Check_After_Event();

			Release_CamObject();
		}

		return CCamera::Tick(TimeDelta);
	}
	else
	{
		m_fAccTime = 0.0;
		m_iNumAtCheck = 0;
		m_iNumEyeCheck = 0;
	}
	return 0;
}

_int CCamera_Cutscene::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Cutscene::Render()
{
	return S_OK;
}

void CCamera_Cutscene::Read_File(const _tchar * pDatFilePath, _uint iEventNumber)
{
	HANDLE hFile = CreateFile(pDatFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	DWORD dwByte = 0;
	_uint iCount = 0;

	iCount = 0;
	ReadFile(hFile, &iCount, sizeof(_uint), &dwByte, nullptr);

	for (_uint i = 0; i < iCount; ++i)
	{
		_tchar szBuff[MAX_PATH];
		ZeroMemory(szBuff, sizeof(_tchar) * MAX_PATH);

		ReadFile(hFile, szBuff, sizeof(_tchar) * MAX_PATH, &dwByte, nullptr);

		_uint iSize = 0;
		ReadFile(hFile, &iSize, sizeof(_uint), &dwByte, nullptr);

		vector<_float4> vecMovePoints;
		for (_uint i = 0; i < iSize; ++i)
		{
			_float4 vPos;
			ReadFile(hFile, &vPos, sizeof(_float4), &dwByte, nullptr);
			vecMovePoints.push_back(vPos);
		}

		_uint iSize2 = 0;
		ReadFile(hFile, &iSize2, sizeof(_uint), &dwByte, nullptr);

		vector<_float4> vecAtPoints;
		for (_uint j = 0; j < iSize2; ++j)
		{
			_float4 vAtPos;
			ReadFile(hFile, &vAtPos, sizeof(_float4), &dwByte, nullptr);
			vecAtPoints.push_back(vAtPos);
		}

		_float fSpeed = 0.f;
		ReadFile(hFile, &fSpeed, sizeof(_float), &dwByte, nullptr);
		_uint iNumEye = 0;
		ReadFile(hFile, &iNumEye, sizeof(_uint), &dwByte, nullptr);
		_uint iNumAt = 0;
		ReadFile(hFile, &iNumAt, sizeof(_uint), &dwByte, nullptr);

		CCam_Object* pCamObject = new CCam_Object;
		pCamObject->Set_vecMovePoints(vecMovePoints);
		pCamObject->Set_vecAt(vecAtPoints);
		pCamObject->Set_Speed(fSpeed);
		pCamObject->Set_NumPoints(iNumEye);
		pCamObject->Set_NumAt(iNumAt);

		m_mapCamera.emplace(szBuff, pCamObject);

		m_pCamObject = pCamObject;

		Update_Positions();

		Check_Number(iEventNumber);
	}
	CloseHandle(hFile);
}

void CCamera_Cutscene::Play_Camera(_double TimeDelta)
{
	_vector vEyePosition, vAtPosition;

	if (!m_bFixEye)
	{
		if (m_iNumEyeCheck + 1 >= m_vecMovePoints.size())
			vEyePosition = XMLoadFloat4(&m_vecMovePoints[m_iNumEyeCheck]);
		else
		{
			_vector vMovePoint1, vMovePoint2;
			vMovePoint1 = XMLoadFloat4(&m_vecMovePoints[m_iNumEyeCheck]);
			vMovePoint2 = XMLoadFloat4(&m_vecMovePoints[m_iNumEyeCheck + 1]);
			vEyePosition = XMVectorCatmullRom(vMovePoint1, vMovePoint1, vMovePoint2, vMovePoint2, m_fAccTime);
		}
	}
	else
	{
		//if (m_vecMovePoints.size() == 1)
		vEyePosition = XMLoadFloat4(&m_vecMovePoints[m_iNumEyeCheck]);
		//else
		//vEyePosition = m_vecMovePoints[m_iNumEyeCheck + 1];
	}

	if (!m_bFixAt)
	{
		if (m_iNumAtCheck + 1 >= m_vecAt.size())
			vAtPosition = XMLoadFloat4(&m_vecAt[m_iNumAtCheck]);
		else
		{
			_vector vAtPoint1, vAtPoint2;
			vAtPoint1 = XMLoadFloat4(&m_vecAt[m_iNumAtCheck]);
			vAtPoint2 = XMLoadFloat4(&m_vecAt[m_iNumAtCheck + 1]);
			vAtPosition = XMVectorCatmullRom(vAtPoint1, vAtPoint1, vAtPoint2, vAtPoint2, m_fAccTime);
		}
	}
	else
	{
		//if (m_vecAt.size() == 1)
		vAtPosition = XMLoadFloat4(&m_vecAt[m_iNumAtCheck]);
		//else
		//vAtPosition = m_vecAt[m_iNumAtCheck + 1];
	}

	_vector vNewLook = vAtPosition - vEyePosition;

	vNewLook = XMVector3Normalize(vNewLook);

	_vector vUp = { 0.f , 1.f , 0.f , 0.f };

	_vector vRight = XMVector3Cross(vUp, vNewLook);

	vUp = XMVector3Cross(vNewLook, vRight);

	m_pTransform->Set_State(CTransform::STATE::POSITION, vEyePosition);
	m_pTransform->Set_State(CTransform::STATE::RIGHT, vRight);
	m_pTransform->Set_State(CTransform::STATE::UP, vUp);
	m_pTransform->Set_State(CTransform::STATE::LOOK, vNewLook);
}

void CCamera_Cutscene::Update_Positions()
{
	if (m_pCamObject == nullptr)
		return;

	m_vecMovePoints = *m_pCamObject->Get_MovingPoints();
	m_vecAt = *m_pCamObject->Get_At();
	m_fSpeed = m_pCamObject->Get_Speed();
}
 
void CCamera_Cutscene::Release_CamObject()
{
	m_pCamObject = nullptr;

	auto& iter = m_mapCamera.begin();
	for (; iter != m_mapCamera.end();)
	{
		Safe_Delete(iter->second);
		iter = m_mapCamera.erase(iter);
	}

	m_iNumAtCheck = 0;
	m_iNumEyeCheck = 0;
	m_fAccTime = 0.f;
	m_bFixAt = false;
	m_bFixEye = false;
	m_bOnceEye = false;
	m_bOnceAt = false;
	m_bCancle = false;

	if (m_eEventNum != EVENT_CUTSCENE::IRON_FIST_SMALL) 
		m_eEventNum = EVENT_CUTSCENE::NONE;
}

void CCamera_Cutscene::Check_Number(_uint iNumber)
{
	if (iNumber == 0)
	{
		m_eEventNum = EVENT_CUTSCENE::NONE;
	}
	else if (iNumber == 1)
	{
		m_eEventNum = EVENT_CUTSCENE::ZHUANGZI;
	}

	else if (iNumber == 2)
	{
		m_eEventNum = EVENT_CUTSCENE::BEAUVOIR;
	}

	else if (iNumber == 3)
	{
		m_eEventNum = EVENT_CUTSCENE::TANK;
	}

	else if (iNumber == 4)
	{
		m_eEventNum = EVENT_CUTSCENE::ENGELS;
	}

	else if (iNumber == 5)
	{
		m_eEventNum = EVENT_CUTSCENE::CROSS;
	}

	else if (iNumber == 6)
	{
		m_eEventNum = EVENT_CUTSCENE::IRON_FIST_9S;
	}

	else if (iNumber == 7)
	{
		m_eEventNum = EVENT_CUTSCENE::IRON_FIST_SMALL;
	}

	else if (iNumber == 8)
	{
		m_eEventNum = EVENT_CUTSCENE::ENTRANCE;
	}
}

void CCamera_Cutscene::Check_After_Event()
{
	if (m_bCancle)
	{
		CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

		pCamMgr->Set_MainCam_On();
	}
	
	else
	{
		if (m_eEventNum == EVENT_CUTSCENE::NONE)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_On();
		}

		else if (m_eEventNum == EVENT_CUTSCENE::ZHUANGZI)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

			//pCamMgr->Set_MainCam_CutsceneTo3rd_On(vCamPos);
			pCamMgr->Set_MainCam_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_TOP);

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CZhuangzi* pBoss = (CZhuangzi*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", 0);

			if (pBoss == nullptr)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			pBoss->Set_UI_Engage();

			RELEASE_INSTANCE(CGameInstance);
		}

		else if (m_eEventNum == EVENT_CUTSCENE::BEAUVOIR)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			_vector vCamPos = m_pTransform->Get_State(CTransform::STATE::POSITION);

			pCamMgr->Set_MainCam_CutsceneTo3rd_On(vCamPos);

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
			if (nullptr == pAndroid)
				return;

			pAndroid->Set_TeleportBool(false);
			pAndroid->Set_NoneTeleport(false);

			CLevel_OperaBackStage::m_bEngage_UI = true;
		}

		else if (m_eEventNum == EVENT_CUTSCENE::TANK)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CTank* pTank = (CTank*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Boss", 0);

			if (pTank == nullptr)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			pTank->Set_UI_Engage();

			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_On();
			Release_9S_Fix();

			RELEASE_INSTANCE(CGameInstance);
		}

		else if (m_eEventNum == EVENT_CUTSCENE::ENGELS)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_On();

			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			CEngels* pBoss = (CEngels*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ROBOTGENERAL, L"Boss", 0);

			if (pBoss == nullptr)
			{
				RELEASE_INSTANCE(CGameInstance);
				return;
			}

			pBoss->Set_UI_Engage();

			RELEASE_INSTANCE(CGameInstance);
		}

		else if (m_eEventNum == EVENT_CUTSCENE::CROSS)
		{
			// go to top view ??? or go to 3rd view
			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));
			if (nullptr == pCurtain)
				return;

			pCurtain->Set_Animation(CBeauvoirCurtain::PATTERN::CLOSING);

			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_TOPB);
		}

		else if (m_eEventNum == EVENT_CUTSCENE::IRON_FIST_9S)
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();
			
			pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/IFSmall.dat", 7);
			CMinigameManager* pManager = CMinigameManager::Get_Instance();

			CIron_Android9S* pAndroid = (CIron_Android9S*)pManager->Find_Player(MINIGAME_IRONFIST);
			if (nullptr == pAndroid)
				return;

			pAndroid->Set_Animation(ANIM_IRON9S_IDLE);
			
			CIron_Humanoid_Small* pMonster = (CIron_Humanoid_Small*)pManager->Find_Object(MINIGAME_IRONFIST, MINIGAME_MONSTER);
			if (nullptr == pMonster)
				return;

			pMonster->Set_PlayAnimation(true);
	
		}

		else if (m_eEventNum == EVENT_CUTSCENE::IRON_FIST_SMALL)
		{
			// start 3 2 1 fight! ui
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_IronFistCam_On();

			m_eEventNum = EVENT_CUTSCENE::IRON_FIST_SMALL_END;

			CMinigameManager* pManager = CMinigameManager::Get_Instance();

			CIron_Humanoid_Small* pMonster = (CIron_Humanoid_Small*)pManager->Find_Object(MINIGAME_IRONFIST, MINIGAME_MONSTER);
			if (nullptr == pMonster)
				return;

			pMonster->Set_Animation(ANIM_IRONHS_IDLE);

			CGameInstance* pGameInstance = CGameInstance::Get_Instance();
			pGameInstance->Get_Observer(TEXT("OBSERVER_DIALOGUE_HACKING"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HACKINGMSG, nullptr));

			CMiniGame_IronFist* pMiniGame = (CMiniGame_IronFist*)pManager->Get_CurrentMiniGame();
			if (nullptr == pMiniGame)
				return;

			pMiniGame->Set_StartMessageOn(true);
		}
		else if (m_eEventNum == EVENT_CUTSCENE::ENTRANCE)
		{
			CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(0, 0, CUI_Dialogue_SystemMessage::iMsgOnQuest);

			CGameInstance::Get_Instance()->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));

			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_On();
		}
	}
}

void CCamera_Cutscene::Release_9S_Fix()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CAndroid_9S* pAndroid = (CAndroid_9S*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Android_9S", 0);
	if (nullptr == pAndroid)
		return;

	pAndroid->Set_Fix(false);
}

CCamera_Cutscene * CCamera_Cutscene::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Cutscene* pGameInstance = new CCamera_Cutscene(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCamera_Cutscene");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CCamera_Cutscene * CCamera_Cutscene::Clone(void * pArg)
{
	CCamera_Cutscene* pGameInstance = new CCamera_Cutscene(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCamera_Cutscene");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCamera_Cutscene::Free()
{
	__super::Free();

	auto& iter = m_mapCamera.begin();
	for (; iter != m_mapCamera.end();)
	{
		Safe_Delete(iter->second);
		iter = m_mapCamera.erase(iter);
	}
}
