
// RouterSimulatorDoc.h : CRouterSimulatorDoc 类的接口
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
protected: // 仅从序列化创建
	CRouterSimulatorDoc();
	DECLARE_DYNCREATE(CRouterSimulatorDoc)

// 特性
public:
	CRoadNet * m_pRoadNet;
	CList<CLineInMap> * m_pLineInMap;
	CRouterRunner * m_pRun;
	CSimulatorCfg m_Cfg;

// 操作
public:

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CRouterSimulatorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	void OnUpdateAll();
};
