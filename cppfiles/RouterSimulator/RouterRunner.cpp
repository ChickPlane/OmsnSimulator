// RouterRunner.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "RouterRunner.h"
#include "RouterSimulatorDoc.h"
#include "RoadNet.h"
#include "Intersection.h"
#include "RoadPoint.h"
#include "commonmsg.h"
#include "RoutingProtocolBSW.h"
#include "MobileSocialNetworkHost.h"
#include "RoutingProtocolHslpo.h"
#include "RoutingProtocolEncAnony.h"


// CRouterRunner

IMPLEMENT_DYNCREATE(CRouterRunner, CWinThread)

CRouterRunner::CRouterRunner()
	:m_pDoc(NULL)
{
}

CRouterRunner::~CRouterRunner()
{
}

BOOL CRouterRunner::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CRouterRunner::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

void CRouterRunner::SetDoc(CRouterSimulatorDoc * pDoc)
{
	m_pDoc = pDoc;
}

void CRouterRunner::OnStartDij(WPARAM wParam, LPARAM lParam)
{
	CRoadNet * pNet = m_pDoc->m_pRoadNet;
	pNet->m_pDijTable = new DijTable();
	pNet->m_pDijTable->Init(pNet->m_allDijNodes);

	int nDijCount = pNet->m_allDijNodes.GetSize();
	int nGroupSize = sqrt(nDijCount);
	for (int i = 0; i < nGroupSize; ++i)
	{
		for (int j = i; j < nDijCount; j+=nGroupSize)
		{
			pNet->m_pDijTable->RunDijOn(j);
			POSITION pos = m_pDoc->GetFirstViewPosition();
			while (pos != NULL)
			{
				CView* pView = m_pDoc->GetNextView(pos);
				pView->PostMessage(AFTERONEDIJNODE, j);
			}
		}
	}
	OnCreateHosts(m_pDoc->m_Cfg.m_nNodeCount + SERVER_NODE_COUNT, 10);
	OutputDebugString(_T("\nInit OK!!!"));
	POSITION pos = m_pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = m_pDoc->GetNextView(pos);
		pView->PostMessage(MSG_ID_DATA_PREPARE_FINISHED);
	}
}

void CRouterRunner::OnCreateHosts(WPARAM wParam, LPARAM lParam)
{
	int nHostCount = wParam;
	int nTryMax = lParam;
	CreateHosts(nHostCount, nTryMax);
}

void CRouterRunner::CreateSocialRelation()
{
	int nHostCount = m_pDoc->m_pRoadNet->m_allHosts.GetSize();
	double fTrustRateBase = pow((double)nHostCount, (1.0 / 4.0));
	double fTrustInitValue = 0.9;
	for (int i = 0; i < nHostCount; ++i)
	{
		CMobileSocialNetworkHost * pHost = (CMobileSocialNetworkHost*)m_pDoc->m_pRoadNet->m_allHosts.GetAt(i);
		double fTrustValue = fTrustInitValue;
		int nLoopCount = fTrustRateBase + 0.5;
		for (int j = 0; j < nLoopCount; ++j)
		{
			CTrustValue fTrustValueTmp = 0;
			while (true)
			{
				int nSelect = rand() % nHostCount;
				if (pHost->FindTrust(nSelect, fTrustValueTmp))
				{
					continue;
				}
				pHost->AddTrust(nSelect, fTrustValue);
				break;
			}
		}
		fTrustValue *= fTrustInitValue;
		nLoopCount = fTrustRateBase * fTrustRateBase + 0.5;
		for (int j = 0; j < nLoopCount; ++j)
		{
			CTrustValue fTrustValueTmp = 0;
			while (true)
			{
				int nSelect = rand() % nHostCount;
				if (pHost->FindTrust(nSelect, fTrustValueTmp))
				{
					continue;
				}
				pHost->AddTrust(nSelect, fTrustValue);
				break;
			}
		}
		fTrustValue *= fTrustInitValue;
		nLoopCount = fTrustRateBase * fTrustRateBase * fTrustRateBase + 0.5;
		for (int j = 0; j < nLoopCount; ++j)
		{
			CTrustValue fTrustValueTmp = 0;
			while (true)
			{
				int nSelect = rand() % nHostCount;
				if (pHost->FindTrust(nSelect, fTrustValueTmp))
				{
					continue;
				}
				pHost->AddTrust(nSelect, fTrustValue);
				break;
			}
		}
	}
}

void CRouterRunner::CreateSocialEvenRelation()
{
	int nHostCount = m_pDoc->m_pRoadNet->m_allHosts.GetSize();
	for (int i = SERVER_NODE_COUNT; i < nHostCount; ++i)
	{
		CMobileSocialNetworkHost * pHost = (CMobileSocialNetworkHost*)m_pDoc->m_pRoadNet->m_allHosts.GetAt(i);
		for (int j = SERVER_NODE_COUNT; j < nHostCount; ++j)
		{
			int nRand = rand() % 101;
			if (nRand < DEFAULT_LOW_TRUST_VALUE)
			{
				continue;
			}
			pHost->AddTrust(j, nRand / 100.0);
		}
	}
}

void CRouterRunner::CreateHosts(int nHostCount, int nTryMax)
{
	m_pDoc->m_pRoadNet->m_allHosts.RemoveAll();
	CreateHostsWithFixedZero();
	unsigned int nSeed = time(NULL);
	CString strRand;
	strRand.Format(_T("\nRand (%d)"), nSeed);
	OutputDebugString(strRand);
	//srand(time(NULL));
	int nCreatedCount = 1;
	int nA, nB;
	int nPointCount = m_pDoc->m_pRoadNet->m_allPoints.GetSize();
	CHost * pHost = NULL;
	while (nCreatedCount < nHostCount)
	{
#if 0
		pHost = new CHost();
#else
		pHost = new CMobileSocialNetworkHost();
#endif
		if (nCreatedCount < nHostCount * 3 / 5 + 1)
		{
			pHost->SetSpeed(GenerateRandomSpeed(HOST_SPEED_PEDESTRIANS_MIN, HOST_SPEED_PEDESTRIANS_MAX));
		}
		else
		{
			pHost->SetSpeed(GenerateRandomSpeed(HOST_SPEED_CAR_MIN, HOST_SPEED_CAR_MAX));
		}
		int nTryTimes = 0;
		while (!pHost->m_schedule.IsScheduleAvailable())
		{
			++nTryTimes;
			if (nTryTimes > nTryMax)
			{
				return;
			}
			nA = rand() % nPointCount;
			nB = rand() % nPointCount;
			m_pDoc->m_pRoadNet->RouteTwoRoadPoints(*pHost, m_pDoc->m_pRoadNet->m_allPoints[nA], m_pDoc->m_pRoadNet->m_allPoints[nB], pHost->m_schedule);
		}
		pHost->m_nId = nCreatedCount;
		m_pDoc->m_pRoadNet->m_allHosts.Add(pHost);
		++nCreatedCount;
	}
	if (nHostCount > 10000)
	{
		ASSERT(0);
	}
	else
	{
		CreateSocialEvenRelation();
	}
}

void CRouterRunner::CreateHostsWithFixedZero()
{
	int nPointCount = m_pDoc->m_pRoadNet->m_allInterSections.GetSize();
	CHost * pHost = new CMobileSocialNetworkHost();
	pHost->m_nId = 0;

	CDoublePoint mapcenter;
	m_pDoc->m_pRoadNet->GetMapCenter(mapcenter);

	int nTryTimes = 0;
	double fDistance = DBL_MAX;
	CHostRouteEntry zeroEntry;
	zeroEntry.m_lnSimTime = 0;
	for (int i = 0; i < 20; ++i)
	{
		int nZero = rand() % nPointCount;
		CDoublePoint zeroPoint = *m_pDoc->m_pRoadNet->m_allInterSections[nZero]->m_pPoint;
		double fDistanceTmp = CDoublePoint::GetDistance(zeroPoint, mapcenter);
		if (fDistanceTmp < fDistance)
		{
			fDistance = fDistanceTmp;
			zeroEntry.m_Position = zeroPoint;
		}
	}
	pHost->m_schedule.m_Entries.Add(zeroEntry);
	m_pDoc->m_pRoadNet->m_allHosts.Add(pHost);
}

double CRouterRunner::GenerateRandomSpeed(double fMin, double fMax)
{
	int range = (fMax - fMin) * 10;
	double speedincrease = rand() % range;
	double fRet = fMin + speedincrease / 10.0;
	return fRet;
}

BEGIN_MESSAGE_MAP(CRouterRunner, CWinThread)
	ON_THREAD_MESSAGE(MSG_ID_START_DIJ, OnStartDij)
	ON_THREAD_MESSAGE(MSG_ID_SET_HOSTS, OnCreateHosts)
END_MESSAGE_MAP()


// CRouterRunner 消息处理程序
