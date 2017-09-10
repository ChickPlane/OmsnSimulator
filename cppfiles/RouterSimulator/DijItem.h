#pragma once
#include "DijConnection.h"

#define DOUBLE_COMPARE_LIMIT (1.0e-10)

class CDijItem
{
public:
	CDijItem();
	virtual ~CDijItem();

	void SetValue(const CDijConnection & pNode);
	void SetValue(int nFrom, int nTo, int nNext, double fDistance);
	void SetFixed();
	bool IsFixed();

	bool operator == (const CDijItem & src) const;
	bool operator != (const CDijItem & src) const;

	int m_nFrom;
	int m_nTo;
	int m_nNext;
	double m_fDistance;

private:
	bool m_bFixed;
};

