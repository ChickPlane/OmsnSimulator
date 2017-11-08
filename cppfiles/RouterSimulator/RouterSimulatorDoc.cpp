
// RouterSimulatorDoc.cpp : CRouterSimulatorDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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


// CRouterSimulatorDoc ����/����

CRouterSimulatorDoc::CRouterSimulatorDoc()
	:m_pRoadNet(NULL)
	, m_pLineInMap(NULL)
{
	// TODO: �ڴ����һ���Թ������
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

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CRouterSimulatorDoc ���л�

void CRouterSimulatorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CRouterSimulatorDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
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

// ������������֧��
void CRouterSimulatorDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
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

// CRouterSimulatorDoc ���

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


// CRouterSimulatorDoc ����


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

	// TODO:  �ڴ������ר�õĴ�������

	return TRUE;
}


void CRouterSimulatorDoc::OnCloseDocument()
{
	// TODO: �ڴ����ר�ô����/����û���
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
