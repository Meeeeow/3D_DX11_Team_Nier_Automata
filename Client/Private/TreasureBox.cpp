#include "stdafx.h"
#include "..\Public\TreasureBox.h"
#include "Player.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_HUD_Pointer.h"
#include "UI_HUD_DirectionIndicator.h"
#include "UI_HUD_StageObjective.h"
#include "Inventory.h"
#include "EffectFactory.h"
#include "SoundMgr.h"

const	_tchar*		CTreasureBox::TREASUREBOX_OBSERVER_KEY[] = {
	TEXT("OBSERVER_TREASUREBOX_0"),
	TEXT("OBSERVER_TREASUREBOX_1"),
	TEXT("OBSERVER_TREASUREBOX_2"),
	TEXT("OBSERVER_TREASUREBOX_3"),
	TEXT("OBSERVER_TREASUREBOX_4"),
	TEXT("OBSERVER_TREASUREBOX_5"),
};


CTreasureBox::CTreasureBox(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CTreasureBox::CTreasureBox(const CTreasureBox & rhs)
	: CGameObjectModel(rhs)
{

}

CTreasureBox * CTreasureBox::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CTreasureBox* pInstance = new CTreasureBox(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CTreasureBox::Clone(void * pArg)
{
	CTreasureBox* pInstance = new CTreasureBox(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTreasureBox::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pHitBox);
	Safe_Release(m_pMiddlePoint);
	Safe_Release(m_pNavigation);
	Safe_Release(m_pInventory);
}

HRESULT CTreasureBox::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CTreasureBox::NativeConstruct(void * pArg)
{
	// UI에서 확인할 고유 ObjID 값 부여
	static _uint iCTreasureBox_InstanceID = OriginID_TreasureBox;

	m_iObjID = iCTreasureBox_InstanceID++;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		MO_INFO* pInfo = (MO_INFO*)pArg;
		m_pTransform->Set_WorldMatrix(XMLoadFloat4x4(&pInfo->m_matWorld));
	}

	// 충돌 체크에 참여
	m_bCollision = true;

	// Pass_Anim
	m_iPassIndex = 1;

	m_eAnimState = CTreasureBox::ANIMSTATE::INACTIVATE;

	SetUp_TreasureBox();

	Check_Pause();

	return S_OK;
}

HRESULT CTreasureBox::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, TEXT("Com_Renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, TEXT("Com_Transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, L"TreasureBox", TEXT("Com_Model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	/* For.Com_Navigation*/
	_uint	iCurrentIndex = 0;
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	LEVEL eLevelID = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	if (eLevelID == LEVEL::LOADING)
		eLevelID = static_cast<LEVEL>(pGameInstance->Get_NextLevelID());

	switch (eLevelID)
	{
	case Client::LEVEL::STAGE1:
		if (FAILED(Add_Components((_uint)LEVEL::STAGE1, PROTO_KEY_NAVIGATION_DEFAULT, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	case Client::LEVEL::ROBOTMILITARY:
		break;
	case Client::LEVEL::AMUSEMENTPARK:
		if (FAILED(Add_Components((_uint)LEVEL::AMUSEMENTPARK, PROTO_KEY_NAVIGATION_AMUSE, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	case Client::LEVEL::OPERABACKSTAGE:
		if (FAILED(Add_Components((_uint)LEVEL::OPERABACKSTAGE, PROTO_KEY_NAVIGATION_OPERA, COM_KEY_NAVIGATION, (CComponent**)&m_pNavigation, &iCurrentIndex)))
			return E_FAIL;
		break;
	default:
		break;
	}

	/* For.Com_AABB */
	Engine::CCollider::DESC		ColliderDesc;

	ColliderDesc.vPivot = m_pModel->Get_MiddlePoint();
	ColliderDesc.fRadius = m_pModel->Get_Radius();
	ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_AABB, COM_KEY_HITBOX, (CComponent**)&m_pHitBox, &ColliderDesc)))
		return E_FAIL;
	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_COLLIDER_SPHERE, COM_KEY_MIDDLEPOINT, (CComponent**)&m_pMiddlePoint, &ColliderDesc)))
		return E_FAIL;


	if (LEVEL::STAGE1 != eLevelID)
	{
		//wstring wstr = L"OBSERVER_TREASUREBOX_" + to_wstring(m_iObjID);

		//pGameInstance->Create_Observer(wstr.c_str(), this);

		pGameInstance->Create_Observer(TREASUREBOX_OBSERVER_KEY[m_iObjID - OriginID_TreasureBox], this);

		//pGameInstance->Get_Observer(wstr.c_str())->Subscribe(TEXT("SUBJECT_PLAYER"));

		pGameInstance->Get_Observer(TREASUREBOX_OBSERVER_KEY[m_iObjID - OriginID_TreasureBox])->Subscribe(TEXT("SUBJECT_PLAYER"));
	}

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_INVENTORY, COM_KEY_INVENTORY, (CComponent**)&m_pInventory)))
		return E_FAIL;

	return S_OK;
}

void CTreasureBox::SetUp_TreasureBox()
{
	// 박스 번호에 따라 인벤토리에 각각 다른 구성품 할당

	static _uint	iBoxIndex = 0;
	
	switch (++iBoxIndex)
	{
	case 1:
		Fill_Box1();
		break;
	case 2:
		Fill_Box2();
		break;
	case 3:
		Fill_Box3();
		break;
	case 4:
		Fill_Box4();
		break;
	case 5:
		Fill_Box5();
		break;
	case 6:
		Fill_Box6();
		break;
	default:
		break;
	}
}

void CTreasureBox::Fill_Box1()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 1000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_SMALL)->iItemCount = 3;
	m_pInventory->Open_ItemSlot(ITEMCODE_EQUIPMENT_40TH_SPEAR)->iItemCount = 1;
	m_pInventory->Open_ItemSlot(ITEMCODE_ENFORCECHIP_SKILL_LASER)->iItemCount = 1;

	return;
}

void CTreasureBox::Fill_Box2()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 2000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_SMALL)->iItemCount = 3;
	m_pInventory->Open_ItemSlot(ITEMCODE_EQUIPMENT_40TH_KNUCKLE)->iItemCount = 1;
	m_pInventory->Open_ItemSlot(ITEMCODE_ENFORCECHIP_SKILL_LIGHT)->iItemCount = 1;

	return;
}

void CTreasureBox::Fill_Box3()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount += 3000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE)->iItemCount += 3;

	_uint	iItemSlot = 0;
	_uint	iItemCount = 0;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	return;
}

void CTreasureBox::Fill_Box4()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount += 4000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE)->iItemCount += 3;

	_uint	iItemSlot = 0;
	_uint	iItemCount = 0;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	return;
}

void CTreasureBox::Fill_Box5()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount += 5000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_LARGE)->iItemCount += 3;

	_uint	iItemSlot = 0;
	_uint	iItemCount = 0;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	return;
}

void CTreasureBox::Fill_Box6()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount += 6000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_EXLARGE)->iItemCount += 1;

	_uint	iItemSlot = 0;
	_uint	iItemCount = 0;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	iItemSlot = (rand() % 10) + 20;
	iItemCount = (rand() % 15) + 1;

	m_pInventory->Open_ItemSlot(iItemSlot)->iItemCount = iItemCount;

	return;
}

void CTreasureBox::Release_TreasureBox(CPlayer * pPlayer)
{
	if (nullptr == pPlayer)
		return;

	CInventory*	pPlayerInventory = nullptr;

	pPlayerInventory = dynamic_cast<CInventory*>(pPlayer->Get_Component(COM_KEY_INVENTORY));

	if (nullptr == pPlayerInventory)
		return;

	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	_uint iSize = (_uint)m_pInventory->Get_InventorySize();

	for (_uint i = 0; i < iSize; ++i)
	{
		ITEMDESC*	ItemDesc = m_pInventory->Open_ItemSlot(i);

		if (0 < ItemDesc->iItemCount)
		{
			pPlayerInventory->Open_ItemSlot(i)->iItemCount += ItemDesc->iItemCount;
			
			CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc((_uint)i, ItemDesc->iItemCount, CUI_Dialogue_SystemMessage::iMsgAchieved);
		
			pGameInstance->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));

			ItemDesc->iItemCount = 0;
		}
	}

	CUI_HUD_StageObjective::UIOBJECTIVEDESC		ObjectiveDesc(CUI_HUD_StageObjective::QUEST::TRESUREBOX);

	pGameInstance->Get_Observer(TEXT("OBSERVER_STAGE_OBJECTIVE"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_STAGE_OBJECTIVE, &ObjectiveDesc));

	CSoundMgr::Get_Instance()->StopSound(CSoundMgr::CHANNELID::UI);
	CSoundMgr::Get_Instance()->PlaySound(L"TB_Open.mp3", CSoundMgr::CHANNELID::UI);
}

_bool CTreasureBox::Check_Player()
{
	Engine::CCollider* pInstance = dynamic_cast<Engine::CCollider*>(CGameInstance::Get_Instance()->Get_ComponentPtr((_uint)LEVEL::STATIC, L"LayerPlayer", COM_KEY_MIDDLEPOINT, 0));
	if (pInstance != nullptr)
	{
		_vector vPivot = XMLoadFloat3(&m_pModel->Get_MiddlePoint()) + m_pTransform->Get_State(CTransform::STATE::POSITION);
		_vector vRay = m_pTransform->Get_State(CTransform::STATE::LOOK);
		_matrix matRotation = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_fRotation));

		vRay = XMVector4Transform(vRay, matRotation);

		_float3 vPosition = _float3(0.f, 0.f, 0.f);
		_float	fDistance = 0.f;
		if (pInstance->Collision_Sphere(vPivot, vRay, vPosition, fDistance))
		{
			m_fRotation = 0;
			if (fDistance <= 3.f)
			{
				if (m_eAnimState == CTreasureBox::ANIMSTATE::INACTIVATE)
				{
					m_eAnimState = CTreasureBox::ANIMSTATE::ENABLE;
				}
				return true;
			}
			//if (fDistance <= 20.f)
			//	m_iFlagUI |= FLAG_UI_CALL_DIRECTION;
		}
		m_fRotation += 1.f;
		if (m_fRotation >= 45.f)
			m_fRotation = -m_fRotation;
	}
	if (m_eAnimState == CTreasureBox::ANIMSTATE::ENABLE)
		m_eAnimState = CTreasureBox::ANIMSTATE::INACTIVATE;
	return false;
}

_int CTreasureBox::Tick(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	if ((_uint)LEVEL::STAGE1 == pGameInstance->Get_LevelID() && false == m_bIsObserverSet)
	{
		//wstring wstr = L"OBSERVER_TREASUREBOX_" + to_wstring(m_iObjID);

		//pGameInstance->Create_Observer(wstr.c_str(), this);

		pGameInstance->Create_Observer(TREASUREBOX_OBSERVER_KEY[m_iObjID - OriginID_TreasureBox], this);

		//pGameInstance->Get_Observer(wstr.c_str())->Subscribe(TEXT("SUBJECT_PLAYER"));

		pGameInstance->Get_Observer(TREASUREBOX_OBSERVER_KEY[m_iObjID - OriginID_TreasureBox])->Subscribe(TEXT("SUBJECT_PLAYER"));
	}

	switch (m_eAnimState)
	{
	case Client::CTreasureBox::ANIMSTATE::ENABLE:
		m_iFlagUI |= FLAG_UI_CALL_POINTER;
		m_iAnimIndex = 1;
		break;
	case Client::CTreasureBox::ANIMSTATE::ACTIVATE:
		m_iAnimIndex = 0;
		break;
	case Client::CTreasureBox::ANIMSTATE::DISABLE:
		m_iAnimIndex = 2;
		break;
	case Client::CTreasureBox::ANIMSTATE::INACTIVATE:
		m_iAnimIndex = 1;
		break;
	case Client::CTreasureBox::ANIMSTATE::NONE:
		break;
	default:
		break;
	}
	Check_Player();

	Update_UI(dTimeDelta);

	Update_Collider();


	m_pModel->Set_Animation(m_iAnimIndex);	

	m_pModel->Synchronize_Animation(m_pTransform, m_pNavigation);

	return (_int)m_eState;
}

_int CTreasureBox::LateTick(_double dTimeDelta)
{
	CGameInstance*	pGameInstance = CGameInstance::Get_Instance();

	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	if (pGameInstance->Culling(m_pTransform, m_pModel->Get_Radius()))
	{
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);
		m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::SHADOWDEPTH, this);
	}

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return 0;
}

HRESULT CTreasureBox::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasureBox::Create_OpenEffect()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	CEffectFactory* pFactory = CEffectFactory::Get_Instance();
	EFFECT_PACKET tPacket;
	tPacket.eLevel = static_cast<LEVEL>(pGameInstance->Get_LevelID());
	tPacket.fMaxLifeTime = 1.f; // turn off it when i want;
	XMStoreFloat4(&tPacket.tEffectMovement.vPosition, m_pTransform->Get_State(CTransform::STATE::POSITION));

	tPacket.ulID = CHECKSUM_EFFECT_9S_APPEAR;
	pFactory->Load_Effect(L"..\\Bin\\Data\\EffectSaveData\\9S_Appear.dat", &tPacket);

	return S_OK;
}

_bool CTreasureBox::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
{
	if (iColliderType == (_uint)CCollider::TYPE::AABB)
	{
		if (m_pHitBox->Collision_AABBToAABB(pGameObject->Get_ColliderAABB()))
		{
			m_iFlagCollision |= FLAG_COLLISION_HITBOX;
			return true;
		}
	}
	return false;
}

void CTreasureBox::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CTreasureBox::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		if (m_eAnimState == CTreasureBox::ANIMSTATE::ENABLE)
		{
			m_eAnimState = CTreasureBox::ANIMSTATE::ACTIVATE;

			PACKET* pPacket = (PACKET*)pMessage;

			Release_TreasureBox(static_cast<CPlayer*>(pPacket->pData));
			Create_OpenEffect();
		}
	}
}

_int CTreasureBox::VerifyChecksum(void * pMessage)
{
	if (nullptr == pMessage)
	{
		return FALSE;
	}
	else
	{
		const unsigned long*	check = (const unsigned long*)pMessage;

		if (*check == CHECKSUM_COMMON_INTERACTION)
		{
			return TRUE;
		}
	}
	return FALSE;
}

HRESULT CTreasureBox::Update_UI(_double dDeltaTime)
{
	if (m_iFlagUI & FLAG_UI_CALL_POINTER)
	{
		CUI_HUD_Pointer::UIPOINTERDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eTargetType = CUI_HUD_Pointer::POINTERTYPE::OBJ;
		desc.dTimeDuration = 3.0;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_POINTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_POINTER, &desc));

		RELEASE_INSTANCE(CGameInstance);

		m_iFlagUI ^= FLAG_UI_CALL_POINTER;
	}
	else
	{
		CUI_HUD_Pointer::UIPOINTERDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eTargetType = CUI_HUD_Pointer::POINTERTYPE::OBJ;
		desc.dTimeDuration = -1.0;

		XMStoreFloat4(&desc.vWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_POINTER"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_POINTER, &desc));

		RELEASE_INSTANCE(CGameInstance);
	}
	if (m_iFlagUI & FLAG_UI_CALL_DIRECTION)
	{
		CUI_HUD_DirectionIndicator::UIINDICATORDESC	desc;

		desc.iObjID = m_iObjID;
		desc.eObjState = m_eState;
		desc.eIndicatorType = CUI_HUD_DirectionIndicator::INDICATORTYPE::OBJECTIVE;

		XMStoreFloat4(&desc.vTargetWorldPos, m_pTransform->Get_State(CTransform::STATE::POSITION));

		CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

		pGameInstance->Get_Observer(TEXT("OBSERVER_DIRECTION_INDICATOR"))->Notify((void*)&PACKET(CHECKSUM_UI_HUD_DIRECTION_INDICATOR, &desc));

		RELEASE_INSTANCE(CGameInstance);

		m_iFlagUI ^= FLAG_UI_CALL_DIRECTION;
	}
	return S_OK;
}
