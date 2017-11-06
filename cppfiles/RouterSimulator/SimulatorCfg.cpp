#include "stdafx.h"
#include "SimulatorCfg.h"
#include <fstream>
using namespace std;


CSimulatorCfg::CSimulatorCfg()
	: m_strSettingFilePath(NULL)
	, m_strWorkFolder(NULL)
	, m_strComment(NULL)
	, m_strMapName(NULL)
	, m_strProtocolName(NULL)
	, m_nNodeCount(0)
	, m_fCommunicateRadius(0)
	, m_nBswCopyCount(0)
	, m_nK(0)
	, m_fPrivacyHigh(0)
	, m_fPrivacyLow(0)
	, m_fAnonyRadius(0)
	, m_nTimeOutSecond(0)
{
}

CSimulatorCfg& CSimulatorCfg::operator=(const CSimulatorCfg & src)
{
	CopyString(&m_strSettingFilePath, src.m_strSettingFilePath);
	CopyString(&m_strWorkFolder, src.m_strWorkFolder);
	CopyString(&m_strMapName, src.m_strMapName);
	CopyString(&m_strProtocolName, src.m_strProtocolName);
	m_nNodeCount = src.m_nNodeCount;
	m_fCommunicateRadius = src.m_fCommunicateRadius;
	m_nBswCopyCount = src.m_nBswCopyCount;
	m_nK = src.m_nK;
	m_fPrivacyHigh = src.m_fPrivacyHigh;
	m_fPrivacyLow = src.m_fPrivacyLow;
	m_fAnonyRadius = src.m_fAnonyRadius;
	m_nTimeOutSecond = src.m_nTimeOutSecond;
	return *this;
}

CSimulatorCfg::~CSimulatorCfg()
{
	if (m_strSettingFilePath)
	{
		delete m_strSettingFilePath;
	}
	if (m_strWorkFolder)
	{
		delete m_strWorkFolder;
	}
	if (m_strMapName)
	{
		delete m_strMapName;
	}
	if (m_strProtocolName)
	{
		delete m_strProtocolName;
	}
}

#define LINE_LENGTH 200

void CSimulatorCfg::ReadFromFile(char * strSettingFilePath)
{
	CopyString(&m_strSettingFilePath, strSettingFilePath);
	char inputBuffer[LINE_LENGTH+1];
	ifstream fin(m_strSettingFilePath);

	fin.getline(inputBuffer, LINE_LENGTH);
	CopyString(&m_strWorkFolder, inputBuffer);
	CopyString(&m_strComment, GetFolderName(m_strWorkFolder));
	fin.getline(inputBuffer, LINE_LENGTH);
	CopyString(&m_strMapName, inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	CopyString(&m_strProtocolName, inputBuffer);

	fin.getline(inputBuffer, LINE_LENGTH);
	m_nNodeCount = atoi(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_fCommunicateRadius = atof(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_nBswCopyCount = atoi(inputBuffer);

	fin.getline(inputBuffer, LINE_LENGTH);
	m_nK = atoi(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_fPrivacyHigh = atof(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_fPrivacyLow = atof(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_fAnonyRadius = atof(inputBuffer);
	fin.getline(inputBuffer, LINE_LENGTH);
	m_nTimeOutSecond = atoi(inputBuffer);

	fin.close();
}

void CSimulatorCfg::CopyString(char ** pStrDes, char * strSrc)
{
	if (*pStrDes)
	{
		delete *pStrDes;
		*pStrDes = NULL;
	}
	if (strSrc)
	{
		int nLength = strlen(strSrc);
		*pStrDes = new char[nLength + 1];
		strcpy_s(*pStrDes, nLength + 1, strSrc);
	}
}

char * CSimulatorCfg::GetFolderName(char * pPathName)
{
	int nPathLength = strlen(pPathName);
	for (int i = nPathLength; i >= 0; --i)
	{
		if (pPathName[i] == '\\' || pPathName[i] == '/')
		{
			return pPathName + i + 1;
		}
	}
	return NULL;
}
