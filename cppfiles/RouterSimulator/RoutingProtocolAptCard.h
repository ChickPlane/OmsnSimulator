#pragma once
#include "RoutingProtocol.h"
#include "RoutingProcessHello.h"
#include "RoutingProcessBSW.h"
#include "RoutingProcessAptCard.h"

class CRoutingProtocolAptCard :
	public CRoutingProtocol,
	public CRoutingProcessHelloUser,
	public CRoutingProcessBswUser,
	public CRoutingProcessACUser
{
public:
	CRoutingProtocolAptCard();
	virtual ~CRoutingProtocolAptCard();

	virtual void CreateQueryMission(const CQueryMission * pMission);
	static void SetStaticParameters(int nK, int nSeg, double fTrust, SIM_TIME lnAcTimeout);
	virtual void SetLocalParameters(int nBswCopyCount);
	virtual COLORREF GetImportantLevel() const;
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual void Turn(BOOL bOn);
	virtual int GetCarryingPkgNumber(int nParam);
	CString GetDebugString() const;
	virtual void OnEngineConnection(BOOL bAnyOneNearby, BOOL bDifferentFromPrev);

public:
	// Hello User
	virtual void OnHearHelloFromOthers(CRoutingProcessHello * pCallBy, const CPkgHello * pPkg) {};
	virtual void OnBuiltConnectWithOthers(CRoutingProcessHello * pCallBy, const CPkgAck * pPkg);
	virtual CPkgHello * GetHelloPackage(CRoutingProcessHello * pCallBy);
	virtual CPkgAck * GetAckPackage(CRoutingProcessHello * pCallBy, CRoutingProtocol * pTo);

	// BSW User
	virtual BOOL IsTheLastHop(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg, const CRoutingProtocol * pTheOther, const CSentence * pTriger);
	virtual BOOL IsPackageForMe(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual CPkgBswData * CopyPackage(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnBswPkgReachDestination(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);
	virtual void OnPackageFirstSent(CRoutingProcessBsw * pCallBy, const CPkgBswData * pPkg);

	// APT CARD User
	virtual BOOL IsTrustful(CRoutingProcessAptCard * pCallBy, const CRoutingProtocol * pOther) const;
	virtual void OnGetNewCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg);
	virtual void OnGetNoneCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg);

protected:
	BOOL IsFriend(const CRoutingProtocol * pOther) const;
	BOOL IsLongTimeNoSee(const CRoutingProtocol * pOther) const;
	CRoutingProcessAptCard * GetAptCardProcess() const;
	CRoutingProcessBsw * GetQueryProcess() const;
	CRoutingProcessBsw * GetReplyProcess() const;
	CRoutingProcessHello * GetHelloProcess() const;

	static CPkgAptCardReply * LbsPrepareReply(const CPkgAptCardQuery * pQuery);
	virtual void PrepareAllWaitingQueries();
	virtual void PrepareQuery(const CQueryMission * pMission, CAppointmentCard * pSelectedAptCard);
	virtual void CleanPseudonyms();
	virtual void RemovePsedunym(const CPseudonymPair & replyTarget);

	static BOOL IsReplyTmpDestination(CPseudonymPair * pPseudonymPairs, int nPseudonymNumber, const CPseudonymPair & replyTarget);
	static POSITION GetPseudonymPos(const CList<CPseudonymPairRecord> & m_PseduonymPairs, const CPseudonymPair & replyTarget);

	void ResetAll();

	virtual CPkgAptCardReply * SwitchToNextAgency(const CPkgAptCardReply * pReply);
	void SendQueries(CRoutingProtocol * pTheOther);

private:
	int m_nHelloProcessId;
	int m_nQueryBswProcessId;
	int m_nReplyBswProcessId;
	int m_nAptCardProcessId;

	static double gm_fTrust;
	CMap<const CRoutingProtocol*, const CRoutingProtocol*, SIM_TIME, SIM_TIME> m_lastTimeExchange;
	CList<CQueryMission*> m_WaitingMissions;
	CList<CPseudonymPairRecord> m_PseduonymPairs;
	int m_nSentAcCardPkgNumber;
	int m_nSentAcCardNumber;
};

