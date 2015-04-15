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


int RoutingTable::ComparePrefixes(IPaddr& prefix1, IPaddr& prefix2)
{
	if ((prefix1.dw >> (32 - prefix1.SubnetMaskCIDR)) == (prefix2.dw >> (32 - prefix2.SubnetMaskCIDR))) return 1;
	
	return 0;
}
