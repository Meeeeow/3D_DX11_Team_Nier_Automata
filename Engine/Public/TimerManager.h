#pragma once
#ifndef __ENGINE_TIMER_MANAGER_H__
#define __ENGINE_TIMER_MANAGER_H__

#include "Timer.h"

BEGIN(Engine)
class CTimerManager final : public CBase
{
	DECLARE_SINGLETON(CTimerManager)
private:
	explicit CTimerManager();
	virtual ~CTimerManager() DEFAULT;

public:
	typedef	unordered_map<const _tchar*, CTimer*>	TIMERS;

public:
	HRESULT						Add_Timer(const _tchar* pTimerTag);
	_double						Compute_Timer(const _tchar* pTimerTag);

private:
	CTimer*						Find_Timer(const _tchar* pTimerTag);

private:
	TIMERS						m_mapTimers;

public:
	virtual void				Free() override;

};
END

#endif // !__ENGINE_TIMER_MANAGER_H__