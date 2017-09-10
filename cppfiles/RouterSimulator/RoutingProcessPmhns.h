#pragma once
#include "RoutingProcess.h"

class CRoutingProtocol;

class CRoutingUserProcessPmhns
{
public:
	virtual void OnGetPmhnsRep(const CList<CRoutingProtocol*> & path);
	virtual void OnGetPmhnsData(const CRoutingMsg * pMsg);
};

enum
{
	DSR_TIMER_CMD_MAX
};

class CRoutingProcessPmhns :
	public CRoutingProcess
{
public:
	CRoutingProcessPmhns();
	virtual ~CRoutingProcessPmhns();

	virtual void GetRoutings();
	virtual void SendDataPackage(const CList<CRoutingProtocol*> & path, const CRoutingDataEnc & encData);
	virtual void GenerateMission(const CRoutingDataEnc & encData);

	virtual void OnReceivedMsg(const CRoutingMsg * pMsg);
	virtual void OnEngineTimer(int nCommandId);

	virtual void OnGetRReq(const CRoutingMsg * pMsg);
	virtual void OnGetRRep(const CRoutingMsg * pMsg);
	virtual void OnGetData(const CRoutingMsg * pMsg);

	virtual void WriteLog(const CString & strLog);
	virtual bool IsNewReq(CRoutingProtocol * pProtocol, int nReqId) const;
	virtual bool RecordReq(CRoutingProtocol * pProtocol, int nReqId);
	virtual int GetCarryingMessages(CList<CRoutingDataEnc> & appendTo) const;

	bool m_bDsrLogSwitch;
	int m_nReqId;
	CRoutingUserProcessPmhns * m_pUser;
	CMap<CRoutingProtocol *, CRoutingProtocol *, int, int> m_ReqMap;

};

