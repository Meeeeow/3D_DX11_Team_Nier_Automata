#pragma once
#ifndef __ENGINE_LAYER_H__
#define __ENGINE_LAYER_H__

#include "GameObject.h"

BEGIN(Engine)
class CLayer final : public CBase
{
private:
	explicit CLayer();
	virtual ~CLayer() DEFAULT;

public:
	typedef list<class CGameObject*>				OBJECTS;

public:
	HRESULT						NativeConstruct();

	HRESULT						Add_GameObject(class CGameObject* pGameObject);

	_int						Tick(_double dTimeDelta);
	_int						LateTick(_double dTimeDelta);
	_int						Collect_Event();

public:
	class CGameObject*			Get_GameObjectPtr(_uint iIndex);
	OBJECTS*					Get_GameObjects() { return &m_listObjects; }
	size_t						Get_ListSize() { return m_listObjects.size(); }
	list<class CGameObject*>*	Get_ObjectList();

public:
	HRESULT						Set_Pause(_bool bPause);
	HRESULT						Set_MiniGamePause(_bool bPause);
#ifdef _DEBUG
	HRESULT						Set_LayerName(wstring wstr);
	wstring						m_wstrDebugLayer;
#endif // _DEBUG
	

private:
	OBJECTS						m_listObjects;
	OBJECTS						m_listDeadObjects;


public:
	static	CLayer*				Create();
	virtual void				Free() override;

};
END
#endif // !__ENGINE_LAYER_H__