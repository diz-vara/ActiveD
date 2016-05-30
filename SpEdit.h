// CSpEdit control for editing system keys
// Created 050511 20:20

#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "resource.h"


class CSpEdit : public CWindowImpl<CSpEdit,CEdit>
  ,public CMessageFilter
{
public:
  CSpEdit() : m_bCtrl(0), m_bAlt(0), m_bShift(0) {}

  DECLARE_WND_SUPERCLASS(NULL, CSpEdit::GetWndClassName())

  BEGIN_MSG_MAP(CSpEdit)
      MESSAGE_HANDLER(WM_KEYDOWN,OnKeyDown)
      MESSAGE_HANDLER(WM_CHAR,OnChar)
      MESSAGE_HANDLER(WM_KEYUP,OnKeyUp)
      MESSAGE_HANDLER(WM_SYSKEYUP,OnKeyUp)
      MESSAGE_HANDLER(WM_SYSKEYDOWN,OnChar)
      MESSAGE_HANDLER(WM_DEADCHAR,OnChar)
      MESSAGE_HANDLER(WM_SYSCHAR,OnChar)
      MESSAGE_HANDLER(WM_SYSDEADCHAR,OnChar)
  END_MSG_MAP()
  
  BOOL PreTranslateMessage(MSG* pMsg)
  {
    if (pMsg->message == WM_SYSKEYDOWN)
      return 1;
    return 0;
  }

private:
  bool m_bCtrl;
  bool m_bAlt;
  bool m_bShift;

  LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

