#pragma once
#include "SimulatorCommon.h"
#include "TestSessionBsw.h"

class CRoutingProtocol;

class CTestSessionAptCard :
	public CTestSessionBsw
{
public:
	CTestSessionAptCard();
	CTestSessionAptCard(const CTestSessionAptCard & src);
	virtual CTestSessionAptCard & operator = (const CTestSessionAptCard & src);
	virtual ~CTestSessionAptCard();

	void SetCardInfo(USERID nCid, int nCapt);

	USERID m_nSender;
	USERID m_nLBS;
private:
	USERID m_nCid;
	int m_nCapt;
};