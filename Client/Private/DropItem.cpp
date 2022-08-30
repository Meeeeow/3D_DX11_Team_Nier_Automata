#include "stdafx.h"
#include "..\Public\DropItem.h"
#include "Player.h"
#include "UI_Dialogue_SystemMessage.h"
#include "UI_HUD_Pointer.h"
#include "UI_HUD_DirectionIndicator.h"
#include "Inventory.h"

CDropItem::CDropItem(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObjectModel(pGraphicDevice, pContextDevice)
{

}

CDropItem::CDropItem(const CDropItem & rhs)
	: CGameObjectModel(rhs)
{

}

CDropItem * CDropItem::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CDropItem* pInstance = new CDropItem(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CDropItem::Clone(void * pArg)
{
	CDropItem* pInstance = new CDropItem(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CDropItem::Free()
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

HRESULT CDropItem::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CDropItem::NativeConstruct(void * pArg)
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

	m_eAnimState = CDropItem::ANIMSTATE::INACTIVATE;

	SetUp_TreasureBox();

	Check_Pause();

	return S_OK;
}

HRESULT CDropItem::SetUp_Components()
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



	_uint	iTestValue = 1;
	wstring wstr = L"OBSERVER_TREASUREBOX_" + to_wstring(m_iObjID);
	pGameInstance->Create_Observer(wstr.c_str(), this);

	pGameInstance->Get_Observer(wstr.c_str())->Subscribe(TEXT("SUBJECT_PLAYER"));

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_INVENTORY, COM_KEY_INVENTORY, (CComponent**)&m_pInventory)))
		return E_FAIL;

	return S_OK;
}

void CDropItem::SetUp_TreasureBox()
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
		break;
	case 6:
		break;
	default:
		break;
	}
}

void CDropItem::Fill_Box1()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 1000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_SMALL)->iItemCount = 5;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE)->iItemCount = 1;

	return;
}

void CDropItem::Fill_Box2()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 3000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE)->iItemCount = 5;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_LARGE)->iItemCount = 2;

	return;
}

void CDropItem::Fill_Box3()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 30000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_LARGE)->iItemCount = 3;

	return;
}

void CDropItem::Fill_Box4()
{
	m_pInventory->Clear_Inventory();
	m_pInventory->Open_ItemSlot(ITEMCODE_MONEY)->iItemCount = 50000;
	m_pInventory->Open_ItemSlot(ITEMCODE_EXPENDABLES_RECOVERY_MIDDLE)->iItemCount = 2;

	return;
}

void CDropItem::Release_TreasureBox(CPlayer * pPlayer)
{
	if (nullptr == pPlayer)
		return;

	CInventory*	pPlayerInventory = nullptr;

	pPlayerInventory = dynamic_cast<CInventory*>(pPlayer->Get_Component(COM_KEY_INVENTORY));

	if (nullptr == pPlayerInventory)
		return;

	_uint iSize = (_uint)m_pInventory->Get_InventorySize();

	for (_uint i = 0; i < iSize; ++i)
	{
		ITEMDESC*	ItemDesc = m_pInventory->Open_ItemSlot(i);

		if (0 < ItemDesc->iItemCount)
		{
			pPlayerInventory->Open_ItemSlot(i)->iItemCount += ItemDesc->iItemCount;
			
			CUI_Dialogue_SystemMessage::UISYSMSGDESC	SysMsgDesc((_uint)i, ItemDesc->iItemCount, CUI_Dialogue_SystemMessage::iMsgAchieved);

			CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

			pGameInstance->Get_Observer(TEXT("OBSERVER_SYSMSG"))->Notify((void*)&PACKET(CHECKSUM_UI_DIALOGUE_SYSMSG, &SysMsgDesc));

			RELEASE_INSTANCE(CGameInstance);

			ItemDesc->iItemCount = 0;
		}
	}
}

_bool CDropItem::Check_Player()
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
				if (m_eAnimState == CDropItem::ANIMSTATE::INACTIVATE)
					m_eAnimState = CDropItem::ANIMSTATE::ENABLE;
				return true;
			}
			//if (fDistance <= 20.f)
			//	m_iFlagUI |= FLAG_UI_CALL_DIRECTION;
		}
		m_fRotation += 1.f;
		if (m_fRotation >= 45.f)
			m_fRotation = -m_fRotation;
	}
	if (m_eAnimState == CDropItem::ANIMSTATE::ENABLE)
		m_eAnimState = CDropItem::ANIMSTATE::INACTIVATE;
	return false;
}

_int CDropItem::Tick(_double dTimeDelta)
{
	switch (m_eAnimState)
	{
	case Client::CDropItem::ANIMSTATE::ENABLE:
		m_iFlagUI |= FLAG_UI_CALL_POINTER;
		m_iAnimIndex = 1;
		break;
	case Client::CDropItem::ANIMSTATE::ACTIVATE:
		m_iAnimIndex = 0;
		break;
	case Client::CDropItem::ANIMSTATE::DISABLE:
		m_iAnimIndex = 2;
		break;
	case Client::CDropItem::ANIMSTATE::INACTIVATE:
		m_iAnimIndex = 1;
		break;
	case Client::CDropItem::ANIMSTATE::NONE:
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

_int CDropItem::LateTick(_double dTimeDelta)
{
	m_pModel->Update_CombinedTransformationMatrix(dTimeDelta);

	m_pRenderer->Add_RenderGroup(CRenderer::RENDERGROUP::ALPHATEST, this);

	if (m_pHitBox != nullptr)
		m_pRenderer->Add_DebugGroup(CRenderer::DEBUGGROUP::COLLIDER, m_pHitBox);

	return 0;
}

HRESULT CDropItem::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

_bool CDropItem::Check_Collision(CGameObject * pGameObject, _uint iColliderType)
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

void CDropItem::Collision(CGameObject * pGameObject)
{
	if (m_iFlagCollision & FLAG_COLLISION_HITBOX)
	{
		m_pHitBox->Result_AABBToAABB(pGameObject->Get_ColliderAABB(), dynamic_cast<CTransform*>(pGameObject->Get_Component(COM_KEY_TRANSFORM)), dynamic_cast<CNavigation*>(pGameObject->Get_Component(COM_KEY_NAVIGATION)));
	}
	m_iFlagCollision &= FLAG_COLLISION_INITIALIZE;
}

void CDropItem::Notify(void * pMessage)
{
	if (TRUE == VerifyChecksum(pMessage))
	{
		if (m_eAnimState == CDropItem::ANIMSTATE::ENABLE)
		{
			m_eAnimState = CDropItem::ANIMSTATE::ACTIVATE;

			PACKET* pPacket = (PACKET*)pMessage;

			Release_TreasureBox(static_cast<CPlayer*>(pPacket->pData));
		}
	}
}

_int CDropItem::VerifyChecksum(void * pMessage)
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

HRESULT CDropItem::Update_UI(_double dDeltaTime)
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
