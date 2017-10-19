#pragma once
#include "RoutingProtocol.h"
class CRoutingProtocolAptCard :
	public CRoutingProtocol
{
public:
	CRoutingProtocolAptCard();
	virtual ~CRoutingProtocolAptCard();

	virtual void OnReceivedMsg(const CYell * pMsg);
	virtual void CreateQueryMission(const CQueryMission * pMission);

	virtual void SetParameters(int nK, int nSeg, int nCopyCount, double fTrust, SIM_TIME lnAcTimeout);
	virtual COLORREF GetInportantLevel() const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);

	virtual int GetDebugNumber(int nParam);
	CString GetDebugString() const;

protected:
	int m_nAptCardProcessId;
};

