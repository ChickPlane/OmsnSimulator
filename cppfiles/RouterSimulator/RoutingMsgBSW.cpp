#include "stdafx.h"
#include "RoutingMsgBSW.h"
#include "RoutingProtocolBSW.h"
#include "RoutingProtocol.h"
#include "Host.h"



CRoutingMsgBSW::CRoutingMsgBSW()
	: m_nCopyCount(0)
	, m_nBswId(INVALID_BSW_ID)
{
	m_nMsgType = BSW_MSG_TYPE_DATA;
}


CRoutingMsgBSW::CRoutingMsgBSW(const CRoutingMsgBSW & src)
{
	*this = src;
}

CRoutingMsgBSW & CRoutingMsgBSW::operator=(const CRoutingMsgBSW & src)
{
	CRoutingMsg::operator=(src);
	m_nCopyCount = src.m_nCopyCount;
	m_nBswId = src.m_nBswId;
	m_KnownNodes.RemoveAll();
	POSITION pos = src.m_KnownNodes.GetHeadPosition();
	while (pos)
	{
		m_KnownNodes.AddTail(src.m_KnownNodes.GetNext(pos));
	}
	return *this;
}

CRoutingMsgBSW::~CRoutingMsgBSW()
{
}

void CRoutingMsgBSW::InitValues_Data(CRoutingProtocol * pFrom, int nCopyCount)
{
	m_nMsgType = BSW_MSG_TYPE_DATA;
	ChangeBswId();
	m_nCopyCount = nCopyCount;
	ASSERT(m_nCopyCount >= 1);
	m_KnownNodes.RemoveAll();
	m_KnownNodes.AddHead(pFrom);
}

void CRoutingMsgBSW::InitValues_Hello(CRoutingProtocol * pFrom)
{
	m_nMsgType = BSW_MSG_TYPE_HELLO;
	InitOneHopBroadcost(pFrom);
	m_nCopyCount = 0;
	m_KnownNodes.RemoveAll();
}

void CRoutingMsgBSW::InitValues_HelloAck(CRoutingProtocol * pFrom, CRoutingProtocol * pTo)
{
	m_nMsgType = BSW_MSG_TYPE_HELLO_ACK;
	InitOneHopUnicost(pFrom, pTo);
	m_nCopyCount = 0;
	m_KnownNodes.RemoveAll();
}

void CRoutingMsgBSW::SetSendValues_Data(CRoutingProtocol * pFrom, CRoutingProtocol * pTo)
{
	InitOneHopUnicost(pFrom, pTo);
}

void CRoutingMsgBSW::UpdateRecordMsg(CRoutingProtocol * pKnownProtocol)
{
	m_KnownNodes.AddTail(pKnownProtocol);
	m_nCopyCount = (m_nCopyCount + 1) / 2;
}

void CRoutingMsgBSW::MergeMessage(const CRoutingMsgBSW & src)
{
	POSITION posOther = src.m_KnownNodes.GetHeadPosition();
	while (posOther)
	{
		CRoutingProtocolBSW * pTest = (CRoutingProtocolBSW*)src.m_KnownNodes.GetNext(posOther);
		bool bFound = IsKnownNode(pTest);
		if (!bFound)
		{
			m_KnownNodes.AddTail(pTest);
		}
	}
	m_nCopyCount += src.m_nCopyCount;
}

bool CRoutingMsgBSW::IsKnownNode(CRoutingProtocol * pTest) const
{
	POSITION posSelf = m_KnownNodes.GetHeadPosition();
	while (posSelf)
	{
		CRoutingProtocolBSW * pSelf = (CRoutingProtocolBSW*)m_KnownNodes.GetNext(posSelf);
		if (pTest == pSelf)
		{
			return true;
		}
	}
	return false;
}

int CRoutingMsgBSW::sm_nMaxBswId = INVALID_BSW_ID;

void CRoutingMsgBSW::ChangeBswId()
{
	++sm_nMaxBswId;
	m_nBswId = sm_nMaxBswId;
}
