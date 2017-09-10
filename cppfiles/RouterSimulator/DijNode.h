#pragma once
#include "RoadPointConnection.h"
#include "DijConnection.h"
class CDijNode
{
public:
	CDijNode();
	CDijNode(CIntersection * pIntersection, int nID);

	~CDijNode();

	void AddNewEntry(CDijConnection & entry);
	void CreateDijConnections(CArray<PtrIntersection> & allInterSections);
	int GetNextDijId(CArray<PtrIntersection> & allInterSections, int nNextIntersectionId, CDijConnection & newConn);

	CIntersection * m_pIntersection;
	int m_nID;
	CArray<CDijConnection> m_DijToDij;
};

