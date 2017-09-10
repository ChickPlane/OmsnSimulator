#pragma once
#include "MsgInfo.h"
#include "SimulatorCommon.h"
#include "MsgInsideInfo.h"
#include "RoutingDataEnc.h"
class CHost;
class CRoutingProtocol;

class CRoutingMsg
{
public:
	CRoutingMsg();
	CRoutingMsg(const CRoutingMsg & src);
	CRoutingMsg & operator = (const CRoutingMsg & src);
	virtual ~CRoutingMsg();

	bool IsBelongTo(CRoutingProtocol * pOwner) const;
	void ChangeMsgId();
	void InitByData(const CRoutingDataEnc & srcData);

	int m_nMessageId;
	CRoutingProtocol * m_pSource;
	CRoutingProtocol * m_pFrom;
	CRoutingProtocol * m_pTo;
	CRoutingProtocol * m_pDestination;
	SIM_TIME m_lnTimeOut;

	int m_nMsgType;
	int m_nProtocolType;

	CRoutingDataEnc * m_pData;

protected:
	void InitOneHopBroadcost(CRoutingProtocol * pSource);
	void InitOneHopUnicost(CRoutingProtocol * pSource, CRoutingProtocol * pDestination);
	void CopyData(const CRoutingDataEnc * pDataSrc);

private:
	static int sm_nDefaultMessageId;
};

