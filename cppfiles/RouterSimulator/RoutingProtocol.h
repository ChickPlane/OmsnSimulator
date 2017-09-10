#pragma once
#include "RoutingMsg.h"
#include "EngineUser.h"
#include "ProtocolInfo.h"
class CHost;
class CHostEngine;
class CRoutingProcess;

#define INVALID_PROCESS_ID -1

class CRoutingProtocol :
	public CEngineUser
{
public:
	CRoutingProtocol();
	virtual ~CRoutingProtocol();

	virtual void SendPackage(const CRoutingDataEnc & encData) = 0;
	virtual void OnReceivedMsg(const CRoutingMsg * pMsg) = 0;
	virtual void SetEnvironment(CHost * pHost, CHostEngine * pEngine);
	virtual void RecordNewPackage(CRoutingDataEnc & encData);
	virtual void OnEngineTimer(int nCommandId);
	virtual void GetAllCarryingMessages(CList<CRoutingDataEnc> & ret) const;

	virtual void OnDelivered(const CRoutingMsg * pMsgBase);

	virtual void SetCommunicateRadius(double fCommunicationRadius);
	virtual double GetCommunicateRadius() const;

	virtual int GetInportantLevel() const { return 0; }

	CHost * GetHost() const;
	int GetHostId() const;
	CDoublePoint GetHostPostion(SIM_TIME lnTime) const;
	virtual int AddProcess(CRoutingProcess * pProcess);
	virtual int GetInfoList(CList<CString> & ret);

	CMsgHopInfo GetMsgHopInfo(int nComment, HOP_INFO_TYPE eType) const;
	virtual void TransmitMessage(CRoutingProcess * pFromProcess, CRoutingProtocol * pTo, CRoutingMsg * pMsg);
	virtual int GetProcessId(CRoutingProcess * pProcess);

	void WriteLog(const CString & strLog);
protected:
	CHost * m_pHost;
	double m_fCommunicationRadius;
	CArray<CRoutingProcess *> m_Processes;
	CString m_strLogPrefix;
};

