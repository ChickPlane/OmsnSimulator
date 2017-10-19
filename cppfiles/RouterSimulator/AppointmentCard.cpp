#include "stdafx.h"
#include "AppointmentCard.h"


CAppointmentCard::CAppointmentCard()
	: m_nPsd(UID_INVALID)
	, m_bIsReady(FALSE)
{
}


CAppointmentCard::CAppointmentCard(const CAppointmentCard & src)
{
	*this = src;
}

CAppointmentCard::~CAppointmentCard()
{
}

BOOL CAppointmentCard::IsFinal() const
{
	return m_bIsReady;
}

BOOL CAppointmentCard::CheckDuplicatedUid(USERID uUid) const
{
	POSITION pos = m_EQ.GetHeadPosition();
	while (pos)
	{
		if (m_EQ.GetNext(pos) == uUid)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CString CAppointmentCard::GetString() const
{
	CString strRet;
	strRet.Format(_T("AC %d %d :_ "), m_nCid, m_nCapt);
	POSITION pos = m_EQ.GetHeadPosition();
	while (pos)
	{
		CString strTmp;
		strTmp.Format(_T("%d,"), m_EQ.GetNext(pos));
		strRet += strTmp;
	}
	return strRet;
}

CAppointmentCard & CAppointmentCard::operator=(const CAppointmentCard & src)
{
	m_nCid = src.m_nCid;
	m_nCapt = src.m_nCapt;
	m_nAid = src.m_nAid;
	m_nAapt = src.m_nAapt;
	m_lnTimeout = src.m_lnTimeout;
	m_bIsReady = src.m_bIsReady;
	m_EQ.RemoveAll();
	POSITION pos = src.m_EQ.GetHeadPosition();
	while (pos)
	{
		m_EQ.AddTail(src.m_EQ.GetAt(pos));
		src.m_EQ.GetNext(pos);
	}

	m_Mark.RemoveAll();
	pos = src.m_Mark.GetHeadPosition();
	while (pos)
	{
		m_Mark.AddTail(src.m_Mark.GetAt(pos));
		src.m_Mark.GetNext(pos);
	}

	m_nDC = src.m_nDC;
	m_nPsd = src.m_nPsd;
	return *this;
}
