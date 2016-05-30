// ViewSettDlg.h : Declaration of the CViewSettDlg
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>


// CViewSettDlg

class CViewSettDlg : 
	public CDialogImpl<CViewSettDlg>
	//,public CDialogResize<CViewSettDlg>
	,public CMessageFilter
	,public CWinDataExchange<CViewSettDlg>
{
public:
	CViewSettDlg()
	{
	}

	~CViewSettDlg()
	{
	}

	enum { IDD = IDD_VIEWSETDLG };

//public data
  CString m_strName;
  int m_nTimer;
  BOOL m_bSaveOnClose;


//maps
  BEGIN_MSG_MAP(CViewSettDlg)
	  COMMAND_ID_HANDLER(IDOK, OnOK)
	  COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	  MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    //CHAIN_MSG_MAP(CDialogImpl<CViewSettDlg>)
    COMMAND_HANDLER(IDC_LOADDEF, BN_CLICKED, OnOK)
    COMMAND_HANDLER(IDC_SAVEDEF, BN_CLICKED, OnOK)
    COMMAND_HANDLER(IDC_SAVENOW, BN_CLICKED, OnOK)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CViewSettDlg)
    DDX_TEXT(IDC_NAME, m_strName)
    DDX_INT_RANGE(IDC_TIME, m_nTimer, 1, 999)
    DDX_CHECK(IDC_SAVEONCLOSE, m_bSaveOnClose)
  END_DDX_MAP()

// Handler prototypes:
	BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  //BOOL DoDataExchange(BOOL bSaveAndValidate, UINT uID) {  }

};


