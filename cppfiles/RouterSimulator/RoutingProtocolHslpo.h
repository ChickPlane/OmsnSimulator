#pragma once
#include "RoutingProtocol.h"

class CRoutingProtocolHslpo :
	public CRoutingProtocol
{
public:
	CRoutingProtocolHslpo();
	virtual ~CRoutingProtocolHslpo();

	virtual void OnReceivedMsg(const CRoutingMsg * pMsg);
	virtual void SendPackage(const CRoutingDataEnc & encData);

	virtual void SetPrivacyParam(int nK, double fHigh, double fLow);
	virtual void SetCopyCount(int nCopyCount);
	virtual int GetInportantLevel() const;
protected:
	int m_nHslpoProcessId;
};

