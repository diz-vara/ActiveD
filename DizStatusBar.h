// CDizStatusBar.h : interface of the CDizStatusBar class
//        050618, 16:00
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "ActD.h"
#include <AtlCtrlS.h>
#include <AtlCtrlX.h>


class CDizStatusBar : public CMultiPaneStatusBarCtrlImpl<CDizStatusBar>
{
public:
  
  DECLARE_WND_SUPERCLASS(_T("CDizStatusBar"), GetWndClassName())

  CDizStatusBar();
  ~CDizStatusBar();
  LRESULT Init();
private:


  HBRUSH m_brRed;
  HBRUSH m_brGreen;
  HBRUSH m_brBlue;
  HBRUSH m_brGrey;

  HBRUSH *m_pbrBat;
  HBRUSH *m_pbrCMS;
  HBRUSH *m_pbrPower;

  int m_nPower;

  CStatic m_stcPower;
  CStatic m_stcBat;
  CStatic m_stcCMS;

  BEGIN_MSG_MAP(CDizStatus)
    MESSAGE_HANDLER(WM_CREATE, OnCreate)
    MESSAGE_HANDLER(WM_CREATE, OnSize)
    MESSAGE_HANDLER(WM_SETTEXT, OnSetWindowText)
    MESSAGE_HANDLER(SB_SETTEXT, OnSetText)
    MESSAGE_HANDLER(SB_SIMPLE, OnSimple)
    MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
    CHAIN_MSG_MAP(CMultiPaneStatusBarCtrlImpl<CDizStatusBar>)
  END_MSG_MAP()

  LRESULT OnSetWindowText(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSetText(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSimple(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
  LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  void SetPower(int Pow);
  void SetBatCMS(WORD wBatCMS);

};
