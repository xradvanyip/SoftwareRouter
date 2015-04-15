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
	int ComparePrefixes(IPaddr& prefix1, IPaddr& prefix2);
};

