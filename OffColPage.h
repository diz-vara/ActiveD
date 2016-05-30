//created 050505 18:00
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"
#include <atlddx.h>


// COffColPage dialog

class COffColPage : 
	public CPropertyPageImpl<COffColPage>
	,public CWinDataExchange<COffColPage>
  ,public CMessageFilter
{

public:
	COffColPage();
	virtual ~COffColPage();

// Dialog Data
	enum { IDD = IDD_OFFSETPAGE };

  BEGIN_MSG_MAP(COffColPage)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
    COMMAND_CODE_HANDLER(BN_CLICKED, OnBnClicked)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		COMMAND_CODE_HANDLER(EN_KILLFOCUS, OnEnKillFocus)
		COMMAND_CODE_HANDLER(EN_SETFOCUS, OnEnSetFocus)
		CHAIN_MSG_MAP(CPropertyPageImpl<COffColPage>)
  END_MSG_MAP()

  BEGIN_DDX_MAP(CViewSettDlg)
    DDX_INT_RANGE(IDC_MV1, m_nOffsMV[1], 0, m_nOffsMV[2])
    DDX_INT_RANGE(IDC_MV2, m_nOffsMV[2], m_nOffsMV[1], m_nOffsMV[3])
    DDX_INT_RANGE(IDC_MV3, m_nOffsMV[3], m_nOffsMV[2], m_nOffsMV[4])
    DDX_INT_RANGE(IDC_MV4, m_nOffsMV[4], m_nOffsMV[3], 255)
  END_DDX_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
  LRESULT OnBnClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnEnKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
  LRESULT OnEnSetFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	int OnApply();
  BOOL PreTranslateMessage(MSG* pMsg);

  BOOL OnKillActive()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    if (pLoop)
      pLoop->RemoveMessageFilter(this);
    return 0;
  }
  BOOL OnSetActive()
  {
    CMessageLoop* pLoop = _Module.GetMessageLoop();
    ATLASSERT(pLoop != NULL);
    pLoop->AddMessageFilter(this);
    return 0;
  }

protected:
  COLORREF m_colOffset[8];
  int m_nOffsMV[6];
};
