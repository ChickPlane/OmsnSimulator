#pragma once
#include "EngineUser.h"
#include "ProtocolInfo.h"
#include "QueryMission.h"
#include "MsgShowInfo.h"

class CHost;
class CHostEngine;
class CRoutingProcess;
class CYell;

#define INVALID_PROCESS_ID -1

class CRoutingProtocol :
	public CEngineUser
{
public:
	CRoutingProtocol();
	virtual ~CRoutingProtocol();

	virtual void CreateQueryMission(const CQueryMission * pMission) = 0;
	virtual void OnReceivedMsg(const CYell * pMsg) = 0;
	virtual void SetEnvironment(CHost * pHost, CHostEngine * pEngine);
	virtual void OnEngineTimer(int nCommandId);
	virtual void GetAllCarryingMessages(CMsgShowInfo & allMessages) const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);

	virtual void OnDelivered(const CQueryMission * pMission);

	virtual void SetCommunicateRadius(double fCommunicationRadius);
	virtual double GetCommunicateRadius() const;

	virtual COLORREF GetInportantLevel() const { return 0; }

	CHost * GetHost() const;
	int GetHostId() const;
	CDoublePoint GetHostPostion(SIM_TIME lnTime) const;
	virtual int AddProcess(CRoutingProcess * pProcess);

	CMsgHopInfo GetMsgHopInfo(int nComment, HOP_INFO_TYPE eType) const;
	virtual void TransmitMessage(CRoutingProcess * pFromProcess, CRoutingProtocol * pTo, CYell * pMsg);
	virtual int GetProcessId(CRoutingProcess * pProcess);
	virtual int GetDebugNumber(int nParam);
	virtual CString GetDebugString() const;

	void WriteLog(const CString & strLog);
protected:
	CHost * m_pHost;
	double m_fCommunicationRadius;
	CArray<CRoutingProcess *> m_Processes;
	CString m_strLogPrefix;
};
