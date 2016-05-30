#include "StatMap.h"

class CMapDlg :
	public CDialogImpl<CMapDlg>
	,public CDialogResize<CMapDlg>
	,public CMessageFilter
{
private:
	CStatMap m_Map;
  CStatMap *m_pParMap;
public:
	enum { IDD = IDD_MAPDLG };

	CMapDlg() : m_pParMap(0) {}
	CMapDlg(CStatMap  * const map) : m_pParMap(map), m_Map(*map) {}

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CMapDlg)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    COMMAND_HANDLER(IDC_LOADNAMES, BN_CLICKED, LoadNames)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CDialogResize<CMapDlg>)
	END_MSG_MAP()


  BEGIN_DLGRESIZE_MAP(CMapDlg)
		DLGRESIZE_CONTROL(IDC_LOADNAMES, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X|DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()


	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT LoadNames(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  
  //050506
  void RedrawMap() 
  {
    m_Map.RedrawWindow();
  }
	
};


