#include "stdafx.h"
#include "..\Public\Camera_IronFist.h"
#include "CameraMgr.h"
#include "MinigameManager.h"

CCamera_IronFist::CCamera_IronFist(_pGraphicDevice pDevice, _pContextDevice pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_IronFist::CCamera_IronFist(const CCamera_IronFist & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_IronFist::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCamera_IronFist::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	pCameraMgr->Set_IronFistCam_To_Mgr(this);

	m_eState = OBJSTATE::DISABLE;

	return S_OK;
}

_int CCamera_IronFist::Tick(_double TimeDelta)
{
	if (OBJSTATE::ENABLE == m_eState)
	{
		//Update_Position(TimeDelta);
		return CCamera::Tick(TimeDelta);
	}
	
	return 0;
}

_int CCamera_IronFist::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_IronFist::Render()
{
	return S_OK;
}

void CCamera_IronFist::Update_Position(_double TimeDelta)
{
	CMinigameManager* pMiniGame = CMinigameManager::Get_Instance();
	if (nullptr == pMiniGame)
		return;

	CGameObject* pGameObject = pMiniGame->Find_Player(MINIGAME_IRONFIST);
	if (nullptr == pGameObject)
		return;

	CTransform* pTransform = (CTransform*)pGameObject->Get_Component(COM_KEY_TRANSFORM);
	if (nullptr == pTransform)
		return;

	_vector vPlayPos = pTransform->Get_State(CTransform::STATE::POSITION);
	_vector vMyPos = XMLoadFloat4(&m_vPrePos);

	vMyPos = XMVectorSetY(vPlayPos, XMVectorGetY(vPlayPos));
	vMyPos = XMVectorSetZ(vPlayPos, XMVectorGetZ(vPlayPos));

	_vector vLerpPos = XMVectorLerp(vMyPos, vPlayPos, _float(1.0));
	vLerpPos = XMVectorSetW(vLerpPos, 1.f);

	XMStoreFloat4(&m_vPrePos, vLerpPos);

	_vector vPrePos = m_pTransform->Get_State(CTransform::STATE::POSITION);
	
	vLerpPos = XMVectorSetY(vLerpPos, XMVectorGetY(vPrePos));
	vLerpPos += XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * -6.f;

	m_pTransform->Set_State(CTransform::STATE::POSITION, vLerpPos);
}

void CCamera_IronFist::Set_LookAt()
{
	//CMinigameManager* pMiniGame = CMinigameManager::Get_Instance();
	//if (nullptr == pMiniGame)
	//	return;

	//CGameObject* pGameObject = pMiniGame->Find_Player(MINIGAME_IRONFIST);
	//if (nullptr == pGameObject)
	//	return;

	//CTransform* pTransform = (CTransform*)pGameObject->Get_Component(COM_KEY_TRANSFORM);
	//if (nullptr == pTransform)
	//	return;

	//_vector vPlayPos = pTransform->Get_State(CTransform::STATE::POSITION);
	//_vector vPlayRight = XMVector3Normalize(pTransform->Get_State(CTransform::STATE::RIGHT));

	//_vector vMyLook = XMVectorZero();
	//_vector vMyUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	//_vector vMyPos = vPlayPos;

	//vMyLook = vPlayRight;

	//_vector vMyRight = XMVector3Cross(vMyUp, vMyLook);

	//m_pTransform->Set_State(CTransform::STATE::RIGHT, vMyRight);
	//m_pTransform->Set_State(CTransform::STATE::LOOK, vMyLook);

	//vMyPos += XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::UP)) * 2.f;
	//vMyPos += XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE::LOOK)) * -6.f;

	//XMStoreFloat4(&m_vPrePos, vMyPos);

	//m_pTransform->Set_State(CTransform::STATE::POSITION, vMyPos);

	m_pTransform->Set_State(CTransform::STATE::RIGHT, XMVectorSet(1.f, 0.f, 0.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::UP, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
	m_pTransform->Set_State(CTransform::STATE::POSITION, XMVectorSet(4.8f, 2.5f, -8.9f, 1.f));
}

CCamera_IronFist * CCamera_IronFist::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_IronFist* pGameInstance = new CCamera_IronFist(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Create CCamera_IronFist");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CCamera_IronFist * CCamera_IronFist::Clone(void * pArg)
{
	CCamera_IronFist* pGameInstance = new CCamera_IronFist(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Create CCamera_IronFist");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCamera_IronFist::Free()
{
	__super::Free();
}
