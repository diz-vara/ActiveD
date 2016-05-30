// TrigViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActD.h"
#include "TrigViewDlg.h"


// CTrigViewDlg dialog

//static memebers - lists of controls

static UINT idAct[16] = 
{
  IDC_LOW0,  IDC_LOW1,  IDC_LOW2,  IDC_LOW3, 
  IDC_LOW4,  IDC_LOW5,  IDC_LOW6,  IDC_LOW7, 
  IDC_LOW8,  IDC_LOW9,  IDC_LOW10, IDC_LOW11, 
  IDC_LOW12, IDC_LOW13, IDC_LOW14, IDC_LOW15 
};

static UINT idTrCol[16] = 
{
  IDC_TRCOL0,  IDC_TRCOL1,  IDC_TRCOL2,  IDC_TRCOL3,  
  IDC_TRCOL4,  IDC_TRCOL5,  IDC_TRCOL6,  IDC_TRCOL7,
  IDC_TRCOL8,  IDC_TRCOL9,  IDC_TRCOL10, IDC_TRCOL11,  
  IDC_TRCOL12, IDC_TRCOL13, IDC_TRCOL14, IDC_TRCOL15  
};


CTrigViewDlg::CTrigViewDlg()
{
  m_psp.dwFlags |= PSP_HASHELP;
}

CTrigViewDlg::~CTrigViewDlg()
{
}


int CTrigViewDlg::Find(UINT* Array, UINT Value)
{
  for (int i = 0; i < 16; i++)
  {
    if (Value == Array[i])
      return i;
  }

  return -1;
}

// CTrigViewDlg message handlers
LRESULT CTrigViewDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hWnd = (HWND)lParam;
	HDC hDC = (HDC)wParam;
	COLORREF color(::GetSysColor(COLOR_3DFACE));
	UINT nID = ::GetDlgCtrlID(hWnd);

  int nAns = Find(idTrCol,nID);

  if (nAns >= 0 && nAns < 16)
  {
    color = m_colTrig[nAns];
	  HBRUSH hbr = CreateSolidBrush(color);
	  return (LRESULT)hbr;
  }
  else
    return 0;

}


LRESULT CTrigViewDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

  WORD wOne(1);
  WORD wAct(_Settings.m_wTrigAct);
  //копирую глобальные цвета в локальные, диалога
  for (int i = 0; i < 16; i++)
  {
    m_colTrig[i] = _Settings.m_colorTrig[i];
    if (wOne & wAct)
    {
      m_bTrigAct[i] = 1;
      GetDlgItem(idAct[i]).SetWindowText("HIGH");
    }
    else
    {
      m_bTrigAct[i] = 0;
      GetDlgItem(idAct[i]).SetWindowText("LOW");
    }
    wOne <<= 1;

  }

	DoDataExchange(false);
	return TRUE;
}

BOOL CTrigViewDlg::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID)
{
  for (int i = 0; i < 16; i++)
    DDX_CHECK(idAct[i], m_bTrigAct[i]);

  return TRUE;
}

LRESULT CTrigViewDlg::OnBnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  int nIdx = Find(idAct,wID);
  if (nIdx >= 0 && nIdx < 16)
  {
    m_bTrigAct[nIdx] = !m_bTrigAct[nIdx];
    CheckDlgButton(wID, m_bTrigAct[nIdx]);
    if (m_bTrigAct[nIdx])
      GetDlgItem(wID).SetWindowText("HIGH");
    else
      GetDlgItem(wID).SetWindowText("LOW");
    SetModified();
  }

  //второй заход - ищем среди цветов
  nIdx = Find(idTrCol,wID);
  if (nIdx < 0 || nIdx > 15)
    return 0;
  COLORREF color = m_colTrig[nIdx];

	CColorDialog dlg(color,CC_RGBINIT,m_hWnd);

	if (dlg.DoModal() == IDOK)
	{
		color = dlg.GetColor();
    m_colTrig[nIdx] = color;
		GetDlgItem(wID).RedrawWindow();
    SetModified();
	}


  return 1;
}

int CTrigViewDlg::OnApply()
{

  DoDataExchange(TRUE);
  WORD wOne(1);
  WORD wRes(0);
  //копирую глобальные цвета в локальные, диалога
  for (int i = 0; i < 16; i++)
  {
    _Settings.m_colorTrig[i] = m_colTrig[i];
    if (m_bTrigAct[i])
      wRes |= wOne;
    wOne <<= 1;
  }
  _Settings.m_wTrigAct = wRes;
  return 1;



}
