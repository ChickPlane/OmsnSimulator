#include "stdafx.h"
#include "RoadInsideRouter.h"


CRoadInsideRouter::CRoadInsideRouter()
{
}


CRoadInsideRouter::~CRoadInsideRouter()
{
}

void CRoadInsideRouter::InsertItem(int nSrcId, int nNextId, int nDesId, double fDistance)
{
	CRoadInsideRouterEntry tmpEntry;
	tmpEntry.m_nSrcId = nSrcId;
	tmpEntry.m_nNextId = nNextId;
	tmpEntry.m_nDesId = nDesId;
	tmpEntry.m_fDistance = fDistance;
	for (int i = 0; i < m_AllEntries.GetSize(); ++i)
	{
		if (m_AllEntries[i].m_nDesId == nDesId)
		{
			if (m_AllEntries[i].m_fDistance < fDistance)
			{
				m_AllEntries[i] = tmpEntry;
			}
			return;
		}
	}
	m_AllEntries.Add(tmpEntry);
}
