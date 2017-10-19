#pragma once
#include "AptCardAgencyRecord.h"
class CAptCardFromSameAgency
{
public:
	CAptCardFromSameAgency();
	CAptCardFromSameAgency(const CAptCardFromSameAgency & src);
	CAptCardFromSameAgency & operator = (const CAptCardFromSameAgency & src);
	virtual ~CAptCardFromSameAgency();

	CString GetString() const;

	CList<CAptCardAgencyRecord> m_Records;
};

