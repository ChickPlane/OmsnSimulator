#pragma once
#include "SimulatorCommon.h"

class CAppointmentCard;

class CAptCardAgencyRecord
{
public:
	CAptCardAgencyRecord();
	CAptCardAgencyRecord(const CAptCardAgencyRecord & src);
	CAptCardAgencyRecord& operator = (const CAptCardAgencyRecord & src);
	virtual ~CAptCardAgencyRecord();

	void InitByAptCard(CAppointmentCard * pCard);
	CString GetString() const;

	USERID m_nAidOld;
	int m_nAAptOld;

	USERID m_nExchangeTo;
	int n_nAAptNew;

	SIM_TIME m_lnTimeout;
	bool m_bTheLastRelay;
};

