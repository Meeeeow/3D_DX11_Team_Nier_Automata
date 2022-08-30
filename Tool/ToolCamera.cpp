#include "stdafx.h"
#include "ToolCamera.h"
#include "GameInstance.h"


CToolCamera::CToolCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CCamera(pGraphicDevice, pContextDevice)
{
}

CToolCamera::CToolCamera(const CToolCamera & rhs)
	: CCamera(rhs)
{

}

HRESULT CToolCamera::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCamera::NativeConstruct_Prototype(CCamera::DESC tCameraDesc)
{
	if (FAILED(__super::NativeConstruct((void*)&tCameraDesc)))
		return E_FAIL;

	m_pTransform->Set_TransformDesc(tCameraDesc.tTransformDesc);

	return S_OK;
}

HRESULT CToolCamera::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_int CToolCamera::Tick(_double dTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	Update_Positions();

	D3D11_VIEWPORT		tViewportDesc;
	ZeroMemory(&tViewportDesc, sizeof(D3D11_VIEWPORT));
	_uint iNumViews = 1;
	m_pContextDevice->RSGetViewports(&iNumViews, &tViewportDesc);
	POINT pt = { tViewportDesc.Width / 2.f , tViewportDesc.Height / 2.f };

	ClientToScreen(g_hWnd, &pt);

	if (m_bReset)
	{
		m_fAccTime = 0;
		m_iNumEyeCheck = 0;
		m_iNumAtCheck = 0;
		m_bReset = false;
	}

	if (!m_bCamTestOn)
	{
		if (m_bAction)
			SetCursorPos(pt.x, pt.y);

		Move_Free(dTimeDelta);
	}


	else // m_bCamTestOn == true
	{
		if(m_bPlayCam)
		m_fAccTime += dTimeDelta * m_fSpeed;

		if (m_fAccTime >= 1.f)
			m_fAccTime = 1.f;

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

			if (m_iNumAtCheck + 1 >= m_vecAt.size() && m_iNumEyeCheck + 1 >= m_vecMovePoints.size()) // reset all
			{
				m_bFixAt = false;
				m_bFixEye = false;
				m_iNumAtCheck = 0;
				m_iNumEyeCheck = 0;

				if (m_vecAt.size() == 1)
				{
					m_bFixAt = true;
				}

				if (m_vecMovePoints.size() == 1)
				{
					m_bFixEye = true;
				}
			}
		}
		//내가 만들 함수.
		if (m_bPlayCam && m_bCamTestOn)
		Play_CamTest();
	}

	return __super::Tick(dTimeDelta);
}

_int CToolCamera::LateTick(_double dTimeDelta)
{
	return _int();
}

_vector CToolCamera::Get_CamPos()
{
	return m_pTransform->Get_State(CTransform::STATE::POSITION);
}

void CToolCamera::Move_Free(_double dTimeDelta)
{
	if (!m_bAction)
		return;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if ((pGameInstance->Key_Pressing(DIK_LEFTARROW)))
		m_pTransform->Move_Left(dTimeDelta *2.0);
	if ((pGameInstance->Key_Pressing(DIK_RIGHTARROW)))
		m_pTransform->Move_Right(dTimeDelta*2.0);
	if ((pGameInstance->Key_Pressing(DIK_UPARROW)))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_pTransform->Move_Up(dTimeDelta*2.0);
		else
			m_pTransform->Move_Front(dTimeDelta*2.0);
	} 
	if ((pGameInstance->Key_Pressing(DIK_DOWNARROW)))
	{
		if (pGameInstance->Key_Pressing(DIK_LSHIFT))
			m_pTransform->Move_Down(dTimeDelta*2.0);
		else
			m_pTransform->Move_Back(dTimeDelta*2.0);
	}

	if (pGameInstance->Mouse_Pressing(CInputDevice::DIMB::DIMB_RB))
	{
		_float fX = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_X);
		_float fY = pGameInstance->Get_DIMouseMoveState(CInputDevice::DIMM::DIMM_Y);

		m_pTransform->Rotation_Axis(dTimeDelta * fX * 0.05f);
		m_pTransform->Rotation_Axis(dTimeDelta * fY * 0.05f, m_pTransform->Get_State(CTransform::STATE::RIGHT));
	}
}

void CToolCamera::Play_Cam()
{
}

void CToolCamera::Play_CamTest()
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

void CToolCamera::Update_Positions()
{
	if (m_pCamObject == nullptr)
		return;

	m_vecMovePoints = *m_pCamObject->Get_MovingPoints();
	m_vecAt = *m_pCamObject->Get_At();
	m_fSpeed = m_pCamObject->Get_Speed();


}

CToolCamera * CToolCamera::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CToolCamera* pInstance = new CToolCamera(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);
	return pInstance;
}

CToolCamera * CToolCamera::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, CCamera::DESC tCameraDesc)
{
	CToolCamera* pInstance = new CToolCamera(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype(tCameraDesc)))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CToolCamera::Clone(void * pArg)
{
	CToolCamera* pInstance = new CToolCamera(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CToolCamera::Free()
{
	__super::Free();
}
