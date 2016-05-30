
/////////////CStatMap

#include "stdafx.h"
#include "resource.h"

#include <AtlMisc.h>
#include ".\statmap.h"
#include "MapDlg.h"
#include "ActD.h"





CStatMap::CStatMap() : m_hParent(0), m_hBmp(0), m_hMemDC(0),
	m_pBmp(0), m_nModules(2), m_nSubDiv(2), m_bTouch(true),
	m_bChild(false), m_pMainMap(0), m_bSingleSel(false),
	m_bElapsed(true), m_nChans(0),
  m_pMapDlg(0), m_pChNames(0),
  m_pParMap(0), m_pOffMap(0)
{
  ZeroMemory(m_bOn, sizeof(m_bOn));
  //ZeroMemory(m_sChNames, sizeof(m_sChNames));

	m_nCols = 32/m_nSubDiv;
	m_nRows = m_nModules*m_nSubDiv;

}

CStatMap::CStatMap( CStatMap &smap, bool bSingle) :
	m_nModules(smap.m_nModules),
	m_nSubDiv(smap.m_nSubDiv),
	m_bTouch(smap.m_bTouch),
	m_bChild(true),
	m_pMainMap(smap.m_pMainMap),
	m_pOffMap(smap.m_pMainMap),
	m_bSingleSel(bSingle),
	m_bElapsed(true),
  m_nChans(smap.m_nChans),
  m_pMapDlg(0),
  m_pParMap(smap.m_bOn),
  m_pChNames(smap.m_pChNames)
{
	for (int i = 0; i < TOTCHANS; i++) 
  {
		m_bOn[i] = smap.m_bOn[i];
	}

	m_nCols = 32/m_nSubDiv;
	m_nRows = m_nModules*m_nSubDiv;
  //if (m_nChans > 16) m_nChans = 16;
}

CStatMap::~CStatMap()
{
  if (m_pMainMap == 0 && m_pParMap == 0 && m_pChNames)
  {
    delete []m_pChNames;
  }
}


LRESULT CStatMap::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	return 1;
}

LRESULT CStatMap::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	OnSize(uMsg,wParam,lParam,bHandled);
	int nColP(m_nCols+1);
	int nRowP(m_nRows);

	if (m_bTouch) nRowP++;

	CPaintDC dc(m_hWnd);
	if ( !IsWindow() || m_hMemDC.m_hDC == 0) return 0;
	m_pBmp = m_hMemDC.SelectBitmap(m_hBmp);


	COLORREF color(0);
	COLORREF cOff = _Settings.GetOffColor();
	COLORREF cDsbl = _Settings.GetNonAvailColor();
	/*  0-128 100-200 200-  */
	int nColIdx;
	m_hMemDC.FillSolidRect(&m_rect,cDsbl);

	int i,j,cell;

	//вертикальные линии - первые 16 каналов + 8 EXT
	for (i = 0; i <= (m_nCols+1); i++) {
		m_hMemDC.MoveTo(i*m_nStep,0);
		if (m_bTouch && i <= 17 && (m_nSubDiv > 3 || i == 0 || (i+1)%m_nSubDiv == 0) )
			m_hMemDC.LineTo(i*m_nStep,m_nBot+1);
		else
			m_hMemDC.LineTo(i*m_nStep,m_nvStep*m_nRows);
	}

	char chRow[] = "ABCDEFGHabcdefghijklmnop";

	for (j = 0; j < m_nRows; j++) {
		if (j%m_nSubDiv == 0) {
			m_hMemDC.MoveTo(0,j*m_nvStep);
			if (m_nStep > 6 && m_nvStep > 6) m_hMemDC.TextOut(1,j*m_nvStep+m_nvStep/2-5,chRow+j/m_nSubDiv,1);
		} else {
			m_hMemDC.MoveTo(m_nStep,j*m_nvStep);
		}
		m_hMemDC.LineTo(m_nRight+1,j*m_nvStep);

		for (i = 1; i < m_nCols+1; i++) {
			cell = j*m_nCols+i-1;
			if (m_pOffMap)
				nColIdx = m_pOffMap[cell];
      else
				nColIdx = m_bOn[cell];
			if (j*m_nCols+i-1 < m_nChans && (!m_pMainMap || m_pMainMap[cell]) ) 
      {
				if (m_bOn[cell]) 
					color = _Settings.GetOffsColor(nColIdx);
				else if(m_bOn[cell] == 0) 
					color = cOff;
				m_hMemDC.FillSolidRect(i*m_nStep+1,j*m_nvStep+1,m_nStep-1,m_nvStep-1,color);
        if (m_nStep > 16 && m_nvStep > 16)
        {
          m_hMemDC.TextOut(i*m_nStep+2,j*m_nvStep,GetName(cell));
        }
			}
		}
	}
	m_hMemDC.MoveTo(0,j*m_nvStep);
	m_hMemDC.LineTo(m_nRight,j*m_nvStep);

	if (m_bTouch) {
		//touchproofs
		int nExtWid = (m_nSubDiv >3) ? 1:2;
		m_hMemDC.MoveTo(0,m_nRows*m_nvStep+2);
		m_hMemDC.LineTo(m_nStep*(8*nExtWid+1),m_nRows*m_nvStep+2);
		m_hMemDC.MoveTo(0,(m_nRows+1)*m_nvStep+2);
		m_hMemDC.LineTo(m_nStep*(8*nExtWid+1),(m_nRows+1)*m_nvStep+2);
		if (m_nStep > 6) m_hMemDC.TextOut(1,(m_nRows)*m_nvStep+m_nvStep/2-6,"x",1);
		for (i = 0; i < 8; i++) {
			cell = 8*32 + i;
			if (m_pOffMap)
				nColIdx = m_pOffMap[cell];
			else
				nColIdx = m_bOn[cell];
			if (!m_pMainMap || m_pMainMap[cell]) {
				if (m_bOn[cell]) 
					color = _Settings.GetOffsColor(nColIdx);
				else if(m_bOn[cell] == 0) 
					color = cOff;
				m_hMemDC.FillSolidRect(m_nStep+i*m_nStep*nExtWid+1,m_nRows*m_nvStep+3,m_nStep*nExtWid-1,m_nvStep-1,color);
			}

		}
	}

	BitBlt(dc,0,0,m_rect.right,m_rect.bottom,m_hMemDC,0,0,SRCCOPY);
	

	return 1;
}

LRESULT CStatMap::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
  CRect rect;
	GetClientRect(&rect);
	int i;
	int Num(0);
	
	int x,y;
	x = m_nX; y = m_nY;

	//for (i = 0; i < TOTCHANS; i++) m_bOldOn[i] = m_bOn[i];

	//040403 - используем полученные во время движения

	if (1|| m_bElapsed) {
		if (y >= 0 && y <= 8) {
			if (x >= 0) {
				Num = y*32+x;
				if (!m_pMainMap || m_pMainMap[y*32+x]) {
					if (m_bSingleSel) {
						for (i = 0; i < TOTCHANS; i++) 
              m_bOn[i] = 0;
						m_bOn[Num] = -1;
					} else
						m_bOn[Num] = (m_bOn[Num]) ? 0:-1;
				}
			} else {
				//No ALL command for single-selection map 040418
				if (m_bSingleSel) return 0;
				UCHAR bAll;
				Num = -1;
        //if (TRIAL && !m_pMainMap && y > 0 && y < m_nModules) return 0;
				for (i = 0; i < ((y >= m_nModules) ? 8:32); i++) {
					if (!m_pMainMap || m_pMainMap[y*32+i]) {
						bAll  = m_bOn[y*32+i];
						break;
					}
				}
				for (i = 0; i < ((y >= m_nModules) ? 8:32); i++) {
					if (!m_pMainMap || m_pMainMap[y*32+i]) {
						m_bOn[y*32+i] = -(!bAll);
					}
				}
			}
      RedrawWindow();

		}

		::PostMessage(m_hParent,WM_DIZ_MAPCHANGED,(WPARAM)Num,(LPARAM)m_hWnd);
	}


	
	
	//m_TTip.TrackActivate(&ti,TRUE);
	return 0;
}



LRESULT CStatMap::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
int nHStep,nVStep;
int nRows(m_nRows);
int nSkip(0);
if (m_bTouch) {nRows++; nSkip = 2;}

	GetClientRect(&m_rect);
	if (m_rect.bottom && m_rect.right) {
		nHStep = m_rect.Width()/(m_nCols+1);
		if (nRows > 0)
			nVStep = (m_rect.Height()-1-nSkip)/(nRows);
		else
			nVStep = 0;
		if (nHStep < 1) nHStep = 1;
		if (nVStep < 1) nVStep = 1;

		if (nHStep/nVStep > 2) {
			//шире - можно уменьшить разбиение
			if (m_nSubDiv == 4) Resize(2);
			else if (m_nSubDiv == 2) Resize(1);
		}

		if (nVStep/nHStep > 2) {
			//уже - можно увешичить разбиение
			if (m_nSubDiv == 2) Resize(4);
			else if (m_nSubDiv == 1) Resize(2);
		}

	}

	SAFE_DELETE_HANDLE(m_hBmp);
	m_hBmp = CreateCompatibleBitmap(m_hDC,m_rect.Width(),m_rect.Height());
	//Функция из чистого апи - необходимо преобразование
  m_pBmp = (HBITMAP)SelectObject(m_hMemDC,m_hBmp);


	m_nStep = m_rect.Width()/(m_nCols+1);
	if (nRows > 0)
		m_nvStep = (m_rect.Height()-nSkip-1)/(nRows);
	else
		m_nvStep = 0;
	m_nBot = m_nvStep*(nRows)+nSkip;
	m_nRight = m_nStep*(m_nCols+1);

	return 1;
}

BOOL CStatMap::PreTranslateMessage(MSG* pMsg)
{
	return 0; //IsDialogMessage(pMsg);
}

// must be called before first usage
bool CStatMap::MapInit(HWND hWnd, bool bSingle)
{
static char ToolStr[] = "It is a tooltip!";
UINT uid = 0;

	m_bSingleSel = bSingle;
	m_nDblClkTime = GetDoubleClickTime();
	m_hParent = hWnd;
	m_TTip.Create(m_hWnd);
	m_TTip.SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	GetWindowRect(&m_rect);

	m_hDC = GetDC();
  m_hMemDC.CreateCompatibleDC(m_hDC);
	m_hBmp = CreateCompatibleBitmap(m_hDC,m_rect.Width(),m_rect.Height());
	m_hMemDC.SetTextColor(RGB(0,0,155));
	m_hMemDC.SetBkColor(GetSysColor(COLOR_BTNFACE));
	m_hMemDC.SetBkMode(TRANSPARENT);
	//Функция из чистого апи - необходимо преобразование
  m_pBmp = (HBITMAP)SelectObject(m_hMemDC,m_hBmp);

	SendMessage(WM_SIZE,0,0);

  ti.cbSize = sizeof(TOOLINFO);
	strncpy(m_chToolStr,"tool",32);

  ti.uFlags = TTF_TRACK|TTF_ABSOLUTE;///*TTF_TRACK|*/TTF_SUBCLASS;
  ti.hwnd = m_hWnd;
  ti.hinst = _Module.GetModuleInstance();
  ti.uId = (UINT_PTR)m_hWnd;
  ti.lpszText = m_chToolStr;
      // ToolTip control will cover the whole window
  ti.rect.left = m_rect.left;    
  ti.rect.top = m_rect.top;
  ti.rect.right = m_rect.right;
  ti.rect.bottom = m_rect.bottom;

	//ti.rect.left = ti.rect.top = ti.rect.bottom = ti.rect.right = 0; 

	m_TTip.AddTool(&ti);
	m_TTip.TrackActivate(&ti,TRUE);
	//m_TTip.TrackPosition(5,5);

	m_TrEv.cbSize = sizeof(TRACKMOUSEEVENT);
	m_TrEv.hwndTrack = m_hWnd;
	m_TrEv.dwHoverTime = HOVER_DEFAULT;
	m_TrEv.dwFlags = TME_HOVER|TME_LEAVE;
	TrackMouseEvent(&m_TrEv);

	m_nCols = 32/m_nSubDiv;
	m_nRows = m_nModules*m_nSubDiv;

	return true;
}

LRESULT CStatMap::OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
char chRow[] = "ABCDEFGHX";

	int nExtWid = (m_nSubDiv >3) ? 1:2;
	int Num(-1);

	//при пустой карте
	if (m_nStep < 1 || m_nvStep < 1)
		return 0;


	CPoint pnt(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
  int x = pnt.x; if (x > 0) x /= m_nStep; x--;
  int y = pnt.y; if (y > 0)  y /= m_nvStep;

	if (y > m_nRows-1) {
		y = 8; //(pnt.y - 2)/m_nvStep/m_nSubDiv;
		if (x > 0) x /= nExtWid;
		if (x > 7) y = -1;
	} else {
		if (x >= m_nCols) y = -1;
		else {
			if (x >= 0) x += m_nCols*(y%m_nSubDiv);
			y /= m_nSubDiv;
		}
	}

  ATLTRACE("StatMap - y = %d\n",y);
  if (y >= 0 && y < m_nModules) 
  {
		m_TTip.TrackActivate(&ti,TRUE);
    if (x >= 0) {
			Num = y*32+x;
			//040703 - проверка на реально вставленные модули (с запретом на изменение отсутствующих)
			if (Num >= m_nChans)
			{
				strncpy(m_chToolStr,"No module",30);
				m_nX = m_nY = -1;
				m_TTip.UpdateTipText(&ti);
				ClientToScreen(&pnt);
				m_TTip.TrackPosition(pnt.x+15,pnt.y+15);
				TrackMouseEvent(&m_TrEv);
				return 0;
			}
			strncpy(m_chToolStr,GetName(x,y),30);
			if (wParam && (m_nY != y || m_nX != x)) {
				if (!m_pMainMap || m_pMainMap[Num]) {
					if (m_bSingleSel) {
						for (int i = 0; i < TOTCHANS; i++) m_bOn[i] = 0;
						m_bOn[Num] = -1;
					} else
						m_bOn[Num] = !m_bOn[Num];
					m_nX = x; m_nY = y; 
					Redraw();
					::PostMessage(GetParent(),WM_DIZ_MAPCHANGED,(WPARAM)Num,(LPARAM)m_hWnd);
				}
			}
    } 
    else 
    {
			if (x == -2) sprintf(m_chToolStr,"%c 17-32",chRow[y]);
			else sprintf(m_chToolStr,"%c 1-32",chRow[y]);
    }
	} 
  else if (y == 8 /*m_nModules*/) 
  {
		m_TTip.TrackActivate(&ti,TRUE);
    if (x >= 0) {
			Num = y*32+x;
			strncpy(m_chToolStr,GetName(x,y),30);
	    //sprintf(m_chToolStr,"Ext %d",x+1);
			if (wParam && (m_nY != y || m_nX != x)) {
				if (!m_pMainMap || m_pMainMap[Num]) {
					if (m_bSingleSel) {
						for (int i = 0; i < TOTCHANS; i++) m_bOn[i] = 0;
						m_bOn[Num] = -1;
					} else
						m_bOn[Num] = ~m_bOn[Num];
					Redraw();
					::PostMessage(GetParent(),WM_DIZ_MAPCHANGED,(WPARAM)Num,(LPARAM)m_hWnd);
				}
			}
    } 
    else 
    {
				sprintf(m_chToolStr,"Ext 1-8");
    }
	}
	if (y < 0) 
  {
    ATLTRACE("TTip desactivate\n");
    m_TTip.ShowWindow(0);
		//m_TTip.TrackActivate(&ti,false);
	} 
  else 
  {
		m_TTip.UpdateTipText(&ti);
	  ClientToScreen(&pnt);
		m_TTip.TrackPosition(pnt.x+15,pnt.y+15);
	} 
	TrackMouseEvent(&m_TrEv);
	m_nX = x; m_nY = y; 

	return 0;
}

LRESULT CStatMap::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  ATLTRACE("StatMap: OnMouese Leave\n");
  m_TTip.ShowWindow(0);
	//m_TTip.TrackActivate(&ti,false);
	//TrackMouseEvent(&m_TrEv);

	return 0;
}


void CStatMap::Resize(int nSubDivs, int nChans, int nTouch)
{

int nRows(m_nRows);
int nSkip(0);
if (m_bTouch) {nRows++; nSkip = 2;}

	if (nChans >= 0) 
	{
    //if (nChans > 16) nChans = 16;
		m_nChans = nChans;

		m_nModules = nChans/32 + ((nChans%32) ? 1:0);
	}
	if (nSubDivs > 0) m_nSubDiv = nSubDivs;
	m_nCols = 32/m_nSubDiv;
	m_nRows = m_nModules*m_nSubDiv;
	if (nTouch >= 0) 	m_bTouch = (nTouch > 0);
	if (nSubDivs < 0) {
		GetClientRect(&m_rect);
		if (m_rect.bottom && m_rect.right) {
			int nHStep = m_rect.Width()/(m_nCols+1);
			int nVStep(0);
			if (nRows > 0)
				nVStep = (m_rect.Height()-1-nSkip)/(nRows);
			if (nHStep < 1) nHStep = 1;
			if (nVStep < 1) nVStep = 1;

			if (nHStep/nVStep > 2) {
				//шире - можно уменьшить разбиение
				if (m_nSubDiv == 4) m_nSubDiv = 2;
				else if (m_nSubDiv == 2) m_nSubDiv = 1;
			}

			if (nVStep/nHStep > 2) {
				//уже - можно увешичить разбиение
				if (m_nSubDiv == 2) m_nSubDiv = 4;
				else if (m_nSubDiv == 1) m_nSubDiv = 2;
			}
		}

		m_nCols = 32/m_nSubDiv;
		m_nRows = m_nModules*m_nSubDiv;
	}
		nRows = m_nRows; if (m_bTouch) {nRows++; nSkip = 2;}
		m_nStep = m_rect.Width()/(m_nCols+1);

		//здесь все правильно!!!
		if (nRows > 0)
			m_nvStep = (m_rect.Height()-nSkip-1)/(nRows);
		else 
			m_nvStep = 0;
		m_nBot = m_nvStep*(nRows)+nSkip;
		m_nRight = m_nStep*(m_nCols+1);


	Redraw();
}


LRESULT CStatMap::OnLButtonDblClk(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!m_bChild) {
		CMapDlg dlg(this);
		dlg.DoModal();
		Redraw();
		::PostMessage(m_hParent,WM_DIZ_MAPCHANGED,(WPARAM)-1,(LPARAM)m_hWnd);
	}
	return 0;
}


const char* CStatMap::GetName(int x, int y)
{
char chRow[] = "ABCDEFGHXabcdefghijklmnopqrstuvw";
static char str[32];

		if (y == 8)
    {
      if(m_pChNames && m_pChNames[(256+x)*8])
      {
        strncpy(m_chName, m_pChNames+(256+x)*8, 7);
        m_chName[7] = 0;
      }
      else
	      sprintf(m_chName,"Ext%d",x+1);
    }
		else
    {
      if(m_pChNames && m_pChNames[(32*y+x)*8])
      {
        strncpy(m_chName, m_pChNames+(32*y+x)*8, 7);
        m_chName[7] = 0;
      }
      else
  	    sprintf(m_chName,"%c%02d",chRow[y],x+1);
    }

		return m_chName;

}

const char* CStatMap::GetName(int N)
{
	int y = N/32;
	int x = N%32;
	return GetName(x,y);


}

void CStatMap::Redraw()
{
  RedrawWindow();
  //if (m_pMapDlg)
  //{
  //  m_pMapDlg->RedrawMap();
  //}
}
