#pragma once
#include "HostRouteTable.h"
#include "RoutingProtocol.h"
#include "HostInfo.h"
#include "MsgShowInfo.h"
#include "MsgCntJudgeReceiverReport.h"

class CMsgCntJudgeReceiverReport;

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
	virtual void UpdateNetworkLocations(const CMsgCntJudgeReceiverReport* pWholeReport);
	virtual void OnEnterNewTimePeriod();
	virtual void OnPackageArrived(const CQueryMission * pMission);
	virtual bool IsReceivedPackage(const CQueryMission * pMission);
	virtual void GetAllCarryingMessages(CMsgShowInfo & allMessages);
	const CMsgCntJudgeReceiverReport* GetRecentReport() const;

	CDoublePoint GetPosition(SIM_TIME lnSimTime) const;
	void Reset();
	void GetInfo(CHostInfo & ret);

	CHostRouteTable m_schedule;
	CRoutingProtocol * m_pProtocol;
	int m_nId;
	CMap<int, int, int, int> m_nReceivedMsgs;

protected:
	virtual BOOL IsDifferentList(const CReceiverReportItem & reportItem);

protected:
	double m_fSpeed;

private:
	int * m_pDirNeighbours;
	int m_nDirNeighbourCount;
	int * m_pAllNeighbours;
	int m_nAllNeighbourCount;
	const CMsgCntJudgeReceiverReport* m_pCurrentReport;
	BOOL m_bDifferentFromPrev;
	BOOL m_bAnyOneNearby;
};

