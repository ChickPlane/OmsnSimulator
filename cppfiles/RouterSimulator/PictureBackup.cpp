#include "stdafx.h"
#include "PictureBackup.h"



CPictureBackup::CPictureBackup()
{
	m_nVersion = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_pInsideDc = NULL;
}

void CPictureBackup::Init(int nWidth, int nHeight, CDC * pDC)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_Image.Create(m_nWidth, m_nHeight, DEFAULT_BPP);
	m_ImageBackup.Create(m_nWidth, m_nHeight, DEFAULT_BPP);
	Save(0);
}

void CPictureBackup::Reset()
{
	m_nVersion = 0;
	ReleaseInsideDc();
	if (!m_ImageBackup.IsNull())
	{
		m_ImageBackup.Destroy();
	}
	if (!m_Image.IsNull())
	{
		m_Image.Destroy();
	}
	m_nWidth = 0;
	m_nHeight = 0;
}

void CPictureBackup::FillSolidRect(COLORREF color)
{
	CDC * pImgDc = GetInsideDc();
	pImgDc->FillSolidRect(0, 0, m_nWidth, m_nHeight, color);
	ReleaseInsideDc();
}

void CPictureBackup::BitBlt(CDC * pDC, CDC & MemDC)
{
	m_Image.Draw(pDC->m_hDC, 0, 0);
}

void CPictureBackup::BitBlt(CDC * pDC, CDC & MemDC, CBitmap & MemBitmap)
{
	int nWidth, nHeight;
	MemDC.SelectObject(&MemBitmap);
	pDC->BitBlt(0, 0, m_nWidth, m_nHeight, &MemDC, 0, 0, SRCCOPY);
}

void CPictureBackup::Save(int nVersion)
{
	m_nVersion = nVersion;
}

void CPictureBackup::Load(int xDest, int yDest, int nDestWidth, int nDestHeight)
{
	m_ImageBackup.BitBlt(m_Image.GetDC(), xDest, yDest, nDestWidth, nDestHeight, xDest, yDest);
	m_Image.ReleaseDC();
}

int CPictureBackup::GetVersion()
{
	return m_nVersion;
}

CDC * CPictureBackup::GetInsideDc()
{
	if (m_pInsideDc)
	{
		ASSERT(0);
		return m_pInsideDc;
	}
	m_pInsideDc = CDC::FromHandle(m_ImageBackup.GetDC());
	return m_pInsideDc;
}

void CPictureBackup::ReleaseInsideDc()
{
	if (m_pInsideDc)
	{
		m_ImageBackup.ReleaseDC();
		m_pInsideDc = NULL;
	}
}

CPictureBackup::~CPictureBackup()
{
	Reset();
}
