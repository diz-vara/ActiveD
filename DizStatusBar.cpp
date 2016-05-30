// CDizStatusBar.cpp : implementation of the CDizStatusBar class
//        050618, 17:00
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\dizstatusbar.h"
#include "resource.h"

CDizStatusBar::CDizStatusBar(void) :
  m_nPower(0)
{
}

CDizStatusBar::~CDizStatusBar(void)
{
    DeleteObject(m_brGreen);
    DeleteObject(m_brRed);
    DeleteObject(m_brBlue);
    DeleteObject(m_brGrey);
}


LRESULT CDizStatusBar::OnSetText(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  bHandled = false;

  //const char *cp = (const char*)lParam;
  //wParam = (WPARAM)(ID_DEFAULT_PANE);
  return S_OK;
}



LRESULT CDizStatusBar::OnSetWindowText(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  //const char *cp = (const char*)lParam;
  //SetPaneText(ID_DEFAULT_PANE,cp);
  return S_OK;
}

LRESULT CDizStatusBar::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{

  return S_OK;
}


LRESULT CDizStatusBar::OnSimple(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  //if (wParam == 0)
  //  SetSimple(false);
  int nCommand = (wParam == 0) ? SW_SHOW:SW_HIDE;
  m_stcPower.ShowWindow(nCommand);
  m_stcBat.ShowWindow(nCommand);
  m_stcCMS.ShowWindow(nCommand);

  bHandled = false; 
  return S_OK;
}

LRESULT CDizStatusBar::Init()
{
  m_brGreen = CreateSolidBrush(cGreen);
  m_brRed = CreateSolidBrush(cRed);
  m_brBlue = CreateSolidBrush(cBlue);
  m_brGrey = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

  m_pbrPower = m_pbrBat = m_pbrCMS = &m_brGrey;

  //SetSimple(false);
  int Panes[4] = {ID_POWER_PANE,ID_BAT_PANE, ID_CMS_PANE, ID_DEFAULT_PANE};
  //int Parts[4] = {140,40,40,100};
  SetPanes(Panes,4,true);

  //SetPaneWidth(ID_POWER_PANE,140);
  //SetPaneWidth(ID_BAT_PANE,40);
  //SetPaneWidth(ID_CMS_PANE,40);


  CRect rc;
  GetPaneRect(ID_POWER_PANE,rc);
  rc.InflateRect(-1,-1);
  m_stcPower.Create(m_hWnd,rc,"No drivers",WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE|SS_CENTER);

  GetPaneRect(ID_BAT_PANE,rc);
  rc.InflateRect(-1,-1);
  m_stcBat.Create(m_hWnd,rc,"BAT",WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE|SS_CENTER);

  GetPaneRect(ID_CMS_PANE,rc);
  rc.InflateRect(-1,-1);
  m_stcCMS.Create(m_hWnd,rc,"CMS",WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_VISIBLE|SS_CENTER);

  //m_stcPower.SetBkMode(TRANSPARENT);
  return S_OK;
}

LRESULT CDizStatusBar::OnCreate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  //Init();

  //CREATESTRUCT *pCS = (CREATESTRUCT*)lParam;
  //Create(pCS->hwndParent,pCS->lpszName);

  //to call the base
  bHandled = false;
  return S_OK;
}

LRESULT CDizStatusBar::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{

  HDC hDC = (HDC)wParam;
  HWND hWnd = (HWND)lParam;

  ::SelectObject(hDC,GetStockObject(DEFAULT_GUI_FONT));
  ::SetBkMode(hDC,TRANSPARENT);

  if (hWnd == m_stcPower)
    return (LRESULT)(*m_pbrPower);
  else if (m_nPower >= 0)
  {
    if (hWnd == m_stcBat)
      return (LRESULT)(*m_pbrBat);
    else if (hWnd == m_stcCMS)
      return (LRESULT)(*m_pbrCMS);
  }


  return (LRESULT)m_brGrey;
}

void CDizStatusBar::SetPower(int Pow)
{

  m_nPower = Pow;
  if (Pow >= 0)
  {
    m_pbrPower = &m_brGreen;
    m_stcPower.SetWindowText("Power ON");
  }
  else if (Pow < 0)
  {
    m_pbrPower = &m_brBlue;
    m_stcPower.SetWindowText("Power OFF/disconnected");
  }
  else
  {
    m_pbrPower = &m_brGrey;
    m_stcPower.SetWindowText("No Drivers");
  }

  m_stcPower.RedrawWindow();
  m_stcBat.RedrawWindow();
  m_stcCMS.RedrawWindow();

}


void CDizStatusBar::SetBatCMS(WORD wBatCMS)
{

  //CMS within range  
  if (wBatCMS & 1)
  {
    m_pbrCMS = &m_brRed;
  }
  else
  {
    m_pbrCMS = &m_brGreen;
  }

  //Battery LOW
  if (wBatCMS & 4)
  {
    m_pbrBat = &m_brRed;
  }
  else
  {
    m_pbrBat = &m_brGreen;
  }

  m_stcBat.RedrawWindow();
  m_stcCMS.RedrawWindow();
}
