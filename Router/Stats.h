#pragma once

#include "Frame.h"
#include "afxtempl.h"

enum DIRECTION {In, Out};

struct Statistic {
	int IntId;
	DIRECTION d;
	FRAME_TYPE FrameType;
	WORD Lay3Type;
	BYTE Lay4Type;
	char HasFrameType, HasLay3Type, HasLay4Type;
	UINT count;
};

class Stats
{
public:
	Stats(void);
	~Stats(void);
private:
	CArray<Statistic> table;
	BOOL Enabled;
public:
	CCriticalSection m_cs_stats;
	BOOL IsEnabled(void);
	void SetEnabled(BOOL bEnable = TRUE);
	void Add(int IntId, DIRECTION FDirection, Frame * f);
	void Reset(void);
};

