#pragma once

class CBuildingType
{
public:
	CBuildingType();
	~CBuildingType();
	CString m_strName;
	int m_nEnployeeLimit;
	int m_nVisitLimit;
	int m_nLivingLimit;
};

