#include "StdAfx.h"
#include "harddlg.h"
#include "MainFrm.h"
#include ".\harddlg.h"
#include <math.h>

static int FrqTab[] = {0, 2048, 1024, 512, 256, 128};



CHardDlg::CHardDlg(void) : m_nChans(0), m_nTouch(0), m_uTimer(500),
	m_nModules(4), m_pMainFrm(0), m_nFinalFrq(0), m_nDecIdx(0),
  m_colBAT(0), m_colCMS(0), m_nPresent(0), aMode(0L),
  m_brGreen(0), m_brRed(0), m_pbrCMS(0), m_pbrBAT(0)
{
}

CHardDlg::~CHardDlg(void)
{
  if (_Settings.GetProgSave())
  {
    SaveSettings();
  }

  DeleteObject(m_brGreen);
  DeleteObject(m_brRed);
}


LRESULT CHardDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	((CUpDownCtrl)GetDlgItem(IDC_NMODS)).SetRange(0,76);
  if (_Settings.GetProgLoad())
  {
    m_bTouch = _Settings.GetInt("Main","Touch",0);
    m_nTouch = (m_bTouch) ? 8:0;

    m_nModules = _Settings.GetInt("Main","Modules",2);
    m_bTrigDecimate = _Settings.GetInt("Main","TrigDec",0);
    m_nDecIdx = _Settings.GetInt("Main","DecIdx",0);
  }
  else
  {
    m_bTouch = 0;
    m_nTouch = 0;
    m_nModules = 2;
    m_bTrigDecimate = 1;
    m_nDecIdx = 0;
  }

  m_nFinalFrq = FrqTab[m_nDecIdx];

	SetDlgItemInt(IDC_TIMER,m_uTimer,0);
  if (!::IsWindow(m_cmbDecimate.m_hWnd))
  {
	  m_cmbDecimate.Attach(GetDlgItem(IDC_DECIMATE));
	  m_cmbDecimate.AddString("Keep original");
	  m_cmbDecimate.AddString("2048 Hz");
	  m_cmbDecimate.AddString("1024 Hz");
	  m_cmbDecimate.AddString("512 Hz");
	  m_cmbDecimate.AddString("256 Hz");
	  m_cmbDecimate.AddString("128 Hz");
	  m_cmbDecimate.SetCurSel(m_nDecIdx);
  }
  
  CheckDlgButton(IDC_TRIGDEC, m_bTrigDecimate);
  GetDlgItem(IDC_TRIGDEC).EnableWindow(m_nDecIdx > 0);
  CheckDlgButton(IDC_TOUCH, m_bTouch);

  SetDlgItemInt(IDC_MODN, m_nModules, 0);

  SAFE_DELETE_HANDLE(m_brGreen);
  SAFE_DELETE_HANDLE(m_brRed);
  m_brGreen = CreateSolidBrush(cGreen);
  m_brRed = CreateSolidBrush(cRed);

  if (m_pbrBAT == 0)
    m_pbrBAT = &m_brRed;
  if (m_pbrCMS == 0) 
  m_pbrCMS = &m_brRed;

	DlgResize_Init(false, true, WS_CLIPCHILDREN);
  if (!m_Map.m_hWnd)
  {
	  m_Map.SubclassWindow(GetDlgItem(IDC_MAP));
	  m_Map.MapInit(m_hWnd);
  }

	m_Map.Resize(4,8);
  
  DoDataExchange(false);
  SetNofChans(m_nChans, m_nTouch);
  SetMode(-1);
	return 0;
}

LRESULT CHardDlg::OnEnChangeModn(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  //if (m_pMainFrm->IsFile() == ID_FILE_NEW)
	m_nModules = GetDlgItemInt(IDC_MODN);
  if (m_pMainFrm && m_pMainFrm->IsFile() == GENERIC_WRITE && TRIAL)
    m_nModules = 1;
	m_nChans = m_nModules*8;
  m_bTouch = IsDlgButtonChecked(IDC_TOUCH);
	m_nTouch = ( m_bTouch) ? 8:0;
	SetDlgItemInt(IDC_NCH,m_nChans,0);
  //SetNofChans(m_nChans, m_nTouch);


  if (IsWindowVisible())   
  {
    SetNofChans(m_nChans, m_nTouch);
    if (m_nPresent)
      SetMode(aMode.GetMode());
  }

	return 0;
}


// 040619 - включаю/выключаю все контролы
LRESULT CHardDlg::EnableControls(bool bEnable)
{

  if (GetDlgItem(IDC_DECIMATE).IsWindow())
  {
    GetDlgItem(IDC_CMS).EnableWindow(bEnable);
    GetDlgItem(IDC_BATTERY).EnableWindow(bEnable);
    GetDlgItem(IDC_DECIMATE).EnableWindow(bEnable);
    GetDlgItem(IDC_TRIGDEC).EnableWindow(bEnable && (m_nDecIdx > 0));
  }

	return 0;
}

LRESULT CHardDlg::SetMode(short nMode)
{
CString str;

  if (nMode == 0)
    nMode = 0;

  ATLTRACE("CHardDlg::SetMode %d\n",nMode);

	//начинаем с того, что доустанавливаем переменные 
	m_nModules = GetDlgItemInt(IDC_MODN);

  if (m_pMainFrm && m_pMainFrm->IsFile() == GENERIC_WRITE && TRIAL)
    m_nModules = 1;

	m_nChans = m_nModules*8;
  m_bTouch = IsDlgButtonChecked(IDC_TOUCH);
	m_nTouch = ( m_bTouch ) ? 8:0;

  //050219 - for switched offf...
	int nChans(m_nChans);
	int nTouch(m_nTouch);

  
  //050117 - ввожу if для возможности вызова с -1
  aMode = actMode((short)nMode);
	if (nMode >= 0)
  {
	  nChans = aMode.GetPinChans();
	  nTouch = aMode.GetTouch();
    m_nPresent = 1;
  }
  else
    m_nPresent = 0;


	EnableControls(m_nPresent > 0);
  if (nMode < 0)
  {
    nChans = m_nChans;
    nTouch = m_nTouch;
  }


  
  
		//correction for installed modules
	if (nChans > m_nChans) nChans = m_nChans;
	if (nTouch > m_nTouch) nTouch = m_nTouch;

	if (nChans < 0) nChans = 0;

  SetNofChans(nChans, nTouch);

	if (m_pMainFrm)
  {
		::PostMessage(m_pMainFrm->m_hWnd,WM_DIZ_CHANGEMODE,(WPARAM)nChans, nTouch);
  }

	if (nMode < 0)
		str = "Disconnected";
	else {
		str.Format("Mode %s\n%d\nFrequency %d Hz\n\n%d Pin channels\n%d channels total",
      (aMode.GetMode()&16) ? "[Mk2]":"",
			aMode.GetMode()&15,aMode.GetFrq(), aMode.GetPinChans(),
      aMode.GetBufChans());
		GetDlgItem(IDC_TOUCH).EnableWindow(aMode.GetTouch());
	}

	//041108 - я буду определять фильтр здесь
  //   050228 - перенес в CMainFrame
	m_nDecCf = 1;
	if (m_nFinalFrq > 0)
	{
		m_nDecCf = aMode.GetFrq()/m_nFinalFrq;
    if (m_nDecCf < 1)
      m_nDecCf = 1;
	}

	SetDlgItemText(IDC_MODE,str);


 	return 0;
}



BOOL CHardDlg::PreTranslateMessage(MSG* pMsg)
{
	HWND hWnd = GetFocus();
	CWindow Wnd; Wnd.m_hWnd = hWnd;
	int idFocus =::GetDlgCtrlID(hWnd);
	DWORD vKey(0);

  //ATLTRACE(" CHardDlg::Pretranslate message %X %X %X\n", m_hWnd, pMsg->hwnd, pMsg->message);

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		vKey = pMsg->wParam;
		if (vKey == VK_RETURN)
		{
			if (EditUpdate(idFocus))
				return TRUE;
		} 
	default:
		break;
	}



	return IsDialogMessage(pMsg);

}


BOOL CHardDlg::EditUpdate(UINT idFocus)
{
BOOL b;
UINT u;

	switch (idFocus)
			{
			case IDC_TIMER:
				u = GetDlgItemInt(IDC_TIMER,&b,0);
				if (b && u > 0 && u <= 1000) 
				{
					m_uTimer = u;
					m_pMainFrm->SetTimer(u);
				}
				else
					SetDlgItemInt(IDC_TIMER,m_uTimer,0);
				break;
			case IDC_MODN:
				u = GetDlgItemInt(IDC_MODN,&b,0);
				if (b && u > 0 && u <= 32)
					OnEnChangeModn(0,0,m_hWnd,b);
				break;
			default:
				return FALSE;
	
			}
	((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
	return TRUE;
}
LRESULT CHardDlg::OnCbnSelendokDecimate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int i = m_cmbDecimate.GetCurSel();
	if (i < 0 || i > 5)
	{
		i = m_nDecIdx;
		m_cmbDecimate.SetCurSel(m_nDecIdx);
	}
	else
		m_nDecIdx = i;

	m_nFinalFrq = FrqTab[i];

  SetMode(aMode.GetMode());
  if (m_pMainFrm)
    m_pMainFrm->PostMessage(WM_DIZ_MAPCHANGED,0,0);

	return 0;
}

int CHardDlg::GetDecimate(bool bTrig) const
{
  if (m_nDecCf <= 1 ||  (bTrig && !m_bTrigDecimate))
    return 1;
	else
		return aMode.GetFrq() / m_nFinalFrq;
}

int CHardDlg::GetFrq(bool bTrig) const
{
	if (m_nFinalFrq == 0 || (bTrig && !m_bTrigDecimate))
		return aMode.GetFrq();
	else
		return m_nFinalFrq;

}

LRESULT CHardDlg::OnBnClickedTrigdec(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_bTrigDecimate = (IsDlgButtonChecked(IDC_TRIGDEC) != 0);
  m_pMainFrm->PostMessage(WM_DIZ_MAPCHANGED,0,0);

  return 0;
}

LRESULT CHardDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
  HWND hWnd = (HWND)lParam;
  HBRUSH hBr(0);

  //Если окно запрещено, ничего не меняем
  if (! ::IsWindowEnabled(hWnd))
    return 0;


  HDC hDC = (HDC)wParam;
  int nID = ::GetDlgCtrlID(hWnd);


  COLORREF col(0);

  if (nID == IDC_BATTERY)
  {
    col = m_colBAT;
    hBr = *m_pbrBAT;
  }
  else if (nID == IDC_CMS)
  {
    col = m_colCMS;
    hBr = *m_pbrCMS;
  }
  if (col)  ::SetBkColor(hDC,col);

  return (LRESULT)hBr;
}

void CHardDlg::SetBatCMS(DWORD dwBatCMS)
{

  //CMS within range  
  if (dwBatCMS & 1)
  {
    m_colCMS = cRed;
    m_pbrCMS = &m_brRed;
  }
  else
  {
    m_colCMS = cGreen;
    m_pbrCMS = &m_brGreen;
  }

  //Battery LOW
  if (dwBatCMS & 4)
  {
    m_colBAT = cRed;
    m_pbrBAT = &m_brRed;
  }
  else
  {
    m_colBAT = cGreen;
    m_pbrBAT = &m_brGreen;
  }
  
  GetDlgItem(IDC_BATTERY).RedrawWindow();
  GetDlgItem(IDC_CMS).RedrawWindow();


}

void CHardDlg::SaveSettings()
{
  _Settings.WriteValue("Main","Touch",m_bTouch);
  _Settings.WriteValue("Main","Modules",m_nModules);
  _Settings.WriteValue("Main","TrigDec",m_bTrigDecimate);
  _Settings.WriteValue("Main","DecIdx",m_nDecIdx);
}

LRESULT CHardDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

  return 0;
}
