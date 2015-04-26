#include "stdafx.h"
#include "Timer.h"


Timer::Timer(void)
{
	m_bStop = true;
	m_msTimeout = -1;
	m_hThreadDone = NULL;
	m_hThreadDone = CreateEvent(NULL,FALSE, FALSE, NULL);
	ASSERT(m_hThreadDone);
	SetEvent(m_hThreadDone);
}


Timer::~Timer(void)
{
	//dont destruct until the thread is done
	DWORD ret = WaitForSingleObject(m_hThreadDone,INFINITE);
	ASSERT(ret == WAIT_OBJECT_0);
	Sleep(500);
}


void Timer::Timeout()
{
}


void Timer::StartTimer(void)
{
	if (m_bStop == false) return;
	m_bStop = false;
	ResetEvent(m_hThreadDone);
	AfxBeginThread(TickerThread, this);
}


UINT Timer::TickerThread(LPVOID pParam)
{
	Timer* me = (Timer*) pParam;
	ASSERT(me->m_msTimeout!=-1);
	while (!me->m_bStop)
	{
		Sleep(me->GetTimeout());
		me->Timeout();
	}
	SetEvent(me->m_hThreadDone);
	return 0;
}


void Timer::StopTimer(void)
{
	if (m_bStop == true) return;
	m_bStop = true;
	WaitForSingleObject(m_hThreadDone,INFINITE);
}


int Timer::GetTimeout(void)
{
	return m_msTimeout;
}


void Timer::SetTimeout(int t)
{
	m_msTimeout = t;
}
