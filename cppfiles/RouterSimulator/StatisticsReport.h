#pragma once
class CStatisticsReport
{
public:
	CStatisticsReport();
	CStatisticsReport(const CStatisticsReport & src);
	virtual ~CStatisticsReport();

	CStatisticsReport & operator = (const CStatisticsReport & src);

	int m_nStartedPackages;
	int m_nDeliveredPackages;
	int m_nStartAnonyCount;
	int m_nFinishAnonyCount;

	double m_fAveLatency;
	double m_fAveAnonyTimeCost;
	double m_fAveAnonyDistance;
	double m_fAveObfuscationNum;
	double m_fMaxAnonyDistance;
	double m_fAveTotalHops;
	int m_nSourceOnRingCount;
};

