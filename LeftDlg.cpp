#include "StdAfx.h"
#include ".\leftdlg.h"


CLeftDlg::CLeftDlg(void)
{
}

CLeftDlg::~CLeftDlg(void)
{
}


LRESULT CLeftDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	this->SetWindowText(_T("Left"));

	DlgResize_Init(false, true, WS_CLIPCHILDREN);
	m_Map.SubclassWindow(GetDlgItem(IDC_FRAME));
	m_Map.MapInit(m_hWnd);

	m_Map.Resize(4,8);
	return 0;
}
