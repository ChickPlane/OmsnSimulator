#include "stdafx.h"
#include "WktParse.h"
#include <fstream>
#include <string>

using namespace std;


CWktParse::CWktParse()
{
}


CWktParse::~CWktParse()
{
}

CList<CLineInMap> * CWktParse::Parse(char * filename, double & l, double & r, double & t, double & b)
{
	ifstream fin(filename);
	CList<CLineInMap> * pRet = new CList<CLineInMap>();
	int nInputmax = 40 * 2000;
	char * pInputBuffer = new char[nInputmax];

	bool bHasTail = true;

	string strOneLine;

	while (!fin.eof())
	{
		fin.getline(pInputBuffer, nInputmax - 1);
		if (strlen(pInputBuffer) == 0)
		{
			continue;
		}
		string strOneLineTmp(pInputBuffer);
		bool bHead = CheckHead(strOneLineTmp);
		bool bTail = CheckTail(strOneLineTmp);

		if (!bHasTail)
		{
			if (bHead)
			{
				delete pRet;
				return NULL;
			}
			else
			{
				strOneLine += strOneLineTmp;
			}
		}
		else
		{
			if (!bHead)
			{
				delete pRet;
				return NULL;
			}
			strOneLine = strOneLineTmp;
		}
		if (!bTail)
		{
			bHasTail = false;
			continue;
		}
		bHasTail = true;

		int nStartPos = CheckFormat(strOneLine);

		pRet->AddTail(ParseOneLine(strOneLine, nStartPos, l, r, t, b));
	}
	fin.close();
	delete [] pInputBuffer;
	return pRet;
}

int CWktParse::ParseBuildings(char * filename, double l, double r, double t, double b, CList<CBuilding> & result)
{
	ifstream fin(filename);
	int nInputmax = 200;
	char * pInputBuffer = new char[nInputmax];


	while (!fin.eof())
	{
		fin.getline(pInputBuffer, nInputmax - 1);
		if (strlen(pInputBuffer) == 0)
		{
			continue;
		}
		string strOneLine(pInputBuffer);
		CBuilding tmp = ParseBuilding(strOneLine, l, r, t, b);
		if (l <= tmp.m_X && tmp.m_X <= r && t <= tmp.m_Y && tmp.m_Y <= b)
		{
			result.AddTail(tmp);
		}
	}
	fin.close();
	delete[] pInputBuffer;
	return result.GetSize();
}

CBuilding CWktParse::ParseBuilding(string strOneLine, double l, double r, double t, double b)
{
	double x;
	double y;
	CBuilding ret;
	int nStartPos = sm_PointHead.length();
	int nLength = strOneLine.length();
	if (!IsDoubleChar(strOneLine[nStartPos]))
	{
		ASSERT(0);
	}
	int nState = 1;
	char strDoubleBuffer[30];
	char strType[100];
	int nDoubleLength = 0;
	int nTypeLength = 0;
	CDoublePoint newDp;
	for (int i = nStartPos; i < nLength; ++i)
	{
		switch (nState)
		{
		case 1:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
			}
			else
			{
				strDoubleBuffer[nDoubleLength] = 0;
				ret.m_X = atof(strDoubleBuffer);
				ASSERT(ret.m_X > 10);
				nDoubleLength = 0;
				nState = 2;
			}
			break;
		}
		case 2:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
				nState = 3;
			}
			break;
		}
		case 3:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
			}
			else
			{
				strDoubleBuffer[nDoubleLength] = 0;
				ret.m_Y = atof(strDoubleBuffer);
				nDoubleLength = 0;
				ASSERT(strOneLine[i] == ')');
				nState = 4;
			}
			break;
		}
		case 4:
		{
			if (';' == strOneLine[i])
			{
				nState = 5;
				nTypeLength = 0;
			}
			break;
		}
		case 5:
		{
			if (';' != strOneLine[i])
			{
				strType[nTypeLength++] = strOneLine[i];
			}
			else
			{
				strType[nTypeLength] = 0;
				ret.m_strMainType = strType;
				nState = 6;
				nTypeLength = 0;
			}
			break;
		}
		case 6:
		{
			if (';' != strOneLine[i])
			{
				strType[nTypeLength++] = strOneLine[i];
			}
			else
			{
				i++;
				strType[nTypeLength] = 0;
				ret.m_strSubType = strType;
				return ret;
			}
			break;
		}
		}
	}
	ASSERT(false);
	return ret;
}

int CWktParse::CheckFormat(string strOneLine)
{
	string strHead("LINESTRING");
	int nStartPos = strOneLine.find(strHead, 0);
	nStartPos += strHead.length();
	while (nStartPos < strOneLine.length())
	{
		if (IsDoubleChar(strOneLine[nStartPos]))
		{
			break;
		}
		++nStartPos;
	}
	return nStartPos;
}

bool CWktParse::CheckHead(string strOneLine)
{
	string strHead("LINESTRING");
	int nStartPos = strOneLine.find(strHead, 0);
	if (nStartPos != 0)
	{
		return false;
	}
	nStartPos += strHead.length();
	while (nStartPos < strOneLine.length())
	{
		if (strOneLine[nStartPos] == '(')
		{
			break;
		}
		if (strOneLine[nStartPos] != ' ')
		{
			return false;
		}
		++nStartPos;
	}
	return true;
}

bool CWktParse::CheckTail(string strOneLine)
{
	string strTail(")");
	int nEnd = strOneLine.rfind(strTail);
	if (nEnd == string::npos)
	{
		return false;
	}
	return true;
}

CLineInMap CWktParse::ParseOneLine(string strOneLine, int nStartPos, double & l, double & r, double & t, double & b)
{
	static int runtime = 0;
	runtime++;
	double x;
	double y;
	CLineInMap ret;
	int nLength = strOneLine.length();
	if (!IsDoubleChar(strOneLine[nStartPos]))
	{
		return ret;
	}
	int nState = 1;
	char strDoubleBuffer[30];
	int nDoubleLength = 0;
	CDoublePoint newDp;
	for (int i = nStartPos; i < nLength; ++i)
	{
		switch (nState)
		{
		case 1:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
			}
			else
			{
				strDoubleBuffer[nDoubleLength] = 0;
				newDp.m_X = atof(strDoubleBuffer);
				ASSERT(newDp.m_X > 10);
				nDoubleLength = 0;
				nState = 2;
			}
			break;
		}
		case 2:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
				nState = 3;
			}
			break;
		}
		case 3:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
			}
			else
			{
				strDoubleBuffer[nDoubleLength] = 0;
				newDp.m_Y = atof(strDoubleBuffer);
				nDoubleLength = 0;
				if (strOneLine[i] == ',')
				{
					ret.m_Line.Add(newDp);
					UpdateRange(newDp, l, r, t, b);
					nState = 4;
				}
				else if (strOneLine[i] == ')')
				{
					ret.m_Line.Add(newDp);
					UpdateRange(newDp, l, r, t, b);
					ASSERT(ret.m_Line.GetSize() > 1);
#if 0
					CString strOut;
					strOut.Format(_T("\n%d:\t%d"), runtime, ret.m_Line.GetSize());
					OutputDebugString(strOut);
#endif
					return ret;
				}
				else
					ASSERT(false);
			}
			break;
		}
		case 4:
		{
			if (IsDoubleChar(strOneLine[i]))
			{
				strDoubleBuffer[nDoubleLength++] = strOneLine[i];
				nState = 1;
			}
			break;
		}
		}
	}
	return ret;
}

void CWktParse::UpdateRange(const CDoublePoint & point, double & l, double & r, double & t, double & b)
{
	if (point.m_X < l)
	{
		l = point.m_X;
	}
	if (point.m_X > r)
	{
		r = point.m_X;
	}
	if (point.m_Y < t)
	{
		t = point.m_Y;
	}
	if (point.m_Y > b)
	{
		b = point.m_Y;
	}
}

bool CWktParse::IsDoubleChar(char c)
{
	if (isdigit(c) || c == '.')
	{
		return true;
	}
	else
	{
		return false;
	}
}

std::string CWktParse::sm_PointHead = "POINT (";
