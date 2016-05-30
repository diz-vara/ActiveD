// AcqTrigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActD.h"
#include "AcqTrigDlg.h"

//static members - arrays of controls

static UINT idEdit[] =
{
  IDC_MAINTIMER, IDC_FRAMELEN, IDC_PRETRIG, IDC_POSTTRIG, IDC_TRIDLE
};

static UINT idMode[] = 
{
  IDC_GATED, IDC_TRIGGERED
};
//AND buttons
static UINT idAnd[] = 
{
  IDC_AND1, IDC_AND2, IDC_AND3
};


//first group - trigger controls
//STATIC
static UINT idTrTimeCtrl[] = 
{
  IDC_TST0, IDC_PRETRIG, IDC_TST1, IDC_TST2, IDC_POSTTRIG, IDC_TST3, IDC_SFREE, IDC_GATE1, IDC_TRIDLE
};

static UINT idTrStCtrl[] = 
{
  IDC_TST4, IDC_TST5, IDC_TST6, IDC_TSTN, IDC_TST7, IDC_TST8,
  //BIT NAMES
  IDC_TB0, IDC_TB1, IDC_TB2, IDC_TB3, IDC_TB4, IDC_TB5, IDC_TB6, IDC_TB7, 
  IDC_TB8, IDC_TB9, IDC_TBA, IDC_TBB, IDC_TBC, IDC_TBD, IDC_TBE, IDC_TBF,
  //first AND - will be visible only for triggers
  IDC_AND1
};
//LOHI
static UINT idTrLoHi[] = 
{
  IDC_LOHI0, IDC_LOHI1, IDC_LOHI2, IDC_LOHI3, 
  IDC_LOHI4, IDC_LOHI5, IDC_LOHI6, IDC_LOHI7, 
  IDC_LOHI8, IDC_LOHI9, IDC_LOHIA, IDC_LOHIB, 
  IDC_LOHIC, IDC_LOHID, IDC_LOHIE, IDC_LOHIF
};

// HILO
static UINT idTrHiLo[] = 
{
  IDC_HILO0, IDC_HILO1, IDC_HILO2, IDC_HILO3, 
  IDC_HILO4, IDC_HILO5, IDC_HILO6, IDC_HILO7, 
  IDC_HILO8, IDC_HILO9, IDC_HILOA, IDC_HILOB, 
  IDC_HILOC, IDC_HILOD, IDC_HILOE, IDC_HILOF
};
//===============================================================

//===================== First And Group =========================

static UINT idAStCtrl[] = 
{
  IDC_TSA1, IDC_TSA2, IDC_TSA3,
  //BIT NAMES
  IDC_AB0, IDC_AB1, IDC_AB2, IDC_AB3, IDC_AB4, IDC_AB5, IDC_AB6, IDC_AB7, 
  IDC_AB8, IDC_AB9, IDC_ABA, IDC_ABB, IDC_ABC, IDC_ABD, IDC_ABE, IDC_ABF,
  //Second AND - will be visible only with first group
  IDC_AND2
};
//HI
static UINT idAHi[] = 
{
  IDC_HIA0, IDC_HIA1, IDC_HIA2, IDC_HIA3, 
  IDC_HIA4, IDC_HIA5, IDC_HIA6, IDC_HIA7, 
  IDC_HIA8, IDC_HIA9, IDC_HIAA, IDC_HIAB, 
  IDC_HIAC, IDC_HIAD, IDC_HIAE, IDC_HIAF
};

// LO
static UINT idALo[] = 
{
  IDC_LOA0, IDC_LOA1, IDC_LOA2, IDC_LOA3, 
  IDC_LOA4, IDC_LOA5, IDC_LOA6, IDC_LOA7, 
  IDC_LOA8, IDC_LOA9, IDC_LOAA, IDC_LOAB, 
  IDC_LOAC, IDC_LOAD, IDC_LOAE, IDC_LOAF
};
//===============================================================
//===================== Second And Group =========================

static UINT idBStCtrl[] = 
{
  IDC_TSB1, IDC_TSB2, IDC_TSB3,
  //BIT NAMES
  IDC_BB0, IDC_BB1, IDC_BB2, IDC_BB3, IDC_BB4, IDC_BB5, IDC_BB6, IDC_BB7, 
  IDC_BB8, IDC_BB9, IDC_BBA, IDC_BBB, IDC_BBC, IDC_BBD, IDC_BBE, IDC_BBF,
  //Second AND - will be visible only with first group
  IDC_AND3
};
//HI
static UINT idBHi[] = 
{
  IDC_HIB0, IDC_HIB1, IDC_HIB2, IDC_HIB3, 
  IDC_HIB4, IDC_HIB5, IDC_HIB6, IDC_HIB7, 
  IDC_HIB8, IDC_HIB9, IDC_HIBA, IDC_HIBB, 
  IDC_HIBC, IDC_HIBD, IDC_HIBE, IDC_HIBF
};

// LO
static UINT idBLo[] = 
{
  IDC_LOB0, IDC_LOB1, IDC_LOB2, IDC_LOB3, 
  IDC_LOB4, IDC_LOB5, IDC_LOB6, IDC_LOB7, 
  IDC_LOB8, IDC_LOB9, IDC_LOBA, IDC_LOBB, 
  IDC_LOBC, IDC_LOBD, IDC_LOBE, IDC_LOBF
};
//===============================================================

//===================== Third And Group =========================

static UINT idCStCtrl[] = 
{
  IDC_TSC1, IDC_TSC2, IDC_TSC3,
  //BIT NAMES
  IDC_CB0, IDC_CB1, IDC_CB2, IDC_CB3, IDC_CB4, IDC_CB5, IDC_CB6, IDC_CB7, 
  IDC_CB8, IDC_CB9, IDC_CBA, IDC_CBB, IDC_CBC, IDC_CBD, IDC_CBE, IDC_CBF,
};
//HI
static UINT idCHi[] = 
{
  IDC_HIC0, IDC_HIC1, IDC_HIC2, IDC_HIC3, 
  IDC_HIC4, IDC_HIC5, IDC_HIC6, IDC_HIC7, 
  IDC_HIC8, IDC_HIC9, IDC_HICA, IDC_HICB, 
  IDC_HICC, IDC_HICD, IDC_HICE, IDC_HICF
};

// LO
static UINT idCLo[] = 
{
  IDC_LOC0, IDC_LOC1, IDC_LOC2, IDC_LOC3, 
  IDC_LOC4, IDC_LOC5, IDC_LOC6, IDC_LOC7, 
  IDC_LOC8, IDC_LOC9, IDC_LOCA, IDC_LOCB, 
  IDC_LOCC, IDC_LOCD, IDC_LOCE, IDC_LOCF
};
//===============================================================

ArSize asTrig[] = 
{
  {idTrStCtrl, SIZE(idTrStCtrl)},
  {idTrTimeCtrl, SIZE(idTrTimeCtrl)},
  {idTrLoHi, SIZE(idTrLoHi)},
  {idTrHiLo, SIZE(idTrHiLo)}
};

ArSize asGate1[] = 
{
  {idAStCtrl, SIZE(idAStCtrl)},
  {idAHi, SIZE(idAHi)},
  {idALo, SIZE(idALo)}
};

ArSize asGate2[] = 
{
  {idBStCtrl, SIZE(idBStCtrl)},
  {idBHi, SIZE(idBHi)},
  {idBLo, SIZE(idBLo)}
};

ArSize asGate3[] = 
{
  {idCStCtrl, SIZE(idCStCtrl)},
  {idCHi, SIZE(idCHi)},
  {idCLo, SIZE(idCLo)}
};


#define SIZEA(a) (sizeof(a)/sizeof(ArSize))
// CAcqTrigDlg dialog

//ctor
CAcqTrigDlg::CAcqTrigDlg()
{
  m_nAcqMode = 0;
  m_bAnd[0] = m_bAnd[1] = m_bAnd[2] = 0;

  m_psp.dwFlags |= PSP_HASHELP;

  for (int bit = 0; bit < 16; bit++)
  {
    m_bTrLoHi[bit] = 0;
    m_bTrHiLo[bit] = 0;
    m_bAHi[bit] = 0;
    m_bALo[bit] = 0;
    m_bBHi[bit] = 0;
    m_bBLo[bit] = 0;
    m_bCHi[bit] = 0;
    m_bCLo[bit] = 0;
  }

}

//dtor
CAcqTrigDlg::~CAcqTrigDlg()
{
  //m_edStartKey.Detach();
}

// CAcqTrigDlg message handlers
LRESULT CAcqTrigDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
  //m_brRed.CreateSolidBrush(RGB(255,100,0));
  //m_brGreen.CreateSolidBrush(RGB(0,255,100));

  CMessageLoop* pLoop = _Module.GetMessageLoop();
  ATLASSERT(pLoop != NULL);
  pLoop->AddMessageFilter(this);

  m_colWhite = GetSysColor(COLOR_WINDOW);
  m_brWhite.CreateSolidBrush(m_colWhite);
	CenterWindow();
  AcqTrParams aP = _Settings.GetAcqAndTrigParams();
  m_uTimer = _Settings.GetTimer();

  //начинаем разбирать структуру
  m_nFrameLen = aP.nFrameLen;
  m_nPreTrig = aP.nPreTrig;
  m_nPostTrig = aP.nPostTrig;
  m_nTrIdle = aP.nIdle;

  m_nPreTrig = m_nFrameLen - m_nPostTrig;
  if (m_nPreTrig < 0)
  {
    m_nPreTrig = 0;
    m_nPostTrig = m_nFrameLen;
  }

  //SetDlgItemInt(IDC_FRAMELEN, m_nFrameLen);
  //SetDlgItemInt(IDC_PRETRIG, m_nPreTrig);
  //SetDlgItemInt(IDC_POSTTRIG, m_nPostTrig);

  m_nAcqMode = aP.nAcqMode;
  if (m_nAcqMode < 0 || m_nAcqMode > 1)
    m_nAcqMode = 0;

  switch (m_nAcqMode)
  {
  default:
  case 0: //Gated
    ShowGroup(asTrig, SIZEA(asTrig), 0);
    //ShowArray(idTrTimeCtrl,SIZE(idTrTimeCtrl),0);
    GetDlgItem(IDC_AND3).ShowWindow(SW_SHOW);
    GetDlgItem(IDC_GATE0).ShowWindow(SW_SHOW);
    break;
  case 1: //triggered
    ShowGroup(asTrig, SIZEA(asTrig), 1);
    //ShowArray(idTrTimeCtrl,SIZE(idTrTimeCtrl),1);
    GetDlgItem(IDC_GATE0).ShowWindow(SW_HIDE);
    GetDlgItem(IDC_AND3).ShowWindow(SW_HIDE);
    break;
  }

  CheckDlgButton(idMode[m_nAcqMode],1);

  for (int i = 0; i < 3; i++) 
  {
    m_bAnd[i] = aP.bAnd[i];
    CheckDlgButton(idAnd[i], m_bAnd[i]);
  }

  WORD wWord(0);
  WORD wMask(0);

  wWord = aP.wTrLoHi;
  wMask |= wWord;
  Word2Array(wWord, m_bTrLoHi);

  wWord = aP.wTrHiLo;
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bTrHiLo);


  wWord = aP.wHi[0];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bAHi);

  wWord = aP.wLo[0];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bALo);

  wWord = aP.wHi[1];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bBHi);

  wWord = aP.wLo[1];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bBLo);

  wWord = aP.wHi[2];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bCHi);

  wWord = aP.wLo[2];
  wWord &= ~wMask;
  wMask |= wWord;
  Word2Array(wWord, m_bCLo);



  CheckButtons(m_bTrLoHi, idTrLoHi);
  CheckButtons(m_bTrHiLo, idTrHiLo);
  CheckButtons(m_bAHi, idAHi);
  CheckButtons(m_bALo, idALo);
  CheckButtons(m_bBHi, idBHi);
  CheckButtons(m_bBLo, idBLo);
  CheckButtons(m_bCHi, idCHi);
  CheckButtons(m_bCLo, idCLo);


  DoDataExchange(false);
  EnableGates();


	return TRUE;
}

int CAcqTrigDlg::OnApply()
{

  UINT nID = ::GetDlgCtrlID(GetFocus());
  if (!EditUpdate(nID))
    return PSNRET_INVALID;


  DoDataExchange(TRUE);

  AcqTrParams aP;
  //начинаем собирать структуру
  m_nPreTrig = m_nFrameLen - m_nPostTrig;
  if (m_nPreTrig < 0)
  {
    m_nPreTrig = 0;
    m_nPostTrig = m_nFrameLen;
  }
  if (m_nAcqMode < 0 || m_nAcqMode > 1)
    m_nAcqMode = 0;

  aP.nFrameLen = m_nFrameLen;

  aP.nPreTrig = m_nPreTrig;

  aP.nPostTrig = m_nPostTrig;

  aP.nAcqMode = m_nAcqMode;
  aP.nIdle = m_nTrIdle;

  for (int i = 0; i < 3; i++)
    aP.bAnd[i] = m_bAnd[i];

  WORD wWord(0);
  WORD wMask(0);
  WORD wEMask(0);

  wWord = Array2Word(m_bTrLoHi);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wTrLoHi = wWord;

  wWord = Array2Word(m_bTrHiLo);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wTrHiLo = wWord;


  wWord = Array2Word(m_bAHi);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wHi[0] = wWord;

  wWord = Array2Word(m_bALo);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wLo[0] = wWord;

  wWord = Array2Word(m_bBHi);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wHi[1] = wWord;

  wWord = Array2Word(m_bBLo);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wLo[1] = wWord;

  wWord = Array2Word(m_bCHi);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wHi[2] = wWord;

  wWord = Array2Word(m_bCLo);
  wWord &= ~wMask;
  wMask |= wWord;
  aP.wLo[2] = wWord;

  _Settings.m_acqtrParams = aP;
  _Settings.m_uTimer = m_uTimer;

  return PSNRET_NOERROR;
}


LRESULT CAcqTrigDlg::OnBnClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
int nIdx;

  SetModified();
  //1 - радиогруппа
  nIdx = Find(idMode, SIZE(idMode),wID);
  if (nIdx >= 0)
  {
    m_nAcqMode = nIdx;

    switch (nIdx)
    {
    default:
    case 0: //Gated
      ShowGroup(asTrig, SIZEA(asTrig), 0);
      //ShowArray(idTrTimeCtrl,SIZE(idTrTimeCtrl),0);
      GetDlgItem(IDC_AND3).ShowWindow(SW_SHOW);
      GetDlgItem(IDC_GATE0).ShowWindow(SW_SHOW);
      break;
    case 1: //triggered
      ShowGroup(asTrig, SIZEA(asTrig), 1);
      //ShowArray(idTrTimeCtrl,SIZE(idTrTimeCtrl),1);
      GetDlgItem(IDC_GATE0).ShowWindow(SW_HIDE);
      GetDlgItem(IDC_AND3).ShowWindow(SW_HIDE);
      break;
    }

    EnableGates();
    return S_OK;
  }
  //AND checkboxes
  nIdx = Find(idAnd, SIZE(idAnd),wID);
  if (nIdx >= 0)
  {
    m_bAnd[nIdx] = IsDlgButtonChecked(wID);
    //первый гейт разрешен для гейтмода и триггера с установленным A
    EnableGates();
    return 1;
  }
 
  nIdx = Find(idTrLoHi, SIZE(idTrLoHi),wID);
  if (nIdx >= 0)
  {
    m_bTrLoHi[nIdx] = IsDlgButtonChecked(wID);
    if (m_bTrLoHi[nIdx])
    {
      m_bTrHiLo[nIdx] = 0;
      CheckDlgButton(idTrHiLo[nIdx],0);
    }
    EnableGates();
    return 1;
  }
  
  nIdx = Find(idTrHiLo, SIZE(idTrHiLo),wID);
  if (nIdx >= 0)
  {
    m_bTrHiLo[nIdx] = IsDlgButtonChecked(wID);
    if (m_bTrHiLo[nIdx])
    {
      m_bTrLoHi[nIdx] = 0;
      CheckDlgButton(idTrLoHi[nIdx],0);
    }
    EnableGates();
    return 1;
  }
  
  nIdx = Find(idAHi, SIZE(idAHi),wID);
  if (nIdx >= 0)
  {
    m_bAHi[nIdx] = IsDlgButtonChecked(wID);
    if (m_bAHi[nIdx])
    {
      m_bALo[nIdx] = 0;
      CheckDlgButton(idALo[nIdx],0);
    }
    EnableGates();
    return 1;
  }
  
  nIdx = Find(idALo, SIZE(idALo),wID);
  if (nIdx >= 0)
  {
    m_bALo[nIdx] = IsDlgButtonChecked(wID);
    if (m_bALo[nIdx])
    {
      m_bAHi[nIdx] = 0;
      CheckDlgButton(idAHi[nIdx],0);
    }
    EnableGates();
    return 1;
  }

  nIdx = Find(idBHi, SIZE(idBHi),wID);
  if (nIdx >= 0)
  {
    m_bBHi[nIdx] = IsDlgButtonChecked(wID);
    if (m_bBHi[nIdx])
    {
      m_bBLo[nIdx] = 0;
      CheckDlgButton(idBLo[nIdx],0);
    }
    EnableGates();
    return 1;
  }
  
  nIdx = Find(idBLo, SIZE(idBLo),wID);
  if (nIdx >= 0)
  {
    m_bBLo[nIdx] = IsDlgButtonChecked(wID);
    if (m_bBLo[nIdx])
    {
      m_bBHi[nIdx] = 0;
      CheckDlgButton(idBHi[nIdx],0);
    }
    EnableGates();
    return 1;
  }

  nIdx = Find(idCHi, SIZE(idCHi),wID);
  if (nIdx >= 0)
  {
    m_bCHi[nIdx] = IsDlgButtonChecked(wID);
    if (m_bCHi[nIdx])
    {
      m_bCLo[nIdx] = 0;
      CheckDlgButton(idCLo[nIdx],0);
    }
    return 1;
  }
  
  nIdx = Find(idCLo, SIZE(idCLo),wID);
  if (nIdx >= 0)
  {
    m_bCLo[nIdx] = IsDlgButtonChecked(wID);
    if (m_bCLo[nIdx])
    {
      m_bCHi[nIdx] = 0;
      CheckDlgButton(idCHi[nIdx],0);
    }
    return 1;
  }

  return 1;
}



LRESULT CAcqTrigDlg::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	HWND hWnd = (HWND)lParam;
	UINT nID = ::GetDlgCtrlID(hWnd);

  if (nID == IDC_KEY_START)
  {

    
  }
  return 0;
}




LRESULT CAcqTrigDlg::OnEnKillFocus(WORD /*wNotifyCode*/, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
  if (!EditUpdate(wID)) 
    return E_FAIL;

  return S_OK;
}

LRESULT CAcqTrigDlg::OnEnSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
 if (wID == IDC_KEY_START)
 {
   SetDlgItemText(wID, "Press Key");
 }
 ((CEdit)GetDlgItem(wID)).SetSel(0,-1);
  return S_OK;

}

BOOL CAcqTrigDlg::PreTranslateMessage(MSG* pMsg)
{
  if (pMsg->message == WM_KEYDOWN)
  {
    if (pMsg->wParam == VK_TAB)
    {
      if (GetAsyncKeyState(VK_SHIFT) < 0)
        PrevDlgCtrl();
      else
        NextDlgCtrl();
      return true;
    }
    else if (pMsg->wParam == VK_RETURN)
    {
      UINT nID = ::GetDlgCtrlID(GetFocus());
      EditUpdate(nID);
     //((CEdit)GetDlgItem(nID)).SetSel(0,-1);
      return true;
    }
  }
  return false;
}



//inner functions
void CAcqTrigDlg::ShowArray(UINT *pArray, int nSize, BOOL bShow)
{
  for (int i = 0; i < nSize; i++)
  {
    GetDlgItem(pArray[i]).ShowWindow(bShow);
  }
}

void CAcqTrigDlg::EnableArray(UINT *pArray, int nSize, BOOL bEnable)
{
  for (int i = 0; i < nSize; i++)
  {
    GetDlgItem(pArray[i]).EnableWindow(bEnable);
  }
}


void CAcqTrigDlg::ShowGroup(ArSize *pGroup, int nSize, BOOL bShow)
{
  for (int i = 0; i < nSize; i++)
  {
    ShowArray(pGroup[i].pAr, pGroup[i].nSize, bShow);
  }
}

void CAcqTrigDlg::EnableGroup(ArSize *pGroup, int nSize, BOOL bEnable)
{
  for (int i = 0; i < nSize; i++)
  {
    EnableArray(pGroup[i].pAr, pGroup[i].nSize, bEnable);
  }
}

void CAcqTrigDlg::EnableGates()
{
int bit;

    BOOL bA1 = m_bAnd[0] || (m_nAcqMode == 0);
    EnableGroup(asGate1,SIZEA(asGate1), bA1);

    //if (m_nAcqMode) //для режима 0 (FREE) первый гейт всегда разрешен
    bA1 &= m_bAnd[1];
    EnableGroup(asGate2,SIZEA(asGate2), bA1);

    //третий гейт в тригрежиме всегда доступен!!!
    if (m_nAcqMode == 0)
      bA1 &= m_bAnd[2];
    else
      bA1 = true;

    EnableGroup(asGate3,SIZEA(asGate3), bA1);

    //нужна проверка на нажатые кнопки сверху
    //триг режим
    if (m_nAcqMode == 1)
    {
      for (bit = 0; bit < 16; bit++)
      {
        if (m_bTrLoHi[bit] || m_bTrHiLo[bit])
        {
          m_bAHi[bit] = 0;
          m_bALo[bit] = 0;
          m_bBHi[bit] = 0;
          m_bBLo[bit] = 0;
          m_bCHi[bit] = 0;
          m_bCLo[bit] = 0;
          CheckDlgButton(idAHi[bit],0);
          CheckDlgButton(idALo[bit],0);
          CheckDlgButton(idBHi[bit],0);
          CheckDlgButton(idBLo[bit],0);
          CheckDlgButton(idCHi[bit],0);
          CheckDlgButton(idCLo[bit],0);
          GetDlgItem(idAHi[bit]).EnableWindow(false);
          GetDlgItem(idALo[bit]).EnableWindow(false);
          GetDlgItem(idBHi[bit]).EnableWindow(false);
          GetDlgItem(idBLo[bit]).EnableWindow(false);
          GetDlgItem(idCHi[bit]).EnableWindow(false);
          GetDlgItem(idCLo[bit]).EnableWindow(false);
       }
      }
    }

    BOOL bAnd = ((m_nAcqMode == 0) || (m_nAcqMode == 1 && m_bAnd[0]));
    if (bAnd)
    {
      for (bit = 0; bit < 16; bit++)
      {
        if (m_bAHi[bit] || m_bALo[bit])
        {
          m_bBHi[bit] = 0;
          m_bBLo[bit] = 0;
          m_bCHi[bit] = 0;
          m_bCLo[bit] = 0;
          CheckDlgButton(idBHi[bit],0);
          CheckDlgButton(idBLo[bit],0);
          CheckDlgButton(idCHi[bit],0);
          CheckDlgButton(idCLo[bit],0);
          GetDlgItem(idBHi[bit]).EnableWindow(false);
          GetDlgItem(idBLo[bit]).EnableWindow(false);
          GetDlgItem(idCHi[bit]).EnableWindow(false);
          GetDlgItem(idCLo[bit]).EnableWindow(false);
       }
      }
    }
    bAnd &= (m_nAcqMode == 0 || m_bAnd[1]);
    if (bAnd)
    {
      for (bit = 0; bit < 16; bit++)
      {
        if (m_bBHi[bit] || m_bBLo[bit])
        {
          m_bCHi[bit] = 0;
          m_bCLo[bit] = 0;
          CheckDlgButton(idCHi[bit],0);
          CheckDlgButton(idCLo[bit],0);
          GetDlgItem(idCHi[bit]).EnableWindow(false);
          GetDlgItem(idCLo[bit]).EnableWindow(false);
       }
      }
    }
}


WORD CAcqTrigDlg::Array2Word(const BOOL* pArray)
{ 
  WORD wWord(0);
  WORD wOne(1);

  for (int i = 0; i < 16; i++)
  {
    if (pArray[i])
    {
      wWord |= wOne;
    }
    wOne <<= 1;
  }
  return wWord;
}

void CAcqTrigDlg::Word2Array(WORD wWord, BOOL* pArray)
{
  WORD wOne(1);

  for (int i = 0; i < 16; i++)
  {
    if (wWord & wOne)
      pArray[i] = 1;
    else 
      pArray[i] = 0;
    wOne <<= 1;
  }
}

void CAcqTrigDlg::CheckButtons(BOOL* pArray, UINT *pID)
{
  for (int i = 0; i < 16; i++)
  {
    CheckDlgButton(pID[i],pArray[i]);
  }

}

BOOL CAcqTrigDlg::EditUpdate(UINT nID)
{
int* arEdits[] = {&m_uTimer, &m_nFrameLen, &m_nPreTrig, &m_nPostTrig, &m_nTrIdle};
int Idx = Find(idEdit,SIZE(idEdit),nID);
  if ( Idx >= 0 )
  {
    int nOld = *(arEdits[Idx]);
    if (!DoDataExchange(TRUE,nID))
    {
      *(arEdits[Idx]) = nOld;
      SetDlgItemInt(nID, nOld, true);
      ((CEdit)GetDlgItem(nID)).SetSel(0,-1);
      return 0;
    }
  }

  switch(nID)
  {
  case IDC_FRAMELEN:
    m_nPostTrig = m_nFrameLen - m_nPreTrig;
    if (m_nPostTrig < 0)
    {
      m_nPreTrig = 0;
      m_nPostTrig = m_nFrameLen;
    }
    break;
  case IDC_PRETRIG:
    m_nPostTrig = m_nFrameLen - m_nPreTrig;
    if (m_nPostTrig < 0)
    {
      m_nPostTrig = 0;
      m_nFrameLen = m_nPreTrig;
    }
    break;
  case IDC_POSTTRIG:
    m_nPreTrig = m_nFrameLen - m_nPostTrig;
    if (m_nPreTrig < 0)
    {
      m_nPreTrig = 0;
      m_nFrameLen = m_nPostTrig;
    }
    break;
  default:
    break;
  }
  DoDataExchange(false);

  if (Idx >= 0)
    ((CEdit)GetDlgItem(nID)).SetSel(0,-1);
  
  return TRUE;


}

void CAcqTrigDlg::OnHelp()
{
  CString str(g_strHelpFile);

  str += "::\\ActD.htm";
  HtmlHelp(GetDesktopWindow(),
          str,
          HH_DISPLAY_TOPIC,
          NULL) ;





}
