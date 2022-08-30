#include "..\Public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{

}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
{

}

HRESULT CCamera::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (pArg != nullptr)
		memcpy(&m_tDesc, pArg, sizeof(CCamera::DESC));

	m_pTransform = CTransform::Create(m_pGraphicDevice, m_pContextDevice);
	if (m_pTransform == nullptr)
		return E_FAIL;

	m_pTransform->Set_TransformDesc(m_tDesc.tTransformDesc);

	if (FAILED(Syncronize_CameraState()))
		return E_FAIL;

	return S_OK;
}

_int CCamera::Tick(_double dTimeDelta)
{
	CPipeLine* pPipeLine = CPipeLine::Get_Instance();

	_matrix		matWorld = m_pTransform->Get_WorldMatrix();
	_matrix		matView = XMMatrixInverse(nullptr, matWorld);
	pPipeLine->Set_Transform(CPipeLine::TRANSFORM::D3DTS_VIEW, matView);

 	_matrix		matProj = XMMatrixPerspectiveFovLH(m_tDesc.fFOV, m_tDesc.fAspect, m_tDesc.fNear, m_tDesc.fFar);
	pPipeLine->Set_Transform(CPipeLine::TRANSFORM::D3DTS_PROJ, matProj);

	return 0;
}

_int CCamera::LateTick(_double dTimeDelta)
{
	return _int();
}

const _matrix CCamera::Get_WorldMatrix()
{
	if (nullptr == m_pTransform)
		return XMMatrixIdentity();

	return m_pTransform->Get_WorldMatrix();
}

CTransform * CCamera::Get_Transform()
{
	if (nullptr == m_pTransform)
		return nullptr;

	return m_pTransform;
}

HRESULT CCamera::Syncronize_CameraState()
{
	_vector		vLook = XMLoadFloat3(&m_tDesc.vAt) - XMLoadFloat3(&m_tDesc.vEye);
	m_pTransform->Set_State(CTransform::STATE::LOOK, XMVector3Normalize(vLook));

	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_tDesc.vAxis), vLook);
	m_pTransform->Set_State(CTransform::STATE::RIGHT, XMVector3Normalize(vRight));

	_vector		vUp = XMVector3Cross(vLook, vRight);
	m_pTransform->Set_State(CTransform::STATE::UP, XMVector3Normalize(vUp));

	_vector		vPosition = XMLoadFloat3(&m_tDesc.vEye);
	vPosition = XMVectorSetW(vPosition, 1.f);
	m_pTransform->Set_State(CTransform::STATE::POSITION, vPosition);


	CPipeLine* pPipeLine = CPipeLine::Get_Instance();
	pPipeLine->Set_Near(m_tDesc.fNear);
	pPipeLine->Set_Far(m_tDesc.fFar);
	pPipeLine->Set_FOV(m_tDesc.fFOV);

	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
}
