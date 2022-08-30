#include "..\Public\Observer.h"
#include "GameObject.h"
#include "SubjectManager.h"
#include "Subject.h"

CObserver::CObserver(CGameObject* pTarget)
	: m_pOwner(pTarget)
{
	Safe_AddRef(m_pOwner);
}

HRESULT CObserver::Subscribe(CSubject * pSubject)
{
	if (nullptr == pSubject)
	{
		MSGBOX("Failed to Subscribe Subject");
		return E_FAIL;
	}
	if (nullptr != m_pOwner)
	{
		pSubject->Add_Observer(this);
	}
	return S_OK;
}

HRESULT CObserver::Subscribe(const _tchar * pSubjectTag)
{
	CSubject*	pSubject = CSubjectManager::Get_Instance()->Get_Subject(pSubjectTag);

	if (nullptr == pSubject)
	{
		MSGBOX("Failed to Subscribe Subject");
		return E_FAIL;
	}
	if (nullptr != m_pOwner)
	{
		pSubject->Add_Observer(this);
	}
	return S_OK;
}

HRESULT CObserver::UnSubscribe(CSubject * pSubject)
{
	if (nullptr == pSubject)
	{
		MSGBOX("Failed to UnSubscribe Subject");
		return E_FAIL;
	}
	pSubject->Delete_Observer(this);

	return S_OK;
}

HRESULT CObserver::UnSubscribe(const _tchar * pSubjectTag)
{
	CSubject*	pSubject = CSubjectManager::Get_Instance()->Get_Subject(pSubjectTag);

	if (nullptr == pSubject)
	{
		MSGBOX("Failed to UnSubscribe Subject");
		return E_FAIL;
	}
	pSubject->Delete_Observer(this);

	return S_OK;
}

void CObserver::Notify(void * pMessage)
{
	m_pOwner->Notify(pMessage);
}

CObserver * CObserver::Create(CGameObject* pTarget)
{
	CObserver* pInstance = new CObserver(pTarget);

	if (nullptr == pInstance)
	{
		MSGBOX("Failed To Creating CObserver");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObserver::Free()
{
	Safe_Release(m_pOwner);
}
