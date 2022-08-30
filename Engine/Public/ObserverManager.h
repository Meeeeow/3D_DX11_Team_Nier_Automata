#pragma once
#ifndef __ENGINE_OBSERVER_MANAGER_H__
#define __ENGINE_OBSERVER_MANAGER_H__

#include "Observer.h"

BEGIN(Engine)
class ENGINE_DLL CObserverManager final : public CBase
{
	DECLARE_SINGLETON(CObserverManager)

private:
	explicit CObserverManager();
	virtual ~CObserverManager() DEFAULT;

public:
	typedef unordered_map<const _tchar*, CObserver*>		OBSERVERS;

public:
	HRESULT			Create_Observer(const _tchar* pObserverTag, class CGameObject* pTarget);
	CObserver*		Get_Observer(const _tchar* pObserverTag);
	HRESULT			Delete_Observer(const _tchar* pObserverTag);
	void			Release_Observer();

private:
	OBSERVERS		m_mapObservers;

public:
	virtual void	Free();

};

END
#endif // !__ENGINE_OBSERVER_MANAGER_H__