// ViewSettDlg.cpp : Implementation of CViewSettDlg

#include "stdafx.h"
#include "ViewSettDlg.h"
#include ".\viewsettdlg.h"

LRESULT CViewSettDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  if (m_nTimer < 1) m_nTimer = 1;
  if (m_nTimer >= 999) m_nTimer = 999;
  DoDataExchange(false);
  return 1;
}

LRESULT CViewSettDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (wID == IDC_LOADDEF)
  {
    EndDialog(wID);
    return 1;
  }



	if (::GetDlgCtrlID(GetFocus()) == IDC_NAME)
	{
		((CEdit)(GetDlgItem(IDC_NAME))).SetSel(0,-1);
		return 0;
	}
  else if (::GetDlgCtrlID(GetFocus()) == IDC_TIME)
  {
    int nTime = m_nTimer;
    if ( DoDataExchange(true, IDC_TIME) == FALSE)
    {
      m_nTimer = nTime;
      DoDataExchange(false, IDC_TIME);
    }
		((CEdit)(GetDlgItem(IDC_TIME))).SetSel(0,-1);
    return 0;
  }

  //по IDC_SAVEDEF попаду сюда же!
  if (DoDataExchange(true))
  {
    EndDialog(wID);
    return 1;
  }
  else
    return 0;
}

LRESULT CViewSettDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(wID);
  return 1;
}

BOOL CViewSettDlg::PreTranslateMessage(MSG* pMsg)
{


  return false;
}



