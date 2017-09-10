#pragma once

class CHost;
class CRoutingMsg;
class CRoutingProtocol;

class CTransmitionRecord
{
public:
	CTransmitionRecord();
	CTransmitionRecord(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CRoutingMsg * pMsg, int nMsgId);
	CTransmitionRecord(const CTransmitionRecord & src);
	CTransmitionRecord & operator = (const CTransmitionRecord & src);
	~CTransmitionRecord();

	CRoutingProtocol * m_pFrom;
	CRoutingProtocol * m_pTo;
	CRoutingMsg * m_pMsg;
	int m_nMsgId;
};

