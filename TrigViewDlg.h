//created 050504 18:30
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>


// CTrigViewDlg dialog

class CTrigViewDlg :
	public CPropertyPageImpl<CTrigViewDlg>
	,public CWinDataExchange<CTrigViewDlg>
	//,public CSettings
{

public:
	CTrigViewDlg();
	virtual ~CTrigViewDlg();

// Dialog Data
	enum { IDD = IDD_TRIGVIEWDLG };

	BEGIN_MSG_MAP(CTrigViewDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
    COMMAND_CODE_HANDLER(BN_CLICKED, OnBnClicked)
		CHAIN_MSG_MAP(CPropertyPageImpl<CTrigViewDlg>)
  END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStnClickedColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
  LRESULT OnBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	int OnApply();

protected:
  int Find(UINT* Array, UINT Value);
  COLORREF m_colTrig[16];
  BOOL m_bTrigAct[16];
};
