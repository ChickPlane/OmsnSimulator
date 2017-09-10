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

void CHost::OnHearMsg(CRoutingMsg * pMsg)
{
	if (m_pProtocol)
	{
		m_pProtocol->OnReceivedMsg(pMsg);
	}
}

void CHost::OnPackageArrived(const CRoutingDataEnc & encData)
{
	int nDataId = encData.GetDataId(this);
	ASSERT(nDataId != INVALID_DATA_ID);
	m_nReceivedMsgs[nDataId] = 1;

	CString strOut;
	int nToId = m_nId;
	strOut.Format(_T("Receive [%7d] DESTINATION (%d) !!!"), nDataId, nToId);
	m_pProtocol->WriteLog(strOut);
}

bool CHost::IsReceivedPackage(const CRoutingDataEnc * pEncData)
{
	int nValue;
	return true == m_nReceivedMsgs.Lookup(pEncData->GetDataId(this), nValue);
}

void CHost::GetAllCarryingMessages(CList<CRoutingDataEnc> & ret)
{
	ret.RemoveAll();
	m_pProtocol->GetAllCarryingMessages(ret);
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
