// CAcqTrigDlg dialog
// Created 050505 21:02

#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>
#include "SpEdit.h"
#include <Htmlhelp.h>

  typedef struct ArSize
  {
    UINT *pAr;
    int nSize;
  } ArSize;




class CAcqTrigDlg :
	public CPropertyPageImpl<CAcqTrigDlg>
	,public CWinDataExchange<CAcqTrigDlg>
	,public CMessageFilter
{

public:
	CAcqTrigDlg();
	virtual ~CAcqTrigDlg();

// Dialog Data
	enum { IDD = IDD_ACQTRIGDLG };

  BEGIN_MSG_MAP(CAcqTrigDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_CODE_HANDLER(BN_CLICKED, OnBnClicked)
		COMMAND_CODE_HANDLER(EN_KILLFOCUS, OnEnKillFocus)
		COMMAND_CODE_HANDLER(EN_SETFOCUS, OnEnSetFocus)
		//MESSAGE_HANDLER(WM_KEYDOWN, OnChar)
		CHAIN_MSG_MAP(CPropertyPageImpl<CAcqTrigDlg>)
    MESSAGE_HANDLER(WM_HELP, On_Help)  
  END_MSG_MAP()

  BEGIN_DDX_MAP(CAcqTrigDlg)
    DDX_INT_RANGE(IDC_FRAMELEN, m_nFrameLen, 100, 10000)
    DDX_INT_RANGE(IDC_PRETRIG, m_nPreTrig, 0, m_nFrameLen)
    DDX_INT_RANGE(IDC_POSTTRIG, m_nPostTrig,0,m_nFrameLen)
    DDX_INT_RANGE(IDC_TRIDLE, m_nTrIdle,0,10000)
    DDX_INT_RANGE(IDC_MAINTIMER, m_uTimer,50,500)
    //DDX_CONTROL(IDC_KEY_START, m_edStartKey)
  END_DDX_MAP()

  //BOOL DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID = -1);
  LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnEnKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnEnSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	int OnApply();
  BOOL PreTranslateMessage(MSG* pMsg);
  BOOL OnKillActive()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    if (pLoop)
      pLoop->RemoveMessageFilter(this);
    return 0;
  }
  BOOL OnSetActive()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    GetDlgItem(IDC_FRAMELEN).EnableWindow(_Settings.m_nFileMode != 3);

    return 0;
  }


  CSpEdit m_edStartKey;

  //050921
  LRESULT On_Help(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
  {
    HELPINFO *lphi = (HELPINFO*)lParam;
    CString str;
    str.Format("On_Help %06X %06X", lphi->iCtrlId, lphi->dwContextId);
    //MessageBox(str);
    HtmlHelp(
              GetDesktopWindow(),
              g_strHelpFile,
              HH_HELP_CONTEXT,
              lphi->dwContextId) ;

    return 1;
  }

  void OnHelp();

protected:
  //brushes
  //CBrush m_brRed;
  //CBrush m_brGreen;
  CBrush m_brWhite;
  COLORREF m_colWhite;


  //data members
  //Acqusition Mode (0 = free, 1 = gated, 2 = triggered)
  int m_nAcqMode;

  int m_nFrameLen;
  int m_nPreTrig;
  int m_nPostTrig;
  int m_nTrIdle;

  BOOL m_bAnd[3];

  BOOL m_bTrLoHi[16];
  BOOL m_bTrHiLo[16];

  BOOL m_bAHi[16];
  BOOL m_bALo[16];

  BOOL m_bBHi[16];
  BOOL m_bBLo[16];

  BOOL m_bCHi[16];
  BOOL m_bCLo[16];

  int m_uTimer;

  void ShowArray(UINT *pArray, int nSize, BOOL bShow = 0);
  void EnableArray(UINT *pArray, int nSize, BOOL bEnable = 0);
  void ShowGroup(ArSize *pGroup, int nSize, BOOL bShow = 0);
  void EnableGroup(ArSize *pGroup, int nSize, BOOL bEnable = 0);
  void EnableGates();
  WORD Array2Word(const BOOL* pArray);
  void Word2Array(WORD wWord, BOOL *pArray);
  void CheckButtons(BOOL* pArray, UINT *pID);
  BOOL EditUpdate(UINT nID);

};
