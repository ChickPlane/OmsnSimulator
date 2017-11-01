#pragma once
#include "EngineUser.h"
#include "ProtocolInfo.h"
#include "QueryMission.h"
#include "MsgShowInfo.h"
#include "RoutingProcess.h"
#include "DoublePoint.h"
#include "HostGui.h"
#include "MsgCntJudgeReceiverReport.h"

class CHost;
class CHostEngine;
class CYell;

#define INVALID_PROCESS_ID -1

class CRoutingProtocol :
	public CEngineUser
{
public:
	CRoutingProtocol();
	virtual ~CRoutingProtocol();

	virtual void CreateQueryMission(const CQueryMission * pMission) = 0;
	virtual void OnReceivedMsg(const CYell * pMsg);
	virtual void OnEngineConnection(const CList<CJudgeTmpRouteEntry> & m_Hosts, const CMsgCntJudgeReceiverReport* pWholeReport);
	virtual void SetEnvironment(CHost * pHost, CHostEngine * pEngine);
	virtual void OnEngineTimer(int nCommandId);
	virtual void GetAllCarryingMessages(CMsgShowInfo & allMessages) const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);

	virtual void OnDelivered(const CQueryMission * pMission);

	virtual void SetCommunicateRadius(double fCommunicationRadius);
	virtual double GetCommunicateRadius() const;

	virtual COLORREF GetImportantLevel() const { return 0; }

	CHost * GetHost() const;
	int GetHostId() const;
	CDoublePoint GetHostPostion(SIM_TIME lnTime) const;
	virtual int AddProcess(CRoutingProcess * pProcess);

	virtual void TransmitMessage(CRoutingProtocol * pTo, CYell * pMsg);
	virtual void TransmitSingleSentence(CSentence * pSentence);
	virtual int GetProcessId(CRoutingProcess * pProcess);
	virtual int GetDebugNumber(int nParam);
	virtual CString GetDebugString() const;

	void WriteLog(const CString & strLog);

	static BOOL gm_bEnableLbsp;

protected:
	CHost * m_pHost;
	double m_fCommunicationRadius;
	CArray<CRoutingProcess *> m_Processes;
	CString m_strLogPrefix;
};
