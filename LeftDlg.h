#pragma once
#include "StatMap.h"
#include "resource.h"

class CLeftDlg :
     public CDialogImpl<CLeftDlg>,
     public CDialogResize<CLeftDlg>
{
private:
     CStatMap m_Map;
public:
   enum { IDD = IDD_DIALOGBAR };
	CLeftDlg(void);
	~CLeftDlg(void);
	void SetNofChans(int nChans, int nTouch) {
		int N = nChans/32; 
		if (nChans%32) N++;
		m_Map.Resize(-1,nChans,nTouch);
		m_Map.Resize(-1,nChans,nTouch);
	}

  char* GetMap() {return m_Map.m_bOn;}
  size_t GetMapSize() const {return sizeof(m_Map.m_bOn);}

  HWND GetMapHwnd() const {return m_Map.m_hWnd;}
  char* GetNewChanNames() {return m_Map.GetOrCreateNames(); }
  char* GetChanNames() {return m_Map.m_pChNames; }
  const char* GetChanName(int nChan) {return m_Map.GetName(nChan); }

    BEGIN_MSG_MAP(CLeftDlg)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
          CHAIN_MSG_MAP(CDialogResize<CLeftDlg>)
    END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CLeftDlg)
       DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_X)
       DLGRESIZE_CONTROL(IDC_FRAME, DLSZ_SIZE_Y)
		END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
		//void SetMainMap (const UCHAR *map) {m_Map.SetMainMap(map);}
		UCHAR GetCell(int Num) const {return m_Map.GetCell(Num);}
		void RedrawMap() {m_Map.Redraw();}

    //050219 - для передачи сведений в основное окно
 

};
