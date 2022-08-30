#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END


BEGIN(Client)
class CObjectPool :
	public CBase
{
	DECLARE_SINGLETON(CObjectPool)

public:
	HRESULT NativeConstruct();


private:
	unordered_map<const _tchar*, vector<CGameObject*>>	m_mapObjectPool;

public:
	HRESULT		Add_ObjectPool(const _tchar* _szProto
							 , LEVEL _eLevel
							 , const _tchar* _szLayer
							 , _int _iNumber
							 , void* pArg = nullptr);
	HRESULT		Clear_ObjectPool();

	vector<CGameObject*> Get_GameObjectVector(const _tchar* szName);

private:
	explicit CObjectPool();
public:
	virtual	void Free() override;
};
END