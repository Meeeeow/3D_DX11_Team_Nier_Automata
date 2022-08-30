#include "stdafx.h"
#include "..\Public\Camera_Debug.h"

#include "GameInstance.h"
#include "MainCamera.h"
#include "CameraMgr.h"


CCamera_Debug::CCamera_Debug(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{

}

CCamera_Debug::CCamera_Debug(const CCamera_Debug & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Debug::NativeConstruct_Prototype()
{
	__super::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCamera_Debug::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	CCameraMgr* pCameraMgr = CCameraMgr::Get_Instance();

	pCameraMgr->Set_DebugCam_To_Mgr(this);

	m_eState = OBJSTATE::DISABLE;


	return S_OK;
}

_int CCamera_Debug::Tick(_double TimeDelta)
{
	if (m_eState == OBJSTATE::ENABLE)
	{
		CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

		if (m_bOnce == false)
		{
			CTransform* pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr((_uint)LEVEL::STATIC, TEXT("LayerPlayer"), TEXT("Com_Transform"), 0);

			_vector vPosition = pPlayerTransform->Get_State(CTransform::STATE::POSITION);

			m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);

			m_bOnce = true;
		}
		

		if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		{
			m_pTransform->Move_Front(TimeDelta* 3.0);
		}

		if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		{
			m_pTransform->Move_Back(TimeDelta * 3.0);
		}

		if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		{
			m_pTransform->Move_Left(TimeDelta * 3.0);
		}

		if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		{
			m_pTransform->Move_Right(TimeDelta * 3.0);
		}

		_long		MouseMove = 0;

		if (pGameInstance->Key_Pressing(DIK_Q))
		{
			m_pTransform->Rotation_Axis(TimeDelta * -m_fPower);
		}
		if (pGameInstance->Key_Pressing(DIK_E))
		{
			m_pTransform->Rotation_Axis(TimeDelta * m_fPower);
		}
		if (pGameInstance->Key_Down(DIK_R))
		{
			m_fPower += 0.01f;
		}
		if (pGameInstance->Key_Down(DIK_T))
		{
			m_fPower -= 0.01f;
		}

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_X))
		{
			m_pTransform->Rotation_Axis(MouseMove * TimeDelta * 0.02f);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_Y))
		{
			m_pTransform->Rotation_Axis(MouseMove * TimeDelta * 0.02f, m_pTransform->Get_State(CTransform::STATE::RIGHT));
		}

		if (pGameInstance->Key_Down(DIK_BACKSLASH))
		{
			CCameraMgr* pCamMgr = CCameraMgr::Get_Instance();

			pCamMgr->Set_MainCam_On();

			CPlayer* pPlayer = (CPlayer*)pGameInstance->Get_GameObjectPtr((_uint)LEVEL::STATIC, L"LayerPlayer", 0);

			pPlayer->Set_Targeting_False();
		}

		RELEASE_INSTANCE(CGameInstance);

		return CCamera::Tick(TimeDelta);
	}
	else
	{
		m_bOnce = false;
	}
	return 0;
}

_int CCamera_Debug::LateTick(_double TimeDelta)
{
	return _int();
}

HRESULT CCamera_Debug::Render()
{
	return S_OK;
}

CCamera_Debug * CCamera_Debug::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CCamera_Debug* pGameInstance = new CCamera_Debug(pDevice, pDeviceContext);

	if (FAILED(pGameInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCamera_Debug");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

CCamera_Debug * CCamera_Debug::Clone(void * pArg)
{
	CCamera_Debug* pGameInstance = new CCamera_Debug(*this);

	if (FAILED(pGameInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCamera_Debug");
		Safe_Release(pGameInstance);
	}

	return pGameInstance;
}

void CCamera_Debug::Free()
{
	__super::Free();

}