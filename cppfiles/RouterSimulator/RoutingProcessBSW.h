#pragma once
#include "RoutingProcess.h"
#include "RoutingDataEncBSW.h"

class CRoutingMsgBSW;

enum
{
	BSW_TIMER_CMD_HELLO,
	BSW_TIMER_CMD_MAX
};

class CRoutingProcessBSW :
	public CRoutingProcess
{
public:
	CRoutingProcessBSW();
	virtual ~CRoutingProcessBSW();
	void SetCopyCount(int nCopyCount);

	virtual void GenerateMission(const CRoutingDataEnc & encData);
	virtual void OnReceivedMsg(const CRoutingMsg * pMsg);
	virtual void OnEngineTimer(int nCommandId);

	virtual void OnReceivedHello(const CRoutingMsg * pMsg);
	virtual void OnReceivedHelloAck(const CRoutingMsg * pMsg);
	virtual void OnReceivedData(const CRoutingMsg * pMsg);
	virtual void SendHello();
	virtual void SendHelloAck(CRoutingProtocol * pTo);
	virtual void SendData(CRoutingProtocol * pNext);

	virtual void CleanTimeOutData();

	virtual void WriteLog(const CString & strLog);

	CList<CRoutingMsgBSW*> m_DataList;
	virtual void StartBinarySprayWaitProcess();
	virtual void ContinueBinarySprayWaitProcess();
	CRoutingMsgBSW * CheckDuplicateInDataList(int nBswId);

	virtual int GetCarryingMessages(CList<CRoutingDataEnc> & appendTo) const;
	virtual int GetMsgCount() const;

protected:
	int m_nCopyCount;
	bool m_bWork;
	bool m_bBswLogSwitch;
};

