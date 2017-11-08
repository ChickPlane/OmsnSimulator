#pragma once
#include "EngineUser.h"
#include "ProtocolInfo.h"
#include "QueryMission.h"
#include "MsgShowInfo.h"
#include "RoutingProcess.h"
#include "DoublePoint.h"
#include "HostGui.h"
#include "MsgCntJudgeReceiverReport.h"
#include "TestRecord.h"

class CHost;
class CHostEngine;
class CYell;

#define INVALID_PROCESS_ID -1

typedef enum {
	PROTOCOL_PKG_TYPE_SUPPLY,
	PROTOCOL_PKG_TYPE_QUERY,
	PROTOCOL_PKG_TYPE_REPLY,
}PROTOCOL_PKG_TYPE;

class CRoutingProtocol :
	public CEngineUser
{
public:
	CRoutingProtocol();
	virtual ~CRoutingProtocol();

	virtual void CreateQueryMission(const CQueryMission * pMission) = 0;
	virtual void OnReceivedMsg(const CYell * pMsg);
	virtual void OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);
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
	virtual int GetCarryingPkgNumber(int nParam);
	virtual CString GetDebugString() const;

	void WriteLog(const CString & strLog);

	virtual double GetProtocolRecordValue(int nEventId) const;
	virtual void SetProtocolRecordValue(int nEventId, double fValue);

	static BOOL gm_bEnableLbsp;

protected:
	virtual BOOL SetSissionRecord(int nSessionId, int nEventId);
	virtual BOOL SetSissionForwardNumber(int nSessionId, int nForwardNumber);
	void OnSessionRecordChanged();
	void OnProtocolRecordChanged();
	CTestRecord * GetSessionRecord(int nSessionId);
	CProtocolRecord * GetProtocolRecord() const;

	int GetTotalSessionStatisticEntryNumber() { return m_nSessionRecordEntrySize + ENGINE_RECORD_MAX; }

protected:
	CHost * m_pHost;
	double m_fCommunicationRadius;
	CArray<CRoutingProcess *> m_Processes;
	CString m_strLogPrefix;
	static CMap<int, int, CTestRecord *, CTestRecord *> gm_allSessionRecords;
	static CMap<const CRoutingProtocol *, const CRoutingProtocol *, CProtocolRecord *, CProtocolRecord *> gm_allProtocolRecords;
	int m_nSessionRecordEntrySize;
	int m_nProtocolRecordEntrySize;
	int m_nForwardBoundary;
};
