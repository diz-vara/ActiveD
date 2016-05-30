// MapDlg.cpp : implementation of the CBiPolDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "BiPolDlg.h"
#include ".\bipoldlg.h"



LRESULT CBiPolDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
     // center the dialog on the screen
     CenterWindow();
/*
     // set icons
     HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
          IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
     SetIcon(hIcon, TRUE);
     HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
          IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
     SetIcon(hIconSmall, FALSE);
*/

     
     DlgResize_Init();

     m_Map.SubclassWindow(GetDlgItem(IDC_FRAME));
     m_Map.MapInit(m_hWnd,true); //Set in SINGLE mod
     m_Map2.SubclassWindow(GetDlgItem(IDC_FRAME2));
     m_Map2.MapInit(m_hWnd,true); //Set in SINGLE mod
		 if (m_pMainMap) {
			 m_Map.SetMainMap(m_pMainMap, m_pNames);
			 m_Map2.SetMainMap(m_pMainMap, m_pNames);
		 }
		int N = m_nChans/32; 
		if (m_nChans%32) N++;

		m_Map.Resize(-1,m_nChans,m_nTouch);
		m_Map2.Resize(-1,m_nChans,m_nTouch);
		int Code(SW_HIDE);
		if (m_nItem)
			Code = SW_SHOW;

		if(m_bpData.IsValid()) {
			m_Map.m_bOn[m_bpData.GetSubtr()] = 1;
			m_Map2.m_bOn[m_bpData.GetMinue()] = 1;
			SetDlgItemText(IDC_NAME,m_bpData.GetName());
		}

		GetDlgItem(IDC_REMOVE).ShowWindow(Code);

     return TRUE;
}



LRESULT CBiPolDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
char Name[24];

	if (!m_bpData.IsValid()) {
		MessageBox("Both Subtrahend and Minuend must be set!");
		return 0;
	}
	GetDlgItem(IDC_NAME).GetWindowText(Name,20);
	m_bpData.SetName(Name);


	EndDialog(wID);
  return 0;
}

LRESULT CBiPolDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
     EndDialog(wID);
     return 0;
}

BOOL CBiPolDlg::PreTranslateMessage(MSG* pMsg)
{
     return IsDialogMessage(pMsg);
}


LRESULT CBiPolDlg::OnMapChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hWndFrom = (HWND)lParam;
	int nNum = (int)wParam;
	int idx(-1);
	if (hWndFrom == GetDlgItem(IDC_FRAME)) 
		idx = 0;
	else if (hWndFrom == GetDlgItem(IDC_FRAME2)) 
		idx = 1;

	//error
	if (nNum < 0 || idx < 0)
		return 0;

	m_bpData.SetData(idx,nNum);
	CString str,strM(m_Map.GetName(m_bpData.GetMinue())),strS(m_Map2.GetName(m_bpData.GetSubtr()));
	str.Empty();
	if (m_bpData.IsValid()) {
		str.Format("%s - %s",strS, strM);
	}
	SetDlgItemText(IDC_SNAME,str);
	SetDlgItemText(IDC_NAME,str);
	m_bpData.SetName(str);

	return 0;
}

