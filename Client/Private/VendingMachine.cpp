#include "stdafx.h"
#include "..\Public\VendingMachine.h"
#include "Level_Loading.h"
#include "UI_HUD_Pointer.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "UI_HUD_Controller.h"
#include "SoundMgr.h"

CVendingMachine::CVendingMachine(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CVendingMachine::CVendingMachine(const CVendingMachine & rhs)
	: CGameObjectModel(rhs)
{

}

HRESULT CVendingMachine::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVendingMachine::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	MO_INFO* pInfo = (MO_INFO*)pArg;
	m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	LEVEL eLevel = (LEVEL)pGameInstance->Get_NextLevelID();
	m_iPassIndex = (_uint)MESH_PASS_INDEX::VENDINGMACHINE;
	m_eAnimState = ANIMSTATE::INACTIVE;

	switch (eLevel)
	{
	case Client::LEVEL::STAGE1:
		m_eNextLevel = LEVEL::ZHUANGZISTAGE;
		break;
	case Client::LEVEL::ZHUANGZISTAGE:
		m_eNextLevel = LEVEL::ROBOTGENERAL;
		break;
	case Client::LEVEL::ROBOTGENERAL:
		m_eNextLevel = LEVEL::AMUSEMENTPARK;
		break;
	case Client::LEVEL::AMUSEMENTPARK:
		m_eNextLevel = LEVEL::OPERABACKSTAGE;
		m_eAnimState = ANIMSTATE::CLOSE;
		break;
	case Client::LEVEL::OPERABACKSTAGE:
		break;
	default:
		break;
	}
	m_eGroup = CRenderer::RENDERGROUP::ALPHATEST;
	m_bActive = false;

	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCVendingMachine_InstanceID = OriginID_VendingMachine;

	m_iObjID = iCVendingMachine_InstanceID++;

	return S_OK;
}

_int CVendingMachine::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	//if (false == m_bIsObserverSet)
	//{
	//	wstring wstr = L"OBSERVER_VENDING_MACHINE_" + to_wstring(m_iObjID);
	//	pGameInstance->Create_Observer(wstr.c_str(), this);
	//	pGameInstance->Get_Observer(wstr.c_str())->Subscribe(TEXT("SUBJECT_PLAYER"));
	//	m_bIsObserverSet = true;
	//}

	Check_Player(pGameInstance, dTimeDelta);
	
	m_pModel->Set_Animation((_uint)m_eAnimState);

	Check_Key(pGameInstance);

	Update_UI(dTimeDelta);

	return _int();
}

_int CVendingMachine::LateTick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_bPause)
		dTimeDelta = 0.0;

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	if (pGameInstance->Culling(m_pTransform, m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(m_eGroup, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	return _int();
}

HRESULT CVendingMachine::Render()
{
	m_pModel->SetUp_RawValue("g_isActive", &m_bActive, sizeof(bool));

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVendingMachine::SetUp_Components()
{
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_MODEL_VENDING_MACHINE, COM_KEY_MODEL, (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

void CVendingMachine::Check_Player(CGameInstance* pGameInstance, _double dTimeDelta)
{
	CCollider* pCollider = dynamic_cast<CCollider*>(pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_MIDDLEPOINT));
	if (nullptr != pCollider )
	{
		_vector vOrigin = m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vRay = XMVector4Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * -1.f;
		_matrix matRotation = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotation));

		vRay = XMVector4Transform(vRay, matRotation);

		_float3	vPosition = _float3(0.f, 0.f, 0.f);
		_float	fDistance = 0.f;

		m_bCollided = pCollider->Collision_Sphere(vOrigin, vRay, vPosition, fDistance);

		// Ray Collided
		if(m_bCollided && fDistance <= 3.f && m_bActive)
		{
			m_dDuration = 0.0;
			if (m_eAnimState == ANIMSTATE::INACTIVE || m_eAnimState == ANIMSTATE::CLOSE)
			{
				if (false == m_bIsOpenSound)
				{
					CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
					CSoundMgr::Get_Instance()->PlaySound(L"TB_Open.mp3", CSoundMgr::CHANNELID::UI);
					m_bIsOpenSound = true;
				}

				m_eAnimState = ANIMSTATE::OPEN;
				m_pModel->Set_NoneBlend();
			}
			return;
		}
		// Ray didn't Collide
		else if (!m_bCollided)
		{
			m_dDuration += dTimeDelta;
			m_fRotation += 1.f;
			if (m_fRotation >= 45.f)
				m_fRotation = -m_fRotation;

			if (m_dDuration >= 1.5)
			{
				if (m_eAnimState == ANIMSTATE::OPEN)
				{
					m_eAnimState = ANIMSTATE::CLOSE;
					m_bIsOpenSound = false;
					m_pModel->Set_NoneBlend();
				}
				m_dDuration = 0.0;
			}
		}
	}
}

void CVendingMachine::Check_Key(CGameInstance * pGameInstance)
{
	if (true == m_bCollided && m_eAnimState == ANIMSTATE::OPEN)
	{
		if (true == pGameInstance->Key_Down(DIK_R))
		{
			//CGameInstance* pGameInstance = CGameInstance::Get_Instance();

			CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

			tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
			tDesc.eObjState = OBJSTATE::DISABLE;
			tDesc.iObjID = m_iObjID;

			XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(this->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

			pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

			CLevel_Loading* pInstance = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, m_eNextLevel, (_uint)LEVEL::LOADING);

			if (pInstance == nullptr)
			{
				return;
			}
			if (m_eNextLevel == LEVEL::AMUSEMENTPARK)
			{
				CUI_HUD_Controller::COMMANDDESC	tCommandDesc;

				tCommandDesc.iCommandFlag |= COMMAND_HUD_RELEASE;
				tCommandDesc.iCommandFlag |= COMMAND_HUD_DISABLE;				

				pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tCommandDesc));
			}
			pGameInstance->Get_CurrentLevel()->Set_NextLevel(pInstance);
		}
	}
}

void CVendingMachine::Set_AlphaTest()
{
	m_bActive = true;
}

void CVendingMachine::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		//if (m_bCollided)
		//{
		//	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

		//	CUI_HUD_DirectionIndicator::UIINDICATORDESC	tDesc;

		//	tDesc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;
		//	tDesc.eObjState = OBJSTATE::DISABLE;
		//	tDesc.iObjID = m_iObjID;

		//	XMStoreFloat4(&tDesc.vTargetWorldPos, dynamic_cast<CTransform*>(this->Get_Component(COM_KEY_TRANSFORM))->Get_State(CTransform::STATE::POSITION));

		//	pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &tDesc));

		//	CLevel_Loading* pInstance = CLevel_Loading::Create(m_pGraphicDevice, m_pContextDevice, m_eNextLevel, (_uint)LEVEL::LOADING);
		//
		//	if (pInstance == nullptr)
		//	{
		//		return;
		//	}
		//	if (m_eNextLevel == LEVEL::AMUSEMENTPARK)
		//	{
		//		CUI_HUD_Controller::COMMANDDESC	tCommandDesc;

		//		tCommandDesc.iCommandFlag |= COMMAND_HUD_DISABLE;
		//		tCommandDesc.iCommandFlag |= COMMAND_HUD_RELEASE;

		//		pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tCommandDesc));
		//	}
		//	//else
		//	//{
		//	//	CUI_HUD_Controller::COMMANDDESC	tCommandDesc;

		//	//	tCommandDesc.iCommandFlag |= COMMAND_HUD_DISABLE;

		//	//	pGameInstance->Get_Observer(TEXT("OBSERVER_HUD_CONTROLLER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_CONTROLLER, &tCommandDesc));
		//	//}

		//	pGameInstance->Get_CurrentLevel()->Set_NextLevel(pInstance);
		//}
	}
}

_int CVendingMachine::VerifyChecksum(void * pMessage)
{
	//if (nullptr == pMessage)
	//{
	//	return FALSE;
	//}
	//else
	//{
	//	const unsigned long*	check = (const unsigned long*)pMessage;

	//	//if (*check == CHECKSUM_COMMON_INTERACTION)
	//	//{
	//	//	return TRUE;
	//	//}
	//}
	return FALSE;
}

HRESULT CVendingMachine::Update_UI(_double dDeltaTime)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if (ANIMSTATE::OPEN == m_eAnimState)
	{
		CUI_HUD_Pointer::UIPOINTERDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eTargetType = CUI_HUD_Pointer::POINTERTYPE::OBJ;
		desc.dTimeDuration = 3.0;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_POINTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_POINTER, &desc));

		//m_iFlagUI ^= FLAG_UI_CALL_POINTER;
	}
	else
	{
		CUI_HUD_Pointer::UIPOINTERDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eTargetType = CUI_HUD_Pointer::POINTERTYPE::OBJ;
		desc.dTimeDuration = -1.0;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_POINTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_POINTER, &desc));

	}
	if (m_iFlagUI & FLAG_UI_CALL_DIRECTION)
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eObjState = m_eState;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));

		m_iFlagUI ^= FLAG_UI_CALL_DIRECTION;
	}
	return S_OK;
}

CVendingMachine * CVendingMachine::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CVendingMachine* pInstance = new CVendingMachine(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CVendingMachine * CVendingMachine::Clone(void * pArg)
{
	CVendingMachine* pInstance = new CVendingMachine(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CVendingMachine::Free()
{
	Safe_Release(m_pModel);
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);

	__super::Free();
}
