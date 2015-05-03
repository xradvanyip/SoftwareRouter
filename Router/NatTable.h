#pragma once

#include "Timer.h"
#include "Interface.h"
#include "afxtempl.h"

enum NAT_TYPE {STATICNAT, DYNAMICNAT, PAT};

struct Translation {
	BYTE Lay4Prot;
	NAT_TYPE type;
	NAT_MODE mode;
	IPaddr global;
	IPaddr local;
	union {
		WORD global_port;
		WORD global_icmp_id;
	};
	union {
		WORD local_port;
		WORD local_icmp_id;
	};
	BOOL IsPortForward;
	UINT timeout;
};

class NatTable : private Timer
{
public:
	NatTable(void);
	~NatTable(void);
private:
	CArray<Translation> TableEntry;
	CCriticalSection m_cs_table;
	CCriticalSection m_cs_pool;
	CCriticalSection m_cs_nat_sw;
	CCriticalSection m_cs_time;
	CCriticalSection m_cs_icmpid;
	BOOL IsPoolSet;
	IPaddr PoolStart, PoolEnd;
	CMap<DWORD,DWORD,BOOL,BOOL> Reservation;
	CMap<WORD,WORD,BOOL,BOOL> IcmpIdReservation;
	BOOL DynNatEnabled, PatEnabled;
public:
	int Translate(Frame * buffer, Interface * src_if);
	void AddRule(Translation newRule);
	int RemoveStaticRule(int index);
	int IsGlobalIP(IPaddr checkIP);
	int IsPoolConfigured(void);
	IPaddr GetFirstIPinPool(void);
	IPaddr GetLastIPinPool(void);
	void SetPool(IPaddr first, IPaddr last);
	void ResetRuleTimeout(int index);
	virtual void Timeout();
	void RemoveAllDynamic(void);
	int IsDynNatEnabled(void);
	int IsPatEnabled(void);
	void SetDynNatEnabled(int bEnabled = TRUE);
	void SetPatEnabled(int bEnabled = TRUE);
	int CreateDynamicEntry(Frame * buffer);
	int CreateOverload(Frame * buffer, Interface * src_if);
};

