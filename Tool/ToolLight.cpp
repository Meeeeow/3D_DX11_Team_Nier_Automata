#include "stdafx.h"
#include "ToolLight.h"
#include "VIBuffer_Sphere.h"

CToolLight::CToolLight(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CToolObjectBase(pGraphicDevice, pContextDevice)
{
}

CToolLight::CToolLight(const CToolLight & rhs)
	: CToolObjectBase(rhs)
{
}

HRESULT CToolLight::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CToolLight::NativeConstruct(void * pArg)
{

	return S_OK;
}

_int CToolLight::Tick(_double dTimeDelta)
{

	return _int();
}

_int CToolLight::LateTick(_double dTimeDelta)
{

	return _int();
}

HRESULT CToolLight::Render()
{

	return S_OK;
}

LIGHTDESC CToolLight::Get_LightDesc()
{
	return LIGHTDESC();
}

HRESULT CToolLight::Set_LightDesc()
{
	return E_NOTIMPL;
}

CToolLight * CToolLight::Create(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
{
	CToolLight* pInstance = new CToolLight(pGraphicDevice, pContextDevice);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CToolLight::Clone(void * pArg)
{
	CToolLight* pInstance = new CToolLight(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CToolLight::Free()
{
	__super::Free();

}
