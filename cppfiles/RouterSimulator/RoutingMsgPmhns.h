#pragma once
#include "RoutingMsg.h"
#include "DoublePoint.h"

typedef enum
{
	PMHNS_MSG_TYPE_DATA,
	PMHNS_MSG_TYPE_RREQ,
	PMHNS_MSG_TYPE_RREP,
	PMHNS_MSG_TYPE_MAX,
}DSR_MSG_TYPE;

class CRoutingProtocolPmhns;

class CRoutingMsgPmhns :
	public CRoutingMsg
{
public:
	CRoutingMsgPmhns();
	CRoutingMsgPmhns(const CRoutingMsgPmhns & src);
	CRoutingMsgPmhns(const CRoutingMsg & src);
	CRoutingMsgPmhns & operator = (const CRoutingMsgPmhns & src);
	virtual ~CRoutingMsgPmhns();

	virtual bool IsRequestArea(const CDoublePoint & msgPosition);
	void InitValues_RReq(CRoutingProtocol * pSource, int nReqId);
	void InitValues_RRep(CRoutingProtocol * pSource);
	void InitValues_Data(const CList<CRoutingProtocol*> & RoutePath);

	void SetRetransmissionRReq(CRoutingProtocol * pFrom);
	void SetRetransmissionRRep(CRoutingProtocol * pFrom);
	void SetRetransmissionData(CRoutingProtocol * pFrom);

	CRoutingProtocol * GetNext(CRoutingProtocol * pFrom);
	CRoutingProtocol * GetPrev(CRoutingProtocol * pFrom);

	CList<CRoutingProtocol*> m_ReqPath;
	int m_nReqId;

};

