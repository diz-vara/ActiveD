// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "actd.h"
#include "aboutdlg.h"
#include ".\aboutdlg.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CString str;
  if (TRIAL)
    str = "Trial";
  else
    str = "Full";

#ifdef _DEBUG
  str += "/DEBUG";
#endif

  m_Link.SetHyperLink(_T("mailto:diz-vara@yandex.ru"));
  //m_Link.SetLabel("Write me");
  m_Link.SubclassWindow(GetDlgItem(IDC_LINK));
  
  GetDlgItem(IDC_TRIAL).SetWindowText(str);
	CenterWindow(GetParent());
	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CAboutDlg::OnStnClickedText(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  // TODO: Add your control notification handler code here

  return 0;
}
