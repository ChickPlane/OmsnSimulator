#pragma once
#include "RoutingProcessBSW.h"
class CRoutingProcessHslpo :
	public CRoutingProcessBSW
{
public:
	CRoutingProcessHslpo();
	virtual ~CRoutingProcessHslpo();

	virtual void GenerateMission(const CRoutingDataEnc & encData);
	virtual void SendData(CRoutingProtocol * pNext);
	virtual void OnReceivedData(const CRoutingMsg * pMsg);

	virtual void SetPrivacyParam(int nK, double fHigh, double fLow);
	virtual int GetAnonymityPackageCount() const;

protected:
	int m_nK;
	double m_fPrivacyHigh;
	double m_fPrivacyLow;
};

