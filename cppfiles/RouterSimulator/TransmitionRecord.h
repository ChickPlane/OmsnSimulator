#pragma once

class CHost;
class CYell;
class CRoutingProtocol;

class CTransmitionRecord
{
public:
	CTransmitionRecord();
	CTransmitionRecord(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CYell * pMsg, int nMsgId);
	CTransmitionRecord(const CTransmitionRecord & src);
	CTransmitionRecord & operator = (const CTransmitionRecord & src);
	~CTransmitionRecord();

	BOOL IsUnicast() const;

	CRoutingProtocol * m_pFrom;
	CRoutingProtocol * m_pTo;
	CYell * m_pMsg;
	int m_nMsgId;
};

