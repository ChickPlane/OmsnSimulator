#pragma once
class CCommonFunctions
{
public:
	CCommonFunctions();
	virtual ~CCommonFunctions();

	static char * GetCharStringFromCString(const CString & str);
	static void PickMFromNDisorder(int nM, char * pArr, int nN);
};

template<class TKey, class TValue>
class CPair_T
{
public:
	TKey m_Key;
	TValue m_Value;
	bool operator < (const CPair_T<TKey, TValue> & theOther)
	{
		return (m_Key < theOther.m_Key);
	}
	bool operator == (const CPair_T<TKey, TValue> & theOther)
	{
		return (m_Key == theOther.m_Key);
	}
	CPair_T<TKey, TValue> & operator = (const CPair_T<TKey, TValue> & src)
	{
		m_Key = src.m_Key;
		m_Value = src.m_Value;
		return *this;
	}
};

template<class T>
class CCommonFunctions_T
{
public:
	static void QuickIncreaseSort(T * pArr, int nLength);
	static void GetRandomDisorderArray(T * pArr, int nLength);

	static void QuickIncreaseSortPtr(T ** pArr, int nLength);

private:
	static void QuickIncSortIteration(T * pArrStart, T * pArrEnd);
	static void QuickIncSortIterationPtr(T ** ppArrStart, T ** ppArrEnd);
};

template<class T>
void CCommonFunctions_T<T>::GetRandomDisorderArray(T * pArr, int nLength)
{
	CPair_T<int, int> * pRandomArray = new CPair_T<int, int>[nLength];
	for (int i = 0; i < nLength; ++i)
	{
		pRandomArray[i].m_Key = rand();
		pRandomArray[i].m_Value = i;
	}
	CCommonFunctions_T<CPair_T<int, int>>::QuickIncreaseSort(pRandomArray, nLength);

	for (int i = 0; i < nLength; ++i)
	{
		pRandomArray[i].m_Key = pRandomArray[i].m_Value;
		pRandomArray[i].m_Value = i;
	}
	CCommonFunctions_T<CPair_T<int, int>>::QuickIncreaseSort(pRandomArray, nLength);

	int nIndexPick = 0, nIndexPut = 0;
	nIndexPut = pRandomArray[nIndexPick].m_Value;

	char * pReplaced = new char[nLength];
	memset(pReplaced, 0, nLength);
	T Picking, Putting;
	Picking = pArr[nIndexPick];
	pReplaced[nIndexPick] = 1;

	for (int i = 0; i < nLength; ++i)
	{
		Putting = pArr[nIndexPut];
		pArr[nIndexPut] = Picking;
		Picking = Putting;

		if (pReplaced[nIndexPut] != 0)
		{
			for (int j = 0; j < nLength; ++j)
			{
				if (pReplaced[j] == 0)
				{
					nIndexPick = j;
					Picking = pArr[nIndexPick];
					break;
				}
			}
		}
		else
		{
			nIndexPick = nIndexPut;
		}
		pReplaced[nIndexPick] = 1;
		nIndexPut = pRandomArray[nIndexPick].m_Value;
	}

	delete[] pRandomArray;
	delete[] pReplaced;
}

template<class T>
void CCommonFunctions_T<T>::QuickIncSortIteration(T * pArrStart, T * pArrEnd)
{
	if (pArrStart == pArrEnd)
	{
		return;
	}
	T CmpValue = *pArrEnd;
	T * pToBeReplace = pArrStart;
	T * pTest = pArrStart;
	while (pTest != pArrEnd)
	{
		if (*pTest < CmpValue || *pTest == CmpValue)
		{
			if (pTest != pToBeReplace)
			{
				T tmp = *pTest;
				*pTest = *pToBeReplace;
				*pToBeReplace = tmp;
			}
			++pToBeReplace;
		}
		++pTest;
	}
	if (pArrEnd == pToBeReplace)
	{
		QuickIncSortIteration(pArrStart, pToBeReplace - 1);
	}
	else
	{
		T tmp = *pTest;
		*pTest = *pToBeReplace;
		*pToBeReplace = tmp;

		if (pArrStart != pToBeReplace)
		{
			QuickIncSortIteration(pArrStart, pToBeReplace - 1);
		}
		QuickIncSortIteration(pToBeReplace + 1, pArrEnd);
	}
}

template<class T>
void CCommonFunctions_T<T>::QuickIncreaseSort(T * pArr, int nLength)
{
	if (nLength <= 1)
	{
		return;
	}
	QuickIncSortIteration(pArr, pArr + nLength - 1);
}

template<class T>
void CCommonFunctions_T<T>::QuickIncSortIterationPtr(T ** ppArrStart, T ** ppArrEnd)
{
	if (ppArrStart == ppArrEnd)
	{
		return;
	}
	T * pCmpValue = *ppArrEnd;
	T ** ppToBeReplace = ppArrStart;
	T ** ppTest = ppArrStart;
	while (ppTest != ppArrEnd)
	{
		if (**ppTest < *pCmpValue || **ppTest == *pCmpValue)
		{
			if (ppTest != ppToBeReplace)
			{
				T* tmp = *ppTest;
				*ppTest = *ppToBeReplace;
				*ppToBeReplace = tmp;
			}
			++ppToBeReplace;
		}
		++ppTest;
	}
	if (ppArrEnd == ppToBeReplace)
	{
		QuickIncSortIteration(ppArrStart, ppToBeReplace - 1);
	}
	else
	{
		T* tmp = *ppTest;
		*ppTest = *ppToBeReplace;
		*ppToBeReplace = tmp;

		if (ppArrStart != ppToBeReplace)
		{
			QuickIncSortIteration(ppArrStart, ppToBeReplace - 1);
		}
		QuickIncSortIteration(ppToBeReplace + 1, ppArrEnd);
	}
}

template<class T>
void CCommonFunctions_T<T>::QuickIncreaseSortPtr(T ** pArr, int nLength)
{
	if (nLength <= 1)
	{
		return;
	}
	GetRandomDisorderArrayPtr(pArr, pArr + nLength - 1);
}
