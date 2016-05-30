#pragma once

#include "Thread.h"

class CMainFrame;
class CActDView;

class GrThread :
	public CThreadImpl
{
protected:
  const int m_nType;
	CMainFrame * const m_pFrame;
	CActDView *m_pView;
	CMessageLoop theLoop;

public:
	DWORD m_idThread;

public:
	GrThread(CMainFrame* const pFrm = 0, int Type = -1);
	~GrThread(void);
	virtual DWORD Run();
	CActDView* GetView() const {return m_pView;}
};


