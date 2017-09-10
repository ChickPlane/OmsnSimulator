#pragma once
#include "RoutingMsg.h"
#include "EngineUser.h"
#include "ProtocolInfo.h"
class CHost;
class CHostEngine;
class CRoutingProtocol;

class CRoutingProcess : public CEngineUser
{
public:
	CRoutingProcess();
	virtual ~CRoutingProcess();

	virtual void GenerateMission(const CRoutingDataEnc & encData) = 0;
	virtual void SetEnvironment(CHost * pHost, CHostEngine * pEngine);
	void SetProtocol(CRoutingProtocol * pProtocol) { m_pProtocol = pProtocol; }
	void SetProcessID(int nProcessID) { m_nProcessID = nProcessID; }
	int GetProcessID() { return m_nProcessID; }

	virtual void OnReceivedMsg(const CRoutingMsg * pMsg) = 0;
	virtual void OnEngineTimer(int nCommandId) = 0;
	virtual void TransmitMessage(CRoutingProtocol * pTo, CRoutingMsg * pMsg);
	virtual int GetCarryingMessages(CList<CRoutingDataEnc> & appendTo) const = 0;
	virtual int GetInfoList(CList<CString> & ret);
	virtual void WriteLog(const CString & strLog);

protected:
	CHost * m_pHost;
	CRoutingProtocol * m_pProtocol;
	int m_nProcessID;
};

