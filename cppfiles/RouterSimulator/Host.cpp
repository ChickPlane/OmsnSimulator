#include "stdafx.h"
#include "Host.h"


CHost::CHost()
	: m_pProtocol(NULL)
	, m_nId(0)
{
}


CHost::CHost(const CHost & src)
{
	*this = src;
}

CHost & CHost::operator=(const CHost & src)
{
	m_schedule = src.m_schedule;
	m_pProtocol = NULL;
	m_nId = src.m_nId;
	return *this;
}

CHost::~CHost()
{
	Reset();
}

double CHost::GetDefaultSpeed() const
{
	return m_fSpeed;
}

void CHost::SetSpeed(double fSpeed)
{
	m_fSpeed = fSpeed;
}

void CHost::OnHearMsg(const CYell * pYell)
{
	if (m_pProtocol)
	{
		m_pProtocol->OnReceivedMsg(pYell);
	}
}

void CHost::OnPackageArrived(const CQueryMission * pMission)
{
	m_nReceivedMsgs[pMission->m_nMissionId] = 1;
	ASSERT(m_nId == pMission->m_nMissionId);

	CString strOut;
	int nToId = m_nId;
	strOut.Format(_T("Receive [%7d] DESTINATION (%d) !!!"), pMission->m_nMissionId, nToId);
	m_pProtocol->WriteLog(strOut);
}

bool CHost::IsReceivedPackage(const CQueryMission * pMission)
{
	int nValue;
	return true == m_nReceivedMsgs.Lookup(pMission->m_nMissionId, nValue);
}

void CHost::GetAllCarryingMessages(CMsgShowInfo & allMessages)
{
	m_pProtocol->GetAllCarryingMessages(allMessages);
}

CDoublePoint CHost::GetPosition(SIM_TIME lnSimTime) const
{
	return m_schedule.GetPosition(lnSimTime);
}

void CHost::Reset()
{
	m_schedule.Reset();
	if (m_pProtocol)
	{
		delete m_pProtocol;
		m_pProtocol = NULL;
	}
	m_nReceivedMsgs.RemoveAll();
}

void CHost::GetInfo(CHostInfo & ret)
{
	ret.Reset();
	ret.m_nHostId = m_nId;
}
