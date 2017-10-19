#include "stdafx.h"
#include "CommonFunctions.h"


CCommonFunctions::CCommonFunctions()
{
	rand();
}


CCommonFunctions::~CCommonFunctions()
{
}

BOOL CCommonFunctions::PickMFromNDisorder(int nM, char * pArr, int nN)
{
	BOOL bReverse = FALSE;
	if (2 * nM > nN)
	{
		bReverse = TRUE;
		nM = nN - nM;
	}

	memset(pArr, 0, sizeof(char) * nN);
	int nIndex = rand() % nN;
	for (int i = 0; i < nM; ++i)
	{
		int nRand = rand() % (nN - i);
		int j = 0;
		while (TRUE)
		{
			if (pArr[nIndex] != 0)
			{
				nIndex = (nIndex + 1) % nN;
				continue;
			}
			else
			{
				if (j == nRand)
				{
					pArr[nIndex] = 1;
					nIndex = (nIndex + 1) % nN;
					break;
				}
				else
				{
					++j;
					nIndex = (nIndex + 1) % nN;
				}
			}
		}
	}
	return bReverse;
}