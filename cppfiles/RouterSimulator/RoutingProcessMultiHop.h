#pragma once
#include "RoutingProcess.h"
#include "SentenceMultiHop.h"

class CRoutingProcessMultiHop;

class CRoutingProcessMultiHopUser
{
public:
	virtual CRoutingProtocol * GetNextHop(CRoutingProcessMultiHop * pCallBy, USERID nDestinationId) = 0;
	virtual CPkgMultiHop * GetMultiHopDataCopy(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg);

	virtual void OnMultiHopMsgArrived(CRoutingProcessMultiHop * pCallBy, const CPkgMultiHop * pPkg) {}
};

class CRoutingProcessMultiHop :
	public CRoutingProcess
{
public:
	CRoutingProcessMultiHop();
	virtual ~CRoutingProcessMultiHop();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	static void SetParameters(double fObfuscationRadius) { gm_fObfuscationRadius = fObfuscationRadius; }
	virtual void SetProcessUser(CRoutingProcessMultiHopUser * pUser) { m_pUser = pUser; }
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);
	virtual void OnEncounterUser(CRoutingProtocol * pTheOther, CList<CSentence *> & SendingList) {}
	virtual void SendPkgToMultiHopHost(const CPkgMultiHop * pPkg);

protected:
	virtual void OnReceivedDataPkg(const CPkgMultiHop * pPkg);
private:
	CRoutingProcessMultiHopUser * m_pUser;
	static double gm_fObfuscationRadius;
};

