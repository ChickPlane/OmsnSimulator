
// RouterSimulatorDoc.h : CRouterSimulatorDoc ��Ľӿ�
//


#pragma once
#include "LineInMap.h"
#include "SimulatorCfg.h"

class CRoadNet;
class CRouterRunner;

enum {
	DOC_MESSAGE_UPDATE_ALL = WM_USER + 100
};

class CRouterSimulatorDoc : public CDocument
{
protected: // �������л�����
	CRouterSimulatorDoc();
	DECLARE_DYNCREATE(CRouterSimulatorDoc)

// ����
public:
	CRoadNet * m_pRoadNet;
	CList<CLineInMap> * m_pLineInMap;
	CRouterRunner * m_pRun;
	CSimulatorCfg m_Cfg;

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CRouterSimulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	void OnUpdateAll();
};
