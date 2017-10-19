#pragma once

class CTestRecord;

class CTestSession
{
public:
	CTestSession();
	CTestSession(const CTestSession & src);
	virtual CTestSession & operator = (const CTestSession & src);
	virtual ~CTestSession();

	void InitSession(int nSessionId, CTestRecord * pRecord);

	int m_nSessionId;
	CTestRecord * m_pRecord;
};

