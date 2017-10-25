#pragma once
class CStatisticSummary
{
public:
	CStatisticSummary();
	CStatisticSummary(const CStatisticSummary & src);
	CStatisticSummary & operator = (const CStatisticSummary & src);
	virtual ~CStatisticSummary();

	CArray<double> m_RealData;
	CArray<CString> m_StringData;

	CString m_ProtocolName;
	int m_nRandomSeed;
	int m_nTotleNodeCount;
	int m_nMsgCount;
	int m_nDeliveryCount;
	int m_nAnonymityK;
	double m_fLatency;
	int m_nAnonymityStartCount;
	int m_nAnonymityEndCount;
	int m_nAnonymityTimeCost;
	int m_nTrustValue;
	double m_fAveInterHopCount;
	double m_fAveAnonySurroundingCount;
	double m_fAveRealSurrounding;
	double m_fRadius;
	double m_fAnonymityDistance;
	double m_fAnonymityMaxDistance;
	int m_nAveStartTime;
	double m_fAveRingCount;
	double m_fAveObfuscationNum;
	int m_nOnRingCount;

	CString GetString();
	CString GetUiString();
	void GetString(char * pOut);

};

