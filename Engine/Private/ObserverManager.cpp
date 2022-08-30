#include "..\Public\ObserverManager.h"

IMPLEMENT_SINGLETON(CObserverManager)

CObserverManager::CObserverManager()
{
}

HRESULT CObserverManager::Create_Observer(const _tchar * pObserverTag, CGameObject* pTarget)
{
	auto& iter = find_if(m_mapObservers.begin(), m_mapObservers.end(), CTagFinder(pObserverTag));

	if (iter != m_mapObservers.end())
	{
		return E_FAIL;
	}
	m_mapObservers.emplace(pObserverTag, CObserver::Create(pTarget));

	return S_OK;
}

CObserver * CObserverManager::Get_Observer(const _tchar * pObserverTag)
{
	auto& iter = find_if(m_mapObservers.begin(), m_mapObservers.end(), CTagFinder(pObserverTag));

	if (iter == m_mapObservers.end())
	{
		return nullptr;
	}
	return iter->second;
}

HRESULT CObserverManager::Delete_Observer(const _tchar * pObserverTag)
{
	auto& iter = find_if(m_mapObservers.begin(), m_mapObservers.end(), CTagFinder(pObserverTag));

	if (iter == m_mapObservers.end())
	{
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapObservers.erase(iter);

	return S_OK;
}

void CObserverManager::Release_Observer()
{
	for (auto& pair : m_mapObservers)
	{
		Safe_Release(pair.second);
	}
	m_mapObservers.clear();
}

void CObserverManager::Free()
{
	for (auto& pair : m_mapObservers)
	{
		Safe_Release(pair.second);
	}
	m_mapObservers.clear();
}
