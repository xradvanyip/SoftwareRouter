#include "stdafx.h"
#include "NatTable.h"
#include "Router.h"

#define ICMP_ID 1
#define TCP_ID 6
#define UDP_ID 17


NatTable::NatTable(void)
	: IsPoolSet(FALSE)
	, DynNatEnabled(FALSE)
	, PatEnabled(FALSE)
{
	Reservation.InitHashTable(717);
	IcmpIdReservation.InitHashTable(717);
	SetTimeout(5000);
	StartTimer();
}


NatTable::~NatTable(void)
{
}


int NatTable::Translate(Frame * buffer, Interface * src_if)
{
	int i;
	NAT_MODE if_mode = src_if->GetNATmode();
	IPaddr srcIP, destIP;
	BYTE prot;
	WORD srcPort = 0, destPort = 0, icmpId = 0;
	BOOL InsideDone = FALSE, OutsideDone = FALSE;

	if (if_mode == DISABLED) return 0;

	srcIP = buffer->GetSrcIPaddr();
	destIP = buffer->GetDestIPaddr();
	if ((if_mode == INSIDE) && (src_if->IsInLocalNetwork(destIP))) return 0;
	prot = buffer->GetLay4Type();
	if ((prot == TCP_ID) || (prot == UDP_ID))
	{
		srcPort = buffer->GetLay4SrcPort();
		destPort = buffer->GetLay4DestPort();
	}
	else if (prot == ICMP_ID) icmpId = buffer->GetICMPIdentifier();
	
	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if ((!InsideDone) && (TableEntry[i].mode == INSIDE))   // nat mode INSIDE
		{
			if (if_mode == INSIDE)   // interface mode INSIDE
			{
				if ((TableEntry[i].type != PAT) && (TableEntry[i].local.dw == srcIP.dw))
				{
					if (TableEntry[i].IsPortForward)
					{
						if ((TableEntry[i].Lay4Prot == prot) && (TableEntry[i].local_port == srcPort))
						{
							buffer->SetSrcIPaddr(TableEntry[i].global);
							buffer->SetLay4SrcPort(TableEntry[i].global_port);
							if (prot == TCP_ID) buffer->FillTCPChecksum();
							else buffer->FillUDPChecksum();
							InsideDone = TRUE;
						}
					}
					else
					{
						buffer->SetSrcIPaddr(TableEntry[i].global);
						ResetRuleTimeout(i);
						InsideDone = TRUE;
					}
				}
				if ((TableEntry[i].type == PAT) && (TableEntry[i].local.dw == srcIP.dw) && (TableEntry[i].Lay4Prot == prot))
				{
					if (((prot == TCP_ID) || (prot == UDP_ID)) && (TableEntry[i].local_port == srcPort))
					{
						buffer->SetSrcIPaddr(TableEntry[i].global);
						buffer->SetLay4SrcPort(TableEntry[i].global_port);
						if (prot == TCP_ID) buffer->FillTCPChecksum();
						else buffer->FillUDPChecksum();
						ResetRuleTimeout(i);
						InsideDone = TRUE;
					}
					if ((prot == ICMP_ID) && (TableEntry[i].local_icmp_id == icmpId))
					{
						buffer->SetSrcIPaddr(TableEntry[i].global);
						buffer->SetICMPIdentifier(TableEntry[i].global_icmp_id);
						buffer->FillICMPChecksum();
						ResetRuleTimeout(i);
						InsideDone = TRUE;
					}
				}
			}
			else   // interface mode OUTSIDE
			{
				if ((TableEntry[i].type != PAT) && (TableEntry[i].global.dw == destIP.dw))
				{
					if (TableEntry[i].IsPortForward)
					{
						if ((TableEntry[i].Lay4Prot == prot) && (TableEntry[i].global_port == destPort))
						{
							buffer->SetDestIPaddr(TableEntry[i].local);
							buffer->SetLay4DestPort(TableEntry[i].local_port);
							if (prot == TCP_ID) buffer->FillTCPChecksum();
							else buffer->FillUDPChecksum();
							OutsideDone = TRUE;
						}
					}
					else
					{
						buffer->SetDestIPaddr(TableEntry[i].local);
						ResetRuleTimeout(i);
						OutsideDone = TRUE;
					}
				}
				if ((TableEntry[i].type == PAT) && (TableEntry[i].global.dw == destIP.dw) && (TableEntry[i].Lay4Prot == prot))
				{
					if (((prot == TCP_ID) || (prot == UDP_ID)) && (TableEntry[i].global_port == destPort))
					{
						buffer->SetDestIPaddr(TableEntry[i].local);
						buffer->SetLay4DestPort(TableEntry[i].local_port);
						if (prot == TCP_ID) buffer->FillTCPChecksum();
						else buffer->FillUDPChecksum();
						ResetRuleTimeout(i);
						InsideDone = TRUE;
					}
					if ((prot == ICMP_ID) && (TableEntry[i].global_icmp_id == icmpId))
					{
						buffer->SetDestIPaddr(TableEntry[i].local);
						buffer->SetICMPIdentifier(TableEntry[i].local_icmp_id);
						buffer->FillICMPChecksum();
						ResetRuleTimeout(i);
						InsideDone = TRUE;
					}
				}
			}
		}

		if ((!OutsideDone) && (TableEntry[i].mode == OUTSIDE))   // NAT mode OUTSIDE
		{
			if (if_mode == INSIDE)   // interface mode INSIDE
			{
				if (TableEntry[i].local.dw == destIP.dw)
				{
					if (TableEntry[i].IsPortForward)
					{
						if ((TableEntry[i].Lay4Prot == prot) && (TableEntry[i].local_port == destPort))
						{
							buffer->SetDestIPaddr(TableEntry[i].global);
							buffer->SetLay4DestPort(TableEntry[i].global_port);
							if (prot == TCP_ID) buffer->FillTCPChecksum();
							else buffer->FillUDPChecksum();
							InsideDone = TRUE;
						}
					}
					else
					{
						buffer->SetDestIPaddr(TableEntry[i].global);
						InsideDone = TRUE;
					}
				}
			}
			else   // interface mode OUTSIDE
			{
				if (TableEntry[i].global.dw == srcIP.dw)
				{
					if (TableEntry[i].IsPortForward)
					{
						if ((TableEntry[i].Lay4Prot == prot) && (TableEntry[i].global_port == srcPort))
						{
							buffer->SetSrcIPaddr(TableEntry[i].local);
							buffer->SetLay4SrcPort(TableEntry[i].local_port);
							if (prot == TCP_ID) buffer->FillTCPChecksum();
							else buffer->FillUDPChecksum();
							OutsideDone = TRUE;
						}
					}
					else
					{
						buffer->SetSrcIPaddr(TableEntry[i].local);
						OutsideDone = TRUE;
					}
				}
			}
		}
	}
	m_cs_table.Unlock();

	if ((!InsideDone) && (if_mode == INSIDE))
	{
		if ((IsDynNatEnabled()) && (CreateDynamicEntry(buffer) == 0)) return 0;
		if ((IsPatEnabled()) && (CreateOverload(buffer,src_if) == 0)) return 0;
		return 1;
	}

	return 0;
}


void NatTable::AddRule(Translation newRule)
{
	int i, count;
	CSingleLock lock(&m_cs_table);

	lock.Lock();
	count = TableEntry.GetCount();
	if (TableEntry.IsEmpty())
	{
		TableEntry.Add(newRule);
		theApp.GetRouterDlg()->InsertTranslation(0,newRule);
		return;
	}

	for (i=0;i < count;i++)
	{
		if (newRule.type == STATICNAT)
		{
			if ((!newRule.IsPortForward) && ((TableEntry[i].global.dw == newRule.global.dw) || (TableEntry[i].local.dw == newRule.local.dw))) return;
			if ((newRule.IsPortForward) && (TableEntry[i].Lay4Prot == newRule.Lay4Prot)
				&& (((TableEntry[i].global.dw == newRule.global.dw) && (TableEntry[i].global_port == newRule.global_port))
				|| ((TableEntry[i].local.dw == newRule.local.dw) && (TableEntry[i].local_port == newRule.local_port)))) return;
			if (TableEntry[i].type != STATICNAT) break;
		}
		if ((newRule.type == DYNAMICNAT) && (TableEntry[i].type == PAT)) break;
		if ((newRule.type == PAT) && (i > 0) && (TableEntry[i-1].type == PAT)
			&& (TableEntry[i].Lay4Prot != newRule.Lay4Prot) && (TableEntry[i-1].Lay4Prot == newRule.Lay4Prot)) break;
	}

	if (i == count)
	{
		TableEntry.Add(newRule);
		theApp.GetRouterDlg()->InsertTranslation(count,newRule);
	}
	else
	{
		TableEntry.InsertAt(i,newRule);
		theApp.GetRouterDlg()->InsertTranslation(i,newRule);
	}
}


int NatTable::RemoveStaticRule(int index)
{
	CSingleLock lock(&m_cs_table);
	
	lock.Lock();
	if (TableEntry[index].type != STATICNAT) return 1;

	TableEntry.RemoveAt(index);
	theApp.GetRouterDlg()->RemoveTranslation(index);
	return 0;
}


int NatTable::IsGlobalIP(IPaddr checkIP)
{
	int i;
	CSingleLock lock(&m_cs_table);

	lock.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (TableEntry[i].global.dw == checkIP.dw) return 1;
	}
	
	return 0;
}


int NatTable::IsPoolConfigured(void)
{
	return IsPoolSet;
}


IPaddr NatTable::GetFirstIPinPool(void)
{
	CSingleLock lock(&m_cs_pool);
	
	lock.Lock();
	return PoolStart;
}


IPaddr NatTable::GetLastIPinPool(void)
{
	CSingleLock lock(&m_cs_pool);
	
	lock.Lock();
	return PoolEnd;
}


void NatTable::SetPool(IPaddr first, IPaddr last)
{
	m_cs_pool.Lock();
	PoolStart = first;
	PoolEnd = last;
	IsPoolSet = TRUE;
	theApp.GetRouterDlg()->EditPool(PoolStart,PoolEnd);
	m_cs_pool.Unlock();
}


void NatTable::ResetRuleTimeout(int index)
{
	m_cs_time.Lock();
	TableEntry[index].timeout = 60;
	m_cs_time.Unlock();
}


void NatTable::Timeout()
{
	int i;

	m_cs_table.Lock();
	m_cs_time.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (TableEntry[i].type != STATICNAT)
		{
			TableEntry[i].timeout -= 5;
			if (TableEntry[i].timeout == 0)
			{
				if (TableEntry[i].Lay4Prot == ICMP_ID)
				{
					m_cs_icmpid.Lock();
					IcmpIdReservation.RemoveKey(TableEntry[i].global_icmp_id);
					m_cs_icmpid.Unlock();
				}
				if (TableEntry[i].type == DYNAMICNAT)
				{
					m_cs_pool.Lock();
					Reservation.RemoveKey((TableEntry[i].global.dw << TableEntry[i].global.SubnetMaskCIDR) >> TableEntry[i].global.SubnetMaskCIDR);
					m_cs_pool.Unlock();
				}
				TableEntry.RemoveAt(i);
				theApp.GetRouterDlg()->RemoveTranslation(i);
				i--;
			}
			else theApp.GetRouterDlg()->UpdateTranslation(i,TableEntry[i].timeout);
		}
	}
	m_cs_time.Unlock();
	m_cs_table.Unlock();
}


void NatTable::RemoveAllDynamic(void)
{
	int i;

	m_cs_table.Lock();
	for (i=0;i < TableEntry.GetCount();i++)
	{
		if (TableEntry[i].type != STATICNAT)
		{
			TableEntry.RemoveAt(i);
			theApp.GetRouterDlg()->RemoveTranslation(i);
			i--;
		}
	}
	m_cs_pool.Lock();
	Reservation.RemoveAll();
	m_cs_pool.Unlock();
	m_cs_icmpid.Lock();
	IcmpIdReservation.RemoveAll();
	m_cs_icmpid.Unlock();
	m_cs_table.Unlock();
}


int NatTable::IsDynNatEnabled(void)
{
	BOOL result;

	m_cs_nat_sw.Lock();
	result = DynNatEnabled;
	m_cs_nat_sw.Unlock();
	
	return result;
}


int NatTable::IsPatEnabled(void)
{
	BOOL result;

	m_cs_nat_sw.Lock();
	result = PatEnabled;
	m_cs_nat_sw.Unlock();
	
	return result;
}


void NatTable::SetDynNatEnabled(int bEnabled)
{
	m_cs_nat_sw.Lock();
	DynNatEnabled = bEnabled;
	if (!DynNatEnabled) RemoveAllDynamic();
	m_cs_nat_sw.Unlock();
}


void NatTable::SetPatEnabled(int bEnabled)
{
	m_cs_nat_sw.Lock();
	PatEnabled = bEnabled;
	if (!PatEnabled) RemoveAllDynamic();
	m_cs_nat_sw.Unlock();
}


int NatTable::CreateDynamicEntry(Frame * buffer)
{
	DWORD i;
	DWORD firstNum;
	DWORD lastNum;
	BOOL value;
	Translation newRule;
	CSingleLock lock(&m_cs_pool);

	lock.Lock();
	firstNum = (PoolStart.dw << PoolStart.SubnetMaskCIDR) >> PoolStart.SubnetMaskCIDR;
	lastNum = (PoolEnd.dw << PoolEnd.SubnetMaskCIDR) >> PoolEnd.SubnetMaskCIDR;

	for (i=firstNum;i <= lastNum;i++) if (!Reservation.Lookup(i,value)) break;
	if (i > lastNum) return 1;

	if (PoolStart.SubnetMaskCIDR == 0) newRule.global.dw = 0;
	else newRule.global.dw = (PoolStart.dw >> (32 - PoolStart.SubnetMaskCIDR)) << (32 - PoolStart.SubnetMaskCIDR);

	newRule.global.dw |= i;
	newRule.global.SubnetMaskCIDR = PoolStart.SubnetMaskCIDR;
	newRule.local = buffer->GetSrcIPaddr();
	newRule.type = DYNAMICNAT;
	newRule.mode = INSIDE;
	newRule.IsPortForward = 0;
	newRule.timeout = 60;
	
	Reservation.SetAt(i,TRUE);
	lock.Unlock();
	AddRule(newRule);
	buffer->SetSrcIPaddr(newRule.global);

	return 0;
}


int NatTable::CreateOverload(Frame * buffer, Interface * src_if)
{
	WORD i;
	WORD srcPort, srcId;
	BOOL value;
	Interface *out_if;
	Translation newRule;
	CSingleLock lock(&m_cs_icmpid);

	if (src_if->GetIndex() == 1) out_if = theApp.GetInt2();
	else out_if = theApp.GetInt1();

	if (!out_if->IsEnabled()) return 1;

	newRule.Lay4Prot = buffer->GetLay4Type();
	newRule.global = out_if->GetIPAddrStruct();
	newRule.local = buffer->GetSrcIPaddr();
	
	if (newRule.Lay4Prot == TCP_ID)
	{
		srcPort = buffer->GetLay4SrcPort();
		newRule.global_port = srcPort;
		newRule.local_port = srcPort;
		buffer->SetSrcIPaddr(newRule.global);
		buffer->FillTCPChecksum();
	}
	else if (newRule.Lay4Prot == UDP_ID)
	{
		srcPort = buffer->GetLay4SrcPort();
		newRule.global_port = srcPort;
		newRule.local_port = srcPort;
		buffer->SetSrcIPaddr(newRule.global);
		buffer->FillUDPChecksum();
	}
	else if (newRule.Lay4Prot == ICMP_ID)
	{
		srcId = buffer->GetICMPIdentifier();
		newRule.local_icmp_id = srcId;
		
		lock.Lock();
		if (IcmpIdReservation.Lookup(srcId,value))
		{
			for (i=1;i <= 65530;i++) if (!IcmpIdReservation.Lookup(i,value)) break;
			if (i > 65530) return 1;
		}
		else i = srcId;
		Reservation.SetAt(i,TRUE);
		lock.Unlock();

		newRule.global_icmp_id = i;
		buffer->SetICMPIdentifier(i);
		buffer->SetSrcIPaddr(newRule.global);
		buffer->FillICMPChecksum();
	} else return 1;

	newRule.type = PAT;
	newRule.mode = INSIDE;
	newRule.IsPortForward = 0;
	newRule.timeout = 60;

	AddRule(newRule);
	return 0;
}
