#include "stdafx.h"
#include "..\Public\ObjectPool.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CObjectPool)

CObjectPool::CObjectPool()
{
}

HRESULT CObjectPool::NativeConstruct()
{
	return S_OK;
}

HRESULT CObjectPool::Add_ObjectPool(const _tchar* _szProto, LEVEL _eLevel, const _tchar* _szLayer, _int _iNumber, void* pArg)
{
	CGameObject* pGameObj = nullptr;

	m_mapObjectPool.emplace(_szProto, vector<CGameObject*>());  
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	for (_int i = 0; i < _iNumber; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayerWithPtr(static_cast<_uint>(_eLevel), _szProto, _szLayer, &pGameObj, pArg)))
		{
			assert(false);
			return E_FAIL;
		}
		m_mapObjectPool[_szProto].emplace_back(pGameObj);
		//Safe_AddRef(pGameObj);
	}

	return S_OK;
}

HRESULT CObjectPool::Clear_ObjectPool()
{
	for (auto& pIter : m_mapObjectPool)
	{
		//for (auto& pVecIter : pIter.second)
		//{
		//	Safe_Release(pVecIter);
		//}
		pIter.second.clear();
	}
	m_mapObjectPool.clear();

	return S_OK;
}

vector<CGameObject*> CObjectPool::Get_GameObjectVector(const _tchar * szName)
{
	return m_mapObjectPool[szName];
}


void CObjectPool::Free()
{
	Clear_ObjectPool();
}