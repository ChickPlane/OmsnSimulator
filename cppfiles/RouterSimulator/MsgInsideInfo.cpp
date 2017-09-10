#include "stdafx.h"
#include "MsgInsideInfo.h"


CMsgInsideInfo::CMsgInsideInfo()
{
}


CMsgInsideInfo::CMsgInsideInfo(const CMsgInsideInfo & src)
{

}

CMsgInsideInfo & CMsgInsideInfo::operator=(const CMsgInsideInfo & src)
{
	m_allRecords.RemoveAll();
	POSITION pos = src.m_allRecords.GetHeadPosition();
	while (pos)
	{
		m_allRecords.AddTail(src.m_allRecords.GetNext(pos));
	}
	m_nDepartureCount = src.m_nDepartureCount;
	m_nArriveCount = src.m_nArriveCount;
	return *this;
}

int CMsgInsideInfo::GetStateIndex(int nState) const
{
	int nRet = -1;
	POSITION pos = m_allRecords.GetHeadPosition();
	while (pos)
	{
		++nRet;
		if (m_allRecords.GetNext(pos).m_nComment == nState)
		{
			return nRet;
		}
	}
	return -1;
}

bool CMsgInsideInfo::IsAnExtent(const CMsgInsideInfo & src) const
{
	POSITION pos = m_allRecords.GetHeadPosition();
	POSITION posSrc = src.m_allRecords.GetHeadPosition();
	while (pos)
	{
		if (!posSrc)
		{
			return true;
		}
		CMsgHopInfo selfInfo = m_allRecords.GetNext(pos);
		CMsgHopInfo otherInfo = src.m_allRecords.GetNext(posSrc);
		if (selfInfo != otherInfo)
		{
			return false;
		}
	}
	return false;
}

void CMsgInsideInfo::GetCountAndIndex(stCountAndPosition ret[MSG_HOP_STATE_MAX])
{
	memset(ret, 0, sizeof(stCountAndPosition) * MSG_HOP_STATE_MAX);
	POSITION pos = m_allRecords.GetHeadPosition(), posLast;
	while (pos)
	{
		posLast = pos;
		CMsgHopInfo info = m_allRecords.GetNext(pos);
		ret[info.m_nComment].nCount++;
		if (ret[info.m_nComment].nCount == 1)
		{
			ret[info.m_nComment].pos = posLast;
		}
	}
}

CMsgInsideInfo::~CMsgInsideInfo()
{
}

void CMsgInsideInfo::RecordInfo(const CMsgHopInfo & info)
{
	switch (info.m_eInfoType)
	{
	case HOP_INFO_TYPE_DEPARTURE:
	{
		RecordDeparture(info.m_pProtocol, info.m_Location, info.m_Time, info.m_nComment);
		break;
	}
	case HOP_INFO_TYPE_ARRIVE:
	{
		RecordArrive(info.m_pProtocol, info.m_Location, info.m_Time, info.m_nComment);
		break;
	}
	case HOP_INFO_TYPE_EVENT:
	{
		RecordEvent(info.m_pProtocol, info.m_Location, info.m_Time, info.m_nComment);
		break;
	}
	default:
	{
		ASSERT(false);
	}
	}
}

void CMsgInsideInfo::RecordDeparture(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment)
{
	CMsgHopInfo hopInfo = GenerateRecord(pProtocol, dpLocation, lnTime, nComment);
	hopInfo.m_eInfoType = HOP_INFO_TYPE_DEPARTURE;
	m_allRecords.AddTail(hopInfo);
	++m_nDepartureCount;
}

void CMsgInsideInfo::RecordArrive(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment)
{
	CMsgHopInfo hopInfo = GenerateRecord(pProtocol, dpLocation, lnTime, nComment);
	hopInfo.m_eInfoType = HOP_INFO_TYPE_ARRIVE;
	m_allRecords.AddTail(hopInfo);
	++m_nArriveCount;
}

void CMsgInsideInfo::RecordEvent(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment)
{
	CMsgHopInfo hopInfo = GenerateRecord(pProtocol, dpLocation, lnTime, nComment);
	hopInfo.m_eInfoType = HOP_INFO_TYPE_EVENT;
	m_allRecords.AddTail(hopInfo);
}

CMsgHopInfo CMsgInsideInfo::GenerateRecord(const CRoutingProtocol * pProtocol, CDoublePoint dpLocation, SIM_TIME lnTime, int nComment)
{
	CMsgHopInfo hopInfo;
	hopInfo.m_pProtocol = pProtocol;
	hopInfo.m_Location = dpLocation;
	hopInfo.m_Time = lnTime;
	hopInfo.m_nComment = nComment;
	return hopInfo;
}

bool CMsgInsideInfo::IsHolding()
{
	if (m_nArriveCount == m_nDepartureCount)
	{
		return true;
	}
	else
	{
		return false;
	}
}

double CMsgInsideInfo::GetHopCount() const
{
	if (m_nDepartureCount < m_nArriveCount)
	{
		return -1;
	}
	if (m_nDepartureCount > m_nArriveCount + 1)
	{
		return -2;
	}
	if (m_nDepartureCount == m_nArriveCount)
	{
		return m_nDepartureCount;
	}
	else
	{
		return m_nDepartureCount + 0.5;
	}
}

const CMsgHopInfo & CMsgInsideInfo::GetRecordAt(POSITION pos) const
{
	return m_allRecords.GetAt(pos);
}

const CMsgHopInfo & CMsgInsideInfo::GetTailRecord() const
{
	return m_allRecords.GetTail();
}

POSITION CMsgInsideInfo::GetRecordWith(int nComment) const
{
	POSITION pos = m_allRecords.GetHeadPosition(), lastPos;
	while (pos)
	{
		lastPos = pos;
		CMsgHopInfo info = m_allRecords.GetNext(pos);
		if (info.m_nComment == nComment)
		{
			return lastPos;
		}
	}
	return pos;
}
