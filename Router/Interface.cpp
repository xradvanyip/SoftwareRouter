#include "stdafx.h"
#include "Interface.h"


Interface::Interface(int index)
	: MACAddress(_T("unknown"))
	, IntIndex(index)
	, Enabled(FALSE)
	, IPaddrIsSet(FALSE)
{
	int i;

	NATmode = OUTSIDE;
	for (i=0;i < 6;i++) MACAddrStruct.b[i] = 0x00;
	for (i=0;i < 4;i++) IPAddrStruct.b[i] = 0x00;
	IPAddrStruct.SubnetMaskCIDR = 0;
}


Interface::~Interface(void)
{
}


int Interface::GetIndex(void)
{
	return IntIndex;
}


CStringA Interface::GetName(void)
{
	return IntName;
}


CString Interface::GetDescription(void)
{
	return IntDescription;
}


void Interface::SetName(CStringA name)
{
	IntName = name;
}


void Interface::SetName(PSTR name)
{
	IntName.SetString(name);
}


void Interface::SetDescription(CString desc)
{
	IntDescription = desc;
}


void Interface::SetDescription(PSTR desc)
{
	IntDescription = CString(desc);
}


void Interface::SetDescription(PWSTR desc)
{
	IntDescription.SetString(desc);
}


BOOL Interface::IsEnabled(void)
{
	return Enabled;
}


void Interface::SetEnabled(BOOL bEnable)
{
	Enabled = bEnable;
}


CString Interface::GetMACAddress(void)
{
	return MACAddress;
}


MACaddr Interface::GetMACAddrStruct(void)
{
	return MACAddrStruct;
}


void Interface::SetMACAddress(PBYTE mac_addr)
{
	int i;

	MACAddress.Empty();
	for (i=0;i < 6;i++) {
		if (i == 5) MACAddress.AppendFormat(_T("%.2X"),(int)mac_addr[i]);
		else MACAddress.AppendFormat(_T("%.2X-"),(int)mac_addr[i]);
		MACAddrStruct.b[i] = mac_addr[i];
	}
}


CString Interface::GetIPAddrString(void)
{
	CString IPstr;

	IPstr.Format(_T("%u.%u.%u.%u/%u"),IPAddrStruct.b[3],IPAddrStruct.b[2],IPAddrStruct.b[1],IPAddrStruct.b[0],IPAddrStruct.SubnetMaskCIDR);
	
	return IPstr;
}


IPaddr Interface::GetIPAddrStruct(void)
{
	return IPAddrStruct;
}


CString Interface::GetPrefixString(void)
{
	IPaddr prefix = GetPrefixStruct();
	CString PrefixStr;

	PrefixStr.Format(_T("%u.%u.%u.%u/%u"),prefix.b[3],prefix.b[2],prefix.b[1],prefix.b[0],prefix.SubnetMaskCIDR);

	return PrefixStr;
}


IPaddr Interface::GetPrefixStruct(void)
{
	IPaddr prefix;

	prefix.dw = (IPAddrStruct.dw >> (32 - IPAddrStruct.SubnetMaskCIDR)) << (32 - IPAddrStruct.SubnetMaskCIDR);
	prefix.SubnetMaskCIDR = IPAddrStruct.SubnetMaskCIDR;

	return prefix;
}


BOOL Interface::IsIPaddrConfigured(void)
{
	return IPaddrIsSet;
}


void Interface::SetIPAddress(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE cidr)
{
	IPAddrStruct.b[0] = b1;
	IPAddrStruct.b[1] = b2;
	IPAddrStruct.b[2] = b3;
	IPAddrStruct.b[3] = b4;
	IPAddrStruct.SubnetMaskCIDR = cidr;

	IPaddrIsSet = TRUE;
}


void Interface::SetIPAddress(IPaddr ip_addr)
{
	IPAddrStruct = ip_addr;

	IPaddrIsSet = TRUE;
}


BYTE Interface::GetMaskCIDR(void)
{
	return IPAddrStruct.SubnetMaskCIDR;
}


NAT_MODE Interface::GetNATmode(void)
{
	return NATmode;
}


void Interface::SetNATmode(NAT_MODE mode)
{
	NATmode = mode;
}
