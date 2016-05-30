// CSpEdit control for editing system keys
// Created 050511 20:25

#pragma once
#include "stdafx.h"
#include "SpEdit.h"

LRESULT CSpEdit::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  return S_OK;
}

LRESULT CSpEdit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

  switch(wParam)
  {
  case VK_SHIFT:
    m_bShift = true;
    break;
  case VK_CONTROL:
    m_bCtrl = true;
    break;
  case 18:
    m_bAlt = true;
    break;
  default:
    break;
  }
  return S_OK;
}

LRESULT CSpEdit::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
char cBuf[40];
  
  switch(wParam)
  {
  case VK_SHIFT:
    m_bShift = false;
    return 0;
  case VK_CONTROL:
    m_bCtrl = false;
    return 0;
  case 18:
    m_bAlt = false;
    return 0;
  default:
    break;
  }
  GetKeyNameText(lParam,cBuf,38);

  CString str = "<";
  if (m_bCtrl)
    str += "Ctrl+";
  if (m_bAlt || (lParam & 1<<29)) 
    str += "Alt+";
  if (m_bShift)
    str += "Shift+";


  str += cBuf;
  str += ">";

  SetWindowText(str);
  SetSel(0,-1);
  wParam = 0;
  return S_OK;
}
