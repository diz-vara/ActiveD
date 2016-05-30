// ActDView.cpp : implementation of the CActDView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"



#include <commctrl.h>
#include "ActDView.h"
#include ".\actdview.h"
#include <AtlMisc.h>
#include "MainFrm.h"
#include "DlgView.h"
#include "_GrBitmap.h"

static bool m_bMessageLog(0);

CActDView::~CActDView() 
{
  ATLTRACE("~ActDView\n");

  m_pLoop->RemoveMessageFilter(this);
	//if (!m_bChild) SetPar(m_hParent);

	m_hWnd = 0;
  m_saChNames.RemoveAll();
	SAFE_DELETE_HANDLE(m_hIcon);
  SAFE_DELETE_ARRAY(m_Pnt);
	ATLTRACE("View-Delete Thread %x %X\n", GetCurrentThreadId(), m_uThread);
	if (::IsWindow((m_hWnd))) 
    DestroyWindow();	
  else
  {
  	ATLTRACE("NO WINDOW TO DESTROY!!! %8x %8X\n", GetCurrentThreadId(), m_uThread);
    //DestroyWindow();
  }

}


BOOL CActDView::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->hwnd != m_hWnd)
    return false;
  
  if (pMsg->message == WM_DIZ_SHOWMESS)
  {
    //m_bMessageLog = true;
    //ShowWindow(SW_HIDE);
    //SetParent(m_hParent);
		//ModifyStyle(0,WS_CHILD);
    Kill_Timer();

    ATLTRACE("ShowMessages %x\n", GetCurrentThreadId());
    return true;
  }
  else if (pMsg->message == WM_TIMER) 
    pMsg->message = WM_DIZ_VTIMER;
	else if (pMsg->message == WM_KEYDOWN)
		return m_pDialog->PreTranslateMessage(pMsg);
	return FALSE;
}

LRESULT CActDView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	ATLTRACE("View::OnPaint %8x", m_hWnd);

	CPaintDC dc(m_hWnd);
	if (m_hMemBmp)
  {
		dc.FillSolidRect(0,0,30, m_nWinH,_Settings.GetBgColor());
		BitBlt(dc, 30,0, m_nWinW, m_nWinH, m_hMemDC, 0,0, SRCCOPY);
    if (m_hTextBmp)
    {
		  BitBlt(dc, 0,0, 30, m_nWinH, m_hTextDC, 0,0, SRCINVERT);
		  BitBlt(dc,30,0, 30, m_nWinH, m_hTextDC,30,0, SRCINVERT);
    }
  }
  else
  {
    dc.FillSolidRect(0,0,m_nWinW, m_nWinH,m_colBG);
  }
	return 0;

}

LRESULT CActDView::OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//m_bChild = !m_bChild;
	SetPar( (m_bChild) ? 0:m_hParent);
	
	return 0;
}

LRESULT COscView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  //050406
  if (!InitializeCriticalSectionAndSpinCount(&m_csBitMaps, 0x80000400) ) 
      return 0;
  return CActDView::OnCreate(uMsg, wParam, lParam, bHandled);
}

LRESULT CActDView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  //050318 - инициализация критической секции               Vстарший бит разрешает исключения
  //if (!InitializeCriticalSectionAndSpinCount(&m_csCounts, 0x80000400) ) 
  //    return 0;


  CREATESTRUCT *cs = (CREATESTRUCT*)lParam;
	m_hParent = cs->hwndParent;
	CRect rc(0,0,100,200);
	//SetSplitterRect(0);
	//m_cxySplitBar = 4;
	//m_cxyMin = 35;
	//m_bFullDrag = true;
	if (m_pMainFrame) m_hFrame = m_pMainFrame->m_hWnd;
	// create command bar window
	// load command bar images
	//m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	//SetMenu(NULL);

	//HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_TB1, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	//CreateSimpleReBar(ATL_SIMPLE_REBAR_STYLE|CCS_VERT);
	//	AddSimpleReBarBand(hWndCmdBar);
	//AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	//CreateSimpleStatusBar();


  //050301 - пытаюсь зарегистрировать в лупе граф. потока.
	ATLASSERT(m_pLoop != NULL);
	m_pLoop->AddMessageFilter(this);
	//m_pLoop->AddIdleHandler(this);


	m_nDur = 8;
	m_nFrq = 2048;

	m_hDC = GetDC();
  m_hMemDC.CreateCompatibleDC(m_hDC);

  //050716 - здесь тоже может быть текст (метки времени)
  m_hMemDC.SetTextColor(RGB(220,220,220));
  m_hMemDC.SetBkColor(0);
  m_hMemDC.SetBkMode(TRANSPARENT);
  
  //050507 - text out
  m_hTextDC.CreateCompatibleDC(m_hDC);
  m_hTextDC.SetTextColor(RGB(220,220,220));
  m_hTextDC.SetBkColor(0);
  m_hTextDC.SetBkMode(TRANSPARENT);

  m_hTempDC.CreateCompatibleDC(m_hDC);
	//m_pTrigBuf = new WORD[37];
  pnGrey = CreatePen(PS_SOLID,1,RGB(127,127,127));
	pnGreen = CreatePen(PS_SOLID,1,_Settings.GetColor(0));
  pnBorder = CreatePen(PS_SOLID,1,RGB(255,0,0));
  pnWarning = CreatePen(PS_SOLID,1,RGB(255,255,0));


	//040707 
	//m_pMainBuf = m_pMainFrame->GetMainBuf();

	m_bmpArrowOut.LoadBitmap(MAKEINTRESOURCE(IDB_ARROWOUT));
	m_bmpArrowIn.LoadBitmap(MAKEINTRESOURCE(IDB_ARROWIN));
  m_hIcon = AtlLoadIcon((m_nType) ? IDI_SNAP:IDI_OSC);
	SetIcon(m_hIcon,false);

  m_colBG = GetSysColor(COLOR_APPWORKSPACE);


	return S_OK;
}

LRESULT CActDView::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

LRESULT COscView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//SetSplitterRect();
CRect rc;

	//ATLTRACE("View::OnSize %8x\n", m_hWnd);

  EnterCriticalSection(&m_csBitMaps);
  Kill_Timer();
  m_nWinBegPix = 0;
  
  if (m_pWriteBmp)
  {
    delete m_pWriteBmp;
    m_pWriteBmp = 0;
    m_pReadBmp = 0;
    m_dwTime = 0;
  }


  if (m_hOldBmp)
    m_hMemDC.SelectBitmap(m_hOldBmp);

	SAFE_DELETE_HANDLE(m_hMemBmp);
	SAFE_DELETE_HANDLE(m_hTextBmp);
	GetClientRect(rc);
	m_hMemBmp = CreateCompatibleBitmap(m_hDC,rc.Width(),rc.Height());
	m_hTextBmp = CreateCompatibleBitmap(m_hDC,60,rc.Height());
  m_hOldBmp = m_hMemDC.SelectBitmap(m_hMemBmp);
  m_hOldTBmp = m_hTextDC.SelectBitmap(m_hTextBmp);
  m_nWinH = rc.Height();
  m_nWinW = rc.Width()-30;
  m_nWinFrameCnt = 0;

	
  
  m_hMemDC.FillSolidRect(rc,GetSysColor(COLOR_BTNFACE));
  m_hTextDC.FillSolidRect(0,0,60,rc.Height(),0);
	//BitBlt(dc,0,0,rc.right,rc.bottom,m_hMemDC,0,0,SRCCOPY);
	//m_nOldCnt = 0;
  LeaveCriticalSection(&m_csBitMaps);


	bHandled = FALSE;
	return E_FAIL;
}

void CActDView::SetPar(HWND hPar) 
{
		CRect rc(rcDefault), rcW(rcDefault);
		CPoint pnTL, pnBR;

		if (hPar) 
		{
			m_hParent = hPar; 
			::GetClientRect(hPar,&rc);
		}
		SetParent(hPar); 
		GetWindowRect(&rcW);

		if (m_bChild) {
			m_rcIn = rcW;
			if (m_rcOut != rcDefault) {
				rc = m_rcOut;

			} else {
				rc.OffsetRect(40,-40);
			}
		} else {
			if (m_rcIn != rcDefault) {
				rc = m_rcIn;
			}
			m_rcOut = rcW;
			pnTL = rcW.TopLeft();
			pnBR = rcW.BottomRight();
			::ScreenToClient(m_hParent,&pnTL);
			::ScreenToClient(m_hParent,&pnBR);
			m_rcOut = CRect(pnTL,pnBR);
		}

		m_bChild = (hPar != 0);
		if (m_bChild)
			ModifyStyle(0,WS_CHILD);
		else
			ModifyStyle(WS_CHILD,0);


		SendMessage(WM_NCPAINT,1);
		MoveWindow(&rc);

}

LRESULT CActDView::OnOne(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString str;
	str.Format("Command ID == %d",wID);
	MessageBox(str);

	return 0;
}

void CActDView::OnFinalMessage(HWND hWnd)
{
  ATLTRACE("View: OnFinalMessage\n");
	__super::OnFinalMessage(hWnd);
}

//LRESULT CActDView::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
//{
	//SAFE_DELETE_ARRAY(m_pTrigBuf);
	//SAFE_DELETE_ARRAY(m_pRefBuf);
	//SAFE_DELETE_ARRAY(m_pChanBuf);

	//bHandled = true;
	//ATLTRACE("View::OnClose %8x %8X %8X\n", m_hWnd, m_uThread, GetCurrentThreadId());

	//::PostMessage(m_hFrame,WM_DIZ_VIEW_CLOSE,0,(DWORD)this);
  //m_bMessageLog = true;
	//return S_OK;
//}

LRESULT CActDView::OnActivate(UINT uMsg, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	static bool bHere(true); 
  if (m_bHere)
    return S_OK;

  ATLTRACE("ActdView::OnActivate %x %d %x\n", GetCurrentThreadId(), bHere, wParam);

	if (!bHere ) {
		WORD wActive = LOWORD(wParam);
		BOOL bActive(uMsg == WM_CHILDACTIVATE);
		if (!bActive)
			bActive = (wActive == WA_ACTIVE || wActive == WA_CLICKACTIVE);

		bHere = true;

		if (bActive)	::PostMessage(m_hFrame,WM_DIZ_VIEW_ACTIVATE,0,(DWORD)this);
	}

	//bHandled = false;
	
	bHere = false;
	::ShowWindow(m_hParent,SW_SHOW);
	::SetActiveWindow(m_hParent);
  ATLTRACE("ActDView::OnActivate ---- end\n");
	return S_OK;
}

LRESULT CActDView::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HBITMAP *pArrow(&m_bmpArrowOut.m_hBitmap);
	if (!m_bChild)		pArrow = &m_bmpArrowIn.m_hBitmap;

	

	ATLTRACE("View-NcPaint Thread %x\n", GetCurrentThreadId());

	if (0 && !m_bChild) {
		bHandled = 0;
		return E_FAIL;
	} else {
		CRect rcC;
		CWindowDC dc(m_hWnd);
		DefWindowProc();
		//if (!m_bActive) return true;


		//if (hdc == 0) return 0;


     DWORD dwStyle = GetStyle();
     CSize szFrame = (dwStyle & WS_THICKFRAME) ?
          CSize(GetSystemMetrics(SM_CXSIZEFRAME),
                  GetSystemMetrics(SM_CYSIZEFRAME)) :
          CSize(GetSystemMetrics(SM_CXFIXEDFRAME),
                    GetSystemMetrics(SM_CYFIXEDFRAME));
		 
		 CSize szButton = CSize(GetSystemMetrics(SM_CXMENUSIZE), GetSystemMetrics(SM_CYMENUSIZE));


     int cxIcon = GetSystemMetrics(SM_CXSIZE); // width of caption icon/button

     // Compute rectangle
     GetWindowRect(&rcC);      // window rect in screen coords

     rcC -= CPoint(rcC.left, rcC.top);   // shift origin to (0,0)
		 rcC.left  /*= rcC.CenterPoint().x - 40;*/ += szFrame.cx;                    // frame
		 rcC.right -= szFrame.cx+(szButton.cx * 2);                    // frame
     rcC.top   += szFrame.cy;                    // top = end of frame
     rcC.bottom = rcC.top + GetSystemMetrics(SM_CYCAPTION)  // height of caption
          - GetSystemMetrics(SM_CYBORDER)-1;                    // minus gray shadow border

		 //dc.FillSolidRect(&rcC,RGB(50,0,200));
		 dwStyle = DC_BUTTONS|DC_TEXT|DC_GRADIENT|DC_ICON;
		 if (m_bActive)
			 dwStyle |= DC_ACTIVE;

		 ::DrawCaption(m_hWnd,dc.m_hDC,&rcC,dwStyle   );
		 rcC.left = rcC.right - szButton.cx;
		 rcC.top = rcC.bottom - szButton.cy;
		 ::DrawFrameControl(dc.m_hDC,&rcC,DFC_BUTTON,DFCS_BUTTONPUSH);
	
		HBITMAP bmpOld = m_hTempDC.SelectBitmap(*pArrow);
		dc.BitBlt(rcC.left+1, rcC.top+1 ,14,14,m_hTempDC,0,0,SRCINVERT);
		m_hTempDC.SelectBitmap(bmpOld);
		ATLTRACE("View-NcPaint Thread --- end\n");
		return S_OK;
	}

}

LRESULT CActDView::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT COscView::SetFrq(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	//m_nDur = 4;

  EnterCriticalSection(&m_csBitMaps);
	int Frq(wParam);
	if (lParam > 0) m_nDur = lParam;
  if (Frq > 0) m_nFrq = Frq;
	SetBuffers();

  //050415 - введено для правильного выбора фильтровых коэффициентов при изменении частоты.
	m_cfLP = m_pDialog->GetCfLP();
	m_cfHP = m_pDialog->GetCfHP();

	m_bChange = true;
  SendMessage(WM_SIZE);

  LeaveCriticalSection(&m_csBitMaps);


	return 0;
}


void CActDView::Test(void)
{
		ATLTRACE("TEST %x\n",m_hWnd);
}

LRESULT CActDView::OnTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
		ATLTRACE("TEST %x\n",m_hWnd);
		return 0;
}



LRESULT CActDView::OnSetActive(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  ATLTRACE("OnSetActive %x\n", GetCurrentThreadId());
	m_bActive = (wParam != 0);

  m_bHere = true;
  if (m_bActive)
	{
		SetActiveWindow();
		BringWindowToTop();
    if (lParam == 0)
      ::PostMessage(m_hFrame,WM_DIZ_VIEW_ACTIVATE,0,(DWORD)this);
	}
	RedrawWindow(0,0,RDW_INVALIDATE|RDW_FRAME);
  m_bHere = false;
	return 0;
}

//041001
LRESULT COscView::OnCfChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == 0)
		m_cfLP = m_pDialog->GetCfLP();
	else if (wParam == 1)
		m_cfHP = m_pDialog->GetCfHP();
	
	m_bChange = 1;
	return 0;
}

void COscView::SetBuffers(void)
{

	//ATLTRACE("View::SetBuffers ENTER\n", m_hWnd);

	m_nBufChans = m_pMainFrame->GetActChans();


	m_nRefNum = m_pDialog->GetRefNum();
	m_nDispChans = m_pDialog->GetDispNum();
	m_nBpChans = m_pDialog->GetBiPolNum();
  m_nTotChans = m_nDispChans + m_nBpChans;

	//ATLTRACE("View::SetBuffers %d\n", m_nTotChans);

	SAFE_DELETE_ARRAY(m_pTrigBuf);
	SAFE_DELETE_ARRAY(m_pOldHPx);
	SAFE_DELETE_ARRAY(m_pOldLPx);
	SAFE_DELETE_ARRAY(m_pOldHPy);
	SAFE_DELETE_ARRAY(m_pOldLPy);

	SAFE_DELETE_ARRAY(m_pChanBuf);

  m_nMaxCnt = 0;
  m_nMaxTrigCnt = 0;
  SAFE_DELETE_ARRAY(m_pRefBuf);
  


  
	m_nLength = m_nFrq * m_nDur;


  if (m_nTotChans > 0)
  {
	  m_pOldHPx = new double[2*m_nTotChans];
	  ZeroMemory(m_pOldHPx,2*m_nTotChans*sizeof(double));
	  m_pOldLPx = new double[3*m_nTotChans];
	  ZeroMemory(m_pOldLPx,3*m_nTotChans*sizeof(double));

	  m_pOldHPy = new double[2*m_nTotChans];
	  ZeroMemory(m_pOldHPy,2*m_nTotChans*sizeof(double));
	  m_pOldLPy = new double[3*m_nTotChans];
	  ZeroMemory(m_pOldLPy,3*m_nTotChans*sizeof(double));
  }

  return;

}

LRESULT CActDView::OnNcLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  ATLTRACE("NcLButtonDown enter\n");
	if (wParam == HTMINBUTTON)
  {
		SetPar( (m_bChild) ? 0:m_hParent);
  }
	else if (wParam == HTCLOSE)
  {
		//SetActiveWindow();
		//BringWindowToTop();
    //m_bActive = true;

    //::PostMessage(m_hFrame,WM_DIZ_VIEW_ACTIVATE,1,(DWORD)this);
    //Sleep(200);
    ::PostMessage(m_hFrame,WM_DIZ_VIEW_CLOSE,0,(DWORD)this);
    bHandled = true;
  }
  else
  {
		DefWindowProc();
    //Sleep(200);
  }
  ATLTRACE("NcLButtonDown exit\n");
	return S_OK;
}


void static CALLBACK ViewTimeProc(
  UINT uID,      
  UINT uMsg,     
  DWORD dwUser,  
  DWORD dw1,     
  DWORD dw2      
)
{
int nTest(123);
  COscView* pView = (COscView*)dwUser;
  nTest = 123;
  pView->SendMessage(WM_DIZ_VTIMER,dwUser,(LPARAM)(&nTest));
  //pView->OnDizVTimer(0,0,0,bH);
	ATLTRACE("ViewTimer Thread %x %d\n", GetCurrentThreadId(),nTest);
  return;
}

#pragma optimize("at", on)
#pragma runtime_checks( "", off )

void COscView::Draw(int nBase, int nCnt, int nTCnt )
{
  CPen pnOld;
  int nZero;
  COLORREF Blue(RGB(0,0,200));
  COLORREF Red(RGB(255,0,0));
  COLORREF Bg(RGB(50,50,50));

  const COLORREF *pTrCol = _Settings.GetTrigColors();
  WORD wTrigMask = _Settings.GetTrigWord();


  COLORREF color;
  int h(0), y0(0);
  int chan;
  long lTmp;
  

  if (nCnt <= 0)
    return;

  //ATLTRACE("DRAW: entering bitmaps\n");
  EnterCriticalSection(&m_csBitMaps);
  //ATLTRACE("         DRAW: bitmaps ENTERED\n");

  
  CRect rc;
	GetClientRect(&rc);
  m_nWinH = rc.Height();
  m_nWinW = rc.Width()-30;

  //2. - определяю длину нужного участка
  //2.1 - уже здесь вопрос: что заливать? Ведь для всех разов, кроме первого, нужно не трогать первую колонку...




  //отдельная зона для триг-канала
  int nTrigZone = (m_pDialog->IsTrig()) ? 55:0;


  //запоминаемый битмэп еще не создан
  if (m_hMemBmp == 0)
  {
	  m_hMemBmp = CreateCompatibleBitmap(m_hDC,rc.Width(),rc.Height());
	  m_hTextBmp = CreateCompatibleBitmap(m_hDC,60,rc.Height());
    m_hOldBmp = m_hMemDC.SelectBitmap(m_hMemBmp);
    m_hOldTBmp = m_hTextDC.SelectBitmap(m_hTextBmp);
  }

  //ATLTRACE("m_pWriteBmp = %8x\n", m_pWriteBmp);


  //все, битмэп получен
  m_pWriteBmp->SetUsed();
  m_pWriteBmp->m_nBegFrame = m_nWinFrameCnt;


	int nP(0);
	int nBufPos;
  register short min, max, d;
	register short  a,b,c(0);


  int x0 = rc.left = (m_nWinFrameCnt)   * (m_nWinW) /m_nLength; 
	int x1 = (m_nWinFrameCnt + nCnt)   * (m_nWinW) /m_nLength;
  rc.right = x1;

	//заливка прямоугольника
  CRect rc1(rc);
  COLORREF col(_Settings.GetBgColor());

	//050417 не стираю первую колонку нового битмэпа - в ней могут быть старые точки
  if (m_nWinFrameCnt > 0)    
    rc1.left++;
  // else  col = RGB(50,50,0);
 
  rc1.right++;
  m_pWriteBmp->FillSolidRect(&rc1,col); 




  //050407 
  // рисование секундных тиков
  int nOneSec = m_nFrq - m_nOneSecCnt;
  while (nOneSec >= 0 && nOneSec <= nCnt)
  {
    int x = nOneSec * (m_nWinW)/m_nLength + x0;
    m_pWriteBmp->Line(pnGrey,x,rc.top,x,rc.bottom/2);
    nOneSec   += m_nFrq;
    m_nOneSecCnt -= m_nFrq;
  } 
  m_nOneSecCnt += nCnt-1;



	if (m_nTotChans)
	{
		h = (m_nWinH-nTrigZone)/m_nTotChans;
    //050429 - отдельная заливка для биполяров
	  if (m_nBpChans)
	  {
      y0 = m_nDispChans*h;
      rc1.top = y0;
      rc1.bottom -= nTrigZone;
      m_pWriteBmp->FillSolidRect(&rc1,_Settings.GetBPColor()); 
		  m_pWriteBmp->Line(pnGrey,x0, y0, x1, y0);
	  }
		y0 = -h/2 + 10;
		long *pBuf;
		int nResol = m_pDialog->m_nScalePnt;

    x0 = ( m_nWinFrameCnt  * (m_nWinW)/m_nLength);


		for (chan = 0; chan < m_nTotChans; chan++)
		{
      //ATLTRACE("Plot: chan %d\n",chan);

			y0 += h;
      m_pWriteBmp->Line(pnGrey,x0,y0,x1,y0);
      if (chan >= m_nDispChans)
        nResol = m_pDialog->m_nBPScalePnt;


      //050507 - channel names
      if (chan < m_saChNames.GetSize())
			{
				m_hTextDC.SetTextColor(_Settings.GetBgColor() ^ _Settings.GetColor(chan));
        m_hTextDC.TextOut(1,y0-8, m_saChNames[chan]);
			}

      nP = 0;
			pBuf = m_pChanBuf+m_nMaxCnt*chan + nBase;

      //ATLTRACE("Plot: chan %d\n",chan);
			if ((m_nWinFrameCnt <= 0 && !m_bScroll) || m_bZero || m_bChange) 
        m_Zero[chan] = pBuf[0];

			nZero = m_Zero[chan];
      //не вычитаем ноль при хайпассе
      if (m_cfHP.Is()) nZero = 0;

			a = x0; b = a;
      c =(short)((nZero-pBuf[0])*h/nResol);
      min = max = d = c;
			m_Pnt[nP++] = CPoint(x0,d+y0);
			//min = max = -1;
			// безреферентное отведения - просто вычитаем первую точку буфера

      //if (chan == 0)  ATLTRACE(" Pl: (j + %d) * %d / %d = %d\n",m_nWinFrameCnt, m_nWinW, m_nLength, x0);

			for (int j = 1; j < nCnt ; j++ ) 
			{
				nBufPos = j;
				b = (short)((j + m_nWinFrameCnt)  * (m_nWinW)/m_nLength);

        //if (chan == 0 && j < 3) ATLTRACE(" Pl: (%d + %d) * %d / %d = %d\n",j,m_nWinFrameCnt, m_nWinW, m_nLength, b);

        //проверка на перехлест здесь не нужна
				//while (nBufPos >= m_nLengPlus) nBufPos -= m_nLengPlus;

				lTmp = pBuf[nBufPos];
        lTmp = nZero - lTmp;
        lTmp = lTmp*h/nResol;
				if (lTmp + y0 > rc.bottom) 
					lTmp = rc.bottom - y0-10;
				if (lTmp + y0 < 0) 
					lTmp =  10-y0;

				c = (short)lTmp;

				if ( a == b ) {
					if ( min > c ) min = c;
					else if ( max < c ) max = c;
				} else {
					if ( d < c ) {
						m_Pnt[nP].x = a; m_Pnt[nP++].y = min+y0; //CPoint(a,min);
						if ( min != max  )  {
							m_Pnt[nP].x = a; m_Pnt[nP++].y = max+y0; //CPoint(a,max);
						}
					} else {
						m_Pnt[nP].x = a; m_Pnt[nP++].y = max+y0; //CPoint(a,max);
						if ( min != max  )  {
							m_Pnt[nP].x = a; m_Pnt[nP++].y = min+y0; //CPoint(a,min);
						}
					}
				a = b; d = c;
				min = max = c;
				}
			} //конец цикла по j

			m_Pnt[nP++] = CPoint(b,c+y0);
      //ATLTRACE("Plot: nP = %d\n", nP);
			if ( nP < 2 ) 
				a = 2;

      //попробуем обозначить границу

			short s = 2047*h/5000;

			pnGreen.DeleteObject();
			pnGreen = CreatePen(PS_SOLID,1,_Settings.GetColor(chan));

      m_pWriteBmp->PolyLine(pnGreen, m_Pnt, nP);
      //m_pWriteBmp->Line(pnBorder, m_Pnt[nP-2].x, m_Pnt[nP-2].y, m_Pnt[nP-1].x, m_Pnt[nP-1].y);
      //m_pWriteBmp->Line(pnWarning, m_Pnt[0].x, m_Pnt[0].y, m_Pnt[2].x, m_Pnt[2].y);
      //m_pWriteBmp->SetPixels(m_Pnt[0].x, m_Pnt[0].y, RGB(0,255,255));
      //m_pWriteBmp->SetPixels(m_Pnt[nP-1].x, m_Pnt[nP-1].y, RGB(255,0,255));
      //if (chan == 0) ATLTRACE("PolyLIne %d %d - %d %d   - %d %d\n",m_Pnt[0].x, m_Pnt[0].y,m_Pnt[1].x, m_Pnt[1].y,m_Pnt[nP-1].x, m_Pnt[nP-1].y);
      
		} //конец цикла по "нормальным" каналам

	}



	if (nTrigZone)
	{
    DWORD dwTr;
    //ATLTRACE("    %d\n", nTCnt);
		for (int bit = 0; bit < 16; bit++) {
			int x(0),X0(-1),Y0(-100);
			int y = rc.bottom - 52 + bit * 3 + bit/8*2;
			DWORD dwMask = 1 << bit;
			for (nBufPos = 0; nBufPos < nTCnt; nBufPos++) {
				int yy(y);
				x = (m_nTrigFrameCnt + nBufPos)*(m_nWinW)/m_nTLength; 
				if (x == X0 && y == Y0) continue;
				if (X0 < 0) X0 = x;
        dwTr = m_pTrigBuf[nBufPos] ^ wTrigMask;

        if ((dwTr & dwMask)) {
					color = Bg;
				} else {
					color = pTrCol[bit];
					//yy--;
					//y = y+1;
				}
				for (int xx = X0; xx <= x; xx++) {
					m_pWriteBmp->SetPixels(xx,yy,color);
					//m_hMemDC.SetPixel(xx,yy+2,color);
				}
				X0 = xx; Y0 = yy;
			} //конец цикла по точкам
		} //конец цикла по битам
	} // if (nTrigZone)
  m_nTrigFrameCnt += (nTCnt-1);

  //ATLTRACE("Plot: FrC=%d, PixCnt=%d",)

  m_nWinFrameCnt += (nCnt-1);

  //if ( m_nTrigFrameCnt >= m_nTLength) m_nTrigFrameCnt -= m_nTLength;
  //if (m_nWinFrameCnt >= m_nLength) m_nWinFrameCnt -= m_nLength;



  LeaveCriticalSection(&m_csBitMaps);
  //ATLTRACE("DRAW: bitmaps leaved\n");

}
#pragma runtime_checks( "", restore )


LRESULT COscView::OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  //Карта отображаемых каналов
	CStatMap* pDispMap(0);
	//карта отображаемых каналов
	pDispMap = m_pDialog->GetDispMap();
   //Карта референтных каналов
	CStatMap* pRefMap(0);
	int nRefNum = m_pDialog->GetRefNum();
  m_nPinChans = m_pMainFrame->GetPinChans();


  if (nRefNum)
	{
		//карта референсных каналов
		pRefMap = m_pDialog->GetRefMap();
  }

  _BiPol *pBP = m_pDialog->GetBiPolArr()->GetData();

  
  SetBuffers();
  m_saChNames.RemoveAll();
  //здесь я предполагаю, что монополяры у меня в строгой последовательности 
  //  благо другого варианта сейчас все равно нет
  int nChanIdx(0);
  CSimpleArray<CString> saBpNames;
  for (int i = 0; i < TOTCHANS; i++) 
  {
    m_dcMapDisp[i].Clear();
    if (pRefMap)
    {
      m_dcMapDisp[i].m_bRef = (pRefMap->GetCell(i) != 0);
    }

    if (pDispMap && pDispMap->GetCell(i))
    {
      m_dcMapDisp[i].m_nMono = nChanIdx++ + 1;
      m_saChNames.Add(CString(pDispMap->GetName(i)));
    }
    for (int bp = 0; bp < m_nBpChans; bp++)
    {
				int chan = pBP[bp].GetSubtr();
				//if (chan >= 256) chan = chan-256+m_nPinChans; //for mode 4
				int chan2 = pBP[bp].GetMinue();
				//if (chan2 >= 256) chan2 = chan2-256+m_nPinChans; //for mode 4
       
				//040717 - проверка присутствия обоих каналов
				if ( m_pMainFrame->m_HardDlg.GetCell(chan) && m_pMainFrame->m_HardDlg.GetCell(chan2))
        {
          if (i == chan)
          {
            m_dcMapDisp[i].m_arBpNum.Add(bp+1);
          }
          if (i == chan2)
          {
            m_dcMapDisp[i].m_arBpNum.Add(-(bp+1));
          }
          saBpNames.Add(CString(pBP[bp].GetName()));

        }
    }
  }

  for (int bp = 0; bp < saBpNames.GetSize(); bp++)
    m_saChNames.Add(saBpNames[bp]);
  
  if (m_hTextDC)
    m_hTextDC.FillSolidRect(0,0,60,m_nWinH,0);
	m_bChange = true;
	m_bZero = true;
  return S_OK;
}

//050429 - восстановление положения окна из запомненного
BOOL CActDView::SetPosition(RECT *prcIn, RECT *prcOut, bool bChild)
{
  m_rcIn = *prcIn;
  m_rcOut = *prcOut;
  m_bChild = bChild;
  RECT *pRC(0);
  RECT rc;

  if (m_bChild)
  {
    pRC = prcIn;
  }
  else
  {
    pRC = prcOut;
  }

		if (m_bChild) 
		{
  		SetParent(m_hParent); 
			::GetClientRect(m_hParent,&rc);
			if (pRC->right > rc.right)
				pRC->right = rc.right;
			if (pRC->left < rc.left)
				pRC->left = rc.left;
			if (pRC->top < 0)
				pRC->top = 0;
			ModifyStyle(0,WS_CHILD);
		}
    else
    {
      SetParent(0);
			ModifyStyle(WS_CHILD,0);
    }


		SendMessage(WM_NCPAINT,1);
		MoveWindow(pRC);
    return TRUE;
}

//050429 - сохранение и восстановление положения окна
BOOL CActDView::GetPosition(RECT *prcIn, RECT *prcOut)
{
  CRect rcW, rcC;
  //SetParent(0);

  if (m_hParent)
    ::GetWindowRect(m_hParent, &rcC);

  GetWindowRect(&rcW);
  if (m_bChild)
  {
    rcW.OffsetRect(-(rcC.left+GetSystemMetrics(SM_CXEDGE)), -(rcC.top+GetSystemMetrics(SM_CYEDGE)));
    m_rcIn = rcW;
  }
  else
    m_rcOut = rcW;

  *prcIn = m_rcIn;
  *prcOut = m_rcOut;
  return m_bChild;
}

int CActDView::GetNum() const
{
  return m_pDialog->GetNumber();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// COscView
void COscView::Kill_Timer()
{
  if (m_uTimerID)
  {
    KillTimer(m_uTimerID);
    //timeKillEvent(m_uTimerID);
    m_uTimerID = 0;
  }

}

void COscView::Set_Timer(UINT uTime)
{

  m_dwTime = 0;
  Kill_Timer();
  m_uTimerID = SetTimer(1,uTime);
  //m_uTimerID = timeSetEvent(uTime,2,&ViewTimeProc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
}


LRESULT COscView::OnDizVTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
DWORD dwTime, dwDiff(0);
int nWinDur(m_nDur*1000);
CRect rcDst;
CRect rcSrc;
CString str;

//VTimer - копирование битмэпов


  if (!TryEnterCriticalSection(&m_csBitMaps))
  {
    ATLTRACE("VTimer: Can't Enter BitMaps..\n!!!!!");
    EnterCriticalSection(&m_csBitMaps);
    //return 0;
  }

  //ATLTRACE("Bitmaps ENTERED\n");
  try
  {
    if (m_pReadBmp == 0)
      throw("m_pReadBmp == 0\n");

    //050406 - признак конца
    if (m_pReadBmp->GetH() == 0)
    {
      Kill_Timer();
      m_dwTime = 0;
      delete m_pWriteBmp;
      m_pWriteBmp = 0;
      m_pReadBmp = 0;
      SAFE_DELETE_HANDLE(m_hMemBmp);
      SAFE_DELETE_HANDLE(m_hTextBmp);
    }

    dwTime = timeGetTime();

    //при первом входе только время устанавливаем - рисуем только дальше
    if (m_dwTime != 0)
    {
      //определяем, сколько прошло времени
		  dwDiff = dwTime	-	m_dwTime;

	    //ATLTRACE("View - OnDizVTimer Thread %x\n", GetCurrentThreadId());
      //050402 - теперь нужно определить, в каком месте отображаемого окна я буду находиться
      //  но для этого мне нужно два числа: начало (где закончили в прошлый раз) и полученное
      //  dwDiff...
      // имеем:
      //  m_nDur - длину в четвертьсекундах (250 мс)
      //  m_nLength - длину в отсчетах

      // получили длину отобр. участка в отсчетах
      int nWinEnd = dwDiff * (m_nWinW) / nWinDur;
      int nWinLen = nWinEnd - m_nWinBegPix;

      //ATLTRACE("nWinLen %d / %d %d %d\n", nWinLen, dwDiff, m_nWinW, nWinDur);
      //нечего рисовать
      if (nWinLen < 1){
        //LeaveCriticalSection(&m_csBitMaps);
        //throw("nWinLen < 1\n");
        str.Format("nWinLen < 1 %ld %ld %d %d\n", dwTime, dwDiff, m_nWinW, nWinDur);
        throw((const char*)str);
      }

      _GrBitmap *pBmp;

      CRect rcSrc(m_nWinBegPix,0,nWinEnd,m_nWinH);

      if (!m_bScroll)
      {
        //проверка на "перехлест"
        if (nWinEnd >= m_nWinW)
        {
          //текущий вывод "перехлестывает" за ширину окна - нужно два раза
          rcSrc.right = m_nWinW;
          //ATLTRACE(" ууCopy %d points to %d..%d from %d --", rc.Width(), rc.left, rc.right, m_nWinW);

          pBmp = m_pReadBmp->Copy(m_hMemDC, rcSrc);

          //Если копирование не удалось, возвращается 0 и время не меняется
          if (pBmp == 0)
          {
            //LeaveCriticalSection(&m_csBitMaps);
            throw("pBmp == 0\n");
          }
          m_pReadBmp = pBmp;

          while (nWinEnd >= m_nWinW) 
          {
            //ATLTRACE(": ");
            nWinEnd -= m_nWinW;
          }
          rcSrc.right = nWinEnd;
          rcSrc.left = 0;
          m_pReadBmp->SetFree();
          m_pReadBmp = m_pReadBmp->GetNext();
          //ATLTRACE(" rest %d (%d %d) %d\n", rc.Width(), rc.left, rc.right, dwTime-m_dwTime);
        }

        if (rcSrc.Width() > 0)
        {
          //ATLTRACE("%08x Copy %d points to %d..%d from %d\n", m_pReadBmp, rc.Width(), rc.left, rc.right, m_nWinW);
          //rc.left--;
          pBmp = m_pReadBmp->Copy(m_hMemDC, rcSrc);
          //Если копирование не удалось, возвращается 0 и время не меняется
          if (pBmp == 0)
          {
            //LeaveCriticalSection(&m_csBitMaps);
            throw("pBmp == 0 2\n");
          }
          m_pReadBmp = pBmp;
        }
      } 
      else
      {
        //а это - для скроллирования
        //скроллируем на всю ширину окна
        int nX = m_nWinW - nWinLen; 
        BitBlt(m_hMemDC, 0 ,0, nX, m_nWinH, m_hMemDC.m_hDC, nWinLen,0, SRCCOPY);

        rcDst = rcSrc;
        rcDst.left = nX;
        rcDst.right = m_nWinW;

        //а вот здесь, как и в осце, нужно проверять на конец...
        if (nWinEnd >= m_nWinW)
        {
          //ATLTRACE("ScrollCopy %d..%d to %d...%d (%d)\n", rcSrc.left, rcSrc.right, rcDst.left, rcDst.right, m_nWinW);
          //текущий вывод "перехлестывает" за ширину окна - нужно два раза
          rcSrc.right = m_nWinW;
          rcDst.right = rcDst.left + rcSrc.Width();

          //ATLTRACE(" ууCopy %d points to %d..%d from %d --", rcSrc.Width(), rcSrc.left, rc.right, m_nWinW);

      
          //ATLTRACE(" 1st Copy %d..%d to %d...%d (%d)\n", rcSrc.left, rcSrc.right, rcDst.left, rcDst.right, m_nWinW);
          pBmp = m_pReadBmp->Copy(m_hMemDC, rcDst, rcSrc);

          //Если копирование не удалось, возвращается 0 и время не меняется
          if (pBmp == 0)
          {
            //LeaveCriticalSection(&m_csBitMaps);
            throw("pBmp == 0 3\n");
          }
          m_pReadBmp = pBmp;

          while (nWinEnd >= m_nWinW) 
          {
            //ATLTRACE(": ");
            nWinEnd -= m_nWinW;
          }
          rcSrc.right = nWinEnd;
          rcSrc.left = 0;

        //rcSrc.right = 0;
        
          m_pReadBmp->SetFree();
          m_pReadBmp = m_pReadBmp->GetNext();
          rcDst.left = rcDst.right;
          rcDst.right = rcDst.left + rcSrc.Width();
          //ATLTRACE(" 2nd Copy %d..%d to %d...%d (%d)\n", rcSrc.left, rcSrc.right, rcDst.left, rcDst.right, m_nWinW);
          //ATLTRACE(" rest %d (%d %d) %d\n", rc.Width(), rc.left, rc.right, dwTime-m_dwTime);
        }

        if (rcSrc.Width() > 0)
        {
          //ATLTRACE("%08x Copy %d points to %d..%d from %d\n", m_pReadBmp, rc.Width(), rc.left, rc.right, m_nWinW);
          //rc.left--;
          pBmp = m_pReadBmp->Copy(m_hMemDC, rcDst, rcSrc);
          //Если копирование не удалось, возвращается 0 и время не меняется
          if (pBmp == 0)
          {
            //LeaveCriticalSection(&m_csBitMaps);
            throw("pBmp == 0 4\n ");
          }
          m_pReadBmp = pBmp;
        }

      }
      
      m_nWinBegPix = nWinEnd;
      if (m_nWinBegPix < 0) m_nWinBegPix = 0;
      while ((int)(dwTime - m_dwTime) >= nWinDur) 
      {
        //ATLTRACE(" .");
        m_dwTime += nWinDur;
      }
      //RedrawWindow();
		  BitBlt(m_hDC, 30,0, m_nWinW, m_nWinH, m_hMemDC, 0,0, SRCCOPY);
      if (m_hTextBmp)
      {
        CRect rc(0,0,30,m_nWinH);
        //::SetBkColor(m_hDC, _Settings.GetBgColor());
        //::ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
		    //BitBlt(m_hDC, 0,0, 30, m_nWinH, m_hTextDC, 0,0, BLACKNESS);
		    BitBlt(m_hDC, 30,0, 30, m_nWinH, m_hTextDC, 30,0, SRCINVERT);
      }

    } 
    else //if (m_dwTime != 0)
    {
      //это означает, что мы выводим окно заново
      m_nWinBegPix = 0;
      m_dwTime = dwTime;
    }
  }
  catch(const char *cp)
  {
    ATLTRACE(cp);
    cp;
  }


  //RedrawWindow();
  LeaveCriticalSection(&m_csBitMaps);
  //ATLTRACE("Bitmaps LEAVED\n");

  return 0;
}


COscView::~COscView()
{
  ATLTRACE("~OscView\n");
  Kill_Timer();
  Sleep(30);
	SAFE_DELETE_ARRAY(m_pOldHPx);
	SAFE_DELETE_ARRAY(m_pOldLPx);
	SAFE_DELETE_ARRAY(m_pOldHPy);
	SAFE_DELETE_ARRAY(m_pOldLPy);
	SAFE_DELETE_ARRAY(m_pTrigBuf);
	SAFE_DELETE_ARRAY(m_pRefBuf);
	SAFE_DELETE_ARRAY(m_pChanBuf);
  if (m_pWriteBmp) delete m_pWriteBmp;

}



LRESULT COscView::Plot(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	register int i,j;
  static int nSec(0);
	long* pWrite(0), lTmp;
  int nTCnt(0);



	//ATLTRACE("View::Plot %8x", m_hWnd);
	//ATLTRACE("View-Plot Thread %x\n", GetCurrentThreadId());
	
  int nCnt = (int)wParam;

	double *pX, *pY;
  
  //Plot - вход в процедуру

  //050402 - почему-то встречается "нулевая" ситуация. Вообще-то, хотелось бы ее сделать флагом окончания...
  // 050402 16:57 - проверка вставлена в место вызыва (CMainFrame::OnTimer), здесь убираю
  if (nCnt <= 0) 
  {
    if (lParam == 0 && m_pWriteBmp)
    {
      m_pWriteBmp->GetNext()->Resize(0);
      m_pWriteBmp->Resize(0);
    }
    return 0;
  }

	CSimpleArray<_ChanW>* pArChanW = (CSimpleArray<_ChanW>*)lParam;
  //nCnt = (*pArChanW)[0].GetNew();
  nCnt++;
  //ATLTRACE("View received %d / %dpoints\n",nCnt);

	int nRefN(0);	//для гарантии - еще один счетчик



	//ATLTRACE("Disp %d Ref %d\n", m_nDispChans, nRefNum);
	long L(0); //обохожусь лонгом потому, что данных только 24 бита.(а каналов - не более 256)
	int nChanIdx;
	nChanIdx = m_nBufChans;

	//если длина введенного участка больше чем встретившаяся до сих пор
	if (nCnt > m_nMaxCnt || m_pRefBuf == 0)
	{
		m_nMaxCnt = nCnt * 12/10;
		//перераспределяем референтный буфе
		SAFE_DELETE_ARRAY(m_pRefBuf);
		m_pRefBuf = new long[m_nMaxCnt];
    //050329 - и все остальные буфера тоже здесь!!!
		SAFE_DELETE_ARRAY(m_pChanBuf);
    if (m_nTotChans)
    {
	    m_pChanBuf = new long[m_nMaxCnt*m_nTotChans];
    }
	}


	//Обнуление референтного буфера
  ZeroMemory(m_pRefBuf, nCnt*sizeof(long));
  if (m_nTotChans)
  {
	  //Обнуление канального буфера (чтобы быть спокойным за биполяры)
    SecureZeroMemory(m_pChanBuf, m_nTotChans*m_nMaxCnt*sizeof(long));
  }

  nChanIdx = 0;

  //остаток от начала текущего фрагмента до границы окна
  int nZeroIdx((m_nLength - m_nWinFrameCnt)%m_nLength);
  if ( nZeroIdx >= nCnt)
    nZeroIdx = -1;

	//за исключением первого запуска (ну - или точки с пометкой m_bChange)
	//первой точкой нового буфера является последняя старого
	// - и это нужно учитывать...
	// Так получается, что с нулевой точкой я ВООБЩЕ НИЧЕГО НЕ ДОЛЖЕН ДЕЛАТЬ!!!????
	int nBeg(1);
	if (m_bChange)
		nBeg = 0;


  //ATLTRACE("Plot: buf = %8x, Cnt = %d(%d)\n", m_pChanBuf,nCnt, m_nMaxCnt);
  //050208 - цикл по переданным (всем вводимым) каналам
  for (i = 0; i < pArChanW->GetSize(); i++)
	{
		//здесь я пользуюсь тем фактом, что беру копию - то есть внутренние указатели
		//на прежнем месте.... Но на сей раз мне нужно изменить оригинал
		_ChanW chanW = pArChanW->operator[](i) ;
		short chanMap = chanW.GetChanNum();
    

    bool bRef = m_dcMapDisp[chanMap].m_bRef;
    bool bDisp = (m_dcMapDisp[chanMap].m_nMono != 0);
    int  nBp = m_dcMapDisp[chanMap].m_arBpNum.GetSize();


    //ATLTRACE("chan %d %d\n", chan, nChanIdx);

    pWrite = m_pChanBuf+m_nMaxCnt*nChanIdx;


    //050221 - обслуживание триггерного канала
    //  ??? нужно ли проверять флаг????
    if (chanMap == -1 )
    {
      nTCnt = chanW.GetNew() + 1;
      //ATLTRACE("nTCnt = %d\n",nTCnt);
      m_nTLength =  chanW.GetFrq() * m_nDur;
      if (nTCnt > m_nMaxTrigCnt)
      {
        m_nMaxTrigCnt = nTCnt*12/10;
        SAFE_DELETE_ARRAY(m_pTrigBuf);
	      m_pTrigBuf = new WORD[m_nMaxTrigCnt];
      }
			for (j = 0; j < nTCnt; j++) 
      {
        lTmp = chanW.Get();
        //if (j < 10) ATLTRACE("%d get %lx\n",i, w);
        m_pTrigBuf[j] = (WORD)lTmp;
      }
    } //конец обслуживания триггерного канала

     
														
    if (chanMap >= 0 && (bRef || bDisp || nBp)) 
		{
			for (j = 0; j < nCnt; j++) 
			{
        lTmp = chanW.Get();
  
        //первый шаг - подсчет рефернтов
        if (bRef) 
        {
					m_pRefBuf[j] += lTmp;
        }

        if (bDisp)
        {
          //Здесь может возникнуть "перекрытие"....
          pWrite[j] = lTmp;
        }

        //050418 - цикл по возможным частям биполяра
        for (int bp = 0; bp < nBp; bp++)
        {
          int nB = m_dcMapDisp[chanMap].m_arBpNum[bp];
          long sB = (nB > 0) ? 1:-1;
          nB = abs(nB) - 1;
          long* pB = m_pChanBuf+m_nMaxCnt*(m_nDispChans + nB);
          pB[j] += lTmp * sB;
          if ( j == 0)
          {
            //ATLTRACE("Bp %d (%d)\n", pB[j],lTmp*sB);
          }
        }
			} // конец цикла по точкам

      //канал был в числе референтов
      if (bRef)
      {
  			nRefN++;
      }

      if (bDisp)
      {
        nChanIdx++;
      }

		} //конец присутствующего канала
	}  //конец цикла по каналам

	if (nRefN) //я все-таки проверю еще один раз - а вдруг каналов не оказалось???
	{
		for (j = 0; j < nCnt; j++) 
		{
			m_pRefBuf[j] /= nRefN;
		} // конец цикла деления
	}

	//050329 - отдельный цикл по отображаемым каналам
  for (nChanIdx = 0; nChanIdx < m_nTotChans; nChanIdx++)
  {
    pWrite = m_pChanBuf+m_nMaxCnt*nChanIdx;
		//050417
		for (j = 0; j < nCnt; j++) 
		{
			lTmp = pWrite[j];

      //только для монополяров - вычитание референта
      if (nChanIdx < m_nDispChans)
        lTmp -= m_pRefBuf[j];

      //фильтр высоких частот
      if (m_cfHP.Is())
			{
				pX = m_pOldHPx+nChanIdx*2;
				if (j >= nBeg) pX[1] = pX[0];
				pX[0] = (double)lTmp;

				pY = m_pOldHPy+nChanIdx*2;
				if (j >= nBeg) pY[1] = pY[0];
				if (j <= nBeg && m_bChange)
				{
					pX[1] = pX[0];
					pY[1] = 0;
				}

				pY[0] = m_cfHP.b[0]*pX[0] + m_cfHP.b[1]*pX[1] -
					m_cfHP.a[1]*pY[1];

        //if (j <= 3)  ATLTRACE("HP (%d) %f %f %f %f\n", j, pX[1], pX[0], pY[1], pY[0]);

				lTmp = (long)pY[0];
			} //if (m_cfHP.Is()) - конец фильтра высоких частот

			if (m_cfLP.Is())
			{
				pX = m_pOldLPx+nChanIdx*3;
        if (j >= nBeg) 
        {
					pX[2] = pX[1];
					pX[1] = pX[0];
        }
				pX[0] = (double)lTmp;

				pY = m_pOldLPy+nChanIdx*3;

        if (j >= nBeg) 
        {
					pY[2] = pY[1];
					pY[1] = pY[0];
        }

        if (j <= nBeg && m_bChange)
				{
					pX[1] = pX[2] = pX[0];
					pY[1] = pY[2] = pX[0];
				}

				pY[0] = m_cfLP.b[0]*pX[0] + m_cfLP.b[1]*pX[1] + m_cfLP.b[2]*pX[2] -
					m_cfLP.a[1]*pY[1] - m_cfLP.a[2]*pY[2];

				lTmp = (long)pY[0];
			} //if (m_cfLP.Is())


      pWrite[j] = lTmp;
  
		} // конец цикла по точкам
    //ATLTRACE("%d %d %d\n", pWrite[0], pWrite[1], pWrite[nCnt-1]);

		//if ((m_bZero || !m_bScroll) && nZeroIdx >= 0) 	m_Zero[nChanIdx] = pWrite[nZeroIdx];
  } // конец цикла по отображаемым каналам




  //050329 - в соответствии с новой идеологией здесь должно пойти рисование...
  //  что мне для него нужно????
  //1 - получаю клиентский прямоугольник - нужен для ширины и высоты

  //Plot - рисование
  CRect rc;
  GetClientRect(&rc);

  //ничего нет - создаю новое
  if (m_pWriteBmp == 0)
  {
    m_pWriteBmp = new _GrBitmap(m_hDC, &rc);
    //первая - ссылка на себя
    m_pWriteBmp->SetNext(m_pWriteBmp);
    m_pReadBmp = m_pWriteBmp;
    m_nOneSecCnt = m_nFrq;
    m_nWinFrameCnt = 0;
    m_nTrigFrameCnt = 0;
    m_nWinBegPix = 0;
  } else {
    //в этом месте - для запуска таймера после второго ввода
    if (m_uTimerID == 0)     Set_Timer(m_nTimer);
  }

  //в этом месте - для запуска таймера после первого ввода
  //if (m_uTimerID == 0)     Set_Timer(20);

  //проверка на выход за пределы окна
  int nBase(0);
  if (m_nWinFrameCnt + nCnt >= m_nLength || m_nTrigFrameCnt + nTCnt >= m_nTLength )
  {
    //050416 - рисовать нужно, вклчая следующую точку
    int nC = m_nLength-m_nWinFrameCnt + 1;
    int nTC = m_nTLength-m_nTrigFrameCnt + 1;

    //ATLTRACE("  WinFrame: %d + %d = %d >= %d\n", m_nWinFrameCnt, nCnt,m_nWinFrameCnt+nCnt, m_nLength);
    //ATLTRACE("  TrigFrame: %d + %d = %d >= %d\n", m_nTrigFrameCnt, nTCnt, m_nTrigFrameCnt+nTCnt, m_nTLength);
    Draw(0, nC, nTC);

    //выходящую за пределы точку убираем
    nC -= 2;
    nCnt -= nC;
    nBase = nC;
    nTCnt -= (nTC-2);
    //ATLTRACE("Draw: L=%d, Fc=%d, C=%d\n", m_nLength, m_nWinFrameCnt, nCnt);
    //переходим на следующую
    //получили (или создали) свободное
    m_pWriteBmp = m_pWriteBmp->GetFree(&rc);
    m_nOneSecCnt = m_nFrq;
    m_nWinFrameCnt = -1;
    m_nTrigFrameCnt = -1;
  } 
  Draw(nBase,nCnt,nTCnt);

		m_bZero = false;
    if (m_bChange)
      RedrawWindow();
	m_bChange = false;

		return 0;
}

LRESULT COscView::OnChangeTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
int nNewTimer = (int)wParam;

  ATLTRACE("ChangeTimer %x %d\n", m_uTimerID, nNewTimer);
  m_nTimer = nNewTimer;
  if (m_uTimerID != 0)
    Set_Timer(m_nTimer);

  return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// CSnapView
LRESULT CSnapView::OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  m_hTextDC.FillSolidRect(0,0,60,m_nWinH,0);
  Plot();
  return S_OK;
}


LRESULT CSnapView::Frame(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
int pnt;
CString str;
Pointer24 *pPointer;
int nOffset;
long Lng;
double cf;
long off;

static char cType[20], cTime[20], cTWord[20];
bool bMapChanged(false);

  FillMemory(m_pOffMap, TOTCHANS, -1);


  if (wParam)
    pPointer = (Pointer24*)wParam;
  else
    pPointer = &m_ReadPointer;

  //050630 - убрано const m_pFChans == 0 для обеспечения работы без аналог. каналов
  if (pPointer == 0)
    return E_FAIL;

  int24* pBuf = pPointer->pBuf;

  if (pBuf == 0)
     return E_FAIL;

  int24* pChBuf;

  m_nCurRecord = pPointer->nRec;
  if (m_nCurRecord >= 0)
    str.Format("Record %d | ",m_nCurRecord + 1);
  else
    str = "Current record | ";
  if (pPointer->bPlus)
  {
    char *cp = (char*)pPointer->pBuf;
    m_flBaseTime = (float)m_pFile->GetRecTypeAndTime(cp, cType, cTime, cTWord);

    while (*cp)
    {
      ATLTRACE(cp);
      ATLTRACE("\n..");
      str += cp;
      str += " | ";
      cp += strlen(cp);
      cp++;
    }
    str.Replace("\024"," ");
    str.Replace("\025"," ");

  }
  else
  {
    m_flBaseTime = g_flNaN;
  }
  ATLTRACE("BaseTime %f\n",m_flBaseTime);

  ::SetWindowText(m_hWndStatusBar, str);

  //Если (как это и должно быть) первый канал - триггер
  //if (m_pFChans[0].nType == 0)
  //{
    pChBuf = pBuf + m_pTChan->dwOffs;
    for (pnt = 0; pnt < m_nTLength; pnt++)
    {
      m_pTrigBuf[pnt] = (WORD)pChBuf[pnt];
    }
  //}


  for(int chan = 0; chan < m_nChanNum; chan++)
  {
    DWORD dwOffs = m_pFChans[chan].dwOffs;
    pChBuf = pBuf + m_pFChans[chan].dwOffs;
    nOffset = 0;

    cf = m_pFChans[chan].flCal;
    off = m_pFChans[chan].nOff;

    if (m_nLength)
    {
      for (pnt = 0; pnt < m_nLength; pnt++)
      {
        Lng = (long)pChBuf[pnt];
        //вот в этом месте я должен учитывать коэфф. и смещение!!!
        m_pChanBuf[chan*m_nLength+pnt] = Lng;
        Lng = (long)((double)Lng*cf) + off;
        nOffset += (Lng >> 14);
      }
      nOffset = abs(nOffset/ m_nLength);
      nOffset = _Settings.GetOffsIdx(nOffset) + 1;
      if (m_pOffMap &&  m_pOffMap[m_pFChans[chan].nNum] != nOffset)
      {
        m_pOffMap[m_pFChans[chan].nNum] = nOffset;
        bMapChanged = true;
      }
    }
  }

  if (!m_bDataPresent)
  {
    m_bDataPresent = true;
    m_colBG = _Settings.GetBgColor();
    m_hTextDC.FillSolidRect(0,0,60,m_nWinH,0);
  }

  if (bMapChanged)
    m_pDialog->PostMessage(WM_DIZ_MAPCHANGED);
  Plot();
  //ATLTRACE("SnapReseived %X\n",m_Pointer.pBuf);
  return S_OK;
}

LRESULT CSnapView::OnCfChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == 0)
		m_cfLP = m_pDialog->GetCfLP();
	else if (wParam == 1)
		m_cfHP = m_pDialog->GetCfHP();

  Plot();
  return S_OK;
}

LRESULT CSnapView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//SetSplitterRect();
CRect rc;

	ATLTRACE("SnapView::OnSize %8x\n", m_hWnd);

  m_nWinBegPix = 0;
  

  if (m_hOldBmp)
    m_hMemDC.SelectBitmap(m_hOldBmp);

	SAFE_DELETE_HANDLE(m_hMemBmp);
	SAFE_DELETE_HANDLE(m_hTextBmp);
  //::SendMessage(m_hWndStatusBar, WM_SIZE, 0, 0);
	GetClientRect(rc);
  UpdateBarsPosition(rc);
  //RECT rectSB;
  //::GetWindowRect(m_hWndStatusBar, &rectSB);
  //rc.bottom -= rectSB.bottom - rectSB.top;

  m_hMemBmp = CreateCompatibleBitmap(m_hDC,rc.Width(),rc.Height());
	m_hTextBmp = CreateCompatibleBitmap(m_hDC,60,rc.Height());
  m_hOldBmp = m_hMemDC.SelectBitmap(m_hMemBmp);
  m_hOldTBmp = m_hTextDC.SelectBitmap(m_hTextBmp);
  m_nWinH = rc.Height();
  m_nWinW = rc.Width()-30;
  m_nWinFrameCnt = 0;

	
  
  m_hMemDC.FillSolidRect(rc,m_colBG);
  m_hTextDC.FillSolidRect(0,0,60,rc.Height(),0);
	//BitBlt(dc,0,0,rc.right,rc.bottom,m_hMemDC,0,0,SRCCOPY);
	//m_nOldCnt = 0;


  Plot();
	bHandled = FALSE;
	ATLTRACE("SnapView::OnSize --- end\n");

  return S_OK;
}

LRESULT CSnapView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  LRESULT res = CActDView::OnCreate(uMsg, wParam, lParam, bHandled);
  if (res == S_OK)
  {
   	CreateSimpleStatusBar();
	  ::ShowWindow(m_hWndStatusBar, SW_SHOWNOACTIVATE);
	  UpdateLayout();
    ATLTRACE("Snap-create\n");
    m_bDataPresent = false;
  }
  return res;
}

CSnapView::~CSnapView()
{

  //вообще-то фильтровальные "буфера" здесь могут и не понадобиться... Ну да ладно,
  //пусть удаляются - не помешают....
	//SAFE_DELETE_ARRAY(m_pOldHPx);
	//SAFE_DELETE_ARRAY(m_pOldLPx);
	//SAFE_DELETE_ARRAY(m_pOldHPy);
	//SAFE_DELETE_ARRAY(m_pOldLPy);

  //вот эти будут нужны!
  SAFE_DELETE_ARRAY(m_pRefBuf);
	SAFE_DELETE_ARRAY(m_pChanBuf);
  //да и этот, наверное, тоже....
  SAFE_DELETE_ARRAY(m_pTrigBuf);
  SAFE_DELETE_ARRAY(m_pReadBuf24);
  //новые
  SAFE_DELETE_ARRAY(m_pnChanNums);
  SAFE_DELETE_ARRAY(m_pDrawBuf);
}

//050531  WM_DIZ_SNAPINIT, (WPARAM)(&m_arChanW), (LPARAM)(&m_atrParams)
//         m_arpView[i]->PostMessage(WM_DIZ_SNAPINIT, 0, (LPARAM)(&m_atrParams));
LRESULT CSnapView::OnSnapInit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
//CSimpleArray<_ChanW>* parChanW = (CSimpleArray<_ChanW>*)wParam;
//AcqTrParams* patrParams = (AcqTrParams*)lParam;


  SAFE_DELETE_ARRAY(m_pTrigBuf);
  SAFE_DELETE_ARRAY(m_pChanBuf);
  SAFE_DELETE_ARRAY(m_pRefBuf);
  SAFE_DELETE_ARRAY(m_pnChanNums);
  SAFE_DELETE_ARRAY(m_pDrawBuf);
  SAFE_DELETE_ARRAY(m_pReadBuf24);


  m_nLength = 0;
  m_pOffMap = ((CSnapDlg*)m_pDialog)->GetOffMap();

  //длина в мс
  m_nDur = m_pFile->GetDuration();
  //каналов на 1 меньше, так как под триггер не отводим
  m_nChanNum = m_pFile->GetNumOfChans();
  //длина триггерного канала в отсчетах
  m_nTLength = m_pFile->GetTrLen();
  //m_nTLength = (*parChanW)[0].GetLen();
  if (m_nTLength > 0)
    m_pTrigBuf = new WORD[m_nTLength];



  //m_nPinChans = m_pFile->GetNumOfPin();

  if (m_nChanNum > 0)
  {
    //длина аналогового канала в отсчетах
    m_nLength = m_pFile->GetChanLen();
    //m_nLength = (*parChanW)[1].GetLen();
    //канальный буфер - под все вводимые каналы
    m_pChanBuf = new long[m_nLength*m_nChanNum];
    m_pRefBuf = new long[m_nLength];
    m_pDrawBuf = new long[m_nLength];
    m_pnChanNums = new int[m_nChanNum];
    int ch(0);
    for (int chan = 0; chan <= m_nChanNum; chan++)
    {
      if (m_pFile->GetChannelNumber(chan) >= 0 )
        m_pnChanNums[ch++] = m_pFile->GetChannelNumber(chan); //+1 - без триггера!!!
    }

  }
	m_cfLP = m_pDialog->GetCfLP();
	m_cfHP = m_pDialog->GetCfHP();

  m_pReadBuf24 = new int24[m_pFile->GetRecSize()];
  m_ReadPointer.bPlus = 0;
  m_ReadPointer.pBuf = m_pReadBuf24;
  m_pFChans = m_pFile->GetFileChans();
  m_pTChan = m_pFile->GetTrChan();
  if (m_pFile->IsFile() == GENERIC_READ)
  {
    PlotRecord(0);
  }
  else if (m_pFile->IsFile() == GENERIC_WRITE)
  {
    if (m_nCurRecord < 0)
      m_nCurRecord = m_pFile->GetRecNum() - 1;
    PlotRecord(m_nCurRecord);
  }

  return S_OK;
}

BOOL CSnapView::Plot()
{
int chan;
int pnt;
long* pBuf;
HPEN penOld;
COLORREF Bg(RGB(50,50,50));
COLORREF color;

  if (!m_bDataPresent || (m_nTLength <= 0 && m_nLength <= 0)) // || m_nChanNum <= 0 || m_pChanBuf == 0)
    return FALSE;

	ATLTRACE("CSnapView: plot\n");
  const COLORREF *pTrCol = _Settings.GetTrigColors();
  WORD wTrigMask = _Settings.GetTrigWord();
  int nResol = m_pDialog->m_nScalePnt;


  CRect rc;
	GetClientRect(&rc);
  UpdateBarsPosition(rc);
  m_nWinH = rc.Height();
  rc.right -= 30;
  m_nWinW = rc.Width();
  int y0(0);
  int h(0);

  int nDispChans = m_pDialog->GetDispNum();
  int nBpChans = m_pDialog->GetBiPolNum();
  m_nTotChans = nDispChans + nBpChans;


  //отдельная зона для триг-канала
  int nTrigZone = (m_pDialog->IsTrig()) ? 55:0;

  //Карта отображаемых каналов
	CStatMap* pDispMap(0);
	//карта отображаемых каналов
	pDispMap = m_pDialog->GetDispMap();
   //Карта референтных каналов
	CStatMap* pRefMap(0);
	int nRefNum = m_pDialog->GetRefNum();
  if (nRefNum)
	{
		//карта референсных каналов
		pRefMap = m_pDialog->GetRefMap();
  }


  if (m_hMemBmp == 0)
  {
	  m_hMemBmp = CreateCompatibleBitmap(m_hDC,rc.Width(),rc.Height());
	  m_hTextBmp = CreateCompatibleBitmap(m_hDC,60,rc.Height());
  }

  CRect rc1(rc);
  //COLORREF col(_Settings.GetBgColor());

  if (m_nTotChans)
    h = (m_nWinH-nTrigZone)/m_nTotChans;
  else
    h = 0;

	m_hMemDC.FillSolidRect(rc, _Settings.GetBgColor());

  if (nBpChans)
	{
    y0 = nDispChans*h+4;
    //rc.bottom = y0;
    rc1.top = y0;
    rc1.bottom -= nTrigZone;
    m_hMemDC.FillSolidRect(&rc1,_Settings.GetBPColor()); 
    penOld = m_hMemDC.SelectPen(pnWarning);
    m_hMemDC.MoveTo(rc.left,y0);
		m_hMemDC.LineTo(rc.right, y0);
    m_hMemDC.SelectPen(penOld);
	}

	y0 = -h/2 + 4;



  int nRefN(0);
  //обнуление референтного массива
  ZeroMemory(m_pRefBuf,m_nLength*sizeof(long));

  int nLen(-1);

  if (nRefNum)
  {
    for (chan = 0; chan < m_nChanNum; chan++)
    {
      if (pRefMap->GetCell(m_pnChanNums[chan]))
      {
        if (nLen < 0)
        {

        }
        for (pnt = 0; pnt < m_nLength; pnt++)
        {
          m_pRefBuf[pnt] += m_pChanBuf[chan*m_nLength+pnt];
        }
        nRefN++;
      }
    }
    //ATLASSERT(nRefN == nRefNum);
    if (nRefN)
    {
      for (pnt = 0; pnt < m_nLength; pnt++)
      {
        m_pRefBuf[pnt] /= nRefN;
      }
    }
    
  }

  //дальше - хуже... Мне же каким-то образом нужно сначала определить, сколько у меня каналов
  // а потом - пройтись по ним. Иначе с "нулевыми линиями" и прочей вертикальной ерундой не разберусь


  for (chan = 0; chan < m_nChanNum; chan++)
  {
    if (pDispMap->GetCell(m_pnChanNums[chan]))
    {

      pBuf = m_pChanBuf + chan*m_nLength;
      for (pnt = 0; pnt < m_nLength; pnt++)
      {
        m_pDrawBuf[pnt] = pBuf[pnt]-m_pRefBuf[pnt];
      }

      //фильтр high-pass


      //фильтр low-pass

 
      //ось

			y0 += h;
		  m_hTextDC.SetTextColor(_Settings.GetBgColor() ^ _Settings.GetColor(chan));

      m_hTextDC.TextOut(1,y0-8, pDispMap->GetName(m_pnChanNums[chan]) );
      //вот это все выделяется в отдельную ф-ию....
      PolyLine(chan, h, y0, nResol);
    }
    

  }

  _BiPol *pBP = m_pDialog->GetBiPolArr()->GetData();

  for (int bp = 0; bp < m_pDialog->GetBiPolNum(); bp++)
  {
		int chan1 = pBP[bp].GetSubtr();
		//if (chan >= 256) chan = chan-256+m_nPinChans; //for mode 4
		int chan2 = pBP[bp].GetMinue();
		//if (chan2 >= 256) chan2 = chan2-256+m_nPinChans; //for mode 4
    long *pBuf2(0);
    pBuf = 0;
		//040717 - проверка присутствия обоих каналов
		if ( m_pDialog->m_pMainMap[chan1] && m_pDialog->m_pMainMap[chan2])
    {
      //оба канала присутствуют - нужно определить их 
      //  положение в получаемом списке
      for (int c = 0; c < m_nChanNum; c++)
      {
        if (m_pnChanNums[c] == chan1)
          pBuf = m_pChanBuf + c*m_nLength;
        if (m_pnChanNums[c] == chan2)
          pBuf2 = m_pChanBuf + c*m_nLength;
      }
      if (pBuf && pBuf2)
      {
        for (pnt = 0; pnt < m_nLength; pnt++)
        {
          m_pDrawBuf[pnt] = pBuf[pnt]-pBuf2[pnt];
        }
      }
    }

  	y0 += h;
	  m_hTextDC.SetTextColor(_Settings.GetBgColor() ^ _Settings.GetColor(bp));
    m_hTextDC.TextOut(1,y0-8, pBP[bp].GetName() );

    //вот это все выделяется в отдельную ф-ию....
    PolyLine(bp, h, y0, m_pDialog->m_nBPScalePnt);

  } //конец цикла по биполярам

	if (nTrigZone)
	{
    DWORD dwTr;
    //ATLTRACE("    %d\n", nTCnt);
		for (int bit = 0; bit < 16; bit++) {
			int x(0),X0(-1),Y0(-100);
			int y = rc.bottom - 52 + bit * 3 + bit/8*2;
			DWORD dwMask = 1 << bit;
			for (pnt = 0; pnt < m_nTLength; pnt++) {
				int yy(y);
				x = (pnt*m_nWinW)/m_nTLength; 
				if (x == X0 && y == Y0) continue;
				if (X0 < 0) X0 = x;
        dwTr = m_pTrigBuf[pnt] ^ wTrigMask;

        if ((dwTr & dwMask)) {
					color = Bg;
				} else {
					color = pTrCol[bit];
					//yy--;
					//y = y+1;
				}
				for (int xx = X0; xx <= x; xx++) {
          m_hMemDC.SetPixel(xx,yy,color);
          m_hMemDC.SetPixel(xx,yy+1,color);
					//m_pWriteBmp->SetPixels(xx,yy,color);
					//m_hMemDC.SetPixel(xx,yy+2,color);
				}
				X0 = xx; Y0 = yy;
			} //конец цикла по точкам
		} //конец цикла по битам
	} // if (nTrigZone)


  //050716 - тута я буду пытаться отрисовать метки времени
  if (m_flBaseTime != g_flNaN)
  {
    int nBase = (int)( Round(m_flBaseTime*1000) );
    int nTime = nBase;
    char cTime[32];
    if (nBase < 0)
    {
      //Trig - preTrig time
      nTime = (-nBase)%1000;
    }
    else
      nTime = 1000-(nBase%1000);
    int x(0);
    HPEN penOld = m_hMemDC.SelectPen(pnGrey);

    m_hMemDC.MoveTo(0,rc.top);
    m_hMemDC.LineTo(0,rc.bottom);
    sprintf(cTime,"%.2f",(float)nBase/1000.f );
    m_hMemDC.TextOut(1,1,cTime);

    while (nTime < m_nDur)
    {
      x = nTime*m_nWinW/m_nDur;
      m_hMemDC.MoveTo(x,rc.top);
      m_hMemDC.LineTo(x,rc.bottom);
      sprintf(cTime,"%d",(nTime+nBase)/1000 );
      m_hMemDC.TextOut(x+1,1,cTime);
      nTime += 1000;
    }
    m_hMemDC.SelectPen(penOld);
  
  
  }

  RedrawWindow();
	ATLTRACE("CSnapView: plot --- end\n");
  return TRUE;
}

void CSnapView::PolyLine(int chan, int h, int y0, int nResol)
{
long lZero;
long lTmp;
short a,b,c,d;
short max,min;
int nP;
HPEN penOld;
int pnt;
  int x0 = 0;
  int x1 = m_nWinW;


      //фильтр высоких частот
      if (m_cfHP.Is())
			{
        m_X[0] = (double)m_pDrawBuf[0];
        m_Y[0] = 0;

        for (pnt = 0; pnt < m_nLength; pnt++)
        {
          m_X[1] = m_X[0];
          m_Y[1] = m_Y[0];
          m_X[0] = (double)m_pDrawBuf[pnt];

				  m_Y[0] = m_cfHP.b[0]*m_X[0] + m_cfHP.b[1]*m_X[1] -
					  m_cfHP.a[1]*m_Y[1];

          m_pDrawBuf[pnt] = (long)m_Y[0];
        }
			} //if (m_cfHP.Is()) - конец фильтра высоких частот

			if (m_cfLP.Is())
			{
        m_X[1] = m_X[0] = (double)m_pDrawBuf[0];
				m_Y[0] = m_Y[1] = m_X[0];

        for (pnt = 0; pnt < m_nLength; pnt++)
        {
          m_X[2] = m_X[1];
          m_X[1] = m_X[0];
          m_Y[2] = m_Y[1];
          m_Y[1] = m_Y[0];
          m_X[0] = (double)m_pDrawBuf[pnt];



				  m_Y[0] = m_cfLP.b[0]*m_X[0] + m_cfLP.b[1]*m_X[1] + m_cfLP.b[2]*m_X[2] -
					  m_cfLP.a[1]*m_Y[1] - m_cfLP.a[2]*m_Y[2];

  				m_pDrawBuf[pnt] = (long)m_Y[0];
        }
			} //if (m_cfLP.Is())


      //050602 - ноль считаем по первым 50 точкам
      lZero = 0;
      for (pnt = 0; pnt < 50 && pnt < m_nLength; pnt++)
      {
        lZero += m_pDrawBuf[pnt];
      }
      lZero /= pnt;

      
      m_hMemDC.SelectPen(pnGrey);
      m_hMemDC.MoveTo(x0,y0);
      m_hMemDC.LineTo(x1,y0);

      //построение ломаной
      
			a = x0; b = a;
      c =(short)((lZero-m_pDrawBuf[0])*h/nResol);
      min = max = d = c;
      nP = 0;
			m_Pnt[nP++] = CPoint(x0,d+y0);
			//min = max = -1;
			// безреферентное отведения - просто вычитаем первую точку буфера

      //if (chan == 0)  ATLTRACE(" Pl: (j + %d) * %d / %d = %d\n",m_nWinFrameCnt, m_nWinW, m_nLength, x0);

			for (pnt = 1; pnt < m_nLength ; pnt++ ) 
			{
				b = (short)((pnt)  * (m_nWinW)/m_nLength);

        //if (chan == 0 && j < 3) ATLTRACE(" Pl: (%d + %d) * %d / %d = %d\n",j,m_nWinFrameCnt, m_nWinW, m_nLength, b);

        //проверка на перехлест здесь не нужна
				//while (nBufPos >= m_nLengPlus) nBufPos -= m_nLengPlus;

				lTmp = m_pDrawBuf[pnt];
        lTmp = lZero - lTmp;
        lTmp = lTmp*h/nResol;
				if (lTmp + y0 > m_nWinH) 
					lTmp = m_nWinH - y0-10;
				if (lTmp + y0 < 0) 
					lTmp =  10-y0;

				c = (short)lTmp;

				if ( a == b ) {
					if ( min > c ) min = c;
					else if ( max < c ) max = c;
				} else {
					if ( d < c ) {
						m_Pnt[nP].x = a; m_Pnt[nP++].y = min+y0; //CPoint(a,min);
						if ( min != max  )  {
							m_Pnt[nP].x = a; m_Pnt[nP++].y = max+y0; //CPoint(a,max);
						}
					} else {
						m_Pnt[nP].x = a; m_Pnt[nP++].y = max+y0; //CPoint(a,max);
						if ( min != max  )  {
							m_Pnt[nP].x = a; m_Pnt[nP++].y = min+y0; //CPoint(a,min);
						}
					}
				a = b; d = c;
				min = max = c;
				}
			} //конец цикла по j

			m_Pnt[nP++] = CPoint(b,c+y0);
      //ATLTRACE("Plot: nP = %d\n", nP);
			if ( nP < 2 ) 
				a = 2;

      //попробуем обозначить границу


			pnGreen.DeleteObject();
			pnGreen = CreatePen(PS_SOLID,1,_Settings.GetColor(chan));
      m_hMemDC.SelectPen(pnGreen);
      m_hMemDC.Polyline(m_Pnt, nP);

}

BOOL CSnapView::PlotRecord(int nRec)
{
  BOOL bH;
  if (m_ReadPointer.pBuf)
  {
    m_nCurRecord = nRec;
    if (m_nCurRecord >= 0 && nRec < m_pFile->GetRecNum())
    {
      m_pFile->ReadRecord(nRec, &m_ReadPointer);
      Frame(0, 0, 0, bH);
    }
    else
    {
      m_bDataPresent = false;
    }


  }
  return true;
}
