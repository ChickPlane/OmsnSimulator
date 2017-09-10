#include "stdafx.h"
#include "MobileSocialNetworkHost.h"


CMobileSocialNetworkHost::CMobileSocialNetworkHost()
{
}


CMobileSocialNetworkHost::~CMobileSocialNetworkHost()
{
}

void CMobileSocialNetworkHost::AddTrust(int nHostId, double fTrustValue)
{
	CTrustEntry newEntry;
	newEntry.m_nHostId = nHostId;
	newEntry.m_fTrustValue = fTrustValue;
	m_TrustList[nHostId] = newEntry;
}

bool CMobileSocialNetworkHost::FindTrust(int nHostId, CTrustValue & fTrustValue)
{
	CTrustEntry findResult;
	if (m_TrustList.Lookup(nHostId, findResult))
	{
		fTrustValue = findResult.m_fTrustValue;
		return true;
	}
	return false;
}
