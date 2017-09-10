#pragma once
#include "DoublePoint.h"
#include "LineInMap.h"
#include <string>
#include "Building.h"
using namespace std;

class CWktParse
{
public:
	CWktParse();
	~CWktParse();

	static CList<CLineInMap> * Parse(char * filename, double & l, double & r, double & t, double & b);
	static int ParseBuildings(char * filename, double l, double r, double t, double b, CList<CBuilding> & result);
	static CBuilding ParseBuilding(string strOneLine, double l, double r, double t, double b);
	static int CheckFormat(string);
	static bool CheckHead(string strOneLine);
	static bool CheckTail(string strOneLine);
	static CLineInMap ParseOneLine(string strOneLine, int nStartPos, double & l, double & r, double & t, double & b);
	inline static void UpdateRange(const CDoublePoint & point, double & l, double & r, double & t, double & b);

	static bool IsDoubleChar(char c);

private:
	static string sm_PointHead;
};

