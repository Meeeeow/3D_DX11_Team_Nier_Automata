#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{

}

HRESULT CPipeLine::Update_Variable()
{

	XMStoreFloat4x4(&m_matInverseView, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matView)));
	memcpy(&m_vCameraPosition, &m_matInverseView.m[3][0], sizeof(_float4));
	XMStoreFloat4x4(&m_matInverseProj, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matProj)));

	return S_OK;
}

void CPipeLine::Free()
{

}
