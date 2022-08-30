#include "..\Public\Subject.h"
#include "ObserverManager.h"
#include "Observer.h"

CSubject::CSubject()
{

}

HRESULT CSubject::Add_Observer(CObserver * pObserver)
{
	auto& iter = find(m_listObservers.begin(), m_listObservers.end(), pObserver);
	if (iter == m_listObservers.end())
	{
		m_listObservers.push_back(pObserver);
		Safe_AddRef(pObserver);
	}
	else
	{
		return S_OK;
	}
	return S_OK;
}

HRESULT CSubject::Add_Observer(const _tchar * pObserverTag)
{
	CObserver* pObserver = CObserverManager::Get_Instance()->Get_Observer(pObserverTag);

	auto& iter = find(m_listObservers.begin(), m_listObservers.end(), pObserver);

	if (iter == m_listObservers.end())
	{
		m_listObservers.push_back(pObserver);
		Safe_AddRef(pObserver);
	}
	else
	{
		return S_OK;
	}
	return S_OK;
}

HRESULT CSubject::Delete_Observer(CObserver * pObserver)
{
	auto& iter = find(m_listObservers.begin(), m_listObservers.end(), pObserver);
	if (iter == m_listObservers.end())
	{
		Safe_Release(pObserver);
		m_listObservers.erase(iter);
	}
	else
	{
		return S_OK;
	}
	return S_OK;
}

HRESULT CSubject::Delete_Observer(const _tchar * pObserverTag)
{
	CObserver* pObserver = CObserverManager::Get_Instance()->Get_Observer(pObserverTag);

	auto& iter = find(m_listObservers.begin(), m_listObservers.end(), pObserver);

	if (iter == m_listObservers.end())
	{
		Safe_Release(pObserver);
		m_listObservers.erase(iter);
	}
	else
	{
		return S_OK;
	}
	return S_OK;
}

void CSubject::Notify(void * pMessage)
{
	for (auto& pObserver : m_listObservers)
	{
		pObserver->Notify(pMessage);
	}
}

CSubject * CSubject::Create()
{
	CSubject* pInstance = new CSubject;

	if (nullptr == pInstance)
	{
		MSGBOX("Failed To Creating CSubject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSubject::Free()
{
	for (auto& pObserver : m_listObservers)
	{
		Safe_Release(pObserver);
	}
	m_listObservers.clear();
}
