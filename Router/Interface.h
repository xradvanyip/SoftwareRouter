#pragma once

#include <pcap.h>
#include "Frame.h"

enum NAT_MODE {INSIDE, OUTSIDE};

class Interface
{
public:
	Interface(int index);
	~Interface(void);
private:
	int IntIndex;
	CStringA IntName;
	CString IntDescription;
	BOOL Enabled;
	CString MACAddress;
	MACaddr MACAddrStruct;
	IPaddr IPAddrStruct;
	IPaddr PrefixStruct;
	BOOL IPaddrIsSet;
	NAT_MODE NATmode;
	WORD IpHeaderID;
	pcap_t *handle;
	Frame *FrameBuffer;
	CCriticalSection m_cs_sw;
	CCriticalSection m_cs_ip;
	CCriticalSection m_cs_handle;
	CCriticalSection m_cs_send;
	void SetPrefixStruct(void);
public:
	int GetIndex(void);
	CStringA GetName(void);
	CString GetDescription(void);
	void SetName(CStringA name);
	void SetName(PSTR name);
	void SetDescription(CString desc);
	void SetDescription(PSTR desc);
	void SetDescription(PWSTR desc);
	BOOL IsEnabled(void);
	void SetEnabled(BOOL bEnable = TRUE);
	CString GetMACAddress(void);
	MACaddr GetMACAddrStruct(void);
	void SetMACAddress(PBYTE mac_addr);
	CString GetIPAddrString(void);
	IPaddr GetIPAddrStruct(void);
	IPaddr GetBroadcastIPAddrStruct(void);
	int IsLocalIP(IPaddr& ip);
	int IsInLocalNetwork(IPaddr& ip);
	CString GetPrefixString(void);
	IPaddr GetPrefixStruct(void);
	BOOL IsIPaddrConfigured(void);
	void SetIPAddress(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE cidr);
	void SetIPAddress(IPaddr ip_addr);
	BYTE GetMaskCIDR(void);
	void SetNATmode(NAT_MODE mode);
	NAT_MODE GetNATmode(void);
	WORD GenerateIpHeaderID(void);
	Frame * GetBuffer(void);
	int OpenAdapter(void);
	pcap_t * GetPcapHandle(void);
	void StartReceive(void);
	static UINT ReceiveThread(void * pParam);
	int SendFrame(Frame * buffer, IPaddr *NextHop = NULL, BOOL UseARP = TRUE);
};

