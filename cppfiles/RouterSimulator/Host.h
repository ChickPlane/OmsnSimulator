#pragma once
#include "HostRouteTable.h"
#include "RoutingProtocol.h"
#include "HostInfo.h"
#include "MsgShowInfo.h"
#include "HostGui.h"
#include "MsgCntJudgeReceiverReport.h"
class CHost
{
public:
	CHost();
	CHost(const CHost & src);
	CHost & operator = (const CHost & src);
	virtual ~CHost();
	virtual double GetDefaultSpeed() const;
	virtual void SetSpeed(double fSpeed);
	virtual void OnHearMsg(const CYell * pYell);
	virtual void OnConnection(const CList<CJudgeTmpRouteEntry> & m_Hosts, const CMsgCntJudgeReceiverReport* pWholeReport);
	virtual void OnPackageArrived(const CQueryMission * pMission);
	virtual bool IsReceivedPackage(const CQueryMission * pMission);
	virtual void GetAllCarryingMessages(CMsgShowInfo & allMessages);

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

