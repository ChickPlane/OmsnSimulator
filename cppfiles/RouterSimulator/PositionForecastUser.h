#pragma once
#include "SimulatorCommon.h"
class CPositionForecastUser
{
public:
	CPositionForecastUser();
	virtual ~CPositionForecastUser();
	virtual void OnFinishedForecastOnce(SIM_TIME lnSimTime) = 0;
};

