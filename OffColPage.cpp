// OffColPage.cpp : implementation file

#include "stdafx.h"
#include "ActD.h"
#include "OffColPage.h"

static UINT idMV[4] = 
  {IDC_MV1, IDC_MV2, IDC_MV3, IDC_MV4};
static UINT idOfCol[8] = 
{ IDC_OFCOLANAVAIL, IDC_OFCOLOFF, IDC_OFCOLNODATA,
  IDC_OFCOL0, IDC_OFCOL1, IDC_OFCOL2, IDC_OFCOL3, IDC_OFCOL4};


// COffColPage dialog

COffColPage::COffColPage()
{
  m_psp.dwFlags |= PSP_HASHELP;
}

COffColPage::~COffColPage()
{

}



// COffColPage message handlers
LRESULT COffColPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();
  m_nOffsMV[0] = 0;
  m_nOffsMV[5] = 250;

  m_colOffset[0] = _Settings.m_colorOffsAnavail;
  m_colOffset[1] = _Settings.m_colorOffsOff;
  m_colOffset[2] = _Settings.m_colorOffsNone;

  for (int i = 0; i < 4; i++)
  {
    m_nOffsMV[i+1] = _Settings.m_nOffsMV[i];
    m_colOffset[i+3] = _Settings.m_colorOffs[i];
  }
  m_colOffset[3+4] = _Settings.m_colorOffs[4];
	DoDataExchange(false);


	return TRUE;
}

int COffColPage::OnApply()
{
int ret = PSNRET_NOERROR;

  UINT wID = ::GetDlgCtrlID(GetFocus());
	int nIdx = Find(idMV,SIZE(idMV),wID);
  if (nIdx >= 0 )
  {
		int nSave = m_nOffsMV[nIdx+1];
    if (!DoDataExchange(TRUE,wID))
    {
			m_nOffsMV[nIdx+1] = nSave;
			SetDlgItemInt(wID, nSave, true);
      ret = PSNRET_INVALID;
    }
    ((CEdit)GetDlgItem(wID)).SetSel(0,-1);
		return ret;
  }

  if (Find(idMV, 4, wID) < 0)
  {
    _Settings.m_colorOffsAnavail = m_colOffset[0];
    _Settings.m_colorOffsOff = m_colOffset[1];
    _Settings.m_colorOffsNone = m_colOffset[2];

    for (int i = 0; i < 4; i++)
    {
      _Settings.m_nOffsMV[i] = m_nOffsMV[i+1];
      _Settings.m_colorOffs[i] = m_colOffset[i+3];
    }
    _Settings.m_colorOffs[4] = m_colOffset[4+3];
    return   PSNRET_NOERROR;

  }
  else
      return PSNRET_NOERROR ;

}


LRESULT COffColPage::OnBnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  int nIdx = Find(idOfCol,8,wID);
  if (nIdx < 0 || nIdx > 7)
    return 0;

  COLORREF color = m_colOffset[nIdx];

	CColorDialog dlg(color,CC_RGBINIT,m_hWnd);

	if (dlg.DoModal() == IDOK)
	{
		color = dlg.GetColor();
    m_colOffset[nIdx] = color;
		GetDlgItem(wID).RedrawWindow();
    SetModified();
	}

  return 1;
}

LRESULT COffColPage::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND hWnd = (HWND)lParam;
	HDC hDC = (HDC)wParam;
	COLORREF color(::GetSysColor(COLOR_3DFACE));
	UINT nID = ::GetDlgCtrlID(hWnd);

  int nAns = Find(idOfCol,8,nID);

  if (nAns >= 0 && nAns < 8)
  {
    color = m_colOffset[nAns];
	  HBRUSH hbr = CreateSolidBrush(color);
	  return (LRESULT)hbr;
  }
  else
    return 0;

}


LRESULT COffColPage::OnEnKillFocus(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	int nIdx = Find(idMV,SIZE(idMV),wID);
  if (nIdx >= 0 )
  {
		int nSave = m_nOffsMV[nIdx+1];
    if (!DoDataExchange(TRUE,wID))
    {
			m_nOffsMV[nIdx+1] = nSave;
			SetDlgItemInt(wID, nSave, true);
      ((CEdit)GetDlgItem(wID)).SetSel(0,-1);
      return 0;
    }
  }

  return 1;
}

LRESULT COffColPage::OnEnSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (Find(idMV,SIZE(idMV),wID) >= 0 )
  {
		((CEdit)GetDlgItem(wID)).SetSel(0,-1);
	}
  return 1;
}


BOOL COffColPage::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
  {
    if (GetAsyncKeyState(VK_SHIFT) < 0)
      PrevDlgCtrl();
    else
      NextDlgCtrl();
    return true;
  }
  return false;
}

