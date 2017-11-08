
// RouterSimulatorDoc.cpp : CRouterSimulatorDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "RouterSimulator.h"
#endif

#include "RouterSimulatorDoc.h"

#include <propkey.h>
#include "WktParse.h"
#include "RoadNet.h"
#include "RouterRunner.h"
#include <direct.h>
#include "CommonFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRouterSimulatorDoc

IMPLEMENT_DYNCREATE(CRouterSimulatorDoc, CDocument)

BEGIN_MESSAGE_MAP(CRouterSimulatorDoc, CDocument)
	ON_COMMAND(MSG_ID_START_DIJ, OnUpdateAll)
END_MESSAGE_MAP()


// CRouterSimulatorDoc 构造/析构

CRouterSimulatorDoc::CRouterSimulatorDoc()
	:m_pRoadNet(NULL)
	, m_pLineInMap(NULL)
{
	// TODO: 在此添加一次性构造代码
	m_pRoadNet = new CRoadNet();
	m_pRun = NULL;
}

CRouterSimulatorDoc::~CRouterSimulatorDoc()
{
	delete m_pRoadNet;
	m_pRoadNet = NULL;
}

BOOL CRouterSimulatorDoc::OnNewDocument()
{
	return FALSE;

	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CRouterSimulatorDoc 序列化

void CRouterSimulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CRouterSimulatorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CRouterSimulatorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CRouterSimulatorDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRouterSimulatorDoc 诊断

#ifdef _DEBUG
void CRouterSimulatorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRouterSimulatorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRouterSimulatorDoc 命令


BOOL CRouterSimulatorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CString strFilename(lpszPathName);

	int len = WideCharToMultiByte(CP_ACP, 0, strFilename, -1, NULL, 0, NULL, NULL);
	char *filename = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strFilename, -1, filename, len, NULL, NULL);

	m_Cfg.ReadFromFile(filename);
	delete[] filename;

	char newFolderPath[200] = {0};
	m_Cfg.GetFolderPathByParams(newFolderPath, 200);
	if (ENOENT == _mkdir(newFolderPath))
	{
		AfxMessageBox(_T("Create folder error!"));
		return FALSE;
	}

	double fLeft = DBL_MAX, fRight = 0.0, fTop = DBL_MAX, fBottom = 0.0;
	m_pLineInMap = CWktParse::Parse(m_Cfg.m_strMapName, fLeft, fRight, fTop, fBottom);
	if (m_pLineInMap == NULL)
	{
		AfxMessageBox(_T("MAP PATH ERROR!"));
		return FALSE;
	}

	m_pRoadNet->Init(*m_pLineInMap, fLeft, fRight, fTop, fBottom);

	m_pRun = (CRouterRunner*)AfxBeginThread(RUNTIME_CLASS(CRouterRunner));
	m_pRun->SetDoc(this);
	m_pRun->PostThreadMessage(MSG_ID_START_DIJ,0,0);

	// TODO:  在此添加您专用的创建代码

	return TRUE;
}


void CRouterSimulatorDoc::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类
	DWORD ExitCode = 0;
	if (m_pRun)
	{
		GetExitCodeThread(m_pRun->m_hThread, &ExitCode);
		TerminateThread(m_pRun->m_hThread, ExitCode);
	}
	//AfxEndThread(ExitCode, true);
	if (m_pLineInMap)
	{
		delete m_pLineInMap;
		m_pLineInMap = NULL;
	}

	CDocument::OnCloseDocument();
}

void CRouterSimulatorDoc::OnUpdateAll()
{
	UpdateAllViews(NULL);
}
