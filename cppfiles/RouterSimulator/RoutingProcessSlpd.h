#pragma once
#include "RoutingProcess.h"
#include "Sentence.h"
#include "TimeOutPair.h"
#include "SentenceSlpd.h"
#include "RoutingProtocol.h"
#include "QueryMission.h"

class CRoutingProcessSlpd;

class CRoutingProcessSlpdUser
{
public:
	virtual BOOL IsTrustful(CRoutingProcessSlpd * pCallBy, const CRoutingProtocol * pOther) const = 0;
	virtual CPkgSlpd * GetSlpdDataCopy(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg);

	virtual void OnFirstSlpdObfuscationForward(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg) {}
	virtual void OnNewSlpdPseudo(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg) {}
	virtual void OnNewSlpdPseudoOver(CRoutingProcessSlpd * pCallBy, const CPkgSlpd * pPkg) = 0;
};

class CRoutingProcessSlpd :
	public CRoutingProcess
{
public:
	CRoutingProcessSlpd();
	virtual ~CRoutingProcessSlpd();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	static void SetParameters(int nK);
	virtual void SetProcessUser(CRoutingProcessSlpdUser * pUser) { m_pUser = pUser; }
	virtual void SetK(int nK) { m_nK = nK; }
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);
	virtual void OnEncounterUser(CRoutingProtocol * pTheOther, CList<CSentence *> & SendingList);
	virtual void CreateQueryMission(CPkgSlpd * pPkg);

	virtual CList<CTimeOutPair<CSlpdUserAndPseudo>> & GetPseudonymList() { return m_PseudonymList; }
	virtual BOOL IsInPseudonymList(SLPD_USERID lnPseudonym, BOOL bAutoDelete, CSlpdUserAndPseudo & ret);

	virtual int GetObfuscationCount() const;

protected:
	virtual void OnReceiveNewPseudoPkg(const CPkgSlpd * pPkg);

private:
	CRoutingProcessSlpdUser * m_pUser;
	CList<CTimeOutPair<CSlpdUserAndPseudo>> m_PseudonymList;
	CList<CTimeOutPair<CPkgSlpd *>> m_ForwardingList;
	static int gm_PseudonymMax;
	static int m_nK;
};

