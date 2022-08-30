#include "..\Public\LightManager.h"
#include "Light.h"
#include "GraphicDebugger.h"

IMPLEMENT_SINGLETON(CLightManager)

CLightManager::CLightManager()
{
}

HRESULT CLightManager::Add_Light(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pLightTag, const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	if (pGraphicDevice == nullptr ||
		pContextDevice == nullptr)
		FAILMSG("LightManager Add_Light - Device is nullptr");

	CLight*	pInstance = CLight::Create(pGraphicDevice, pContextDevice, pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc);
	if (pInstance == nullptr)
		return E_FAIL;

	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
	{
		list<CLight*>	LightList;
		LightList.push_back(pInstance);

		m_mapLight.emplace(pLightTag, LightList);
	}
	else
		pLightList->push_back(pInstance);
#ifdef _DEBUG
	wstring wstr = pLightTag;
	string strTag(wstr.begin(), wstr.end());
	
	GRAPHICDEBUGGER()->Notice(strTag.c_str(), false);
	GRAPHICDEBUGGER()->Notice(" In Light; Count : ", false);
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::DARKYELLOW);
	if(pLightList != nullptr)
		GRAPHICDEBUGGER()->Notice(to_string(pLightList->size()).c_str());
	else
		GRAPHICDEBUGGER()->Notice("1");
	GRAPHICDEBUGGER()->ConsoleTextColor(CGraphicDebugger::COLOR::WHITE);
#endif // _DEBUG


	return S_OK;
}

HRESULT CLightManager::Add_Light_ForTool(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pLightTag, const _tchar * pShaderFilePath, const char * pTechniqueName, const LIGHTDESC & tLightDesc, const LIGHTDEPTHDESC& tDepthDesc)
{
	if (pGraphicDevice == nullptr ||
		pContextDevice == nullptr)
		FAILMSG("LightManager Add_Light - Device is nullptr");

	CLight*	pInstance = CLight::Create_ForTool(pGraphicDevice, pContextDevice, pShaderFilePath, pTechniqueName, tLightDesc, tDepthDesc);
	if (pInstance == nullptr)
		return E_FAIL;

	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
	{
		list<CLight*>	LightList;
		LightList.push_back(pInstance);

		m_mapLight.emplace(pLightTag, LightList);
	}
	else
		pLightList->push_back(pInstance);

	return S_OK;
}

HRESULT CLightManager::Add_Light_Prototype(_pGraphicDevice pGraphicDevice, _pContextDevice pContextDevice, const _tchar * pPrototypeLightTag, const _tchar * pShaderFilePath, const char * pTechniqueName)
{
	if (pGraphicDevice == nullptr || pContextDevice == nullptr)
		return E_FAIL;

	auto& iter = find_if(m_mapPrototypeLight.begin(), m_mapPrototypeLight.end(), CTagFinder(pPrototypeLightTag));
	if (iter != m_mapPrototypeLight.end())
		return E_FAIL;

	CLight* pInstance = CLight::Create(pGraphicDevice, pContextDevice, pShaderFilePath, pTechniqueName);
	if (pInstance == nullptr)
		return E_FAIL;

	m_mapPrototypeLight.emplace(pPrototypeLightTag, pInstance);

	return S_OK;
}

HRESULT CLightManager::Add_Light(const _tchar * pPrototypeLightTag, const _tchar * pLightTag, const LIGHTDESC & tLightDesc)
{
	CLight* pLight = Get_Light(pPrototypeLightTag, tLightDesc);
	if (pLight == nullptr)
		return E_FAIL;

	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
	{
		list<CLight*>	LightList;
		LightList.push_back(pLight);

		m_mapLight.emplace(pLightTag, LightList);
	}
	else
		pLightList->push_back(pLight);

	return S_OK;
}


_int CLightManager::Tick(_double dTimeDelta)
{
	for (auto& Pair : m_mapLight)
	{
		auto& begin = Pair.second.begin();
		auto& end = Pair.second.end();

		while (begin != end)
		{
			LIGHTSTATE eState = (LIGHTSTATE)(*begin)->Tick(dTimeDelta);

			if (LIGHTSTATE::DISABLE == eState)
			{
				m_listDeadLights.push_back(*begin);
				begin = Pair.second.erase(begin);
				//++begin;
			}
			else
				++begin;
		}
	}

	return 0;
}

_int CLightManager::LateTick(_double dTimeDelta)
{
	for (auto& Pair : m_mapLight)
	{
		auto& begin = Pair.second.begin();
		auto& end = Pair.second.end();

		while (begin != end)
		{
			(*begin)->LateTick(dTimeDelta);
			++begin;
		}
	}

	return 0;
}

HRESULT CLightManager::Render()
{
	for (auto& Pair : m_mapLight)
	{
		for (auto& pLight : Pair.second)
		{
			if (pLight != nullptr)
				pLight->Render();
		}
	}
	return S_OK;
}


void CLightManager::ResetPosition_Shadow_DirectionLight(const _tchar * pLightTag, _fvector vPosition, _uint iLightIndex)
{
	CLight* pLight = Get_Light(pLightTag, iLightIndex);
	if (nullptr == pLight)
		return;

	pLight->ResetPosition_Shadow_DirectionLight(vPosition);
}

void CLightManager::Trace_SpotLight(const _tchar * pLightTag, _fvector vTargetPosition)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Trace_SpotLight(vTargetPosition);
}

void CLightManager::Trace_SpotLight_EyeToAt(const _tchar * pLightTag, _fvector vEye, _fvector vAt)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
	{
		pLight->ResetPosition_SpotLight(vEye);
		pLight->Trace_SpotLight(vAt);
	}
}

CLight * CLightManager::Get_Light(const _tchar * pLightTag, _uint iIndex)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (iter == m_mapLight.end())
		return nullptr;

	auto& Light = iter->second.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++Light;

	return *Light;
}

CLight * CLightManager::Get_Light(const _tchar * pPrototypeLightTag, const LIGHTDESC& tLightDesc)
{
	auto& iter = find_if(m_mapPrototypeLight.begin(), m_mapPrototypeLight.end(), CTagFinder(pPrototypeLightTag));
	if (iter == m_mapPrototypeLight.end())
		return nullptr;

	return iter->second->Clone(tLightDesc);
}

size_t CLightManager::Get_LightMapSize()
{
	return m_mapLight.size();
}

const wchar_t * CLightManager::Get_LightTag(_uint iIndex)
{
	auto& iter = m_mapLight.begin();
	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return iter->first;
}

size_t CLightManager::Get_LightSize(const _tchar * pLightTag)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));

	return iter->second.size();
}

list<CLight*>* CLightManager::Find_LightMap(const _tchar * pLightTag)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));

	if (iter == m_mapLight.end())
		return nullptr;

	return &iter->second;
}

HRESULT CLightManager::LightMap_Active(const _tchar * pLightTag)
{
	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
		return E_FAIL;

	for (auto& pLight : *pLightList)
		pLight->Active();

	return S_OK;
}

HRESULT CLightManager::LightMap_DeActive(const _tchar * pLightTag)
{
	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
		return E_FAIL;

	for (auto& pLight : *pLightList)
		pLight->DeActive();

	return S_OK;
}

HRESULT CLightManager::Delete_LightList(const _tchar * pLightTag)
{
	auto map = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));

	auto& begin = map->second.begin();
	auto& end = map->second.end();

	for (; begin != end; ++begin)
	{
		Safe_Release(*begin);
		begin = map->second.erase(begin);
	}
	map->second.clear();

	return S_OK;
}

HRESULT CLightManager::Delete_LightIndex(const _tchar * pLightTag, _uint iLightIndex)
{
	auto map = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));

	auto& begin = map->second.begin();
	auto& end = map->second.end();

	for (_uint i = 0; i < iLightIndex; ++i)
		++begin;

	Safe_Release(*begin);
	begin = map->second.erase(begin);

	return S_OK;
}

HRESULT CLightManager::DeActive_All()
{
	for (auto& Pair : m_mapLight)
	{
		for (auto& pLight : Pair.second)
			pLight->DeActive();
	}

	return S_OK;
}

LIGHTDESC*  CLightManager::Get_LightDesc(const _tchar * pLightTag, _uint iLightIndex)
{
	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr || pLightList->size() <= 0)
		return nullptr;

	auto iter = pLightList->begin();
	for (_uint i = 0; i < iLightIndex; ++i)
		++iter;


	return (*iter)->Get_LightDesc();
}

LIGHTDEPTHDESC* CLightManager::Get_LightDepthDesc(const _tchar * pLightTag, _uint iLightIndex)
{
	list<CLight*>* pLightList = Find_LightMap(pLightTag);
	if (pLightList == nullptr)
	{
		return nullptr;
	}

	auto iter = pLightList->begin();
	for (_uint i = 0; i < iLightIndex; ++i)
		++iter;


	return (*iter)->Get_DepthDesc();
}

void CLightManager::Increase_LightColor(const _tchar* pLightTag, _float4 vIncreasePower, _float4 vMaxIncreasePower, _uint iColor)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Increase_LightColor(vIncreasePower, vMaxIncreasePower, (CLight::COLOR)iColor);
}

void CLightManager::Decrease_LightColor(const _tchar* pLightTag, _float4 vDecreasePower, _float4 vMinIncreasePower, _uint iColor)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Decrease_LightColor(vDecreasePower, vMinIncreasePower, (CLight::COLOR)iColor);
}

void CLightManager::Increase_InnerDegree(const _tchar * pLightTag, _float fIncreaseDegree, _float fMaxDegree)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Increase_InnerDegree(fIncreaseDegree, fMaxDegree);
}

void CLightManager::Decrease_InnerDegree(const _tchar * pLightTag, _float fDecreaseDegree)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Decrease_InnerDegree(fDecreaseDegree);
}

void CLightManager::Increase_OuterDegree(const _tchar * pLightTag, _float fIncreaseDegree, _float fMaxDegree)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Increase_OuterDegree(fIncreaseDegree, fMaxDegree);
}

void CLightManager::Decrease_OuterDegree(const _tchar * pLightTag, _float fDecreaseDegree)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Decrease_OuterDegree(fDecreaseDegree);
}

void CLightManager::Increase_Range(const _tchar * pLightTag, _float fIncreaseRange, _float fMaxRange)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Increase_Range(fIncreaseRange, fMaxRange);
}

void CLightManager::Decrease_Range(const _tchar * pLightTag, _float fDecreaseRange, _float fMinRange)
{
	auto& iter = find_if(m_mapLight.begin(), m_mapLight.end(), CTagFinder(pLightTag));
	if (m_mapLight.end() == iter)
		return;

	for (auto& pLight : iter->second)
		pLight->Decrease_Range(fDecreaseRange, fMinRange);
}

HRESULT CLightManager::Clear()
{
	for (auto& Pair : m_mapLight)
	{
		for (auto& pLight : Pair.second)
			Safe_Release(pLight);
		Pair.second.clear();
	}
	m_mapLight.clear();

	for (auto& DeadLight : m_listDeadLights)
		Safe_Release(DeadLight);

	m_listDeadLights.clear();

	return S_OK;
}

void CLightManager::SSAO(bool SSAO)
{
	m_bSSAO = SSAO;
}

const bool & CLightManager::Get_SSAO()
{
	// TODO: 여기에 반환 구문을 삽입합니다.
	return m_bSSAO;
}

void CLightManager::Free()
{
	for (auto& DeadLight : m_listDeadLights)
		Safe_Release(DeadLight);

	m_listDeadLights.clear();

	for (auto& Pair : m_mapLight)
	{
		for (auto& pLight : Pair.second)
			Safe_Release(pLight);
		Pair.second.clear();
	}
	
	for (auto& Pair : m_mapPrototypeLight)
	{
		Safe_Release(Pair.second);
	}

	m_mapPrototypeLight.clear();

	m_mapLight.clear();
}
