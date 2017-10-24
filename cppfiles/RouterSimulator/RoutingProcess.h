#pragma once
#include "EngineUser.h"
#include "ProtocolInfo.h"
#include "QueryMission.h"
#include "MsgShowInfo.h"
class CHost;
class CHostEngine;
class CRoutingProtocol;
class CYell;
class CSentence;

class CRoutingProcess : public CEngineUser
{
public:
	CRoutingProcess();
	virtual ~CRoutingProcess();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);
	virtual void OnEngineTimer(int nCommandId);
	virtual void StartWork(BOOL bStart);
	virtual void MarkProcessIdToSentences(CList<CSentence*> & SendingList) const;
	virtual void MarkProcessIdToSentences(CSentence* pSentence) const;

	virtual int GetCarryingMessages(CMsgShowInfo & allMessages) const;
	virtual int GetInfoList(CList<CString> & ret) const;

	virtual void TransmitMessage(CRoutingProtocol * pTo, CYell * pMsg);
	virtual void WriteLog(const CString & strLog);
protected:
	CRoutingProtocol * m_pProtocol;
	int m_nProcessID;
};

