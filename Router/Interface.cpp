#include "stdafx.h"
#include "Interface.h"
#include "Router.h"


Interface::Interface(int index)
	: MACAddress(_T("unknown"))
	, IntIndex(index)
	, Enabled(FALSE)
	, IPaddrIsSet(FALSE)
	, handle(NULL)
{
	int i;

	NATmode = DISABLED;
	for (i=0;i < 6;i++) MACAddrStruct.b[i] = 0x00;
	for (i=0;i < 4;i++) IPAddrStruct.b[i] = 0x00;
	IPAddrStruct.SubnetMaskCIDR = 0;
	FrameBuffer = new Frame();
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
	BOOL result;

	m_cs_sw.Lock();
	result = Enabled;
	m_cs_sw.Unlock();
	
	return result;
}


void Interface::SetEnabled(BOOL bEnable)
{
	Frame RipMessage;
	
	m_cs_sw.Lock();
	if (Enabled != bEnable)
	{
		Enabled = bEnable;
		if (Enabled)
		{
			theApp.GetRIB()->AddDirectlyConnected(this);
			theApp.GetARPtable()->AddInterfaceBroadcast(this);

			if (theApp.GetRIB()->IsRipEnabled())
			{
				theApp.GetRIB()->ResetTime();
				RipMessage.GenerateRipRequestMessage(IPAddrStruct);
				SendFrame(&RipMessage);
			}

			StartReceive();
		}
		else
		{
			theApp.GetRIB()->RemoveDirectlyConnected(this);
			theApp.GetARPtable()->RemoveOnInterface(this);
			theApp.GetRIB()->ResetTime();
		}
	}
	m_cs_sw.Unlock();
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

	m_cs_ip.Lock();
	IPstr.Format(_T("%u.%u.%u.%u/%u"),IPAddrStruct.b[3],IPAddrStruct.b[2],IPAddrStruct.b[1],IPAddrStruct.b[0],IPAddrStruct.SubnetMaskCIDR);
	m_cs_ip.Unlock();

	return IPstr;
}


IPaddr Interface::GetIPAddrStruct(void)
{
	CSingleLock lock(&m_cs_ip);

	lock.Lock();
	return IPAddrStruct;
}


IPaddr Interface::GetBroadcastIPAddrStruct(void)
{
	IPaddr BcIP;
	DWORD wcMask = ~0;

	m_cs_ip.Lock();
	wcMask >>= IPAddrStruct.SubnetMaskCIDR;
	BcIP.dw = IPAddrStruct.dw | wcMask;
	BcIP.SubnetMaskCIDR = IPAddrStruct.SubnetMaskCIDR;
	m_cs_ip.Unlock();

	return BcIP;
}


int Interface::IsLocalIP(IPaddr& ip)
{
	CSingleLock lock(&m_cs_ip);

	lock.Lock();
	if (IPAddrStruct.dw == ip.dw) return 1;
		
	return 0;
}


int Interface::IsInLocalNetwork(IPaddr& ip)
{
	CSingleLock lock(&m_cs_ip);

	lock.Lock();
	if ((ip.dw >> (32 - PrefixStruct.SubnetMaskCIDR)) == (PrefixStruct.dw >> (32 - PrefixStruct.SubnetMaskCIDR))) return 1;

	return 0;
}


CString Interface::GetPrefixString(void)
{
	CString PrefixStr;

	m_cs_ip.Lock();
	PrefixStr.Format(_T("%u.%u.%u.%u/%u"),PrefixStruct.b[3],PrefixStruct.b[2],PrefixStruct.b[1],PrefixStruct.b[0],PrefixStruct.SubnetMaskCIDR);
	m_cs_ip.Unlock();

	return PrefixStr;
}


IPaddr Interface::GetPrefixStruct(void)
{
	CSingleLock lock(&m_cs_ip);
	
	lock.Lock();
	return PrefixStruct;
}


void Interface::SetPrefixStruct(void)
{
	PrefixStruct.dw = (IPAddrStruct.dw >> (32 - IPAddrStruct.SubnetMaskCIDR)) << (32 - IPAddrStruct.SubnetMaskCIDR);
	PrefixStruct.SubnetMaskCIDR = IPAddrStruct.SubnetMaskCIDR;
}


BOOL Interface::IsIPaddrConfigured(void)
{
	return IPaddrIsSet;
}


void Interface::SetIPAddress(BYTE b1, BYTE b2, BYTE b3, BYTE b4, BYTE cidr)
{
	m_cs_ip.Lock();
	IPAddrStruct.b[0] = b1;
	IPAddrStruct.b[1] = b2;
	IPAddrStruct.b[2] = b3;
	IPAddrStruct.b[3] = b4;
	IPAddrStruct.SubnetMaskCIDR = cidr;
	SetPrefixStruct();
	IpHeaderID = 0;
	m_cs_ip.Unlock();

	IPaddrIsSet = TRUE;
}


void Interface::SetIPAddress(IPaddr ip_addr)
{
	m_cs_ip.Lock();
	IPAddrStruct = ip_addr;
	SetPrefixStruct();
	IpHeaderID = 0;
	m_cs_ip.Unlock();

	IPaddrIsSet = TRUE;
}


BYTE Interface::GetMaskCIDR(void)
{
	CSingleLock lock(&m_cs_ip);

	lock.Lock();
	return IPAddrStruct.SubnetMaskCIDR;
}


NAT_MODE Interface::GetNATmode(void)
{
	CSingleLock lock(&m_cs_natmode);

	lock.Lock();
	return NATmode;
}


void Interface::SetNATmode(NAT_MODE mode)
{
	m_cs_natmode.Lock();
	NATmode = mode;
	m_cs_natmode.Unlock();
}


WORD Interface::GenerateIpHeaderID(void)
{
	WORD value;

	m_cs_ip.Lock();
	value = IpHeaderID;
	if (IpHeaderID == 0xFFFF) IpHeaderID = 0;
	else IpHeaderID++;
	m_cs_ip.Unlock();

	return value;
}


Frame * Interface::GetBuffer(void)
{
	return FrameBuffer;
}


int Interface::OpenAdapter(void)
{
	char errbuf[PCAP_ERRBUF_SIZE];
	CStringA errorstring;
	int flag = PCAP_OPENFLAG_PROMISCUOUS | PCAP_OPENFLAG_NOCAPTURE_LOCAL | PCAP_OPENFLAG_MAX_RESPONSIVENESS;

	m_cs_handle.Lock();
	if (!handle) handle = pcap_open(IntName,65536,flag,1000,NULL,errbuf);
	if (!handle)
	{
		errorstring.Format("Unable to open the adapter on INTERFACE %d!\r\n%s",IntIndex,errbuf);
		theApp.GetRouterDlg()->MessageBox(CString(errorstring),_T("Error"),MB_ICONERROR);
		return 0;
	}
	m_cs_handle.Unlock();

	return 1;
}


pcap_t * Interface::GetPcapHandle(void)
{
	return handle;
}


void Interface::StartReceive(void)
{
	if ((!handle) && (!OpenAdapter())) return;
	AfxBeginThread(Interface::ReceiveThread,this);
}


UINT Interface::ReceiveThread(void * pParam)
{
	Interface *iface = (Interface *) pParam;
	Frame *buffer = iface->GetBuffer();
	pcap_t *handle = iface->GetPcapHandle();
	CStringA errorstring;
	pcap_pkthdr *header = NULL;
	const u_char *frame = NULL;
	int retval = 0;

	retval = pcap_setbuff(handle,102400);
	if (retval == -1)
	{
		errorstring.Format("Error during the buffer allocation on INTERFACE %d!\r\n%s",iface->GetIndex(),pcap_geterr(handle));
		theApp.GetRouterDlg()->MessageBox(CString(errorstring),_T("Error"),MB_ICONERROR);
		return 0;
	}
	
	while ((iface->IsEnabled()) && ((retval = pcap_next_ex(handle,&header,&frame)) >= 0))
	{
		if (retval == 0) continue;
		buffer->AddFrame(header->len,frame);
	}
	if (retval == -1)
	{
		errorstring.Format("Error receiving the packets on INTERFACE %d!\r\n%s",iface->GetIndex(),pcap_geterr(handle));
		theApp.GetRouterDlg()->MessageBox(CString(errorstring),_T("Error"),MB_ICONERROR);
	}
			
	return 0;
}


int Interface::SendFrame(Frame * buffer, IPaddr *NextHop, BOOL UseARP)
{
	MACaddr req;
	int retval;
	
	if (!IsEnabled()) return 0;

	buffer->SetSrcMAC(MACAddrStruct);
	if ((UseARP) && (buffer->GetLay3Type() == 0x0800))
	{
		if (NextHop) retval = theApp.GetARPtable()->GetMAC(*NextHop,this,&req);
		else retval = theApp.GetARPtable()->GetMAC(buffer->GetDestIPaddr(),this,&req);
		if (!retval) buffer->SetDestMAC(req);
		else return 0;
	}
	
	m_cs_send.Lock();
	
	if ((!handle) && (!OpenAdapter()))
	{
		m_cs_send.Unlock();
		return 1;
	}

	if (pcap_sendpacket(handle,buffer->GetData(),buffer->GetLength()) != 0)
	{
		CStringA errorstring;
		errorstring.Format("Error sending the packets on INTERFACE %d!\r\n%s",IntIndex,pcap_geterr(handle));
		theApp.GetRouterDlg()->MessageBox(CString(errorstring),_T("Error"),MB_ICONERROR);
		return 1;
	}
	m_cs_send.Unlock();

	// create statistics for the outgoing frame
	theApp.GetStatistics()->Add(IntIndex,Out,buffer);
	
	return 0;
}
