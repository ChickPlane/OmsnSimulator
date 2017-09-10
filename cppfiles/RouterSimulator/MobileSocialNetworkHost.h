#pragma once
#include "Host.h"
#include "TrustEntry.h"
class CMobileSocialNetworkHost :
	public CHost
{
public:
	CMobileSocialNetworkHost();
	virtual ~CMobileSocialNetworkHost();

	void AddTrust(int nHostId, double fTrustValue);
	bool FindTrust(int nHostId, CTrustValue & fTrustValue);
private:
	CMap<int, int, CTrustEntry, CTrustEntry&> m_TrustList;
};

