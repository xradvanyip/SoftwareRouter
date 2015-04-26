#pragma once

class Timer
{
public:
	Timer(void);
	~Timer(void);
	void StartTimer(void);
	void StopTimer(void);
	int GetTimeout(void);
	void SetTimeout(int t);
protected:
	int m_msTimeout;
	virtual void Timeout();
private:
	HANDLE m_hThreadDone;
	bool m_bStop;
	static UINT TickerThread(LPVOID pParam);
};

