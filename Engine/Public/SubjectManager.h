#pragma once
#ifndef __ENGINE_SUBJECT_MANAGER_H__
#define __ENGINE_SUBJECT_MANAGER_H__

#include "Subject.h"

BEGIN(Engine)
class ENGINE_DLL CSubjectManager final : public CBase
{
	DECLARE_SINGLETON(CSubjectManager)

private:
	explicit CSubjectManager();
	virtual ~CSubjectManager() DEFAULT;

public:
	typedef unordered_map<const _tchar*, CSubject*>		SUBJECTS;

public:
	HRESULT			Create_Subject(const _tchar* pSubjectTag);
	CSubject*		Get_Subject(const _tchar* pSubjectTag);
	HRESULT			Delete_Subject(const _tchar* pSubjectTag);
	void			Release_Subject();

private:
	SUBJECTS		m_mapSubjects;

public:
	virtual void	Free();
};

END
#endif // !__ENGINE_SUBJECT_MANAGER_H__