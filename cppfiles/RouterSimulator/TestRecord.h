#pragma once
class CTestRecord
{
public:
	CTestRecord();
	CTestRecord(const CTestRecord & src);
	CTestRecord & operator=(const CTestRecord & src);
	virtual ~CTestRecord();

	int m_nSessionId;
};

