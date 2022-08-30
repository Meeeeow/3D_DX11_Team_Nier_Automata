#include "..\Public\SubjectManager.h"

IMPLEMENT_SINGLETON(CSubjectManager)

CSubjectManager::CSubjectManager()
{
}

HRESULT CSubjectManager::Create_Subject(const _tchar * pSubjectTag)
{
	auto& iter = find_if(m_mapSubjects.begin(), m_mapSubjects.end(), CTagFinder(pSubjectTag));

	if (iter != m_mapSubjects.end())
	{
		return E_FAIL;
	}
	m_mapSubjects.emplace(pSubjectTag, CSubject::Create());
	
	return S_OK;
}

CSubject * CSubjectManager::Get_Subject(const _tchar * pSubjectTag)
{
	auto& iter = find_if(m_mapSubjects.begin(), m_mapSubjects.end(), CTagFinder(pSubjectTag));

	if (iter == m_mapSubjects.end())
	{
		return nullptr;
	}
	return iter->second;
}

HRESULT CSubjectManager::Delete_Subject(const _tchar * pSubjectTag)
{
	auto& iter = find_if(m_mapSubjects.begin(), m_mapSubjects.end(), CTagFinder(pSubjectTag));

	if (iter == m_mapSubjects.end())
	{
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_mapSubjects.erase(iter);

	return S_OK;
}

void CSubjectManager::Release_Subject()
{
	for (auto& pair : m_mapSubjects)
	{
		Safe_Release(pair.second);
	}
	m_mapSubjects.clear();
}

void CSubjectManager::Free()
{
	for (auto& pair : m_mapSubjects)
	{
		Safe_Release(pair.second);
	}
	m_mapSubjects.clear();
}
