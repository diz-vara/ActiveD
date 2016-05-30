// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <AtlFrame.h>
#include <AtlSplit.h>
#include <AtlCtrlX.h>

#include "StatMap.h"
//#include "LeftDlg.h"
#include "HardDlg.h"
#include "DataFile.h"
#include "DizStatusBar.h"
#include "InfoPanel.h"
#include "ListPanel.h"
#include <Htmlhelp.h>

class CActDView;
class CDlgView;
class CSettSheet;


class CMainFrame : public CFrameWindowImpl<CMainFrame> , public CUpdateUI<CMainFrame>, public CMessageFilter, public CIdleHandler
{

public:

	//ctor
	CMainFrame() : m_nOscCnt(0), 	m_pCurrView(0), m_hAct2(INVALID_HANDLE_VALUE),
		m_dwMode(0), m_bRun(false), m_pChanBuf(0), m_nActChans(-1),
		m_uTimerID(0), m_nOnChans(0), m_dwChanBufLen(0), m_pfFilter(0), m_pFiltBuf(0),
    m_dwBatCMS(0xFFFFFF), m_nPinChans(0), m_nExtChans(0),
    m_pPropSheet(0), m_bNoRec(0), m_bManRec(0),
    m_bManStart(0), m_bManStop(0), m_u64RunTime(0), m_nSnapCnt(0), m_nSnapNum(0),
    m_nOscNum(0), m_pFileMap(0), m_bPresent(0)
    ,m_File(this)
    ,m_ListPanel(this)
    ,m_nCurRec(0)
		,m_bNotActivate(false)
    ,m_bReady(false)
    ,m_bFirst(false)
    //051212
    //ScreenSaver
    ,m_bSSave(false)
    //LowPower
    ,m_bLowPow(false)
    //PowerOff
    ,m_bPowOff(false)
    //ParametersSet
    ,m_bSParams(false)

	 {}

	 //dtor
	 ~CMainFrame();
 
     DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

		 CSimpleArray<CActDView*> m_arpView;
     CSimpleArray<CDlgView*>  m_arpDialog;
		 CActDView *m_pCurrView;

     //CActDView m_view;
     CSplitterWindow m_vSplit;
     CHorSplitterWindow m_hSplit;
     HWND m_hRight,m_hRightD,m_hRightUp,m_hRightE;
     CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;
     CTabbedChildWindow< CDotNetTabCtrl<CTabViewTabItem> > m_LeftTab;
		 //CLeftDlg m_LeftDlg;
		 CHardDlg m_HardDlg;
		 char *m_pMainMap;
     //050601 - файловая карта
     const char* m_pFileMap;
     HWND m_hMainMap;
		 //UCHAR m_MapPresent[TOTCHANS];

     
     CCommandBarCtrl m_CmdBar;

     virtual BOOL PreTranslateMessage(MSG* pMsg);
     virtual BOOL OnIdle();

     BEGIN_UPDATE_UI_MAP(CMainFrame)
          UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
          
          UPDATE_ELEMENT(ID_FILE_NEW, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_FILE_OPEN, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_FILE_CLOSE, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_RUN_GO, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_RUN_PAUSE, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_RUN_WRDISABLE, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_RUN_WRMANUAL, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_OSC_NEW, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_SNAP_NEW, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_EXPORT_MATLAB, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_FILE_OPENCONFIG, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
          UPDATE_ELEMENT(ID_FILE_SAVECONFIG, UPDUI_TOOLBAR|UPDUI_MENUPOPUP)
     END_UPDATE_UI_MAP()

     BEGIN_MSG_MAP(CMainFrame)
          MESSAGE_HANDLER(WM_DIZ_VIEW_ACTIVATE, OnViewActivate)
          MESSAGE_HANDLER(WM_CREATE, OnCreate)
          MESSAGE_HANDLER(WM_SIZE, OnSize)
          COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
          COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
          COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
          COMMAND_ID_HANDLER(ID_FILE_CLOSE, OnFileClose)
          COMMAND_ID_HANDLER(ID_OSC_NEW, OnOscNew)
          COMMAND_ID_HANDLER(ID_SNAP_NEW, OnSnapNew)
          COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
          COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
          COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
          MESSAGE_HANDLER(WM_PAINT, OnPaint)
          MESSAGE_HANDLER(WM_DIZ_DLG_ACTIVATE, OnDlgActivate)
          MESSAGE_HANDLER(WM_DIZ_VIEW_CLOSE, OnViewClose)
          MESSAGE_HANDLER(WM_DIZ_MAPCHANGED, OnMapChanged)
          COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCopy)
					COMMAND_ID_HANDLER(ID_WINDOW_CASCADE, OnWindowCascade)
					COMMAND_ID_HANDLER(ID_WINDOW_TILE, OnWindowTile)
					COMMAND_ID_HANDLER(ID_WINDOW_DOCKALL, OnWindowDockAll)
					MESSAGE_HANDLER(WM_TIMER, OnTimer)
					COMMAND_ID_HANDLER(ID_RUN_GO, OnRunGo)
					COMMAND_ID_HANDLER(ID_RUN_TEST, OnRunTest)
					MESSAGE_HANDLER(WM_CLOSE, OnClose)
					COMMAND_ID_HANDLER(ID_TOOLS_SETTINS, OnToolsSettins)
          COMMAND_ID_HANDLER(ID_TOOLS_LOADELECTRODENAMES, OnToolsLoadelectrodenames)
					COMMAND_ID_HANDLER(ID_RUN_PAUSE, OnPause)
					COMMAND_ID_HANDLER(ID_RUN_WRDISABLE, OnWrDisable)
					COMMAND_ID_HANDLER(ID_RUN_WRMANUAL, OnWrManual)
          MESSAGE_HANDLER(WM_DIZ_READFRAME, OnReadFrame)
          COMMAND_ID_HANDLER(ID_EXPORT_MATLAB, OnMatExp)
          COMMAND_ID_HANDLER(ID_FILE_OPENCONFIG, OnConfOpen)
          COMMAND_ID_HANDLER(ID_FILE_SAVECONFIG, OnConfSave)
          CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
          CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
          COMMAND_ID_HANDLER(ID_HELP_ACTDHELP, OnActdHelp)  
     END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//   LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//   LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//   LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

     LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnOscNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnSnapNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnViewActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnDlgActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnViewClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
     LRESULT OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

		 //040717
		 int GetPinChans() const {return m_aMode.GetPinChans(); }
		 int GetBufChans() const {return m_aMode.GetBufChans(); }
     //050407
     void Stop();

     //050415 - возвращаем финальную (децимированную) частоту
     int GetFrq() const;// {return m_HardDlg.GetFrq(); }
     //050505 - Для неготовых команд
     LRESULT NotImplemented(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnWindowDockAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

     //050608
     CDataFile* const GetFile()   {return &m_File;}
      
     //050701
     LRESULT OnReadFrame(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

     //050729
     const char* GetTitle()
     {
       static char No[] = "no file";
       if (m_File.IsFile() == 0)
         return No;
       else
         return m_File.GetTitle();
     }

     //050801
     LRESULT OnConfOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
     LRESULT OnConfSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

     //050920
     LRESULT OnActdHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

     //051129
     int IsFile() const {return m_File.IsFile();}

protected:
     HWND CreateClient();
		 //счетчик осциллоскопов
		 int m_nOscCnt;
		 //номера осциллоскопов
		 int m_nOscNum;
		 //счетчик снэпшотов
		 int m_nSnapCnt;
		 //номера снэпшотов
		 int m_nSnapNum;
     //HWND m_hWndClient;
		 int m_nChans;

		 //050202 - Включенные каналы
		 int m_nOnChans;
		 //050202 - длина канального буфера
		 DWORD m_dwChanBufLen;


		 //040619
		 //Active-2 handler
		 HANDLE m_hAct2;
		 //режим работы Active2 (int)
		 long m_dwMode;
		 //режим работы Active2 (actMode)
		 actMode m_aMode;
		 //USB - командный блок
		 UCHAR m_ucUsb[64];

		 long *m_pBuf;
		 long *m_pChanBuf;
	   _int64 m_i64Buf,m_i64Disp;
		 DWORD m_dwPointer;
		 bool m_bRun;

		 //количество каналов Active2 (сколько передается)
		 int m_nActChans;

		 //050207 - массив рабочих каналов
	   CSimpleArray<_ChanW> m_arChanW;

     //050219 - критическая секция для доступа к массиву каналов
     CRITICAL_SECTION m_csWChans;

     //050428 - секция для доступа к массивам видов и диалогов
     CRITICAL_SECTION m_csArrays;

     //050430 - секция для таймера
     CRITICAL_SECTION m_csTimer;

     //050228 - фильтры перенесены из CHardDlg
    // double* Filter values
    double* m_pfFilter;
    // длина FIR - фильтра 
    int m_nFiltLen;
    // половина фильтра
    int m_nHalfLen;
    //коэфф. децимации
    int m_nDecCf;
    //позиция в фильтровальном буфере
    int m_nFiltBufPos;
    //буфер для промежуточного хранения подлежащих фильтрации данных
    long *m_pFiltBuf;
    // признак "старта" - нужно заполнять фильтр. буфер
    bool m_bStart;

    //050301 - счетчик входных значений для децимации
    int m_nDecCnt;

    //050419 - флаг децимации триггерного канала
    bool m_bTrigDec;

    //050426 - маска батарей и CMS
    DWORD m_dwBatCMS;
    //отдельно - пин-каналы
    int m_nPinChans;
    //Ext (touchProofs)
    int m_nExtChans;

    //050428 - общая функция для удаления вида и диалога
    void RemoveOsc(int Idx);

    AcqTrParams m_atrParams;

    //050511 - для анализа триггера
    DWORD m_dwOldTrig;

    //050512 - для немодальных пропертей
    CSettSheet* m_pPropSheet;

    //050513
    //функция строит маски, по которым будут отслеживаться изменения триггера
    void BuildTrigEvents();
    //функция будет возвращать целочисленный код того, что нужно будет сделать:
    WORD TestTrig(DWORD dwNewTrig);

    //общее слово - все имеющие значение биты
    DWORD m_dwValidBits;

    //Gate conditions
    DWORD m_dwGateHi[3];
    DWORD m_dwGateLo[3];

    //флаги стоптрига
    //bool  m_bEndTrig;
    //DWORD m_dwEndTrigLoHi;
    //DWORD m_dwEndTrigHiLo;

    //флаги старттрига
    bool  m_bStTrig;
    DWORD m_dwStTrigLoHi;
    DWORD m_dwStTrigHiLo;

    int m_nAcqMode;


    //050516 - пауза ввода
    BOOL m_bPause;

    //050517 - счетчик полученных отсчетов
    DWORD m_dwTickCnt;
    //пауза для повторного триггера - в отсчетах
    DWORD m_dwTrigBlock;
    //050517 последнее полученное триггерное значение
    DWORD m_dwLastTrigCnt;

    //050518
    CDataFile m_File;
    BOOL m_bNeedPause;
    BOOL m_bNeedStart;

    //050523
    BOOL m_bManRec;

    BOOL m_bManStart;
    BOOL m_bManStop;

    //время нажатия кнопки Run
    unsigned hyper m_u64RunTime;

    //050618
    CDizStatusBar m_StsBar;
    CInfoPanel  m_InfoPanel;
    CFInfoPanel  m_FInfoPanel;
    bool m_bPresent;

    //050624
    void RemoveAllViews();

    //050701 - List panel
    CListPanel m_ListPanel;

    //050702 - current record
    int m_nCurRec;

		//050705 - home
		bool m_bNotActivate;

    //050715
    bool m_bReady;

    //050725
    bool m_bFirst;

    //050921
    CString m_strModulePath;

    //051212
    //ScreenSaver
    BOOL m_bSSave;
    //LowPower
    BOOL m_bLowPow;
    //PowerOff
    BOOL m_bPowOff;
    //ParametersSet
    BOOL m_bSParams;

public:
		 //
		 int m_nOffsBuf[300];
		 UINT m_uTimerID;
    BOOL m_bNoRec;



public:
     LRESULT OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		 LRESULT OnRunGo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 LRESULT OnRunTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		 //вторая размерность массива
		 int GetActChans() const { return m_nActChans;}
		 LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		 long* GetMainBuf() const {return m_pBuf;}
		 LRESULT OnToolsSettins(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		void SetTimer(UINT uTime);

    LRESULT OnToolsLoadelectrodenames(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
 	  LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
 	  LRESULT OnWrDisable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
 	  LRESULT OnWrManual(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    //050726
 	  LRESULT OnMatExp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
