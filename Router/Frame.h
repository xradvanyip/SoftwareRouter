#pragma once

#include <agents.h>

#define ETH2_HDR_LEN 14

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
	~Frame(void);
private:
	unbounded_buffer<BufferedFrame *> buffer;
	u_char *frame;
	u_int length;
	WORD CalculateChecksum(int count, u_char *addr);
public:
	void AddFrame(u_int length, const u_char *data);
	void GetFrame(void);
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
	BYTE GetLay4Type(void);
	WORD GetLay4SrcPort(void);
	WORD GetLay4DestPort(void);
	IPaddr GetSrcIPaddr(void);
	IPaddr GetDestIPaddr(void);
	BYTE GetTTL(void);
	void DecTTL(void);
	int IsIPChecksumValid(void);
	void FillIPChecksum(void);
	void FillUDPChecksum(void);
};

