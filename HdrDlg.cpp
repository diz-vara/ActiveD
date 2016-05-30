// HeaderDlg.cpp : implementation of the CHeaderDlg class
//    050628
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"

#include "resource.h"
#include "ActD.h"
#include "HDRdlg.h"
#include ".\hdrdlg.h"

static char Months[12][4] = 
{
"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};


LRESULT CHeaderDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
SYSTEMTIME stTime;
CString str;
int Num;


  GetLocalTime(&stTime);
  GetDateFormat(MAKELCID(MAKELANGID(9,0),SORT_DEFAULT),0,&stTime,"dd-MMM-yyyy",m_lpzDateLong,12); //
  GetDateFormat(MAKELCID(MAKELANGID(9,0),SORT_DEFAULT),0,&stTime,"dd.MM.yy",m_lpzDateShort,10); //


  str = m_lpzDateLong;
  str += " ";

  GetTimeFormat(0,TIME_FORCE24HOURFORMAT,&stTime,"hh.mm.ss",m_lpzTime,10);
  str += m_lpzTime;
  str.MakeUpper();
  GetDlgItem(IDC_DATETIME).SetWindowText(str);


	CenterWindow(GetParent());


  m_cmbPatCode.Attach(GetDlgItem(IDC_PATCODE));
  m_cmbPatSex.Attach(GetDlgItem(IDC_PATSEX));

  m_cmbPatSex.AddString("F");
  m_cmbPatSex.AddString("M");

  m_cmbMonth.Attach(GetDlgItem(IDC_PBMONTH));
  m_cmbMonth.AddString("Jan");
  m_cmbMonth.AddString("Feb");
  m_cmbMonth.AddString("Mar");
  m_cmbMonth.AddString("Arp");
  m_cmbMonth.AddString("May");
  m_cmbMonth.AddString("Jun");
  m_cmbMonth.AddString("Jul");
  m_cmbMonth.AddString("Aug");
  m_cmbMonth.AddString("Sep");
  m_cmbMonth.AddString("Oct");
  m_cmbMonth.AddString("Nov");
  m_cmbMonth.AddString("Dec");


  ((CUpDownCtrl)GetDlgItem(IDC_SPBDAY)).SetRange(1,31);
  ((CUpDownCtrl)GetDlgItem(IDC_SPBYEAR)).SetRange(1900,2200);


  m_nPatNum = _Settings.GetInt("Patients","NumberOf",0);
  GetList("Patients", m_nPatNum);
  for (int i = 0; i < m_nPatNum; i++)
  {
    Num = m_arNums[i];
    str.Format("pat%03d",Num);
    m_cmbPatCode.AddString(_Settings.GetString("Patients",str));
    m_cmbPatCode.SetItemData(i, (DWORD)Num);
  }

  m_bPatChanged = false;
  m_nSubj = -1;
  
  m_cmbInvCode.Attach(GetDlgItem(IDC_INVCODE));
  m_nInvNum = _Settings.GetInt("InvCodes","NumberOf",0);
  GetList("InvCodes", m_nInvNum);
  for (i = 0; i < m_nInvNum; i++)
  {
    Num = m_arNums[i];
    str.Format("inv%03d",Num);
    m_cmbInvCode.AddString(_Settings.GetString("InvCodes",str));
    m_cmbInvCode.SetItemData(i, (DWORD)Num);
  }


  m_cmbResearcher.Attach(GetDlgItem(IDC_RESEARCHER));
  m_nResNum = _Settings.GetInt("Researchers","NumberOf",0);
  GetList("Researchers", m_nResNum);
  for (i = 0; i < m_nResNum; i++)
  {
    Num = m_arNums[i];
    str.Format("res%03d",Num);
    m_cmbResearcher.AddString(_Settings.GetString("Researchers",str));
    m_cmbResearcher.SetItemData(i, (DWORD)Num);
  }

  m_cmbEquipment.Attach(GetDlgItem(IDC_EQUIPMENT));
  m_nEquipNum = _Settings.GetInt("Equipment","NumberOf",0);
  GetList("Equipment", m_nEquipNum);
  for (i = 0; i < m_nEquipNum; i++)
  {
    Num = m_arNums[i];
    str.Format("equ%03d",Num);
    m_cmbEquipment.AddString(_Settings.GetString("Equipment",str));
    m_cmbEquipment.SetItemData(i, (DWORD)Num);
  }


  m_cmbStimCode.Attach(GetDlgItem(IDC_STIMCODE));
  m_nStimNum = _Settings.GetInt("StimCodes","NumberOf",0);
  GetList("StimCodes", m_nStimNum);
  for (i = 0; i < m_nStimNum; i++)
  {
    Num = m_arNums[i];
    str.Format("stim%03d",Num);
    m_cmbStimCode.AddString(_Settings.GetString("StimCodes",str));
    m_cmbStimCode.SetItemData(i, (DWORD)Num);
  }


  return TRUE;
}

LRESULT CHeaderDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
CString str, str2;
bool bNoSpace;

  if (wID == IDOK)
  {
    UINT nID = ::GetDlgCtrlID(GetFocus());
    if (nID == IDC_PATCODE)  
			return ComboString(nID);
    else if (nID == 1001)
    {
      nID = ::GetDlgCtrlID(::GetParent(GetFocus()));
      return ComboString(nID);
    }
    else if (nID == IDC_PATNAME)
    {
      GetDlgItemText(IDC_PATNAME,str);
      if (!Check(str, bNoSpace))
      {
        GetDlgItem(IDC_PATNAME).SetFocus();
        ((CEdit)GetDlgItem(IDC_PATNAME)).SetSel(0,-1);
        return E_FAIL;
      }
    }

    m_cmbPatCode.GetWindowText(str);
    Check(str, bNoSpace);
    if (str.IsEmpty() || !bNoSpace)
    {
      MessageBox("Patient code must be selected!");
      return E_FAIL;
    }

    m_strSubj = str;

    m_cmbInvCode.GetWindowText(str);
    if (_Settings.IsExpSubFolder() && str.IsEmpty())
    {
      MessageBox("Investigation code\n (type of experiment) \nmust be selected!");
      return E_FAIL;
    }
    m_strExp = str;

    if (m_bPatChanged)
      SavePat();

    SaveOrder(IDC_PATCODE);
    SaveOrder(IDC_INVCODE);
    SaveOrder(IDC_RESEARCHER);
    SaveOrder(IDC_EQUIPMENT);
    SaveOrder(IDC_STIMCODE);



    m_cmbPatCode.GetWindowText(m_strLocSubj);
    m_strLocSubj += " ";
    m_cmbPatSex.GetWindowText(str2);
    if (str2.IsEmpty()) str2 = "X";
    m_strLocSubj += str2; m_strLocSubj += " ";

    int nDay = GetDlgItemInt(IDC_PBDAY);
    int nMonth = m_cmbMonth.GetCurSel();
    int nYear = GetDlgItemInt(IDC_PBYEAR);
    if (nDay < 1 || nDay > 31 || nMonth < 0 || nYear < 1800)
      str2 = "X";
    else
    {
      str2.Format("%02d-%3s-%04d",nDay,Months[nMonth],nYear);
    }
    m_strLocSubj += str2; m_strLocSubj += " ";

    GetDlgItem(IDC_PATNAME).GetWindowText(str2);
    if (!Check(str2,bNoSpace)) 
    {
      GetDlgItem(IDC_PATNAME).SetFocus();
      ((CEdit)GetDlgItem(IDC_PATNAME)).SetSel(0,-1);
      return E_FAIL;
    }
    if (str2.IsEmpty()) str2 = "X";
    str2.Replace(' ', '_');
    m_strLocSubj += str2; m_strLocSubj += " ";

    m_strLocRec = "Startdate ";
    m_strLocRec += m_lpzDateLong;
    m_strLocRec += " ";

    GetDlgItem(IDC_INVCODE).GetWindowText(str2);
    if (str2.IsEmpty()) str2 = "X";
    str2.Replace(' ', '_');
    m_strLocRec += str2; m_strLocRec += " ";

    GetDlgItem(IDC_RESPCODE).GetWindowText(str2);
    if (str2.IsEmpty()) str2 = "X";
    str2.Replace(' ', '_');
    m_strLocRec += str2; m_strLocRec += " ";

    GetDlgItem(IDC_EQUIPCODE).GetWindowText(str2);
    if (str2.IsEmpty()) str2 = "X";
    str2.Replace(' ', '_');
    m_strLocRec += str2; m_strLocRec += " ";

    GetDlgItem(IDC_STIMCODE).GetWindowText(str2);
    if (str2.IsEmpty()) str2 = "X";
    str2.Replace(' ', '_');
    m_strLocRec += str2; m_strLocRec += " ";
    //str2 = m_strLocSubj;
    //str2 += "/n";
    //str2 += m_strLocRec;
    //MessageBox(str2);
  }

	EndDialog(wID);
	return wID;
}


LRESULT CHeaderDlg::ComboString(UINT wID, bool bSelected)
{
  CString str, str2;
  const char* pSection(0);
  CComboBox *pCmb;
  int* pNum;
  int n(-1);
  bool bNoSpace;

  switch(wID)
  {
  case IDC_PATCODE:
    if (m_bPatChanged)
      SavePat();
    pCmb = &m_cmbPatCode;
    pNum = &m_nPatNum;
    break;
  case IDC_INVCODE:
    pCmb = &m_cmbInvCode;
    pNum = &m_nInvNum;
    break;
  case IDC_RESEARCHER:
    pCmb = &m_cmbResearcher;
    pNum = &m_nResNum;
    break;
  case IDC_EQUIPMENT:
    pCmb = &m_cmbEquipment;
    pNum = &m_nEquipNum;
    break;
  case IDC_STIMCODE:
    pCmb = &m_cmbStimCode;
    pNum = &m_nStimNum;
    break;
  default:
    return S_OK;
  }


  if (bSelected)
  {
    n = pCmb->GetCurSel();
    pCmb->GetLBText(n, str);
  }
  else
  {
    pCmb->GetWindowText(str);
    if (!Check(str, bNoSpace))
    {
      pCmb->SetFocus();
      pCmb->SetEditSel(0,-1);
      return E_FAIL;
    }
    else if (!bNoSpace)
    {
      str.Empty();
    }
    n = pCmb->FindStringExact(0,str);
  }
  int code;
  if (n != CB_ERR)
  {
    code = (int)pCmb->GetItemData(n);
		if (n > 0)
			pCmb->DeleteString(n);
  }
  else
  {
    //new string
    if (str.IsEmpty())
      return E_FAIL;
    code = ++(*pNum);
    switch(wID)
    {
    case IDC_PATCODE:
			m_cmbMonth.SetCurSel(-1);
			m_cmbPatSex.SetCurSel(-1);
			SetDlgItemInt(IDC_PBDAY,0);
			SetDlgItemInt(IDC_PBYEAR,0);
			SetDlgItemText(IDC_PATNAME,"");
      break;
    case IDC_INVCODE:
      pSection = "InvCodes";
      str2.Format("inv%03d",code);
      break;
    case IDC_RESEARCHER:
      pSection = "Researchers";
      str2.Format("res%03d",code);
      break;
    case IDC_EQUIPMENT:
      pSection = "Equipment";
      str2.Format("equ%03d",code);
      break;
    case IDC_STIMCODE:
      pSection = "StimCodes";
      str2.Format("stim%03d",code);
      break;
    default:
      return E_FAIL;
    }
    n = 1;
  }

  //pSection != 0 только если появилось что-то новое
  if (pSection)
  {
    _Settings.WriteValue(pSection,str2,str);
    _Settings.WriteValue(pSection,"NumberOf",*pNum);
  }
  if (n > 0)
  {
	  pCmb->InsertString(0,str);
    pCmb->SetItemData(0,code);
	  pCmb->SetCurSel(0);
  }



  return S_OK;
}
LRESULT CHeaderDlg::OnCbnKillfocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return ComboString(wID);
}

int CHeaderDlg::GetLocSubj(char *chLocSubj) const
{
  strncpy(chLocSubj, m_strLocSubj, 79);

  return 0;
}

int CHeaderDlg::GetLocRec(char *chLocRec) const
{
  strncpy(chLocRec, m_strLocRec, 79);

  return 0;
}

int CHeaderDlg::GetShortDate(char *chStartDate) const 
{
  strncpy(chStartDate, m_lpzDateShort, 8);
  return 8;
}


LRESULT CHeaderDlg::OnSelEndOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  CString str, str2, str0;
  int n;

  if (wID == IDC_PATCODE)
  {
    //rewrite subj data in ini-file only for non-zero field
    if (m_nSubj >= 0 && m_bPatChanged)
    {
        SavePat();
    }

    n = m_cmbPatCode.GetCurSel();
    DWORD dwData = m_cmbPatCode.GetItemData(n);
    if (n > 0)
    {
      m_cmbPatCode.GetLBText(n,str);
	    m_cmbPatCode.InsertString(0,str);
	    m_cmbPatCode.SetCurSel(0);
      m_cmbPatCode.DeleteString(n+1);
      m_cmbPatCode.SetItemData(0,dwData);
    }
    m_nSubj = dwData;

    str0.Format("pat%03d",dwData);
    str = str0;
    str += "Sex";
    m_cmbPatSex.SetCurSel(_Settings.GetInt("Patients",str,-1));

    str = str0;
    str += "Year";
    SetDlgItemInt(IDC_PBYEAR,_Settings.GetInt("Patients",str,0));

    str = str0;
    str += "Month";
    m_cmbMonth.SetCurSel(_Settings.GetInt("Patients",str,-1));

    str = str0;
    str += "Day";
    SetDlgItemInt(IDC_PBDAY, _Settings.GetInt("Patients",str,0));

    str = str0;
    str += "Name";
    SetDlgItemText(IDC_PATNAME, _Settings.GetString("Patients",str,""));


    m_bPatChanged = false;
  }
  else
  {
    return ComboString(wID, true);
  }

  return S_OK;
}


LRESULT CHeaderDlg::OnPatChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
m_bPatChanged = true;
  return 0;
}


void CHeaderDlg::SavePat()
{
int n;
CString str,str2,str0;

  n = (int)m_cmbPatCode.GetItemData(0);
  str0.Format("pat%03d",n);
  str = str0;
  m_cmbPatCode.GetLBText(0,str2);
  _Settings.WriteValue("Patients",str,str2);

  str = str0;
  str += "Sex";
  _Settings.WriteValue("Patients",str,m_cmbPatSex.GetCurSel());

  str = str0;
  str += "Year";
  GetDlgItem(IDC_PBYEAR).GetWindowText(str2);
  _Settings.WriteValue("Patients",str,str2);

  str = str0;
  str += "Month";
  _Settings.WriteValue("Patients",str,m_cmbMonth.GetCurSel());

  str = str0;
  str += "Day";
  GetDlgItem(IDC_PBDAY).GetWindowText(str2);
  _Settings.WriteValue("Patients",str,str2);

  str = str0;
  str += "Name";
  GetDlgItem(IDC_PATNAME).GetWindowText(str2);
  _Settings.WriteValue("Patients",str,str2);

}


//050721 - общая ф-ия обработки списков очередности
int CHeaderDlg::GetList(const char* Sect, int NumOf)
{
CString strList = _Settings.GetString(Sect,"Order");
const char* cp = strList;
m_arNums.RemoveAll();
int Num;
  while(cp && *cp)
  {
    if (sscanf(cp,"%d",&Num) > 0)
      m_arNums.Add(Num);
    else
      break;
    cp = strchr(cp, ',');
    if (cp)
      cp++;
  }

  //поведение при несовпадении размеров
  if (m_arNums.GetSize() != NumOf)
  {
    m_arNums.RemoveAll();
    for (int i = 0; i < NumOf; i++)
      m_arNums.Add(i+1);
  }
 
  return m_arNums.GetSize();
}

int  CHeaderDlg::SaveOrder(UINT nID)
{
int N(0);
CComboBox *pCmb;
CString str, str2;
const char* pSection;

  switch(nID)
  {
  case IDC_PATCODE:
    pSection = "Patients";
    pCmb = &m_cmbPatCode;
    N = m_nPatNum;
    break;
  case IDC_INVCODE:
    pSection = "InvCodes";
    pCmb = &m_cmbInvCode;
    N = m_nInvNum;
    break;
  case IDC_RESEARCHER:
    pSection = "Researchers";
    pCmb = &m_cmbResearcher;
    N = m_nResNum;
    break;
  case IDC_EQUIPMENT:
    pSection = "Equipment";
    pCmb = &m_cmbEquipment;
    N = m_nEquipNum;
    break;
  default:
    return 0;
 }

  if (N)
  {
    _Settings.WriteValue(pSection,"NumberOf",N);
    str2.Empty();

    for (int i = 0; i < N; i++)
    {
      int Order = (int)pCmb->GetItemData(i);
      str.Format("%d,",Order);
      str2 += str;
    }

    _Settings.WriteValue(pSection,"Order",str2);
  }

  return N;

}


bool CHeaderDlg::Check(const char* cp, bool &bNoSpace)
{
int ch;
  bNoSpace = false;

  while ( (ch = (int)*cp++) != 0)
  {
    if (ch < 32 || ch > 126)
    {
      MessageBox("Only US-ASCII printable charecters\nwith codes 32-126 (0x20-7E)\nare allowed");
      return false;
    }
    else if (ch != 0x20)
      bNoSpace = true;
  
  }


  return true;
}

LRESULT CHeaderDlg::OnEnKillfocusPatname(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  bool bNoSpace;
  CString str;
  GetDlgItemText(IDC_PATNAME,str);
  if (!Check(str, bNoSpace))
  {
    GetDlgItem(IDC_PATNAME).SetFocus();
    ((CEdit)GetDlgItem(IDC_PATNAME)).SetSel(0,-1);
    return E_FAIL;
  }

  return S_OK;
}

LRESULT CHeaderDlg::OnBnClickedPrevious(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
  m_cmbResearcher.SetCurSel(0);
  m_cmbEquipment.SetCurSel(0);
  m_cmbInvCode.SetCurSel(0);
  m_cmbPatCode.SetCurSel(0);
  m_cmbStimCode.SetCurSel(0);

  OnSelEndOk(0, IDC_PATCODE, m_hWnd,  bHandled);
  //m_bPatChanged = true;
  //ComboString(IDC_PATCODE,true);
  return 0;
}
