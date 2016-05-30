// DlgView.h : interface of the CDlgView class
//
// 040418 - выделен из MainFrm
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"

#include <AtlFrame.h>
#include <AtlSplit.h>

#include "StatMap.h"
//#include "LeftDlg.h"
#include "HardDlg.h"

#define _ATL_USE_DDX_FLOAT 1

#include <atlddx.h>

//class CMainFrame;
#include "GrThread.h"
//class GrThread;

class CDlgView :
     public CDialogImpl<CDlgView>,
     public CDialogResize<CDlgView>,
		 public CMessageFilter,
		 public CWinDataExchange<CDlgView>
{ 
protected:
	CStatMap m_mapDisp,m_mapRef;
	int m_nChans,m_nMod;
	int m_nTouch;
	CListViewCtrl m_lstBP;
 	CComboBox m_cmbFLP;
	CComboBox m_cmbFHP;
	CSimpleArray<_BiPol> m_arBP;
	//040625 - ссылка на соотв. окно
	CActDView* m_pView;
	CMainFrame* const m_pFrame;
	GrThread *m_pThread;

  //050507 - имена
  char *m_pNames;

	/* удалены 050208
	//040703
	//¬ектор референтных каналов
	CSimpleArray<int> m_arRef;
	//¬ектор отображаемых каналов
	CSimpleArray<int> m_arDisp;
	*/

	//020508
	//количество референтных каналов
	int m_nRef;
	//количество отображаемых каналов
	int m_nDisp;

	//040710
	// == 1 for uV, == 1000 for mv
	int m_nScaleUnit;
	// scale itself
	double m_flScale;

  //050805 - separate scale for BP
	// == 1 for uV, == 1000 for mv
	int m_nBPScaleUnit;
	// scale itself
	double m_flBPScale;

	//Trigger
	BOOL m_bTrigger;
	BOOL m_bBiPol;


  //050419 - инверси€ тригканала
  //   убрана 050504 - теперь инд. маска! BOOL m_bTrigInv;
  //050427
  CBitmap m_bmpTool;
  int m_nNumber;
  CString m_strSection;
  int m_nFLP;
  int m_nFHP;

  //050503
  CString m_strName;
  int m_nTimer;

  //050504
  BOOL m_bSaveOnClose;

public:

	const char *m_pMainMap;

	// scale in 'points' (1 == 1/32 uV)
	int m_nScalePnt;
  //050805
  int m_nBPScalePnt;

  const int m_nType;
	virtual void Pure() = 0;
	
	enum { IDD = IDD_DLGVIEW };

		 CDlgView(CMainFrame* pFrm = 0, int Num = 0, int Type = -1) : m_pView(0)
				,m_pFrame(pFrm), m_pThread(0), m_nNumber(Num), m_nTimer(20),
        m_bSaveOnClose(0), m_pNames(0), m_nFHP(0), m_nFLP(0), 
        m_nScalePnt(20*32), m_nScaleUnit(1), m_flScale(20), m_nType(Type),
        m_nDisp(0), m_nRef(0)
         {}
		 ~CDlgView();
     BOOL PreTranslateMessage(MSG* pMsg);

     BEGIN_MSG_MAP(CDlgView)
					COMMAND_ID_HANDLER(IDOK, OnOK)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
					MESSAGE_HANDLER(WM_CHILDACTIVATE, OnActivate)
          MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
          MESSAGE_HANDLER(WM_SIZE, OnSize)
					NOTIFY_HANDLER(IDC_BPLIST, NM_DBLCLK, OnNMDblclkBplist)
					COMMAND_HANDLER(IDC_CLOSE, BN_CLICKED, OnBnClickedClose)
					//COMMAND_HANDLER(IDC_LENGTH, CBN_SELENDOK, OnCbnSelendokLength)
					NOTIFY_HANDLER(IDC_SSCALE, UDN_DELTAPOS, OnDeltaposSscale)
					NOTIFY_HANDLER(IDC_SBPSCALE, UDN_DELTAPOS, OnDeltaposSscale)
					COMMAND_HANDLER(IDC_TRIGGER, BN_CLICKED, OnBnClickedTrigger)
					COMMAND_HANDLER(IDC_BIPOLAR, BN_CLICKED, OnBnClickedBipolar)
					//COMMAND_HANDLER(IDC_SCROLL, BN_CLICKED, OnBnClickedScroll)
					//COMMAND_HANDLER(IDC_ZERO, BN_CLICKED, OnBnClickedZero)
					COMMAND_HANDLER(IDC_CMBFLP, CBN_SELENDOK, OnCmbFLP)
					COMMAND_HANDLER(IDC_CMBFHP, CBN_SELENDOK, OnCmbFHP)
          MESSAGE_HANDLER(WM_DIZ_MAPCHANGED, OnMapChanged)
          //NOTIFY_HANDLER(IDC_S_LENGTH, UDN_DELTAPOS, OnDeltaposSLength)
          COMMAND_HANDLER(IDC_TOOL, BN_CLICKED, OnBnClickedTool)
          CHAIN_MSG_MAP(CDialogResize<CDlgView>)
          DEFAULT_REFLECTION_HANDLER()
     END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CDlgView)
          DLGRESIZE_CONTROL(IDC_TRIGGER, DLSZ_MOVE_X|DLSZ_MOVE_Y)
          DLGRESIZE_CONTROL(IDC_CLOSE, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_BIPOLAR, DLSZ_MOVE_X)
					DLGRESIZE_CONTROL(IDC_BPLIST, DLSZ_MOVE_X|DLSZ_SIZE_Y)
					DLGRESIZE_CONTROL(IDC_BPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_SBPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSTATIC, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSCALEUNIT, DLSZ_MOVE_X|DLSZ_MOVE_Y)

          DLGRESIZE_CONTROL(IDC_REFTXT, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_DISPTXT, DLSZ_SIZE_X)
          BEGIN_DLGRESIZE_GROUP()
               DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
               DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_X)
          END_DLGRESIZE_GROUP()
          DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
          DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  //050503 - загрузка параметров из именованной секции
  virtual bool LoadSettings(bool bFulLoad, LPCTSTR sSect = 0);
  virtual void SaveSettings(LPCTSTR sSect = 0);
	
	void SetNofChans(int nChans, int nTouch) 
	{
		m_nChans = nChans;
		m_nTouch = nTouch;

		m_mapDisp.Resize(-1,m_nChans,nTouch);
		m_mapRef.Resize(-1,m_nChans,nTouch);
		RedrawMap();
	}


	void SetMainMap (const char *map,  char *pNames) 
	{
		m_pMainMap = map;
    m_pNames = pNames;
		m_mapDisp.SetMainMap(map, pNames);
		m_mapRef.SetMainMap(map, pNames);
	
	}

	void CorrectMap();
	void RedrawMap() {m_mapDisp.RedrawWindow(); m_mapRef.RedrawWindow();}

     
	LRESULT OnNMDblclkBplist(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnCbnSelendokLength(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//050208 - € теперь не держу списков, возвращаю просто номера и ссылки на карты
	int GetRefNum() const {return m_nRef; }
	int GetDispNum() const {return m_nDisp; }
	CStatMap* GetRefMap() {return &m_mapRef;}
	CStatMap* GetDispMap() {return &m_mapDisp;}



	// 040710
	virtual BOOL DoDataExchange(BOOL bSaveAndValidate = FALSE, UINT nCtlID = (UINT)-1);
	virtual BOOL EditUpdate(int idFocus, double flValue = -1.);
	LRESULT OnDeltaposSscale(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedTrigger(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	bool IsTrig() const {return m_bTrigger != 0;}
	int GetBiPolNum() const 
	{
		if (m_bBiPol)
			return m_arBP.GetSize();
		else 
			return 0;
	}
	CSimpleArray<_BiPol>* GetBiPolArr() {return &m_arBP;}
	LRESULT OnBnClickedBipolar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	//040726
	CActDView* GetView() const {return m_pView;}
	LRESULT OnCmbFLP(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCmbFHP(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	//041001 - filters
	fltCf2 GetCfLP();;
	fltCf1 GetCfHP();
  LRESULT OnBnClickedTool(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  int GetNumber() const {return m_nNumber;}
  //050625
  virtual bool ChangeRec(DWORD vKey)
  {
    return false;
  }

  //050630
  virtual void SetOffMap() {}
  //050701
  virtual bool GoToRec(int Rec) {return false;}
  virtual bool IsUnLocked() const {return false;}
  virtual bool IsLocked() const {return false;}

  virtual void OnFinalMessage(HWND hWnd)
  {
    ATLTRACE("DlgFinalMessage %d %x\n", m_pThread);
    //delete this;
  }

  //050802
  void SetViewTitle();
};

class COscDlg: 
  //public CDialogImpl<COscDlg>, 
  public CDlgView
       
{
private:
	CUpDownCtrl m_udDur;
	int m_nDur;
	BOOL m_bScroll;

public:
  virtual void Pure() {}

  enum { IDD = IDD_OSCDLG };

  LRESULT OnDeltaposSLength(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
	LRESULT OnBnClickedScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedZero(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  virtual BOOL DoDataExchange(BOOL bSaveAndValidate = false , UINT nCtlID = -1);
	virtual BOOL EditUpdate(int idFocus, double flValue = -1.);
  //050503 - загрузка параметров из именованной секции
  virtual bool LoadSettings(bool bFulLoad, LPCTSTR sSect = 0);
  virtual void SaveSettings(LPCTSTR sSect = 0);

     BEGIN_MSG_MAP(COscDlg)
					COMMAND_ID_HANDLER(IDOK, OnOK)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
					//MESSAGE_HANDLER(WM_CHILDACTIVATE, OnActivate)
          MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
					COMMAND_HANDLER(IDC_SCROLL, BN_CLICKED, OnBnClickedScroll)
					COMMAND_HANDLER(IDC_ZERO, BN_CLICKED, OnBnClickedZero)
          NOTIFY_HANDLER(IDC_S_LENGTH, UDN_DELTAPOS, OnDeltaposSLength)
          CHAIN_MSG_MAP(CDlgView)
          DEFAULT_REFLECTION_HANDLER()
     END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(COscDlg)
          DLGRESIZE_CONTROL(IDC_TRIGGER, DLSZ_MOVE_X|DLSZ_MOVE_Y)
          DLGRESIZE_CONTROL(IDC_CLOSE, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_BIPOLAR, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_LENGTH, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_SLENGTH, DLSZ_MOVE_X)
					DLGRESIZE_CONTROL(IDC_BPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_SBPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSTATIC, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSCALEUNIT, DLSZ_MOVE_X|DLSZ_MOVE_Y)

          DLGRESIZE_CONTROL(IDC_REFTXT, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_DISPTXT, DLSZ_SIZE_X)
          BEGIN_DLGRESIZE_GROUP()
               DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
               DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_X)
          END_DLGRESIZE_GROUP()
          DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
          DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()


  COscDlg(CMainFrame* pFrm = 0, int Num = 0) : 
        CDlgView(pFrm, Num, 0)
        ,m_bScroll(0), m_nDur(8)
        {
//          IDD = IDD_OSCDLG;
        }
  HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL);
};


class CSnapDlg: 
  //public CDialogImpl<COscDlg>, 
  public CDlgView
       
{
private:
  int m_nTrNum;
  int m_nSelBeg;
  int m_nSelEnd;
  int m_nViewBeg;
  int m_nViewEnd;
  int m_nRecNum;
  char m_cOffMap[TOTCHANS];
	CUpDownCtrl m_udTrNum;
  bool m_bUnLock;
  CBitmap m_bmpLock;
  CBitmap m_bmpUnLock;

  bool _GoToRec(int nRec);

public:
  virtual void Pure() {}

  enum { IDD = IDD_SNAPDLG };

	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  virtual BOOL DoDataExchange(BOOL bSaveAndValidate = false , UINT nCtlID = -1);
	virtual BOOL EditUpdate(int idFocus, double flValue = -1.);
  //050503 - загрузка параметров из именованной секции
  virtual bool LoadSettings(bool bFulLoad, LPCTSTR sSect = 0);
  virtual void SaveSettings(LPCTSTR sSect = 0);

     BEGIN_MSG_MAP(CSnapDlg)
					COMMAND_ID_HANDLER(IDOK, OnOK)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
					//MESSAGE_HANDLER(WM_CHILDACTIVATE, OnActivate)
          MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
          COMMAND_HANDLER(IDC_TRPREV, BN_CLICKED, OnBnClickedTrprev)
          COMMAND_HANDLER(IDC_TRFIRST, BN_CLICKED, OnBnClickedTrfirst)
          COMMAND_HANDLER(IDC_TRNEXT, BN_CLICKED, OnBnClickedTrnext)
          COMMAND_HANDLER(IDC_TRLAST, BN_CLICKED, OnBnClickedTrlast)
					NOTIFY_HANDLER(IDC_STRNUM, UDN_DELTAPOS, OnDeltaposStrnum)
          COMMAND_HANDLER(IDC_LOCK, BN_CLICKED, OnBnClickedLock)
          CHAIN_MSG_MAP(CDlgView)
          DEFAULT_REFLECTION_HANDLER()
     END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CSnapDlg)
          DLGRESIZE_CONTROL(IDC_TRIGGER, DLSZ_MOVE_X|DLSZ_MOVE_Y)
          DLGRESIZE_CONTROL(IDC_CLOSE, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_BIPOLAR, DLSZ_MOVE_X)
					DLGRESIZE_CONTROL(IDC_BPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_SBPSCALE, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSTATIC, DLSZ_MOVE_X|DLSZ_MOVE_Y)
					DLGRESIZE_CONTROL(IDC_BPSCALEUNIT, DLSZ_MOVE_X|DLSZ_MOVE_Y)

          DLGRESIZE_CONTROL(IDC_REFTXT, DLSZ_MOVE_X)
          DLGRESIZE_CONTROL(IDC_DISPTXT, DLSZ_SIZE_X)
          BEGIN_DLGRESIZE_GROUP()
               DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
               DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_X)
          END_DLGRESIZE_GROUP()
          DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
          DLGRESIZE_CONTROL(IDC_FRAME2, DLSZ_SIZE_Y)
  END_DLGRESIZE_MAP()


  CSnapDlg(CMainFrame* pFrm = 0, int Num = 0) : 
        CDlgView(pFrm, Num, 1)
        ,m_nRecNum(-1)
        ,m_bUnLock(false)
        {
        }
  HWND Create(HWND hWndParent, LPARAM dwInitParam = NULL);
  int SetNofRecords (int nRec, int nCurr = -1) 
  {
    m_nRecNum = nRec;
    if (nCurr >= 0 && !m_bUnLock)
    {
      m_nTrNum = nCurr;
      SetDlgItemInt(IDC_TRNUM,m_nTrNum);
    }
		m_udTrNum.SetRange(1,m_nRecNum);
    return m_nRecNum;
  }
  LRESULT OnBnClickedTrprev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedTrfirst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedTrnext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnBnClickedTrlast(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  virtual bool ChangeRec(DWORD vKey);

  //050630
  void SetOffMap()
  {
    m_mapDisp.SetOffMap(m_cOffMap);
    m_mapRef.SetOffMap(m_cOffMap);
  }

  char* GetOffMap() {return m_cOffMap;}

	LRESULT OnDeltaposStrnum(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);

  //050701
  bool GoToRec(int nRec);
  bool IsUnLocked() const {return m_bUnLock;}
  bool IsLocked() const {return !m_bUnLock;}

  //050702
  LRESULT OnBnClickedLock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  int GetCurRecord() const {return m_nTrNum;}
};