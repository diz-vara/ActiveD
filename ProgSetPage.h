//created 050504 21:00
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>

// CProgSetPage dialog

class CProgSetPage : 
	public CPropertyPageImpl<CProgSetPage>
	,public CWinDataExchange<CProgSetPage>
{

public:
	CProgSetPage();
	virtual ~CProgSetPage();

// Dialog Data
	enum { IDD = IDD_PROGSET };

  BEGIN_MSG_MAP(CProgSetPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDC_WINLOAD, OnWinLoad)
    COMMAND_HANDLER(IDC_BFOLDER, BN_CLICKED, OnBFolder)
    COMMAND_HANDLER(IDC_BROWSE, BN_CLICKED, OnBrowse)
    COMMAND_HANDLER(IDC_PROGLOAD, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_PROGSAVE, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_WINLOAD, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_WINDEF, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_WININD, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_BEXP, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_BSUBJ, BN_CLICKED, Change)
    COMMAND_HANDLER(IDC_EFOLDER, EN_CHANGE, Change)

		CHAIN_MSG_MAP(CPropertyPageImpl<CProgSetPage>)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CViewSettDlg)
    DDX_CHECK(IDC_PROGLOAD, _Settings.m_bProgLoad)
    DDX_CHECK(IDC_PROGSAVE, _Settings.m_bProgSave)
    DDX_CHECK(IDC_WINLOAD, _Settings.m_bWinLoad)
    DDX_RADIO(IDC_WINDEF, _Settings.m_nWinLoadInd)
    DDX_CHECK(IDC_BFOLDER, _Settings.m_bFileFolder)
    DDX_CHECK(IDC_BEXP, _Settings.m_bExpSubFolder)
    DDX_CHECK(IDC_BSUBJ, _Settings.m_bSubjSubFolder)
    DDX_TEXT(IDC_EFOLDER, _Settings.m_strFolder)
    DDX_CHECK(IDC_PLUS, _Settings.m_bBdfPlus)
    EnableFolderGroup(_Settings.m_bFileFolder);
  END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	int OnApply();
  
	LRESULT OnWinLoad(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBrowse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT Change(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  int OnKillActive()
  {
    if (OnApply() != PSNRET_NOERROR)
      return 1;
    else
      return 0;
  }

  BOOL OnSetActive()
  {
    GetDlgItem(IDC_PLUS).EnableWindow(_Settings.m_nFileMode != 3);
    return TRUE;
  }

protected:
  void EnableFolderGroup(BOOL bEnable);
};
