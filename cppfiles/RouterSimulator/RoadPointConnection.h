#pragma once
#include "streethead.h"

class CRoadPointConnection
{
public:
	CRoadPointConnection();
	CRoadPointConnection(const CRoadPointConnection & src);
	CRoadPointConnection(PtrRoad pRoad, int nSelf, int nOther, double fDistance);
	void SetValue(PtrRoad pRoad, int nSelf, int nOther, double fDistance);
	virtual ~CRoadPointConnection();

	CRoadPointConnection & operator = (const CRoadPointConnection & src);
	bool operator == (const CRoadPointConnection & src);
	bool SamePair(const PtrRoadPoint pSelf, const PtrRoadPoint pOther);
	PtrRoadPoint GetTheOtherEnd();
	PtrRoadPoint GetSelfEnd();

	double m_fDistance;
	PtrRoad m_pRoad;
	int m_nPosSelf;
	int m_nPosOther;

private:
	PtrRoadPoint m_pSelf;
	PtrRoadPoint m_pOther;
};

