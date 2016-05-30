// HeaderDlg.h : interface of the CAboutDlg class
//     050628
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <atlddx.h>


class CHeaderDlg : public CDialogImpl<CHeaderDlg>,
  		 public CWinDataExchange<CHeaderDlg>

{
private:
  char m_lpzDateShort[10];
  char m_lpzDateLong[12];
  char m_lpzTime[10];
  CComboBox m_cmbPatCode;
  CComboBox m_cmbPatSex;
  CComboBox m_cmbMonth;
  CComboBox m_cmbInvCode;
  CComboBox m_cmbResearcher;
  CComboBox m_cmbEquipment;
  CString m_strLocSubj;
  CString m_strLocRec;
  CString m_strSubj;
  CString m_strExp;

  bool m_bPatChanged;
  CSimpleArray<int> m_arNums;
  int m_nSubj;
  //050721 - кол-во пациентов
  int m_nPatNum;
  //050721 - кол-во типов эксперимента
  int m_nInvNum;
  //050721 - кол-во экспериментаторов
  int m_nResNum;
  //050721 - кол-во типов оборудования
  int m_nEquipNum;

	LRESULT CHeaderDlg::ComboString(UINT wID, bool bSelected = false);
  void SavePat();
  int GetList(const char* Sect, int NumOf);
  int SaveOrder(UINT nID);
  bool Check(const char* cp, bool &bNosSpace);

  //051216
  CComboBox m_cmbStimCode;
  int m_nStimNum;

public:
	enum { IDD = IDD_HDRDLG };


  BEGIN_MSG_MAP(CHeaderDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    COMMAND_CODE_HANDLER(CBN_KILLFOCUS, OnCbnKillfocus)
    //COMMAND_HANDLER(IDC_PATCODE, CBN_KILLFOCUS, OnCbnKillfocus)
    //COMMAND_HANDLER(IDC_INVCODE, CBN_KILLFOCUS, OnCbnKillfocus)
    //COMMAND_HANDLER(IDC_RESEARCHER, CBN_KILLFOCUS, OnCbnKillfocus)
    //COMMAND_HANDLER(IDC_EQUIPMENT, CBN_KILLFOCUS, OnCbnKillfocus)
    COMMAND_CODE_HANDLER(CBN_SELENDOK, OnSelEndOk)
    //COMMAND_HANDLER(IDC_PATCODE, CBN_SELENDOK, OnSelEndOk)
    COMMAND_HANDLER(IDC_PBDAY, EN_CHANGE, OnPatChanged)
    COMMAND_HANDLER(IDC_PBMONTH, CBN_SELCHANGE, OnPatChanged)
    COMMAND_HANDLER(IDC_PBYEAR, EN_CHANGE, OnPatChanged)
    COMMAND_HANDLER(IDC_PATNAME, EN_CHANGE, OnPatChanged)
    COMMAND_HANDLER(IDC_PATNAME, EN_KILLFOCUS, OnEnKillfocusPatname)
    COMMAND_HANDLER(IDC_PREVIOUS, BN_CLICKED, OnBnClickedPrevious)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CHeaderDlg)
    //DDX_CONTROL(IDC_PATCODE, m_cmbPatCode)
  END_DDX_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnCbnKillfocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

  int GetLocSubj(char *chLocSubj) const;
  int GetLocRec(char *chLocRec) const;
  int GetShortDate(char *chStartDate) const;
  CString GetSubjStr() const {return m_strSubj;}
  CString GetExpStr() const {return m_strExp;}

  LRESULT OnSelEndOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnPatChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnEnKillfocusPatname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedPrevious(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
