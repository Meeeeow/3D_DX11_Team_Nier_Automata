#pragma once
#ifndef __ENGINE_SUBJECT_H__
#define __ENGINE_SUBJECT_H__

#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CSubject final : public CBase
{
public:
	typedef list<class CObserver*>			OBSERVERS;

private:
	explicit CSubject();
	virtual ~CSubject() DEFAULT;

public:
	HRESULT						Add_Observer(class CObserver* pObserver);
	HRESULT						Add_Observer(const _tchar* pObserverTag);
	HRESULT						Delete_Observer(class CObserver* pObserver);
	HRESULT						Delete_Observer(const _tchar* pObserverTag);

	void						Notify(void* pMessage = nullptr);

private:
	OBSERVERS					m_listObservers;

public:
	static	CSubject*			Create();
	virtual	void				Free() override;
};
END
#endif // !__ENGINE_SUBJECT_H__