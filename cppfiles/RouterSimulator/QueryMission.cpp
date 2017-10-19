#include "stdafx.h"
#include "QueryMission.h"


CQueryMission::CQueryMission()
	: m_SenderId(UID_LBSP0)
	, m_RecverId(UID_LBSP0)
{
	ChangeID();
}


CQueryMission::CQueryMission(const CQueryMission & src)
{
	*this = src;
}

CQueryMission & CQueryMission::operator=(const CQueryMission & src)
{
	m_nMissionId = src.m_nMissionId;
	m_SenderId = src.m_SenderId;
	m_RecverId = src.m_RecverId;
	m_lnTimeOut = src.m_lnTimeOut;
	return *this;
}

CQueryMission::~CQueryMission()
{
}

void CQueryMission::ChangeID()
{
	m_nMissionId = ++sm_nMissionIdMax;
}

int CQueryMission::sm_nMissionIdMax = 0;
