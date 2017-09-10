#pragma once

class CRouterSimulatorDoc;
class CRoadNet;

enum {
	MSG_ID_START_DIJ = WM_USER + 100,
	MSG_ID_SET_HOSTS
};

// CRouterRunner

class CRouterRunner : public CWinThread
{
	DECLARE_DYNCREATE(CRouterRunner)

protected:
	CRouterRunner();           // 动态创建所使用的受保护的构造函数
	virtual ~CRouterRunner();

	CRouterSimulatorDoc * m_pDoc;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void SetDoc(CRouterSimulatorDoc * pDoc);

protected:
	DECLARE_MESSAGE_MAP()
	void OnStartDij(WPARAM wParam, LPARAM lParam);
	void OnCreateHosts(WPARAM wParam, LPARAM lParam);
	void CreateSocialRelation();
	void CreateSocialEvenRelation();
	void CreateHosts(int nHostCount, int nTryMax);
	void CreateHostsWithFixedZero();
	double GenerateRandomSpeed(double fMin, double fMax);
};

