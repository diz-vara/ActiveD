#include "StdAfx.h"
#include "resource.h"
#include ".\grthread.h"
#include "ActDView.h"
#include "MainFrm.h"

GrThread::GrThread(CMainFrame* const pFrm, int Type) : m_pFrame(pFrm), m_pView(0),
    m_nType(Type)
{

}

GrThread::~GrThread(void)
{
	if(m_pView)
  {
		delete m_pView;
  }
  ATLTRACE("GrThread DTR %X %X (%x, %x)\n",GetCurrentThreadId(), m_idThread, m_pView, this);

	//::PostThreadMessage(m_idThread,WM_QUIT,0,0);
}


DWORD GrThread::Run()
{
CRect rc;

		//_Module.AddMessageLoop(&theLoop);
		m_idThread = GetCurrentThreadId();

	ATLTRACE("GtThread-Run Thread %x (%d)\n", GetCurrentThreadId(),m_nType);

	::GetClientRect(m_pFrame->m_hRightUp,&rc);
  switch(m_nType)
  {
  case 0: //Osc
	  m_pView = new COscView(m_pFrame, &theLoop);
    break;
  case 1: //Snap
	  m_pView = new CSnapView(m_pFrame->GetFile(), m_pFrame, &theLoop);
    break;
  default:
    break;
  }

  if (m_pView)
  {
	  m_pView->Create(m_pFrame->m_hRightUp , &rc, NULL, WS_MAXIMIZEBOX|WS_OVERLAPPED|WS_CHILD|WS_SYSMENU |WS_CAPTION|WS_THICKFRAME| WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
	  m_pView->SetPar(m_pFrame->m_hRightUp);		
  }

	int nRet = theLoop.Run();
  ATLTRACE("GrThread %X exit \n",m_idThread );

  if (m_pView)
	  delete m_pView;
	m_pView = 0;
  //_Module.RemoveMessageLoop();

	return nRet;
}




