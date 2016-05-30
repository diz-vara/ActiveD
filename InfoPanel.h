// InfoPanel.h : interface of the CInfoPanel class
//        050618, 17:35
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "StatMap.h"
#include "resource.h"

class CDataFile;

class CInfoPanel :
  public CDialogImpl<CInfoPanel>
  ,public CDialogResize<CInfoPanel>
{
public:
   enum { IDD = IDD_INFOPANEL };

    BEGIN_MSG_MAP(CInfoPanel)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
          CHAIN_MSG_MAP(CDialogResize<CInfoPanel>)
    END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CInfoPanel)
       DLGRESIZE_CONTROL(IDC_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

  CInfoPanel(void);
  virtual ~CInfoPanel(void);
};

class CFInfoPanel :
  public CDialogImpl<CFInfoPanel>
  ,public CDialogResize<CFInfoPanel>
{
public:
   enum { IDD = IDD_FINFOPANEL };

    BEGIN_MSG_MAP(CFInfoPanel)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
          CHAIN_MSG_MAP(CDialogResize<CFInfoPanel>)
    END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CFInfoPanel)
       DLGRESIZE_CONTROL(IDC_INFO, DLSZ_SIZE_X | DLSZ_SIZE_Y)
       DLGRESIZE_CONTROL(IDC_MAP, DLSZ_SIZE_X | DLSZ_MOVE_Y)
		END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT SetFile(CDataFile* pFile = 0);

  CFInfoPanel(void);
  virtual ~CFInfoPanel(void);
	void RedrawMap() {m_Map.Redraw();}

private:
   CStatMap m_Map;

};
