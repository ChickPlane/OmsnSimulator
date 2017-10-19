#include "stdafx.h"
#include "AptCardAgencyRecord.h"
#include "AppointmentCard.h"


CAptCardAgencyRecord::CAptCardAgencyRecord()
	: m_bTheLastRelay(FALSE)
{
}


CAptCardAgencyRecord::CAptCardAgencyRecord(const CAptCardAgencyRecord & src)
{
	*this = src;
}

CAptCardAgencyRecord& CAptCardAgencyRecord::operator=(const CAptCardAgencyRecord & src)
{
	m_nAidOld = src.m_nAidOld;
	m_nAAptOld = src.m_nAAptOld;
	n_nAAptNew = src.n_nAAptNew;
	m_nExchangeTo = src.m_nExchangeTo;
	m_lnTimeout = src.m_lnTimeout;
	m_bTheLastRelay = src.m_bTheLastRelay;
	return *this;
}

CAptCardAgencyRecord::~CAptCardAgencyRecord()
{
}

void CAptCardAgencyRecord::InitByAptCard(CAppointmentCard * pCard)
{
	m_nAidOld = pCard->m_nAid;
	m_nAAptOld = pCard->m_nAapt;
	m_lnTimeout = pCard->m_lnTimeout;
}

CString CAptCardAgencyRecord::GetString() const
{
	CString strOut;
	strOut.Format(_T("%d\t%d ;\t%d\t%d"), m_nAidOld, m_nAAptOld, m_nExchangeTo, n_nAAptNew);
	return strOut;
}
