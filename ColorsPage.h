//created 050502 0045
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>


class CColorsPage :
	public CPropertyPageImpl<CColorsPage>
	,public CWinDataExchange<CColorsPage>
	,public CSettings
{
public:
	//ctors
	CColorsPage(void);
	CColorsPage(CSettings &Set) : CSettings(Set) 
	{
    m_psp.dwFlags |= PSP_HASHELP;
  }

	~CColorsPage(void);

private:
	CListBox m_lbColors;


public:
	enum { IDD = IDD_COLORSDLG };

	BEGIN_MSG_MAP(CColorsPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		COMMAND_HANDLER(IDC_COLBGBP, STN_CLICKED, OnStnClickedColor)
		COMMAND_HANDLER(IDC_COLBG, STN_CLICKED, OnStnClickedColor)
		COMMAND_HANDLER(IDC_COL1, STN_CLICKED, OnStnClickedColor)
		COMMAND_HANDLER(IDC_COL2ODD, STN_CLICKED, OnStnClickedColor)
		COMMAND_HANDLER(IDC_COL2EVEN, STN_CLICKED, OnStnClickedColor)
		MESSAGE_HANDLER(WM_CTLCOLORLISTBOX, OnCtlColorListbox)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
		COMMAND_HANDLER(IDC_COLLIST, LBN_SELCHANGE, OnLbnSelchangeCollist)
		COMMAND_HANDLER(IDC_COLORS1, BN_CLICKED, OnBnClickedColors)
		COMMAND_HANDLER(IDC_COLORS2, BN_CLICKED, OnBnClickedColors)
		COMMAND_HANDLER(IDC_COLORSC, BN_CLICKED, OnBnClickedColors)
		COMMAND_HANDLER(IDC_NUMCOL, EN_KILLFOCUS, OnEnKillfocusNumcol)
		NOTIFY_HANDLER(IDC_SNUMCOL, UDN_DELTAPOS, OnDeltaposSnumcol)
    COMMAND_HANDLER(IDC_BIPOLSEPCOLOR, BN_CLICKED, OnBnClickedBipolsepcolor)
    //CHAIN_MSG_MAP(CDialogResize<CSettingsDlg>)
		CHAIN_MSG_MAP(CPropertyPageImpl<CColorsPage>)
	END_MSG_MAP()

	//BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);


	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnStnClickedColor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorListbox(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLbnSelchangeCollist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
	LRESULT OnBnClickedColors(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnEnKillfocusNumcol(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDeltaposSnumcol(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
  LRESULT OnBnClickedBipolsepcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	int OnApply();
  int OnKillActive()
  {
    if (OnApply() != PSNRET_NOERROR)
      return 1;
    else
      return 0;
  }

};
