#pragma once
#include "SimulatorCommon.h"
class CHost;
class CRoutingProtocolBSW;

class CNearbyNodeEntry
{
public:
	CNearbyNodeEntry();
	CNearbyNodeEntry(const CNearbyNodeEntry & src);
	CNearbyNodeEntry & operator = (const CNearbyNodeEntry & src);
	~CNearbyNodeEntry();

	SIM_TIME m_lnTimeOut;
	CRoutingProtocolBSW * m_pProtocol;
};

