#pragma once
class CSimulatorCfg
{
public:
	CSimulatorCfg();
	CSimulatorCfg& operator = (const CSimulatorCfg & src);
	virtual ~CSimulatorCfg();
	void ReadFromFile(char * strSettingFilePath);

	static void CopyString(char ** pStrDes, char * strSrc);
	static char * GetFolderName(char * pPathName);
	void GetFolderPathByParams(char * strRet, int nRetLen) const;
	const char * GetFolderNameByParams();
	void GetParametersString(char * strRet, int nRetLen) const;

	char * m_strSettingFilePath;

	char * m_strComment;
	char * m_strWorkFolder;
	char * m_strMapName;
	char * m_strProtocolName;
	int m_nNodeCount;
	double m_fCommunicateRadius;
	int m_nBswCopyCount;

	int m_nK;
	double m_fPrivacyHigh;
	double m_fPrivacyLow;
	double m_fAnonyRadius;
	int m_nTimeOutSecond;

private:
	char m_strFolderName[200];

};

