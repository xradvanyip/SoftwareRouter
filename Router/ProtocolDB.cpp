#include "stdafx.h"
#include "ProtocolDB.h"

ProtocolDB *ProtocolDB::m_instance = NULL;
CCriticalSection ProtocolDB::m_cs_singleton;

ProtocolDB::ProtocolDB(void)
	: f_eth2(NULL)
	, f_ip(NULL)
	, f_ports(NULL)
{
	f_eth2 = fopen("ethernet2_protocols.txt","r");
	f_ip = fopen("ip_protocols.txt","r");
	f_ports = fopen("ports.txt","r");

	if (f_eth2) CreateEth2ProtocolList();
	if (f_ip) CreateIPProtocolList();
	if (f_ports)
	{
		CreateTCPAppList();
		CreateUDPAppList();
	}
}


ProtocolDB::~ProtocolDB(void)
{
}


ProtocolDB * ProtocolDB::GetInstance(void)
{
	if (m_instance == NULL)
	{
		m_cs_singleton.Lock();
		if (m_instance == NULL) m_instance = new ProtocolDB();
		m_cs_singleton.Unlock();
	}

	return m_instance;
}


CString ProtocolDB::CheckTextFiles(void)
{
	CString error(_T("Can't open:"));

	if ((f_eth2) && (f_ip) && (f_ports)) return _T("");
	if (!f_eth2) error.AppendFormat(_T("\r\nethernet2_protocols.txt"));
	if (!f_ip) error.AppendFormat(_T("\r\nip_protocols.txt"));
	if (!f_ports) error.AppendFormat(_T("\r\nports.txt"));

	return error;
}


CString ProtocolDB::GetEth2ProtocolName(WORD type)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr,"%%[^\t]s%.4X",type);
	sprintf(numstr,"%.4X",type);
	m_cs_file_eth2.Lock();
	while (fgets(tmp,100,f_eth2) != NULL) if ((strstr(tmp,numstr) != NULL) && (sscanf(tmp,scanstr,namestr) > 0)) {
		found = 1;
		break;
	}
	rewind(f_eth2);
	m_cs_file_eth2.Unlock();
	if (found) name.Format("%s",namestr);
	else name.Format("0x%X",type);
	
	return CString(name);
}


WORD ProtocolDB::GetEth2ProtocolNum(CStringA Name)
{
	WORD num;
	char tmp[100], scanstr[50];
	
	sprintf(scanstr,"%s\t%%hX",Name);
	m_cs_file_eth2.Lock();
	while (fgets(tmp,100,f_eth2) != NULL) if ((strstr(tmp,Name) != NULL) && (sscanf(tmp,scanstr,&num) > 0)) break;
	rewind(f_eth2);
	m_cs_file_eth2.Unlock();
	
	return num;
}


void ProtocolDB::CreateEth2ProtocolList(void)
{
	char tmp[100], namestr[50];

	m_cs_file_eth2.Lock();
	while (fgets(tmp,100,f_eth2) != NULL) if (sscanf(tmp,"%[^\t]s",namestr) > 0) Eth2ProtocolList.Add(CStringA(namestr));
	rewind(f_eth2);
	m_cs_file_eth2.Unlock();
}


int ProtocolDB::FindInEth2ProtocolList(WORD key)
{
	int i;

	for (i=0;i < Eth2ProtocolList.GetCount();i++) if (Eth2ProtocolList[i].Compare(CStringA(GetEth2ProtocolName(key))) == 0) return i;
	return -1;
}


CArray<CStringA> & ProtocolDB::GetEth2ProtocolList(void)
{
	return Eth2ProtocolList;
}


CString ProtocolDB::GetIPProtocolName(BYTE type)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr,"%%[^\t]s%u",type);
	sprintf(numstr,"%u",type);
	m_cs_file_ip.Lock();
	while (fgets(tmp,100,f_ip) != NULL) if ((strstr(tmp,numstr) != NULL) && (sscanf(tmp,scanstr,namestr) > 0)) {
		found  = 1;
		break;
	}
	rewind(f_ip);
	m_cs_file_ip.Unlock();
	if (found) name.Format("%s",namestr);
	else name.Format("%u",type);
	
	return CString(name);
}


BYTE ProtocolDB::GetIPProtocolNum(CStringA Name)
{
	WORD num;
	char tmp[100], scanstr[50];
	
	sprintf(scanstr,"%s\t%%hu",Name);
	m_cs_file_ip.Lock();
	while (fgets(tmp,100,f_ip) != NULL) if ((strstr(tmp,Name) != NULL) && (sscanf(tmp,scanstr,&num) > 0)) break;
	rewind(f_ip);
	m_cs_file_ip.Unlock();
	
	return num;
}


void ProtocolDB::CreateIPProtocolList(void)
{
	char tmp[100], namestr[50];

	m_cs_file_ip.Lock();
	while (fgets(tmp,100,f_ip) != NULL) if (sscanf(tmp,"%[^\t]s",namestr) > 0) IPProtocolList.Add(CStringA(namestr));
	rewind(f_ip);
	m_cs_file_ip.Unlock();
}


int ProtocolDB::FindInIPProtocolList(BYTE key)
{
	int i;

	for (i=0;i < IPProtocolList.GetCount();i++) if (IPProtocolList[i].Compare(CStringA(GetIPProtocolName(key))) == 0) return i;
	return -1;
}


CArray<CStringA> & ProtocolDB::GetIPProtocolList(void)
{
	return IPProtocolList;
}


WORD ProtocolDB::GetPortNumber(CStringA AppName)
{
	WORD num = 0;
	char tmp[100], scanstr[50];
	
	sprintf(scanstr,"%s\t%%*3c\t%%hu",AppName);
	m_cs_file_ports.Lock();
	while (fgets(tmp,100,f_ports) != NULL) if ((strstr(tmp,AppName) != NULL) && (sscanf(tmp,scanstr,&num) > 0)) break;
	rewind(f_ports);
	m_cs_file_ports.Unlock();
	
	return num;
}


CString ProtocolDB::GetAppName(WORD port, int isExtended)
{
	char tmp[100], scanstr[50];
	char namestr[50], numstr[10];
	int found = 0;
	CStringA name;

	sprintf(scanstr,"%%[^\t]s%%*3c\t%u",port);
	sprintf(numstr,"%u",port);
	m_cs_file_ports.Lock();
	while (fgets(tmp,100,f_ports) != NULL) if ((strstr(tmp,numstr) != NULL) && (sscanf(tmp,scanstr,namestr) > 0)) {
		found = 1;
		break;
	}
	rewind(f_ports);
	m_cs_file_ports.Unlock();
	if ((isExtended) && (found)) name.Format("%s (%u)",namestr,port);
	else if (found) name.Format("%s",namestr);
	else name.Format("%u",port);
	
	return CString(name);
}


void ProtocolDB::CreateTCPAppList(void)
{
	char tmp[100], namestr[50];

	m_cs_file_ports.Lock();
	while (fgets(tmp,100,f_ports) != NULL) if ((strstr(tmp,"TCP") != NULL) && (sscanf(tmp,"%[^\t]s",namestr) > 0)) TCPAppList.Add(CStringA(namestr));
	rewind(f_ports);
	m_cs_file_ports.Unlock();
}


int ProtocolDB::FindInTCPAppList(WORD port)
{
	int i;

	for (i=0;i < TCPAppList.GetCount();i++) if (TCPAppList[i].Compare(CStringA(GetAppName(port))) == 0) return i;
	return -1;
}


void ProtocolDB::CreateUDPAppList(void)
{
	char tmp[100], namestr[50];

	m_cs_file_ports.Lock();
	while (fgets(tmp,100,f_ports) != NULL) if ((strstr(tmp,"UDP") != NULL) && (sscanf(tmp,"%[^\t]s",namestr) > 0)) UDPAppList.Add(CStringA(namestr));
	rewind(f_ports);
	m_cs_file_ports.Unlock();
}


int ProtocolDB::FindInUDPAppList(WORD port)
{
	int i;

	for (i=0;i < UDPAppList.GetCount();i++) if (UDPAppList[i].Compare(CStringA(GetAppName(port))) == 0) return i;
	return -1;
}


CArray<CStringA> & ProtocolDB::GetTCPAppList(void)
{
	return TCPAppList;
}


CArray<CStringA> & ProtocolDB::GetUDPAppList(void)
{
	return UDPAppList;
}
