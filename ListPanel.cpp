// ListPanel.cpp : implementation of the CListPanel class
//        050618, 17:35
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DataFile.h"
#include ".\listpanel.h"
#include "MainFrm.h"

CListPanel::CListPanel(CMainFrame* pFrm) :
    m_pFile(0)
    ,m_pMainFrm(pFrm)
    ,m_bSelfSel(false)
{
}

CListPanel::~CListPanel(void)
{
}


LRESULT CListPanel::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(false, true, WS_CLIPCHILDREN);
  GetDlgItem(IDC_LIST).SetWindowText("No File");
	m_Map.SubclassWindow(GetDlgItem(IDC_MAP));
	m_Map.MapInit(m_hWnd);
  m_Map.EnableWindow(false);

  //m_List.SubclassWindow(GetDlgItem(IDC_LIST));
  m_List.Attach(GetDlgItem(IDC_LIST));
	m_List.SetViewType(LVS_REPORT);
	m_List.ModifyStyle(LVS_SINGLESEL,LVS_SINGLESEL);
	m_List.SetExtendedListViewStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);//|LVS_EX_ONECLICKACTIVATE);
  m_List.InsertColumn(0,"Nr",LVCFMT_RIGHT,30,-1);

	return S_OK;
}



LRESULT CListPanel::SetFile(CDataFile* pFile, const char* pMainMap)
{
  CString str;
  m_pFile = pFile;
  char szTime[12];
  char szType[12];
  char szTWord[20];

  if (pFile)
  {
    const char *pMap = pFile->GetMap();
    m_Map.SetMainMap(pMap, pFile->GetChanNames());
    m_Map.CopyMap(pMap);
    m_Map.Resize(-1,pFile->GetPinChans(),pFile->GetExtChans());
    if (pMainMap)
      m_Map.SetMainMap(pMainMap, pFile->GetChanNames());

    m_List.DeleteColumn(1);
    m_List.DeleteColumn(2);
    m_List.DeleteColumn(3);
    if (m_pFile->IsPlus())
    {
      m_List.InsertColumn(1,"Type",LVCFMT_RIGHT,40,1);
    }
    m_List.InsertColumn(2,"time",LVCFMT_RIGHT,60,2);
    if (m_pFile->IsPlus())
    {
      m_List.InsertColumn(3,"Trig",LVCFMT_RIGHT,70,1);
    }
    //m_List.AddColumn("time",3);

    for (int rec = 0; rec < m_pFile->GetRecNum(); rec++)
    {
      str.Format("%3d",rec+1);
      m_List.AddItem(rec,0,str);
      if (m_pFile->IsPlus())
      {
        ZeroMemory(szTime,12);
        ZeroMemory(szType,12);
        m_pFile->GetRecTypeAndTime(rec, szType, szTime, szTWord);
        m_List.SetItem(rec,1,LVIF_TEXT,szType,0,0,0,0);
        m_List.SetItem(rec,2,LVIF_TEXT,szTime,0,0,0,0);
        m_List.SetItem(rec,3,LVIF_TEXT,szTWord,0,0,0,0);
      }
      else
      {
        str.Format("%.3f", m_pFile->GetDurationFl()*(rec));
        m_List.SetItem(rec,1,LVIF_TEXT,str,0,0,0,0);
      }
    }
    m_List.ShowWindow(SW_SHOW);
  }
  else
  {
    m_List.DeleteAllItems();
    m_List.DeleteColumn(2);
    m_List.DeleteColumn(1);
  }
	return S_OK;
}

LRESULT CListPanel::OnLvnItemchangedList(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
  LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
  
  if (!m_bSelfSel && m_pMainFrm)
    m_pMainFrm->PostMessage(WM_DIZ_READFRAME,pNMLV->iItem+1);
  return 0;
}


//050708
LRESULT CListPanel::AddRecord(int nRec, const char* pTime, const char* pType, const char *pTWord)
{
CString str;

  if (nRec >= 0)
  {
    str.Format("%3d",nRec+1);
    int nR = m_List.AddItem(nRec,0,str);
    if (pType)
      m_List.SetItem(nR,1,LVIF_TEXT,pType,0,0,0,0);
    if (pTime)
      m_List.SetItem(nR,2,LVIF_TEXT,pTime,0,0,0,0);
    if (pTWord)
      m_List.SetItem(nR,3,LVIF_TEXT,pTWord,0,0,0,0);
  }

  ATLTRACE("AAAAAAAAAAAAAA AddRecrod %d %s %s\n", nRec, pTime, pType);

  return S_OK;
}