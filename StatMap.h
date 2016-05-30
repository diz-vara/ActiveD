// StatMap.h : interface of the CCStatMap class
//а этот файл где лежит???
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "ActD.h"
#include <atlmisc.h>

class _BiPol 
{
protected:
	int	m_nNums[2];	//m_nMinuend, m_nSubtrahend;
	char m_czName[40];
public:
	_BiPol() {m_czName[0] = 0; m_nNums[0] = m_nNums[1] = -1;}
	_BiPol(int nMin, int nSubtr, CString strName)
	{strncpy(m_czName,strName,39); m_nNums[0] = nMin; m_nNums[1] = nSubtr;}
	_BiPol(int nMin, int nSubtr, const char cName[])
	{strncpy(m_czName,cName,39); m_nNums[0] = nMin; m_nNums[1] = nSubtr;}


	void SetData(int newData[2]) {
		m_nNums[0]= newData[0];
		m_nNums[1]= newData[1];
	}

	void SetData(int idx, int Data) {
		ATLASSERT(idx == 0 || idx == 1);
		m_nNums[idx] = Data;
	}
	//уменьшаемое
	int GetSubtr() const {return m_nNums[0];} 

	//вычитаемое
	int GetMinue() const {return m_nNums[1];}

	//произвольное им€
	const char* GetName() const {return (const char*)m_czName;}
	const char* SetName(const char* Name) {strncpy(m_czName,Name,39); return Name;}
	bool IsValid() const {return ( (m_nNums[0] >= 0 && m_nNums[1] >= 0));}
};


class CMapDlg;

class CStatMap: public CWindowImpl<CStatMap,CStatic>, public CMessageFilter
{
friend class CMapDlg;
private:
	int m_nBot;
	int m_nRight;
	int m_nStep;
	int m_nvStep;
	int m_nTPleft;
	int m_nTPtop;
	HWND	m_hParent;
	CRect m_rect;
	char m_chToolStr[32];
	char m_chName[32];
	TRACKMOUSEEVENT m_TrEv;
	//040404 - будем рисовать в пам€ти
  HDC m_hDC;
	CDC m_hMemDC;
	HBITMAP m_hBmp, m_pBmp;
	bool m_bTouch;
	bool m_bChild;
	bool m_bSingleSel;
	UINT m_nDblClkTime;
	bool m_bElapsed;
  CMapDlg *m_pMapDlg;

protected:
	int m_nCols;
	int m_nRows;
	//количество инсталлированных модулей
	int m_nModules;
	//на сколько р€дов делим модуль
	int m_nSubDiv;
	//массив электродов (A-H)
	int m_nY;
	//Ќомер электрода в массиве
	int m_nX;
	//UCHAR m_bOldOn[TOTCHANS];
	const char* m_pMainMap;

	//040703
	// reak number of channels
	int m_nChans;

  //050506
	char* m_pParMap;

  //050630 - отдельна€ карта дл€ смещений
  const char* m_pOffMap;

public:

	//CTORs
	CStatMap();
	CStatMap( CStatMap &sma, bool bSingle = false);
  ~CStatMap();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	char m_bOn[TOTCHANS];
  char *m_pChNames;

	CToolTipCtrl m_TTip;
	TOOLINFO ti;
	void Resize( int nSubDivs = -1, int nChans = -1, int nTouch = -1);


	BEGIN_MSG_MAP(CStatMap)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnLButtonDblClk)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// must be called before first usage
	bool MapInit(HWND hWnd, bool bSingle = false);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	const char* GetName(int x, int y);
	const char* GetName(int N);
	void SetMainMap (const char* map, char* pNames = 0) {
		m_pMainMap = map;
    m_pOffMap = map;
    m_pChNames = pNames;
	}

  void SetOffMap(const char* map)
  {
    m_pOffMap = map;
  }
	
	UCHAR GetCell(int CellNum) const
	{
		if (CellNum >= 0 && (CellNum < m_nChans || (m_bTouch && CellNum >= 32*8 && CellNum < 33*8)))
		{
			if (!m_pMainMap || m_pMainMap[CellNum])
				return m_bOn[CellNum];
			else
				return 0;
		}
		else
			return 0;
	}

	UCHAR GetCell(int X, int Y) const
	{
		return GetCell(Y*32+X);
	}

  size_t GetMapSize() {return sizeof(m_bOn);}
  char* GetMap() {return m_bOn;}
  char* GetOrCreateNames()
  {
    if (m_pChNames == 0)
      m_pChNames = new char[TOTCHANS*8];
    ZeroMemory(m_pChNames,TOTCHANS*8);
    return m_pChNames;
  }

  //050506
  void Redraw();
  //050620
  void CopyMap(const char* cp)
  {
    ::CopyMemory(m_bOn,cp,TOTCHANS);
  }
};
