#include "StdAfx.h"
#include ".\_grbitmap.h"
#include "ActD.h"

_GrBitmap::_GrBitmap(HDC hDC = 0, CRect *prc = 0) :
      m_dwMsStart(0),
      m_dwMsEnd(0),
      m_dwSampleStart(0),
      m_dwSampleEnd(0),
      m_nHeight(0),
      m_nWidth(0),
      m_nLength(0),
      m_bFree(true),
      m_pNext(0),
      m_hDC(0), m_hParDC(hDC)

{
  if (prc)
  {
    m_nWidth = prc->Width();
    m_nHeight = prc->Height();
  }
  m_hDC.CreateCompatibleDC(m_hParDC);
  if (m_hDC)
 	  m_hBmp = CreateCompatibleBitmap(hDC,m_nWidth+1,m_nHeight);
  m_hOldBmp = m_hDC.SelectBitmap(m_hBmp);
  ATLTRACE("_GrBitmap::ctr %08x  ",this);
  ATLTRACE("Bitmap creation %d %d\n", m_nWidth, m_nHeight);
}

_GrBitmap::_GrBitmap(_GrBitmap &Gr):
      m_dwMsStart(Gr.m_dwMsStart),
      m_dwMsEnd(Gr.m_dwMsEnd),
      m_dwSampleStart(Gr.m_dwSampleStart),
      m_dwSampleEnd(Gr.m_dwSampleEnd),
      m_nHeight(Gr.m_nHeight),
      m_nWidth(Gr.m_nWidth),
      m_nLength(Gr.m_nLength),
      m_bFree(true),
      m_pNext(0), m_hParDC(Gr.m_hParDC)
{
  m_hDC.CreateCompatibleDC(m_hParDC);
  if (m_hDC)
 	  m_hBmp = CreateCompatibleBitmap(m_hParDC,m_nWidth+1,m_nHeight);
  m_hOldBmp = m_hDC.SelectBitmap(m_hBmp);
  //ATLTRACE("Bitmap copy %d %d\n", m_nWidth, m_nHeight);

}


_GrBitmap::~_GrBitmap(void)
{
  _GrBitmap *pNext = m_pNext;
  m_pNext = 0;
  CancelDC(m_hDC);
  if (pNext && pNext != this && pNext->m_pNext)
  {
    delete pNext;
  }
  ATLTRACE("~~_GrBitmap::dtr %08X\n",this);
  m_hDC.SelectBitmap(m_hOldBmp);
 	SAFE_DELETE_HANDLE(m_hBmp);
  DeleteDC(m_hDC);
}

 _GrBitmap* _GrBitmap::SetNext(_GrBitmap* pNext)
 {
   m_pNext = pNext; 
   return pNext;
 }

 _GrBitmap* _GrBitmap::GetFree(CRect *prc)
{
  _GrBitmap *pN(this);

  if (!m_bFree && m_pNext)
    pN = m_pNext;
  if (!pN->m_bFree)
  {
    _GrBitmap* pNew = new _GrBitmap(*this);
    pNew->SetNext(m_pNext);
    m_pNext = pNew;
    pN = pNew;
  }

  pN->Resize(prc);
  pN->m_nEndPix = 0;

  return pN;
}

void _GrBitmap::Resize(CRect *prc)
{

  if (prc)
  {
    if (prc->Height() != m_nHeight || prc->Width() != m_nWidth)
    {
      m_hDC.SelectBitmap(m_hOldBmp);
      SAFE_DELETE_HANDLE(m_hBmp);
      m_nHeight = prc->Height();
      m_nWidth = prc->Width();
      if (m_hDC)
 	      m_hBmp = CreateCompatibleBitmap(m_hDC,m_nWidth,m_nHeight);
      m_hOldBmp = m_hDC.SelectBitmap(m_hBmp);
    }
    //тестовое заполнение синим цветом
    //FillSolidRect(prc,RGB(0,0,200));
  }
  else
  {
    m_hDC.SelectBitmap(m_hOldBmp);
    SAFE_DELETE_HANDLE(m_hBmp);
    m_nHeight = 0;
    m_nWidth = 0;
  }
  //ATLTRACE("Bitmap resize %d %d\n", nW, prc->Height());

}


// 050404 пн
///////////
void _GrBitmap::Line(HPEN Pen, int x0, int y0, int x1, int y1)
{
CPoint pnt;
  HPEN pnOld = m_hDC.SelectPen(Pen);
  //int nROP =  m_hDC.GetROP2();
  //m_hDC.SetROP2(R2_XORPEN);
  m_hDC.MoveTo(x0,y0);
  m_hDC.LineTo(x1,y1);
  //m_hDC.SetROP2(nROP);
  

  m_hDC.SelectPen(pnOld);

}

void _GrBitmap::PolyLine(CPen Pen, POINT* pPnt, int nPnt)
{
  HPEN pnOld = m_hDC.SelectPen(Pen);
  
  m_hDC.MoveTo(pPnt[0].x, pPnt[0].y);
  m_hDC.Polyline(pPnt, nPnt);
  
  m_hDC.SelectPen(pnOld);
  m_nEndPix = pPnt[nPnt-1].x;

}



//NB!!!!!!!!!
// ставит две точки по вертикали
void _GrBitmap::SetPixels(int nX, int nY, COLORREF color)
{
	m_hDC.SetPixel(nX,nY,color);
	m_hDC.SetPixel(nX,nY+1,color);
}



_GrBitmap* _GrBitmap::Copy(HDC hDC, CRect rcDst, CRect rcSrc) 
{
//TODO: Bitmap copy
  //ATLTRACE("Bmp%08X rc.left=%d rc.right=%d\n",this, rc.left, rc.right);
  //m_nBegPix = rc.left - m_nBegPx;// + m_nBegPix;

  if (rcSrc.Width() < 0)
    rcSrc = rcDst;

  int nRest = m_nEndPix - m_nBegPix;
  //rc.left = m_nBegPx+m_nBegPix;

  if (rcSrc.left < 0)
  {
    nRest = nRest;
  }

  //::SetBkColor(hDC, RGB(250,0,0));
  //::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);

  //050414 - проверка на рисование за пределами заполненного участка
  if (rcSrc.right > m_nEndPix+1) 
  {
    ATLTRACE("%d > %d\n", rcSrc.right, m_nEndPix);
    return 0;
  }

  if (rcSrc.Width() <= nRest)
  {

    if (BitBlt(hDC, rcDst.left,rcDst.top, rcDst.Width(), rcDst.Height(), m_hDC,rcSrc.left,rcSrc.top,SRCCOPY) == 0)
    {
      DWORD dw = GetLastError();
      ATLTRACE("_GrBitmap::Copy - BitBlt error %d\n",dw);
    }
    //ATLTRACE("Bmp copy %08X - b%d (%d %d) r%d w%d e%d\n", this,m_nBegPix, rc.left, rc.right, nRest, m_nWidth, m_nEndPix);
    m_nBegPix += rcSrc.Width() - 1;

    //вывод линии конца
    rcDst.left = rcDst.right;
    rcDst.right = rcDst.left+1;
    ::SetBkColor(hDC, RGB(0,250,250));
    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rcDst, NULL, 0, NULL);
    return this;
  }
  else
  {
/*
    if (BitBlt(hDC, rc.left, rc.top, nRest, rc.Height(), m_hDC,m_nBegPix,0,SRCCOPY) == 0)
    {
      DWORD dw = GetLastError();
      ATLTRACE("_GrBitmap::Copy - BitBlt error %d\n",dw);
    }
    ATLTRACE("Bmp copy %08X - b%d (%d %d) r%d ---\n", this,m_nBegPix, rc.left, rc.left + nRest, nRest);
    rc.left = rc.left + nRest;

    CRect rc1(rc);
    rc1.right = rc1.left;
    rc1.left--;
    ::SetBkColor(hDC, RGB(250,0,0));
    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc1, NULL, 0, NULL);
*/
    SetFree();

    return m_pNext;
  
  }

}


