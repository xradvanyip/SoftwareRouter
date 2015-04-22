#pragma once

#include "Interface.h"

struct ArpEntry {
	IPaddr ip;
	MACaddr mac;
	Interface *i;
};

class ArpTable
{
public:
	ArpTable(void);
	~ArpTable(void);
private:
	CArray<ArpEntry> TableEntry;
	CCriticalSection m_cs_arp;
public:
	void InsertDefaults(void);
	int GetMAC(IPaddr ip, Interface *iface, MACaddr *result);
	void SendArpRequest(IPaddr dest_ip, Interface *iface);
	void ReplyToRequest(Frame * buffer, Interface *iface);
	void ProcessArpReply(Frame * buffer, Interface *iface);
	void AddInterfaceBroadcast(Interface *iface);
	void RemoveAll(void);
	void RemoveOnInterface(Interface *iface);
	int CompareIP(IPaddr& ip1, IPaddr& ip2);
};

