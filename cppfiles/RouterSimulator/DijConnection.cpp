#include "stdafx.h"
#include "DijConnection.h"
#include "Intersection.h"
#include "DijNode.h"


CDijConnection::CDijConnection()
{
}


CDijConnection::CDijConnection(const CDijConnection & src)
{
	*this = src;
}

CDijConnection::~CDijConnection()
{
}

CDijNode * CDijConnection::GetFirstDijNode()
{
	return m_Intersections[0]->GetDij();
}

const CDijNode * CDijConnection::GetFirstDijNode() const
{
	return m_Intersections[0]->GetDij();
}

CDijNode * CDijConnection::GetLastDijNode()
{
	return m_Intersections[m_Intersections.GetSize() - 1]->GetDij();
}

const CDijNode * CDijConnection::GetLastDijNode() const
{
	return m_Intersections[m_Intersections.GetSize() - 1]->GetDij();
}

CString CDijConnection::GetString()
{
	CString strOut;
	CString strTmp;
	for (int i = 0; i < m_Intersections.GetSize(); ++i)
	{
		if (m_Intersections[i]->IsDijNode())
		{
			strTmp.Format(_T("[%d(%d)]"), m_Intersections[i]->GetId(), m_Intersections[i]->GetDij()->m_nID);
		}
		else
		{
			strTmp.Format(_T("[%d]"), m_Intersections[i]->GetId());
		}
		strOut += strTmp;
	}
	strTmp.Format(_T("--%f"), m_fDistance);
	strOut += strTmp;
	return strOut;
}

CDijConnection & CDijConnection::operator=(const CDijConnection & src)
{
	m_Intersections.Copy(src.m_Intersections);
	m_fDistance = src.m_fDistance;
	return *this;
}
