f// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ActDView.h"
#include "MainFrm.h"
#include ".\mainfrm.h"
#include <AtlMisc.h>
#include "MapDlg.h"
//#include "LeftDlg.h"
#include <math.h>
#include "DlgView.h"
#include "ActD.h"
#include "DataWin.h"
#include "SettSheet.h"
#include "datafile.h"

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"htmlhelp.lib")

extern dOPEN_DRIVER_ASYNC OPEN_DRIVER_ASYNC;
extern dUSB_WRITE USB_WRITE;
extern dREAD_MULTIPLE_SWEEPS READ_MULTIPLE_SWEEPS;
extern dREAD_POINTER READ_POINTER;
extern dCLOSE_DRIVER_ASYNC CLOSE_DRIVER_ASYNC;
extern dCLOSE_DRIVER CLOSE_DRIVER;


void CALLBACK TimeProc(
  UINT uID,      
  UINT uMsg,     
  DWORD dwUser,  
  DWORD dw1,     
  DWORD dw2      
);






CMainFrame::~CMainFrame()
{
	SAFE_DELETE_ARRAY(m_pBuf);

	//050202 - обнуляю число включенных каналов
	m_nOnChans = 0;
	// и длину канального буфера
	m_dwChanBufLen = 0;

	SAFE_DELETE_ARRAY(m_pChanBuf);
  SAFE_DELETE_ARRAY(m_pfFilter);
  SAFE_DELETE_ARRAY(m_pFiltBuf);
  if (m_pPropSheet)
  {
    delete m_pPropSheet;
    m_pPropSheet = 0;
  }
  
  //050219 - удаление критической секции
  DeleteCriticalSection(&m_csWChans);
  DeleteCriticalSection(&m_csArrays);
  DeleteCriticalSection(&m_csTimer);
  ATLTRACE("~CMainFrame\n");

}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
int i;
BOOL b;
char cTime[32], cType[32], cWord[32];

  if (pMsg->message == WM_DIZ_SNAPPLOT)
  {
    int nRec = (int)(pMsg->lParam);
    if (nRec >= 0)
    {
      ATLTRACE("___MainFrame:: WM_DIZ_SNAPPLOT %d\n",nRec);
      Pointer24* P = (Pointer24*)(pMsg->wParam);
      if (P->bPlus)
      {
        m_File.GetRecTypeAndTime((char*)P->pBuf,cType,cTime, cWord);
        m_ListPanel.AddRecord(nRec,cTime,cType, cWord);
      }
      else
      {
        m_ListPanel.AddRecord(nRec,"NN",0);
      }

    }
    for (i = 0; i < m_arpView.GetSize(); i++)
    {
      if (m_arpView[i]->GetType() == 1)
      {
        if (nRec >= 0)
          ((CSnapDlg*)m_arpDialog[i])->SetNofRecords(nRec+1,nRec+1);
        if (((CSnapDlg*)m_arpDialog[i])->IsLocked())
          m_arpView[i]->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
      }
    }
    if (nRec >= 0)
      m_ListPanel.SetRecord(nRec+1); // .AddRecord(nRec,0,0);
    return TRUE;
  }
  else if (pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSCHAR)
  {
    return false;
  }
	else if (pMsg->message == WM_DIZ_CHANGEMODE) {

		m_nChans = (int)pMsg->wParam;
		m_nExtChans = (int)(pMsg->lParam);


		//for (i = 0; i < m_nChans; i++) m_MapPresent[i] = 1;
		//for ( ; i < 9*32; i++) m_MapPresent[i] = 0;

		//m_HardDlg.SetNofChans(m_nChans,m_nExtChans);

    //050428 - секция для доступа к массивам видов и диалогов
    EnterCriticalSection(&m_csArrays);
    //ATLTRACE("PreTranslate: csArrays Entered\n");
		for (i = 0; i < m_arpDialog.GetSize(); i++) {
      if (m_arpDialog[i]->m_nType == 0)
			  m_arpDialog[i]->SetNofChans(m_nChans,m_nExtChans);
		}
    LeaveCriticalSection(&m_csArrays);
    //ATLTRACE("PreTranslate: csArrays Leaved\n");
		m_nActChans = m_aMode.GetBufChans();
    m_nPinChans = m_aMode.GetPinChans();
    //m_nExtChans = m_aMode.GetTouch();

    OnMapChanged(WM_DIZ_MAPCHANGED,0,0,b);
		return TRUE;
	} else if (pMsg->message == WM_DIZ_MAPCHANGED) {
    OnMapChanged(pMsg->message, pMsg->wParam, pMsg->lParam, b);
    return b;

	}


  if (pMsg->hwnd && pMsg->hwnd != m_hWnd)
  {
    if (m_HardDlg.m_hWnd && pMsg->hwnd == m_HardDlg.m_hWnd && m_HardDlg.PreTranslateMessage(pMsg))
		  return TRUE;

    if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		  return TRUE;

    if (m_pCurrView && ::IsWindow(*m_pCurrView)) 	  return m_pCurrView->PreTranslateMessage(pMsg);

		return false; 
  }
  return false;
}

BOOL CMainFrame::OnIdle()
{

  //050428 - секция для доступа к массивам видов и диалогов
  //ATLTRACE("OnIdle: csArrays Entered %X\n", GetCurrentThreadId());
  UIEnable(ID_RUN_GO, m_bPresent && m_bReady && m_File.IsFile() != GENERIC_READ,1);
  //ATLTRACE("OnIdle: csArrays Leaved\n");
	UISetCheck(ID_RUN_GO, m_bRun, 1);

	UISetCheck(ID_RUN_WRDISABLE, ((m_File.IsFile() != GENERIC_WRITE) || m_bNoRec), 1);
  UIEnable(ID_RUN_WRDISABLE, m_File.IsFile() == GENERIC_WRITE, 1);

  UIEnable(ID_RUN_PAUSE, m_bRun, 1);
  UIEnable(ID_RUN_WRMANUAL, m_bRun, 1);

  UIEnable(ID_FILE_NEW, m_bPresent && !m_bRun && !(m_File.IsFile()), 1);
  UIEnable(ID_FILE_OPEN, (!m_bRun && !m_File.IsFile()), 1);
  UIEnable(ID_FILE_CLOSE, (!m_bRun && m_File.IsFile()), 1);
  UIEnable(ID_OSC_NEW, m_bPresent && m_File.IsFile() != GENERIC_READ, 1);
  UIEnable(ID_SNAP_NEW, m_File.IsFile() || (m_bRun && m_nAcqMode == 1), 1);

  UISetCheck(ID_RUN_PAUSE, (m_bPause || m_bNeedPause) && ~m_bNeedStart, 1);
  if (m_nAcqMode == 0)
  {
    //GATE recording
	  UISetCheck(ID_RUN_WRMANUAL, !m_bPause || m_bNeedStart, 1);
  }
  else if (m_nAcqMode == 1)
  {
    //TRIGER recording
	  UISetCheck(ID_RUN_WRMANUAL, (m_bManRec || m_bManStart) && ~m_bManStop, 1);
  }

  UIEnable(ID_EXPORT_MATLAB, m_File.IsFile() && !m_bRun && m_File.GetRecNum() > 0, 1);

  UIEnable(ID_FILE_OPENCONFIG, m_arpView.GetSize() == 0, 1);
  UIEnable(ID_FILE_SAVECONFIG, m_arpView.GetSize() == 0, 1);
  UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  //bHandled = false;
  UpdateLayout();
  return S_OK;
}


LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

  SetThreadLocale(9);

  //050219 - инициализация критической секции               Vстарший бит разрешает исключения
  if (!InitializeCriticalSectionAndSpinCount(&m_csWChans, 0x80000400) ) 
      return 0;

  //050428 - секция для доступа к массивам видов и диалогов
  if (!InitializeCriticalSectionAndSpinCount(&m_csArrays, 0x80000400) ) 
      return 0;
  
  //050430 - секция для таймера
  if (!InitializeCriticalSectionAndSpinCount(&m_csTimer, 0x80000400) ) 
      return 0;
  

  m_pBuf = new long[BUFSIZE];
	_Settings.SetRegistryKey("DIZ\\ActD");
	_Settings.LoadColors();

  // create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

  //050618 - changing simple status bar to the advanced one
	CreateSimpleStatusBar();
  m_StsBar.SubclassWindow(m_hWndStatusBar);
  //m_hWndStatusBar = m_StsBar;
  m_StsBar.Init();
  
  //m_StsBar.SetSimple(false);
  //m_StsBar.Init();


  //m_stcPower.SetBkColor(255);

  //m_hIcon = LoadIcon(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MAINFRAME));
  //m_StsBar.SetPaneIcon(500,m_hIcon);


  m_hWndClient = CreateClient();
	

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

  m_bPause = false;
  m_bNeedPause = false;
  m_bNeedStart = false;

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);



  //050920
  TCHAR szModule1[_MAX_PATH];
  TCHAR szModule2[_MAX_PATH];
  TCHAR *pszFileName;
  ::GetModuleFileName(_Module.GetModuleInstance(),szModule1,_MAX_PATH);
  ::GetFullPathName(szModule1, _MAX_PATH, szModule2, &pszFileName);
  if (pszFileName) 
    *pszFileName = 0;
  m_strModulePath = szModule2;
  g_strHelpFile = m_strModulePath;
  g_strHelpFile += "ActD.chm";

/*

  if (OPEN_DRIVER_ASYNC == 0) 
    MessageBox("DRIVERS NOT FOUND!!!","Error",MB_OK|MB_ICONERROR);
*/

  //050505 - загрузка параметров программы:
  // главная карта
  // кол-во модулей
  // тачпруфы
  // децимация
  // децимация триггера
  if (_Settings.GetProgLoad())
  {
    try
    {
      _Settings.GetStruct("Main","MapPresent",m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
    }
    catch(...)
    {
      ZeroMemory(m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
    }
    _Settings.LoadAcqAndTrigParams();
    
    char *pMap = m_HardDlg.GetMap();
    for (UINT i = 0; i < m_HardDlg.GetMapSize(); i++)
    {
      if (*pMap)
        *pMap = -1;
      pMap++;
    }

    if (_Settings.IsChanNames())
    {
      char *pNames = m_HardDlg.GetNewChanNames();
      _Settings.LoadChanNames(pNames);
    }
  }

  //050520 - Starting File Thread
  //m_File.SetMainFrame(this);
  m_File.Start();

  //050616 - таймер запускаем только если есть драйвера!!!
  if (OPEN_DRIVER_ASYNC)
    m_uTimerID = timeSetEvent(_Settings.GetTimer(),50,&TimeProc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);

  Sleep(50);

  if (_Settings.GetProgLoad() )
  {
    WINDOWPLACEMENT wpl;
    wpl.length = sizeof(WINDOWPLACEMENT);
    try
    {
      _Settings.GetStruct("Main","WindowPlacement",&wpl,sizeof(WINDOWPLACEMENT));
      SetWindowPlacement(&wpl);
    }
    catch(...)
    {
      ShowWindow(SW_RESTORE);
    }

      
  }
  CString str;
  str.Format("ActiveD (%s)", _Settings.GetConfFileTitle());
  SetWindowText(str);

	UpdateLayout();
	//RedrawWindow();


  return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  if (m_File.IsFile())
    return E_FAIL;
  else
  {
    if (m_File.NewFile())
    {
      m_pFileMap = m_File.FileInit(&m_arChanW, &m_atrParams, &m_csWChans, m_dwMode);
      m_nCurRec = 0;
      m_HardDlg.SetMode(m_aMode.GetMode());


      CString str;
      str.Format("%s - ActiveD", m_File.GetTitle());
      SetWindowText(str);

      //m_FInfoPanel.SetFile(&m_File);
      //m_LeftTab.AddTab(m_FInfoPanel,"File Info");
      
      //050701 - List panel
      m_ListPanel.SetFile(&m_File,m_pMainMap);
      m_LeftTab.AddTab(m_ListPanel,"List");

      if (m_nSnapCnt < 1)
        PostMessage(WM_COMMAND,ID_SNAP_NEW,0);
      else
      {
        //снэпшоты уже открыты - нужно им передать файловую карту...
        for (int view = 0; view < m_arpView.GetSize(); view++)
        {
          if (m_arpView[view]->GetType() == 1)
          {
            m_arpDialog[view]->SetMainMap(m_pFileMap, m_File.GetChanNames());
            m_arpDialog[view]->SetOffMap();
            m_arpDialog[view]->SetViewTitle();
        		//m_arpDialog[view]->SetNofChans(m_File.GetPinChans(),m_File.GetExtChans()); //(m_nChans,m_nExtChans);//
          }
        }
      }
      return S_OK;
    }
  }

  return E_FAIL;
}

LRESULT CMainFrame::OnFileClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  if (!m_File.IsFile())
    return 1;

  if (m_File.Close() != S_OK)
    return E_FAIL;
  //050617 - close all windows
  RemoveAllViews();

        
  //m_HardDlg.ShowFile(SW_HIDE);
  m_HardDlg.EnableWindow(true);


  m_FInfoPanel.SetFile(0);
  m_LeftTab.RemoveTab(m_FInfoPanel);
  m_ListPanel.SetFile(0);
  m_LeftTab.RemoveTab(m_ListPanel);

  CString str;
  str.Format("ActiveD (%s)", _Settings.GetConfFileTitle());
  SetWindowText(str);

  return S_OK;
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (m_File.IsFile())
    return 0;
  
  //050617 - close all windows
  RemoveAllViews();

  if (m_File.Open() == S_OK)
  {
    m_nCurRec = 0;
    m_FInfoPanel.SetFile(&m_File);
    m_pFileMap = m_File.GetMap();
    m_LeftTab.AddTab(m_FInfoPanel,"File Info");
    
    //050701 - List panel
    m_ListPanel.SetFile(&m_File);
    m_LeftTab.AddTab(m_ListPanel,"List");
    //m_LeftTab.DisplayTab(m_ListPanel,0);
    //050701 - List panel
    m_LeftTab.DisplayTab(m_FInfoPanel,0);


    CString str;
    str.Format("%s - ActiveD", m_File.GetTitle());
    SetWindowText(str);
    PostMessage(WM_COMMAND,ID_SNAP_NEW);
    return S_OK;
  }
  else
    return E_FAIL;
}


LRESULT CMainFrame::OnOscNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//if (!m_view)


	CString str;
	m_hSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hRightD) ;
	::ShowWindow(m_hRightE,SW_HIDE);

	str.Format("Osc %d",m_nOscNum+1);
	CRect rc;
	::GetClientRect(m_hRightUp,&rc);




	COscDlg *pNewDlg = new COscDlg(this, m_nOscNum);
	
	pNewDlg->Create(m_tabbedChildWindow);
	//m_tabbedChildWindow.AddTab(*pNewDlg, str);
  m_tabbedChildWindow.AddTabWithIcon(pNewDlg->m_hWnd,str,IDI_OSC);
  pNewDlg->SetMainMap(m_pMainMap, m_HardDlg.GetChanNames());
	pNewDlg->SetNofChans(m_nChans,m_nExtChans);
	//pNewDlg->SetView(pNewView);
	m_tabbedChildWindow.DisplayTab(pNewDlg->m_hWnd);

	CActDView *pNewView = pNewDlg->GetView();
	m_pCurrView = pNewView;

	m_nOscCnt++; m_nOscNum++;


  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);
  m_arpDialog.Add(pNewDlg);
	m_arpView.Add(pNewView);
  LeaveCriticalSection(&m_csArrays);

  if (_Settings.GetWinLoad() >= 0)
	  pNewDlg->LoadSettings(true);

  if (m_bRun)
	  pNewView->PostMessage(WM_DIZ_SETFRQ,m_HardDlg.GetFrq(),0	);


	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	_Module.RegisterAppId("ActD");

	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainFrame::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(&rc,0);

	return 0;
}



HWND CMainFrame::CreateClient()
{

		DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | MDIS_ALLCHILDSTYLES;
		DWORD dwExStyle = WS_EX_CLIENTEDGE;

		CLIENTCREATESTRUCT ccs;
		//ccs.hWindowMenu = hWindowMenu;
		//ccs.idFirstChild = nFirstChildID;

		if((GetStyle() & (WS_HSCROLL | WS_VSCROLL)) != 0)
		{
			// parent MDI frame's scroll styles move to the MDICLIENT
			dwStyle |= (GetStyle() & (WS_HSCROLL | WS_VSCROLL));

			// fast way to turn off the scrollbar bits (without a resize)
			ModifyStyle(WS_HSCROLL | WS_VSCROLL, 0, SWP_NOREDRAW | SWP_FRAMECHANGED);
		}
		CRect rcVert;
		GetClientRect(&rcVert);

		// create the vertical splitter
		m_vSplit.Create(m_hWnd, rcVert, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		// set the vertical splitter parameters
		m_vSplit.m_cxyMin = 35; // minimum size
		m_vSplit.SetSplitterPos(115); // from left
		m_vSplit.m_bFullDrag = true; // ghost bar disabled

    m_LeftTab.Create(m_vSplit,rcDefault);

    m_InfoPanel.Create(m_LeftTab);
    m_FInfoPanel.Create(m_LeftTab);
    m_ListPanel.Create(m_LeftTab);
		m_HardDlg.Create(m_LeftTab,WS_DISABLED);
		m_HardDlg.SetMainWnd(this);

		//GetClientRect(m_hRight,&rcVert);
		m_hSplit.Create(m_vSplit.m_hWnd, rcVert, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

		// set the vertical splitter parameters
		m_hSplit.m_cxyMin = 160; // minimum size
		m_hSplit.SetSplitterPos(rcVert.Height()-150); // from top == 40 from bottom
		m_hSplit.m_bFullDrag = true; // ghost bar disabled

		m_vSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hSplit);

		//Главная карта - ссылка на "Левую панель"
		m_pMainMap = m_HardDlg.GetMap();
    //окно главной карты - для сравнения в обработчике
    m_hMainMap = m_HardDlg.GetMapHwnd();



    if (OPEN_DRIVER_ASYNC == 0)
    {
      m_LeftTab.AddTab(m_InfoPanel,"NoHard");

      m_HardDlg.ShowWindow(SW_HIDE);
      m_InfoPanel.ShowWindow(SW_SHOW);
    }
    else
    {
      m_LeftTab.AddTab(m_HardDlg,"Hardware");
      m_HardDlg.ShowWindow(SW_SHOW);
      m_InfoPanel.ShowWindow(SW_HIDE);
      m_bPresent = true;
    }
		
    m_vSplit.SetSplitterPane(SPLIT_PANE_LEFT,m_LeftTab);
		//Right Upper
    HWND hWndClient = ::CreateWindowEx(dwExStyle, _T("MDIClient"), NULL,
          dwStyle, 0, 0, 100, 100, m_hSplit.m_hWnd, (HMENU)LongToHandle(ATL_IDW_CLIENT),
          _Module.GetModuleInstance(), (LPVOID)&ccs);
 
		if (hWndClient == NULL)
    {
          ATLTRACE2(atlTraceUI, 0, _T("Frame failed to create CLIENT.\n"));
          return NULL;
    }

		m_hSplit.SetSplitterPane(SPLIT_PANE_LEFT,hWndClient) ;
		m_hRightUp = hWndClient;



		//Right Lower

	// If you plan on having any tab views needing
	// notifications to be reflected (such as if you
	// have a custom draw list view), be sure to include
	// the following line.  If you know you won't need
	// notifications to be reflected, it can cut down
	// on the message traffic if you don't.
	// Also beware that reflecting notifications back
	// to some common controls (especially a tree view)
	// can cause visual anomolies unless you superclass
	// the control and add "DEFAULT_REFLECTION_HANDLER()"
	// to the message map.
	m_tabbedChildWindow.SetReflectNotifications(true);
	m_LeftTab.SetReflectNotifications(true);
	

	m_tabbedChildWindow.SetTabStyles(CTCS_TOOLTIPS|CTCS_BOTTOM);
	m_LeftTab.SetTabStyles(CTCS_SCROLL);

  m_hRightE = ::CreateWindowEx(dwExStyle, _T("MDIClient"), NULL,
          dwStyle, 0, 0, 100, 100, m_hSplit.m_hWnd, (HMENU)LongToHandle(ATL_IDW_CLIENT),
          _Module.GetModuleInstance(), (LPVOID)&ccs);

	m_hSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hRightE) ;
	m_tabbedChildWindow.Create(m_hSplit, rcDefault);
	m_hRightD = m_tabbedChildWindow.m_hWnd;


    //050601 - "файловая карта" в начале совпадает с главной



		return m_vSplit.m_hWnd;//   .GetSplitterPane(SPLIT_PANE_RIGHT) ;



	//return m_hWndClient;
}
LRESULT CMainFrame::OnEditCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_vSplit.SetSplitterRect();
	return 0;
}


LRESULT CMainFrame::OnViewActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{

	if (m_bNotActivate)
  {
    ATLTRACE("ViewActivate - m_bNotActivate == TRUE\n");
		return S_OK;
  }

	CActDView* pView = (CActDView*)lParam;

  //050428 - секция для доступа к массивам видов и диалогов
  ATLTRACE("OnViewActivate: csArrays wanted\n");
  if (! TryEnterCriticalSection(&m_csArrays))
  {
    //PostMessage(uMsg,wParam,lParam);
    return E_FAIL;
  }

  ATLTRACE("OnViewActivate: csArrays Entered\n");
  int idx(-1);

  try 
  {
	  idx = m_arpView.Find(pView);
	  if (idx < 0) 
      throw("Activate: idx < 0\n");

	  m_pCurrView = pView;

	  if ( wParam == 0 && m_arpDialog.GetSize() > 0)
		  m_tabbedChildWindow.DisplayTab(m_arpDialog[idx]->m_hWnd,0 );

	  for (int i = 0; i < m_arpView.GetSize(); i++) 
    {
		  m_arpView[i]->m_bActive = (i == idx);
      if (::IsWindow(m_arpView[i]->m_hWnd))
		    m_arpView[i]->RedrawWindow(0,0,RDW_INVALIDATE|RDW_FRAME);
	  }
    if (m_arpDialog[idx]->m_nType == 1)
    {
      m_ListPanel.SetRecord(((CSnapDlg*)(m_arpDialog[idx]))->GetCurRecord());
    }

  }
  catch (const char* cp)
  {
    ATLTRACE(cp);
  }

  LeaveCriticalSection(&m_csArrays);
  ATLTRACE("OnViewActivate: csArrays Leaved\n");
	//ATLTRACE("ViewAct %d\n",idx);
	return S_OK;
}


LRESULT CMainFrame::OnDlgActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{

	CDlgView* pView = (CDlgView*)lParam;

  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

  try {

    int idx = m_arpDialog.Find(pView);

    if (idx < 0)
      throw("dlgact^ idx < 0\n");

	  //ATLTRACE(" DlgAct %d\n",idx);

	  for (int i = 0; i < m_arpView.GetSize(); i++) {
		  if (m_arpView[i]->m_bActive)
			  m_arpView[idx]->PostMessage(WM_DIZ_SETACTIVE,0);
	  }

	  if (idx >= 0 && m_arpView.GetSize() > idx) {
		  m_arpView[idx]->PostMessage(WM_DIZ_SETACTIVE,1);
			//m_pCurrView = m_arpView[idx];
		  //m_arpView[idx]->SetActiveWindow();
		  //m_arpView[idx]->BringWindowToTop();
	  }

  }
  catch (const char *cp)
  {
    ATLTRACE(cp);
  }

  LeaveCriticalSection(&m_csArrays);
  return 1;
}


LRESULT CMainFrame::OnViewClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
  //050428 - секция для доступа к массивам видов и диалогов
  ATLTRACE("OnViewClose %x\n",GetCurrentThreadId());
  //EnterCriticalSection(&m_csArrays);
  //EnterCriticalSection(&m_csTimer);
  ATLTRACE("OnViewClose - sections entered\n");
  try 
  {


 	  CActDView* pView = (CActDView*)lParam;
    if (pView->GetType() == 1 && pView->GetNum() == 0 && m_File.IsFile())
    {
      if (IDOK == MessageBox("Close File?","",MB_OKCANCEL))
      {
        PostMessage(WM_COMMAND,ID_FILE_CLOSE,0);
      }
      //else
      throw((const char*)0);
    }

	  int idx = m_arpView.Find(pView);
    ATLTRACE("ViewCLOSE %d\n",idx);
	  if (idx < 0) 
    {
      throw((const char*)0 /*"View: not found"*/);
    }

    RemoveOsc(idx);
  }
  catch(const char *cp)
  {
    //LeaveCriticalSection(&m_csTimer);
    //LeaveCriticalSection(&m_csArrays);
    ATLTRACE("OnViewClose - sections leaved\n");
    if (cp)
    {
      ATLTRACE(cp);
      MessageBox(cp);
    }
    return S_OK;
  }

  //LeaveCriticalSection(&m_csTimer);
  //LeaveCriticalSection(&m_csArrays);
  ATLTRACE("OnViewClose - sections leaved\n");
  return S_OK;
}


LRESULT CMainFrame::OnWindowCascade(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
#define HOROFFS 40
#define VEROFFS 22

	int nWinNum(0);
	CRect rcClient;


	::GetClientRect(this->m_hRightUp,&rcClient);

  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

	for (int i = 0; i < m_arpView.GetSize(); i++) {
		if (!IsChild(m_arpView[i]->m_hWnd)) continue;
		nWinNum++;
	}
	if (nWinNum < 1) return 0;

	int nWid = rcClient.Width()-HOROFFS*(nWinNum-1);
	int nLeft = HOROFFS*(nWinNum-1);
	int nHeight = rcClient.Height()-VEROFFS*(nWinNum-1);

	CRect rcW(nLeft,0,nLeft+nWid,nHeight);

	for (i = 0; i < m_arpView.GetSize(); i++) {
		if (!IsChild(m_arpView[i]->m_hWnd)) continue;
		m_arpView[i]->MoveWindow(&rcW);
		rcW.OffsetRect(-HOROFFS,VEROFFS);
	}

  LeaveCriticalSection(&m_csArrays);
	return 0;
}

LRESULT CMainFrame::OnWindowTile(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
#define HOROFFS 40
#define VEROFFS 22

	int nWinNum(0),nHorDiv;
	CRect rcClient;
	double flNum;

  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

	::GetClientRect(this->m_hRightUp,&rcClient);
	for (int i = 0; i < m_arpView.GetSize(); i++) {
		if (!IsChild(m_arpView[i]->m_hWnd)) continue;
		nWinNum++;
	}
	if (nWinNum < 1) return 0;
	flNum = sqrt((double)nWinNum);
	nHorDiv = (int)(floor(flNum));
	int nVerDiv = nWinNum/nHorDiv;
	if (nWinNum%nHorDiv) nVerDiv++;

	int Width = rcClient.Width()/nHorDiv;
	int Height = rcClient.Height()/nVerDiv;
	CRect rcW(0,0,Width,Height);
	int H(0),V(0),cnt(0);
	for (i = 0; i < m_arpView.GetSize(); i++) {
		cnt++;
		if (!IsChild(m_arpView[i]->m_hWnd)) continue;
		m_arpView[i]->MoveWindow(&rcW);
		if (++V < nVerDiv)
			rcW.OffsetRect(0,Height);
		else {
			V = 0;
			H++;
			rcW.OffsetRect(Width,-rcW.top);
		}
	}
  LeaveCriticalSection(&m_csArrays);

	return 0;
}







LRESULT CMainFrame::OnMapChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{



  //050222 - если сообщение пришло не от главной карты, на до него дела нет...
  if (lParam == 0 || (HWND)lParam == m_hMainMap)
  {
    //050219 - вход в критическую секцию; для начала - проба с отладочной печатью
    if (!TryEnterCriticalSection(&m_csWChans))
    {
      ATLTRACE("\nOnMapChanged:: Chans busy... ");
      //PostMessage(uMsg,wParam,lParam);
      //return 0;
      EnterCriticalSection(&m_csWChans);
    }
    //ATLTRACE("OnMapChaned:...WChans Entered ----");

    if (m_aMode.GetMode() <= 0)
    {
      char *pMap = m_HardDlg.GetMap();
      for (UINT i = 0; i < m_HardDlg.GetMapSize(); i++)
      {
        if (*pMap)
          *pMap = -1;
        pMap++;
      }
      m_HardDlg.RedrawMap();
    }


    //050219 - здесь считаю число включенных каналов
    m_nOnChans = 0;
    ATLTRACE("m_arChanW.RemoveAll();\n");
    m_arChanW.RemoveAll();

	  //offset in the wokring buffer
	  DWORD dwOffs = 0;

    //050518 - Триггерный канал будет первым!!!
    //last channel at list will be status (trig)
    //get trigger frequency
    int nFrq = m_HardDlg.GetFrq(true);
    DWORD dwLen = nFrq * 60;
    int nRate(0);
    if (nFrq)
      nRate = nFrq/m_HardDlg.GetFrq();
    m_arChanW.Add(_ChanW(0,-1,dwOffs, dwLen, nFrq, "TRIG", nRate));
	  dwOffs += (dwLen+1);

    //частота (возможно - децимированная)
    nFrq = m_HardDlg.GetFrq();
	  //len (in long) for one channel (30 s)
	  dwLen = nFrq * 60;

	  //start from 2 (excl stat and synch)
	  for (int chan = 0; chan < m_nChans; chan++) 
	  {
		  if (m_pMainMap[chan])
		  {
			  m_nOnChans++;
        m_arChanW.Add(_ChanW(chan+2,chan,dwOffs, dwLen, nFrq, m_HardDlg.GetChanName(chan)));
        //050412 - добавляю одну точку
			  dwOffs += (dwLen+1);
		  }
	  }

	  //050426 - ремэпирование оффсетов екстканалов
	  for (chan = 0; chan < m_nExtChans; chan++) 
	  {
		  if (m_pMainMap[chan+256])
		  {
			  m_nOnChans++;
			  m_arChanW.Add(_ChanW(chan+2+m_nPinChans,chan+256,dwOffs, dwLen, nFrq, m_HardDlg.GetChanName(chan+256)));
        //050412 - добавляю одну точку
			  dwOffs += (dwLen+1);
		  }
	  }


	  //длина канального буфера
    // 050412 - добавляю одну точку
	  m_dwChanBufLen = dwOffs;

    //050610 - если файл еще не был инициализирован, нужно это сделать сейчас
    //050709 - пишу инициализацию только если нет записей!!!
    if (m_File.IsFile() == GENERIC_WRITE && m_pFileMap && m_File.GetRecNum() < 1)
      m_pFileMap = m_File.FileInit(&m_arChanW, &m_atrParams, &m_csWChans, m_dwMode);
    LeaveCriticalSection(&m_csWChans);
    //ATLTRACE("OnMapChanged:... WChans Leaved\n");
  }

  //050426 - отпарвляем сообщения видам
  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

  for (int i = 0; i < m_arpDialog.GetSize(); i++) 
	{
    //050601 - передаем только осциллам
    if (m_arpDialog[i]->m_nType == 1 && m_File.IsFile())
    {
      m_arpDialog[i]->SetNofChans(m_File.GetPinChans(), m_File.GetExtChans());
    }
	  m_arpDialog[i]->PostMessage(WM_DIZ_MAPCHANGED);
	}
  LeaveCriticalSection(&m_csArrays);

	return 0;

}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
CString str("_____");
int chan,i;
DWORD lPointer(0);
bool bRestart(false);
int nDisp(0);
long dwMode(-1);
//050222 - признак вхождеиня в крит. секцию
bool bCS(false);
bool bRun(m_bRun);
//тригответ
int nTAns;
//счетчики для режимов без децимации и с децимацией
int nT0Cnt, nTDCnt;


static DWORD dwOldTrig(0);
DWORD dwBatCMS(0);

//изменения в триггере
DWORD dwNewTrig;

//050525 - переменные для фиксации времени. Почему-то должны быть статическими
static SYSTEMTIME stmTime;
static FILETIME ftmTime;

  if (m_uTimerID == 0)
  {
    ATLTRACE("- - OnTimer after STOP\n");
    return 0;
  }

  //050430 - секция для таймера
  if (!TryEnterCriticalSection(&m_csTimer))
  {
    ATLTRACE("Timer busy\n ");
    Sleep(20);
    EnterCriticalSection(&m_csTimer);
  }

  
  try
  {
    //0402018 - проверка на отсутствие драйверов вообще...  
    if (OPEN_DRIVER_ASYNC == 0)
        throw("No drivers");
    //050219 - вход в критическую секцию; для начала - проба с отладочной печатью
    if (m_hAct2 == INVALID_HANDLE_VALUE) 
    {
		//open driver
			m_hAct2 = OPEN_DRIVER_ASYNC();
			if (m_hAct2 == INVALID_HANDLE_VALUE) 
				throw("Driver not opened!");

      //ATLTRACE("Timer thread %X\n", GetCurrentThreadId());
			m_ucUsb[0] = 0;
			USB_WRITE(m_hAct2,m_ucUsb);

			

			READ_MULTIPLE_SWEEPS(m_hAct2,m_pBuf,BUFSIZE*4);
			//stop???
			m_ucUsb[0] = 0xFF;
			USB_WRITE(m_hAct2,m_ucUsb);
			m_dwPointer = 0;
			m_i64Buf = m_i64Disp = 0;
      bRun = false;
      //m_bFirst = true;
		}

		//lPointer = 0;
		//READ_POINTER(m_hAct2,&lPointer);

		for (i = 0; i < 10; i++) {
			READ_POINTER(m_hAct2,&lPointer);
			if (lPointer/4 != m_dwPointer) break;
			Sleep(10);
		}

		lPointer /= 4;
		if (m_dwPointer == lPointer) 
    {
      dwMode = -1;
			throw("No cord or power");
    }
		ULONG lNew;
		if (lPointer >= m_dwPointer)
			lNew = lPointer - m_dwPointer;
		else {
			lNew = lPointer + BUFSIZE - m_dwPointer;
		}



  	m_i64Buf += lNew;


		DWORD dwDispBufPos = (DWORD)(m_i64Disp%(BUFSIZE));
		DWORD dwSynch(0);

		int a,b;
		_int64 One(2);
		long lTmp;

		b = (dwDispBufPos+1)&(BUFSIZE-1);
    dwMode = (m_pBuf[b] & 0xAE000000) >> 25; //in Mk1 was 2E, for Mk2 AE

    if (m_nActChans < 0)
    {
		  m_aMode= actMode(dwMode);
		  m_nActChans = m_aMode.GetBufChans();
    }

    if (m_nActChans)
      nDisp = (int)(m_i64Buf-m_i64Disp)/m_nActChans;
    else
      nDisp = 0;

    //050128 - если по-честному, то проверять нужно всё!!!
    // 050427 - так и сделано....
		for (i = 0; i < nDisp; i++)
		{
			a = (dwDispBufPos+i*m_nActChans)&(BUFSIZE-1);
			b = (dwDispBufPos+i*m_nActChans+1)&(BUFSIZE-1);
			dwSynch = m_pBuf[a];
      dwMode = (m_pBuf[b] & 0xFE000000) >> 25;  //FE means that we collect CMS and BAT signals
  		if (dwSynch != 0xFFFFFF00) 
      { 
        dwMode = -1;
        throw("Synch lost");
      }

      if ( (dwMode & 0x57) != m_dwMode)   //was 17 (for Mk1, new - 57 (for mk2))
        throw("Mode changed");
    }

    // 0000 0000 0000 0000 0000 0000
    // 23   19   15   11   7    3
    //
    //биты 20 (CMS) и 22 (low bat)
    dwBatCMS = (dwMode & 0x28) >> 3;
    //оставить биты 17,18,19,21


		m_dwPointer = lPointer;

    //ATLTRACE("%16I64d %12d %5d\n",m_i64Buf, lNew, nDisp);

		chan = 0;


    if (!TryEnterCriticalSection(&m_csWChans))
    {
      ATLTRACE("OnTimer: Chans busy... ");
      EnterCriticalSection(&m_csWChans);
      //ATLTRACE(".. refused\n");
      //return 0;
    }
    //ATLTRACE("TIMER csWChans entered\n");

    bCS = true;
    //ATLTRACE("OnTimer: Entered --- ");

		bool bNeedRedraw(false);

    //копии общих переменных для инд. обрабоки каналов
    int nFiltBufPos;
    bool bStart;
    long *pFiltBuf;
    int nDecCnt;
    //для фильтрации
    double flSum;
    //счетчик для фильтра
    int nF;
    //счетчик для фильтруемого сигнала
    int nS;

    //ATLTRACE("Timer received %d points",nDisp);
    //счетчик переданных точек (без децимации - равен полученному
    int nD(nDisp);


    //050207 - цикл только по включенным каналам
		// +1 означает статусный канал (в конце)
    bool bDec(m_nDecCf > 1);
    int nStoreDecCnt(-1);
    int nStoreFiltBufPos(-1);
    //m_arTrigPos.RemoveAll();

    nT0Cnt = nTDCnt = -1;

		for (int cn = 0; cn < m_arChanW.GetSize(); cn++)
		{
			chan = m_arChanW[cn].GetChanOff();
      m_arChanW[cn].StoreW();
      if (chan == 0) 
        chan = 1;
      //копирование общих переменных для инд. обработки в канале
      nFiltBufPos = m_nFiltBufPos;
      bStart = m_bStart;

      nDecCnt = -1;
      //decimation flag  

      if (bDec && (chan > 1 || m_bTrigDec))
        nDecCnt = m_nDecCnt;

      //буфер конкретного канала
      pFiltBuf = m_pFiltBuf + cn*m_nFiltLen;


      nD = 0;
			m_nOffsBuf[chan] = 0;

      //первая получка после запуска
      if (bRun && m_u64RunTime == 0)
      {
        GetLocalTime(&stmTime);
        SystemTimeToFileTime(&stmTime,&ftmTime);
        m_u64RunTime = (U64)ftmTime;
        unsigned hyper uDiff = nDisp*10000000/m_HardDlg.GetFrq(); 
        m_u64RunTime -= uDiff;
        m_File.SetRunTime(m_u64RunTime);
      }


			for (i = 0; i < nDisp; i++)
			{
				a = dwDispBufPos+i*m_nActChans+chan;

				b = a&(BUFSIZE-1);
				lTmp = m_pBuf[b];
				lTmp >>= 8;
				if (chan > 1)	//для "аналогвых каналов" - делим на 256
				{
					m_nOffsBuf[chan] += (lTmp >> 14);
				}
        //кладу полученное число в буфер канала...
        if (bRun && !m_bFirst)  
        {
          //050511 - обработка триггерного канала
          if (chan == 1)
          {
            lTmp &= 0xFFFF;
            dwNewTrig = (DWORD)(lTmp & m_dwValidBits);

            //обработка кнопок меню
            if (m_bNeedStart && m_bPause )
            {

              m_bPause = false;
              m_bNeedStart = true;
              //в буфер запихиваю только в гейтрежиме
              if (m_nAcqMode == 0)
              {
                lTmp |= 0xF2000000;
                m_pBuf[b] = lTmp;
              }
            }

            if (!m_bPause && m_bNeedPause)
            {
              m_bPause = true;
              m_bNeedPause = false;
              //в буфер запихиваю только в гейтрежиме
              if (m_nAcqMode == 0)
              {
                lTmp |= 0xF1000000;
                m_pBuf[b] = lTmp;
              }
            }

            if (m_nAcqMode == 1)
            {
              //тригрежим - обработка ручного старта
              if (m_bManStart && !m_bManRec)
              {
                m_bManRec = true;
                m_bManStart = false;
                lTmp |= 0xF0800000;
                m_pBuf[b] = lTmp;
              }

              if (m_bManStop && m_bManRec)
              {
                m_bManRec = false;
                m_bManStop = false;
                lTmp |= 0xF0400000;
                m_pBuf[b] = lTmp;
              }
            }

            if (dwNewTrig != m_dwOldTrig)
            {
              //вызываем ф-ию, определяющую что за триггер
              nTAns = TestTrig (dwNewTrig);

              //тригввод - тестируем маны
              if (m_nAcqMode == 1)
              {
                if (nTAns & MAN_ON)
                {
                  m_bManRec = true;
                  lTmp |= 0xF0800000;
                  m_pBuf[b] = lTmp;
                }
                if (nTAns & MAN_OFF)
                {
                  m_bManRec = false;
                  lTmp |= 0xF0400000;
                  m_pBuf[b] = lTmp;
                }
              }

              switch(nTAns & 7)
              {
              default:
              case 0:
                break;
              case GATE_OFF:
                m_bPause = true;
                if (m_nAcqMode == 0)
                {
                  lTmp |= 0xF1000000;
                  m_pBuf[b] = lTmp;
                }
                break;
              case (TRIG_ON|GATE_ON):
                  m_bPause = false;
                //здесь - без брыка!!!
              case TRIG_ON:
                if (!m_bPause && m_nAcqMode == 1)
                {
                  lTmp |= 0xF4000000;
                  m_pBuf[b] = lTmp;
                  m_bManRec = false;
                }
                break;
              case GATE_ON:
                if (m_bPause)
                {
                  m_bPause = false;
                  if (m_nAcqMode == 0)
                  {
                    lTmp |= 0xF2000000;
                    m_pBuf[b] = lTmp;
                  }
                }
                break;
              }
              //ATLTRACE("  %08X\n", lTmp);
            }
            //счетчик значений, полученных с момента запуска
            m_dwTickCnt++;
          }

          //вот здесь нужно (при необходимости) фильтровать-децимировать...
          if (bDec)
          {
            pFiltBuf[nFiltBufPos] = lTmp;

            //самое начало - заполняем буфер первым значением
            if (bStart)
            {
              //050519 - чтобы не дублировать команды....
              if (chan == 1)  lTmp &= 0xFFFF;

              for (int j = 0; j < m_nFiltLen; j++)
              {
                pFiltBuf[j] = lTmp;
              }
              bStart = false;
            } //end of bStart == true

            //сдвинутая точка - пригодится для тригканала
            int nSt = nFiltBufPos - m_nHalfLen;
            if (nSt < 0) nSt += m_nFiltLen;

             
            if (chan == 1 && !m_bTrigDec)
            {
              //тригканал без децимации
              //Trigger channel without decimation
              lTmp = pFiltBuf[nSt];
              m_arChanW[cn].Put(lTmp);
              nD++;
            }
            else if (++nDecCnt >= m_nDecCf) //нужна ли нам эта точка?
            {
              nDecCnt = 0;
              flSum = 0;
              nF = 0;
              if (chan == 1 ) 
              { 
                nTDCnt++;
                if (m_bTrigDec) 
                {
                  //децимация триггерного канала
                  // итог должен содержать ВСЕ случившиеся за выкидываемый период изменения
                  DWORD dwNew = (DWORD)lTmp; //старое значение
                  DWORD dwDiff;
                  DWORD dwDifSum(0);
                  DWORD dwTrigCommand(0), dwNewTrigCommand, dwManCommand;
                  int nScorr;
                  
                  //nSt -= m_nDecCf;

                  for (nS = nSt-m_nDecCf; nS < nSt; nS++)
                  {
                    nScorr = nS;
                    if (nScorr < 0) nScorr += m_nFiltLen;
                    dwDiff = dwOldTrig ^ pFiltBuf[nScorr];
                    dwDifSum |= dwDiff;
                    dwNewTrigCommand = pFiltBuf[nScorr] & 0xFFF00000;
                    if (dwNewTrigCommand)
                    {
                      dwManCommand =  dwTrigCommand & 0xF0F00000;
                      dwTrigCommand = dwTrigCommand & 0xFF000000;
                      //этот перебор делает следующее: при выклчюение гейта добавляется
                      // к уже существующей команде, включения же триггера и гейта
                      // замещают ее. Таким образом единственной "двойной" командой
                      // будет включение+выключение гейта
                      //  050524 - аналонгично с мануальными

                      switch(dwNewTrigCommand)
                      {
                      case 0xF1000000: //GATE_OFF
                        dwTrigCommand |= dwNewTrigCommand;
                        break;
                      case 0xF2000000: //GATE_ON
                      case 0xF4000000: //TRIG_ON
                        dwTrigCommand = dwNewTrigCommand;
                        break;
                      default:
                        break;
                      }

                      //отдельный свитч по манкомандам
                      switch(dwManCommand)
                      {
                      case 0xF0400000: //MAN_OFF
                        dwTrigCommand |= 0xF0400000;
                        break;
                      case 0xF0800000: //MAN_ON
                        //снимакм MAN_OFF
                        dwTrigCommand &= ~0x00400000;
                        //устанавливаем MAN_ON
                        dwTrigCommand |= 0xF0800000;
                        break;
                      default:
                        break;
                      }

                    }
                  }
                  dwNew = dwOldTrig ^ dwDifSum;
                  dwOldTrig = dwNew;
                  lTmp = (long)(dwNew | dwTrigCommand);
                  //if (dwTrigCommand) ATLTRACE("TrigCommand %08X\n\n",lTmp);
                }
                else
                {
                }
              }
              else if (chan > 1)
              {
                //фильтрация аналоговых каналов
                //вторая половина буфера (от самой старой точки до конца)
                for (nS = nFiltBufPos+1; nS < m_nFiltLen; nS++)
                {
                  //ATLTRACE("%3d %10.7f\n",nF, m_pfFilter[nF]);
                  flSum += (double)pFiltBuf[nS]*m_pfFilter[nF++];
                }
                //первая половина буфера (от начала до самой новой точки)
                for (nS = 0; nS <= nFiltBufPos; nS++)
                {
                  //ATLTRACE("%3d %10.7f\n",nF, m_pfFilter[nF]);
                  flSum += (double)pFiltBuf[nS]*m_pfFilter[nF++];
                }
                ATLASSERT(nF == m_nFiltLen);
                lTmp = (long)flSum;
              }
              //ATLTRACE("%2d %d %d %f\n",cn,pFiltBuf[nFiltBufPos], lTmp,flSum);
              m_arChanW[cn].Put(lTmp);
              nD++;
            }

            if (++nFiltBufPos >= m_nFiltLen) 
              nFiltBufPos = 0;

          } //конец условия bDec 
          else
          {
            //децимировать не нужно
            m_arChanW[cn].Put(lTmp);
            nD++;
          }

        } // конец условия bRun
			} //конец цикла по отсчетам

      if (nDecCnt >= 0)
      {
        nStoreDecCnt = nDecCnt;
        nStoreFiltBufPos = nFiltBufPos;
      }

      if (bRun && !m_bFirst) 
      {
        //050411 - синхронизация позиции
        m_arChanW[cn].ResetRW(nD);
      }

      //050426 - дабы не править триггерный канал!!!
      if (nDisp > 0 && chan >= 2)
      {
        //050224 - внесено в этот цикл (было в отдельном)
			  //определение смещений - пропускаем первые 2 канала
			  // (статусный канал и синхро)
        // не нужно (я и так во включенных) if (!m_pMainMap[chan-2]) continue;
			  //1 == 512 мкв 2 == 1.024 ; 100 = 51.2
			  m_nOffsBuf[chan] = abs(m_nOffsBuf[chan] / nDisp);
        m_nOffsBuf[chan] = _Settings.GetOffsIdx(m_nOffsBuf[chan]) + 1;


        if (chan < m_nPinChans+2)
        {
			    if (m_pMainMap[chan-2] != m_nOffsBuf[chan])
			    {
				    bNeedRedraw = true;
				    m_pMainMap[chan-2] = m_nOffsBuf[chan];
			    }
        } 
        else 
        {
			    if (m_pMainMap[chan-2-m_nPinChans+256] != m_nOffsBuf[chan])
			    {
				    bNeedRedraw = true;
				    m_pMainMap[chan-2-m_nPinChans+256] = m_nOffsBuf[chan];
			    }
        }
      }

		} //конец цикла по включенным каналам

    m_bStart = false;

    //теперь я должен сохранить получившиеся значения в глобальных переменных....
    if (m_nDecCf > 1)
    {
      //nDisp /= m_nDecCf;
      if (nStoreDecCnt >=  0)
      {
        m_nDecCnt = nStoreDecCnt;
        m_nFiltBufPos = nStoreFiltBufPos;
      }
    }


		if (bNeedRedraw)
		{
			m_HardDlg.RedrawMap();
      m_ListPanel.RedrawMap();

      //050428 - секция для доступа к массивам видов и диалогов
      EnterCriticalSection(&m_csArrays);

			for (int i = 0; i < m_arpDialog.GetSize(); i++) 
				m_arpDialog[i]->RedrawMap();

      LeaveCriticalSection(&m_csArrays);
		
		}


    //050402 - добавлена проверка nDisp > 0
		if (bRun && nDisp > 0) 
    {
      // что-то случилось в тригканале - нужно анализировать
      //менять статус кнопок буду один раз на проход (чаще вроде бы незачем)
      //посылаю сообщение себе же - будет обрабатываться в основном потоке
      ::PostThreadMessage(m_File.m_idThread, WM_DIZ_NEWFRAME,WPARAM(nD), LPARAM(&m_arChanW));

			//отправка сообщений всем возможным видам - с адресом буфера
      //050428 - секция для доступа к массивам видов и диалогов
      EnterCriticalSection(&m_csArrays);
      
      
      for (int i = 0; i < m_arpView.GetSize(); i++) 
			{
        if (::IsWindow(m_arpView[i]->m_hWnd) && m_arpView[i]->GetType() == 0)
        {
          //ATLTRACE("Timer sent %d points (%d/%d = %d)\n",nD,nDisp,m_nDecCf,nDisp/m_nDecCf);
				  m_arpView[i]->PostMessage(WM_DIZ_PLOT,WPARAM(nD), LPARAM(&m_arChanW));
        }
				//было до 040707
				//m_arpView[i]->PostMessage(WM_DIZ_PLOT,WPARAM(nDisp), LPARAM(m_pChanBuf));
			}
      LeaveCriticalSection(&m_csArrays);
		}
		//while (m_i64Disp >= BUFSIZE) m_i64Disp -= BUFSIZE;
		m_i64Disp += nDisp * m_nActChans;
    //ATLTRACE("T: %d (%d)\n", nDisp, m_bRun);
    if (!m_bReady)
    {
      m_bReady = true;
      OnIdle();
    }

	}
	catch(char* cp) {
		//BOOL bH;
    ATLTRACE("Exception %s\n",cp);
    //запоминаем, находились ли в активном состоянии
    //m_bReady = false;
    if (m_bRun)
    {
      // останавливаем
		  Stop();
      //Если были активны - сообщаем об ошибке
      MessageBox(cp);
    }
		bRestart = true;
	  str = cp;

	}
  if (bCS)
  {
    //050219 - leav section
    LeaveCriticalSection(&m_csWChans);
    //ATLTRACE("-WChans-- leaved OnTimer\n");
  }


	if (OPEN_DRIVER_ASYNC && (!m_bRun || bRestart)) 
  {
		m_ucUsb[0] = 0x0;
		USB_WRITE(m_hAct2,m_ucUsb);
    Sleep(5);
		CLOSE_DRIVER_ASYNC(m_hAct2);
		m_hAct2 = INVALID_HANDLE_VALUE;
    //m_bFirst = true;
	}


  dwMode = dwMode & 0x57;

  // test for Mk2
  //dwMode |= 0x40;

	if (m_dwMode != dwMode)
	{
		m_aMode = actMode(dwMode);
		m_nActChans = m_aMode.GetBufChans();
    m_nPinChans = m_aMode.GetPinChans();
    //m_nExtChans = m_aMode.GetTouch();

    m_HardDlg.SetMode(m_aMode.GetMode());
    m_StsBar.SetPower(m_aMode.GetMode());
		//UIEnable(ID_FILE_NEW, m_aMode.GetMode() >= 0,1);
		//OnIdle();
	}
	m_dwMode = dwMode;

  if (m_dwBatCMS != dwBatCMS)
  {
    m_HardDlg.SetBatCMS(dwBatCMS);
    m_StsBar.SetBatCMS((WORD)(dwBatCMS&0xFFFF));
    m_dwBatCMS = dwBatCMS;
  }

  //050430 - секция для таймера
  LeaveCriticalSection(&m_csTimer);
  //ATLTRACE("Timer section leaved - OnTimer\n");
  
  //OnIdle();
  return 0;
}

LRESULT CMainFrame::OnRunGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
int i;
CString str;

	if (m_aMode.GetMode() < 0 )
		return 0;

  ATLTRACE("MainFrame Thread %x\n", GetCurrentThreadId());

  //050519 - если хотим останвоиться, нужно сначала закончить запись в текущий фрейм
  if (m_bRun)
  {
    if (!m_bPause)
      m_bNeedPause = true;
    m_bNeedStart = false;
    //m_bPause = true;


    m_bManStart = false;
    if (m_bManRec)
      m_bManStop = true;
    //m_bManRec = false;


    if (m_File.IsWriting()) {
      ATLTRACE("w");
      Sleep(200);
      PostMessage(WM_COMMAND, MAKEWPARAM(ID_RUN_GO,0), (LPARAM)hWndCtl);
      return 0;
    }
  }
  else if (m_nOnChans == 0)
	{
		if ( IDOK != MessageBox("No active channels!!!\nContinue?","Warning",MB_OKCANCEL))
		  return 0;
	}


  //050430 - секция для таймера
  EnterCriticalSection(&m_csTimer);
  
  //050506 - загружаю параметры потребления и триггеров
  m_atrParams = _Settings.GetAcqAndTrigParams();
  

  //ATLTRACE("Timer section entered - OnRunGo\n");
  bool bRun = !m_bRun;
	//m_bRun = !m_bRun;
  //if (m_bRun)  m_bPause = true;
	OnIdle();

  //050512 - modeless propsheet
  if (m_pPropSheet)
  {
    HWND hW = m_pPropSheet->IndexToHwnd(1);
    ::EnableWindow(hW, !m_bRun);
  }


  m_dwOldTrig = (DWORD)-1;
	//KillTimer(1);


  ATLTRACE("Deleting buffers\n");
  SAFE_DELETE_ARRAY(m_pfFilter);
  SAFE_DELETE_ARRAY(m_pFiltBuf);


  //050709
  // Нужно сравнить режим записи (или только частоту???), записанную в файле 
  // (нет, на самом деле - режим) с актуально установленным - и отказаться от запуска,
  //  если они не совпадают.
  if (bRun && m_File.IsFile() == GENERIC_WRITE && m_File.GetRecNum() > 0 )
  {
    if (m_File.GetInputMode() != m_dwMode)
    {
      str.Format("Input mode changed from %d to %d.\nSwitch back to mode %d\nor close the file and open a new one.",
        m_File.GetInputMode(), m_dwMode, m_File.GetInputMode());
      MessageBox(str);
      bRun = false;
    }

  }



  if (!bRun)
	{
    Stop();
    LeaveCriticalSection(&m_csTimer);
		return 0;
	}

  //все, здесь пошел реальный запуск

  m_HardDlg.EnableWindow(0);

  //050517 блокировка повторных триггеров
  m_dwTrigBlock = m_atrParams.nIdle*m_aMode.GetFrq() / 1000;
  //050517 общий счетчик полученных значений
  m_dwTickCnt = 0;
  //050517 последнее полученное триггерное значение
  m_dwLastTrigCnt = 0;

  //------------этот кусок перенес из начала --------------------------
  BuildTrigEvents();

  //снимаю ручной ввод
  m_bManRec = false;
  m_bNeedPause = false;

  //для гейтввода - ставим на паузу
  if (m_nAcqMode != 1)
    m_bPause = true;
  else
    m_bPause = false;
  //а для тригввода - снимаем 


  //передача главной карты файлу
	m_bNotActivate = true;

  if ( !(m_File.IsFile() == GENERIC_WRITE && m_File.GetRecNum() > 0))
    m_pFileMap = m_File.FileInit(&m_arChanW, &m_atrParams, &m_csWChans, m_dwMode);
  //m_pFileMap = m_File.FillMap(m_nOnChans, m_nChans,m_nExtChans,m_pMainMap,m_LeftDlg.GetChanNames());
  if (m_nSnapCnt < 1 && (m_File.IsFile() == GENERIC_WRITE || m_nAcqMode == 1))
  {
    OnSnapNew(0,0,m_hWnd,bHandled);
  }
  //передача карты снэпшотам
  for (i = 0; i < m_arpDialog.GetSize(); i++)
  {
    if (m_arpDialog[i]->m_nType == 1)
    {
			m_arpDialog[i]->SetNofChans(m_nChans,m_nExtChans);
		  m_arpDialog[i]->PostMessage(WM_DIZ_MAPCHANGED);
      m_arpView[i]->PostMessage(WM_DIZ_SNAPINIT, 0, 0);
    }
  }
	m_bNotActivate = false;

  //принудительно создание первого осцилла
  if (m_nOscCnt < 1)
  {
    OnOscNew(0,0,m_hWnd,bHandled);
  }


  //-----------------------------------------------------------------------------


  EnterCriticalSection(&m_csWChans);
  ATLTRACE("GO csWChans entered\n");

	SAFE_DELETE_ARRAY(m_pChanBuf);
	if (m_dwChanBufLen)
	  m_pChanBuf = new long[m_dwChanBufLen];	//reserve memory for one second

	//ошибка выделения памяти
	if (!m_pChanBuf)
	{
		MessageBox("No memory for channel buffer");
		m_arChanW.RemoveAll();
    LeaveCriticalSection(&m_csWChans);
    LeaveCriticalSection(&m_csTimer);
		return 0;
	}

	//										итоговая частота    30 с   каналы плюс статус
  ZeroMemory(m_pChanBuf, m_dwChanBufLen * sizeof(long));

  ATLTRACE("m_arChanW.SetBuff;\n");
  //Каналов должно быть на 1 больше!!!
  for (i = 0; i < m_arChanW.GetSize(); i++)
		m_arChanW[i].SetBuff(m_pChanBuf);

  LeaveCriticalSection(&m_csWChans);
  ATLTRACE("GO csWChans Leaved\n");




  //050228 - вношу сюда формирование децимирующего фильтра (из CHardDlg)
  if ( (m_nDecCf = m_HardDlg.GetDecimate()) > 1)
	{
    SAFE_DELETE_ARRAY(m_pfFilter);
    SAFE_DELETE_ARRAY(m_pFiltBuf);
    //чтобы первая же точка попала....
    m_nDecCnt = m_nDecCf-1;
    double PI(3.1415926535897932384626433832795);
		m_nFiltLen = 32*m_nDecCf+1;
		m_pfFilter = new double[m_nFiltLen];
		double x(0), sum(0), fl(0);

    // формирование промежуточного буфера
    // c одним лишним каналом - для тригканала
    m_pFiltBuf = new long[m_nFiltLen*(m_nOnChans+1)];

    //ставлю позиционер в начало буфера
    m_nFiltBufPos = 0;
    
    //собственно расчет фильтра
		//частоту среза устанавливаю на 0.75 от будущего Найквиста
		double FiltFrq = 0.75/m_nDecCf;
		double *pFilter = new double[m_nFiltLen+1];
		m_nHalfLen = m_nFiltLen/2;
		CDataWin NutWin(m_nFiltLen,"Nuttall");

		for (i = 0; i < m_nHalfLen; i++)
		{
			x = (double)(m_nFiltLen/2-i) * PI * FiltFrq;
			//0.54 - 0.46*cos() - Hamming
			//fl = sin(x)/x * (0.54-0.46*cos(PI*(double)i/nHalfLen));
			fl = sin(x)/x * NutWin[i];
			m_pfFilter[i] = fl;
			sum += fl;
    }
    sum = sum*2+1;
    m_pfFilter[m_nHalfLen] = 1/sum;
          
    for (i = 0; i < m_nHalfLen; i++) {
      m_pfFilter[i] = m_pfFilter[m_nFiltLen-1-i] = m_pfFilter[i]/sum;
		}

    m_bStart = true;
    delete []pFilter;

	}

  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

	for (i = 0; i < m_arpView.GetSize(); i++) {
		//А вот здесь я должен передавать ИТОГОВУЮ частоту!!!
		m_arpView[i]->PostMessage(WM_DIZ_SETFRQ,m_HardDlg.GetFrq(),0	);
	}

  LeaveCriticalSection(&m_csArrays);
	
	//SetTimer(1,m_HardDlg.GetTimer(),0);

  m_bTrigDec = (m_HardDlg.GetDecimate(true) > 1);
	SetTimer(_Settings.GetTimer());

  //Sleep(300);
  m_bRun = bRun;
  //050430 - секция для таймера
  LeaveCriticalSection(&m_csTimer);
  //ATLTRACE("Timer section leaved - OnRunGo\n");

  //check for screen saver
  SystemParametersInfo( SPI_GETSCREENSAVEACTIVE, 0, &m_bSSave, 0); 
  //check for LowPower
  SystemParametersInfo( SPI_GETLOWPOWERACTIVE, 0, &m_bLowPow, 0); 
  //check for PowerOff
  SystemParametersInfo( SPI_GETPOWEROFFACTIVE, 0, &m_bPowOff, 0); 

  //disable screen saver
  SystemParametersInfo( SPI_SETSCREENSAVEACTIVE,
                        FALSE, 0, SPIF_SENDWININICHANGE); 

  //disable monitor low-power
  SystemParametersInfo( SPI_SETLOWPOWERACTIVE,
                        FALSE, 0, SPIF_SENDWININICHANGE); 

  //disable monitor power-off
  SystemParametersInfo( SPI_SETPOWEROFFACTIVE,
                        FALSE, 0, SPIF_SENDWININICHANGE); 
  //параметры скринсейвера и проч. запомнены.
  m_bSParams = true;

  m_HardDlg.EnableWindow(0);

  m_u64RunTime = 0;

	return 0;
}

LRESULT CMainFrame::OnRunTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{



  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);

	for (int i = 0; i < m_arpView.GetSize(); i++) {
		//ATLTRACE(" %x ",m_arpView[i]->m_hWnd);
		m_arpView[i]->Test();
		::PostMessage(m_arpView[i]->m_hWnd,WM_DIZ_TEST,0,0);
	}
  LeaveCriticalSection(&m_csArrays);

	return 0;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
  if (m_bRun)
  {
    MessageBox("Can't close running program. \n  Stop First!!! ");
    return 0;
  }



  if (m_File.IsFile())
  {
    if (m_File.IsFile() == GENERIC_WRITE)
    {
      if (IDYES != MessageBox("Close file?","New file exists",MB_YESNO))
      {
        return E_FAIL;
      }
    }
    m_File.Close();
  }

  if (m_arpView.GetSize() > 0)
  {
    RemoveAllViews();
    Sleep(100);
    PostMessage(WM_CLOSE);
    return S_OK;
  }


  //050430 - секция для таймера
  EnterCriticalSection(&m_csTimer);

	if (m_uTimerID)
	{
		timeKillEvent(m_uTimerID);
		m_uTimerID = 0;
	}

	Sleep(50);

  ATLTRACE("CMainFrame::OnClose\n");


  ::PostThreadMessage(m_File.m_idThread, WM_QUIT,15,0);


	_Settings.SaveColors();
  if (_Settings.GetProgSave())
  {
    _Settings.WriteStruct("Main","MapPresent",m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
    _Settings.WriteAcqAndTrigParams();
    if (m_HardDlg.GetChanNames())
    {
      _Settings.SaveChanNames(m_HardDlg.GetChanNames());
    }
    WINDOWPLACEMENT wpl;
    wpl.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(&wpl);
    _Settings.WriteStruct("Main","WindowPlacement",&wpl,sizeof(WINDOWPLACEMENT));
  }

	bHandled = false;

  //050430 - секция для таймера
  LeaveCriticalSection(&m_csTimer);

  return 0;
}


LRESULT CMainFrame::OnToolsSettins(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (!m_pPropSheet) {
    m_pPropSheet = new CSettSheet(_T("ActiveD Options"), 0 );
  	m_pPropSheet->Create(m_hWnd);
    m_pPropSheet->SetFile(&m_File);
  }
  else
  {
    m_pPropSheet->SetFile(&m_File);
    m_pPropSheet->Show();
  }
  EnableWindow(false);

  HWND hW = m_pPropSheet->IndexToHwnd(1);
  ::EnableWindow(hW, !m_bRun);


  return 0;
}


void CALLBACK TimeProc(
  UINT uID,      
  UINT uMsg,     
  DWORD dwUser,  
  DWORD dw1,     
  DWORD dw2      
)
{
  CMainFrame* pMain = (CMainFrame*)dwUser;
  BOOL bH;
	//ATLTRACE("MainTimer Thread %x\n", GetCurrentThreadId());

  pMain->OnTimer(0,(WPARAM)dwUser,(LPARAM)dw1,bH);
  return;
}


void CMainFrame::SetTimer(UINT uTime)
{

  //050430 - секция для таймера
  EnterCriticalSection(&m_csTimer);
  ATLTRACE("SetTimer - csTimer Entered\n");

	if (m_uTimerID)
	{
		timeKillEvent(m_uTimerID);
		m_uTimerID = 0;
	}
	m_uTimerID = timeSetEvent(uTime,20,&TimeProc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);

  //050430 - секция для таймера
  LeaveCriticalSection(&m_csTimer);
  ATLTRACE("SetTimer - csTimer Leaved\n");

}

//050407
void CMainFrame::Stop()
{
  //050430 - секция для таймера
  //EnterCriticalSection(&m_csTimer);

  ATLTRACE("Stop() %x\n",GetCurrentThreadId());
		//SetTimer(1,250,0);
    m_bRun = 0;

    //050709 - не разрешаю изменения, если в записываемом файле уже есть записи
    if ( !(m_File.IsFile() == GENERIC_WRITE && m_File.GetRecNum() > 0))
		  m_HardDlg.EnableWindow(1);

    //050428 - секция для доступа к массивам видов и диалогов
    if (!TryEnterCriticalSection(&m_csArrays))
    { 
      ATLTRACE("Can't Enter tp csArrays!\n");
      for (int k = 0; k < 100; k++)
      {
        Sleep(10);
        if ( TryEnterCriticalSection(&m_csArrays))
        {
          ATLTRACE("Entered to csArrays after %d ms!\n", k*10);
          break;
        }
      }
      ATLTRACE("Can't Enter tp csArrays!\n");
    }
    for (int i = 0; i < m_arpView.GetSize(); i++) 
		{
      ATLTRACE("СТОП! sent 0 %x\n",WM_DIZ_PLOT);
			m_arpView[i]->PostMessage(WM_DIZ_PLOT,0, 0);
		}
    LeaveCriticalSection(&m_csArrays);

    //050430 - секция для таймера
  //LeaveCriticalSection(&m_csTimer);

  //051212 - восстановление параметров скринсейвера и проч
  if (m_bSParams)
  {
    m_bSParams = false;
    //Restore screen saver
    SystemParametersInfo( SPI_SETSCREENSAVEACTIVE,
                         m_bSSave,0,SPIF_SENDWININICHANGE); 
    //Restore monitor low-power
    SystemParametersInfo( SPI_SETLOWPOWERACTIVE,
                         m_bLowPow, 0, SPIF_SENDWININICHANGE); 
    //Restore monitor power-off
    SystemParametersInfo( SPI_SETPOWEROFFACTIVE,
                         m_bPowOff, 0, SPIF_SENDWININICHANGE); 

  }

}

void CMainFrame::RemoveOsc(int idx)
{
  //050428 - секция для доступа к массивам видов и диалогов
  //ATLTRACE("RemOsc - csTimer trying\n");
  //EnterCriticalSection(&m_csTimer);
  //ATLTRACE("RemOsc - csTimer Entered\n");

  int nType = m_arpDialog[idx]->m_nType;
  int nNum = m_arpDialog[idx]->GetNumber();
  if (nType == 0 && m_nOscCnt <= 1 && m_bRun)
  {
    throw("Impossible to close last view while in running state.");   
    return;
  }


  ATLTRACE("RemoveOsc - %X\n",GetCurrentThreadId());
	m_arpView.RemoveAt(idx);
  m_tabbedChildWindow.RemoveTab(m_arpDialog[idx]->m_hWnd);
  ATLTRACE("RemoveOsc - remove tab %d\n",idx);
  delete m_arpDialog[idx];
  ATLTRACE("RemoveOsc - delete %d\n",idx);
	m_arpDialog.RemoveAt(idx);
  ATLTRACE("RemoveOsc %d\n",idx);

  if (nType == 0) 
  {
    m_nOscCnt--;
    if (nNum == m_nOscNum-1)
    {
      //удалили последний по списку
      m_nOscNum--;
    }
    if (m_nOscCnt == 0)
      m_nOscNum = 0;
  }
  else if (nType == 1)
  {
    m_nSnapCnt--;
    if (nNum == m_nSnapNum-1)
    {
      m_nSnapNum--;
    }
    if (m_nSnapCnt == 0)
      m_nSnapNum = 0;
  }
  


  if (m_arpDialog.GetSize() < 1) {
    m_pCurrView = 0;
		//m_tabbedChildWindow.DestroyTabWindow();
		m_hSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hRightE);
		::ShowWindow(m_hRightE,SW_SHOW);
	}
  else 
  {
    if (idx >= m_arpView.GetSize())
      idx = m_arpView.GetSize() - 1;
    m_pCurrView = m_arpView[idx];
  }

  //LeaveCriticalSection(&m_csTimer);
  //ATLTRACE("RemOsc - csTimer leaved\n");



}

LRESULT CMainFrame::NotImplemented(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  MessageBox("Command Not Implemented");

  return 1;
}

//050505 - задочить и каскадировать все окна
LRESULT CMainFrame::OnWindowDockAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{

	for (int i = 0; i < m_arpView.GetSize(); i++) {
		if (!m_arpView[i]->IsChild()) 
    {
      m_arpView[i]->DockWindow();
    }
	}
  OnWindowCascade(0,0,0,bHandled);
  return 1;
}

LRESULT CMainFrame::OnToolsLoadelectrodenames(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CFileDialog dlg(TRUE, 0,0,0,"Biosemi *.cfg files\0*.cfg\0All Files (*.*)\0*.*");
  if (dlg.DoModal() == IDOK)
  {
    CString strFile = dlg.m_ofn.lpstrFile;
    CString str;
    char *pNames = m_HardDlg.GetNewChanNames();
    for(int i = 0; i < 256; i++)
    {
      str.Format("Chan%d",i+1);
      GetPrivateProfileString("Labels",str,"",pNames+i*8,7,strFile);
    }

    for(i = 0; i < 8; i++)
    {
      str.Format("Tou%d",i+1);
      GetPrivateProfileString("Labels",str,"",pNames+(i+256)*8,7,strFile);
    }
  }
  return 0;
}

//050513
//функция строит маски, по которым будут отслеживаться изменения триггера
void CMainFrame::BuildTrigEvents()
{
  AcqTrParams acq = _Settings.GetAcqAndTrigParams();
  m_dwStTrigHiLo = acq.wTrHiLo;
  m_dwStTrigLoHi = acq.wTrLoHi;
  //m_dwEndTrigHiLo = acq.wETrHiLo;
  //m_dwEndTrigLoHi = acq.wETrLoHi;

  BOOL bAnd[3];
  for (int i = 0; i < 3; i++)
  {
    bAnd[i] = acq.bAnd[i];
  }

  m_nAcqMode = acq.nAcqMode;
  m_dwValidBits = 0;
  //Gate = no trig params
  switch (m_nAcqMode)
  {
  case 1: //Triggered input
    m_bStTrig = (m_dwStTrigHiLo || m_dwStTrigLoHi);
    m_dwValidBits = (m_dwStTrigHiLo | m_dwStTrigLoHi); 
    //последний гейт всегда разрешен
    bAnd[2] = true;
    break;
  default:
  case 0: //Gate - no triggers
    m_bStTrig = false;
    bAnd[0] = true;
    break;
  }


  //перебираем гейт-условия: у них булевских флагов нет....
  BOOL bAndSum = true;
  for (i = 0; i < 3; i++)
  {
    bAndSum = bAndSum && bAnd[i];

    //разрешение последнего гейта в тригрежиме
    if (m_nAcqMode == 1 && i == 2)
      bAndSum = true;

    if (bAndSum)
    {
      m_dwGateHi[i] = acq.wHi[i];
      m_dwGateLo[i] = acq.wLo[i];
    }
    else
    {
      m_dwGateHi[i] = m_dwGateLo[i] = 0;
    }
    m_dwValidBits |= (m_dwGateHi[i] | m_dwGateLo[i]);

    //если не стоит соотв. And - или слово равно нулю, ставим сверхстарший байт
    if (i < 3-m_nAcqMode && m_dwGateHi[i] == 0 && m_dwGateLo[i] == 0)
      m_dwGateHi[i] = 0xF0000;

  }



}

//050513
//функция будет возвращать целочисленный код того, что нужно будет сделать:
// 0 - оставить как есть
// 1 - начать ввод
// -1 - остановиться
// 2 - начать ввод по триггеру ???
WORD CMainFrame::TestTrig(DWORD dwNewTrig)
{

//DWORD dwNewTrig;
//инвертированный триггер - для упрощения проверок
DWORD dwInvNewTrig;
DWORD dwTrigChanged;
DWORD dwTrigLoHi;
DWORD dwTrigHiLo;

  //Триггер изменился!!! Нужно анализировать
  //wNewTrig = (WORD)dwNewTrig;
  //инвертирую для упрощения проверок
  dwInvNewTrig = ~dwNewTrig;
  //изменившиеся биты
  dwTrigChanged = (WORD)( (dwNewTrig ^ m_dwOldTrig) & 0xFFFF);
  //вставшие биты - те из изменившихся, которые стоят в новом
  dwTrigLoHi = dwTrigChanged & dwNewTrig;
  //упавшие биты - те из изменившихся, которых в новом нет
  dwTrigHiLo = dwTrigChanged & dwInvNewTrig;

  //ATLTRACE("old %05X   new %05X   set %04X   cleared %04X\n",  m_dwOldTrig, dwNewTrig, dwTrigLoHi, dwTrigHiLo);

  //запоминаем как "старое значение"
  m_dwOldTrig = dwNewTrig;

  //добавляю к гейтовым значениям старшее слово - для облегчения проверок
  dwNewTrig    |= 0xF0000;
  dwInvNewTrig |= 0xF0000;

  //собственно проверки - 3*2 гейта, стоптриг и старттриг
  // проверяем начиная снизу
  WORD wGate(0);
  WORD wAns(0);
  WORD wMan(0);
  
  //в гейтрежиме проверяем все гейты, в триг - только первые два
  //int nLastGate(m_nAcqMode);

  for (int i = m_nAcqMode; i < 3; i++)
  {
    if ( !( (dwNewTrig & m_dwGateHi[2-i]) || (dwInvNewTrig & m_dwGateLo[2-i]) ) )
    {
      //ATLTRACE("GATE End\n");
      wGate =  GATE_OFF;
      break;
    }

    //050516 - проверка на то, что гейт не просто стоит - он установился!!!
    if (( dwTrigLoHi & m_dwGateHi[2-i]) || (dwTrigHiLo & m_dwGateLo[2-i]))
      wGate = GATE_ON;

  }

  ATLTRACE("GATE %s\n",(wGate == GATE_ON) ? "ON":"OFF");\
  
  //Гейты пройдены
  // если триггера не нужны - возвращаю сообщение о соблюдении гейта
  if (!m_bStTrig) 
  {
    ATLTRACE("  NoTrig - return %x\n",wGate);
    return wGate;
  }


  //Здесь мы уже исключительно в тригрежиме
  dwTrigHiLo   |= 0xF0000;
  dwTrigLoHi   |= 0xF0000;

  //проверка мануал старт/стопа
  //доп проверка - сигнализируем только о включении!
  if (  (dwTrigLoHi & m_dwGateHi[2]) || (dwTrigHiLo & m_dwGateLo[2])  )
  {
    ATLTRACE("MAN Start\n");
    wMan =  MAN_ON;
  }
  else if (  (dwTrigLoHi & m_dwGateLo[2]) || (dwTrigHiLo & m_dwGateHi[2])  )
  {
    ATLTRACE("MAN end\n");
    wMan =  MAN_OFF;
  }

  if ( (dwTrigLoHi & m_dwStTrigLoHi) || (dwTrigHiLo & m_dwStTrigHiLo) )
  {
    //Это - собственно ТТРРРИИИГГГЕРРР
    if (m_dwTickCnt - m_dwLastTrigCnt < m_dwTrigBlock)
    {
      wAns = 0;
      ATLTRACE("ignoring repeated trig   ");
    }
    else
    {
      m_dwLastTrigCnt = m_dwTickCnt;
      wAns = TRIG_ON;
      ATLTRACE("TRIG_ON\n");
    }

  }

    return (wAns | wGate | wMan);

}


//050516 - обработка кнопки "пауза"
LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (m_nAcqMode == 0 && !m_bPause)
  {
    //GATE recording
    m_bNeedPause = true;
    m_bNeedStart = false;
  }
  else if (m_nAcqMode == 1)
  {
    if (m_bPause)
    {
      m_bNeedPause = false;
      m_bNeedStart = true;
    }
    else
    {
      m_bNeedPause = true;
      m_bNeedStart = false;
    }
  }
  OnIdle();
  return 1;
}


LRESULT CMainFrame::OnWrDisable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_bNoRec = !m_bNoRec;
  OnIdle();
  return S_OK;
}

LRESULT CMainFrame::OnWrManual(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (m_nAcqMode == 0 && m_bPause)
  {
    //GATE recording
    m_bNeedPause = false;
    m_bNeedStart = true;
  }
  else if (m_nAcqMode == 1)
  {
    //Triggered recording
    if (m_bManRec)
    {
      m_bManStart = false;
      m_bManStop = true;
    }
    else
    {
      m_bManStart = true;
      m_bManStop = false;
    }
  }
  OnIdle();
  return S_OK;
}

//050530 - SnapShot window
LRESULT CMainFrame::OnSnapNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	//if (!m_view)



	CString str;
	m_hSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hRightD) ;
	::ShowWindow(m_hRightE,SW_HIDE);

  str.Format("Snap %d",m_nSnapNum+1);

	CRect rc;
	::GetClientRect(m_hRightUp,&rc);




	CSnapDlg *pNewDlg = new CSnapDlg(this, m_nSnapNum);
	pNewDlg->Create(m_tabbedChildWindow);
	//m_tabbedChildWindow.AddTab(*pNewDlg, str);
  m_tabbedChildWindow.AddTabWithIcon(pNewDlg->m_hWnd,str,IDI_SNAP);

  if (m_File.IsFile() && m_pFileMap)
	{
    pNewDlg->SetMainMap(m_pFileMap, m_File.GetChanNames());
		pNewDlg->SetNofChans(m_File.GetPinChans(),m_File.GetExtChans()); //(m_nChans,m_nExtChans);//
		pNewDlg->SetNofRecords(m_File.GetRecNum(),1);
	}
  else
  {
    pNewDlg->SetMainMap(m_pMainMap, m_HardDlg.GetChanNames());
		pNewDlg->SetNofChans(m_nChans,m_nExtChans);//
  }
  pNewDlg->SetOffMap();
	
	m_tabbedChildWindow.DisplayTab(pNewDlg->m_hWnd);

	CActDView *pNewView = pNewDlg->GetView();
	m_pCurrView = pNewView;

	m_nSnapCnt++; m_nSnapNum++;


  //050428 - секция для доступа к массивам видов и диалогов
  EnterCriticalSection(&m_csArrays);
  m_arpDialog.Add(pNewDlg);
	m_arpView.Add(pNewView);
  LeaveCriticalSection(&m_csArrays);

  if (_Settings.GetWinLoad() >= 0)
	  pNewDlg->LoadSettings(true);

  //Sleep(20);
  pNewView->SendMessage(WM_DIZ_SNAPINIT, (WPARAM)(&m_File), (LPARAM)(&m_atrParams));
  pNewDlg->GoToRec(m_nCurRec);
  //PostMessage(WM_DIZ_VIEW_ACTIVATE,0,(LPARAM)pNewView);

	return 0;
}

//050624 - удаляем все виды 
void CMainFrame::RemoveAllViews()
{

	m_bNotActivate = true;
  ATLTRACE("OnFileClose:: trying m_csArrays\n");
  EnterCriticalSection(&m_csArrays);
  ATLTRACE("OnFileClose:: m_csArrays ENTERED\n");
  m_pCurrView = 0;
  int N = m_arpDialog.GetSize();
  for (int idx = 0; idx < N-1; idx++) 
  { 
    PostMessage(WM_DIZ_VIEW_CLOSE,0,(LPARAM)(m_arpView[idx]));
/*
    m_arpView[N-idx-1]->PostMessage(WM_SHOWWINDOW,0,0);
    m_tabbedChildWindow.RemoveTab(m_arpDialog[N-idx-1]->m_hWnd);
    ATLTRACE("__deleting dlg %d\n",N-idx-1);
    delete m_arpDialog[N-idx-1];
    ATLTRACE("____deleted dlg %d\n",N-idx-1);
    Sleep(300);
    */
  }
/*
  m_arpDialog.RemoveAll();
  m_arpView.RemoveAll();
  m_nOscNum = m_nOscCnt = m_nSnapNum = m_nSnapCnt = 0;
	m_hSplit.SetSplitterPane(SPLIT_PANE_RIGHT,m_hRightE);
	::ShowWindow(m_hRightE,SW_SHOW);
*/
  Sleep(300);
  if (N > 0)
    //OnViewClose(WM_DIZ_VIEW_CLOSE,0,(LPARAM)(m_arpView[idx]),bH);
    PostMessage(WM_DIZ_VIEW_CLOSE,0,(LPARAM)(m_arpView[idx]));

  LeaveCriticalSection(&m_csArrays);
	m_bNotActivate = false;
}


//050624 - при открытом на чтение файле частоту берем из него!!!
//050415 - возвращаем финальную (децимированную) частоту
int CMainFrame::GetFrq() const
{
  if (m_File.IsFile() == GENERIC_READ)
    return m_File.GetFrq();
  else
    return m_HardDlg.GetFrq(); 
}


//050701
LRESULT CMainFrame::OnReadFrame(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  int nFrame = (int)wParam;

  if (!m_File.IsFile() || nFrame < 0 || nFrame > m_File.GetRecNum())
    return E_FAIL;

  m_ListPanel.SetRecord(nFrame);

  EnterCriticalSection(&m_csArrays);
  if (m_pCurrView && m_pCurrView->GetType() == 1)
  {
    //текущий вид - снэпшот
    int idx = m_arpView.Find(m_pCurrView);
    if (idx >= 0 && idx < m_arpDialog.GetSize() && m_arpDialog[idx]->IsUnLocked() )
    {
      //если текущий снэпшот анлочен - сообщение только ему
      m_arpDialog[idx]->GoToRec(nFrame);

			LeaveCriticalSection(&m_csArrays);
		  return S_OK;
    }
  }

  m_nCurRec = nFrame;
  for (int dlg = 0; dlg < m_arpDialog.GetSize(); dlg++)
  {
    if (m_arpDialog[dlg]->IsLocked())
      m_arpDialog[dlg]->GoToRec(nFrame);
  }
  LeaveCriticalSection(&m_csArrays);

  return S_OK;
}

//050726
LRESULT CMainFrame::OnMatExp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  return m_File.MatExport();
}


//050801
LRESULT CMainFrame::OnConfOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CFileDialog dlg(TRUE,"*.cfg",0,0,"Configuration files (*.cfg)\0*.cfg\0All Files (*.*)\0*.*");

  CString strTitle = "Open configuration. (Current ";
  strTitle += _Settings.GetConfFileTitle();
  strTitle += ")";
  dlg.m_ofn.lpstrTitle = strTitle;

  dlg.m_ofn.Flags |= OFN_CREATEPROMPT;
  dlg.m_ofn.lpstrInitialDir = _Settings.GetConfFileName();

  if (dlg.DoModal() == IDOK)
  {
    //060210 - делаю в крит. секции
    EnterCriticalSection(&m_csTimer);

    CLOSE_DRIVER_ASYNC(m_hAct2);
		m_hAct2 = INVALID_HANDLE_VALUE;
    m_dwMode = 0xFFFFFFFF;

    _Settings.SetConfFile(dlg.m_ofn.lpstrFile,dlg.m_ofn.lpstrFileTitle);
  	_Settings.LoadColors();
    if (_Settings.GetProgLoad())
    {
      try
      {
        _Settings.GetStruct("Main","MapPresent",m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
      }
      catch(...)
      {
        ZeroMemory(m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
      }
      _Settings.LoadAcqAndTrigParams();
      
      char *pMap = m_HardDlg.GetMap();
      for (UINT i = 0; i < m_HardDlg.GetMapSize(); i++)
      {
        if (*pMap)
          *pMap = -1;
        pMap++;
      }

      char *pNames = m_HardDlg.GetNewChanNames();
      if (_Settings.IsChanNames())
      {
        _Settings.LoadChanNames(pNames);
      }
      WINDOWPLACEMENT wpl;
      wpl.length = sizeof(WINDOWPLACEMENT);
      try
      {
        _Settings.GetStruct("Main","WindowPlacement",&wpl,sizeof(WINDOWPLACEMENT));
        SetWindowPlacement(&wpl);
      }
      catch(...)
      {
        ShowWindow(SW_RESTORE);
      }
    }

    m_HardDlg.SendMessage(WM_INITDIALOG);
    m_HardDlg.SetMode(m_aMode.GetMode());
    if (m_pPropSheet)
    {
      m_pPropSheet->DestroyWindow();
      delete m_pPropSheet;
      m_pPropSheet = 0;
    }

    m_nActChans = -1;

    LeaveCriticalSection(&m_csTimer);
  }
  CString str;
  str.Format("ActiveD (%s)", _Settings.GetConfFileTitle());
  SetWindowText(str);

  
  return S_OK;
}

LRESULT CMainFrame::OnConfSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CFileDialog dlg(FALSE,"*.cfg",0,0,"Configuration files (*.cfg)\0*.cfg\0All Files (*.*)\0*.*");

  CString strTitle = "Save configuration. (Current ";
  strTitle += _Settings.GetConfFileTitle();
  strTitle += ")";
  dlg.m_ofn.lpstrTitle = strTitle;

  dlg.m_ofn.Flags  |= OFN_OVERWRITEPROMPT;
  dlg.m_ofn.lpstrInitialDir = _Settings.GetConfFileName();

  CString strNew;

  if (dlg.DoModal() == IDOK)
  {
    strNew = dlg.m_ofn.lpstrFile;
    if (strNew == _Settings.GetConfFileName())
    {
      MessageBox("Can't rewrite file onto itself","Error");
      return E_FAIL;
    }

    if (CopyFile(_Settings.GetConfFileName(), strNew, false))
    {
      _Settings.SetConfFile(strNew,dlg.m_ofn.lpstrFileTitle);
	    _Settings.SaveColors();
      if (_Settings.GetProgSave())
      {
        _Settings.WriteStruct("Main","MapPresent",m_HardDlg.GetMap(), m_HardDlg.GetMapSize());
        _Settings.WriteAcqAndTrigParams();
        if (m_HardDlg.GetChanNames())
        {
          _Settings.SaveChanNames(m_HardDlg.GetChanNames());
        }
        WINDOWPLACEMENT wpl;
        wpl.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(&wpl);
        _Settings.WriteStruct("Main","WindowPlacement",&wpl,sizeof(WINDOWPLACEMENT));
      }
    }
  }
  CString str;
  str.Format("ActiveD (%s)", _Settings.GetConfFileTitle());
  SetWindowText(str);

  return S_OK;
}


//050920
LRESULT CMainFrame::OnActdHelp(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if ( IsWindowEnabled())
    HtmlHelp(GetDesktopWindow(),
            g_strHelpFile,
            HH_DISPLAY_TOPIC,
            NULL) ;

   return S_OK;
}
