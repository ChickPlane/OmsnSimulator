#pragma once

#define DEFAULT_BPP 32

class CPictureBackup
{
public:
	CPictureBackup();
	void Init(int nWidth, int nHeight, CDC * pDC);
	void Reset();
	void FillSolidRect(COLORREF color);
	void BitBlt(CDC * pDC, CDC & MemDC);
	void BitBlt(CDC * pDC, CDC & MemDC, CBitmap & MemBitmap);
	void Save(int nVersion);
	void Load(int xDest, int yDest, int nDestWidth, int nDestHeight);
	int GetVersion();
	CDC * GetInsideDc();
	void ReleaseInsideDc();
	virtual ~CPictureBackup();
	CImage m_Image;
private:
	int m_nVersion;
	int m_nWidth;
	int m_nHeight;
	CImage m_ImageBackup;
	CDC * m_pInsideDc;
};

