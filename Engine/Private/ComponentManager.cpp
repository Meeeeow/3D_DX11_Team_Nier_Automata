#include "..\Public\ComponentManager.h"
#include "GraphicDebugger.h"
IMPLEMENT_SINGLETON(CComponentManager)

CComponentManager::CComponentManager()
{
}

HRESULT CComponentManager::Reserve_Container(_uint iNumLevels)
{
	if (m_pComponents != nullptr)
		return E_FAIL;

	m_pComponents = new COMPONENTS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponentManager::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pComponent)
{
	if (iLevelIndex >= m_iNumLevels || pComponent == nullptr)
		return E_FAIL;

	if (m_pComponents[iLevelIndex].end() != find_if(m_pComponents[iLevelIndex].begin(), m_pComponents[iLevelIndex].end(), CTagFinder(pPrototypeTag)))
		return E_FAIL;

	m_pComponents[iLevelIndex].emplace(pPrototypeTag, pComponent);

#ifdef _DEBUG
	string strLevel = to_string(iLevelIndex);
	wstring wstr = pPrototypeTag;
	string strTag(wstr.begin(), wstr.end());
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	GRAPHICDEBUGGER()->Notice("Level ", false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKBLUE);
	GRAPHICDEBUGGER()->Notice(strLevel.c_str(), false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	GRAPHICDEBUGGER()->Notice(" In ", false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKPURPLE);
	GRAPHICDEBUGGER()->Notice(strTag.c_str(), false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
	GRAPHICDEBUGGER()->Notice(" Create.");
#endif // _DEBUG


	return S_OK;
}

CComponent * CComponentManager::Clone_Component(_uint iLevelIndex, const _tchar * pComponentTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto iter = find_if(m_pComponents[iLevelIndex].begin(), m_pComponents[iLevelIndex].end(), CTagFinder(pComponentTag));
	if (iter == m_pComponents[iLevelIndex].end())
		return nullptr;

	CComponent*	pComponent = iter->second->Clone(pArg);
	if (pComponent == nullptr)
		return nullptr;

	return pComponent;
}

void CComponentManager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pComponents[iLevelIndex])
		Safe_Release(Pair.second);

	m_pComponents[iLevelIndex].clear();
}

void CComponentManager::Free()
{
	unsigned long dwRefCnt = 0;
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pComponents[i])
		{
			Safe_Release(Pair.second);
		}
		m_pComponents[i].clear();
	}
	Safe_Delete_Array(m_pComponents);
}
