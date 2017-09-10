#include "stdafx.h"
#include "RoutingMsgPmhns.h"
#include "RoutingProcessPmhns.h"
#include "Host.h"



CRoutingMsgPmhns::CRoutingMsgPmhns()
	: CRoutingMsg()
{
}


CRoutingMsgPmhns::CRoutingMsgPmhns(const CRoutingMsgPmhns & src)
{
	*this = src;
}

CRoutingMsgPmhns::CRoutingMsgPmhns(const CRoutingMsg & src)
	: CRoutingMsg(src)
{
	m_nMsgType = PMHNS_MSG_TYPE_DATA;
}

CRoutingMsgPmhns & CRoutingMsgPmhns::operator=(const CRoutingMsgPmhns & src)
{
	CRoutingMsg::operator=(src);
	POSITION pos = src.m_ReqPath.GetHeadPosition();
	while (pos)
	{
		m_ReqPath.AddTail(src.m_ReqPath.GetNext(pos));
	}
	m_nReqId = src.m_nReqId;
	return *this;
}

CRoutingMsgPmhns::~CRoutingMsgPmhns()
{
}

bool CRoutingMsgPmhns::IsRequestArea(const CDoublePoint & msgPosition)
{
	return true;
}

void CRoutingMsgPmhns::InitValues_RReq(CRoutingProtocol * pSource, int nReqId)
{
	m_nMsgType = PMHNS_MSG_TYPE_RREQ;
	m_pSource = pSource;
	m_pFrom = pSource;
	m_pTo = NULL;
	m_ReqPath.RemoveAll();
	m_ReqPath.AddHead(pSource);
	m_nReqId = nReqId;
}

void CRoutingMsgPmhns::InitValues_RRep(CRoutingProtocol * pSource)
{
	m_nMsgType = PMHNS_MSG_TYPE_RREP;
	m_pSource = pSource;
	m_pFrom = pSource;
	m_pTo = m_ReqPath.GetTail();
	m_pDestination = m_ReqPath.GetHead();
	m_ReqPath.AddTail(pSource);
	POSITION pos = m_ReqPath.GetTailPosition();
}

void CRoutingMsgPmhns::InitValues_Data(const CList<CRoutingProtocol*> & RoutePath)
{
	m_nMsgType = PMHNS_MSG_TYPE_DATA;
	m_pSource = RoutePath.GetHead();
	m_pFrom = m_pSource;
	m_pTo = NULL;
	m_pDestination = RoutePath.GetTail();
	POSITION pos = RoutePath.GetHeadPosition();
	m_ReqPath.RemoveAll();
	CString strOut = _T("\n");
	CString strTmp;
	int i = 0;
	while (pos)
	{
		CRoutingProtocol * pTmpPro = RoutePath.GetNext(pos);
		if (i == 0)
		{
			ASSERT(pos);
			++i;
			m_pTo = RoutePath.GetAt(pos);
		}
		strTmp.Format(_T("--%d"), pTmpPro->GetHost()->m_nId);
		strOut += strTmp;
		m_ReqPath.AddTail(pTmpPro);
	}
	OutputDebugString(strOut);
}

void CRoutingMsgPmhns::SetRetransmissionRReq(CRoutingProtocol * pFrom)
{
	m_ReqPath.AddTail(pFrom);
	m_pFrom = pFrom;
}

void CRoutingMsgPmhns::SetRetransmissionRRep(CRoutingProtocol * pFrom)
{
	m_pFrom = pFrom;
	m_pTo = GetPrev(pFrom);
}

void CRoutingMsgPmhns::SetRetransmissionData(CRoutingProtocol * pFrom)
{
	m_pFrom = pFrom;
	m_pTo = GetNext(pFrom);
	ASSERT(m_pTo != NULL);
}

CRoutingProtocol * CRoutingMsgPmhns::GetNext(CRoutingProtocol * pFrom)
{
	POSITION pos = m_ReqPath.GetHeadPosition();
	while (pos)
	{
		if (m_ReqPath.GetNext(pos) == pFrom)
		{
			if (pos)
				return m_ReqPath.GetAt(pos);
			else
				return NULL;
		}
	}
	return NULL;
}

CRoutingProtocol * CRoutingMsgPmhns::GetPrev(CRoutingProtocol * pFrom)
{
	POSITION pos = m_ReqPath.GetTailPosition();
	while (pos)
	{
		if (m_ReqPath.GetPrev(pos) == pFrom)
		{
			if (pos)
				return m_ReqPath.GetAt(pos);
			else
				return NULL;
		}
	}
	return NULL;
}