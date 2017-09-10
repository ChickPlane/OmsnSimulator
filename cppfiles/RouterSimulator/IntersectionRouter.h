#pragma once
class CIntersectionRouter
{
public:
	CIntersectionRouter();
	~CIntersectionRouter();

	void SetValue(int nCurrentInterId, int nDestinationPointId, int nNextPointId, double fDistance);

	int m_nCurrentInterId;
	int m_nDestinationPointId;
	int m_nNextPointId;
	double m_fDistance;
};

