#pragma once
#include "SimulatorCommon.h"
#include "MsgHopInfo.h"

class CRoutingProtocol;

struct stCountAndPosition
{
	int nCount;
	POSITION pos;
};

class CMsgInsideInfo
{
public:
	CMsgInsideInfo();
	CMsgInsideInfo(const CMsgInsideInfo & src);
	CMsgInsideInfo & operator = (const CMsgInsideInfo & src);
	int GetStateIndex(int nState) const;
	bool IsAnExtent(const CMsgInsideInfo & src) const;
	void GetCountAndIndex(stCountAndPosition ret[MSG_HOP_STATE_MAX]);

	virtual ~CMsgInsideInfo();
	void RecordInfo(const CMsgHopInfo & info);
	void RecordDeparture(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment);
	void RecordArrive(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment);
	void RecordEvent(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment);
	CMsgHopInfo GenerateRecord(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment);
	bool IsHolding();
	double GetHopCount() const;
	const CMsgHopInfo & GetRecordAt(POSITION pos) const;
	const CMsgHopInfo & GetTailRecord() const;
	POSITION GetRecordWith(int nComment) const;

protected:
	int m_nDepartureCount;
	int m_nArriveCount;
	CList<CMsgHopInfo> m_allRecords;
};

