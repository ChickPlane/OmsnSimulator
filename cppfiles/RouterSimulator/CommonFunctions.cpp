#include "stdafx.h"
#include "CommonFunctions.h"


CCommonFunctions::CCommonFunctions()
{
	rand();
}


CCommonFunctions::~CCommonFunctions()
{
}

char * CCommonFunctions::GetCharStringFromCString(const CString & str)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char *pret = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, str, -1, pret, len, NULL, NULL);
	return pret;
}

void CCommonFunctions::PickMFromNDisorder(int nM, char * pArr, int nN)
{
	memset(pArr, 1, sizeof(char) * nM);
	memset(pArr + nM, 0, sizeof(char) * (nN - nM));

	CCommonFunctions_T<char>::GetRandomDisorderArray(pArr, nN);
}