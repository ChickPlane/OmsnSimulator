#pragma once
#include "RoutingProcess.h"
#include "HostGui.h"
#include "MsgCntJudgeReceiverReport.h"

class CPkgAck;
class CPkgHello;
class CRoutingProcessHello;

class CRoutingProcessHelloUser
{
public:
	virtual void OnHearHelloFromOthers(CRoutingProcessHello * pCallBy, const CPkgHello * pPkg);
	virtual void OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg) = 0;

	virtual CPkgHello * GetHelloPackage(CRoutingProcessHello * pCallBy);
	virtual CPkgAck * GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo);
	virtual CPkgAck * GetOkPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo);
};

enum
{
	HELLO_TIMER_CMD_SEARCH,
	HELLO_TIMER_CMD_MAX
};

class CRoutingProcessHello :
	public CRoutingProcess
{
public:
	CRoutingProcessHello();
	virtual ~CRoutingProcessHello();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	virtual void SetProcessUser(CRoutingProcessHelloUser * pUser) { m_pUser = pUser; }
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);
	virtual void OnSomeoneNearby(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);
	virtual void StartWork(BOOL bStart);
	virtual void EnableHelloMsg(BOOL bEnable) { m_bSendHello = bEnable; }
	static void SetInterval(SIM_TIME lnInterval) { m_lnSearchInterval = lnInterval; }
protected:
	virtual void StartSearhing();
	virtual void SendHelloPackage();

private:
	CRoutingProcessHelloUser * m_pUser;
	BOOL m_bIsSearching;
	BOOL m_bSendHello;
	static SIM_TIME m_lnSearchInterval;

};

