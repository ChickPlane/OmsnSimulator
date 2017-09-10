#pragma once
#include "SimulatorCommon.h"
#include "MsgInsideInfo.h"

class CHost;
class CRoutingProtocol;

#define INVALID_DATA_ID -1

class CRoutingData
{
public:
	CRoutingData();
	CRoutingData(const CRoutingData & src);
	virtual CRoutingData & operator = (const CRoutingData & src);
	virtual ~CRoutingData();
	virtual CRoutingData * GetDuplicate() const;

	void ChangeDataId();

	void SetValue(CHost * pHostFrom, CHost * pHostTo, SIM_TIME lnTimeOut);

	virtual int GetDataId(const CHost * pReader) const;
	virtual CHost * GetHostFrom(const CHost * pReader) const;
	virtual CHost * GetHostTo(const CHost * pReader) const;
	virtual SIM_TIME GetTimeOut(const CHost * pReader) const;

public:
	void RecordHop(const CMsgHopInfo & info);
	CMsgInsideInfo m_Statistic;

protected:
	int m_nDataId;
	CHost * m_pHostFrom;
	CHost * m_pHostTo;
	CRoutingProtocol * m_pProtocolSrc;
	CRoutingProtocol * m_pProtocolDst;
	SIM_TIME m_lnTimeOut;

private:
	static int sm_nDataIdMax;
	int IncreaseDataId();
};

