#include "stdafx.h"
#include "../Public/Event_Collider.h"
#include "Player.h"
#include "MainCamera.h"
#include "Coaster.h"
#include "CameraMgr.h"
#include "Zhuangzi.h"
#include "ZhuangziBridge.h"
#include "Beauvoir.h"
#include "UI_HUD_DirectionIndicator.h"
#include "Level_OperaBackStage.h"
#include "BeauvoirCurtain.h"
#include "Tank.h"
#include "Clown_Fly.h"
#include "UI_HUD_Controller.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_Dialogue_HighlightMessage.h"
#include "SoundMgr.h"

CEvent_Collider::CEvent_Collider(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{
}

CEvent_Collider::CEvent_Collider(const CEvent_Collider & rhs)
	:CGameObjectModel(rhs)
{
}

HRESULT CEvent_Collider::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CEvent_Collider::NativeConstruct(void * pArg)
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	CCollider::DESC tColliderDesc;
	tColliderDesc.vPivot = { 0.f , 0.f , 0.f };
	tColliderDesc.vScale = { 1.f , 1.f , 1.f };

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &tColliderDesc)))
		return E_FAIL;

	m_bCollision = true;

	return S_OK;
}

_int CEvent_Collider::Tick(_double dTimeDelta)
{
	if (m_eState == OBJSTATE::DISABLE)
	{
		return (_int)m_eState;
	}
	m_pHitBox->Update(m_pTransform->Get_WorldMatrix());

	return (_int)m_eState;
}

_int CEvent_Collider::LateTick(_double dTimeDelta)
{
		if (m_pHitBox != nullptr)
			m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return _int();
}

HRESULT CEvent_Collider::Render()
{
	m_pHitBox->Render();
	return S_OK;
}

void CEvent_Collider::Check_Event()
{
	switch (m_eEvent)
	{
	case EVENT::WHEEL_START:
		Event_Wheel_Start();
		break;

	case EVENT::WHEEL_END:
		Event_Wheel_End();
		break;

	case EVENT::COASTER_STAIR:
		Event_Coaster_Stair();
		break;

	case EVENT::COASTER:
		Event_Coaster();
		break;

	case EVENT::ENTRANCE:
		Event_Entrance();
		break;

	case EVENT::ZHUANGZI:
		Event_Zhuangzi();
		break;

	case EVENT::ZHUANGZI_ENTRANCE:
		Event_Zhunagzi_Entrance();
		break;

	case EVENT::BEAUVOIR:
		Event_Beauvoir();
		break;

	case EVENT::TANK:
		Event_Tank();
		break;

	case EVENT::GATE:
		Event_Gate();
		break;

	case EVENT::ENGELS:
		Event_Engels();
		break;

	case EVENT::COASTER1:
		Event_Coaster1();
		break;

	case EVENT::COASTER2:
		Event_Coaster2();
		break;

	case EVENT::COASTER3:
		Event_Coaster3();
		break;

	default:
		break;
	}
}

void CEvent_Collider::Set_Event_Number()
{
	if (!lstrcmp(m_szName, L"WheelStart"))
	{
		m_eEvent = EVENT::WHEEL_START;
	}

	else if (!lstrcmp(m_szName, L"WheelEnd"))
	{
		m_eEvent = EVENT::WHEEL_END;
	}

	else if (!lstrcmp(m_szName, L"CoasterStair"))
	{
		m_eEvent = EVENT::COASTER_STAIR;
	}

	else if (!lstrcmp(m_szName, L"Coaster"))
	{
		m_eEvent = EVENT::COASTER;
	}

	else if (!lstrcmp(m_szName, L"Entrance"))
	{
		m_eEvent = EVENT::ENTRANCE;
	}

	else if (!lstrcmp(m_szName, L"Zhuangzi"))
	{
		m_eEvent = EVENT::ZHUANGZI;
	}

	else if (!lstrcmp(m_szName, L"ZhuangziEntrance"))
	{
		m_eEvent = EVENT::ZHUANGZI_ENTRANCE;
	}

	else if (!lstrcmp(m_szName, L"Beauvoir"))
	{
		m_eEvent = EVENT::BEAUVOIR;
	}

	else if (!lstrcmp(m_szName, L"Tank"))
	{
		m_eEvent = EVENT::TANK;
	}

	else if (!lstrcmp(m_szName, L"Gate"))
	{
		m_eEvent = EVENT::GATE;
	}

	else if (!lstrcmp(m_szName, L"Engels"))
	{
		m_eEvent = EVENT::ENGELS;
	}

	else if (!lstrcmp(m_szName, L"Coaster1"))
	{
		m_eEvent = EVENT::COASTER1;
	}

	else if (!lstrcmp(m_szName, L"Coaster2"))
	{
		m_eEvent = EVENT::COASTER2;
	}

	else if (!lstrcmp(m_szName, L"Coaster3"))
	{
		m_eEvent = EVENT::COASTER3;
	}
}

HRESULT CEvent_Collider::Event_Wheel_Start()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pMainCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pMainCamera == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}
	pMainCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_WHEEL);

	///// For Player //////

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

	if (pPlayer == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pPlayer->Set_Targeting_False();

	///////////////////////
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Wheel_End()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pMainCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pMainCamera == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pMainCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_WHEEL_TO_3RD);


	///// For Player //////

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

	if (pPlayer == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pPlayer->Set_Targeting_False();

	///////////////////////

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Coaster_Stair()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CMainCamera* pMainCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pMainCamera == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pMainCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_3RD_TO_STAIR);

	///// For Player //////

	CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

	if (pPlayer == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pPlayer->Set_Targeting_False();

	///////////////////////

	// Delete Current Event
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = 10002;
		desc.eObjState = OBJSTATE::DEAD;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

HRESULT CEvent_Collider::Event_Coaster()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//CCoaster* pCoaster = (CCoaster*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Coasters", 0);

	//if (pCoaster == nullptr)
	//	return E_FAIL;

	//pCoaster->Start_Coaster();

	//////////////////////////////////////

	CMainCamera* pMainCamera = (CMainCamera*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"Camera", 0);

	if (pMainCamera == nullptr)
	{
		RELEASE_INSTANCE(CGameInstance);
		return E_FAIL;
	}

	pMainCamera->Set_Option(CMainCamera::OPTION::CATMULLROM_STAIR_TO_COASTER);

	///////////////////////////////////

	

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEvent_Collider::Event_Entrance()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/AP_Entrance.dat", 8);

	///////////////////////////////////////////////////////////////////////

	// Get Next Event Position
	CGameObject* pObj = pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, TEXT("Event_Collider"), 5);	// Gate
	if (nullptr == pObj)
		return E_FAIL;

	CTransform*	pTransform = (CTransform*)pObj->Get_Component(COM_KEY_TRANSFORM);

	// Add Next Event
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = 10005;
		desc.eObjState = OBJSTATE::ENABLE;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));
	}

	//// Add SysMsg
	//{
	//	CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc(0, 0, CUI_Dialogue_SystemMessage::iMsgOnQuest);

	//	pGameInstance->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));
	//}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Zhuangzi()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			
	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/ZhuangZi2.dat",1);

	CZhuangzi* pBoss = (CZhuangzi*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"Boss", 0);
	
	pBoss->Set_Start_True();

	pBoss->Set_UI_Engage_Cinematic();

	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.5f);
	//CSoundMgr::Get_Instance()->PlayLoopSound(L"Zhuangzi_Battle_BGM.mp3", CSoundMgr::CHANNELID::BGM);
	//CSoundMgr::Get_Instance()->Set_Volume(CSoundMgr::CHANNELID::BGM, 0.5f);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Zhunagzi_Entrance()
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	CZhuangziBridge* pBridge = (CZhuangziBridge*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::ZHUANGZISTAGE, L"ZhuangziBridge", 0);

	// Go to CZhuangziBridge
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

		tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
		tDesc.eObjState = OBJSTATE::DISABLE;
		tDesc.iObjID = pBridge->Get_ObjID();

		XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(pBridge->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));
	}

	pBridge->Set_State(OBJSTATE::ENABLE);

	CSoundMgr::Get_Instance()->PlaySound(L"Bridge_Down.mp3", CSoundMgr::CHANNELID::BRIDGE);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Beauvoir()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/Beauvoir2.dat", 2);
	
	CBeauvoir* pBoss = (CBeauvoir*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Boss", 0);

	pBoss->Set_Start_True();

	CBeauvoirCurtain* pCurtain = dynamic_cast<CBeauvoirCurtain*>(pGameInstance->Get_GameObjectPtr((_uint)LEVEL::OPERABACKSTAGE, L"Obstacle"));
	if (pCurtain != nullptr)
	{
		pCurtain->Set_Animation(CBeauvoirCurtain::PATTERN::OPENING);
	}

	CLevel_OperaBackStage::m_bStart = true;

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CEvent_Collider::Event_Tank()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	// Delete Current Event
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = 10006;
		desc.eObjState = OBJSTATE::DEAD;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));
	}

	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_CutSceneCam_On(L"../Bin/Data/CamData/Tank.dat", 3);

	CTank* pTank = (CTank*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, L"Boss", 0);

	pTank->Set_UI_Engage_Cinematic();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Gate()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	// Get Next Event Position
	CGameObject* pObj = pGameInstance->Get_GameObjectPtr((_uint)LEVEL::AMUSEMENTPARK, TEXT("Event_Collider"), 6);
	if (nullptr == pObj)
		return E_FAIL;

	CTransform*	pTransform = (CTransform*)pObj->Get_Component(COM_KEY_TRANSFORM);

	// Delete Current Event
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = 10005;
		desc.eObjState = OBJSTATE::DEAD;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));
	}
	// Add Next Event
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = 10006;
		desc.eObjState = OBJSTATE::ENABLE;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));
	}
	// Highlight Message
	{
		CUI_Dialogue_HighlightMessage::EVENTID	eID = CUI_Dialogue_HighlightMessage::EVENTID::BANNED;
		pGameInstance->Get_Observer(TEXT("OBSERVER_HIGHLIGHT_MSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_HIGHLIGHTMSG, &eID));
	}
	return S_OK;
}

HRESULT CEvent_Collider::Event_Engels()
{
	CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

	pCamMgr->Set_MainCam_Shake_Engels_True();
	//CSoundMgr::Get_Instance()->PlayLoopSound(L"Engels_Battle_BGM.mp3", CSoundMgr::CHANNELID::BGM);

	return S_OK;
}

HRESULT CEvent_Collider::Event_Coaster1()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	list<CGameObject*>* pMonsterList = pGameInstance->Get_ObjectList((_uint)LEVEL::AMUSEMENTPARK, L"Monster");

	if (nullptr == pMonsterList)
		return S_OK;

	for (auto& pMonster : *pMonsterList)
	{
		_uint iObjID = pMonster->Get_ObjID();

		if (iObjID >= OriginID_ClownFly)
		{
			if (dynamic_cast<CClown_Fly*>(pMonster)->Get_Wave() == CClown_Fly::WAVE::WAVE_0)
			{
				dynamic_cast<CClown_Fly*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
			}

			else
			{
				continue;
			}
		}

	 }


	return S_OK;
}

HRESULT CEvent_Collider::Event_Coaster2()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	list<CGameObject*>* pMonsterList = pGameInstance->Get_ObjectList((_uint)LEVEL::AMUSEMENTPARK, L"Monster");

	if (nullptr == pMonsterList)
		return S_OK;

	for (auto& pMonster : *pMonsterList)
	{
		_uint iObjID = pMonster->Get_ObjID();

		if (iObjID >= OriginID_ClownFly)
		{
			if (dynamic_cast<CClown_Fly*>(pMonster)->Get_Wave() == CClown_Fly::WAVE::WAVE_1)
			{
				dynamic_cast<CClown_Fly*>(pMonster)->Set_Start_True();
				pMonster->Set_State(OBJSTATE::ENABLE);
			}

			else
			{
				continue;
			}
		}

	}


	return S_OK;
}

HRESULT CEvent_Collider::Event_Coaster3()
{
	return S_OK;
}

_bool CEvent_Collider::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB()) && m_eState != OBJSTATE::DISABLE)
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}

	return false;
}

void CEvent_Collider::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		Check_Event();
		m_eState = OBJSTATE::DISABLE;
	}
}

CEvent_Collider * CEvent_Collider::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, void * pArg)
{
	CEvent_Collider* pGameInstance = new CEvent_Collider(pGraphicDevice, pContextDevice);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CEvent_Collider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CGameObject * CEvent_Collider::Clone(void * pArg)
{
	CEvent_Collider* pGameInstance = new CEvent_Collider(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Clone	CEventCollider");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CEvent_Collider::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitBox);
}