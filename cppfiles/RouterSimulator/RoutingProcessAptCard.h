#pragma once
#include "RoutingProcess.h"
#include "AppointmentCard.h"
#include "AptCardAgencyRecord.h"
#include "AptCardFromSameAgency.h"
#include "TestRecordAptCard.h"
#include "QueryMission.h"
#include "SentenceAptCard.h"
#include "Yell.h"

class CIdAndApt
{
public:
	int nId;
	int nApt;
	int nGroup;
};

class CRoutingProcessAptCard;

class CRoutingProcessACUser
{
public:
	virtual BOOL IsTrustful(CRoutingProcessAptCard * pCallBy, const CRoutingProtocol * pOther) const = 0;

	virtual void OnGetNewCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg) = 0;
	virtual void OnGetNoneCards(CRoutingProcessAptCard * pCallBy, const CPkgAptCardCards * pPkg) = 0;
};

class CRoutingProcessAptCard :
	public CRoutingProcess
{
public:
	CRoutingProcessAptCard();
	virtual ~CRoutingProcessAptCard();

	virtual void SetBasicParameters(int nProcessID, CRoutingProtocol * pProtocol);
	virtual void SetProcessUser(CRoutingProcessACUser * pUser) { m_pUser = pUser; }
	virtual CPkgAptCardCards * GetSendingList(BOOL bNeedReady, int nHoldingCount, CRoutingProtocol * pTo);
	virtual void OnReceivePkgFromNetwork(const CSentence * pPkg, CList<CSentence*> & SendingList);
	CAppointmentCard * SelectMaxMarkAptCardForQuery(SIM_TIME lnTimeout);
	static void CleanSendingList(CList<CAppointmentCard*> & SendingList);
	virtual void OnReceivedCards(const CPkgAptCardCards * pCards);

	virtual int GetAllAcListSize() const;
	virtual int GetTrustListSize() const { return m_TrustCards.GetSize(); }
	virtual int GetReadyListSize() const { return m_ReadyCards.GetSize(); }
	virtual int GetDispenseListSize() const { return m_DispensedCards.GetSize(); }

	virtual void OnEngineTimer(int nCommandId);
	virtual int GetCarryingMessages(CMsgShowInfo & allMessages) const;
	static void SetParameters(int nK, int nSeg, SIM_TIME lnAcTimeout);
	virtual BOOL GetAndRemoveAgencyRecord(USERID uOldId, int nOldApt, CAptCardAgencyRecord & retRecord);

public:
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual COLORREF GetInportantLevel() const;
	virtual int GetCreatedCount() const;
	virtual CString GetAgencyListString() const;

protected:
	void PickDispensedCards(USERID nNextUser, CList<CAppointmentCard*> & SendingList);
	void CreateNewAptCards(CList<CAppointmentCard*> & SendingList);
	void PickReadyAndTrustCards(int nTheOtherReadyNumber, CList<CAppointmentCard*> & SendingList);
	
	static int GetLastAcHopCardNumber(const CList<CAppointmentCard*> & SendingList);
	void PrepareToSend(USERID nNextUser, CList<CAppointmentCard*> & SendingList);

	CAppointmentCard * CreateSingleNewAptCard(CAptCardFromSameAgency * pCreateList);
	int GetUniqueRandomNumber(SIM_TIME lnTimeOut);
	void SweepExsitingAptNumber();
	static BOOL IsTheLastCardRelay(const CAppointmentCard * pCard);

protected:
	virtual void CleanTimeOutData();

	virtual void SendHello();
	void ResetAll();
	void CleanTimeoutAgencyList();
	int GetMark();
	double TestAptCardMark(CAppointmentCard * pCard, SIM_TIME lnTimeout);
	void CheckAptCards(const CAppointmentCard * pCard);
	int GetReadyCount() const;
	BOOL CheckDuplicated();

protected:
	static int gm_nK;
	static int gm_nSeg;
	static SIM_TIME gm_lnAptCardsTimeout;

	CList<CAppointmentCard*> m_DispensedCards;
	CList<CAppointmentCard*> m_ReadyCards;
	CList<CAppointmentCard*> m_TrustCards;
	CMap<USERID, USERID, CAptCardFromSameAgency*, CAptCardFromSameAgency*> m_AgencyList;
	int m_nNeededCardNumber;
	SIM_TIME m_lnLastCreateAptCardTime;
	SIM_TIME m_lnLastCleanTime;

	bool m_bWork;
	bool m_bBswLogSwitch;

private:
	CMap<int, int, SIM_TIME, SIM_TIME> m_ExistingCaptNumbers;
	CList<SIM_TIME> m_EncounterUserList;

	CMap<int, int, SIM_TIME, SIM_TIME> m_DelieverMap;
	CMap<int, int, SIM_TIME, SIM_TIME> m_RelayMap;

	CRoutingProcessACUser * m_pUser;
};

