#pragma once
class ProtocolDB
{
public:
	static ProtocolDB * GetInstance(void);
private:
	ProtocolDB(void);
	~ProtocolDB(void);
	static ProtocolDB *m_instance;
	static CCriticalSection m_cs_singleton;
	CCriticalSection m_cs_file_eth2;
	CCriticalSection m_cs_file_ip;
	CCriticalSection m_cs_file_ports;
	FILE *f_eth2;
	FILE *f_ip;
	FILE *f_ports;
	CArray<CStringA> Eth2ProtocolList;
	CArray<CStringA> IPProtocolList;
	CArray<CStringA> TCPAppList;
	CArray<CStringA> UDPAppList;
public:
	CString CheckTextFiles(void);
	CString GetEth2ProtocolName(WORD type);
	WORD GetEth2ProtocolNum(CStringA Name);
	void CreateEth2ProtocolList(void);
	int FindInEth2ProtocolList(WORD key);
	CArray<CStringA> & GetEth2ProtocolList(void);
	CString GetIPProtocolName(BYTE type);
	BYTE GetIPProtocolNum(CStringA Name);
	void CreateIPProtocolList(void);
	int FindInIPProtocolList(BYTE key);
	CArray<CStringA> & GetIPProtocolList(void);
	WORD GetPortNumber(CStringA AppName);
	CString GetAppName(WORD port, int isExtended = 0);
	void CreateTCPAppList(void);
	int FindInTCPAppList(WORD port);
	void CreateUDPAppList(void);
	int FindInUDPAppList(WORD port);
	CArray<CStringA> & GetTCPAppList(void);
	CArray<CStringA> & GetUDPAppList(void);
};

