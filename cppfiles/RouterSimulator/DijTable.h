#pragma once
#include "streethead.h"
#include "DijRecord.h"

class CDijItem;

#define MARK_NEED_CALCULATE_SHORTEST 1
#define MARK_NOTHING_TO_DO 0

#define ILLEGAL_ID -1

class CSimpleStack
{
public:
	CSimpleStack() :m_pBuffer(NULL), m_nSize(0) {}
	CSimpleStack(int nSize);
	void Init(int nSize);
	~CSimpleStack();
	void Reset();
	void Push(int nNumber) 
	{
		m_pBuffer[m_nSize++] = nNumber; 
	}
	int Pop()
	{
		return m_pBuffer[--m_nSize];
	}
	int * m_pBuffer;
	int m_nSize;
};


class DijTable
{
public:
	DijTable();
	~DijTable();

	void Init(const CArray<PtrDijNode> & m_allDijNodes);
	void RunDijOn(int nBeginId);
	int m_nNodeCount;
	CDijItem * m_pMatrix;
	CArray<int> * m_pDirectNeighboursId;

public:
	int GetNextHocId(int nSrcId, int nDesId);
	double GetDistance(int nSrcId, int nDesId);

	void SearchFixed(int nBeginId);
	int RunDijStep(int nBegin);
	void InsertOneNew(int nNewOneId, int nPrevIndex);
	void BuildSimpleLine(int nAId, int nANextId, int nBId, int nBPrevId, double fDistance);
	int UpdateShortest(int nBegin);
	int GetUnsureShortest(int nBegin, int nCalcuRecordIndex);
	void RemarkRecords(int nNext);
	int GetFixedNeighbours(int nFromId);
	void AddOneNodeToALine(int nAddedId, int nLineNearId, int nLineFarId);
	bool IsStrongConnected(int nLineAId, int nLineBId);

	CString strGetPath(int nFromId, int nToId);

	CDijRecord * m_pRecords;
	int * m_pFixedIndex;
	int m_nFixedCount;

private:
	int * m_pFixedNearNeighbourIds;
	int m_nFixedNearNeighbourCount;

	CSimpleStack m_BackRoad;

private:
	void GetKnownBasedOnSelf(int nBeginId);
	void GetKnownBasedOnNeighbour(int nBeginId);
	void AddSelfToFirst(int nBeginId);
	void AddKnownSelfWithoutPrev(int nBeginId);
	void BuildPrevLinkInRecord(int nBeginId);
	void BuildPrevLinkInRecordFrom(int nFromIndex, int nBeginId);
	bool IsNeighbour(int nAId, int nBId);
	bool IsAllNeighbourFixed(int nBeginId);
	void AddNeighboursKnown(int nBeginId);
	void MarkRemainUnknown(int nBeginId);
};

