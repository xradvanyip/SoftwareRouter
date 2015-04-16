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
	void AddDirectlyConnected(Interface * i);
	void RemoveDirectlyConnected(Interface * i);
	int MatchPrefix(IPaddr& address, IPaddr& prefix);
	int Compare(Route& r1, Route& r2);
	int IsDefaultRoute(Route& r);
	void AddRoute(Route r);
	int RemoveStaticRoute(int index);
private:
	Interface * FindInterface(IPaddr& address);
};

