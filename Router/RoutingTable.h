#pragma once

#include "Interface.h"
#include "afxtempl.h"

enum ROUTE_TYPE {CONNECTED, STATIC, RIP};

struct Route {
	ROUTE_TYPE type;
	IPaddr prefix;
	BYTE AD;
	BYTE metric;
	IPaddr NextHop;
	Interface *i;
};

class RoutingTable
{
public:
	RoutingTable(void);
	~RoutingTable(void);
private:
	CArray<Route> TableEntry;
public:
	CCriticalSection m_cs_table;
	void AddDirectlyConnected(Interface * i);
	void RemoveDirectlyConnected(Interface * i);
	Interface * DoLookup(IPaddr& address, IPaddr **NextHop = NULL);
	int MatchPrefix(IPaddr& address, IPaddr& prefix);
	int Compare(Route& r1, Route& r2);
	int IsDefaultRoute(Route& r);
	void AddRoute(Route r);
	int RemoveStaticRoute(int index);
};

