// ProgSetPage.cpp : implementation file
//

#include "stdafx.h"
#include "ActD.h"
#include "ProgSetPage.h"


// CProgSetPage dialog

CProgSetPage::CProgSetPage()
{
  m_psp.dwFlags |= PSP_HASHELP;
}

CProgSetPage::~CProgSetPage()
{
}





// CProgSetPage message handlers

LRESULT CProgSetPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();
  BOOL b = _Settings.m_bWinLoad;
	DoDataExchange(false);

  GetDlgItem(IDC_WINDEF).EnableWindow(b);
  GetDlgItem(IDC_WININD).EnableWindow(b);

  EnableFolderGroup(_Settings.m_bFileFolder);
  GetDlgItem(IDC_PLUS).EnableWindow(_Settings.m_nFileMode != GENERIC_WRITE);
	return TRUE;
}

int CProgSetPage::OnApply()
{
CString str;
  DoDataExchange(TRUE);

  if (_Settings.m_bFileFolder)
  {
    if (_Settings.m_strFolder.IsEmpty())
    {
      MessageBox("You must select data folder.");
      return PSNRET_INVALID;
    }
    HANDLE hDir = CreateFile(_Settings.m_strFolder,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,0);
    if (hDir == INVALID_HANDLE_VALUE)
    {
      str.Format("Directory %s does not exist.\n   Create it?",_Settings.m_strFolder);
      if (IDYES == MessageBox(str,"no directory", MB_YESNO))
      {
        if (ERROR_SUCCESS != SHCreateDirectoryEx(0,_Settings.m_strFolder,0))
        {
          str.Format("Can't create directory\n \"%s\".",_Settings.m_strFolder);
          MessageBox(str);
          return PSNRET_INVALID;
        }
          
      }
    }
  }

  return PSNRET_NOERROR ;
}

LRESULT CProgSetPage::OnWinLoad(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  DoDataExchange(true,wID);
  BOOL b = _Settings.m_bWinLoad;
  GetDlgItem(IDC_WINDEF).EnableWindow(b);
  GetDlgItem(IDC_WININD).EnableWindow(b);
  return 1;
}


void CProgSetPage::EnableFolderGroup(BOOL bEnable)
{
  UINT ID[] = 
  {
    IDC_BROWSE, IDC_EFOLDER, IDC_BEXP, IDC_BSUBJ
  };

  for (int i = 0; i < sizeof(ID)/sizeof(UINT);i++)
  {
    GetDlgItem(ID[i]).EnableWindow(bEnable);
  }
}


LRESULT CProgSetPage::OnBFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  _Settings.m_bFileFolder = IsDlgButtonChecked(IDC_BFOLDER);
  EnableFolderGroup(_Settings.m_bFileFolder);
  SetModified(true);
  return S_OK;
}

LRESULT CProgSetPage::OnBrowse(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
char Path[MAX_PATH];

      BROWSEINFO bi;

      bi.hwndOwner = m_hWnd;
      bi.pidlRoot = 0;
      bi.pszDisplayName = Path;
      bi.lpszTitle = "Select folder for data storage";
      bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
      bi.lpfn = 0;
      bi.lParam = 0;
      bi.iImage = 0;
      ITEMIDLIST* pIT = SHBrowseForFolder(&bi);
      if (pIT == 0)
        return E_FAIL;

	    IMalloc* pMalloc;
	    SHGetMalloc(&pMalloc); 
	    SHGetPathFromIDList(pIT,Path);
	    pMalloc->Free(pIT);

      _Settings.SetFileFolder(Path);
      SetDlgItemText(IDC_EFOLDER,Path);
      SetModified(true);

  return S_OK;
}


LRESULT CProgSetPage::Change(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   SetModified(true);
   return S_OK;

}
