#pragma once
#include "stdafx.h"
#include "resource.h"
#include "actd.h"
#include <atlddx.h>
#include "StatMap.h"

class CMainFrame;
//#include "mainfrm.h"
class CDataFile;

class CHardDlg :
	public CDialogImpl<CHardDlg>,
  public CDialogResize<CHardDlg>,
	public CWinDataExchange<CHardDlg>,
	public CMessageFilter
{
private:
  //050620 - from CLeftDlg
  CStatMap m_Map;

	CMainFrame *m_pMainFrm;
	actMode aMode;
	int m_nChans;
	int m_nTouch;
	BOOL m_bTouch;
	int m_nModules;
	CComboBox m_cmbDecimate;
	// индекс децимации в списки
	int m_nDecIdx;
	//итоговая частота (после децимации)
	int m_nFinalFrq;
	//коэфф. децимации
	int m_nDecCf;

	UINT m_uTimer;
  BOOL m_bTrigDecimate;
  COLORREF m_colBAT;
  COLORREF m_colCMS;
  HBRUSH *m_pbrBAT;
  HBRUSH *m_pbrCMS;
  HBRUSH m_brRed;
  HBRUSH m_brGreen;
  int m_nPresent;

public:
   enum { IDD = IDD_HARDDLG };

	 CHardDlg(void);
	~CHardDlg(void);

   BOOL PreTranslateMessage(MSG* pMsg);

    BEGIN_MSG_MAP(CHardDlg)
          COMMAND_ID_HANDLER(IDOK, OnOK)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
					COMMAND_HANDLER(IDC_MODN, EN_CHANGE, OnEnChangeModn)
					COMMAND_HANDLER(IDC_TOUCH, BN_CLICKED, OnEnChangeModn)
					COMMAND_HANDLER(IDC_DECIMATE, CBN_SELENDOK, OnCbnSelendokDecimate)
          COMMAND_HANDLER(IDC_TRIGDEC, BN_CLICKED, OnBnClickedTrigdec)
          MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
          CHAIN_MSG_MAP(CDialogResize<CHardDlg>)
    END_MSG_MAP()

    BEGIN_DDX_MAP(CHardDlg)
      DDX_INT_RANGE(IDC_MODN, m_nModules, 0, 76)
      DDX_CHECK(IDC_TOUCH, m_bTouch)
      DDX_CHECK(IDC_TRIGDEC, m_bTrigDecimate)
    END_DDX_MAP()

		BEGIN_DLGRESIZE_MAP(CHardDlg)
       DLGRESIZE_CONTROL(IDC_MAP, DLSZ_SIZE_X)
       DLGRESIZE_CONTROL(IDC_MAP, DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	 LRESULT OnEnChangeModn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	 // 040619
	 LRESULT EnableControls(bool bEnable);
	 LRESULT SetMode(short nMode = 0);
	 UINT GetTimer() const {return m_uTimer;}
	 BOOL EditUpdate(UINT idFocus);
	 void SetMainWnd(CMainFrame* pWnd) {m_pMainFrm = pWnd;	/*SetMode(aMode.GetMode());*/}
	 int GetDecimate(bool bTrig = false) const;
	 int GetFrq(bool bTrig = false) const;
	 LRESULT OnCbnSelendokDecimate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedTrigdec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   void SetBatCMS(DWORD dwBatCMS);
   void SaveSettings();

   //050620- from CLeftDlg
	void SetNofChans(int nChans, int nTouch) {
		int N = nChans/32; 
		if (nChans%32) N++;
		m_Map.Resize(-1,nChans,nTouch);
		m_Map.Resize(-1,nChans,nTouch);
	}

  char* GetMap() {return m_Map.m_bOn;}
  size_t GetMapSize() const {return sizeof(m_Map.m_bOn);}

  HWND GetMapHwnd() const {return m_Map.m_hWnd;}
  char* GetNewChanNames() {return m_Map.GetOrCreateNames(); }
  char* GetChanNames() {return m_Map.m_pChNames; }
  const char* GetChanName(int nChan) {return m_Map.GetName(nChan); }
	UCHAR GetCell(int Num) const {return m_Map.GetCell(Num);}
	void RedrawMap() {m_Map.Redraw();}

};
