// ActDView.h : interface of the CActDView class
//
/////////////////////////////////////////////////////////////////////////////
#define NUMCHANS 256

#pragma once
#include <AtlFrame.h>
#include <AtlSplit.h>
#include "ActD.h"


class CMainFrame;
class CDlgView;
class _GrBitmap;
class CChanWrite;

template <class T> 
class CSplitHor : public CSplitterImpl<T, false> {};

class DispCell
{
//ctor
public:
  DispCell() : m_bRef(false), m_nMono(0)
  {
  }
  ~DispCell()
  {
    m_arBpNum.RemoveAll();
  }

//Write access
  void Clear() 
  {
    m_bRef = 0;
    m_nMono = 0;
    m_arBpNum.RemoveAll();
  }
  
//data
public:
  bool m_bRef;
  int m_nMono;
  CSimpleArray<int> m_arBpNum;
};

class CActDView : //public CSplitHor<CActDView>
  public CFrameWindowImpl<CActDView>
  ,public CMessageFilter
{
protected:
  const int m_nType;
	const CMainFrame* m_pMainFrame;
	bool m_bChild;
	HWND m_hParent;
	HWND m_hFrame;
  CRect m_rcIn, m_rcOut;
	HDC m_hDC;
  //CDC for constant bitmap
	CDC m_hMemDC;
	CDC m_hTextDC;
  //CDC for temporary bitmaps
  CDC m_hTempDC;
  CPen pnGrey,pnGreen,pnBorder, pnWarning, *pPen;
  HBRUSH m_brBack,m_brOld;




	//040630 - количество забуферизованных каналов (Вместе с триггером)
	int m_nBufChans;

	//040705
	int m_nDispChans;
	// BiPolar chans - 040710
	int m_nBpChans;
  // mono+bipolar
  int m_nTotChans;
  // ref chans
  int m_nRefNum;


	POINT *m_Pnt;
	//short min[NUMCHANS], max[NUMCHANS], d[NUMCHANS];
	long m_Zero[NUMCHANS];

	//040707
	//long* m_pMainBuf;

	//041023
	CBitmap m_bmpArrowOut;
	CBitmap m_bmpArrowIn;
	HICON m_hIcon;


  //050301
  CMessageLoop *m_pLoop;

  //050318 - критическая секция для доступа к счетчикам
  //CRITICAL_SECTION m_csCounts;


  //сохраняемая в памяти копия экрана
  HBITMAP m_hMemBmp;
  //место для сохранения копии...
  HBITMAP m_hOldBmp;
  HBITMAP m_hOldTBmp;
  //050507 = место для имен каналов
  HBITMAP m_hTextBmp;

  //050402

  //ширина окна в пикс
  int m_nWinW;
  //высота окна в пикс
  int m_nWinH;

  //050418
  DispCell m_dcMapDisp[TOTCHANS];


  //050426
  int m_nPinChans;

  UINT m_uThread;
  COLORREF m_colBG;

  bool m_bHere;

public:

	int m_nFrq;	//частота дискретизации
	int m_nDur;			//длина в 1/4 секунадах
	int m_nLength;	//всего отсчётов
  int m_nTLength;


	//040703
	CDlgView *m_pDialog;

	bool m_bActive;
	DECLARE_WND_CLASS(NULL)
	//ctor
	CActDView(const CMainFrame* pFrame = 0, CMessageLoop* pLoop = 0, int Type = -1) : 
    m_bChild(0)
    ,m_hParent(0)
		,m_rcIn(rcDefault)
    ,m_rcOut(rcDefault)
		,m_hFrame(0)
    ,m_bActive(false)
		,m_hMemBmp(0)
    ,m_hMemDC(0)
    ,m_nDur(16)
		,m_pMainFrame(pFrame)
		//,m_pMainBuf(0)
		,m_hIcon(0)
    ,m_pLoop(pLoop)
    ,m_nTotChans(0)
    ,m_hOldBmp(0)
    ,m_hTextBmp(0)
    ,m_nType(Type)
    ,m_bHere(false)
		{
			GdiSetBatchLimit(1);
      m_uThread = GetCurrentThreadId();
      m_Pnt = new POINT[8192];
			ATLTRACE("View::ctr %8X %8x\n",m_uThread, m_hWnd);
		}


	void SetPar(HWND hPar);
	
	virtual ~CActDView();

	BOOL PreTranslateMessage(MSG* pMsg);
	//CCommandBarCtrl m_CmdBar;

	BEGIN_MSG_MAP(CActDView)
		MESSAGE_HANDLER(WM_DIZ_TEST, OnTest)
		//MESSAGE_HANDLER(WM_DIZ_PLOT, Plot)
		MESSAGE_HANDLER(WM_DIZ_SETACTIVE, OnSetActive)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_NCRBUTTONDOWN, OnMButtonUp)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_ONE, OnOne)
		COMMAND_ID_HANDLER(ID_TWO, OnOne)
		COMMAND_ID_HANDLER(ID_THREE, OnOne)
		//MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_CHILDACTIVATE, OnActivate)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_NCLBUTTONDOWN, OnNcLButtonDown)
    //CHAIN_MSG_MAP(CSplitHor<CActDView>)
		//MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);



	LRESULT OnOne(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	virtual void OnFinalMessage(HWND hWnd);
	//LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNcPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void Test(void);
	LRESULT OnTest(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	//virtual LRESULT Plot(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetActive(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	BOOL IsChild() {return m_bChild;}
//041001

  //050503

public:
	LRESULT OnNcLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  //050429 - сохранение и восстановление положения окна
  BOOL GetPosition(RECT *prcIn, RECT *prcOut);

  BOOL SetPosition(RECT *prcIn, RECT *prcOut, bool bChild);

  //050505 - принудительное задочивание
  void DockWindow() 
  {
    if (m_hParent)
      SetPar(m_hParent);
  }

  //050507
  CSimpleArray<CString> m_saChNames;

  //050531
  int GetType() const {return m_nType;}
  //050623
  int GetNum() const; 
  virtual BOOL PlotRecord(int nRec)
  {
    return FALSE;
  }

  virtual void Kill_Timer() {}



};


///////////////////////////////////////////////////
// COscView
class COscView : public CActDView
{
protected:
  //Data
	bool m_bZero;
	bool m_bScroll;
	bool m_bChange;
  //начало отобр. уч-ка в пикс.
  int m_nWinBegPix;
	//041001
	fltCf2 m_cfLP;
	fltCf1 m_cfHP;
  //050331
  UINT m_uTimerID;
  //050401 - время для отрисовки
  DWORD m_dwTime;
	//041001
	//буфера для "старых иксов" - для HP & LP фильтров
	double *m_pOldHPx;
	double *m_pOldLPx;
	double *m_pOldHPy;
	double *m_pOldLPy;
  //050419
  int m_nTrigFrameCnt;
	//040630
	// канальный буфер
	long *m_pChanBuf;
	// буфер для рефернса (один канал)
	long *m_pRefBuf;
	// триггерный буфер
	WORD *m_pTrigBuf;
	//050208 - максимально полученная длина буфера для рисования
	int m_nMaxCnt;
  //050419 - максимальная длина тригбуфера
	int m_nMaxTrigCnt;

  //050405
  // Bitmap для записи (рисования)
  _GrBitmap *m_pWriteBmp;
  // Битмэп для чтения (отображения)
  _GrBitmap *m_pReadBmp;

  //050411
  //отнесенный к окну счетчик отсчетов
  int m_nWinFrameCnt;

  //счечик отсчетов на секунду - для тиков
	int m_nOneSecCnt;
  int m_nTimer;


  //methods
  void Set_Timer(UINT uTime);
	void SetBuffers(void);
  void Draw(int nBase, int Cnt, int nTCnt);
  void Kill_Timer();


public:
  COscView(const CMainFrame* pFrame = 0, CMessageLoop* pLoop = 0) : 
      CActDView(pFrame, pLoop, 0)
		  ,m_bScroll(true)
		  ,m_bZero(false)
      ,m_bChange(false)
      ,m_uTimerID(0)
      ,m_dwTime(0) 
		  ,m_pOldHPx(0)
      ,m_pOldLPx(0)
		  ,m_pOldHPy(0)
      ,m_pOldLPy(0)
		  ,m_nMaxCnt(0)
      ,m_nMaxTrigCnt(0)
  		,m_pTrigBuf(0), m_pChanBuf(0), m_pRefBuf(0)
      ,m_pWriteBmp(0)
      ,m_pReadBmp(0)
      ,m_nWinFrameCnt(0)
      ,m_nOneSecCnt(0)
      ,m_nTimer(20)
      {}

  ~COscView();

  LRESULT OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnDizVTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT Plot(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT SetFrq(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCfChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnChangeTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetZero(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  {
    m_bZero = true;
    return S_OK;
  }
	LRESULT OnSetScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
  {
    m_bScroll = (wParam != 0);
    return S_OK;
  }
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  //050406 - критическая секция для доступа к битмэпам
  CRITICAL_SECTION m_csBitMaps;

	BEGIN_MSG_MAP(COscView)
		//MESSAGE_HANDLER(WM_TIMER, OnDizVTimer)
		MESSAGE_HANDLER(WM_DIZ_VTIMER, OnDizVTimer)
		MESSAGE_HANDLER(WM_DIZ_PLOT, Plot)
		MESSAGE_HANDLER(WM_DIZ_MAPCHANGED, OnMapChanged)
		MESSAGE_HANDLER(WM_DIZ_SETFRQ, SetFrq)
		MESSAGE_HANDLER(WM_DIZ_CFCHANGED, OnCfChanged)
		MESSAGE_HANDLER(WM_DIZ_CHANGETIMER, OnChangeTimer)
		MESSAGE_HANDLER(WM_DIZ_SETZERO, OnSetZero)
		MESSAGE_HANDLER(WM_DIZ_SETSCROLL, OnSetScroll)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
    CHAIN_MSG_MAP(CActDView)
  END_MSG_MAP()


};


/////////////////////////////////////////////////////////////////
// CSnapView
class CDataFile;

class CSnapView : public CActDView
{
protected:
  //Data
  CDataFile*  const m_pFile;

	bool m_bChange;
  //начало отобр. уч-ка в пикс.
  int m_nWinBegPix;
	//041001
	fltCf2 m_cfLP;
	fltCf1 m_cfHP;
	//041001
	//буфера для "старых иксов" - для HP & LP фильтров

	double m_X[3];
	double m_Y[3];
  //050419
  int m_nTrigFrameCnt;
	//040630
	// канальный буфер
	long *m_pChanBuf;
	// буфер для рефернса (один канал)
	long *m_pRefBuf;
	// триггерный буфер
	WORD *m_pTrigBuf;
	//050208 - максимально полученная длина буфера для рисования
	int m_nMaxCnt;
  //050419 - максимальная длина тригбуфера
	int m_nMaxTrigCnt;

  //050411
  //отнесенный к окну счетчик отсчетов
  int m_nWinFrameCnt;

  //счечик отсчетов на секунду - для тиков
	int m_nOneSecCnt;
  //methods

  //050531
  //длина тригканала в отсчетах
  int m_nTLength;
  //длина аналогового канала в отсчетах
  int m_nLength;
  //кол-во аналоговых каналов
  int m_nChanNum;
  //массив для реальных номеров каналов
  int *m_pnChanNums;
  //промежуточный буфер для подоготовки к рисованию
  long *m_pDrawBuf;

  BOOL Plot();
  void PolyLine(int Chan, int H, int y0, int nResol);
  
  //050624
  bool m_bDataPresent;

  //050625
  Pointer24 m_ReadPointer;
  int24 *m_pReadBuf24;
  //Массив файловых каналов (здесь - все, в том числе триггре, аннотации и черт в ступе
  const CChanWrite*  m_pFChans;
  const CChanWrite*  m_pTChan;

  char*  m_pOffMap;

  //050714
  int m_nCurRecord;
  //050716
  float m_flBaseTime;
  int m_nDur;


public:
  CSnapView(CDataFile* const pFile, const CMainFrame* pFrame = 0, CMessageLoop* pLoop = 0) : 
      CActDView(pFrame, pLoop, 1)
      ,m_pFile(pFile)
      ,m_bChange(false)
		  //,m_pOldHPx(0)
      //,m_pOldLPx(0)
		  //,m_pOldHPy(0)
      //,m_pOldLPy(0)
		  ,m_nMaxCnt(0)
      ,m_nMaxTrigCnt(0)
  		,m_pTrigBuf(0), m_pChanBuf(0), m_pRefBuf(0)
      ,m_nWinFrameCnt(0)
      ,m_nOneSecCnt(0)
      ,m_pnChanNums(0)
      ,m_pDrawBuf(0)
      ,m_bDataPresent(false)
      ,m_pReadBuf24(0)
      ,m_pFChans(0)
      ,m_pOffMap(0)
      ,m_nCurRecord(-1)
      {
        m_ReadPointer.pBuf = 0;
      }

  ~CSnapView();

  LRESULT OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT Frame(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCfChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSnapInit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  //050406 - критическая секция для доступа к битмэпам
  CRITICAL_SECTION m_csBitMaps;

	BEGIN_MSG_MAP(CSnapView)
		MESSAGE_HANDLER(WM_DIZ_SNAPPLOT, Frame)
    MESSAGE_HANDLER(WM_DIZ_SNAPINIT, OnSnapInit)
		MESSAGE_HANDLER(WM_DIZ_MAPCHANGED, OnMapChanged)
		MESSAGE_HANDLER(WM_DIZ_CFCHANGED, OnCfChanged)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
    CHAIN_MSG_MAP(CActDView)
  END_MSG_MAP()

  BOOL PlotRecord(int nRec);

};
