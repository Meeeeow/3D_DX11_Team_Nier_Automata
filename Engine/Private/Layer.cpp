#include "..\Public\Layer.h"
#include "GraphicDebugger.h"

CLayer::CLayer()
{
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (pGameObject == nullptr)
		return E_FAIL;

	m_listObjects.emplace_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_double dTimeDelta)
{
	auto& iter = m_listObjects.begin();
	auto& iterEnd = m_listObjects.end();

	while (iter != iterEnd)
	{
		OBJSTATE eResult = (OBJSTATE)(*iter)->Tick(dTimeDelta);
		if (eResult == OBJSTATE::DEAD)
		{
#ifdef _DEBUG
			string  strLayer(m_wstrDebugLayer.begin(), m_wstrDebugLayer.end());
			CGraphicDebugger::Get_Instance()->Notice(strLayer.c_str(), CGraphicDebugger::COLOR::DARKGREEN, " Object ", CGraphicDebugger::COLOR::WHITE, "Delete.", CGraphicDebugger::COLOR::WHITE);
#endif // _DEBUG
			m_listDeadObjects.push_back(*iter);
			iter = m_listObjects.erase(iter);
		}
		else
			++iter;
	}
	return 0;
}

_int CLayer::LateTick(_double dTimeDelta)
{
	for (auto& GameObject : m_listObjects)
	{
		if (nullptr != GameObject)
		{
			if (0 > GameObject->LateTick(dTimeDelta))
				return -1;
		}
	}

	return _int();
}

_int CLayer::Collect_Event()
{
	for (auto& GameObject : m_listObjects)
	{
		if (nullptr != GameObject)
		{
			if (0 > GameObject->Collect_Event())
				return -1;
		}
	}
	return _int();
}

CGameObject * CLayer::Get_GameObjectPtr(_uint iIndex)
{
	if (iIndex >= m_listObjects.size())
		return nullptr;

	auto	iter = m_listObjects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return *iter;
}

list<class CGameObject*>* CLayer::Get_ObjectList()
{
	if (m_listObjects.empty())
		return nullptr;

	return &m_listObjects;
}

HRESULT CLayer::Set_Pause(_bool bPause)
{
	for (auto& GameObject : m_listObjects)
	{
		if (nullptr != GameObject)
			GameObject->Set_Pause(bPause);
	}

	return S_OK;
}

HRESULT CLayer::Set_MiniGamePause(_bool bPause)
{
	for (auto& GameObject : m_listObjects)
	{
		if (nullptr != GameObject)
			GameObject->Set_MiniGamePause(bPause);
	}

	return S_OK;
}

#ifdef _DEBUG
HRESULT CLayer::Set_LayerName(wstring wstr)
{
	m_wstrDebugLayer = wstr;
	return S_OK;
}
#endif // _DEBUG

CLayer * CLayer::Create()
{
	CLayer* pInstance = new CLayer();
	if (FAILED(pInstance->NativeConstruct()))
		Safe_Release(pInstance);

	return pInstance;
}

void CLayer::Free()
{
	for (auto& GameObject : m_listObjects)
	{
#ifdef _DEBUG
		string  strLayer(m_wstrDebugLayer.begin(), m_wstrDebugLayer.end());
		CGraphicDebugger::Get_Instance()->Notice(strLayer.c_str(), CGraphicDebugger::COLOR::DARKGREEN, " Object ", CGraphicDebugger::COLOR::WHITE, "Delete.", CGraphicDebugger::COLOR::WHITE);
#endif // _DEBUG
		Safe_Release(GameObject);
	}
	m_listObjects.clear();

	for (auto& DeadObject : m_listDeadObjects)
		Safe_Release(DeadObject);
	m_listDeadObjects.clear();
}

