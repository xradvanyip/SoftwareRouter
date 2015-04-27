#include "stdafx.h"
#include "RoutingTable.h"
#include "Router.h"


RoutingTable::RoutingTable(void)
	: Timer()
	, RipEnabled(FALSE)
	, Time(0)
	, RipUpdateInterval(30)
	, RipInvalidInterval(180)
	, RipFlushInterval(240)
	, RipHoldDownInterval(180)
{
	SetTimeout(5000);
}


RoutingTable::~RoutingTable(void)
{
}


void RoutingTable::AddDirectlyConnected(Interface * i)
{
	Route newRoute;
	Frame RipMessage;
	Interface *rip_out;
	
	newRoute.type = CONNECTED;
	newRoute.prefix = i->GetPrefixStruct();
	newRoute.AD = 0;
	newRoute.metric = 0;
	newRoute.NextHop.HasNextHop = FALSE;
	newRoute.i = i;

	m_cs_table.Lock();
	if (TableEntry.IsEmpty())
	{
		TableEntry.Add(newRoute);
		theApp.GetRouterDlg()->InsertRoute(0,newRoute);
	}
	
	else if ((TableEntry[0].type != CONNECTED)
		|| ((TableEntry[0].prefix.dw == newRoute.prefix.dw) && (TableEntry[0].prefix.SubnetMaskCIDR < newRoute.prefix.SubnetMaskCIDR))
		|| ((TableEntry[0].prefix.dw != newRoute.prefix.dw) && (i->GetIndex() < TableEntry[0].i->GetIndex())))
	{
		TableEntry.InsertAt(0,newRoute);
		theApp.GetRouterDlg()->InsertRoute(0,newRoute);
	}
	
	else
	{
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
	m_cs_table.Unlock();

	if (!IsRipEnabled()) return;
	
	if (i->GetIndex() == 1) rip_out = theApp.GetInt2();
	else rip_out = theApp.GetInt1();

	if (rip_out->IsEnabled())
	{
		RipMessage.AddRipRoute(newRoute.prefix,0);
		RipMessage.GenerateRipResponseMessage(rip_out->GetIPAddrStruct());
		rip_out->SendFrame(&RipMessage);
	}
}


void RoutingTable::RemoveDirectlyConnected(Interface * i)
{
	int j;
	Frame RipMessage;
	Interface *rip_out;
	
	m_cs_table.Lock();
	for (j=0;j < TableEntry.GetCount();j++)
		if (((TableEntry[j].type == CONNECTED) && (TableEntry[j].i == i))
			|| ((TableEntry[j].type == RIP) && (TableEntry[j].update_src == i)))
		{
			RipMessage.AddRipRoute(TableEntry[j].prefix,16);
			TableEntry.RemoveAt(j);
			theApp.GetRouterDlg()->RemoveRoute(j--);
		}
	m_cs_table.Unlock();

	if (!IsRipEnabled()) return;

	if (i->GetIndex() == 1) rip_out = theApp.GetInt2();
	else rip_out = theApp.GetInt1();

	if (rip_out->IsEnabled())
	{
		RipMessage.GenerateRipResponseMessage(rip_out->GetIPAddrStruct());
		rip_out->SendFrame(&RipMessage);
	}
}


int RoutingTable::MatchPrefix(IPaddr& address, IPaddr& prefix)
{
	if ((address.dw >> (32 - prefix.SubnetMaskCIDR)) == (prefix.dw >> (32 - prefix.SubnetMaskCIDR))) return 1;
	
	return 0;
}


int RoutingTable::Compare(Route& r1, Route& r2)
{
	if ((r1.AD == r2.AD) && (r1.prefix.dw == r2.prefix.dw)
		&& (r1.prefix.SubnetMaskCIDR == r2.prefix.SubnetMaskCIDR) && (r1.type == r2.type)
		&& ((r1.type != RIP) || (r1.update_src == r2.update_src))) return 1;
	return 0;
}


int RoutingTable::IsDefaultRoute(Route& r)
{
	if ((r.prefix.dw == 0) && (r.prefix.SubnetMaskCIDR == 0)) return 1;
	return 0;
}


Interface * RoutingTable::DoLookup(IPaddr& address, IPaddr **NextHop)
{
	int i;
	Interface *found_int;

	for (i=0;i < TableEntry.GetCount();i++)
		if (MatchPrefix(address,TableEntry[i].prefix))
		{
			if (TableEntry[i].NextHop.HasNextHop)
			{
				if (NextHop) *NextHop = &TableEntry[i].NextHop;
				found_int = DoLookup(TableEntry[i].NextHop, NextHop);
				if (TableEntry[i].i != found_int)
				{
					theApp.GetRouterDlg()->RemoveRoute(i);
					TableEntry[i].i = found_int;
					theApp.GetRouterDlg()->InsertRoute(i,TableEntry[i]);
				}
				return found_int;
			}
			return TableEntry[i].i;
		}
	
	return NULL;
}


int RoutingTable::AddRoute(Route r)
{
	CSingleLock lock(&m_cs_table);

	return AddRouteUnsafe(r);
}


int RoutingTable::AddRouteUnsafe(Route r)
{
	int i;
	int count;
		
	count = TableEntry.GetCount();
	if (TableEntry.IsEmpty())
	{
		TableEntry.Add(r);
		theApp.GetRouterDlg()->InsertRoute(0,r);
		return 0;
	}

	for (i=0;i < count;i++)
	{
		if (Compare(TableEntry[i],r))
		{
			if ((TableEntry[i].type == RIP) && (TableEntry[i].IsHoldDown)) return 1;
			TableEntry[i].timer = 0;
			if (TableEntry[i].metric != r.metric)
			{
				TableEntry.RemoveAt(i);
				theApp.GetRouterDlg()->RemoveRoute(i);
				return AddRoute(r);
			}
			return 1;
		}
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
	}
	else
	{
		TableEntry.InsertAt(i,r);
		theApp.GetRouterDlg()->InsertRoute(i,r);
	}

	return 0;
}


int RoutingTable::RemoveStaticRoute(int index)
{
	CSingleLock lock(&m_cs_table);
	
	lock.Lock();
	if (TableEntry[index].type != STATIC) return 1;

	TableEntry.RemoveAt(index);
	theApp.GetRouterDlg()->RemoveRoute(index);
	return 0;
}


int RoutingTable::RemoveRouteUnsafe(int index)
{	
	if (index < TableEntry.GetCount())
	{
		TableEntry.RemoveAt(index);
		theApp.GetRouterDlg()->RemoveRoute(index);
		return 0;
	}

	return 1;
}


int RoutingTable::RemoveRipRoute(IPaddr prefix, Interface *src)
{
	int i;

	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if ((TableEntry[i].type == RIP) && (TableEntry[i].prefix.dw == prefix.dw)
			&& (TableEntry[i].prefix.SubnetMaskCIDR == prefix.SubnetMaskCIDR) && (TableEntry[i].update_src == src))
		{
			TableEntry.RemoveAt(i);
			theApp.GetRouterDlg()->RemoveRoute(i);
			m_cs_table.Unlock();
			return 0;
		}
	}
	m_cs_table.Unlock();

	return 1;
}


void RoutingTable::RemoveAllRipRoutes(void)
{
	int i;
	
	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (TableEntry[i].type == RIP) RemoveRouteUnsafe(i--);
	}
	m_cs_table.Unlock();
}


void RoutingTable::ChangeToInvalidUnsafe(int index)
{
	Route tmp;

	tmp.AD = TableEntry[index].AD;
	tmp.i = TableEntry[index].i;
	tmp.IsHoldDown = TableEntry[index].IsHoldDown;
	tmp.metric = 16;
	tmp.NextHop = TableEntry[index].NextHop;
	tmp.prefix = TableEntry[index].prefix;
	tmp.timer = TableEntry[index].timer;
	tmp.type = TableEntry[index].type;
	tmp.update_src = TableEntry[index].update_src;

	RemoveRouteUnsafe(index);
	AddRouteUnsafe(tmp);
}


void RoutingTable::Timeout()
{
	int i;
	BOOL DefaultStaticAdded = FALSE;
	Frame RipMessage[2];
	Interface *int1 = theApp.GetInt1();
	Interface *int2 = theApp.GetInt2();

	m_cs_time.Lock();
	m_cs_table.Lock();

	Time += 5;
	theApp.GetRouterDlg()->SetRipUpdateStatus(RipUpdateInterval - Time);
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (TableEntry[i].type == RIP)
		{
			TableEntry[i].timer += 5;
			if ((TableEntry[i].timer >= RipInvalidInterval) && (TableEntry[i].metric != 16)) ChangeToInvalidUnsafe(i);
			if ((RipHoldDownInterval) && (TableEntry[i].timer >= RipInvalidInterval)
				&& (TableEntry[i].timer <= (RipInvalidInterval + RipHoldDownInterval))) TableEntry[i].IsHoldDown = TRUE;
			else TableEntry[i].IsHoldDown = FALSE;
			if (Time == RipUpdateInterval) RipMessage[TableEntry[i].update_src->GetIndex() - 1].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
			if (TableEntry[i].timer >= RipFlushInterval) RemoveRouteUnsafe(i--);
		}
		else if (Time == RipUpdateInterval)
			if (TableEntry[i].type == CONNECTED)
			{
				RipMessage[TableEntry[i].i->GetIndex() - 1].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
			}
			else if ((!DefaultStaticAdded) && (TableEntry[i].type == STATIC) && (IsDefaultRoute(TableEntry[i])))
			{
				RipMessage[0].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
				RipMessage[1].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
				DefaultStaticAdded = TRUE;
			}
	}
	if (Time == RipUpdateInterval) Time = 0;
	theApp.GetRouterDlg()->SetRipUpdateStatus(RipUpdateInterval - Time);

	m_cs_table.Unlock();
	m_cs_time.Unlock();

	if ((RipMessage[0].GetRipRouteCount()) && (int2->IsEnabled()))
	{
		RipMessage[0].GenerateRipResponseMessage(int2->GetIPAddrStruct());
		int2->SendFrame(&RipMessage[0]);
	}
	if ((RipMessage[1].GetRipRouteCount()) && (int1->IsEnabled()))
	{
		RipMessage[1].GenerateRipResponseMessage(int1->GetIPAddrStruct());
		int1->SendFrame(&RipMessage[1]);
	}
}


void RoutingTable::ResetTime(void)
{
	m_cs_time.Lock();
	Time = 0;
	m_cs_time.Unlock();
}


void RoutingTable::GetRipTimeIntervals(UINT& update, UINT& invalid, UINT& flush, UINT& holddown)
{
	update = RipUpdateInterval;
	invalid = RipInvalidInterval;
	flush = RipFlushInterval;
	holddown = RipHoldDownInterval;
}


void RoutingTable::SetRipTimeIntervals(UINT update, UINT invalid, UINT flush, UINT holddown)
{
	m_cs_time.Lock();
	RipUpdateInterval = update;
	RipInvalidInterval = invalid;
	RipFlushInterval = flush;
	RipHoldDownInterval = holddown;
	m_cs_time.Unlock();
}


BOOL RoutingTable::IsRipEnabled(void)
{
	BOOL result;

	m_cs_rip_sw.Lock();
	result = RipEnabled;
	m_cs_rip_sw.Unlock();
	
	return result;
}


void RoutingTable::SetRipEnabled(BOOL bEnable)
{
	m_cs_rip_sw.Lock();
	RipEnabled = bEnable;
	m_cs_rip_sw.Unlock();
}


UINT RoutingTable::StartRipProcess(void * pParam)
{
	RoutingTable *rib = theApp.GetRIB();
	
	rib->ResetTime();
	rib->SetRipEnabled(TRUE);
	rib->SendRipInitMessages();
	theApp.GetRouterDlg()->SetRipUpdateStatus(theApp.GetRIB()->RipUpdateInterval);
	rib->StartTimer();
		
	return 0;
}


UINT RoutingTable::StopRipProcess(void * pParam)
{
	RoutingTable *rib = theApp.GetRIB();
	
	rib->StopTimer();
	rib->SetRipEnabled(FALSE);
	theApp.GetRouterDlg()->SetRipUpdateStatus(-1);
	rib->RemoveAllRipRoutes();

	return 0;
}


void RoutingTable::SendRipInitMessages(void)
{
	int i;
	BOOL DefaultStaticAdded = FALSE;
	Frame RipMessage[2];
	Interface *int1 = theApp.GetInt1();
	Interface *int2 = theApp.GetInt2();

	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
		if (TableEntry[i].type == CONNECTED)
		{
			RipMessage[TableEntry[i].i->GetIndex() - 1].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
		}
		else if ((!DefaultStaticAdded) && (TableEntry[i].type == STATIC) && (IsDefaultRoute(TableEntry[i])))
		{
			RipMessage[0].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
			RipMessage[1].AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
			DefaultStaticAdded = TRUE;
		}
	m_cs_table.Unlock();

	if (int1->IsEnabled())
	{
		if (RipMessage[1].GetRipRouteCount())
		{
			RipMessage[1].GenerateRipResponseMessage(int1->GetIPAddrStruct());
			int1->SendFrame(&RipMessage[1]);
		}
		RipMessage[1].GenerateRipRequestMessage(int1->GetIPAddrStruct());
		int1->SendFrame(&RipMessage[1]);
	}
	if (int2->IsEnabled())
	{
		if (RipMessage[0].GetRipRouteCount())
		{
			RipMessage[0].GenerateRipResponseMessage(int2->GetIPAddrStruct());
			int2->SendFrame(&RipMessage[0]);
		}
		RipMessage[0].GenerateRipRequestMessage(int2->GetIPAddrStruct());
		int2->SendFrame(&RipMessage[0]);
	}
}


void RoutingTable::ProcessRipRequest(Frame * buffer, Interface * src_if)
{
	int i;
	BOOL DefaultStaticAdded = FALSE;
	Frame RipMessage;

	if (!IsRipEnabled()) return;

	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (((TableEntry[i].type == RIP) && (TableEntry[i].update_src != src_if))
			|| ((TableEntry[i].type == CONNECTED) && (TableEntry[i].i != src_if))
			|| ((!DefaultStaticAdded) && (TableEntry[i].type == STATIC) && (IsDefaultRoute(TableEntry[i]))))
			RipMessage.AddRipRoute(TableEntry[i].prefix,TableEntry[i].metric);
		if (TableEntry[i].type == STATIC) DefaultStaticAdded = TRUE;
	}
	m_cs_table.Unlock();

	if ((src_if->IsEnabled()) && (RipMessage.GetRipRouteCount()))
	{
		RipMessage.GenerateRipResponseMessage(src_if->GetIPAddrStruct(),&buffer->GetSrcIPaddr());
		RipMessage.SetDestMAC(buffer->GetSrcMAC());
		src_if->SendFrame(&RipMessage,NULL,FALSE);
	}
}


void RoutingTable::ProcessRipResponse(Frame * buffer, Interface * src_if)
{
	int i, retval;
	Route newRoute;
	Frame RipMessage;
	Interface *rip_out;

	if (!IsRipEnabled()) return;
	CArray<RipResponseEntry> &received = buffer->ReadRipRoutesFromPacket();

	for (i=0;i < received.GetCount();i++)
	{
		if (received[i].metric == 16)
		{
			retval = RemoveRipRoute(received[i].prefix,src_if);
			if (!retval) RipMessage.AddRipRoute(received[i].prefix,16);
			continue;
		}
		
		newRoute.AD = 120;
		newRoute.i = NULL;
		newRoute.IsHoldDown = 0;
		newRoute.metric = received[i].metric;
		newRoute.NextHop = buffer->GetSrcIPaddr();
		newRoute.prefix = received[i].prefix;
		newRoute.timer = 0;
		newRoute.type = RIP;
		newRoute.update_src = src_if;

		retval = AddRoute(newRoute);
		if (!retval) RipMessage.AddRipRoute(received[i].prefix,received[i].metric);
	}
	buffer->Clear();

	if (!RipMessage.GetRipRouteCount()) return;

	if (src_if->GetIndex() == 1) rip_out = theApp.GetInt2();
	else rip_out = theApp.GetInt1();

	if (rip_out->IsEnabled())
	{
		RipMessage.GenerateRipResponseMessage(rip_out->GetIPAddrStruct());
		rip_out->SendFrame(&RipMessage);
	}
}
