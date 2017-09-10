#include "stdafx.h"
#include "RoutingMsg.h"
#include "Host.h"


CRoutingMsg::CRoutingMsg()
	: m_pSource(NULL)
	, m_pFrom(NULL)
	, m_pTo(NULL)
	, m_pDestination(NULL)
	, m_lnTimeOut(0)
	, m_nMsgType(0)
	, m_nProtocolType(0)
{
	m_pData = new CRoutingDataEnc();
	ChangeMsgId();
}

CRoutingMsg::CRoutingMsg(const CRoutingMsg & src)
{
	*this = src;
}

CRoutingMsg & CRoutingMsg::operator=(const CRoutingMsg & src)
{
	m_pSource = src.m_pSource;
	m_pDestination = src.m_pDestination;
	m_nMessageId = src.m_nMessageId;
	m_lnTimeOut = src.m_lnTimeOut;
	m_pFrom = src.m_pFrom;
	m_pTo = src.m_pTo;
	m_nMsgType = src.m_nMsgType;
	m_nProtocolType = src.m_nProtocolType;

	CopyData(src.m_pData);

	return *this;
}

CRoutingMsg::~CRoutingMsg()
{
	if (m_pData)
	{
		delete m_pData;
	}
}

bool CRoutingMsg::IsBelongTo(CRoutingProtocol * pOwner) const
{
	if (m_pFrom == pOwner)
	{
		return false;
	}
	if (m_pTo != NULL && m_pTo != pOwner)
	{
		return false;
	}
	return true;
}

void CRoutingMsg::ChangeMsgId()
{
	++sm_nDefaultMessageId;
	m_nMessageId = sm_nDefaultMessageId;
}

void CRoutingMsg::InitByData(const CRoutingDataEnc & srcData)
{
	ASSERT(srcData.CanRead(NULL));
	m_pSource = srcData.GetHostFrom(NULL)->m_pProtocol;
	m_pDestination = srcData.GetHostTo(NULL)->m_pProtocol;
	m_lnTimeOut = srcData.GetTimeOut(NULL);
	CopyData(&srcData);
}

void CRoutingMsg::InitOneHopBroadcost(CRoutingProtocol * pFrom)
{
	m_pFrom = pFrom;
	m_pTo = NULL;
}

void CRoutingMsg::InitOneHopUnicost(CRoutingProtocol * pFrom, CRoutingProtocol * pTo)
{
	m_pFrom = pFrom;
	m_pTo = pTo;
}

void CRoutingMsg::CopyData(const CRoutingDataEnc * pDataSrc)
{
	if (m_pData)
	{
		delete m_pData;
		m_pData = NULL;
	}
	if (pDataSrc)
	{
		m_pData = pDataSrc->GetDuplicate();
	}
}

int CRoutingMsg::sm_nDefaultMessageId = 0;
