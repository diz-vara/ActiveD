#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>

class CSettingsDlg :
	public CDialogImpl<CSettingsDlg>
	,public CDialogResize<CSettingsDlg>
	//,public CMessageFilter
	,public CWinDataExchange<CSettingsDlg>
	,public CSettings
{
private:
	CListBox m_lbColors;


public:
	enum { IDD = IDD_SETDLG };
	CSettingsDlg(void);
	CSettingsDlg(CSettings &Set) : CSettings(Set)
	{

	}
	virtual ~CSettingsDlg(void);

	BEGIN_MSG_MAP(CSettingsDlg)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
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
    CHAIN_MSG_MAP(CDialogResize<CSettingsDlg>)
	END_MSG_MAP()

  BEGIN_DLGRESIZE_MAP(CSettingsDlg)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_SEPARATOR, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_COLLIST, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()

	//BOOL PreTranslateMessage(MSG* pMsg);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


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
};
