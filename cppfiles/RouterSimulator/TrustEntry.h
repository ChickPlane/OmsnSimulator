#pragma once
#include "TrustValue.h"
class CTrustEntry
{
public:
	CTrustEntry();
	CTrustEntry(const CTrustEntry & src);
	CTrustEntry & operator = (const CTrustEntry & src);
	~CTrustEntry();

	int m_nHostId;
	CTrustValue m_fTrustValue;
};

