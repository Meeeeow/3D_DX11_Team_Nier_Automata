#pragma once
#ifndef __ENGINE_BASE_H__
#define __ENGINE_BASE_H__

#include "Engine_Defines.h"

BEGIN(Engine)
class ENGINE_DLL CBase abstract
{
public:
	CBase();
	virtual ~CBase() DEFAULT;

public:
	virtual	unsigned	long	AddRef();
	virtual unsigned	long	Release();
	virtual unsigned	long	Count();
private:
	unsigned	long			m_dwRefCnt = 0;

public:
	virtual	void				Free() PURE;

};
END
#endif // !__ENGINE_BASE_H__