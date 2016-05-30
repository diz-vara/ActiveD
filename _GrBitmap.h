#pragma once
#include "stdafx.h"


class _CSize : public CSize
{
};


class _GrBitmap
{

public:
  _GrBitmap(HDC hDC, CRect *prc);
  _GrBitmap(_GrBitmap &Gr);
  ~_GrBitmap(void);
  _GrBitmap* SetNext(_GrBitmap* pNext = 0);
  HBITMAP GetBitmap() {return m_hBmp;}
  _GrBitmap* GetNext() {return m_pNext;}
  _GrBitmap* GetFree(CRect *prc = 0);
  int GetH() const {return m_nHeight;}
  int GetW() const {return m_nWidth;}
  void Resize(CRect *rc);
  void SetUsed() {m_bFree = false; m_nBegPix = 0;}
  void SetFree() {m_bFree = true;}
  void FillSolidRect(CRect *prc, COLORREF color) 
  {
    m_hDC.FillSolidRect(prc, color);
    m_nEndPix = prc->right - 1;

  }

  void Line(HPEN Pen, int x0, int y0, int x1, int y1);
  void PolyLine(CPen Pen, POINT* pPnt, int nPnt);
  void SetPixels(int nX, int nY, COLORREF color);
  _GrBitmap* Copy(HDC hDC, CRect rcDst, CRect rcSrc = CRect(0,0,-1,-1));

  //050413 - ��� �������: ����� ������
  int m_nBegFrame;
  int m_nBegPx;

private:
  HBITMAP m_hBmp;
  HBITMAP m_hOldBmp;
  CDC m_hDC;
  HDC m_hParDC;

  //������ ���� � ��
  DWORD m_dwMsStart;
  //����� ���� � ��
  DWORD m_dwMsEnd;
  //������ ���� � �������
  DWORD m_dwSampleStart;
  //����� ���� � �������
  DWORD m_dwSampleEnd;

  //������ ����
  int m_nHeight;
  //������ ���� (������)
  int m_nWidth;
  //������ ���� (�������)
  int m_nLength;

  //050405 - ����� ������ �����������
  int m_nBegPix;
  int m_nEndPix;

  bool m_bFree;
  _GrBitmap* m_pNext;





};
