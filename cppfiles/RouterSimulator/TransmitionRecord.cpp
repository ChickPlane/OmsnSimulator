#include "stdafx.h"
#include "TransmitionRecord.h"


CTransmitionRecord::CTransmitionRecord()
	: m_pFrom(NULL)
	, m_pTo(NULL)
	, m_pMsg(NULL)
	, m_nMsgId(0)
{
}


CTransmitionRecord::CTransmitionRecord(CRoutingProtocol * pFrom, CRoutingProtocol * pTo, CYell * pMsg, int nMsgId)
	: m_pFrom(pFrom)
	, m_pTo(pTo)
	, m_pMsg(pMsg)
	, m_nMsgId(nMsgId)
{

}

CTransmitionRecord::CTransmitionRecord(const CTransmitionRecord & src)
{
	*this = src;
}

CTransmitionRecord & CTransmitionRecord::operator=(const CTransmitionRecord & src)
{
	m_pFrom = src.m_pFrom;
	m_pTo = src.m_pTo;
	m_pMsg = src.m_pMsg;
	m_nMsgId = src.m_nMsgId;
	return *this;
}

CTransmitionRecord::~CTransmitionRecord()
{
}
