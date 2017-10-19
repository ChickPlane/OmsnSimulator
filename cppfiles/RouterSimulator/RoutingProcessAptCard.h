#pragma once
#include "RoutingProcess.h"
#include "AppointmentCard.h"
#include "AptCardAgencyRecord.h"
#include "AptCardFromSameAgency.h"
#include "TestRecordAptCard.h"
#include "QueryMission.h"
#include "SentenceAptCard.h"
#include "Yell.h"


enum
{
	AC_TIMER_CMD_HELLO,
	AC_TIMER_CMD_MAX
};

class CQueryRecordEntry
{
public:
	CAppointmentCard * m_pCard;
	int m_nQueryId;
	CQueryRecordEntry() :m_pCard(NULL) {}
	~CQueryRecordEntry()
	{
		if (m_pCard)
		{
			delete m_pCard;
		}
	}
};

class CRoutingProcessAptCard :
	public CRoutingProcess
{
public:
	CRoutingProcessAptCard();
	virtual ~CRoutingProcessAptCard();

	virtual void GenerateMission(const CQueryMission * pMission);
	virtual void OnReceivedMsg(const CYell * pMsg);
	virtual void OnEngineTimer(int nCommandId);
	virtual int GetCarryingMessages(CMsgShowInfo & allMessages) const;
	virtual void TurnOn();
	virtual void SetParameters(int nK, int nSeg, int nCopyCount, double fTrust, SIM_TIME lnAcTimeout);

public:
	void OnCardsRequired(USERID nNextUser, const CPkgAptCardAck * pAck);
	virtual int GetInfoList(CMsgShowInfo & allMessages) const;
	virtual COLORREF GetInportantLevel() const;
	virtual int GetReadyCount() const;
	virtual int GetCreatedCount() const;
	virtual CString GetAgencyListString() const;

protected:
	void PickDispensedCards(USERID nNextUser);
	void CreateNewAptCards();
	void PickReadyCards(int nTheOtherReadyNumber);
	BOOL PickMNumberFromNArr(int nM, char * pArr, int nN);
	int GetLastAcHopCardNumber() const;
	void PrepareToSend(USERID nNextUser);
	CAppointmentCard * CreateSingleNewAptCard(CAptCardFromSameAgency * pCreateList);
	int GetUniqueRandomNumber(SIM_TIME lnTimeOut);
	void SweepExsitingAptNumber();
	CAppointmentCard * SelectRandomAptCardForQuery();
	CAppointmentCard * SelectMaxMarkAptCardForQuery();
	BOOL IsTheLastCardRelay(const CAppointmentCard * pCard) const;

protected:
	virtual void StartBinarySprayWaitProcess();
	virtual void ContinueBinarySprayWaitProcess();
	virtual void CleanTimeOutData();

	virtual void SendHello();
	virtual void OnReceivedHello(CRoutingProtocol * pSender, const CPkgAptCardHello * pHello, CList<CSentence*> & Answer);
	virtual void OnReceivedHelloAck(CRoutingProtocol * pSender, const CPkgAptCardAck * pAck, CList<CSentence*> & Answer);
	virtual void OnReceivedCards(CRoutingProtocol * pSender, const CPkgAptCardCards * pCards, CList<CSentence*> & Answer);

	virtual void ForwardQuery(CRoutingProtocol * pTo, CList<CSentence*> & Answer);
	virtual void OnReceivedQuery(CRoutingProtocol * pSender, const CPkgAptCardQuery * pQuery, CList<CSentence*> & Answer);

	virtual void ForwardReply(CRoutingProtocol * pTo, const CPkgAptCardAck * pAck, CList<CSentence*> & Answer);
	virtual void OnReceivedReply(CRoutingProtocol * pSender, const CPkgAptCardReply * pReply, CList<CSentence*> & Answer);

	virtual void PrepareAck(CRoutingProtocol * pTo, CList<CSentence*> & Answer, BOOL bBack);
	virtual void PrepareAllWaitingQueries();
	virtual void PrepareQuery(const CQueryMission * pMission, CAppointmentCard * pSelectedAptCard);
	virtual CPkgAptCardReply * GetRelayReply(const CPkgAptCardReply * pReply);

	virtual CPkgAptCardReply * LbsPrepareReply(const CPkgAptCardQuery * pQuery);

	CPkgBswData * CheckDuplicateInReplyList(int nBswId);
	CPkgBswData * CheckDuplicateInQueryList(int nBswId);
	static BOOL IsReplyTmpDestination(CPseudonymPair * pPseudonymPairs, int nPseudonymNumber, const CPseudonymPair & replyTarget);
	static POSITION IsReplyTmpDestination(CList<CPseudonymPairRecord> & m_PseduonymPairs, const CPseudonymPair & replyTarget);
	BOOL IsFriend(CRoutingProtocol * pOther);
	BOOL IsLongTimeNoSee(CRoutingProtocol * pOther);
	void CleanSendingList();

	void ResetAll();
	void CleanTimeoutAgencyList();
	int GetMark();
	double TestAptCardMark(CAppointmentCard * pCard);
	void CheckAptCards(const CAppointmentCard * pCard);

protected:
	int m_nK;
	int m_nSeg;
	int m_nCopyCount;
	double m_fTrust;
	SIM_TIME m_lnAptCardsTimeout;

	CList<CAppointmentCard*> m_DispensedCards;
	CList<CAppointmentCard*> m_ReadyCards;
	CList<CAppointmentCard*> m_SendingList;
	CMap<USERID, USERID, CAptCardFromSameAgency*, CAptCardFromSameAgency*> m_AgencyList;
	int m_nNeededCardNumber;
	SIM_TIME m_lnLastCreateAptCardTime;
	SIM_TIME m_lnLastCleanTime;

	bool m_bWork;
	bool m_bBswLogSwitch;
	CList<CPkgAptCardQuery*> m_QueryList;
	CList<CPkgAptCardReply*> m_ReplyList;

	CList<CQueryMission*> m_WaitingMissions;
	CList<CPseudonymPairRecord> m_PseduonymPairs;

private:
	CMap<int, int, SIM_TIME, SIM_TIME> m_ExistingCaptNumbers;
	CList<CQueryRecordEntry*> m_MissionStatisticRecords;
	CMap<int, int, CTestRecordAptCard *, CTestRecordAptCard *> m_allSessions;
	CList<SIM_TIME> m_EncounterUserList;

	CMap<CRoutingProtocol*, CRoutingProtocol*, SIM_TIME, SIM_TIME> m_lastTimeExchange;
	CMap<int, int, SIM_TIME, SIM_TIME> m_DelieverMap;
	CMap<int, int, SIM_TIME, SIM_TIME> m_RelayMap;
};

