#pragma once

#include <agents.h>

#define ETH2_HDR_LEN 14

using namespace Concurrency;

enum FRAME_TYPE {ETH2, RAW, SNAP, LLC};

struct IPaddr {
	BYTE b[4];
	BYTE SubnetMaskCIDR;
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
};

