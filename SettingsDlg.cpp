#include "StdAfx.h"
#include ".\settingsdlg.h"
#include <atlddx.h>

CSettingsDlg::CSettingsDlg(void) 
{
}

CSettingsDlg::~CSettingsDlg(void)
{
}

LRESULT CSettingsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();
	DlgResize_Init();
	
	((CUpDownCtrl)GetDlgItem(IDC_SNUMCOL)).SetRange(1,64);
	m_lbColors.Attach(GetDlgItem(IDC_COLLIST));
	m_lbColors.SetCount(m_nNumColors);


	DoDataExchange(false);
	return TRUE;
}



LRESULT CSettingsDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL b = DoDataExchange(TRUE);

	if (::GetDlgCtrlID(GetFocus()) == IDC_NUMCOL)
	{
		((CEdit)(GetDlgItem(IDC_NUMCOL))).SetSel(0,-1);
		m_lbColors.SetCount(m_nNumColors);
		GetDlgItem(IDC_COLLIST).RedrawWindow();
	}
	else
	{
		_Settings = (CSettings)(*this);
		EndDialog(wID);
	}
	return 0;
}

LRESULT CSettingsDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

/*
BOOL CSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}
*/

LRESULT CSettingsDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hWnd = (HWND)lParam;
	HDC hDC = (HDC)wParam;
	COLORREF color(::GetSysColor(COLOR_3DFACE));
	UINT nID = ::GetDlgCtrlID(hWnd);


	switch (nID)
	{
	case IDC_COLBGBP:
    if (m_bBPsep)
		  color = m_colorBGBP;
		break;
	case IDC_COLBG:
		color = m_colorBG;
		break;
	case IDC_COL1:
		if (m_nColSet == 0 ) color = m_colorOne;
		break;
	case IDC_COL2ODD:
		if (m_nColSet == 1 ) color = m_color2Odd;
		break;
	case IDC_COL2EVEN:
		if (m_nColSet == 1 ) color = m_color2Even;
		break;
	
	default:
		//color = GetSysColor(COLOR_BTNFACE);
		return 0;
	}



	HBRUSH hbr = CreateSolidBrush(color);
	return (LRESULT)hbr;
}

LRESULT CSettingsDlg::OnStnClickedColor(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COLORREF *pColor;
	switch (wID)
	{

	case IDC_COLBGBP:
		pColor = &m_colorBGBP;
		break;
	case IDC_COLBG:
		pColor = &m_colorBG;
		break;
	case IDC_COL1:
		pColor = &m_colorOne;
		break;
	case IDC_COL2ODD:
		pColor = &m_color2Odd;
		break;
	case IDC_COL2EVEN:
		pColor = &m_color2Even;
		break;
	}

	CColorDialog dlg(*pColor,CC_RGBINIT,m_hWnd);

	if (dlg.DoModal() == IDOK)
	{
		*pColor = dlg.GetColor();
		GetDlgItem(wID).RedrawWindow();
	}
	return 0;
}


LRESULT CSettingsDlg::OnCtlColorListbox(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	COLORREF color(0);

	HBRUSH hbr = CreateSolidBrush(color);
	//return (LRESULT)hbr;
	return 0;
	
}

LRESULT CSettingsDlg::OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{

	DRAWITEMSTRUCT *pDI = (LPDRAWITEMSTRUCT) lParam;

	if (pDI->itemID < 0)
		return 0;
	int idx(-1);


	CRect rc;
	CDC DC(pDI->hDC);
	COLORREF clearBk  = DC.SetBkColor(0);

	m_lbColors.GetClientRect(&rc);

	if (m_nColSet != 2)
	{
		DC.FillSolidRect(&rc, ::GetSysColor(COLOR_3DFACE));
		return TRUE;
	}

	DC.FillSolidRect(&rc, ::GetSysColor(COLOR_WINDOW));

	//041020 - из-за каких-то глюков мне проще каждый раз перерисовать всё
	for (idx = 0; idx < m_nNumColors && idx < m_arColors.GetSize(); idx++)
	{
		m_lbColors.GetItemRect(idx,&rc);
		DC.FillSolidRect(&rc, m_arColors[idx]);
		rc.DeflateRect(-1,-1,-1,-1);
		DC.Draw3dRect(&rc,0,0);
	}
	
	DC.SetBkColor(clearBk);
	
	return 0;
}

LRESULT CSettingsDlg::OnLbnSelchangeCollist(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	int idx = m_lbColors.GetCurSel();
	if (idx < 0 || idx >= m_arColors.GetSize())
		return 1;

	COLORREF color = m_arColors[idx];
	CRect rc;

	CColorDialog dlg(color,CC_RGBINIT,m_hWnd);
	if (dlg.DoModal() == IDOK)
	{

		color = dlg.GetColor();
		m_arColors[idx] = color;
		m_lbColors.GetItemRect(idx,&rc);
		m_lbColors.RedrawWindow(&rc);
	}
	

	return 0;
}

BOOL CSettingsDlg::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID)
{
bool bEnable[3];

  DDX_CHECK(IDC_BIPOLSEPCOLOR, m_bBPsep);
  GetDlgItem(IDC_COLBGBP).EnableWindow(m_bBPsep);

	DDX_RADIO(IDC_COLORS1,m_nColSet);
	DDX_INT(IDC_NUMCOL, m_nNumColors);

	if (m_nNumColors < 1) m_nNumColors = 1;
	if (m_nNumColors > 64) m_nNumColors = 64;
	SetDlgItemInt(IDC_NUMCOL, m_nNumColors);
	

	if (m_nColSet < 0 || m_nColSet > 2)
		return false;

	for(int i = 0; i < 3; i++)
		bEnable[i] = false;

	bEnable[m_nColSet] = true;
	GetDlgItem(IDC_COL1).EnableWindow(bEnable[0]);
	GetDlgItem(IDC_COL2ODD).EnableWindow(bEnable[1]);
	GetDlgItem(IDC_COL2EVEN).EnableWindow(bEnable[1]);
	GetDlgItem(IDC_NUMCOL).EnableWindow(bEnable[2]);
	GetDlgItem(IDC_SNUMCOL).EnableWindow(bEnable[2]);
	GetDlgItem(IDC_COLLIST).EnableWindow(bEnable[2]);


	return true;
}

LRESULT CSettingsDlg::OnBnClickedColors(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(1,IDC_COLORS1);
	return 0;
}

LRESULT CSettingsDlg::OnEnKillfocusNumcol(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(TRUE,wID);
	m_lbColors.SetCount(m_nNumColors);
	return 0;
}

LRESULT CSettingsDlg::OnDeltaposSnumcol(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	int C;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	m_nNumColors += pNMUpDown->iDelta;
	if (m_nNumColors < 1) m_nNumColors = 1;
	if (m_nNumColors > 64) m_nNumColors = 64;
	while ((C = m_arColors.GetSize()) < m_nNumColors)
	{
		m_arColors.Add(colorDefC[C%8]);
	}
	m_lbColors.SetCount(m_nNumColors);
	m_lbColors.RedrawWindow();

	return 0;
}

LRESULT CSettingsDlg::OnBnClickedBipolsepcolor(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  DoDataExchange(true,IDC_BIPOLSEPCOLOR);
  return 0;
}
