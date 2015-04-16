#include "stdafx.h"
#include "Frame.h"


Frame::Frame(void)
	: frame(NULL)
	, length(0)
{
}


Frame::~Frame(void)
{
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


WORD Frame::GetLay3Type(void)
{
	return MergeBytes(frame[12],frame[13]);
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


IPaddr Frame::GetDestIPaddr(void)
{
	IPaddr dest;
	int i;
	
	for (i=30;i < 34;i++) dest.b[3-i+30] = frame[i];
	return dest;
}


BYTE Frame::GetTTL(void)
{
	return frame[22];
}


void Frame::DecTTL(void)
{
	frame[22] -= 1;
}


WORD Frame::CalculateChecksum(int count, u_char *addr)
{
	register DWORD sum = 0;

	while (count > 1)
	{
		sum += *((WORD *) addr++);
		count -= 2;
	}

	if (count > 0) sum += *addr;

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
