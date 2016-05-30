

#pragma once
#include "StatMap.h"
#include "ActD.h"


class CBiPolDlg :
     public CDialogImpl<CBiPolDlg>
     ,public CDialogResize<CBiPolDlg>
     ,public CMessageFilter
{
private:
     CStatMap m_Map, m_Map2;
		 const char * const m_pMainMap;
		 int m_nChans, m_nTouch;
		 int m_nItem;
		 _BiPol m_bpData;
     char *m_pNames;
		 
public:
     enum { IDD = IDD_BIPOLDLG };

     CBiPolDlg() : m_pMainMap(0), m_nChans(8), m_nTouch(0),
				m_nItem(0), m_bpData() {}

		CBiPolDlg(const char *pMain, int nItem=0, int nChans = 8, int nTouch = 0,
			int nMin = -1, int nSubtr= -1, const char Name[] = "" ) : 
			m_pMainMap(pMain),
			m_nChans(nChans), m_nTouch(nTouch),
			m_nItem(nItem), m_bpData(nMin, nSubtr, Name)
				{}
			
		CBiPolDlg(const char *pMain, int nItem=0, int nChans = 8, int nTouch = 0,
			_BiPol BP = _BiPol(),  char* pNames = 0) : 
			m_pMainMap(pMain),
			m_nChans(nChans), m_nTouch(nTouch),
			m_bpData(BP), m_pNames(pNames)
				{}

		 _BiPol GetData() const {return m_bpData;}

     BOOL PreTranslateMessage(MSG* pMsg);

     BEGIN_MSG_MAP(CBiPolDlg)
          COMMAND_ID_HANDLER(IDOK, OnOK)
          COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
          MESSAGE_HANDLER(WM_DIZ_MAPCHANGED, OnMapChanged)
          //COMMAND_HANDLER(IDC_FRAME, WM_DIZ_MAPCHANGED, OnMapChanged)
					COMMAND_HANDLER(IDC_REMOVE, BN_CLICKED, OnCancel)
					CHAIN_MSG_MAP(CDialogResize<CBiPolDlg>)
     END_MSG_MAP()


  BEGIN_DLGRESIZE_MAP(CBiPolDlg)
          DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_NAME, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_REMOVE, DLSZ_MOVE_X)
          BEGIN_DLGRESIZE_GROUP()
               DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
               DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_X)
          END_DLGRESIZE_GROUP()
          DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
          DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()


     LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

		 LRESULT OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		 //LRESULT OnMapChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnBnClickedRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};


