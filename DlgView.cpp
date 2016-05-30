// DlgView.cpp : implementation of the CDlgView class
//
// 040418 - выделен из MainFrm
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "ActDView.h"
#include "DlgView.h"
#include ".\dlgview.h"
#include "BiPolDlg.h"
#include "MainFrm.h"
#include "GrThread.h"
#include "math.h"
#include "ViewSettDlg.h"

///Dialog
LRESULT CDlgView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{

	///—ейчас здесь попробуем создавать Trhead
	m_pThread = new GrThread(m_pFrame, m_nType);
	m_pThread->Start();
	Sleep(150);

  
  HICON hIcon = (HICON)::LoadImage(
				_Module.GetResourceInstance(),
				MAKEINTRESOURCE(IDR_MAINFRAME),
				IMAGE_ICON, 16, 16, LR_SHARED);
	this->SetIcon(hIcon, ICON_SMALL);

	DlgResize_Init(false, true, WS_CLIPCHILDREN);

  
  m_mapDisp.SubclassWindow(GetDlgItem(IDC_FRAME));
	m_mapDisp.MapInit(m_hWnd);
	m_mapRef.SubclassWindow(GetDlgItem(IDC_FRAME2));
	m_mapRef.MapInit(m_hWnd);



  m_lstBP.Attach(GetDlgItem(IDC_BPLIST));
	m_lstBP.SetViewType(LVS_REPORT);
	//m_lstBP.SetExtendedListViewStyle(LVS_EX_CHECKBOXES);
	CRect rc;
	::GetClientRect(m_lstBP,&rc);
	m_lstBP.InsertColumn(0,"Title",LVCF_TEXT,rc.Width(),-1);
	//InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
	m_lstBP.SetColumnWidth(0,rc.Width()-16);
	m_lstBP.InsertItem(0,"<Add new>");

	//040929
	m_cmbFLP.Attach(GetDlgItem(IDC_CMBFLP));
	m_cmbFLP.AddString("NONE");
	m_cmbFLP.AddString("10 kHz");
	m_cmbFLP.AddString("1 kHz");
	m_cmbFLP.AddString("100 Hz");
	m_cmbFLP.AddString("50 Hz");
	m_cmbFLP.AddString("40 Hz");
	m_cmbFLP.AddString("30 Hz");
	m_cmbFLP.AddString("20 Hz");
	m_cmbFLP.AddString("10 Hz");

	m_cmbFHP.Attach(GetDlgItem(IDC_CMBFHP));
	m_cmbFHP.AddString("NONE");
	m_cmbFHP.AddString("0.1 Hz");
	m_cmbFHP.AddString("0.5 Hz");
	m_cmbFHP.AddString("1 Hz");
	m_cmbFHP.AddString("3 Hz");
	m_cmbFHP.AddString("5 Hz");
	m_cmbFHP.AddString("10 Hz");
	m_cmbFHP.AddString("100 Hz");


	((CUpDownCtrl)GetDlgItem(IDC_SSCALE)).SetRange(1,100);
	((CUpDownCtrl)GetDlgItem(IDC_SBPSCALE)).SetRange(1,100);
	m_bTrigger = true;

  if (_Settings.GetWinLoad() >= 0)
  {
    LoadSettings(false,0);
  }




	//040726 - теперь диалог сам будет создавать вид...
	//m_pView = new CActDView(m_pFrame);


  //050427
	m_bmpTool.LoadBitmap(MAKEINTRESOURCE(IDB_TOOL));
  ((CButton)GetDlgItem(IDC_TOOL)).SetBitmap(m_bmpTool);



  //050317
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);


	return 0;
}

LRESULT CDlgView::OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	LPMSG pMsg = (LPMSG)lParam;

	return this->PreTranslateMessage(pMsg);
}


BOOL CDlgView::PreTranslateMessage(MSG* pMsg)
{
	HWND hWnd = GetFocus();
	CWindow Wnd; Wnd.m_hWnd = hWnd;
	int idFocus =::GetDlgCtrlID(hWnd);
	DWORD vKey(0);

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		vKey = pMsg->wParam;
		if (vKey == VK_RETURN)
		{
			if (EditUpdate(idFocus))
				return TRUE;
		} 
    else if (vKey == VK_PRIOR || vKey == VK_NEXT || vKey == VK_HOME || vKey == VK_END || vKey == VK_LEFT || vKey == VK_RIGHT)
      return ChangeRec(vKey);
    return false;
	default:
		break;
	}



	return IsDialogMessage(pMsg);
}

LRESULT CDlgView::OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	static bool bHere(true);
	if (!m_pView)
		return FALSE;



	if (!bHere) {
		bHere = true;
    m_pFrame->PostMessage(WM_DIZ_VIEW_ACTIVATE,1,(DWORD)m_pView);

		m_pView->PostMessage(WM_DIZ_SETACTIVE,1,1);
	}


	bHere = false;
	return TRUE;
}



LRESULT CDlgView::OnNMDblclkBplist(int idCtrl, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMITEMACTIVATE lpn = (LPNMITEMACTIVATE) pNMHDR;
	CString str,strT;

	int nItem = lpn->iItem;


	m_lstBP.GetItemText(nItem,0,strT);
	str.Format("Item %d, %s",nItem,strT);
	_BiPol BP;
	if (nItem > 0)
		BP = m_arBP[nItem-1];
	CBiPolDlg dlg(m_pMainMap,nItem,m_nChans,m_nTouch,BP, m_pNames);


	int Ans = dlg.DoModal();
	if (Ans == IDOK) {
		_BiPol bpData = dlg.GetData();
		if (nItem == 0) {
			m_lstBP.AddItem(99,0,bpData.GetName());
			m_arBP.Add(bpData);
		} else {
			m_lstBP.SetItemText(nItem,0,bpData.GetName());
			m_arBP[nItem-1] = bpData;
		}
	} else if (nItem != 0 && Ans == IDC_REMOVE) {
		m_lstBP.DeleteItem(nItem);
		m_arBP.RemoveAt(nItem-1);
	}

  //050418 - при добавлении нового канала включаю бипол€ры
  if (nItem == 0 && Ans == IDOK)
  {
    m_bBiPol = true;
    CheckDlgButton(IDC_BIPOLAR,1);
  }

	GetDlgItem(IDC_BIPOLAR).EnableWindow(m_arBP.GetSize() > 0);
	m_pView->PostMessage(WM_DIZ_MAPCHANGED);

	return 0;
}


LRESULT CDlgView::OnBnClickedClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
		HWND hW = GetTopLevelWindow();
		
		::PostMessage(hW,WM_DIZ_VIEW_CLOSE,0,(DWORD)m_pView);
	return 0;
}


//040703 - перенесена из хедера
void CDlgView::CorrectMap()
{
	// 040703 
	// удаление векторов
	// стара€ верси€ состо€ла только из этих двух строк
	m_mapDisp.RedrawWindow();
	m_mapRef.RedrawWindow();

	//050208 - вместо списков просто счетчики
	m_nRef = 0;
	m_nDisp = 0;
	
	for (int chan =0; chan < 9*32; chan++)
	{
		if(m_mapDisp.GetCell(chan) ) 
			m_nDisp++;
		if(m_mapRef.GetCell(chan) ) 
			m_nRef++;

	}

}


BOOL CDlgView::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID)
{
	DDX_FLOAT(IDC_SCALE,m_flScale);
	DDX_CHECK(IDC_TRIGGER,m_bTrigger);
	DDX_CHECK(IDC_BIPOLAR,m_bBiPol);

	GetDlgItem(IDC_BIPOLAR).EnableWindow(m_arBP.GetSize() > 0);
  if (!bSaveAndValidate)
  {
	  m_cmbFLP.SetCurSel(m_nFLP);
	  m_cmbFHP.SetCurSel(m_nFHP);
  }

  DDX_FLOAT(IDC_BPSCALE,m_flBPScale);

  return false;
}


BOOL CDlgView::EditUpdate(int idFocus, double flValue)
{
double flOldValue;
double flNewValue;
int nOldValue;

double *pflScale;
int *pnUnit;
int *pScalePnt;
UINT nIDunit;


	if (idFocus == IDC_SCALE)
	{
    pflScale = &m_flScale;
    pnUnit = &m_nScaleUnit;
    pScalePnt = &m_nScalePnt;
    nIDunit = IDC_SCALEUNIT;
  }
  else if (idFocus == IDC_BPSCALE)
  {
    pflScale = &m_flBPScale;
    pnUnit = &m_nBPScaleUnit;
    pScalePnt = &m_nBPScalePnt;
    nIDunit = IDC_BPSCALEUNIT;
  }
  else
    return TRUE;


  try
	{
			flOldValue = *pflScale;

			if (flValue < 0)
			{
				if(!DDX_Float(idFocus, flNewValue, true)) 
					throw(1);
			}
			else
				flNewValue = flValue;


			if (flNewValue <= 0)
				throw(1);

			//chage from uV to mV
			if (flNewValue >= 1000 && *pnUnit == 1)
			{
				*pflScale = Round(flNewValue/1000,0);
				*pnUnit = 1000;
				SetDlgItemText(nIDunit,"mV");
				DDX_Float(idFocus, *pflScale, false);
			
			} 
			else if (flNewValue < 1 && *pnUnit == 1000)
			{
				*pflScale = flNewValue*1000;
				*pnUnit = 1;
				SetDlgItemText(nIDunit,"uV");
				DDX_Float(idFocus, *pflScale, false);
			} else if (flNewValue < 1 && *pnUnit == 1)
				throw(1);
			else
				*pflScale = flNewValue;
			//calculate points	


			*pScalePnt = (int)(*pflScale* *pnUnit*32);
			*pflScale = Round(*pflScale,2);
			flNewValue = *pflScale;
		
			DDX_Float(idFocus, flNewValue, false);
			((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
			return TRUE;
		//CString str;
		//str.Format("%.2f",m_flScale);
		//SetDlgItemText(idFocus,str);

	}
	catch(int a)
	{
    if (a)
    {
		  DDX_Float(idFocus, flOldValue, false);
		  ((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
		  return FALSE;
    }
    else 
    {
		  DDX_Int(idFocus, nOldValue, true, false);
		  ((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
		  return FALSE;
    }
	}



	return FALSE;
}

LRESULT CDlgView::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{



	return 0;
}

LRESULT CDlgView::OnDeltaposSscale(int idCtrl, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	int iDelta(pNMUpDown->iDelta);

  double flScale;
  int nUnit;
  UINT nID;

  if (idCtrl == IDC_SSCALE)
  {
    flScale = m_flScale;
    nUnit = m_nScaleUnit;
    nID = IDC_SCALE;
  }
  else if (idCtrl == IDC_SBPSCALE)
  {
    flScale = m_flBPScale;
    nUnit = m_nBPScaleUnit;
    nID = IDC_BPSCALE;
  }
  else
    return E_FAIL;


	if (flScale+iDelta > 10 && fmod(flScale,10) == 0 ) 
		iDelta *= 10;
	double flV = Round(flScale + iDelta,0);
	if (flV <= 0) flV = 0.9;
	if (flV >= 500 && nUnit == 1000 && iDelta > 0)
		flV = 500;

	EditUpdate(nID,flV);

	return 1;
}

LRESULT CDlgView::OnBnClickedTrigger(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DDX_Check(IDC_TRIGGER, m_bTrigger, true);
  //if (m_nType == 1) 	
  m_pView->PostMessage(WM_DIZ_MAPCHANGED);

  return 0;
}

LRESULT CDlgView::OnBnClickedBipolar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DDX_Check(IDC_BIPOLAR, m_bBiPol, true);
	m_pView->PostMessage(WM_DIZ_MAPCHANGED);

	return 0;
}


CDlgView::~CDlgView() 
{ 

  ATLTRACE("   ~CDlgView: %8X (%s)\n", GetCurrentThreadId(), m_strSection);

  m_pView->PostMessage(WM_DIZ_SHOWMESS,0,(LPARAM)m_pFrame->m_hWnd);
  m_pView->Kill_Timer();
  if (m_bSaveOnClose)
  {
    SaveSettings(0);
    ATLTRACE("~CDlgView: SaveSettings\n");
  }
  //m_pView->ShowWindow(SW_HIDE);
  Sleep(100);
  if (!m_pView->IsChild())
  {
    m_pView->SetParent(m_pFrame->m_hRightUp);
    m_pView->ModifyStyle(0,WS_CHILD);
  }
  ::PostThreadMessage((DWORD)m_pThread->m_idThread,WM_QUIT,0,0 );
  
  
  ATLTRACE("~CDlgView: Waiting for thread end... %x %x (%s)\n", GetCurrentThreadId(), m_pThread->m_idThread, m_strSection);
  while (!m_pThread->WaitForThread(200) )
  {
    ATLTRACE("Ќе дождались, терминируем %x %x %X\n", GetCurrentThreadId(), m_pThread->m_idThread, m_pThread);
    Sleep(50);
    //WaitForSingleObject(m_pThread->m_hThread,5000);
    TerminateThread(m_pThread->m_hThread,12);
  }

	delete m_pThread;
  
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	if(pLoop != NULL) //ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
	if (m_hWnd) DestroyWindow(); 
  //ATLTRACE("~CDlgView: Destroy %x %x\n", GetCurrentThreadId(), m_pThread->m_idThread);
} 

LRESULT CDlgView::OnCmbFLP(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_nFLP = m_cmbFLP.GetCurSel();
	m_pView->PostMessage(WM_DIZ_CFCHANGED,0);
	return 0;
}

LRESULT CDlgView::OnCmbFHP(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	
  m_nFHP = m_cmbFHP.GetCurSel();
	m_pView->PostMessage(WM_DIZ_CFCHANGED,1);
	return 0;
}

 
fltCf2 CDlgView::GetCfLP()
{
fltCf2 fltCf0; //пустой фильтр

static	fltCf2 Cf[64] = 
{
/////////////////////////// 128 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.99297607419000,   1.98595214838000,   0.99297607419000,   1.,   1.98591918065538,   0.98598511610462),
/*1kHz*/	fltCf2(0.93263559578955,   1.86527119157910,   0.93263559578955,   1.,   1.86217475508976,   0.86836762806843),
/*100Hz*/	fltCf2(0.53427205262961,   1.06854410525922,   0.53427205262961,   1.,   0.89116083933384,   0.24592737118460),
/*50Hz*/	fltCf2(0.32515878919599,   0.65031757839199,   0.32515878919599,   1.,   0.21849466953463,   0.08214048724934),
/*40Hz*/	fltCf2(0.26303458787048,   0.52606917574096,   0.26303458787048,   1.,  -0.01974280827259,   0.07188115975451), 
/*30Hz*/  fltCf2(0.19242490575099,   0.38484981150198,   0.19242490575099,   1.,  -0.32500407202038,   0.09470369502434),
/*20Hz*/  fltCf2(0.11522568442937,   0.23045136885874,   0.11522568442937,   1.,  -0.72594847097801,   0.18685120869549),
/*10Hz*/	fltCf2(0.04055565696671,   0.08111131393341,   0.04055565696671,   1.,  -1.26537402983408,   0.42759665770090),

/////////////////////////// 256 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.98601785119091,   1.97203570238182,   0.98601785119091,   1.,   1.97190475556112,   0.97216664920252),
/*1kHz*/	fltCf2(0.87124681289589,   1.74249362579179,   0.87124681289589,   1.,   1.73092314559632,   0.75406410598725),
/*100Hz*/	fltCf2(0.32515878919599,   0.65031757839199,   0.32515878919599,   1.,   0.21849466953463,   0.08214048724934),
/*50Hz*/	fltCf2(0.15434771312679,   0.30869542625358,   0.15434771312679,   1.,  -0.51122248059674,   0.12861333310390),
/*40Hz*/	fltCf2(0.11522568442937,   0.23045136885874,   0.11522568442937,   1.,  -0.72594847097801,   0.18685120869549), 
/*30Hz*/  fltCf2(0.07643707427303,   0.15287414854606,   0.07643707427303,   1.,  -0.97502878701287,   0.28077708410499),
/*20Hz*/  fltCf2(0.04055565696671,   0.08111131393341,   0.04055565696671,   1.,  -1.26537402983408,   0.42759665770090),
/*10Hz*/	fltCf2(0.01226754976932,   0.02453509953864,   0.01226754976932,   1.,  -1.60464092213351,   0.65371112121079),

/////////////////////////// 512 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.97229574831451,   1.94459149662901,   0.97229574831451,   1.,   1.94407499873044,   0.94510799452759),
/*1kHz*/	fltCf2(0.76403466654262,   1.52806933308525,   0.76403466654263,   1.,   1.48748268026986,   0.56865598590064),
/*100Hz*/	fltCf2(0.15434771312679,   0.30869542625358,   0.15434771312679,   1.,  -0.51122248059674,   0.12861333310390),
/*50Hz*/	fltCf2(0.05790420131881,   0.11580840263762,   0.05790420131881,   1.,  -1.11457436316751,   0.34619116844275),
/*40Hz*/	fltCf2(0.04055565696671,   0.08111131393341,   0.04055565696671,   1.,  -1.26537402983408,   0.42759665770090), 
/*30Hz*/  fltCf2(0.02504929687820,   0.05009859375639,   0.02504929687820,   1.,  -1.42840778557911,   0.52860497309190),
/*20Hz*/  fltCf2(0.01226754976932,   0.02453509953864,   0.01226754976932,   1.,  -1.60464092213351,   0.65371112121079),
/*10Hz*/	fltCf2(0.00339172247919,   0.00678344495838,   0.00339172247919,   1.,  -1.79494975822113,   0.80851664813789),

/////////////////////////// 1024 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.94560992735054,   1.89121985470107,   0.94560992735054,   1.,   1.88921056671974,   0.89322914268241),
/*1kHz*/	fltCf2(0.59851433765218,   1.19702867530437,   0.59851433765218,   1.,   1.06985280388875,   0.32420454671998),
/*100Hz*/	fltCf2(0.05790420131881,   0.11580840263762,   0.05790420131881,   1.,  -1.11457436316751,   0.34619116844275),
/*50Hz*/	fltCf2(0.01825203863219,   0.03650407726439,   0.01825203863219,   1.,  -1.51481643229354,   0.58782458682231),
/*40Hz*/	fltCf2(0.01226754976932,   0.02453509953864,   0.01226754976932,   1.,  -1.60464092213351,   0.65371112121079), 
/*30Hz*/  fltCf2(0.00725339747392,   0.01450679494785,   0.00725339747392,   1.,  -1.69798749009301,   0.72700107998870),
/*20Hz*/  fltCf2(0.00339172247919,   0.00678344495838,   0.00339172247919,   1.,  -1.79494975822113,   0.80851664813789),
/*10Hz*/	fltCf2(0.00089294827662,   0.00178589655323,   0.00089294827662,   1.,  -1.89560349019281,   0.89917528329928),

/////////////////////////// 2048 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.89512542565900,   1.79025085131800,   0.89512542565900,   1.,   1.78264278923006,   0.79785891340594),
/*1kHz*/	fltCf2(0.39152835629918,   0.78305671259836,   0.39152835629918,   1.,   0.45027965605133,   0.11583376914540),
/*100Hz*/	fltCf2(0.01825203863219,   0.03650407726439,   0.01825203863219,   1.,  -1.51481643229354,   0.58782458682231),
/*50Hz*/	fltCf2(0.00516605856422,   0.01033211712844,   0.00516605856422,   1.,  -1.74601143785673,   0.76667567211360),
/*40Hz*/	fltCf2(0.00339172247919,   0.00678344495838,   0.00339172247919,   1.,  -1.79494975822113,   0.80851664813789), 
/*30Hz*/  fltCf2(0.00195760483435,   0.00391520966869,   0.00195760483435,   1.,  -1.84481132337502,   0.85264174271241),
/*20Hz*/  fltCf2(0.00089294827662,   0.00178589655323,   0.00089294827662,   1.,  -1.89560349019281,   0.89917528329928),
/*10Hz*/	fltCf2(0.00022916696851,   0.00045833393702,   0.00022916696851,   1.,  -1.94733185831205,   0.94824852618609),

/////////////////////////// 4096 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.80461968245396,   1.60923936490793,   0.80461968245396,   1.,   1.58188410797607,   0.63659462183979),
/*1kHz*/	fltCf2(0.20168892128515,   0.40337784257029,   0.20168892128515,   1.,  -0.28231906273513,   0.08907474787572),
/*100Hz*/	fltCf2(0.00516605856422,   0.01033211712844,   0.00516605856422,   1.,  -1.74601143785673,   0.76667567211360),
/*50Hz*/	fltCf2(0.00137718318292,   0.00275436636584,   0.00137718318292,   1.,  -1.87009068739226,   0.87559942012394),
/*40Hz*/	fltCf2(0.00089294827662,   0.00178589655323,   0.00089294827662,   1.,  -1.89560349019281,   0.89917528329928), 
/*30Hz*/  fltCf2(0.00050889587362,   0.00101779174725,   0.00050889587362,   1.,  -1.92135037493573,   0.92338595843022),
/*20Hz*/  fltCf2(0.00022916696851,   0.00045833393702,   0.00022916696851,   1.,  -1.94733185831205,   0.94824852618609),
/*10Hz*/	fltCf2(0.00005805279945,   0.00011610559890,   0.00005805279945,   1.,  -1.97354832139816,   0.97378053259596),

/////////////////////////// 8192 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.65804880404950,   1.31609760809901,   0.65804880404950,   1.,   1.22660889932878,   0.40558631686923),
/*1kHz*/	fltCf2(0.08119495693356,   0.16238991386712,   0.08119495693356,   1.,  -0.94178834919249,   0.26656817692673),
/*100Hz*/	fltCf2(0.00137718318292,   0.00275436636584,   0.00137718318292,   1.,  -1.87009068739226,   0.87559942012394),
/*50Hz*/	fltCf2(0.00035572636244,   0.00071145272489,   0.00035572636244,   1.,  -1.93431176468836,   0.93573467013814),
/*40Hz*/	fltCf2(0.00022916696851,   0.00045833393702,   0.00022916696851,   1.,  -1.94733185831205,   0.94824852618609), 
/*30Hz*/  fltCf2(0.00012975883082,   0.00025951766164,   0.00012975883082,   1.,  -1.96041069913199,   0.96092973445527),
/*20Hz*/  fltCf2(0.00005805279945,   0.00011610559890,   0.00005805279945,   1.,  -1.97354832139816,   0.97378053259596),
/*10Hz*/	fltCf2(0.00001460959994,   0.00002921919988,   0.00001460959994,   1.,  -1.98674474997123,   0.98680318837100),

/////////////////////////// 16384 √ц ////////////////////////////////////////////////
/*10kHz*/	fltCf2(0.45971109361932,   0.91942218723865,   0.45971109361932,   1.,   0.66935591907021,   0.16948845540709),
/*1kHz*/	fltCf2(0.02685921446176,   0.05371842892352,   0.02685921446176,   1.,  -1.40732604435822,   0.51476290220526),
/*100Hz*/	fltCf2(0.00035572636244,   0.00071145272489,   0.00035572636244,   1.,  -1.93431176468836,   0.93573467013814),
/*50Hz*/	fltCf2(0.00009040823990,   0.00018081647979,   0.00009040823990,   1.,  -1.96697216080671,   0.96733379376629),
/*40Hz*/	fltCf2(0.00005805279945,   0.00011610559890,   0.00005805279945,   1.,  -1.97354832139816,   0.97378053259596), 
/*30Hz*/  fltCf2(0.00003276291006,   0.00006552582012,   0.00003276291006,   1.,  -1.98013918371807,   0.98027023535832),
/*20Hz*/  fltCf2(0.00001460959994,   0.00002921919988,   0.00001460959994,   1.,  -1.98674474997123,   0.98680318837100),
/*10Hz*/	fltCf2(0.00000366453017,   0.00000732906033,   0.00000366453017,   1.,  -1.99336502174523,   0.99337967986589)
};

	int i = m_nFLP;

  //050415 - ошибка (или без фильтра) - возвращаем пустой
  if (i <= 0 || i > 8) 
    return fltCf0;
  //050415 - нужен учет частоты
  int nFrq = m_pFrame->GetFrq();

  //индекс частоты - логарифм по основанию 2....
  for (int nFrIdx = 0, nFrBase = 128; nFrBase < 100000; nFrIdx++, nFrBase *= 2)
  {
    if (nFrBase == nFrq) break;
  }

  //ќшибка!!!
  if (nFrIdx > 7) return fltCf0;
	
	return Cf[nFrIdx * 8 + i-1];

}

fltCf1 CDlgView::GetCfHP()
{

	
fltCf1 fltCf0; //пустой фильтр
static	fltCf1 Cf[56] = 
{
//////////////// 128 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99755163991910,  -0.99755163991910,   1.00000000000000,  -0.99510327983821),
/*0.5*/	fltCf1(0.98787692619537,  -0.98787692619537,   1.00000000000000,  -0.97575385239073),
/* 1 */	fltCf1(0.97604426947987,  -0.97604426947987,   1.00000000000000,  -0.95208853895974),
/* 3 */	fltCf1(0.93141864152431,  -0.93141864152431,   1.00000000000000,  -0.86283728304862),
/* 5 */	fltCf1(0.89069524812163,  -0.89069524812163,   1.00000000000000,  -0.78139049624326),
/* 10*/	fltCf1(0.80293106706108,  -0.80293106706108,   1.00000000000000,  -0.60586213412217),
/*100*/	fltCf1(0.28948844913626,  -0.28948844913626,   1.00000000000000,   0.42102310172748),

//////////////// 256 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99877431950596,  -0.99877431950596,   1.00000000000000,  -0.99754863901191),
/*0.5*/	fltCf1(0.99390149679546,  -0.99390149679546,   1.00000000000000,  -0.98780299359092),
/* 1 */	fltCf1(0.98787692619537,  -0.98787692619537,   1.00000000000000,  -0.97575385239073),
/* 3 */	fltCf1(0.96449171764151,  -0.96449171764151,   1.00000000000000,  -0.92898343528303),
/* 5 */	fltCf1(0.94218806442394,  -0.94218806442394,   1.00000000000000,  -0.88437612884788),
/* 10*/	fltCf1(0.89069524812163,  -0.89069524812163,   1.00000000000000,  -0.78139049624326),
/*100*/	fltCf1(0.44899735136080,  -0.44899735136080,   1.00000000000000,   0.10200529727840),

//////////////// 512 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99938678394950,  -0.99938678394950,   1.00000000000000,  -0.99877356789900),
/*0.5*/	fltCf1(0.99694142202394,  -0.99694142202394,   1.00000000000000,  -0.99388284404787),
/* 1 */	fltCf1(0.99390149679546,  -0.99390149679546,   1.00000000000000,  -0.98780299359092),
/* 3 */	fltCf1(0.98192495186433,  -0.98192495186433,   1.00000000000000,  -0.96384990372867),
/* 5 */	fltCf1(0.97023360577947,  -0.97023360577947,   1.00000000000000,  -0.94046721155893),
/* 10*/	fltCf1(0.94218806442394,  -0.94218806442394,   1.00000000000000,  -0.88437612884788),
/*100*/	fltCf1(0.61973522717503,  -0.61973522717503,   1.00000000000000,  -0.23947045435005),

//////////////// 1024 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99969329793744,  -0.99969329793744,   1.00000000000000,  -0.99938659587487),
/*0.5*/	fltCf1(0.99846836870509,  -0.99846836870509,   1.00000000000000,  -0.99693673741019),
/* 1 */	fltCf1(0.99694142202394,  -0.99694142202394,   1.00000000000000,  -0.99388284404787),
/* 3 */	fltCf1(0.99088005420253,  -0.99088005420253,   1.00000000000000,  -0.98176010840505),
/* 5 */	fltCf1(0.98489194675534,  -0.98489194675534,   1.00000000000000,  -0.96978389351068),
/* 10*/	fltCf1(0.97023360577947,  -0.97023360577947,   1.00000000000000,  -0.94046721155893),
/*100*/	fltCf1(0.76523028798467,  -0.76523028798467,   1.00000000000000,  -0.53046057596933),

//////////////// 2048 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99984662544857,  -0.99984662544857,   1.00000000000000,  -0.99969325089715),
/*0.5*/	fltCf1(0.99923359742947,  -0.99923359742947,   1.00000000000000,  -0.99846719485893),
/* 1 */	fltCf1(0.99846836870509,  -0.99846836870509,   1.00000000000000,  -0.99693673741019),
/* 3 */	fltCf1(0.99541913849696,  -0.99541913849696,   1.00000000000000,  -0.99083827699391),
/* 5 */	fltCf1(0.99238847572063,  -0.99238847572063,   1.00000000000000,  -0.98477695144125),
/* 10*/	fltCf1(0.98489194675534,  -0.98489194675534,   1.00000000000000,  -0.96978389351068),
/*100*/	fltCf1(0.86700335156646,  -0.86700335156646,   1.00000000000000,  -0.73400670313292),

//////////////// 4096 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99992330684290,  -0.99992330684290,   1.00000000000000,  -0.99984661368579),
/*0.5*/	fltCf1(0.99961665181522,  -0.99961665181522,   1.00000000000000,  -0.99923330363043),
/* 1 */	fltCf1(0.99923359742947,  -0.99923359742947,   1.00000000000000,  -0.99846719485893),
/* 3 */	fltCf1(0.99770431113210,  -0.99770431113210,   1.00000000000000,  -0.99540862226420),
/* 5 */	fltCf1(0.99617969870227,  -0.99617969870227,   1.00000000000000,  -0.99235939740455),
/* 10*/	fltCf1(0.99238847572063,  -0.99238847572063,   1.00000000000000,  -0.98477695144125),
/*100*/	fltCf1(0.92876464398312,  -0.92876464398312,   1.00000000000000,  -0.85752928796624),

//////////////// 8192 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99996165195093,  -0.99996165195093,   1.00000000000000,  -0.99992330390186),
/*0.5*/	fltCf1(0.99980828916161,  -0.99980828916161,   1.00000000000000,  -0.99961657832321),
/* 1 */	fltCf1(0.99961665181522,  -0.99961665181522,   1.00000000000000,  -0.99923330363043),
/* 3 */	fltCf1(0.99885083650513,  -0.99885083650513,   1.00000000000000,  -0.99770167301026),
/* 5 */	fltCf1(0.99808619369278,  -0.99808619369278,   1.00000000000000,  -0.99617238738555),
/* 10*/	fltCf1(0.99617969870227,  -0.99617969870227,   1.00000000000000,  -0.99235939740455),
/*100*/	fltCf1(0.96306684890813,  -0.96306684890813,   1.00000000000000,  -0.92613369781627),

//////////////// 16384 √ц ////////////////////////////////////////////////////
/*0.1*/	fltCf1(0.99998082560782,  -0.99998082560782,   1.00000000000000,  -0.99996165121563),
/*0.5*/	fltCf1(0.99990413539166,  -0.99990413539166,   1.00000000000000,  -0.99980827078332),
/* 1 */	fltCf1(0.99980828916161,  -0.99980828916161,   1.00000000000000,  -0.99961657832321),
/* 3 */	fltCf1(0.99942508791858,  -0.99942508791858,   1.00000000000000,  -0.99885017583715),
/* 5 */	fltCf1(0.99904218030570,  -0.99904218030570,   1.00000000000000,  -0.99808436061140),
/* 10*/	fltCf1(0.99808619369278,  -0.99808619369278,   1.00000000000000,  -0.99617238738555),
/*100*/	fltCf1(0.98118599419454,  -0.98118599419454,   1.00000000000000,  -0.96237198838908)
};

	int i = m_nFHP;

  //050415 - возвращаем нулевой фильтр
  if (i <= 0 || i > 7) 
    return fltCf0;
	
  //050415 - нужен учет частоты
  int nFrq = m_pFrame->GetFrq();

  //индекс частоты - логарифм по основанию 2....
  for (int nFrIdx = 0, nFrBase = 128; nFrBase < 100000; nFrIdx++, nFrBase *= 2)
  {
    if (nFrBase == nFrq) break;
  }

  //ќшибка!!!
  if (nFrIdx > 7) return fltCf0;

	return Cf[nFrIdx * 7 + i-1];

}

LRESULT CDlgView::OnMapChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
    CorrectMap();
		m_pView->PostMessage(WM_DIZ_MAPCHANGED);
    return 0;


}


void CDlgView::SetViewTitle()
{
  m_pView->SetWindowText(m_strSection + " " + m_strName + "[" + m_pFrame->GetTitle() + "]");
}



LRESULT CDlgView::OnBnClickedTool(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CViewSettDlg dlg;
  dlg.m_strName = m_strName;
  dlg.m_nTimer = m_nTimer;
  dlg.m_bSaveOnClose = m_bSaveOnClose;

  bool bSave(false);
  int nAns = dlg.DoModal();

  //ƒва свитча подр€д, так как и по командам сохранени€ € должен
  // переписать измененные в диалоге значени€ в свой класс,
  // а потом уже думать - сохран€ть или нет
  switch (nAns)
  {
  case IDC_LOADDEF:
    LoadSettings(true, "DEFAULT");
    SendMessage(WM_DIZ_MAPCHANGED); //CorrectMap();
    RedrawWindow();
    break;
  case IDC_SAVENOW:
  case IDC_SAVEDEF:
  case IDOK:
    m_bSaveOnClose = dlg.m_bSaveOnClose;
    m_strName = dlg.m_strName;
    SetViewTitle();
    if (dlg.m_nTimer != m_nTimer)
    {
      m_nTimer = dlg.m_nTimer;
      //m_pView->ChangeTimer(m_nTimer);
      m_pView->PostMessage(WM_DIZ_CHANGETIMER,(WPARAM)m_nTimer,0);
    }
    break;
  default:
    break;
  }

  switch (nAns)
  {
  case IDC_SAVENOW:
    SaveSettings(0);  //save now
    break;
  case IDC_SAVEDEF:
    SaveSettings("DEFAULT");
    break;
  default:
    break;
  }

  return 0;
}

//050503 - загрузка параметров из именованной секции
bool CDlgView::LoadSettings(bool bFulLoad, LPCTSTR sSect)
{
	//bFulLoad = 0;
	if (sSect == 0)
  {
    if (_Settings.GetWinLoad() == 1)
      sSect = m_strSection;
    else
      sSect = "DEFAULT";
  }

  //050427 - чтение параметров (будет - через иф)

  //040710 - default scale settings
	m_nScaleUnit = _Settings.GetInt(sSect,"ScaleUnit",1);  //1;	//uV
	m_flScale = (double)_Settings.GetInt(sSect,"Scale",20); //20;	// 20uV
  m_nScalePnt = (int)(m_flScale*m_nScaleUnit*32);
  if (m_nScaleUnit == 1000) SetDlgItemText(IDC_SCALEUNIT,"mV");


  //050805 - default BP scale settings
	m_nBPScaleUnit = _Settings.GetInt(sSect,"BPScaleUnit",1);  //1;	//uV
	m_flBPScale = (double)_Settings.GetInt(sSect,"BPScale",100); //20;	// 20uV
  m_nBPScalePnt = (int)(m_flBPScale*m_nBPScaleUnit*32);
  if (m_nBPScaleUnit == 1000) SetDlgItemText(IDC_BPSCALEUNIT,"mV");


  m_nFLP = _Settings.GetInt(sSect,"FLP",0);
  m_nFHP = _Settings.GetInt(sSect,"FHP",0);
  m_bTrigger = _Settings.GetInt(sSect,"bTrigger",1);
  m_bBiPol = _Settings.GetInt(sSect,"bBiPol",0);

  //050503
  m_strName = _Settings.GetString(sSect, "OscName");

  //050504
  m_bSaveOnClose = _Settings.GetInt(sSect,"SaveOnClose",1);


  BOOL bRet(true);

  if (bFulLoad)
  {
    int nBp;
    CString str;
    _BiPol bpTmp;

    try 
    {
      //050428 - GetStruct будет выбрасывать пустое исключение
      bRet = _Settings.GetStruct(sSect, "DispMap", m_mapDisp.GetMap(), m_mapDisp.GetMapSize());
      bRet = _Settings.GetStruct(sSect, "RefMap", m_mapRef.GetMap(), m_mapRef.GetMapSize());

      m_arBP.RemoveAll();
      m_lstBP.DeleteAllItems();
    	m_lstBP.InsertItem(0,"<Add new>");
      nBp = _Settings.GetInt(sSect,"BiPolNum",0);
      for (int i = 0; i < nBp; i++)
      {
        str.Format("Bp%03d",i+1);
        _Settings.GetStruct(sSect, str, &bpTmp, sizeof(bpTmp));
        m_arBP.Add(bpTmp);
		    m_lstBP.AddItem(99,0,bpTmp.GetName());
      }
      if (nBp <= 0)
        m_bBiPol = false;
  	  GetDlgItem(IDC_BIPOLAR).EnableWindow(m_arBP.GetSize() > 0);

      RECT rcIn;
      RECT rcOut;
      bool bChild;
      if (m_pView)
      {
        bChild = (_Settings.GetInt(sSect,"bChild",1) != 0);
        _Settings.GetStruct(sSect, "rcIn", &rcIn, sizeof(rcIn));
        _Settings.GetStruct(sSect, "rcOut", &rcOut, sizeof(rcOut));
        m_pView->SetPosition(&rcIn, &rcOut, bChild);
      }

      if (bRet)
      SendMessage(WM_DIZ_MAPCHANGED); //CorrectMap();

     
    }
    catch(int I)
    {
      I;
			//MessageBox("LoadSett");
      bRet = false;
    }
  }


  return (bRet != 0);

}

void CDlgView::SaveSettings(LPCTSTR sSect)
{
  if (sSect == 0)
    sSect = m_strSection;

	_Settings.WriteValue(sSect,"ScaleUnit",m_nScaleUnit);  //1;	//uV
	_Settings.WriteValue(sSect,"Scale",(int)m_flScale); //20;	// 20uV
  _Settings.WriteValue(sSect,"FLP", m_nFLP);
  _Settings.WriteValue(sSect,"FHP", m_nFHP);
  _Settings.WriteValue(sSect,"bTrigger",m_bTrigger);
  _Settings.WriteValue(sSect,"bBiPol",m_bBiPol);
  _Settings.WriteValue(sSect,"SaveOnClose",m_bSaveOnClose);

  _Settings.WriteStruct(sSect, "DispMap", m_mapDisp.GetMap(), m_mapDisp.GetMapSize());
  _Settings.WriteStruct(sSect, "RefMap", m_mapRef.GetMap(), m_mapRef.GetMapSize());

  _BiPol bpTmp;
  CString str;
  int  nBp = m_arBP.GetSize();
  _Settings.WriteValue(sSect,"BiPolNum",nBp);
  for (int i = 0; i < nBp; i++)
  {
    str.Format("Bp%03d",i+1);
    _Settings.WriteStruct(sSect, str, &m_arBP[i], sizeof(bpTmp));
  }

  _Settings.WriteValue(sSect, "OscName", m_strName); 
  _Settings.WriteValue(sSect, "Timer", m_nTimer); 

  RECT rcIn;
  RECT rcOut;
  bool bChild;
  if (m_pView)
  {
    bChild = (m_pView->GetPosition(&rcIn, &rcOut) != 0);
    _Settings.WriteValue(sSect,"bChild",bChild);
    _Settings.WriteStruct(sSect, "rcIn", &rcIn, sizeof(rcIn));
    _Settings.WriteStruct(sSect, "rcOut", &rcOut, sizeof(rcOut));
  }


  //050805 - отдельные масшатабы дл€ бипол€ров
	_Settings.WriteValue(sSect,"BPScaleUnit",m_nBPScaleUnit);  //1;	//uV
	_Settings.WriteValue(sSect,"BPScale",(int)m_flBPScale); //20;	// 20uV



}

//////////////// 
// COscDlg
LRESULT COscDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{


	SetWindowText(_T("Dialog View"));
  m_strSection.Format("Osc %d",m_nNumber+1);
        

	m_udDur.Attach(GetDlgItem(IDC_S_LENGTH));
  m_udDur.SetRange(1,999);

  CDlgView::OnInitDialog(uMsg, wParam, lParam, bHandled);

	m_pView = (COscView*)m_pThread->GetView();
  m_pView->PostMessage(WM_DIZ_CHANGETIMER,(WPARAM)m_nTimer,0);
	m_pView->m_pDialog = this;
	m_pView->PostMessage(WM_DIZ_SETSCROLL,(WPARAM)m_bScroll,0);
	m_pView->PostMessage(WM_DIZ_SETFRQ,0,m_nDur); //SetFrq(0);
  m_pView->SetWindowText(m_strSection + " " + m_strName);
  //m_pView->SetWindowText(m_strSection + " " + m_strName + "[" + m_pFrame->GetTitle() + "]");

  DoDataExchange(false);
	return 0;
}


BOOL COscDlg::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID)
{
	DDX_CHECK(IDC_SCROLL,m_bScroll);
  DDX_INT_RANGE(IDC_LENGTH, m_nDur,1,999 );
  CDlgView::DoDataExchange(bSaveAndValidate, nCtlID);
  if (m_pView)
  	m_pView->PostMessage(WM_DIZ_SETSCROLL,(WPARAM)m_bScroll,0);
  return false;
}



//050503 - загрузка параметров из именованной секции
bool COscDlg::LoadSettings(bool bFulLoad, LPCTSTR sSect)
{
	//bFulLoad = 0;
	if (sSect == 0)
  {
    if (_Settings.GetWinLoad() == 1)
      sSect = m_strSection;
    else
      sSect = "DEFAULT";
  }

  //050427 - чтение параметров (будет - через иф)
  m_nDur = _Settings.GetInt(sSect,"Length",8);
  m_nTimer = _Settings.GetInt(sSect, "Timer", 20);
  m_bScroll = _Settings.GetInt(sSect,"bScroll",0);

  BOOL bRet= CDlgView::LoadSettings(bFulLoad, sSect);


  DoDataExchange(false);
  if (m_pView)   	
		m_pView->PostMessage(WM_DIZ_SETFRQ,0,m_nDur); //SetFrq(0);


  return (bRet != 0);

}

void COscDlg::SaveSettings(LPCTSTR sSect)
{
  if (sSect == 0)
    sSect = m_strSection;

  _Settings.WriteValue(sSect,"bScroll",m_bScroll);
  _Settings.WriteValue(sSect,"Length",m_nDur);
  _Settings.WriteValue(sSect, "Timer", m_nTimer); 
  CDlgView::SaveSettings(sSect);

}


BOOL COscDlg::EditUpdate(int idFocus, double flValue)
{
int nOldValue;
int nNewValue;

  if (idFocus == IDC_LENGTH)
  {
    nOldValue = m_nDur;
		if(!DDX_Int(idFocus, nNewValue, true,true,true,1,999) ) 
    {
		  DDX_Int(idFocus, nOldValue, true, false);
		  ((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
		  return FALSE;
    }
    m_nDur = nNewValue;
    DDX_Int(idFocus, nNewValue, true, false);
		((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
    m_pView->PostMessage(WM_DIZ_SETFRQ,0,m_nDur); //SetFrq(0);
  	return TRUE;
  }
  else
    return CDlgView::EditUpdate(idFocus, flValue);

}


HWND COscDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{

		ATLASSERT(m_hWnd == NULL);
		_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< CWindow >*)this);
#ifdef _DEBUG
		m_bModal = false;
#endif //_DEBUG
		HWND hWnd = ::CreateDialogParam(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDD),
					hWndParent, COscDlg::StartDialogProc, dwInitParam);
		ATLASSERT(m_hWnd == hWnd);
		return hWnd;

}

LRESULT COscDlg::OnDeltaposSLength(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  m_nDur += pNMUpDown->iDelta;
  if (m_nDur < 1) m_nDur = 1;
  if (m_nDur > 999) m_nDur = 999;
  DDX_Int(IDC_LENGTH, m_nDur, true, false);
  EditUpdate(IDC_LENGTH);
  return 0;
}

LRESULT COscDlg::OnBnClickedScroll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DDX_Check(IDC_SCROLL, m_bScroll, true);
	m_pView->PostMessage(WM_DIZ_SETSCROLL,(WPARAM)m_bScroll,0);
	return 0;
}

LRESULT COscDlg::OnBnClickedZero(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pView->PostMessage(WM_DIZ_SETZERO,0,0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////
// CSnapDlg
LRESULT CSnapDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{


	SetWindowText(_T("Dialog View"));
        


  CDlgView::OnInitDialog(uMsg, wParam, lParam, bHandled);

	m_pView = (CSnapView*)m_pThread->GetView();
	m_pView->m_pDialog = this;
  m_strSection.Format("Snap %d" ,m_nNumber+1);
  SetViewTitle();
	m_udTrNum.Attach(GetDlgItem(IDC_STRNUM));
	m_udTrNum.SetBuddy(GetDlgItem(IDC_TRNUM));
	m_udTrNum.SetRange(1,1);

	m_bmpLock.LoadBitmap(MAKEINTRESOURCE(IDB_LOCK));
	m_bmpUnLock.LoadBitmap(MAKEINTRESOURCE(IDB_UNLOCK));
  ((CButton)GetDlgItem(IDC_LOCK)).SetBitmap(m_bmpLock);


  //050701 - не разрешаю анлочить первый снэпшот
  if (m_nNumber == 0)
    GetDlgItem(IDC_LOCK).EnableWindow(false);

  DoDataExchange(false);
	return 0;
}


BOOL CSnapDlg::DoDataExchange(BOOL bSaveAndValidate, UINT nCtlID)
{
  if (m_nRecNum > 0 && (nCtlID == IDC_TRNUM || nCtlID == -1))
  {
    int nOldTr = m_nTrNum;
    if (FALSE == DDX_Int(IDC_TRNUM,m_nTrNum,0,bSaveAndValidate,true,1,m_nRecNum))
    {
      m_nTrNum = nOldTr;
      SetDlgItemInt(IDC_TRNUM,m_nTrNum);
    }
    //else m_pView->PlotRecord(m_nTrNum-1);
  }
  CDlgView::DoDataExchange(bSaveAndValidate, nCtlID);
  return false;
}



//050503 - загрузка параметров из именованной секции
bool CSnapDlg::LoadSettings(bool bFulLoad, LPCTSTR sSect)
{
	//bFulLoad = 0;
	if (sSect == 0)
  {
    if (_Settings.GetWinLoad() == 1)
      sSect = m_strSection;
    else
      sSect = "DEFAULT";
  }


  BOOL bRet= CDlgView::LoadSettings(bFulLoad, sSect);


  DoDataExchange(false);

  return (bRet != 0);

}

void CSnapDlg::SaveSettings(LPCTSTR sSect)
{
  if (sSect == 0)
    sSect = m_strSection;

  CDlgView::SaveSettings(sSect);

}


BOOL CSnapDlg::EditUpdate(int idFocus, double flValue)
{
int nOldValue;
int nNewValue;
BOOL bRet(false);

  if (idFocus == IDC_TRNUM)
  {
    nOldValue = m_nTrNum;
		if(!DDX_Int(idFocus, nNewValue,true ,true,true,1,m_nRecNum) ) 
    {
      SetDlgItemInt(IDC_TRNUM,nOldValue);
		  ((CEdit)GetDlgItem(idFocus)).SetSel(0,-1);
		  return FALSE;
    }
    if (nNewValue < 1 || nNewValue > m_nRecNum)
      nNewValue = nOldValue;
    else
      bRet = true;

    _GoToRec(nNewValue);

    //m_pView->PostMessage(WM_DIZ_SETFRQ,0,m_nDur); //SetFrq(0);
  	return bRet;
  }
  else
    bRet = CDlgView::EditUpdate(idFocus, flValue);
	m_pView->PostMessage(WM_DIZ_MAPCHANGED);

  return bRet;

}


HWND CSnapDlg::Create(HWND hWndParent, LPARAM dwInitParam)
{

		ATLASSERT(m_hWnd == NULL);
		_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT< CWindow >*)this);
#ifdef _DEBUG
		m_bModal = false;
#endif //_DEBUG
		HWND hWnd = ::CreateDialogParam(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDD),
					hWndParent, COscDlg::StartDialogProc, dwInitParam);
		ATLASSERT(m_hWnd == hWnd);
    FillMemory(m_cOffMap, TOTCHANS, -1);
		return hWnd;

}

LRESULT CSnapDlg::OnBnClickedTrprev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  ChangeRec(VK_LEFT);
  return 0;
}

LRESULT CSnapDlg::OnBnClickedTrfirst(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  ChangeRec(VK_HOME);
  return 0;
}

LRESULT CSnapDlg::OnBnClickedTrnext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  ChangeRec(VK_RIGHT);
  return 0;
}

LRESULT CSnapDlg::OnBnClickedTrlast(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  ChangeRec(VK_END);
  return 0;
}

//Private function!!!
bool CSnapDlg::_GoToRec(int nRec)
{
  if (nRec > 0 && nRec <= m_nRecNum)
  {
    m_pFrame->PostMessage(WM_DIZ_READFRAME,nRec);
    return true;
  }
  else
    return false;
}


bool CSnapDlg::GoToRec(int nRec)
{
  if (nRec > 0 && nRec <= m_nRecNum)
  {
    m_nTrNum = nRec;
    m_pView->PlotRecord(m_nTrNum-1);
    SetDlgItemInt(IDC_TRNUM,m_nTrNum);
	  ((CEdit)GetDlgItem(IDC_TRNUM)).SetSel(0,-1);
    return true;
  }
  else
    return false;
}

bool CSnapDlg::ChangeRec(DWORD vKey)
{
  switch(vKey)
  {
  case VK_HOME:
    if (m_nRecNum > 0 && m_nTrNum > 1)
      _GoToRec(1);
    break;
  case VK_END:
    if (m_nRecNum > 0 && m_nTrNum < m_nRecNum)
    {
      _GoToRec(m_nRecNum);
    }
    break;
	case VK_LEFT:
		if (m_nRecNum > 0 && m_nTrNum > 1)
    {
      _GoToRec(m_nTrNum-1);
    }
    break;
	case VK_RIGHT:
    if (m_nRecNum > 0 && m_nTrNum < m_nRecNum)
    {
      _GoToRec(m_nTrNum+1);
    }
    break;
  case VK_NEXT:
    if (m_nRecNum > 0 && m_nTrNum < m_nRecNum)
    {
      m_nTrNum += 8;
			if (m_nTrNum > m_nRecNum)
				m_nTrNum = m_nRecNum;
      _GoToRec(m_nTrNum);
    }
    break;
  case VK_PRIOR:
		if (m_nRecNum > 0 && m_nTrNum > 1)
    {
      m_nTrNum -= 8;
			if (m_nTrNum < 1)
				m_nTrNum = 1;
      _GoToRec(m_nTrNum);
    }
    break;
  default:
    return false;
  }

  return true;
}

LRESULT CSnapDlg::OnDeltaposStrnum(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
	LPNMUPDOWN pUD = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	if (pUD->iDelta > 0)
		ChangeRec(VK_LEFT);
	else if (pUD->iDelta < 0)
		ChangeRec(VK_RIGHT);
	else
		return 0;

	return 1;

}

LRESULT CSnapDlg::OnBnClickedLock(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  m_bUnLock = (IsDlgButtonChecked(IDC_LOCK) != 0);
  if (m_bUnLock)
    ((CButton)GetDlgItem(IDC_LOCK)).SetBitmap(m_bmpUnLock);
  else
    ((CButton)GetDlgItem(IDC_LOCK)).SetBitmap(m_bmpLock);


  return 0;
}
