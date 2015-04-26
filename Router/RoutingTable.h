#pragma once

#include "Interface.h"
#include "Timer.h"
#include "afxtempl.h"

enum ROUTE_TYPE {CONNECTED, STATIC, RIP};

struct Route {
	ROUTE_TYPE type;
	IPaddr prefix;
	BYTE AD;
	BYTE metric;
	IPaddr NextHop;
	Interface *i;
	Interface *update_src;
	UINT timer;
	BOOL IsHoldDown;
};

class RoutingTable : private Timer
{
public:
	RoutingTable(void);
	~RoutingTable(void);
private:
	CArray<Route> TableEntry;
	BOOL RipEnabled;
	UINT Time;
	UINT RipInvalidInterval;
	UINT RipFlushInterval;
	UINT RipHoldDownInterval;
	CCriticalSection m_cs_rip_sw;
	CCriticalSection m_cs_time;
public:
	UINT RipUpdateInterval;
	CCriticalSection m_cs_table;
	void AddDirectlyConnected(Interface * i);
	void RemoveDirectlyConnected(Interface * i);
	Interface * DoLookup(IPaddr& address, IPaddr **NextHop = NULL);
	int MatchPrefix(IPaddr& address, IPaddr& prefix);
	int Compare(Route& r1, Route& r2);
	int IsDefaultRoute(Route& r);
	int AddRoute(Route r);
	int AddRouteUnsafe(Route r);
	int RemoveStaticRoute(int index);
	int RemoveRouteUnsafe(int index);
	int RemoveRipRoute(IPaddr prefix, Interface *src);
	void RemoveAllRipRoutes(void);
	void ChangeToInvalidUnsafe(int index);

	virtual void Timeout();
	void ResetTime(void);
	void GetRipTimeIntervals(UINT& update, UINT& invalid, UINT& flush, UINT& holddown);
	void SetRipTimeIntervals(UINT update, UINT invalid, UINT flush, UINT holddown);
	BOOL IsRipEnabled(void);
	void SetRipEnabled(BOOL bEnable = TRUE);
	static UINT StartRipProcess(void * pParam);
	static UINT StopRipProcess(void * pParam);
	void SendRipInitMessages(void);
	void ProcessRipRequest(Frame * buffer, Interface * src_if);
	void ProcessRipResponse(Frame * buffer, Interface * src_if);
};

