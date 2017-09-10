#pragma once
#include "RoutingDataEnc.h"

class CRoutingProcessBSW;

class CRoutingDataEncBSW :
	public CRoutingDataEnc
{
public:
	CRoutingDataEncBSW();
	CRoutingDataEncBSW(const CRoutingDataEncBSW & src);
	virtual CRoutingDataEncBSW & operator = (const CRoutingDataEncBSW & src);
	virtual ~CRoutingDataEncBSW();
	virtual CRoutingDataEncBSW * GetDuplicate() const;

	CList<CRoutingProcessBSW *> m_CarryingProcesses;
};

