#include "stdafx.h"
#include "RoutingDataEncBSW.h"


CRoutingDataEncBSW::CRoutingDataEncBSW()
{
}


CRoutingDataEncBSW::CRoutingDataEncBSW(const CRoutingDataEncBSW & src)
{
	*this = src;
}

CRoutingDataEncBSW & CRoutingDataEncBSW::operator=(const CRoutingDataEncBSW & src)
{
	CRoutingDataEnc::operator=(src);
	m_CarryingProcesses.RemoveAll();
	POSITION pos = src.m_CarryingProcesses.GetHeadPosition();
	while (pos)
	{
		m_CarryingProcesses.AddTail(src.m_CarryingProcesses.GetNext(pos));
	}
	return *this;
}

CRoutingDataEncBSW::~CRoutingDataEncBSW()
{
}

CRoutingDataEncBSW * CRoutingDataEncBSW::GetDuplicate() const
{
	CRoutingDataEncBSW * pRet = new CRoutingDataEncBSW(*this);
	return pRet;
}
