#pragma once

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
	BOOL IPaddrIsSet;
	NAT_MODE NATmode;
	Frame *FrameBuffer;
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
	CString GetPrefixString(void);
	IPaddr GetPrefixStruct(void);
	BOOL IsIPaddrConfigured(void);
	void SetIPAddress(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE cidr);
	void SetIPAddress(IPaddr ip_addr);
	BYTE GetMaskCIDR(void);
	void SetNATmode(NAT_MODE mode);
	NAT_MODE GetNATmode(void);
	Frame * GetBuffer(void);
};

