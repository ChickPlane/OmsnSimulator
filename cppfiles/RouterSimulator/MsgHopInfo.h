#pragma once
#include "DoublePoint.h"
#include "SimulatorCommon.h"

class CRoutingProtocol;

enum {
	MSG_HOP_STATE_SOURCE,
	MSG_HOP_STATE_DESTINATION,
	MSG_HOP_STATE_OTHERS,
	MSG_HOP_STATE_ANONYMITY_BEGIN,
	MSG_HOP_STATE_ANONYMITY_END,
	MSG_HOP_STATE_ANONYMITY_TRANS,
	MSG_HOP_STATE_BSW_BEGIN,
	MSG_HOP_STATE_MAX
};

typedef enum
{
	HOP_INFO_TYPE_EVENT,
	HOP_INFO_TYPE_DEPARTURE,
	HOP_INFO_TYPE_ARRIVE
}HOP_INFO_TYPE;

CString GetCommonName(int nCommon);

class CMsgHopInfo
{
public:
	CMsgHopInfo();
	CMsgHopInfo(const CMsgHopInfo & src);
	CMsgHopInfo & operator = (const CMsgHopInfo & src);
	bool operator == (const CMsgHopInfo & src) const;
	bool operator != (const CMsgHopInfo & src) const;
	virtual ~CMsgHopInfo();

	const CRoutingProtocol * m_pProtocol;
	CDoublePoint m_Location;
	SIM_TIME m_Time;
	int m_nComment;
	HOP_INFO_TYPE m_eInfoType;
};

