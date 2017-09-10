#include "stdafx.h"
#include "DijTable.h"
#include "DijItem.h"
#include "DijNode.h"
#include "DijConnection.h"
#include <fstream>

using namespace std;

//#define OUT_PUT_DEBUG_INFO


DijTable::DijTable()
	:m_pMatrix(NULL)
	,m_nNodeCount(0)
	, m_pRecords(NULL)
	, m_pFixedIndex(NULL)
	, m_pFixedNearNeighbourIds(NULL)
{
}


DijTable::~DijTable()
{
	m_nFixedCount = 0;
	delete[] m_pFixedIndex;
	delete[] m_pRecords;

	delete[] m_pFixedNearNeighbourIds;
}

void DijTable::Init(const CArray<PtrDijNode> & m_allDijNodes)
{
	bool bOutPutSrc = true;
	m_nNodeCount = m_allDijNodes.GetSize();
	if (m_nNodeCount == 0)
	{
		return;
	}
	m_pMatrix = new CDijItem[m_nNodeCount*m_nNodeCount];
	m_pDirectNeighboursId = new CArray<int>[m_nNodeCount];

	m_pRecords = new CDijRecord[m_nNodeCount];

	m_pFixedIndex = new int[m_nNodeCount];
	m_pFixedNearNeighbourIds = new int[m_nNodeCount];

	m_BackRoad.Init(m_nNodeCount);

	for (int i = 0; i < m_nNodeCount; ++i)
	{
		ASSERT(m_allDijNodes[i]->m_nID == i);
		int nDijDij = m_allDijNodes[i]->m_DijToDij.GetSize();
		for (int j = 0; j < nDijDij; ++j)
		{
			int nOther = m_allDijNodes[i]->m_DijToDij[j].GetLastDijNode()->m_nID;
			m_pMatrix[i*m_nNodeCount + nOther].SetValue(m_allDijNodes[i]->m_DijToDij[j]);
			m_pDirectNeighboursId[i].Add(nOther);
		}
		m_pMatrix[i*m_nNodeCount + i].SetValue(i, i, i, 0);
		m_pMatrix[i*m_nNodeCount + i].SetFixed();
	}

	ofstream fOut;
	if (bOutPutSrc)
		fOut.open("a.txt");
	for (int i = 0; i < m_nNodeCount; ++i)
	{
		for (int j = 0; j < m_nNodeCount; ++j)
		{
			ASSERT(fabs(m_pMatrix[i*m_nNodeCount + j].m_fDistance - m_pMatrix[j*m_nNodeCount + i].m_fDistance) < DOUBLE_COMPARE_LIMIT);
			m_pMatrix[j*m_nNodeCount + i].m_fDistance = m_pMatrix[i*m_nNodeCount + j].m_fDistance;
			if (m_pMatrix[i*m_nNodeCount + j].m_fDistance < DBL_MAX)
			{
				if (bOutPutSrc)
					fOut << "[" << i << "]:\t[" << j << "]\t" << m_pMatrix[i*m_nNodeCount + j].m_fDistance << endl;
			}
		}
		if (bOutPutSrc)
			fOut << endl;
	}
	if (bOutPutSrc)
		fOut.close();
}

void DijTable::RunDijOn(int nBeginId)
{
	memset(m_pFixedIndex, -1, sizeof(int)*m_nNodeCount);
	m_BackRoad.Reset();
	m_nFixedCount = 0;

	SearchFixed(nBeginId);

#ifdef OUT_PUT_DEBUG_INFO
	OutputDebugString(_T("\nCopy Exist End!!"));
#endif

	int nNewNext = -1;
	do
	{
		nNewNext = RunDijStep(nBeginId);
	} while (nNewNext != -1);

	MarkRemainUnknown(nBeginId);
}

int DijTable::RunDijStep(int nBegin)
{
	int nNextIndex = UpdateShortest(nBegin);
	if (nNextIndex != -1)
	{
		int nNextId = m_pRecords[nNextIndex].m_nShortestId;
		int nNextPrevId = m_pRecords[nNextIndex].m_nId;
		AddOneNodeToALine(nNextId, nNextPrevId, nBegin);
		InsertOneNew(nNextId, nNextIndex);
		RemarkRecords(nNextId);
	}
	return nNextIndex;
}

void DijTable::InsertOneNew(int nNewOneId, int nPrevIndex)
{
	int nPrevId = -1;
	if (nPrevIndex >= 0)
	{
		nPrevId = m_pRecords[nPrevIndex].m_nId;
	}

	m_pRecords[m_nFixedCount].SetValue(nNewOneId, MARK_NEED_CALCULATE_SHORTEST, nPrevIndex, -1, DBL_MAX);
	m_pFixedIndex[nNewOneId] = m_nFixedCount;
	++m_nFixedCount;
}

void DijTable::BuildSimpleLine(int nAId, int nANextId, int nBId, int nBPrevId, double fDistance)
{
#ifdef OUT_PUT_DEBUG_INFO
	CString strOut;
	strOut.Format(_T("\nBuild:%d->%d\t%d"), nAId, nBId, nANextId);
	OutputDebugString(strOut);
	strOut.Format(_T("\nBuild:%d<-%d\t%d"), nAId, nBId, nBPrevId);
	OutputDebugString(strOut);
#endif
	ASSERT(m_pMatrix[nAId*m_nNodeCount + nBId].IsFixed() == m_pMatrix[nBId*m_nNodeCount + nAId].IsFixed());
	m_pMatrix[nAId*m_nNodeCount + nBId].SetValue(nAId, nBId, nANextId, fDistance);
	m_pMatrix[nAId*m_nNodeCount + nBId].SetFixed();
	m_pMatrix[nBId*m_nNodeCount + nAId].SetValue(nBId, nAId, nBPrevId, fDistance);
	m_pMatrix[nBId*m_nNodeCount + nAId].SetFixed();
}

int DijTable::UpdateShortest(int nBegin)
{
	int nRet = -1;
	float fMin = FLT_MAX;
	for (int i = 0; i < m_nFixedCount; ++i)
	{
		if (m_pRecords[i].m_nMarked == MARK_NEED_CALCULATE_SHORTEST)
		{
			GetUnsureShortest(nBegin, i);
			m_pRecords[i].m_nMarked = MARK_NOTHING_TO_DO;
		}

		int nTmpShortestId = m_pRecords[i].m_nShortestId;
		if (nTmpShortestId == -1)
		{
			continue;
		}

		float fRecentDistance = m_pMatrix[nBegin*m_nNodeCount + nTmpShortestId].m_fDistance;
		ASSERT(!m_pMatrix[nBegin*m_nNodeCount + nTmpShortestId].IsFixed());
		if (fMin > m_pRecords[i].m_fShortestValue)
		{
			fMin = m_pRecords[i].m_fShortestValue;
			nRet = i;
		}
	}
	return nRet;
}

int DijTable::GetUnsureShortest(int nBegin, int nCalcuRecordIndex)
{
	int nMinId = -1;
	double fMin = DBL_MAX;

	int nCalcuCenterId = m_pRecords[nCalcuRecordIndex].m_nId;
	int nStartItemIndex = nCalcuCenterId * m_nNodeCount;

	int nNeighbourCount = m_pDirectNeighboursId[nCalcuCenterId].GetSize();
	for (int i = 0; i < nNeighbourCount; ++i)
	{
		int nNeighbourId = m_pDirectNeighboursId[nCalcuCenterId][i];
		if (m_pFixedIndex[nNeighbourId] >= 0)
		{
			continue;
		}

		int nNeighbourItemIndex = nStartItemIndex + nNeighbourId;

		if (m_pMatrix[nNeighbourItemIndex].m_nNext != m_pMatrix[nNeighbourItemIndex].m_nTo)
		{
			continue;
		}

		if (m_pMatrix[nNeighbourItemIndex].m_fDistance < fMin)
		{
			fMin = m_pMatrix[nNeighbourItemIndex].m_fDistance;
			nMinId = nNeighbourId;
		}
	}
	double fDistanceToBegin = 0;
	if (nCalcuCenterId != nBegin)
	{
		fDistanceToBegin = m_pMatrix[nCalcuCenterId*m_nNodeCount + nBegin].m_fDistance;
	}
	m_pRecords[nCalcuRecordIndex].m_nShortestId = nMinId;
	m_pRecords[nCalcuRecordIndex].m_fShortestValue = fMin + fDistanceToBegin;
	return nMinId;
}

void DijTable::RemarkRecords(int nNext)
{
	for (int i = 0; i < m_nFixedCount; ++i)
	{
		if (m_pRecords[i].m_nShortestId == nNext)
		{
			m_pRecords[i].m_nMarked = MARK_NEED_CALCULATE_SHORTEST;
			m_pRecords[i].m_nShortestId = ILLEGAL_ID;
			m_pRecords[i].m_fShortestValue = DBL_MAX;
		}
	}
}

int DijTable::GetFixedNeighbours(int nFromId)
{
	m_nFixedNearNeighbourCount = 0;
	int nNeighbourCount = m_pDirectNeighboursId[nFromId].GetSize();
	int nStartIndex = nFromId * m_nNodeCount;
	for (int i = 0; i < nNeighbourCount; ++i)
	{
		int nNeighbourId = m_pDirectNeighboursId[nFromId][i];
		if (m_pMatrix[nStartIndex + nNeighbourId].IsFixed())
		{
			m_pFixedNearNeighbourIds[m_nFixedNearNeighbourCount++] = nNeighbourId;
		}
	}
	return m_nFixedNearNeighbourCount;
}

void DijTable::AddSelfToFirst(int nBeginId)
{
	InsertOneNew(nBeginId, ILLEGAL_ID);
}

void DijTable::AddKnownSelfWithoutPrev(int nBeginId)
{
	for (int i = 0; i < m_nNodeCount; ++i)
	{
		if (i != nBeginId && m_pMatrix[nBeginId * m_nNodeCount + i].IsFixed() && m_pMatrix[nBeginId * m_nNodeCount + i].m_nNext != ILLEGAL_ID)
		{
			ASSERT(m_pFixedIndex[i] == ILLEGAL_ID);
			InsertOneNew(i, ILLEGAL_ID);
		}
	}
}

void DijTable::BuildPrevLinkInRecord(int nBeginId)
{
	for (int i = 1; i < m_nFixedCount; ++i)
	{
		BuildPrevLinkInRecordFrom(i, nBeginId);
	}
}

void DijTable::BuildPrevLinkInRecordFrom(int nFromIndex, int nBeginId)
{
	int nTmpIndex = nFromIndex;
	int nTmpId = m_pRecords[nTmpIndex].m_nId;
	ASSERT(nTmpId != ILLEGAL_ID);
	while (nTmpId != nBeginId && m_pRecords[nTmpIndex].m_nPrevIndex == ILLEGAL_ID)
	{
		int nPrevId = m_pMatrix[nTmpId*m_nNodeCount + nBeginId].m_nNext;
		ASSERT(nPrevId != ILLEGAL_ID);
		int nPrevIndex = m_pFixedIndex[nPrevId];
		ASSERT(nPrevIndex != ILLEGAL_ID);
		m_pRecords[nTmpIndex].m_nPrevIndex = nPrevIndex;
		nTmpId = nPrevId;
		nTmpIndex = m_pFixedIndex[nTmpId];
	}

}

bool DijTable::IsNeighbour(int nAId, int nBId)
{
	bool bFind = false;
	for (int i = 0; i < m_pDirectNeighboursId[nAId].GetSize(); ++i)
	{
		if (nBId == m_pDirectNeighboursId[nAId][i])
		{
			bFind = true;
			break;
		}
	}
	if (bFind == false)
	{
		return false;
	}
	bFind = false;
	for (int i = 0; i < m_pDirectNeighboursId[nBId].GetSize(); ++i)
	{
		if (nAId == m_pDirectNeighboursId[nBId][i])
		{
			bFind = true;
			break;
		}
	}
	if (bFind == false)
	{
		return false;
	}
	return true;
}

bool DijTable::IsAllNeighbourFixed(int nBeginId)
{
	m_nFixedNearNeighbourCount = 0;
	int nNeighbourCount = m_pDirectNeighboursId[nBeginId].GetSize();
	int nStartIndex = nBeginId * m_nNodeCount;
	for (int i = 0; i < nNeighbourCount; ++i)
	{
		int nNeighbourId = m_pDirectNeighboursId[nBeginId][i];
		if (!m_pMatrix[nStartIndex + nNeighbourId].IsFixed())
		{
			return false;
		}
	}
	return true;
}

void DijTable::AddNeighboursKnown(int nBeginId)
{
	if (m_nFixedNearNeighbourCount == 0)
	{
		return;
	}

	for (int i = 0; i < m_nNodeCount; ++i)
	{
		if (m_pMatrix[nBeginId * m_nNodeCount + i].IsFixed())
		{
			continue;
		}

		bool bAllFixed = true;

		double fMin = DBL_MAX;
		int nMinNextId = -1;

		for (int j = 0; j < m_nFixedNearNeighbourCount; ++j)
		{
			int nNeighbourId = m_pFixedNearNeighbourIds[j];
			int nItemIndex = nNeighbourId*m_nNodeCount + i;
			if (m_pMatrix[nItemIndex].IsFixed())
			{
				double fCompareDistance = m_pMatrix[nItemIndex].m_fDistance + m_pMatrix[nBeginId*m_nNodeCount + nNeighbourId].m_fDistance;
				if (fCompareDistance < fMin)
				{
					fMin = fCompareDistance;
					nMinNextId = nNeighbourId;
				}
			}
			else
			{
				bAllFixed = false;
				break;
			}
		}
		if (bAllFixed && nMinNextId != ILLEGAL_ID)
		{
			AddOneNodeToALine(nBeginId, nMinNextId, i);
		}
	}
}

void DijTable::MarkRemainUnknown(int nBeginId)
{
	for (int i = 0; i < m_nNodeCount; ++i)
	{
		ASSERT(fabs(m_pMatrix[nBeginId * m_nNodeCount + i].m_fDistance - m_pMatrix[i * m_nNodeCount + nBeginId].m_fDistance) < DOUBLE_COMPARE_LIMIT);
		m_pMatrix[nBeginId * m_nNodeCount + i].SetFixed();
		m_pMatrix[i * m_nNodeCount + nBeginId].SetFixed();
	}
}

void DijTable::GetKnownBasedOnSelf(int nBeginId)
{
	AddSelfToFirst(nBeginId);
	AddKnownSelfWithoutPrev(nBeginId);
	BuildPrevLinkInRecord(nBeginId);
}

void DijTable::GetKnownBasedOnNeighbour(int nBeginId)
{
	if (!IsAllNeighbourFixed(nBeginId))
	{
		return;
	}
	GetFixedNeighbours(nBeginId);
	AddNeighboursKnown(nBeginId);
}

// new-near-...-far
void DijTable::AddOneNodeToALine(int nAddedId, int nLineNearId, int nLineFarId)
{
	ASSERT(IsNeighbour(nAddedId, nLineNearId));

	if (!m_pMatrix[nAddedId * m_nNodeCount + nLineNearId].IsFixed())
	{
		double fDistanceA = m_pMatrix[nAddedId*m_nNodeCount + nLineNearId].m_fDistance;
		double fDistanceB = 0;
		double fDistance = fDistanceA + fDistanceB;
		BuildSimpleLine(nAddedId, nLineNearId, nLineNearId, nAddedId, fDistance);
	}

	int nAId = nLineNearId;
	int nAPrevId = nAId;

	while (nAId != nLineFarId)
	{
		ASSERT(m_pMatrix[nAId*m_nNodeCount + nLineFarId].IsFixed());
		nAId = m_pMatrix[nAId*m_nNodeCount + nLineFarId].m_nNext;
		ASSERT(nAId != ILLEGAL_ID);

		ASSERT(m_pMatrix[nAddedId*m_nNodeCount + nAId].IsFixed() == m_pMatrix[nAId*m_nNodeCount + nAddedId].IsFixed());
		if (!m_pMatrix[nAddedId*m_nNodeCount + nAId].IsFixed())
		{
			double fDistanceA = m_pMatrix[nAddedId*m_nNodeCount + nLineNearId].m_fDistance;
			double fDistanceB = m_pMatrix[nLineNearId*m_nNodeCount + nAId].m_fDistance;
			double fDistance = fDistanceA + fDistanceB;
			BuildSimpleLine(nAddedId, nLineNearId, nAId, nAPrevId, fDistance);
		}

		nAPrevId = nAId;
	}

}

bool DijTable::IsStrongConnected(int nLineAId, int nLineBId)
{
	while (nLineAId != nLineBId)
	{
		if (!m_pMatrix[nLineAId*m_nNodeCount + nLineBId].IsFixed())
		{
			return false;
		}
		nLineAId = m_pMatrix[nLineAId*m_nNodeCount + nLineBId].m_nNext;
	}
	return true;
}

CString DijTable::strGetPath(int nFromId, int nToId)
{
	CString strOut;
	CString strTmp;
	strOut.Format(_T("\nD:%d->%d \t"), nFromId, nToId);
	if (m_pMatrix[nFromId*m_nNodeCount + nToId].m_fDistance == DBL_MAX)
	{
		strOut += _T("too long");
		OutputDebugString(strOut);
		return _T("too long");
	}
	OutputDebugString(strOut);
	while (nFromId != nToId)
	{
		int nNextId = m_pMatrix[nFromId*m_nNodeCount + nToId].m_nNext;
		nFromId = nNextId;
		strOut += strTmp;
	}
	strOut += strTmp;
	return strOut;
}

int DijTable::GetNextHocId(int nSrcId, int nDesId)
{
	return m_pMatrix[nSrcId * m_nNodeCount + nDesId].m_nNext;
}

double DijTable::GetDistance(int nSrcId, int nDesId)
{
	return m_pMatrix[nSrcId * m_nNodeCount + nDesId].m_fDistance;
}

void DijTable::SearchFixed(int nBeginId)
{
	GetKnownBasedOnNeighbour(nBeginId);
	GetKnownBasedOnSelf(nBeginId);
}

CSimpleStack::CSimpleStack(int nSize)
{
	Init(nSize);
}

void CSimpleStack::Init(int nSize)
{
	m_pBuffer = new int[nSize];
	m_nSize = 0;
}

CSimpleStack::~CSimpleStack()
{
	if (m_pBuffer)
	{
		delete m_pBuffer;
	}
}

void CSimpleStack::Reset()
{
	m_nSize = 0;
}
