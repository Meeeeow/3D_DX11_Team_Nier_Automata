#include "..\Public\GameObjectManager.h"
#include "GameObject.h"
#include "Component.h"
#include "GraphicDebugger.h"
#include "Collision.h"
#include "GraphicDebugger.h"

IMPLEMENT_SINGLETON(CGameObjectManager)

CGameObjectManager::CGameObjectManager()
{

}

HRESULT CGameObjectManager::Reserve_Container(_uint iNumLevels)
{
	if (m_pLayers != nullptr)
		FAILMSG("Already Reserved - GameObjectManager Reserve Container");

	m_pLayers = new LAYERS[iNumLevels];
	m_iNumLevels = iNumLevels;

	return S_OK;
}

CGameObject * CGameObjectManager::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	auto&	pIterPrototype = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTagFinder(pPrototypeTag));
	if (pIterPrototype == m_mapPrototype.end())
		NULLMSG("No Prototype with the same name - CObjectManager Insert Object to Layer");

	CGameObject*	pGameObject = pIterPrototype->second->Clone(pArg);
	if (pGameObject == nullptr)
		return nullptr;

	
	return pGameObject;
}

HRESULT CGameObjectManager::Add_GameObject(const _tchar * pPrototypeTag, CGameObject * pGameObject)
{
	if (pGameObject == nullptr)
		FAILMSG("Attempt Insert Nullptr - ObjectManager Add Prototype");

	if (m_mapPrototype.end() != find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTagFinder(pPrototypeTag)))
		FAILMSG("Prototype With the same name already exists - ObjectManager's Add Prototype");

	m_mapPrototype.emplace(pPrototypeTag, pGameObject);

#ifdef _DEBUG
	wstring wstr = pPrototypeTag;
	string strTag(wstr.begin(), wstr.end());
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKGREEN);
	GRAPHICDEBUGGER()->Notice(strTag.c_str(), false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	GRAPHICDEBUGGER()->Notice(" Prototype Create");
#endif // _DEBUG


	return S_OK;
}

HRESULT CGameObjectManager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, void * pArg)
{
	if (m_iNumLevels <= iLevelIndex)
		FAILMSG("LevelIndex is too big - ObjectManager Insert Object to Layer");

	auto&	pIterPrototype = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTagFinder(pPrototypeTag));
	if (pIterPrototype == m_mapPrototype.end())
		FAILMSG("No Prototype with the same name - CObjectManager Insert Object to Layer");

	CGameObject*	pGameObject = pIterPrototype->second->Clone(pArg);
	if (pGameObject == nullptr)
		return E_FAIL;




	auto&	pIterLayer = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (pIterLayer == m_pLayers[iLevelIndex].end())
	{
		CLayer*	pLayer = CLayer::Create();
		if (pLayer == nullptr)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
#ifdef _DEBUG
		pLayer->Set_LayerName(pLayerTag);
#endif // _DEBUG
	}
	else
		pIterLayer->second->Add_GameObject(pGameObject);

#ifdef _DEBUG
	wstring wstrPrototype = pPrototypeTag;
	wstring wstrLayerTag = pLayerTag;
	string  strPrototype(wstrPrototype.begin(), wstrPrototype.end());
	string  strLayer(wstrLayerTag.begin(), wstrLayerTag.end());
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
	GRAPHICDEBUGGER()->Notice(strLayer.c_str(), CGraphicDebugger::COLOR::DARKYELLOW,
		" In ", CGraphicDebugger::COLOR::WHITE, strPrototype.c_str(), CGraphicDebugger::COLOR::DARKGREEN);
#endif // _DEBUG

	return S_OK;
}

HRESULT CGameObjectManager::Add_GameObjectToLayer(_uint iLevelIndex, CGameObject * pGameObject, const _tchar * pLayerTag)
{
	if (m_iNumLevels <= iLevelIndex)
		return E_FAIL;

	auto&	pIterLayer = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (pIterLayer == m_pLayers[iLevelIndex].end())
	{
		CLayer*	pLayer = CLayer::Create();
		if (pLayer == nullptr)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pIterLayer->second->Add_GameObject(pGameObject);



	return S_OK;
}

HRESULT CGameObjectManager::Add_GameObjectToLayerWithPtr(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pLayerTag, CGameObject** ppOut, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	auto	iterPrototype = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTagFinder(pPrototypeTag));
	if (iterPrototype == m_mapPrototype.end())
		return E_FAIL;

	CGameObject*		pGameObject = iterPrototype->second->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	auto	iterLayer = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iterLayer == m_pLayers[iLevelIndex].end())
	{
		CLayer*		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
#ifdef _DEBUG
		pLayer->Set_LayerName(pLayerTag);
#endif // _DEBUG
	}
	else
		iterLayer->second->Add_GameObject(pGameObject);

	*ppOut = pGameObject;

#ifdef _DEBUG
	wstring wstrPrototype = pPrototypeTag;
	wstring wstrLayerTag = pLayerTag;
	string  strPrototype(wstrPrototype.begin(), wstrPrototype.end());
	string  strLayer(wstrLayerTag.begin(), wstrLayerTag.end());
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
	GRAPHICDEBUGGER()->Notice(strLayer.c_str(), CGraphicDebugger::COLOR::DARKYELLOW,
		" In Ptr ", CGraphicDebugger::COLOR::WHITE, strPrototype.c_str(), CGraphicDebugger::COLOR::DARKGREEN);
#endif // _DEBUG

	return S_OK;
}

HRESULT CGameObjectManager::Add_CollisionTag(COLLISION_TAG * pCollisionTag)
{
	if (pCollisionTag == nullptr)
		return E_FAIL;

	m_listCollisionTag.emplace_back(pCollisionTag);

	return S_OK;
}

_bool CGameObjectManager::IsFind_Prototype(const _tchar * pPrototypeTag)
{
	auto& iter = find_if(m_mapPrototype.begin(), m_mapPrototype.end(), CTagFinder(pPrototypeTag));
	if (iter == m_mapPrototype.end())
		return false;

	return true;
}

const _tchar * CGameObjectManager::Get_LayerName(_uint iLevelIndex, _uint iIndex)
{
	auto& iter = m_pLayers[iLevelIndex];
	auto& begin = iter.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++begin;

	return begin->first;
}

size_t CGameObjectManager::Get_LayerSize(_uint iIndex)
{
	return m_pLayers[iIndex].size();
}

size_t CGameObjectManager::Get_LayerObjectSize(_uint iLevelIndex, _uint iIndex)
{
	auto& iter = m_pLayers[iLevelIndex];
	auto& begin = iter.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++begin;

	return begin->second->Get_ListSize();
}

list<class CGameObject*>* CGameObjectManager::Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag)
{
	auto& iter = m_pLayers[iLevelIndex];
	auto& list = find_if(iter.begin(), iter.end(), CTagFinder(pLayerTag));
	if (list == iter.end())
		return nullptr;

	return list->second->Get_ObjectList();
}

void CGameObjectManager::Set_Pause(_bool bPause)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (FAILED(Pair.second->Set_Pause(bPause)))
				return;
		}
	}
}

void CGameObjectManager::Set_MiniGamePause(_bool bPause)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (FAILED(Pair.second->Set_MiniGamePause(bPause)))
				return;
		}
	}
}

_int CGameObjectManager::Tick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (Pair.second->Tick(dTimeDelta) < 0)
				return -1;
		}
	}

	return _int();
}

_int CGameObjectManager::LateTick(_double dTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (Pair.second->LateTick(dTimeDelta) < 0)
				return -1;
		}
	}

	return _int();
}

HRESULT CGameObjectManager::CollisionTick(_double dTimeDelta)
{
	for (auto& pCollisionTag : m_listCollisionTag)
	{
		auto& First = find_if(m_pLayers[pCollisionTag->iFirstLevel].begin()
			, m_pLayers[pCollisionTag->iFirstLevel].end()
			, CTagFinder(pCollisionTag->pFirstTag));
		if (First == m_pLayers[pCollisionTag->iFirstLevel].end())
			continue;

		auto& Second = find_if(m_pLayers[pCollisionTag->iSecondLevel].begin()
			, m_pLayers[pCollisionTag->iSecondLevel].end()
			, CTagFinder(pCollisionTag->pSecondTag));
		if (Second == m_pLayers[pCollisionTag->iSecondLevel].end())
			continue;


		CCollision::Check_LayerCollision(First->second, pCollisionTag->iFirstType, Second->second, pCollisionTag->iSecondType, dTimeDelta);
	}
	return S_OK;
}

_int CGameObjectManager::Collect_Event()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Collect_Event();
	}
	return 0;
}

void CGameObjectManager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

HRESULT CGameObjectManager::Clear_CollisionTag()
{
	for (auto& pCollisionTag : m_listCollisionTag)
	{
		Safe_Delete(pCollisionTag);
	}
	m_listCollisionTag.clear();

	return S_OK;
}

CGameObject * CGameObjectManager::Get_GameObject(_uint iLevelIndex, const _tchar * pLayerTag, _uint iIndex)
{
   	if (m_iNumLevels <= iLevelIndex)
		NULLMSG("LevelIndex is too big - ObjectManager Get GameObject");

	auto&	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		NULLMSG("LayerTag is wrong - ObjectManager Get GameObject");

	return iter->second->Get_GameObjectPtr(iIndex);
}

list<class CGameObject*>* CGameObjectManager::Get_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto& iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second->Get_GameObjects();
}

void CGameObjectManager::Free()
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		if (m_pLayers == nullptr)
			continue;
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}
	for (auto& Pair : m_mapPrototype)
		Safe_Release(Pair.second);
	m_mapPrototype.clear();

	Safe_Delete_Array(m_pLayers);
	Clear_CollisionTag();
}
