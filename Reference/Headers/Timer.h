#pragma once
#ifndef __ENGINE_TIMER_H__
#define __ENGINE_TIMER_H__

#include "Base.h"
BEGIN(Engine)
class CTimer final : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer() DEFAULT;

public:
	HRESULT					Ready_Timer();
	_double					Compute_Timer();

private:
	LARGE_INTEGER			m_tBegin;
	LARGE_INTEGER			m_tEnd;
	LARGE_INTEGER			m_tTick;
	LARGE_INTEGER			m_tCpuTick;

	_double					m_dTimeDelta = 0.0;

public:
	static	CTimer*			Create();
	virtual	void			Free() override;
};
END
#endif // !__ENGINE_TIMER_H__