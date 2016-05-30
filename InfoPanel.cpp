// InfoPanel.h : implementation of the CInfoPanel class
//        050618, 17:35
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\infopanel.h"
#include "DataFile.h"
#include "ActD.h"


CInfoPanel::CInfoPanel(void)
{
}

CInfoPanel::~CInfoPanel(void)
{
}


LRESULT CInfoPanel::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
static char strNoDrivers[] = "\n No Drivers \n\n or file \n\n \"LabView_DLL.dll\" \n\n not found. ";
static char strNoFirstInstance[] = "\n Only first instance \n\n of application \n\n works with hardware. ";

  DlgResize_Init(false, true, WS_CLIPCHILDREN);
  if (USB_WRITE)
    GetDlgItem(IDC_INFO).SetWindowText(strNoFirstInstance);
  else
    GetDlgItem(IDC_INFO).SetWindowText(strNoDrivers);

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
// CFInfoPanel
CFInfoPanel::CFInfoPanel(void)
{
}

CFInfoPanel::~CFInfoPanel(void)
{
}


LRESULT CFInfoPanel::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DlgResize_Init(false, true, WS_CLIPCHILDREN);
  GetDlgItem(IDC_INFO).SetWindowText("No File");
	m_Map.SubclassWindow(GetDlgItem(IDC_MAP));
	m_Map.MapInit(m_hWnd);
  m_Map.EnableWindow(false);
	return S_OK;
}



LRESULT CFInfoPanel::SetFile(CDataFile* pFile)
{
  if (pFile == 0 || pFile->IsFile() != GENERIC_READ)
  {
    GetDlgItem(IDC_INFO).SetWindowText("No File");
  }
  else
  {
    CString str("\n "), str0;
    str += pFile->GetTitle();
    str += "\n\n";
    str += "Start Date and Time\n";
    str += pFile->GetStartDate();
    str += "  ";
    str += pFile->GetStartTime();
    str += "\n\n";
      str0.Format(" %d recorded channels", pFile->GetNumOfChans(false));
      str += str0;
      str += "\n";
      str0.Format(" (%d unique analog)", pFile->GetNumOfChans(true));
      str += str0;
      str += "\n\n";
      str0.Format(" %d records\n", pFile->GetRecNum());
      str += str0;
      //str += "\n";
      str0.Format("of %.3f s each\n", pFile->GetDurationFl());
      str += str0;
      str += "\n\n";
      str0.Format("Sampling frq %d Hz\n", pFile->GetFrq());
      str += str0;
      str += "\n";
      str0.Format("Trigger frq %d Hz\n", pFile->GetTFrq());
      str += str0;
      str += "\nLocal Patient:\n";
      str0 = pFile->GetLocSubj();
      str0.Replace(' ','\n');
      str += str0;
      str += "\n\nLocal Recording:\n";
      str0 = pFile->GetLocRec();
      str0.Replace(' ','\n');
      str += str0;

    GetDlgItem(IDC_INFO).SetWindowText(str);
    const char *pMap = pFile->GetMap();
    m_Map.SetMainMap(pMap, pFile->GetChanNames());
    m_Map.CopyMap(pMap);
    m_Map.Resize(-1,pFile->GetPinChans(),pFile->GetExtChans());

  }

  
  return S_OK;
}

