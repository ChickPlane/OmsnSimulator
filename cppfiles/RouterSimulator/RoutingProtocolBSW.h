#pragma once
#include "RoutingProtocol.h"
#include "RoutingDataEnc.h"

#define BSW_PROCESS_ID 0

class CRoutingProtocolBSW :
	public CRoutingProtocol
{
public:
	CRoutingProtocolBSW();
	virtual ~CRoutingProtocolBSW();

	virtual void OnReceivedMsg(const CRoutingMsg * pMsg);
	virtual void SendPackage(const CRoutingDataEnc & encData);
	virtual void SetCopyCount(int nCopyCount);

	virtual int GetInportantLevel() const;

protected:
	int m_nBswProcessId;
};

