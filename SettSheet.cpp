//created 050502 0030

#include "StdAfx.h"
#include ".\settsheet.h"
#include "ActD.h"

CSettSheet::CSettSheet(_U_STRINGorID title, UINT uStartPage,  HWND hWndParent ) :
    CPropertySheetImpl<CSettSheet> ( title, uStartPage, hWndParent )
{

  m_psh.dwFlags |= PSH_MODELESS|PSH_HASHELP;
  AddPage ( m_pgProgSet );
  //m_pgAcqTrigDlg.m_psp.dwFlags |= PSP_HASHELP;
  AddPage ( m_pgAcqTrigDlg );
  AddPage ( m_pgColors);
  AddPage ( m_pgTrigViewDlg);
  AddPage ( m_pgOffColor );

}

CSettSheet::~CSettSheet(void)
{
  int a = 0;
}

