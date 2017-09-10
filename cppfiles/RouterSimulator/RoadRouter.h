#pragma once
class CRoadRouter
{
public:
	CRoadRouter();
	virtual ~CRoadRouter();

	void SetValue(int nCurrentPointId, int nIntersectionId, int nNextPointId, double fDistance);

	int m_nCurrentPointId;
	int m_nIntersectionId;
	int m_nNextPointId;
	double m_fDistance;
};

