#include "stdafx.h"
#include "StatisticSummary.h"

CStatisticSummary::CStatisticSummary()
	: m_nTotleNodeCount(0)
	, m_nRandomSeed(0)
	, m_nMsgCount(0)
	, m_nDeliveryCount(0)
	, m_nAnonymityK(0)
	, m_fLatency(0)
	, m_nAnonymityStartCount(0)
	, m_nAnonymityEndCount(0)
	, m_nAnonymityTimeCost(0)
	, m_fAnonymityDistance(0)
	, m_fAnonymityMaxDistance(0)
	, m_nTrustValue(0)
	, m_fAveInterHopCount(0)
	, m_fAveAnonySurroundingCount(0)
	, m_fAveRealSurrounding(0)
	, m_fRadius(0)
	, m_fAveRingCount(0)
	, m_nOnRingCount(0)
	, m_fAveObfuscationNum(0)
{
}


CStatisticSummary::CStatisticSummary(const CStatisticSummary & src)
{
	*this = src;
}

CStatisticSummary & CStatisticSummary::operator=(const CStatisticSummary & src)
{
	m_nTotleNodeCount = src.m_nTotleNodeCount;
	m_nMsgCount = src.m_nMsgCount;
	m_nDeliveryCount = src.m_nDeliveryCount;
	m_nAnonymityK = src.m_nAnonymityK;
	m_fLatency = src.m_fLatency;
	m_nAnonymityStartCount = src.m_nAnonymityStartCount;
	m_nAnonymityEndCount = src.m_nAnonymityEndCount;
	m_nAnonymityTimeCost = src.m_nAnonymityTimeCost;
	m_fAnonymityDistance = src.m_fAnonymityDistance;
	m_fAnonymityMaxDistance = src.m_fAnonymityMaxDistance;
	m_nTrustValue = src.m_nTrustValue;
	m_fRadius = src.m_fRadius;
	m_fAveInterHopCount = src.m_fAveInterHopCount;
	m_fAveAnonySurroundingCount = src.m_fAveAnonySurroundingCount;
	m_fAveRealSurrounding = src.m_fAveRealSurrounding;
	m_ProtocolName = src.m_ProtocolName;
	m_nRandomSeed = src.m_nRandomSeed;
	m_fAveRingCount = src.m_fAveRingCount;
	m_nOnRingCount = src.m_nOnRingCount;
	m_fAveObfuscationNum = src.m_fAveObfuscationNum;
	return *this;
}

CStatisticSummary::~CStatisticSummary()
{
}

CString CStatisticSummary::GetString()
{
	CString strRet;
	strRet.Format(_T("%d\t%d\t%d\t%d\t%3f\t%d\t%d\t%d\t%f")
		, m_nTotleNodeCount
		, m_nMsgCount
		, m_nDeliveryCount
		, m_nAnonymityK
		, m_fLatency
		, m_nAnonymityStartCount
		, m_nAnonymityEndCount
		, m_nAnonymityTimeCost
		, (float)m_fAnonymityDistance);
	return strRet;
}
#if 0
void CStatisticSummary::GetString(char * pOut)
{
	sprintf_s(pOut, 150, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\tTr=%d\tK=%d\tRad=%f\t%f\t%f"
		, m_nTotleNodeCount
		, m_nMsgCount
		, m_nDeliveryCount
		, m_nLatency
		, m_nAnonymityStartCount
		, m_nAnonymityEndCount
		, m_nAnonymityTimeCost
		, (float)m_fAnonymityDistance
		, m_nTrustValue
		, m_nAnonymityK
	    , (float)m_fRadius
	    , (float)m_fAveInterHopCount
	    , (float)m_fAveAnonySurroundingCount);
}
#else
void CStatisticSummary::GetString(char * pOut)
{
	int len = WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, NULL, 0, NULL, NULL);
	char *filename = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, m_ProtocolName, -1, filename, len, NULL, NULL);

	double fTmp = m_fAveRingCount * m_nAnonymityEndCount / (m_nTotleNodeCount * m_nOnRingCount);
	sprintf_s(pOut, 150, "%s\t%d\t%d\t%d\t%d\t%3f\t%d\t%f\tT=%d\tK=%d\tR=%f\t%f\t%f\t%f\t%f\t%f\t%d\t%f\t%f"
		, filename
		, m_nRandomSeed
		, m_nTotleNodeCount
		, m_nMsgCount
		, m_nDeliveryCount
		, m_fLatency
		, m_nAnonymityEndCount
		, (float)m_fAnonymityDistance
		, m_nTrustValue
		, m_nAnonymityK
		, (float)m_fRadius
		, (float)m_fAveInterHopCount
		, (float)m_fAveAnonySurroundingCount
		, (float)m_fAveRealSurrounding
		, (float)m_fAnonymityMaxDistance
		, (float)m_fAveRingCount
		, m_nOnRingCount
		, (float)fTmp
		, (float)m_fAveObfuscationNum
	);

	delete[] filename;
}

#endif

CString CStatisticSummary::GetUiString()
{
	CString strOut;
	strOut.Format(_T("T(%4d) | D(%4d) | AS(%4d) | AE(%4d) | LA(%3f)s | AT(%5d)s | AD(%6.3f)m | HP(%2f) ")
		, m_nMsgCount
		, m_nDeliveryCount
		, m_nAnonymityStartCount
		, m_nAnonymityEndCount
		, m_fLatency
		, m_nAnonymityTimeCost
		, m_fAnonymityDistance
		, m_fAveInterHopCount);
	return strOut;
}