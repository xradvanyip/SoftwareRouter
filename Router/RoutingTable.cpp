#include "stdafx.h"
#include "RoutingTable.h"
#include "Router.h"


RoutingTable::RoutingTable(void)
{
}


RoutingTable::~RoutingTable(void)
{
}


void RoutingTable::AddDirectlyConnected(Interface * i)
{
	Route newRoute;
	
	newRoute.type = CONNECTED;
	newRoute.prefix = i->GetPrefixStruct();
	newRoute.AD = 0;
	newRoute.metric = 0;
	newRoute.NextHop.HasNextHop = FALSE;
	newRoute.i = i;

	if (TableEntry.IsEmpty())
	{
		TableEntry.Add(newRoute);
		theApp.GetRouterDlg()->InsertRoute(0,newRoute);
		return;
	}
	
	if ((TableEntry[0].type != CONNECTED)
		|| ((TableEntry[0].prefix.dw == newRoute.prefix.dw) && (TableEntry[0].prefix.SubnetMaskCIDR < newRoute.prefix.SubnetMaskCIDR))
		|| ((TableEntry[0].prefix.dw != newRoute.prefix.dw) && (i->GetIndex() < TableEntry[0].i->GetIndex())))
	{
		TableEntry.InsertAt(0,newRoute);
		theApp.GetRouterDlg()->InsertRoute(0,newRoute);
		return;
	}
	
	if (TableEntry.GetCount() == 1)
	{
		TableEntry.Add(newRoute);
	}
	else
	{
		TableEntry.InsertAt(1,newRoute);
	}
	theApp.GetRouterDlg()->InsertRoute(1,newRoute);
}


void RoutingTable::RemoveDirectlyConnected(Interface * i)
{
	int j;
	
	for (j=0;j < TableEntry.GetCount();j++)
		if ((TableEntry[j].type == CONNECTED) && (TableEntry[j].i->GetIndex() == i->GetIndex()))
		{
			TableEntry.RemoveAt(j);
			theApp.GetRouterDlg()->RemoveRoute(j);
			break;
		}
}


int RoutingTable::MatchPrefix(IPaddr& address, IPaddr& prefix)
{
	if ((address.dw >> (32 - prefix.SubnetMaskCIDR)) == (prefix.dw >> (32 - prefix.SubnetMaskCIDR))) return 1;
	
	return 0;
}


int RoutingTable::Compare(Route& r1, Route& r2)
{
	if ((r1.AD == r2.AD) && (r1.metric == r2.metric)
		&& (r1.prefix.dw == r2.prefix.dw) && (r1.prefix.SubnetMaskCIDR == r2.prefix.SubnetMaskCIDR)
		&& (r1.type == r2.type)) return 1;
	return 0;
}


int RoutingTable::IsDefaultRoute(Route& r)
{
	if ((r.prefix.dw == 0) && (r.prefix.SubnetMaskCIDR == 0)) return 1;
	return 0;
}


Interface * RoutingTable::FindInterface(IPaddr& address)
{
	int i;

	for (i=0;i < TableEntry.GetCount();i++)
		if (MatchPrefix(address,TableEntry[i].prefix)) return TableEntry[i].i;
	
	return NULL;
}


void RoutingTable::AddRoute(Route r)
{
	int i;
	int count = TableEntry.GetCount();
	
	//if (r.NextHop.HasNextHop) r.i = FindInterface(r.NextHop);
	
	if (TableEntry.IsEmpty())
	{
		TableEntry.Add(r);
		theApp.GetRouterDlg()->InsertRoute(0,r);
		return;
	}

	for (i=0;i < count;i++)
	{
		if (Compare(TableEntry[i],r)) return;
		if (IsDefaultRoute(r))
		{
			if (IsDefaultRoute(TableEntry[i]))
			{
				if (TableEntry[i].AD > r.AD) break;
				if ((TableEntry[i].AD == r.AD) && (TableEntry[i].metric > r.metric)) break;
			}
		}
		else
		{
			if (TableEntry[i].AD > r.AD) break;
			if ((TableEntry[i].AD == r.AD) && (TableEntry[i].metric > r.metric)) break;
			if ((TableEntry[i].AD == r.AD)
				&& (TableEntry[i].metric == r.metric)
				&& (TableEntry[i].prefix.dw == r.prefix.dw)
				&& (TableEntry[i].prefix.SubnetMaskCIDR < r.prefix.SubnetMaskCIDR)) break;
			if (IsDefaultRoute(TableEntry[i])) break;
		}
	}

	if (i == count)
	{
		TableEntry.Add(r);
		theApp.GetRouterDlg()->InsertRoute(count,r);
		return;
	}
	else
	{
		TableEntry.InsertAt(i,r);
		theApp.GetRouterDlg()->InsertRoute(i,r);
		return;
	}
}


int RoutingTable::RemoveStaticRoute(int index)
{
	if (TableEntry[index].type != STATIC) return 1;

	TableEntry.RemoveAt(index);
	theApp.GetRouterDlg()->RemoveRoute(index);
	return 0;
}