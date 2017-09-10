#include "stdafx.h"
#include "DijRecord.h"


CDijRecord::CDijRecord()
{
}


CDijRecord::~CDijRecord()
{
}

void CDijRecord::SetValue(int nId, int nMarked, int nPrevIndex, int nShortestId, double fShortestValue)
{
	m_nId = nId;
	m_nMarked = nMarked;
	m_nPrevIndex = nPrevIndex;
	m_nShortestId = nShortestId;
	m_fShortestValue = fShortestValue;

}
