#pragma once
#ifndef __ENGINE_OBSERVER_H__
#define __ENGINE_OBSERVER_H__

#include "Base.h"
#include "Subject.h"

BEGIN(Engine)
class ENGINE_DLL CObserver final : public CBase
{
private:
	explicit CObserver(class CGameObject* pTarget);
	virtual ~CObserver() DEFAULT;

	

public:
	HRESULT						Subscribe(class CSubject* pSubject);
	HRESULT						Subscribe(const _tchar* pSubjectTag);
	HRESULT						UnSubscribe(class CSubject* pSubject);
	HRESULT						UnSubscribe(const _tchar* pSubjectTag);

//private:
	void						Notify(void* pMessage = nullptr);

	//friend class CSubject;
	//friend void			CSubject::Notify(void* pMessage);

private:
	class CGameObject*			m_pOwner = nullptr;

public:
	static	CObserver*			Create(class CGameObject* pTarget);
	virtual void				Free() override;
};
END
#endif // !__ENGINE_OBSERVER_H__