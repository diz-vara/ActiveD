//created 050502 0030

#pragma once

#include "ColorsPage.h"
#include "TrigViewDlg.h"
#include "ProgSetPage.h"
#include "OffColPage.h"
#include "AcqTrigDlg.h"

class CDataFile;

class CSettSheet :
	public CPropertySheetImpl<CSettSheet>
	,public CMessageFilter

{
public:
	CSettSheet( _U_STRINGorID title = (LPCTSTR) NULL, UINT uStartPage = 0, HWND hWndParent = NULL );
	~CSettSheet(void);

  BEGIN_MSG_MAP(CSettSheet)
    MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
    MESSAGE_HANDLER(WM_COMMAND, OnCommand)
  END_MSG_MAP()
  
  BOOL PreTranslateMessage(MSG* pMsg)
  {
    return false;
  }

  LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
  {
    LRESULT lRet = DefWindowProc(uMsg, wParam, lParam);
    if(HIWORD(wParam) == BN_CLICKED && ((LOWORD(wParam) == IDOK && GetResult() > 0) || LOWORD(wParam) == IDCANCEL))
    {
      if (((m_psh.dwFlags & PSH_MODELESS) == 0) && (GetActivePage() == NULL))
        DestroyWindow();
      else
      {
        m_nActivePage = GetActiveIndex();
        ShowWindow(SW_HIDE);
        GetParent().EnableWindow(1);
        GetParent().BringWindowToTop();
      }
    }
    return lRet;
  }

  LRESULT OnSysCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
  {
    if((wParam & 0xFFF0) == SC_CLOSE)
    {
      m_nActivePage = GetActiveIndex();
      ShowWindow(SW_HIDE);
      GetParent().EnableWindow(1);
      GetParent().BringWindowToTop();
    }
    else
      bHandled = false;
    return 0;
  }

  void Show()
  {
    SetActivePage(m_nActivePage);
    SetModified(IndexToHwnd(m_nActivePage),false);
    ShowWindow(SW_SHOW);
  }

  void SetFile(CDataFile* pFile) {m_pFile = pFile;}

private:
  CProgSetPage m_pgProgSet;
	CColorsPage m_pgColors;
  CTrigViewDlg m_pgTrigViewDlg;
  COffColPage m_pgOffColor;
  CAcqTrigDlg m_pgAcqTrigDlg;
  int m_nActivePage;
  CDataFile* m_pFile;
};
