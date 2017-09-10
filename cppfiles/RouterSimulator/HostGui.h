#pragma once
#include "DoublePoint.h"
class CHost;

class CHostGui
{
public:
	CHostGui();
	CHostGui(const CHostGui & src);
	~CHostGui();

	CHostGui & operator = (const CHostGui & src);

	CHost * m_pHost;
	CDoublePoint m_Position;
};

