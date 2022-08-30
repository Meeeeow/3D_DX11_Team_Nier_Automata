#include "..\Public\GameObject.h"

#include "ComponentManager.h"
#include "Component.h"
#include "GameInstance.h"

CGameObject::CGameObject(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice)
	: m_pGraphicDevice(pGraphicDevice), m_pContextDevice(pContextDevice)
{
	Safe_AddRef(m_pContextDevice);
	Safe_AddRef(m_pGraphicDevice);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphicDevice(rhs.m_pGraphicDevice), m_pContextDevice(rhs.m_pContextDevice)
	, m_bBuffers(rhs.m_bBuffers), m_bCollision(rhs.m_bCollision), m_fAlpha(rhs.m_fAlpha)
	, m_bPause(rhs.m_bPause)
{
	Safe_AddRef(m_pContextDevice);
	Safe_AddRef(m_pGraphicDevice);
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CGameObject::Tick(_double dTimeDelta)
{
	return (_uint)m_eState;
}

_int CGameObject::LateTick(_double dTimeDelta)
{
	return _int();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Render_ShadowDepth()
{
	return S_OK;
}

HRESULT CGameObject::Render_MotionBlur()
{
	return S_OK;
}

HRESULT CGameObject::Render_RimDodge()
{
	return S_OK;
}

void CGameObject::Picking()
{

}

_int CGameObject::Collect_Event()
{
	return _int();
}

void CGameObject::Event()
{

}

void CGameObject::Notify(void * pMessage)
{

}

_int CGameObject::VerifyChecksum(void * pMessage)
{
	return 0;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentKey)
{
	auto& iter = find_if(m_mapComponents.begin(), m_mapComponents.end(), CTagFinder(pComponentKey));
	if (iter == m_mapComponents.end())
		return nullptr;

	return iter->second;
}

const char * CGameObject::Get_PassName(_uint iIndex)
{
	return nullptr;
}

_uint CGameObject::Get_MaxPassIndex()
{
	if (m_bBuffers == false)
		return 0;
	return _uint();
}

const char * CGameObject::Get_BufferContainerPassName(_uint iIndex, _uint iContainerSize)
{
	if (m_bBuffers == false)
		return nullptr;
	return nullptr;
}

_uint CGameObject::Get_BufferContainerMaxPassIndex(_uint iIndex)
{
	if (m_bBuffers == false)
		return 0;
	return _uint();
}

_uint CGameObject::Get_BufferContainerSize()
{
	if (m_bBuffers == false)
		return 0;
	return _uint();
}

_uint CGameObject::Get_BufferContainerPassIndex(_uint iIndex)
{
	if (m_bBuffers == false)
		return 0;
	return _uint();
}

_bool CGameObject::Check_Collision(CGameObject * pGameObject, unsigned int iColliderType)
{
	return _bool();
}

void CGameObject::Collision(CGameObject * pGameObject)
{
	return;
}

BoundingBox * CGameObject::Get_AABB()
{
	return nullptr;
}

BoundingSphere * CGameObject::Get_Sphere()
{
	return nullptr;
}

CCollider * CGameObject::Get_ColliderAABB()
{
	return nullptr;
}

CCollider * CGameObject::Get_ColliderSphere()
{
	return nullptr;
}

void CGameObject::Set_Force(_float3 vDir, _float fValue)
{
	m_vForceDir = vDir;
	m_fForce = fValue;
}

_vector CGameObject::Get_MinPos()
{
	return XMVectorZero();
}

_vector CGameObject::Get_MaxPos()
{
	return XMVectorZero();
}

void CGameObject::Set_BufferContainerPassIndex(_uint iIndex, _uint iPassIndex)
{
	if (m_bBuffers == false)
		return;
}


HRESULT CGameObject::Add_Components(_uint iLevelIndex, const _tchar * pComponentTag, const _tchar * pComponentKey, CComponent ** ppOut, void * pArg)
{
	CComponentManager* pComponentManager = CComponentManager::Get_Instance();

	CComponent* pComponent = pComponentManager->Clone_Component(iLevelIndex, pComponentTag, pArg);
	if (pComponent == nullptr)
		return E_FAIL;

	auto& iter = find_if(m_mapComponents.begin(), m_mapComponents.end(), CTagFinder(pComponentKey));
	if (iter != m_mapComponents.end())
		return E_FAIL;

	m_mapComponents.emplace(pComponentKey, pComponent);

	*ppOut = pComponent;
	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CGameObject::Update_UI(_double dDeltaTime)
{
	_double TimeAcc = dDeltaTime;

	return S_OK;
}

void CGameObject::Compute_CamDistanceForAlpha(CTransform * pTransform)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_vector		vMyPosition = pTransform->Get_State(CTransform::STATE::POSITION);
	_vector		vCamPosition = pGameInstance->Get_CameraPosition();
	m_fCameraDistance = XMVectorGetX(XMVector3Length(vCamPosition - vMyPosition));

	RELEASE_INSTANCE(CGameInstance);
}

void CGameObject::Compute_CamDistanceForAlpha(CTransform * pTransform, _float& fDistance)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	_vector		vMyPosition = pTransform->Get_State(CTransform::STATE::POSITION);
	_vector		vCamPosition = pGameInstance->Get_CameraPosition();
	fDistance = XMVectorGetX(XMVector3Length(vCamPosition - vMyPosition));

	RELEASE_INSTANCE(CGameInstance);
}

const _uint CGameObject::Get_RandomAtt() const
{
	if (10 <= m_tObjectStatusDesc.iAtt)
	{
		if (100000 <= m_tObjectStatusDesc.iAtt)
			return (m_tObjectStatusDesc.iAtt + (rand() % 100000));
		else if (10000 <= m_tObjectStatusDesc.iAtt)
			return (m_tObjectStatusDesc.iAtt + (rand() % 10000));
		else if (1000 <= m_tObjectStatusDesc.iAtt)
			return (m_tObjectStatusDesc.iAtt + (rand() % 1000));
		else if (100 <= m_tObjectStatusDesc.iAtt)
			return (m_tObjectStatusDesc.iAtt + (rand() % 100));
		return (m_tObjectStatusDesc.iAtt + rand() % 10);
	}
	else
		return (rand() % 9) + 1;
}

HRESULT CGameObject::Check_Pause()
{
	Set_Pause(CGameInstance::Get_Instance()->Get_Pause());
	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Component : m_mapComponents)
		Safe_Release(Component.second);

	m_mapComponents.clear();

	Safe_Release(m_pContextDevice);
	Safe_Release(m_pGraphicDevice);
}
