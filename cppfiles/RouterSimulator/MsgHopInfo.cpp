#include "stdafx.h"
#include "MsgHopInfo.h"


CMsgHopInfo::CMsgHopInfo()
{
}


CMsgHopInfo::CMsgHopInfo(const CMsgHopInfo & src)
{
	*this = src;
}

CMsgHopInfo & CMsgHopInfo::operator=(const CMsgHopInfo & src)
{
	m_pProtocol = src.m_pProtocol;
	m_Time = src.m_Time;
	m_Location = src.m_Location;
	m_nComment = src.m_nComment;
	m_eInfoType = src.m_eInfoType;
	return *this;
}

bool CMsgHopInfo::operator==(const CMsgHopInfo & src) const
{
	if (m_pProtocol != src.m_pProtocol)
		return false;
	if (m_Time != src.m_Time)
		return false;
	if (m_Location != src.m_Location)
		return false;
	if (m_nComment != src.m_nComment)
		return false;
	if (m_eInfoType != src.m_eInfoType)
		return false;
	return true;
}

bool CMsgHopInfo::operator!=(const CMsgHopInfo & src) const
{
	return !(*this == src);
}

CMsgHopInfo::~CMsgHopInfo()
{
}


CString GetCommonName(int nCommon)
{
	switch (nCommon)
	{
	case MSG_HOP_STATE_SOURCE:
	{
		return _T("SOURCE");
	}
	case MSG_HOP_STATE_DESTINATION:
	{
		return _T("DESTINATION");
	}
	case MSG_HOP_STATE_ANONYMITY_BEGIN:
	{
		return _T("ANONY_BEGIN");
	}
	case MSG_HOP_STATE_ANONYMITY_END:
	{
		return _T("ANONY_END");
	}
	case MSG_HOP_STATE_ANONYMITY_TRANS:
	{
		return _T("ANONY_TRANS");
	}
	case MSG_HOP_STATE_BSW_BEGIN:
	{
		return _T("BSW_BGN");
	}
	}
}