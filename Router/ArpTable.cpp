#include "stdafx.h"
#include "ArpTable.h"
#include "Router.h"


ArpTable::ArpTable(void)
{
}


ArpTable::~ArpTable(void)
{
}


void ArpTable::InsertDefaults(void)
{
	ArpEntry DefEntry;
	int i;
	CRouterDlg *router_dlg = theApp.GetRouterDlg();
			
	DefEntry.ip.b[3] = 224;
	DefEntry.ip.b[2] = 0;
	DefEntry.ip.b[1] = 0;
	DefEntry.ip.b[0] = 9;
	DefEntry.mac.b[0] = 0x01;
	DefEntry.mac.b[1] = 0x00;
	DefEntry.mac.b[2] = 0x5e;
	DefEntry.mac.b[3] = 0x00;
	DefEntry.mac.b[4] = 0x00;
	DefEntry.mac.b[5] = 0x09;
	DefEntry.i = NULL;
	TableEntry.Add(DefEntry);
	router_dlg->InsertArp(0,DefEntry);

	for (i=0;i < 4;i++) DefEntry.ip.b[i] = 255;
	for (i=0;i < 6;i++) DefEntry.mac.b[i] = 0xFF;
	TableEntry.Add(DefEntry);
	router_dlg->InsertArp(1,DefEntry);
}


int ArpTable::GetMAC(IPaddr ip, Interface *iface, MACaddr *result)
{
	int i;

	m_cs_arp.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (CompareIP(ip,TableEntry[i].ip) == 0)
		{
			*result = TableEntry[i].mac;
			m_cs_arp.Unlock();
			return 0;
		}
	}
	m_cs_arp.Unlock();
	SendArpRequest(ip,iface);
	
	return 1;
}


void ArpTable::SendArpRequest(IPaddr dest_ip, Interface *iface)
{
	Frame arp_frame;

	arp_frame.GenerateArpRequest(iface->GetMACAddrStruct(),iface->GetIPAddrStruct(),dest_ip);
	iface->SendFrame(&arp_frame);
}


void ArpTable::ReplyToRequest(Frame * buffer, Interface *iface)
{
	ArpEntry newEntry;
	int i;
	IPaddr ArpTargetIP = buffer->GetArpTargetIP();
	Interface *found_if;
	CSingleLock lock(&m_cs_arp);
	
	newEntry.ip = buffer->GetArpSenderIP();
	newEntry.mac = buffer->GetArpSenderMAC();
	newEntry.i = iface;

	if ((iface->IsLocalIP(ArpTargetIP))
		|| (((found_if = theApp.GetRIB()->DoLookup(ArpTargetIP)) != NULL) && (found_if != iface))
		|| (theApp.GetNAT()->IsGlobalIP(ArpTargetIP)))
	{
		buffer->GenerateArpReply(iface->GetMACAddrStruct(),ArpTargetIP);
		iface->SendFrame(buffer);
	}
		
	if (iface->IsInLocalNetwork(newEntry.ip))
	{
		lock.Lock();
		for (i=0;i < TableEntry.GetCount();i++) if (CompareIP(newEntry.ip,TableEntry[i].ip) == 0) return;

		TableEntry.InsertAt(0,newEntry);
		theApp.GetRouterDlg()->InsertArp(0,newEntry);
	}
}


void ArpTable::ProcessArpReply(Frame * buffer, Interface *iface)
{
	ArpEntry newEntry;
	int i;
	CSingleLock lock(&m_cs_arp);

	if (!iface->IsLocalIP(buffer->GetArpTargetIP())) return;
	
	newEntry.ip = buffer->GetArpSenderIP();
	newEntry.mac = buffer->GetArpSenderMAC();
	newEntry.i = iface;

	lock.Lock();
	for (i=0;i < TableEntry.GetCount();i++) if (CompareIP(newEntry.ip,TableEntry[i].ip) == 0) return;

	TableEntry.InsertAt(0,newEntry);
	theApp.GetRouterDlg()->InsertArp(0,newEntry);
}


void ArpTable::AddInterfaceBroadcast(Interface *iface)
{
	ArpEntry DefEntry;
	int i;

	DefEntry.ip = iface->GetBroadcastIPAddrStruct();
	for (i=0;i < 6;i++) DefEntry.mac.b[i] = 0xFF;
	DefEntry.i = iface;

	m_cs_arp.Lock();
	TableEntry.InsertAt(0,DefEntry);
	theApp.GetRouterDlg()->InsertArp(0,DefEntry);
	m_cs_arp.Unlock();
}


void ArpTable::RemoveAll(void)
{
	int i;

	m_cs_arp.Lock();
	for (i=0;i < TableEntry.GetCount();) if ((TableEntry[i].i != NULL) && (!theApp.IsBroadcast(TableEntry[i].mac)))
	{
		TableEntry.RemoveAt(i);
		theApp.GetRouterDlg()->RemoveArp(i);
	}
	else i++;
	m_cs_arp.Unlock();
}


void ArpTable::RemoveOnInterface(Interface *iface)
{
	int i;

	m_cs_arp.Lock();
	for (i=0;i < TableEntry.GetCount();) if (TableEntry[i].i == iface)
	{
		TableEntry.RemoveAt(i);
		theApp.GetRouterDlg()->RemoveArp(i);
	}
	else i++;
	m_cs_arp.Unlock();
}


int ArpTable::CompareIP(IPaddr& ip1, IPaddr& ip2)
{
	if (ip1.dw != ip2.dw) return 1;
	
	return 0;
}
