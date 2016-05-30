// MapDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "MapDlg.h"



LRESULT CMapDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
	m_Map.MapInit(m_hWnd);

  GetDlgItem(IDC_LOADNAMES).ShowWindow (m_Map.m_pMainMap == 0 );

	return TRUE;
}



LRESULT CMapDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	for (int i = 0; i < TOTCHANS; i++) {
		m_pParMap->m_bOn[i] = m_Map.m_bOn[i];
	}

	EndDialog(wID);
	return 0;
}

LRESULT CMapDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

BOOL CMapDlg::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

//050506
LRESULT CMapDlg::LoadNames(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CFileDialog dlg(TRUE, 0,0,0,"Biosemi *.cfg files\0*.cfg\0All Files (*.*)\0*.*");
  if (dlg.DoModal() == IDOK)
  {
    CString strFile = dlg.m_ofn.lpstrFile;
    CString str;
    char *pNames = m_pParMap->GetOrCreateNames();
    for(int i = 0; i < m_pParMap->m_nChans; i++)
    {
      str.Format("Chan%d",i+1);
      GetPrivateProfileString("Labels",str,"",pNames+i*8,7,strFile);
    }

    for(i = 0; i < 8; i++)
    {
      str.Format("Tou%d",i+1);
      GetPrivateProfileString("Labels",str,"",pNames+(i+256)*8,7,strFile);
    }
    m_Map.m_pChNames = pNames;

  }


  return 1;
}
