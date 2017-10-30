#pragma once
template<class T>
class CTimeOutPair
{
public:

	CTimeOutPair()
	{
	}
	CTimeOutPair(const CTimeOutPair<T> & src)
	{
		*this = src;
	}
	CTimeOutPair<T> & operator = (const CTimeOutPair<T> & src);

	~CTimeOutPair()
	{
	}

	static void InsertToTimeoutPairList(const CTimeOutPair<T> & newPair, CList<CTimeOutPair<T>> & theList);
	static void DeleteTimePairs(CList<CTimeOutPair<T>> & theList, SIM_TIME lnCT);
	SIM_TIME m_lnTimeOut;
	T m_Value;
};

template<class T>
void CTimeOutPair<T>::DeleteTimePairs(CList<CTimeOutPair<T>> & theList, SIM_TIME lnCT)
{
	POSITION pos = theList.GetHeadPosition(), prevpos;
	while (pos)
	{
		prevpos = pos;
		CTimeOutPair<T>& bt = theList.GetNext(pos);
		if (bt.m_lnTimeOut < lnCT)
		{
			theList.RemoveAt(prevpos);
		}
		else
		{
			return;
		}
	}
}

template<class T>
void CTimeOutPair<T>::InsertToTimeoutPairList(const CTimeOutPair<T> & newPair, CList<CTimeOutPair<T>> & theList)
{

	POSITION pos = theList.GetHeadPosition(), prevpos;
	while (pos)
	{
		prevpos = pos;
		CTimeOutPair<T>& bt = theList.GetNext(pos);
		if (bt.m_lnTimeOut > newPair.m_lnTimeOut)
		{
			theList.InsertBefore(prevpos, newPair);
			return;
		}
	}
	theList.AddTail(newPair);
}

template<class T>
CTimeOutPair<T> & CTimeOutPair<T>::operator=(const CTimeOutPair<T> & src)
{
	m_lnTimeOut = src.m_lnTimeOut;
	m_Value = src.m_Value;
	return *this;
}
