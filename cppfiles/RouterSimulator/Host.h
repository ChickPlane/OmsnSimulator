#pragma once
#include "HostRouteTable.h"
#include "RoutingProtocol.h"
#include "HostInfo.h"
class CHost
{
public:
	CHost();
	CHost(const CHost & src);
	CHost & operator = (const CHost & src);
	virtual ~CHost();
	virtual double GetDefaultSpeed() const;
	virtual void SetSpeed(double fSpeed);
	virtual void OnHearMsg(CRoutingMsg * pMsg);
	virtual void OnPackageArrived(const CRoutingDataEnc & encData);
	virtual bool IsReceivedPackage(const CRoutingDataEnc * pEncData);
	virtual void GetAllCarryingMessages(CList<CRoutingDataEnc> & ret);

	CDoublePoint GetPosition(SIM_TIME lnSimTime) const;
	void Reset();
	void GetInfo(CHostInfo & ret);

	CHostRouteTable m_schedule;
	CRoutingProtocol * m_pProtocol;
	int m_nId;
	CMap<int, int, int, int> m_nReceivedMsgs;
protected:
	double m_fSpeed;
};

