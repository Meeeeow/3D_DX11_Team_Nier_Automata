#include "..\Public\Timer.h"



CTimer::CTimer()
{

}

HRESULT CTimer::Ready_Timer()
{
	QueryPerformanceFrequency(&m_tCpuTick);
	QueryPerformanceCounter(&m_tBegin);
	QueryPerformanceCounter(&m_tEnd);
	QueryPerformanceCounter(&m_tTick);
	return S_OK;
}

_double CTimer::Compute_Timer()
{
	QueryPerformanceCounter(&m_tEnd);

	if (m_tEnd.QuadPart - m_tTick.QuadPart >= m_tCpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_tCpuTick);
		m_tTick = m_tEnd;
	}

	m_dTimeDelta = _double(m_tEnd.QuadPart - m_tBegin.QuadPart) / m_tCpuTick.QuadPart;

	m_tBegin = m_tEnd;

	return m_dTimeDelta;
}

CTimer * CTimer::Create()
{
	CTimer* pInstance = new CTimer();
	if (FAILED(pInstance->Ready_Timer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTimer::Free()
{

}


