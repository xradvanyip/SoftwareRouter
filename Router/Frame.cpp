#include "stdafx.h"
#include "Frame.h"


Frame::Frame(void)
	: frame(NULL)
	, length(0)
{
}


Frame::Frame(u_int FrameLength)
{
	length = FrameLength;
	frame = (u_char *) malloc(FrameLength * sizeof(u_char));
}


Frame::~Frame(void)
{
	if (frame) free(frame);
}


void Frame::AddFrame(u_int length, const u_char *data_const)
{
	BufferedFrame *FrameToAdd = (BufferedFrame *) malloc(sizeof(BufferedFrame));

	FrameToAdd->FrameLength = length;
	FrameToAdd->FrameData = (u_char *) malloc(length);
	memcpy(FrameToAdd->FrameData,data_const,length);
	
	send(buffer,FrameToAdd);
}


void Frame::GetFrame(void)
{
	BufferedFrame *FrameToExtract = receive(buffer);

	if (frame) free(frame);
	length = FrameToExtract->FrameLength;
	frame = FrameToExtract->FrameData;

	free(FrameToExtract);
}


void Frame::Clear(void)
{
	if (frame) free(frame);
	frame = NULL;
	length = 0;
}


MACaddr Frame::GetSrcMAC(void)
{
	MACaddr src;
	int i;
	
	for (i=6;i < 12;i++) src.b[i-6] = frame[i];
	return src;
}


void Frame::SetSrcMAC(MACaddr mac)
{
	int i;
	
	for (i=6;i < 12;i++) frame[i] = mac.b[i-6];
}


MACaddr Frame::GetDestMAC(void)
{
	MACaddr dest;
	int i;

	for (i=0;i < 6;i++) dest.b[i] = frame[i];
	return dest;
}


void Frame::SetDestMAC(MACaddr mac)
{
	int i;
	
	for (i=0;i < 6;i++) frame[i] = mac.b[i];
}


u_char * Frame::GetData(void)
{
	return frame;
}


u_int Frame::GetLength(void)
{
	return length;
}


WORD Frame::MergeBytes(BYTE upper, BYTE lower)
{
	WORD num = upper << 8;
	num |= lower;
	return num;
}


BYTE Frame::GetUpperByte(WORD number)
{
	return number >> 8;
}


BYTE Frame::GetLowerByte(WORD number)
{
	return number & 0xFF;
}


FRAME_TYPE Frame::GetType(void)
{
	if (frame[12] >= 0x06) return ETH2;
	else if ((frame[14] == 0xFF) && (frame[15] == 0xFF)) return RAW;
	else if ((frame[14] == 0xAA) && (frame[15] == 0xAA) && (frame[16] == 0x03)) return SNAP;
	else return LLC;
}


WORD Frame::GetLay3Type(void)
{
	return MergeBytes(frame[12],frame[13]);
}


void Frame::SetLay3Type(WORD EtherType)
{
	frame[12] = GetUpperByte(EtherType);
	frame[13] = GetLowerByte(EtherType);
}


BYTE Frame::GetLay4Type(void)
{
	return frame[23];
}


WORD Frame::GetLay4SrcPort(void)
{
	int IP_header_length;

	IP_header_length = (frame[14] & 0x0F) * 4;

	return MergeBytes(frame[ETH2_HDR_LEN+IP_header_length],frame[ETH2_HDR_LEN+IP_header_length+1]);
}


WORD Frame::GetLay4DestPort(void)
{
	int IP_header_length;

	IP_header_length = (frame[14] & 0x0F) * 4;
	
	return MergeBytes(frame[ETH2_HDR_LEN+IP_header_length+2],frame[ETH2_HDR_LEN+IP_header_length+3]);
}


IPaddr Frame::GetSrcIPaddr(void)
{
	IPaddr src;
	int i;
	
	for (i=26;i < 30;i++) src.b[3-i+26] = frame[i];
	return src;
}


void Frame::SetSrcIPaddr(IPaddr ip)
{
	int i;
	
	for (i=26;i < 30;i++) frame[i] = ip.b[3-i+26];
}


IPaddr Frame::GetDestIPaddr(void)
{
	IPaddr dest;
	int i;
	
	for (i=30;i < 34;i++) dest.b[3-i+30] = frame[i];
	return dest;
}


void Frame::SetDestIPaddr(IPaddr ip)
{
	int i;
	
	for (i=30;i < 34;i++) frame[i] = ip.b[3-i+30];
}


BYTE Frame::GetTTL(void)
{
	return frame[22];
}


void Frame::DecTTL(void)
{
	if (frame[22]) frame[22] -= 1;
}


WORD Frame::CalculateChecksum(int count, u_char *addr)
{
	register DWORD sum = 0;
	WORD *addr_w = (WORD *) addr;

	while (count > 1)
	{
		sum += *addr_w++;
		count -= 2;
	}

	if (count > 0) sum += *(u_char *) addr_w;

	while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);

	return (WORD) (~sum);
}


int Frame::IsIPChecksumValid(void)
{
	if (CalculateChecksum((frame[14] & 0x0F) * 4,frame + ETH2_HDR_LEN) == 0) return TRUE;
	
	return FALSE;
}


void Frame::FillIPChecksum(void)
{
	WORD *chksum_ptr = (WORD *) (frame + 24);
	
	*chksum_ptr = 0;
	*chksum_ptr = CalculateChecksum((frame[14] & 0x0F) * 4,frame + ETH2_HDR_LEN);
}


void Frame::FillUDPChecksum(void)
{
	int IP_header_length = (frame[14] & 0x0F) * 4;
	WORD *chksum_ptr = (WORD *) (frame + ETH2_HDR_LEN + IP_header_length + 6);
	
	*chksum_ptr = 0;
	*chksum_ptr = CalculateChecksum(8,frame + ETH2_HDR_LEN + IP_header_length);
}


int Frame::IsArpRequest(void)
{
	if (MergeBytes(frame[20],frame[21]) == 1) return 1;

	return 0;
}


int Frame::IsArpReply(void)
{
	if (MergeBytes(frame[20],frame[21]) == 2) return 1;

	return 0;
}


void Frame::GenerateArpRequest(MACaddr sender_mac, IPaddr sender_ip, IPaddr target_ip)
{
	int i;
	
	length = ETH2_HDR_LEN + ARP_LEN;
	frame = (u_char *) malloc(length * sizeof(u_char));
	for (i=0;i < 6;i++) frame[i] = 0xFF;  // broadcast
	SetLay3Type(0x0806);

	frame[14] = 0;
	frame[15] = 1;
	frame[16] = 0x08;
	frame[17] = 0;
	frame[18] = 0x06;
	frame[19] = 0x04;
	frame[20] = 0;
	frame[21] = 1;
	for (i=22;i < 28;i++) frame[i] = sender_mac.b[i-22];
	for (i=28;i < 32;i++) frame[i] = sender_ip.b[3-i+28];
	for (i=32;i < 38;i++) frame[i] = 0;
	for (i=38;i < 42;i++) frame[i] = target_ip.b[3-i+38];
}


void Frame::GenerateArpReply(MACaddr sender_mac, IPaddr sender_ip)
{
	int i;
	
	SetDestMAC(GetArpSenderMAC());
	frame[21] = 2;
	for (i=22;i < 32;i++) frame[i+10] = frame[i];
	for (i=22;i < 28;i++) frame[i] = sender_mac.b[i-22];
	for (i=28;i < 32;i++) frame[i] = sender_ip.b[3-i+28];
}


MACaddr Frame::GetArpSenderMAC(void)
{
	MACaddr sender;
	int i;

	for (i=22;i < 28;i++) sender.b[i-22] = frame[i];
	return sender;
}


IPaddr Frame::GetArpSenderIP(void)
{
	IPaddr sender;
	int i;

	for (i=28;i < 32;i++) sender.b[3-i+28] = frame[i];
	return sender;
}


IPaddr Frame::GetArpTargetIP(void)
{
	IPaddr target;
	int i;

	for (i=38;i < 42;i++) target.b[3-i+38] = frame[i];
	return target;
}


int Frame::IsIcmpEchoRequest(void)
{
	if ((GetLay4Type() == 1) && (frame[34] == 8)) return 1;
	
	return 0;
}


void Frame::GenerateIcmpEchoReply(IPaddr local_ip)
{
	int i;
	
	for (i=0;i < 6;i++) frame[i] = frame[i+6];
	
	for (i=26;i < 30;i++)
	{
		frame[i+4] = frame[i];
		frame[i] = local_ip.b[3-i+26];
	}
	SetIpTTL(255);
	FillIPChecksum();

	frame[34] = 0;
	FillICMPChecksum();
}


void Frame::GenerateTTLExceeded(Frame *old, IPaddr local_ip, WORD IPHdrID)
{
	WORD w;
	
	length = old->GetLength() + 28;
	frame = (u_char *) malloc(length * sizeof(u_char));
	SetDestMAC(old->GetSrcMAC());
	SetLay3Type(0x0800);

	frame[14] = 0x45;
	frame[15] = 0xC0;
	w = length - ETH2_HDR_LEN;
	frame[16] = GetUpperByte(w);
	frame[17] = GetLowerByte(w);
	frame[18] = GetUpperByte(IPHdrID);
	frame[19] = GetLowerByte(IPHdrID);
	frame[20] = 0;
	frame[21] = 0;
	frame[22] = 64;
	frame[23] = 1;
	SetSrcIPaddr(local_ip);
	SetDestIPaddr(old->GetSrcIPaddr());
	FillIPChecksum();

	frame[34] = 11;
	frame[35] = 0;
	memcpy(frame + 42, old->GetData() + ETH2_HDR_LEN, old->GetLength() - ETH2_HDR_LEN);
	FillICMPChecksum();
}


void Frame::FillICMPChecksum(void)
{
	int IP_header_length = (frame[14] & 0x0F) * 4;
	WORD *chksum_ptr = (WORD *) (frame + ETH2_HDR_LEN + IP_header_length + 2);
	
	*chksum_ptr = 0;
	*chksum_ptr = CalculateChecksum(MergeBytes(frame[16],frame[17]) - IP_header_length, frame + ETH2_HDR_LEN + IP_header_length);
}


int Frame::IsICMPChecksumValid(void)
{
	int IP_header_length = (frame[14] & 0x0F) * 4;

	if (CalculateChecksum(MergeBytes(frame[16],frame[17]) - IP_header_length, frame + ETH2_HDR_LEN + IP_header_length) == 0) return TRUE;
	
	return FALSE;
}
