#pragma once
#include "streethead.h"
class CDijConnection
{
public:
	CDijConnection();
	CDijConnection(const CDijConnection & src);
	virtual ~CDijConnection();
	CDijNode * GetFirstDijNode();
	const CDijNode * GetFirstDijNode() const;
	CDijNode * GetLastDijNode();
	const CDijNode * GetLastDijNode() const;
	CDijConnection & operator = (const CDijConnection & src);

	CArray<PtrIntersection> m_Intersections;
	double m_fDistance;

	CString GetString();
};

