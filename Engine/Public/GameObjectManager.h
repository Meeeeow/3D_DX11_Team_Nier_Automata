#pragma once
#ifndef __ENGINE_GAME_OBJECT_MANAGER_H__
#define __ENGINE_GAME_OBJECT_MANAGER_H__

#include "Layer.h"

BEGIN(Engine)
class CGameObjectManager final : public CBase
{
	DECLARE_SINGLETON(CGameObjectManager)
private:
	explicit CGameObjectManager();
	virtual ~CGameObjectManager() DEFAULT;

public:
	typedef unordered_map<const _tchar*, class CGameObject*>	PROTOTYPES;
	typedef unordered_map<const _tchar*, CLayer*>				LAYERS;

public:
	HRESULT						Reserve_Container(_uint iNumLevels);
	class CGameObject*			Clone_GameObject(const _tchar* pPrototypeTag, void* pArg);
	HRESULT						Add_GameObject(const _tchar* pPrototypeTag, class CGameObject* pGameObject);
	HRESULT						Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg);
	HRESULT						Add_GameObjectToLayer(_uint iLevelIndex, class CGameObject* pGameObject, const _tchar* pLayerTag);
	HRESULT						Add_GameObjectToLayerWithPtr(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, class CGameObject** ppOut, void* pArg);
	HRESULT						Add_CollisionTag(COLLISION_TAG* pCollisionTag);
	_bool						IsFind_Prototype(const _tchar* pPrototypeTag);

public:
	const _tchar*				Get_LayerName(_uint iLevelIndex,_uint iIndex);
	size_t						Get_LayerSize(_uint iIndex);
	size_t						Get_LayerObjectSize(_uint iLevelIndex, _uint iIndex);
	list<class CGameObject*>*	Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);

public:
	void						Set_Pause(_bool bPause);
	void						Set_MiniGamePause(_bool bPause);

public:
	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	HRESULT						CollisionTick(_double dTimeDelta);
	_int						Collect_Event();

	void						Clear(_uint iLevelIndex);
	HRESULT						Clear_CollisionTag();

public:
	class CGameObject*			Get_GameObject(_uint iLevelIndex, const _tchar* pLayerTag, _uint iIndex);
	list<class CGameObject*>*	Get_Layer(_uint iLevelIndex, const _tchar* pLayerTag);

private:
	PROTOTYPES					m_mapPrototype;
	LAYERS*						m_pLayers = nullptr;
	list<COLLISION_TAG*>		m_listCollisionTag;
	_uint						m_iNumLevels = 0;
public:
	virtual void				Free() override;

};
END

#endif // !__ENGINE_GAME_OBJECT_MANAGER_H__