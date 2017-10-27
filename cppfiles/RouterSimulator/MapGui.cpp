// MapGui.cpp : 实现文件
//

#include "stdafx.h"
#include "RouterSimulator.h"
#include "MapGui.h"
#include "Road.h"
#include "Intersection.h"
#include "RoadPoint.h"
#include "RoadNet.h"
#include "RoadPointConnection.h"
#include "DijNode.h"
#include "DijTable.h"
#include "DijItem.h"
#include "commonmsg.h"

#include "MainFrm.h"
#include "HostGui.h"
#include "HostEngine.h"
#include "DlgHostInfo.h"
#include "MobileSocialNetworkHost.h"
#include "CommonFunctions.h"

#define START_TEST_SIM_TIME 800000

void CMapGui::SetScale(int nScaleIndex)
{
	m_nScaleIndex = nScaleIndex;
	m_nIntersectionRadius = sm_fIntersectionRadius / GetScale();
	if (m_nIntersectionRadius < 2)
	{
		m_nIntersectionRadius = 2;
	}

	int nWidth, nHeight;
	GetBackupPictureRange(nWidth, nHeight, m_nScaleIndex);

	m_sizeTotal.cx = nWidth;
	m_sizeTotal.cy = nHeight;

	if (IsWindow(GetSafeHwnd()))
	{
		SetScrollSizes(MM_TEXT, m_sizeTotal);

		CPoint newScrollPos = GetNewScrollPos();
		CRect rectClient;
		GetClientRect(&rectClient);
		if (rectClient.Width() < m_sizeTotal.cx)
		{
			if (newScrollPos.x > 0)
				SetScrollPos(SB_HORZ, newScrollPos.x);
			else
				SetScrollPos(SB_HORZ, 0);
		}
		else
		{
			SetScrollPos(SB_HORZ, 0);
		}

		if (rectClient.Height() < m_sizeTotal.cy)
		{
			if (newScrollPos.y > 0)
				SetScrollPos(SB_VERT, newScrollPos.y);
			else
				SetScrollPos(SB_VERT, 0);
		}
		else
		{
			SetScrollPos(SB_VERT, 0);
		}

		RefreshUi(true);
	}
}

void CMapGui::SetRoadNet(CRoadNet * pRoadNet)
{
	m_pRoadNet = pRoadNet;
	m_pRoadNet->GetMapRange(m_LeftTop, m_RightBottom);
	if (IsWindow(GetSafeHwnd()))
	{
		RefreshUi(true);
	}
	m_RoadPointInfos.SetSize(m_pRoadNet->m_allPoints.GetSize());
	int nLengh = m_RoadPointInfos.GetSize();
	for (int i = 0; i < nLengh; ++i)
	{
		m_RoadPointInfos[i].SetValue(*m_pRoadNet->m_allPoints[i]);
	}
}

void CMapGui::SetEngine(CHostEngine * pEngine)
{
	m_pEngine = pEngine;
}

void CMapGui::RefreshUi(bool bRedrawAll)
{
	if (bRedrawAll)
	{
		m_nBackupVersion++;
		if (m_nBackupVersion == 0)
		{
			m_nBackupVersion++;
		}
	}

	Invalidate();
}

// CMapGui

IMPLEMENT_DYNCREATE(CMapGui, CScrollView)

CMapGui::CMapGui()
	:m_pRoadNet(NULL)
	, m_fScale(1)
	, m_sizeTotal(0, 0)
	, m_LastMousePosition(0, 0)
	, m_nLastSelectedId(-1)
	, m_nDijFinishedCount(0)
	, m_nBackupVersion(0)
	, m_nScaleIndex(0)
	, m_pHostNeedUpdate(NULL)
	, m_pEngine(NULL)
	, m_BackgroundColor(RGB(0,0,0))
	, m_bEngineShouldBeRunning(false)
	, m_bEnginePaused(true)
	, m_llLastDrawTime(0)
	, m_pFollowHost(NULL)
	, m_lnExpectEndTime(0)
	, m_pView(NULL)
	, m_lnLastEngineTime(0)
	, m_nRestartTimes(0)
{

}

CMapGui::~CMapGui()
{
	if (m_pHostNeedUpdate)
	{
		delete m_pHostNeedUpdate;
	}
}


void CMapGui::DrawRoads(CDC * pMemDC)
{
	int nRoadWidth = sm_fRoadWidth / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(254, 216, 157));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nPointCount = m_pRoadNet->m_allPoints.GetSize();
	for (int i = 0; i < nPointCount; ++i)
	{
		CPoint pointStart;
		PtrRoadPoint pRecent = m_pRoadNet->m_allPoints[i];
		GeographicToGraphicPoint(*((CDoublePoint*)pRecent), pointStart);
		int nConnect = pRecent->m_ConnectedPoints.GetSize();
		for (int j = 0; j < nConnect; ++j)
		{
			CPoint pointEnd;
			PtrRoadPoint pOther = pRecent->m_ConnectedPoints[j];
			GeographicToGraphicPoint(*((CDoublePoint*)pOther), pointEnd);
			pMemDC->MoveTo(pointStart);
			pMemDC->LineTo(pointEnd);
		}
	}

	pMemDC->SelectObject(pOldPen);
}

void CMapGui::DrawIntersections(CDC & MemDC, CBitmap & MemBitmap)
{
	int nRoadWidth = sm_fRoadHalfWidth / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(100, 100, 100));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nIntersectionCount = m_pRoadNet->m_allInterSections.GetSize();
	int nRadius = GetIntersectionRadius();
	for (int i = 0; i < nIntersectionCount; ++i)
	{
		PtrIntersection pIntersection = m_pRoadNet->m_allInterSections[i];
		PtrRoadPoint pPoint = pIntersection->m_pPoint;
		CPoint pointCenter;
		GeographicToGraphicPoint(*(CDoublePoint*)pPoint, pointCenter);

		MemDC.Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);

	}
	MemDC.SelectObject(pOldPen);
	MemDC.SelectObject(pOldBrush);
}


void CMapGui::DrawRoadConnections(CDC & MemDC, CBitmap & MemBitmap)
{
	int nRoadWidth = sm_fConnectWidth / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(50, 50, 200));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nIntersectionCount = m_pRoadNet->m_allInterSections.GetSize();
	for (int i = 0; i < nIntersectionCount; ++i)
	{
		//CPen * pTpen = new CPen(PS_SOLID, nRoadWidth, RGB(rand() % 200, rand() % 200, rand() % 200));
		//MemDC.SelectObject(pTpen);

		CPoint pointStart;
		PtrRoadPoint pdStart = (PtrRoadPoint)m_pRoadNet->m_allInterSections[i]->m_pPoint;
		GeographicToGraphicPoint(*pdStart, pointStart);

		int nConnectCount = pdStart->m_NearbyIntersections.GetSize();
		for (int j = 0; j < nConnectCount; ++j)
		{
			int nEndId = pdStart->m_NearbyIntersections[j].m_nIntersectionId;
			CDoublePoint * pdEnd = m_pRoadNet->m_allInterSections[nEndId]->m_pPoint;
			ASSERT(pdEnd != NULL);
			CPoint pointEnd;
			GeographicToGraphicPoint(*pdEnd, pointEnd);
			MemDC.MoveTo(pointStart);
			MemDC.LineTo(pointEnd);
		}
		//delete pTpen;

	}

	MemDC.SelectObject(pOldPen);
}

void CMapGui::DrawAllDijNodes(CDC * pMemDC)
{
	COLORREF color(RGB(255, 193, 19));
	int nRoadWidth = sm_fRoadMarkLineWidth / GetScale();
	int nRadius = sm_fRoadMarkRadius / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, color);
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush Bursh(color);
	CBrush *pOldBrush = pMemDC->SelectObject(&Bursh);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nDijCount = m_pRoadNet->m_allDijNodes.GetSize();
	for (int i = 0; i < nDijCount; ++i)
	{
		CDijNode * pDij = m_pRoadNet->m_allDijNodes[i];
		PtrRoadPoint pPoint = pDij->m_pIntersection->m_pPoint;
		CPoint pointCenter;
		GeographicToGraphicPoint(*(CDoublePoint*)pPoint, pointCenter);

		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);

	}
	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawSchedules(CDC * pMemDC)
{
	int nCount = m_pRoadNet->m_allHosts.GetSize();
	CDoublePoint lt, rb;
	GetRefreshArea(lt, rb);
	for (int i = 0; i < nCount; ++i)
	{
		DrawOneSchedule(pMemDC, i, lt, rb);
	}
}

void CMapGui::DrawOneSchedule(CDC * pMemDC, int nHostId, CDoublePoint & lt, CDoublePoint & rb)
{
	CHost * pTmp = m_pRoadNet->m_allHosts[nHostId];
	int nRoadWidth = sm_fRoadHalfWidth / GetScale();
	int nRadius = sm_fRoadHalfWidth * 1.5 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(10, 100, 200));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush *pOldBrush = pMemDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nPointCount = pTmp->m_schedule.m_Entries.GetSize();
	for (int i = 0; i < nPointCount; ++i)
	{
		CDoublePoint aPoint = pTmp->m_schedule.m_Entries[i].m_Position;
		if (aPoint.m_X < lt.m_X || aPoint.m_X > rb.m_X)
			continue;
		if (aPoint.m_Y < lt.m_Y || aPoint.m_Y > rb.m_Y)
			continue;
		
		CPoint pointCenter;
		GeographicToGraphicPoint(aPoint, pointCenter);

		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);

	}
	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawHosts(CDC * pMemDC)
{
	DrawHosts_AptCard(pMemDC);
	return;

	CDoublePoint lt, rb;
	GetRefreshArea(lt, rb);
	if (!m_pHostNeedUpdate)
	{
		return;
	}
	COLORREF color(RGB(50, 50, 50));
	int nRoadWidth = sm_fRoadHalfWidth / 2 / GetScale();
	if (nRoadWidth == 0)
	{
		nRoadWidth = 1;
	}
	int nRadius = m_Cfg.m_fCommunicateRadius / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, color);
	CPen penCarry(PS_SOLID, nRoadWidth * 3, RGB(250, 100, 50));
	CPen penInfo(PS_SOLID, nRoadWidth * 6, RGB(00, 50, 150));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush *pOldBrush = pMemDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);
	pMemDC->SetTextColor(RGB(0, 0, 0));
	CFont font;

	font.CreateFont(
		30/GetScale(), // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		FW_NORMAL, // nWeight
		FALSE, // bItalic
		FALSE, // bUnderline
		0, // cStrikeOut
		ANSI_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial") // nPitchAndFamily Arial
	);
	pMemDC->SelectObject(&font);
	pMemDC->SetBkMode(TRANSPARENT);

	int nLength = m_pHostNeedUpdate->GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		CPoint pointCenter;
		CDoublePoint hostPosition = m_pHostNeedUpdate->GetAt(i).m_Position;
		if (hostPosition.m_X < lt.m_X || hostPosition.m_X > rb.m_X)
			continue;
		if (hostPosition.m_Y < lt.m_Y || hostPosition.m_Y > rb.m_Y)
			continue;
		GeographicToGraphicPoint(hostPosition, pointCenter);
		CRoutingProtocol * pProtocol = m_pHostNeedUpdate->GetAt(i).m_pHost->m_pProtocol;
		if (pProtocol->GetInportantLevel() > 1)
		{
			pMemDC->SelectObject(&penInfo);
		}
		else if(pProtocol->GetInportantLevel() > 0)
		{
			pMemDC->SelectObject(&penCarry);
		}
		else
		{
			pMemDC->SelectObject(&pen);
		}
		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
		CString strHost;
		strHost.Format(_T("%d"), m_pHostNeedUpdate->GetAt(i).m_pHost->m_nId);
		int nWidth = 50 / GetScale();
		int nHeight = 30 / GetScale();
		CRect rctText(pointCenter.x - nWidth, pointCenter.y - nHeight, pointCenter.x + nWidth, pointCenter.y + nHeight);
		pMemDC->DrawText(strHost, &rctText, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawHosts_AptCard(CDC * pMemDC)
{
	CDoublePoint lt, rb;
	GetRefreshArea(lt, rb);
	if (!m_pHostNeedUpdate)
	{
		return;
	}
	COLORREF color(RGB(50, 50, 50));
	int nRoadWidth = 2*sm_fRoadHalfWidth / GetScale();
	if (nRoadWidth == 0)
	{
		nRoadWidth = 1;
	}
	int nRadius = m_Cfg.m_fCommunicateRadius / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, color);
	CPen penCarry(PS_SOLID, nRoadWidth * 3, RGB(250, 100, 50));
	CPen penInfo(PS_SOLID, nRoadWidth * 6, RGB(00, 50, 150));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush *pOldBrush = pMemDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);
	pMemDC->SetTextColor(RGB(0, 0, 0));
	CFont font;

	font.CreateFont(
		30 / GetScale(), // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		FW_NORMAL, // nWeight
		FALSE, // bItalic
		FALSE, // bUnderline
		0, // cStrikeOut
		ANSI_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial") // nPitchAndFamily Arial
	);
	pMemDC->SelectObject(&font);
	pMemDC->SetBkMode(TRANSPARENT);

	int nLength = m_pHostNeedUpdate->GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		CPoint pointCenter;
		CDoublePoint hostPosition = m_pHostNeedUpdate->GetAt(i).m_Position;
		if (hostPosition.m_X < lt.m_X || hostPosition.m_X > rb.m_X)
			continue;
		if (hostPosition.m_Y < lt.m_Y || hostPosition.m_Y > rb.m_Y)
			continue;
		GeographicToGraphicPoint(hostPosition, pointCenter);
		CRoutingProtocol * pProtocol = m_pHostNeedUpdate->GetAt(i).m_pHost->m_pProtocol;
		CPen * pSwitchPen = NULL;
		if (nRoadWidth > 2)
		{
			pSwitchPen = new CPen(PS_SOLID, nRoadWidth, pProtocol->GetInportantLevel());
			pMemDC->SelectObject(pSwitchPen);
		}
		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
		CString strHost;
		strHost.Format(_T("%d"), m_pHostNeedUpdate->GetAt(i).m_pHost->m_nId);
		int nWidth = 50 / GetScale();
		int nHeight = 30 / GetScale();
		CRect rctText(pointCenter.x - nWidth, pointCenter.y - nHeight, pointCenter.x + nWidth, pointCenter.y + nHeight);
		pMemDC->DrawText(strHost, &rctText, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		if (pSwitchPen)
		{
			delete pSwitchPen;
			pSwitchPen = NULL;
		}
	}

	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawFollowings(CDC * pMemDC)
{
	if (m_pFollowHost == NULL)
	{
		return;
	}
	CDoublePoint lt, rb;
	GetRefreshArea(lt, rb);
	if (!m_pHostNeedUpdate)
	{
		return;
	}
	COLORREF color(RGB(50, 150, 50));
	int nRoadWidth = 3 * sm_fRoadWidth / GetScale();
	int nRadius = 20 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, color);
	CPen    penSelf(PS_SOLID, nRoadWidth, RGB(50, 150, 150));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush *pOldBrush = pMemDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));

	pMemDC->SetBkMode(TRANSPARENT);

	CMobileSocialNetworkHost * pHost = (CMobileSocialNetworkHost *)m_pFollowHost;
	//CRoutingProtocolHslpo * pProtocol = (CRoutingProtocolHslpo*)pHost->m_pProtocol;

	int nLength = m_pHostNeedUpdate->GetSize();
	int nSelfIndex = -1;
	for (int i = 0; i < nLength; ++i)
	{
		int nHostId = m_pHostNeedUpdate->GetAt(i).m_pHost->m_nId;
		CTrustValue fTrust = 0;
		if (nHostId == m_pFollowHost->m_nId)
		{
			nSelfIndex = i;
		}
		if (!pHost->FindTrust(nHostId, fTrust))
		{
			continue;
		}
		if ((double)fTrust <= m_Cfg.m_fPrivacyHigh / 100.0)
		{
			continue;
		}
		CPoint pointCenter;
		CDoublePoint hostPosition = m_pHostNeedUpdate->GetAt(i).m_Position;
		if (hostPosition.m_X < lt.m_X || hostPosition.m_X > rb.m_X)
			continue;
		if (hostPosition.m_Y < lt.m_Y || hostPosition.m_Y > rb.m_Y)
			continue;
		GeographicToGraphicPoint(hostPosition, pointCenter);

		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
	}

	if (nSelfIndex != -1)
	{
		CPoint pointCenter;
		CDoublePoint hostPosition = m_pHostNeedUpdate->GetAt(nSelfIndex).m_Position;
		GeographicToGraphicPoint(hostPosition, pointCenter);

		pMemDC->SelectObject(&penSelf);
		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
	}

	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawRoadPoint(CDC * pMemDC)
{
	COLORREF color(RGB(175, 136, 77));
	int nRoadWidth = sm_fRoadMarkLineWidth / GetScale();
	int nRadius = sm_fRoadHalfWidth / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, color);
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush brush(color);
	CBrush *pOldBrush = pMemDC->SelectObject(&brush);
	//MemDC.SetBkMode(TRANSPARENT);

	int nPointCount = m_pRoadNet->m_allPoints.GetSize();
	for (int i = 0; i < nPointCount; ++i)
	{
		PtrRoadPoint pPoint = m_pRoadNet->m_allPoints[i];
		CPoint pointCenter;
		GeographicToGraphicPoint(*(CDoublePoint*)pPoint, pointCenter);

		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);

	}
	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawCircleAroundPoints(CDC * pMemDC)
{
	COLORREF color(RGB(175, 136, 77));
	int nRoadWidth = sm_fRoadHalfWidth / GetScale();
	int nRadius = sm_fRoadMarkRadius * 2 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(0, 255, 100));
	CPen*    pOldPen = pMemDC->SelectObject(&pen);
	CBrush *pOldBrush = pMemDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	POSITION pos = m_CircleRoadPoints.GetHeadPosition();
	while (pos != NULL)
	{
		PtrRoadPoint pRoadPoint = m_CircleRoadPoints.GetNext(pos);
		CPoint pointCenter;
		GeographicToGraphicPoint(*(CDoublePoint*)pRoadPoint, pointCenter);

		pMemDC->Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
	}

	pMemDC->SelectObject(pOldPen);
	pMemDC->SelectObject(pOldBrush);
}

void CMapGui::DrawConnectionForMarked(CDC & MemDC, CBitmap & MemBitmap)
{

	int nRoadWidth = 4 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(0, 200, 200));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nInfoCount = m_RoadPointInfos.GetSize();
	for (int i = 0; i < nInfoCount; ++i)
	{
		if (m_RoadPointInfos[i].bMarkedByDoubleClick == false)
		{
			continue;
		}
		//CPen * pTpen = new CPen(PS_SOLID, nRoadWidth, RGB(rand() % 200, rand() % 200, rand() % 200));
		//MemDC.SelectObject(pTpen);

		CPoint pointStart;
		PtrRoadPoint pdStart = (PtrRoadPoint)m_pRoadNet->m_allPoints[i];
		GeographicToGraphicPoint(*pdStart, pointStart);

		PtrIntersection pInter = m_pRoadNet->m_allPoints[i]->GetIntersection();
		if (pInter == NULL)
		{
			continue;
		}
		int nConnectCount = pdStart->m_NearbyIntersections.GetSize();
		for (int j = 0; j < nConnectCount; ++j)
		{
			int nEndId = pdStart->m_NearbyIntersections[j].m_nIntersectionId;
			CDoublePoint * pdEnd = m_pRoadNet->m_allInterSections[nEndId]->m_pPoint;
			ASSERT(pdEnd != NULL);
			CPoint pointEnd;
			GeographicToGraphicPoint(*pdEnd, pointEnd);
			MemDC.MoveTo(pointStart);
			MemDC.LineTo(pointEnd);
		}
		//delete pTpen;

	}

	MemDC.SelectObject(pOldPen);
}

void CMapGui::DrawShortPathDirected(CDC & MemDC, CBitmap & MemBitmap)
{
	if (!m_pRoadNet->m_pDijTable)
		return;

	int nRoadWidth = 4 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(0, 200, 200));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nDijCount = m_pRoadNet->m_pDijTable->m_nNodeCount;
	DijTable * pDij = m_pRoadNet->m_pDijTable;
	for (int i = 0; i < nDijCount; ++i)
	{
		for (int j = 0; j < pDij->m_pDirectNeighboursId[i].GetSize(); ++j)
		{
			int nNeighbourId = pDij->m_pDirectNeighboursId[i][j];
			CDijItem * pItem = &(pDij->m_pMatrix[i*nDijCount + nNeighbourId]);
			if (pItem->m_nNext == nNeighbourId)
			{
				if (!pItem->IsFixed())
				{
					continue;
				}
				CDoublePoint * pdStart = m_pRoadNet->m_allDijNodes[i]->m_pIntersection->m_pPoint;
				CDoublePoint * pdEnd = m_pRoadNet->m_allDijNodes[nNeighbourId]->m_pIntersection->m_pPoint;
				CPoint pointStart;
				GeographicToGraphicPoint(*pdStart, pointStart);
				CPoint pointEnd;
				GeographicToGraphicPoint(*pdEnd, pointEnd);
				MemDC.MoveTo(pointStart);
				MemDC.LineTo(pointEnd);
			}
		}

	}

	MemDC.SelectObject(pOldPen);
}

void CMapGui::DrawShortPathPair(CDC & MemDC, CBitmap & MemBitmap)
{
	int nNodeCount = m_RoadPointInfos.GetSize();
	for (int i = 0; i < nNodeCount; ++i)
	{
		if (m_RoadPointInfos[i].bMarkedByDoubleClick == false)
		{
			continue;
		}
		for (int j = i; j < nNodeCount; ++j)
		{
			if (m_RoadPointInfos[j].bMarkedByDoubleClick == false)
			{
				continue;
			}
			int nA = m_pRoadNet->m_allPoints[i]->GetIntersection()->GetDij()->m_nID;
			int nB = m_pRoadNet->m_allPoints[j]->GetIntersection()->GetDij()->m_nID;
			DrawOneSingleShortPath(MemDC, MemBitmap, nA, nB);
		}
	}
}

void CMapGui::DrawOneSingleShortPath(CDC & MemDC, CBitmap & MemBitmap, int nFromId, int nToId)
{
	if (!m_pRoadNet->m_pDijTable)
		return;

	int nRoadWidth = 10 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(0, 50, 150));
	CPen*    pOldPen = MemDC.SelectObject(&pen);

	if (nFromId == nToId)
	{
		return;
	}
	int nPrevId = nFromId;
	DijTable * pDijTable = m_pRoadNet->m_pDijTable;
	int nDijCount = pDijTable->m_nNodeCount;
	int nRecentId = nFromId;
	while (nRecentId != nToId)
	{
		nRecentId = pDijTable->m_pMatrix[nPrevId*nDijCount + nToId].m_nNext;
		if (nRecentId == -1)
		{
			break;
		}
		CDoublePoint * pdStart = m_pRoadNet->m_allDijNodes[nPrevId]->m_pIntersection->m_pPoint;
		CDoublePoint * pdEnd = m_pRoadNet->m_allDijNodes[nRecentId]->m_pIntersection->m_pPoint;
		CPoint pointStart;
		GeographicToGraphicPoint(*pdStart, pointStart);
		CPoint pointEnd;
		GeographicToGraphicPoint(*pdEnd, pointEnd);
		MemDC.MoveTo(pointStart);
		MemDC.LineTo(pointEnd);
		nPrevId = nRecentId;
	}
	MemDC.SelectObject(pOldPen);
}

void CMapGui::DrawDijConnections(CDC & MemDC, CBitmap & MemBitmap)
{
	int nRoadWidth = 6 / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(210, 170, 200));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nDij = m_pRoadNet->m_allDijNodes.GetSize();
	int nRadius = GetIntersectionRadius();
	for (int i = 0; i < nDij; ++i)
	{
		PtrDijNode pDij = m_pRoadNet->m_allDijNodes[i];
		CPoint ptFrom;
		GeographicToGraphicPoint(*pDij->m_pIntersection->m_pPoint, ptFrom);
		int nConnectCount = pDij->m_DijToDij.GetSize();
		for (int j = 0; j < nConnectCount; ++j)
		{
			PtrDijNode pNext = pDij->m_DijToDij[j].GetLastDijNode();
			ASSERT(pNext);
			CPoint ptTo;
			GeographicToGraphicPoint(*pNext->m_pIntersection->m_pPoint, ptTo);
			MemDC.MoveTo(ptFrom);
			MemDC.LineTo(ptTo);
			
		}
	}
	MemDC.SelectObject(pOldPen);
	//MemDC.SelectObject(pOldBrush);
}

void CMapGui::DrawAllBuildings(CDC & MemDC, CBitmap & MemBitmap)
{
	CPen    penCommence(PS_SOLID, 1, RGB(0, 0, 200));
	CBrush brushApartment(RGB(200, 200, 250));
	CPen*    pOldPen = MemDC.SelectObject(&penCommence);
	CBrush bursh(RGB(0, 0, 200));
	CBrush *pOldBrush = MemDC.SelectObject(&bursh);
	//MemDC.SetBkMode(TRANSPARENT);

	int nBuildingCount = m_pRoadNet->m_allBuildings.GetSize();
	int nRadius = GetIntersectionRadius()/3.0+1;
	for (int i = 0; i < nBuildingCount; ++i)
	{
		CBuilding * pPoint = m_pRoadNet->m_allBuildings[i];
		if (pPoint->IsApartment())
		{
			MemDC.SelectObject(&brushApartment);
		}
		else
		{
			MemDC.SelectObject(&bursh);
		}
		CPoint pointCenter;
		GeographicToGraphicPoint(*(CDoublePoint*)pPoint, pointCenter);

		MemDC.Ellipse(pointCenter.x - nRadius, pointCenter.y - nRadius, pointCenter.x + nRadius, pointCenter.y + nRadius);
	}
	MemDC.SelectObject(pOldPen);
	MemDC.SelectObject(pOldBrush);
}

void CMapGui::DrawBuildingsToRoads(CDC & MemDC, CBitmap & MemBitmap)
{
	int nRoadWidth = sm_fRoadHalfWidth / GetScale();
	CPen    pen(PS_SOLID, nRoadWidth, RGB(50, 250, 50));
	CPen*    pOldPen = MemDC.SelectObject(&pen);
	//CBrush *pOldBrush = MemDC.SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	//MemDC.SetBkMode(TRANSPARENT);

	int nBuildingCount = m_pRoadNet->m_allBuildings.GetSize();
	for (int i = 0; i < nBuildingCount; ++i)
	{
		CPoint pointStart;
		CBuilding * pRecent = m_pRoadNet->m_allBuildings[i];
		GeographicToGraphicPoint(*((CDoublePoint*)pRecent), pointStart);
		int nConnect = pRecent->m_pRoadPoints.GetSize();
		for (int j = 0; j < nConnect; ++j)
		{
			CPoint pointEnd;
			PtrRoadPoint pOther = pRecent->m_pRoadPoints[j];
			GeographicToGraphicPoint(*((CDoublePoint*)pOther), pointEnd);
			MemDC.MoveTo(pointStart);
			MemDC.LineTo(pointEnd);
		}
	}

	MemDC.SelectObject(pOldPen);
}

void CMapGui::DrawExtreInImage()
{
	CPictureBackup * pBackup = GetBackup(m_nScaleIndex, m_nBackupVersion);

	CDC * pMemDc = CDC::FromHandle(pBackup->m_Image.GetDC());
	//DrawSchedules(pMemDc);
	//DrawCircleAroundPoints(pMemDc);
	DrawFollowings(pMemDc);
	DrawHosts(pMemDc);
	pBackup->m_Image.ReleaseDC();
}

void CMapGui::GeographicToGraphicPoint(const CDoublePoint & actualPoint, CPoint & showPoint)
{
	CDoublePoint tmpPoint;
	tmpPoint.m_X = actualPoint.m_X - m_LeftTop.m_X + sm_fMargin;
	tmpPoint.m_Y = actualPoint.m_Y - m_LeftTop.m_Y + sm_fMargin;
	tmpPoint.m_X /= GetScale();
	tmpPoint.m_Y /= GetScale();
	showPoint.x = tmpPoint.m_X;
	showPoint.y = tmpPoint.m_Y;
}

void CMapGui::GraphicToGeographicPoint(const CPoint & showPoint, CDoublePoint & actualPoint)
{
	CDoublePoint tmpPoint;
	tmpPoint.m_X = showPoint.x;
	tmpPoint.m_Y = showPoint.y;
	tmpPoint.m_X *= GetScale();
	tmpPoint.m_Y *= GetScale();
	actualPoint.m_X = tmpPoint.m_X + m_LeftTop.m_X - sm_fMargin;
	actualPoint.m_Y = tmpPoint.m_Y + m_LeftTop.m_Y - sm_fMargin;
}

int CMapGui::GetIntersectionRadius()
{
	return m_nIntersectionRadius;
}

CDoublePoint CMapGui::GetMousePosInMapScale(CPoint pt)
{
	CPoint ptnScroll = GetScrollPosition();
	CPoint ptClient = pt;
	ScreenToClient(&ptClient);
	m_LastMousePosition = ptClient;
	if (m_sizeTotal.cx > 0 && m_sizeTotal.cy > 0)
	{
		m_MousePosScale.m_X = 1.0*(ptnScroll.x + ptClient.x) / m_sizeTotal.cx;
		m_MousePosScale.m_Y = 1.0*(ptnScroll.y + ptClient.y) / m_sizeTotal.cy;
		if (m_MousePosScale.m_X > 1)
			m_MousePosScale.m_X = 1;
		if (m_MousePosScale.m_Y > 1)
			m_MousePosScale.m_Y = 1;
	}
	else
	{
		m_MousePosScale.m_X = 0;
		m_MousePosScale.m_Y = 0;
	}
	return m_MousePosScale;
}

CDoublePoint CMapGui::ScreenToMapPoint(CPoint pt)
{
	CDoublePoint ret;
	CPoint ptnScroll = GetScrollPosition();
	CPoint ptClient = pt;
	//ScreenToClient(&ptClient);
	ptClient.x += ptnScroll.x;
	ptClient.y += ptnScroll.y;
	ret.m_X = m_LeftTop.m_X + (m_RightBottom.m_X - m_LeftTop.m_X + 2 * sm_fMargin)*ptClient.x / m_sizeTotal.cx - sm_fMargin;
	ret.m_Y = m_LeftTop.m_Y + (m_RightBottom.m_Y - m_LeftTop.m_Y + 2 * sm_fMargin)*ptClient.y / m_sizeTotal.cy - sm_fMargin;
	return ret;
}

CPoint CMapGui::GetNewScrollPos()
{
	CPoint ret;
	ret.x = m_sizeTotal.cx * m_MousePosScale.m_X - m_LastMousePosition.x;
	ret.y = m_sizeTotal.cy * m_MousePosScale.m_Y - m_LastMousePosition.y;
	return ret;
}

void CMapGui::CreateRandomPackages(int nNumber, SIM_TIME lnTimeOut)
{
	int nNum = nNumber;
	int nHostCount = m_pRoadNet->m_allHosts.GetSize() - SERVER_NODE_COUNT;
	CQueryMission mission;
	CHost * pHostFrom = NULL;
	CHost * pHostTo = m_pRoadNet->m_allHosts.GetAt(0);
	SIM_TIME lnCurrentST = m_pEngine->GetSimTime();
	SIM_TIME lnTimeEnd = lnCurrentST + lnTimeOut;
	m_lnExpectEndTime = lnTimeEnd + 10000;

	m_pEngine->GetSummary().StartTest(lnCurrentST, lnTimeEnd, 1 * 60 * 1000);
	char * pEmpty = new char[nHostCount];
	CCommonFunctions::PickMFromNDisorder(nNum, pEmpty, nHostCount);

	for (int i = SERVER_NODE_COUNT; i < nHostCount + SERVER_NODE_COUNT; ++i)
	{
		if (pEmpty[i - SERVER_NODE_COUNT] == 1)
		{
			pHostFrom = m_pRoadNet->m_allHosts[i];
			mission.m_SenderId = pHostFrom->m_nId;
			mission.m_RecverId = pHostTo->m_nId;
			mission.m_lnTimeOut = lnTimeEnd;
			mission.ChangeID();
			pHostFrom->m_pProtocol->CreateQueryMission(&mission);
		}
	}
	delete[] pEmpty;
	CString strLog;
	strLog.Format(_T("Start:%d To %d"), lnCurrentST, lnTimeEnd);
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->WriteLog(strLog);
}

void CMapGui::BitBlt(CDC * pDC)
{
	CPictureBackup * pBackup = GetBackup(m_nScaleIndex, m_nBackupVersion);
	DrawExtreInImage();
	ULONGLONG start = GetTickCount64();
	pBackup->BitBlt(pDC, m_MemDC);
	ULONGLONG end = GetTickCount64();
	ASSERT(end - start < 60);
}

CPictureBackup * CMapGui::GetBackup(int nScaleIndex, int nBackupVersion)
{
	CPictureBackup * pBackup = m_Backup[nScaleIndex];
	if (pBackup->GetVersion() == nBackupVersion)
	{
		CRect refreshArea = GetRefreshArea();
		pBackup->Load(refreshArea.left, refreshArea.top, refreshArea.Width(), refreshArea.Height());
	}
	else
	{
		pBackup->FillSolidRect(m_BackgroundColor);

		CDC * pInsideDc = pBackup->GetInsideDc();

		DrawRoads(pInsideDc);
		DrawRoadPoint(pInsideDc);
		DrawAllDijNodes(pInsideDc);

		pBackup->ReleaseInsideDc();

		pBackup->Save(nBackupVersion);
	}
	return pBackup;
}

CRect CMapGui::GetRefreshArea()
{
	int nMargin = 200;
	int nWidth = 0, nHeight = 0;
	GetBackupPictureRange(nWidth, nHeight, m_nScaleIndex);
	CPoint ptnScroll = GetScrollPosition();
	CRect ret;
	if (ptnScroll.x < nMargin)
	{
		ret.left = 0;
	}
	else
	{
		ret.left = ptnScroll.x - nMargin;
	}
	if (ptnScroll.y < nMargin)
	{
		ret.top = 0;
	}
	else
	{
		ret.top = ptnScroll.y - nMargin;
	}
	CRect rctClient;
	GetClientRect(&rctClient);
	ret.right = ptnScroll.x + rctClient.Width() + nMargin;
	ret.bottom = ptnScroll.y + rctClient.Height() + nMargin;
	if (ret.right > nWidth)
	{
		ret.right = nWidth;
	}
	if (ret.bottom > nHeight)
	{
		ret.bottom = nHeight;
	}
	return ret;
}

void CMapGui::GetRefreshArea(CDoublePoint & lt, CDoublePoint & rb)
{
	CRect rctShow = GetRefreshArea();
	GraphicToGeographicPoint(rctShow.TopLeft(), lt);
	GraphicToGeographicPoint(rctShow.BottomRight(), rb);
}

void CMapGui::InitBackup()
{
	m_fScales.RemoveAll();
	//m_fScales.Add(0.5);
	m_fScales.Add(1);
	m_fScales.Add(2);
	m_fScales.Add(4);
	m_fScales.Add(8);
	m_Backup.SetSize(m_fScales.GetSize());
	CDC * pDC = GetDC();
	for (int i = 0; i < m_fScales.GetSize(); ++i)
	{
		m_Backup[i] = new CPictureBackup();
		int nWidth = 0, nHeight = 0;
		GetBackupPictureRange(nWidth, nHeight, i);
		m_Backup[i]->Init(nWidth, nHeight, pDC);
	}
	m_nBackupVersion = 1;
	ReleaseDC(pDC);
}

void CMapGui::DestroyBackup()
{
	for (int i = 0; i < m_fScales.GetSize(); ++i)
	{
		delete m_Backup[i];
	}
	m_Backup.RemoveAll();
}

double CMapGui::GetScale()
{
	return m_fScales[m_nScaleIndex];
}

CHost * CMapGui::GetNearestHost(const CDoublePoint & hostPos)
{
	double fDistance = DBL_MAX;
	CHost * pRet = NULL;
	int nLength = m_pHostNeedUpdate->GetSize();
	for (int i = 0; i < nLength; ++i)
	{
		CPoint pointCenter;
		CDoublePoint hostPosition = m_pHostNeedUpdate->GetAt(i).m_Position;
		double fTmpDis = CDoublePoint::GetDistance(hostPos, hostPosition);
		if (fTmpDis < fDistance)
		{
			fDistance = fTmpDis;
			pRet = m_pHostNeedUpdate->GetAt(i).m_pHost;
		}
	}
	return pRet;
}

BEGIN_MESSAGE_MAP(CMapGui, CScrollView)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(MARKFINISHEDDIJNODES, &CMapGui::OnMarkfinisheddijnodes)
	ON_MESSAGE(MSG_ID_MAP_UPDATE_HOSTS, &CMapGui::OnUpdateHosts)
	ON_MESSAGE(MSG_ID_INIT_OK, &CMapGui::OnAllInitOk)
	ON_MESSAGE(MSG_ID_WRITE_LOG, &CMapGui::OnWriteLog)
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BUILDING_OPEN, &CMapGui::OnBuildingOpen)
	ON_COMMAND(ID_ENGINE_RUN, &CMapGui::OnEngineRun)
	ON_COMMAND(ID_ENGINE_PAUSE, &CMapGui::OnEnginePause)
	ON_COMMAND(ID_ENGINE_RESET, &CMapGui::OnEngineReset)
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CMapGui 绘图

void CMapGui::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO:  计算此视图的合计大小
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);

	m_MemDC.CreateCompatibleDC(NULL);
	InitBackup();
	SetScale(2);
}

void CMapGui::GetResonableRange(int & nWidth, int & nHeight)
{
	double diffX = m_RightBottom.m_X - m_LeftTop.m_X, diffY = m_RightBottom.m_Y - m_LeftTop.m_Y;
	nWidth = diffX / GetScale();
	nHeight = diffY / GetScale();
}

void CMapGui::GetBackupPictureRange(int & nWidth, int & nHeight, int nScaleIndex)
{
	double fScale = m_fScales[nScaleIndex];
	double diffX = 2 * sm_fMargin + m_RightBottom.m_X - m_LeftTop.m_X;
	double diffY = 2 * sm_fMargin + m_RightBottom.m_Y - m_LeftTop.m_Y;
	nWidth = diffX / fScale;
	nHeight = diffY / fScale;
}

void CMapGui::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO:  在此添加绘制代码
	BitBlt(pDC);
	ULONGLONG llDrawTime = GetTickCount64();
	m_llLastDrawTime = llDrawTime;
}


// CMapGui 诊断

#ifdef _DEBUG
void CMapGui::AssertValid() const
{
	//CScrollView::AssertValid();
}

#ifndef _WIN32_WCE
void CMapGui::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMapGui 消息处理程序


BOOL CMapGui::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nFlags & MK_CONTROL)
	{
		int nWheelCount = zDelta / WHEEL_DELTA;
		int nScaleIndex = m_nScaleIndex;
		if (nWheelCount > 0)
		{
			while (nWheelCount > 0)
			{
				if (nScaleIndex == 0)
				{
					break;
				}
				--nScaleIndex;
				--nWheelCount;
			}
		}
		else
		{
			while (nWheelCount < 0)
			{
				if (nScaleIndex >= m_fScales.GetSize() - 1)
				{
					break;
				}
				++nScaleIndex;
				++nWheelCount;
			}
		}
		if (nScaleIndex != m_nScaleIndex)
		{
			GetMousePosInMapScale(pt);
			SetScale(nScaleIndex);
		}
	}

	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}


BOOL CMapGui::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CScrollView::OnScroll(nScrollCode, nPos, bDoScroll);
}


void CMapGui::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDoublePoint dp = ScreenToMapPoint(point);
	CHost * pHost = GetNearestHost(dp);
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CHostPropertyParam * pNewShowMsg = new CHostPropertyParam();
	pNewShowMsg->m_pHost = pHost;
	pNewShowMsg->m_pEngine = m_pEngine;
	pMainFrame->ShowHostInfo(pNewShowMsg);
	m_pFollowHost = pHost;
	RefreshUi(false);

#if 0
	bool bPaused = m_bEnginePaused;
	if (!bPaused)
	{
		OnEnginePause();
	}
	CDoublePoint dp = ScreenToMapPoint(point);
	CHost * pHost = GetNearestHost(dp);
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CHostPropertyParam * pNewShowMsg = new CHostPropertyParam();
	pNewShowMsg->m_pHost = pHost;
	pNewShowMsg->m_pEngine = m_pEngine;
	pMainFrame->ShowHostInfo(pNewShowMsg);
	CDlgHostInfo dlg;
	dlg.SetData(pHost, m_pRoadNet);
	if (IDOK == dlg.DoModal())
	{
		if (dlg.m_bFollow)
		{
			m_pFollowHost = pHost;
		}
		RefreshUi(false);
	}
	if (!bPaused)
	{
		OnEngineRun();
	}
	
	return;
#endif

#if 0
	CDoublePoint dp = ScreenToMapPoint(point);
	int nPointCount = m_pRoadNet->m_allPoints.GetSize();
	bool bFindMatch = false;
	double fMin = DBL_MAX;
	int nMin = 0;
	PtrRoadPoint pSelected = m_pRoadNet->FindNearestRoadPoint(dp);
	m_CircleRoadPoints.RemoveAll();
	m_CircleRoadPoints.AddHead(pSelected);
	int nSelectedId = pSelected->GetId();
	if (MK_CONTROL & nFlags)
	{
		m_nLastSelectedId = nSelectedId;
		m_RoadPointInfos[nSelectedId].bMarkedByDoubleClick = true;
	}
	else
	{
		m_RoadPointInfos[nSelectedId].bMarkedByDoubleClick = !m_RoadPointInfos[nSelectedId].bMarkedByDoubleClick;
		if (m_RoadPointInfos[nSelectedId].bMarkedByDoubleClick == true && m_nLastSelectedId != -1)
		{
			PtrRoadPoint pA = m_pRoadNet->m_allPoints[m_nLastSelectedId];
			PtrRoadPoint pB = m_pRoadNet->m_allPoints[nSelectedId];
			//m_pRoadNet->RouteInsideRoad(m_pRoadNet->m_allHosts[0], pA, pB, m_pRoadNet->m_allHosts[0].m_schedule);
			CRoadRouteAgency A,B;
			A.m_pIntersection = pA->GetIntersection();
			B.m_pIntersection = pB->GetIntersection();
			//m_pRoadNet->RouteAgencyToPoint(m_pRoadNet->m_allHosts[0], A, pB, m_pRoadNet->m_allHosts[0].m_schedule);
			//m_pRoadNet->RoutePointToAgency(m_pRoadNet->m_allHosts[0], pA, B, m_pRoadNet->m_allHosts[0].m_schedule);
			m_pRoadNet->RouteTwoRoadPoints(m_pRoadNet->m_allHosts[0], pA, pB, m_pRoadNet->m_allHosts[0].m_schedule);
			m_pEngine->PostThreadMessage(MSG_ID_ENGINE_START, 0, 0);
		}
	}
	RefreshUi(false);
#endif
#if 0
	for (int i = 0; i < nPointCount; ++i)
	{
		double fDistance = CDoublePoint::GetDistance(dp, *m_pRoadNet->m_allPoints[i]);
		if (fDistance < GetIntersectionRadius() * 5)
		{
			if (m_pRoadNet->m_allPoints[i]->IsIntersection() && m_pRoadNet->m_allPoints[i]->GetIntersection()->IsDijNode())
			{
				if (fMin > fDistance)
				{
					fMin = fDistance;
					nMin = i;
					bFindMatch = true;
				}
			}
		}
	}
	if (bFindMatch)
	{
		m_RoadPointInfos[nMin].bMarkedByDoubleClick = !m_RoadPointInfos[nMin].bMarkedByDoubleClick;
		RefreshUi(false);
	}
#endif

	CScrollView::OnLButtonDblClk(nFlags, point);
}


afx_msg LRESULT CMapGui::OnMarkfinisheddijnodes(WPARAM wParam, LPARAM lParam)
{
	static int nLastPercentage = 0;
	int nNodeId = m_pRoadNet->m_allDijNodes[wParam]->m_pIntersection->m_pPoint->GetId();
	ASSERT(m_RoadPointInfos[nNodeId].bIsDijAlgorithmFinished == false);
	m_RoadPointInfos[nNodeId].bIsDijAlgorithmFinished = true;
	m_nDijFinishedCount++;
	if (nLastPercentage != m_nDijFinishedCount * 10 / m_pRoadNet->m_allDijNodes.GetSize())
	{
		nLastPercentage = m_nDijFinishedCount * 10 / m_pRoadNet->m_allDijNodes.GetSize();
		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CString strOut;
		strOut.Format(_T("Dijkstra: %%%d0"), nLastPercentage);
		pMainFrame->WriteLog(strOut);
	}
	if (m_nDijFinishedCount == m_pRoadNet->m_allDijNodes.GetSize())
	{
		CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CString strOut;
		strOut.Format(_T("Dijkstra Finished"));
		pMainFrame->WriteLog(strOut);
		//pMainFrame->m_wndOutput
	}

	return 0;
}


LRESULT CMapGui::OnUpdateHosts(WPARAM wParam, LPARAM lParam)
{
	CArray<CHostGui> * pParam = (CArray<CHostGui> *) wParam;
	if (m_pHostNeedUpdate)
	{
		delete m_pHostNeedUpdate;
	}
	m_pHostNeedUpdate = pParam;
	RefreshUi(false);
	return 0;
}

LRESULT CMapGui::OnAllInitOk(WPARAM wParam, LPARAM lParam)
{
	m_BackgroundColor = RGB(234, 234, 234);
	RefreshUi(true);
	OnEngineRun();
	//SetTimer(MAP_GUI_TIMER_ID_CHECK_WORKING, 5000, NULL);
	if (wParam != 0)
	{
		SetTimer(MAP_GUI_TIMER_ID_START_TEST, 1000, NULL);
		SetTimer(MAP_GUI_TIMER_ID_CHECK_TIMEOUT, 5000, NULL);
	}
	return 0;
}

LRESULT CMapGui::OnWriteLog(WPARAM wParam, LPARAM lParam)
{
	CString * pstrLog = (CString *)lParam;
	if (!pstrLog)
	{
		return 0;
	}
	CMainFrame * pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->WriteLog(*pstrLog);
	delete pstrLog;
	return 0;
}

void CMapGui::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case MAP_GUI_TIMER_ID_CHECK_TIMEOUT:
	{
		if (m_lnExpectEndTime != 0)
		{
			if (m_pEngine->GetSimTime() > m_lnExpectEndTime)
			{
				KillTimer(nIDEvent);
				m_pView->PostMessage(MSG_ID_TEST_COMPLETE, 0, 0);
			}
		}
		break;
	}
	case MAP_GUI_TIMER_ID_START_TEST:
	{
		if (m_pEngine->GetSimTime() > START_TEST_SIM_TIME)
		{
			KillTimer(nIDEvent);
			CreateRandomPackages(100, m_Cfg.m_nTimeOutSecond * 1000);
		}
		break;
	}
	}

	CScrollView::OnTimer(nIDEvent);
}


void CMapGui::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
	CMenu popMenu;
	popMenu.LoadMenu(IDR_MENU_MAPGUI);
	CPoint posMouse;
	GetCursorPos(&posMouse);
	CMenu* pSubMenu = NULL;
	pSubMenu = popMenu.GetSubMenu(0);
	if (pSubMenu != NULL)
	{
		pSubMenu->TrackPopupMenu(0, posMouse.x, posMouse.y, this);
	}
}

void CMapGui::OnBuildingOpen()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(TRUE);

	CString strFilename;

	if (dlg.DoModal() == IDOK)
	{
		strFilename = dlg.GetPathName(); // return full path and filename

		int len = WideCharToMultiByte(CP_ACP, 0, strFilename, -1, NULL, 0, NULL, NULL);
		char *filename = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, filename, len, NULL, NULL);
		
		m_pRoadNet->LoadBuildingSettings(filename);
		m_pRoadNet->ConnectBuildingsToRoads();
		delete[] filename;
		RefreshUi(false);
	}
}


void CMapGui::OnEngineRun()
{
	m_pEngine->PostThreadMessage(MSG_ID_ENGINE_START, 0, 0);
	m_bEngineShouldBeRunning = true;
	m_bEnginePaused = false;
}

void CMapGui::OnEngineReset()
{
	m_pEngine->PostThreadMessage(MSG_ID_ENGINE_RESET, 0, 0);
	m_bEngineShouldBeRunning = false;
}

void CMapGui::OnEnginePause()
{
	m_pEngine->PostThreadMessage(MSG_ID_ENGINE_PAUSE, 0, 0);
	m_bEnginePaused = true;
}

BOOL CMapGui::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;

	return CScrollView::OnEraseBkgnd(pDC);
}


void CMapGui::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_pEngine->BreakMapGui();
	CScrollView::OnClose();
}


void CMapGui::OnDestroy()
{
	CScrollView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CMapGui::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	switch (nChar)
	{
	case 'P':
	{
		if (m_bEnginePaused)
		{
			m_pEngine->PostThreadMessage(MSG_ID_ENGINE_RESUME, 0, 0);
			m_bEnginePaused = false;
		}
		else
		{
			OnEnginePause();
		}
		break;
	}
	// +
	case 187:
	{
		m_pEngine->PostThreadMessage(MSG_ID_ENGINE_SPEED_UP, 0, 0);
		break;
	}
	// -
	case 189:
	{
		m_pEngine->PostThreadMessage(MSG_ID_ENGINE_SPEED_DOWN, 0, 0);
		break;
	}
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
