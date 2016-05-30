// ListPanel.h : interface of the CListPanel class
//        050701, 15:35
/////////////////////////////////////////////////////////////////////////////


#pragma once
#include "StatMap.h"
#include "resource.h"

class CDataFile;
class CMainFrame;


// 050805
//  Мне пришлось завести свой класс списка, так как обычный майкрософтовский
//  контрол при нажатии на мышиную кнопку (как левую, так и правую) входил в модальный цикл
//  глотающий все сообщения - вплоть до отпускания кнопки. Добавленная ф-ия PreTranslateMessage
//  собственно делает только одно - помещает сразу же за сообщением о нажатии кнопки сообщение о ее
//  поднятии. Вроде бы это помогает....

class CMyList : public CListViewCtrl, public CMessageFilter
{
public:
  CMyList() : CListViewCtrl()
  {
	  CMessageLoop* pLoop = _Module.GetMessageLoop();
	  if(pLoop != NULL) //ATLASSERT(pLoop != NULL);
      pLoop->AddMessageFilter(this);
  }

  virtual ~CMyList()
  {
	  CMessageLoop* pLoop = _Module.GetMessageLoop();
	  if(pLoop != NULL) //ATLASSERT(pLoop != NULL);
		  pLoop->RemoveMessageFilter(this);
  }

  BOOL PreTranslateMessage(MSG *pMsg)
  {
    if (pMsg->message == WM_LBUTTONDOWN)
    {
      if (IsWindowVisible()) PostMessage(WM_LBUTTONUP, pMsg->wParam, pMsg->lParam);
    }
    else if (pMsg->message == WM_RBUTTONDOWN)
    {
      if (IsWindowVisible()) PostMessage(WM_RBUTTONUP, pMsg->wParam, pMsg->lParam);
    }
    return false;
  }

};


class CListPanel :
  public CDialogImpl<CListPanel>
  ,public CDialogResize<CListPanel>
{
private:
   CStatMap m_Map;
   CDataFile *m_pFile;
   CMyList m_List;
   CMainFrame* m_pMainFrm;
   bool m_bSelfSel;
public:
   enum { IDD = IDD_LISTPANEL };

    BEGIN_MSG_MAP(CListPanel)
          MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
          NOTIFY_HANDLER(IDC_LIST, NM_CLICK, OnLvnItemchangedList)
          CHAIN_MSG_MAP(CDialogResize<CListPanel>)
    END_MSG_MAP()

		BEGIN_DLGRESIZE_MAP(CListPanel)
       DLGRESIZE_CONTROL(IDC_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
       DLGRESIZE_CONTROL(IDC_MAP, DLSZ_SIZE_X | DLSZ_MOVE_Y)
		END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT SetFile(CDataFile* pFile = 0, const char* pMap = 0);
    
    //050708
    LRESULT AddRecord(int nRec, const char* pTime = 0, const char* pType = 0, const char *pTWord = 0);

  CListPanel(CMainFrame* pFrm = 0);
  virtual ~CListPanel(void);

public:
  LRESULT OnLvnItemchangedList(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/);
  int SetRecord(int nRecord)
  {
    m_bSelfSel = true;
    m_List.SelectItem(nRecord-1);
    m_bSelfSel = false;
    return 0;
  }
 	void RedrawMap() {m_Map.Redraw();}

};
