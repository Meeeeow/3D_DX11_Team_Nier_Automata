#include "stdafx.h"
#include "ToolObjectBase.h"
#include "GameInstance.h"


CToolObjectBase::CToolObjectBase(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: CGameObject(pGraphicDevice, pContextDevice)
{

}

CToolObjectBase::CToolObjectBase(const CToolObjectBase & rhs)
	: CGameObject(rhs)
{
}

HRESULT CToolObjectBase::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolObjectBase::NativeConstruct(void * pArg)
{

	return S_OK;
}

_int CToolObjectBase::Tick(_double dTimeDelta)
{
	return __super::Tick(dTimeDelta);
}

_int CToolObjectBase::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CToolObjectBase::Render()
{
	return S_OK;
}

HRESULT CToolObjectBase::SetUp_Components()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_TRANSFORM, COM_KEY_TRANSFORM, (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(Add_Components((_uint)LEVEL::STATIC, PROTO_KEY_RENDERER, COM_KEY_RENDERER, (CComponent**)&m_pRenderer)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CToolObjectBase::SetUp_ConstantTable()
{
	

	return S_OK;
}

CGameObject * CToolObjectBase::Clone(void * pArg)
{
	return nullptr;
}

void CToolObjectBase::Free()
{
	__super::Free();
	Safe_Release(m_pTransform);
	Safe_Release(m_pRenderer);
//	Safe_Release(m_pCollider);

}
