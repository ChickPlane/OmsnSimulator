#pragma once
class CDijRecord
{
public:
	CDijRecord();
	~CDijRecord();

	void SetValue(int nId, int nMarked, int nPrevIndex, int nShortestId, double fShortestValue);
public:
	int m_nId;
	int m_nMarked;
	int m_nPrevIndex;
	int m_nShortestId;
	float m_fShortestValue;
};
