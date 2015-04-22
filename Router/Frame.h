#pragma once

#include <agents.h>

#define ETH2_HDR_LEN 14
#define ARP_LEN 28

struct MACaddr {
	BYTE b[6];
};

using namespace Concurrency;

enum FRAME_TYPE {ETH2, RAW, SNAP, LLC};

struct IPaddr {
	union {
		BYTE b[4];
		DWORD dw;
	};
	union {
		BYTE SubnetMaskCIDR;
		BYTE HasNextHop;
	};
};

struct BufferedFrame {
	u_int FrameLength;
	u_char *FrameData;
};

class Frame
{
public:
	Frame(void);
	Frame(u_int FrameLength);
	~Frame(void);
private:
	unbounded_buffer<BufferedFrame *> buffer;
	u_char *frame;
	u_int length;
	WORD CalculateChecksum(int count, u_char *addr);
public:
	void AddFrame(u_int length, const u_char *data);
	void GetFrame(void);
	void Clear(void);
	
	MACaddr GetSrcMAC(void);
	void SetSrcMAC(MACaddr mac);
	MACaddr GetDestMAC(void);
	void SetDestMAC(MACaddr mac);
	u_char * GetData(void);
	u_int GetLength(void);
	WORD MergeBytes(BYTE upper, BYTE lower);
	BYTE GetUpperByte(WORD number);
	BYTE GetLowerByte(WORD number);
	WORD GetLay3Type(void);
	void SetLay3Type(WORD EtherType);
	
	/* IPv4 */
	IPaddr GetSrcIPaddr(void);
	void SetSrcIPaddr(IPaddr ip);
	IPaddr GetDestIPaddr(void);
	void SetDestIPaddr(IPaddr ip);
	BYTE GetTTL(void);
	void DecTTL(void);
	int IsIPChecksumValid(void);
	void FillIPChecksum(void);
	BYTE GetLay4Type(void);
	WORD GetLay4SrcPort(void);
	WORD GetLay4DestPort(void);

	/* UDP */
	void FillUDPChecksum(void);

	/* ARP */
	int IsArpRequest(void);
	int IsArpReply(void);
	void GenerateArpRequest(MACaddr sender_mac, IPaddr sender_ip, IPaddr target_ip);
	void GenerateArpReply(MACaddr sender_mac, IPaddr sender_ip);
	MACaddr GetArpSenderMAC(void);
	IPaddr GetArpSenderIP(void);
	IPaddr GetArpTargetIP(void);

	/* ICMP */
	int IsIcmpEchoRequest(void);
	void GenerateIcmpEchoReply(IPaddr local_ip);
	void GenerateTTLExceeded(Frame *old, IPaddr local_ip, WORD IPHdrID);
	void FillICMPChecksum(void);
	int IsICMPChecksumValid(void);
};

