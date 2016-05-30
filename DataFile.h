//created 050516 27:00
#pragma once
#include "stdafx.h"
#include "ActD.h"
#include "Thread.h"

class CMainFrame;
typedef  struct BDFh
  {
    char chID[8];
    char chLocSubj[80];
    char chLocRec[80];
    char chStartDate[8];
    char chStartTime[8];
    char chNumBytes[8];
    char chVersion[44];
    char chRecNum[8];
    char chRecDur[8];
    char chChanNum[4];
  } BDFh;

class CBDFheader 
{
private:
  size_t m_szSize;

public:
  char *m_pBuf;
  BDFh *m_pMainHdr;
  char (*m_pcLabels)[16];
  char (*m_pcTransd)[80];
  char (*m_pcPhDim)[8];
  char (*m_pcPhMin)[8];
  char (*m_pcPhMax)[8];
  char (*m_pcDgMin)[8];
  char (*m_pcDgMax)[8];
  char (*m_pcPreFilt)[80];
  char (*m_pcSamples)[8];
  char (*m_pcReserved)[32];

  CString m_str;
  int m_nChanNum;

public:
  CBDFheader() : m_pBuf(0), m_szSize(0) {}
  ~CBDFheader();
  CBDFheader(int nChans, char* pBuf = 0);

  size_t GetSize() const {return m_szSize;}

  long GetSamples(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcSamples[chan],8);
    return atol(m_str);
  }

  const char* SetSamples(int chan, long nSmpl)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str.Format("%8ld",nSmpl);
    return SetSamples(chan,m_str);
  }

  const char* GetChanLabel(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcLabels[chan],16);
    return m_str;
  }


  const char* GetChanType(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcTransd[chan],80);
    return m_str;
  }

  const char* GetPhMin(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcPhMin[chan],8);
    return m_str;
  }

  const char* GetPhMax(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcPhMax[chan],8);
    return m_str;
  }

  const char* GetDgMin(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcDgMin[chan],8);
    return m_str;
  }

  const char* GetDgMax(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcDgMax[chan],8);
    return m_str;
  }

  const char* GetUnit(int chan)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    m_str = GetField(m_pcPhDim[chan],8);
    return m_str;
  }

  const char* SetChanType(int chan, int nType, int nFrq = 0, int nNum = -1)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    SetChanRes(chan, "Reserved field");
    CString str;
    switch(nType)
    {
    case 0:
      SetChanUnit(chan, "Boolean");
      SetChanDgMaxMin(chan,"-8388608","8388607 ");
      SetChanPhMaxMin(chan,"-8388608","8388607 "); 
      SetChanFilt(chan, "No filtering");
      return SetChanType(chan,"Triggers and Status");
    case 1:
      SetChanUnit(chan, "uV");
      SetChanDgMaxMin(chan,"-8388608","8388607 ");
      SetChanPhMaxMin(chan,"-262144 ","262143  "); 
      str.Format("PreFiltering: HP: 0; LP: %d Hz", nFrq/5);
      SetChanFilt(chan,str);
      if (nNum < 256)
        str.Format("Active Electrode #%c%d#", nNum/32+'A', (nNum%32)+1);
      else
        str.Format("Active Electrode #EXG%d#", nNum-256+1);
      return SetChanType(chan,str);
    case 2:
      SetChanUnit(chan, "Events");
      SetChanDgMaxMin(chan,"-8388608","8388607 ");
      SetChanPhMaxMin(chan,"-1        ","1       "); 
      SetChanFilt(chan, "Events");
      return SetChanType(chan,"EDF annotation");
    default:
      SetChanUnit(chan, "Reserved");
      SetChanDgMaxMin(chan,"-8388608","8388607");
      SetChanPhMaxMin(chan,"-1","1"); 
      SetChanFilt(chan,"--------------------");
      return SetChanType(chan,"Unknown");
    }
  }

  const char* SetChanLabel(int chan, const char* Label, bool bPlus = 1)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    CString str;
    if (bPlus)
      str.Format("EEG %s", Label);
    else
      str.Format("%s", Label);

    strncpySP(m_pcLabels[chan],str,16);
    return m_pcLabels[chan];
  }


  const char* SetChanRes(int chan, const char* pRes)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcReserved[chan],pRes,32);
    return m_pcReserved[chan];
  }

private:
  CString GetField(const char* cp, int len)
  {
    m_str = CString(cp,len);
    return m_str;
  }

  const char* SetChanUnit(int chan, const char* pUnit)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcPhDim[chan],pUnit,8);
    return m_pcPhDim[chan];
  }

  const char* SetChanDgMaxMin(int chan, const char* pMin, const char *pMax)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcDgMax[chan],pMax,8);
    strncpySP(m_pcDgMin[chan],pMin,8);
    return m_pcDgMin[chan];
  }

  const char* SetChanPhMaxMin(int chan, const char* pMin, const char *pMax)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcPhMax[chan],pMax,8);
    strncpySP(m_pcPhMin[chan],pMin,8);
    return m_pcPhMin[chan];
  }

  const char* SetChanType(int chan, const char* pType)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcTransd[chan],pType,80);
    return m_pcTransd[chan];
  }

  const char* SetSamples(int chan, const char* str)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcSamples[chan],str,8);
    return m_pcSamples[chan];
  }


  const char* SetChanFilt(int chan, const char* pFilt)
  {
    if (chan < 0 || chan >= m_nChanNum)
      return 0;
    strncpySP(m_pcPreFilt[chan],pFilt,80);
    return m_pcPreFilt[chan];
  }

  char* strncpySP(char *pDst, const char* pSrc, int nNum)
  {
    //fill pDST string with nNum spaces
    FillMemory(pDst, nNum, 040);

    //length of source string (to avoid Null-termination)
    int N = strlen(pSrc);
    //if greater then dst
    if (N > nNum) 
      N = nNum;
    return strncpy(pDst, pSrc, N);

  }

};

class CChanWrite
{
public:
  short nType;
  short nNum;
  int nDur;
  int nLength;
  int nPreLen;
  DWORD dwOffs;
  long nOff;
  double flCal;
};

class CDataFile : 
	public CThreadImpl
  ,public CMessageFilter

{
private:
  int m_nChanNum;
  CSimpleArray<CChanWrite> m_arFChans;
  int24 *m_pBuf24;
  CRITICAL_SECTION* m_pcsWChans;

  //050531 - нужно, чтобы было куда отправлять сообщения
  const CMainFrame *m_pMainFrm;


  Pointer24 m_Pointers[4];

  bool m_bWriting;
  bool m_bNeedStart;
  bool m_bNeedStop;
  bool m_bNeedReOpen;
  _ChanW *m_pWChans;

  int FindFreeBuf(int nCurBuf = 0);
  int WritePre(int nBuf, int nCnt, WORD wTrig, WORD wSet, WORD wClear);
  BOOL CloseAndReopen();
  HANDLE m_hFile;
  //счетчик точек тригканала - DWORD должно хватить на 72 часа, а ведь это - только с текущего запуска...
  DWORD m_dwCounter;
  //точка тригканала, являющаяся стартовой
  DWORD m_dwStartPoint;
  //счетчик записей в файл
  int m_nRecCnt;

  //время первой записи в файл
  unsigned hyper m_u64StartTime;

  //время нажатия на кнопку Run - должно быть передано из главного таймерного цикла ДО первого получения фрейма
  unsigned hyper m_u64RunTime;
  int m_nFrq;
  int m_nDur;
  int m_nChLen;
  int m_nTrLen;


  void SetStartTime(long Cnt);
  float GetPointTime(DWORD cnt);
  bool m_bPlus;

  //050601 Файл хранит свою карту
 	char m_bMap[TOTCHANS];

  //050607 Файл имеет свои переменные - кол-во каналов и имена
  //int m_nPinChans;
  //int m_nExtChans;
  char *m_pChanNames;

  //050615
  CString m_strFileTitle;
  double m_flDur;
  int m_nRecNum;
  int m_nTFrq;

  //050617
  //file code (0, GENERIC_READ, GENERIC_WRITE)
  DWORD m_nCode;

  //050620 
  //Number of pin and ext channels
  int m_nExtChans;
  int m_nPinChans;

  //050624
  int m_nRecSize;
  int m_nHdrSize;
  int m_nCurrRec;
  LARGE_INTEGER m_i64StartPos;
  LARGE_INTEGER m_i64CurrPos;
  U64 m_i64Zero;
  LARGE_INTEGER m_szFileSize;
  int m_nRecordedChans;

  //050627 - отдельно для тригканала
  CChanWrite m_cwTrChan;

  //050628 - базовый заголовок
  BDFh m_hdrMain;
  CBDFheader *m_pHdr;

  //050701
  CString m_strFile;
  Pointer24 m_PntRead;

  //050712 - файл хранит режим ввода
  DWORD m_dwMode;

  //050716 - пропущенная за счет запрета записи
  bool m_bSkipped;
  //050716 - время начала непрерывной записи
  float m_flNewTime;

  //050726
  void MatWrite(HANDLE hFile, const char* cName, int nT, int nCol, int nRow, double* pBuf);
  int m_nAnnLen;

  //050728
  bool ReadCf(int chan, CChanWrite* pCw);

  //050804
  CRITICAL_SECTION m_csFile;

public:
  UINT m_idThread;



  CDataFile(const CMainFrame* pMainFrm);
  ~CDataFile(void);
  virtual DWORD Run();
  BOOL PreTranslateMessage(MSG* pMsg);

  const char* const CDataFile::FileInit(CSimpleArray<_ChanW> *parChanW, AcqTrParams* pTrParams, CRITICAL_SECTION *pCS, DWORD dwMode = 0);
  //void FileInit(CSimpleArray<_ChanW> *parChanW, int nLen, int nPreLen,  CRITICAL_SECTION *pCS, const CMainFrame* pMainFrm);
  LRESULT NewFrame(int Num, CSimpleArray<_ChanW> *parChanW);
  bool IsWriting() const {return m_bWriting;}
  bool IsInitialized() const {return (m_pMainFrm != 0);}

  const HANDLE& GetFile() const {return m_hFile;}
  BOOL NewFile();
  operator HANDLE() {return m_hFile;}
  int IsFile() const {return m_nCode;}

  void SetRunTime(unsigned hyper u64RunTime)
  {
    m_u64RunTime = u64RunTime;
  }

  //050601 Файл хранит свою карту
  //const UCHAR* const FillMap(int OnChans, int nChans, int nExtChans, const UCHAR* pMainMap, const char* pNames)
  //{
  //  m_nPinChans = nChans;
  //  m_nExtChans = nExtChans;
  //  m_nChanNum = OnChans+1;
  //  CopyMemory(m_pChanNames, pNames, TOTCHANS*8);
  //  CopyMemory(m_bMap, pMainMap, TOTCHANS);
  //  return m_bMap;
  //}
  LRESULT Open();
  LRESULT Close();

  int GetChannelNumber(int chan) const 
  {
		if (m_arFChans.GetSize() == 0 || chan < 0 || chan >= m_nChanNum)
      return -1;
    return m_arFChans[chan].nNum;
  }

  int GetNumOfChans(bool bReal = true) const 
  {
    return (bReal) ? m_nChanNum:m_nRecordedChans;
  }
  //int GetNumOfExt() const {return m_nExtChans;}
  //int GetNumOfPin() const {return m_nPinChans;}
  char* GetChanNames() const {return m_pChanNames;}
  int GetDuration() const {return m_nDur;}
  int GetChanLen() const {return m_nChLen;}
  int GetTrLen() const {return m_nTrLen;}
	const CChanWrite* GetFileChans() const {return m_arFChans.GetData();}
  const CChanWrite* GetTrChan() const {return &m_cwTrChan;}

  //050615
  const char* GetTitle() {return m_strFileTitle;}
  double GetDurationFl() const {return m_flDur;}
  int GetRecNum() const {return m_nRecNum;}
  int GetFrq() const {return m_nFrq;}
  int GetTFrq() const {return m_nTFrq;}
  const char* const GetMap() const {return m_bMap;}

  //050620
  int GetPinChans() const {return m_nPinChans;}
  int GetExtChans() const {return m_nExtChans;}

  //050624
  bool ReadRecord(int nRecN = -1, Pointer24* pReadPointer = 0);
  //050625
  int GetRecSize() const {return m_nRecSize;}  

  //050630 - to set main frame at start
  void SetMainFrame (const CMainFrame* pMainFrm)
  {
    m_pMainFrm = pMainFrm;
  }

  //050701
  bool IsPlus() const {return m_bPlus;}
  double GetRecTypeAndTime(int nRec, char* pType, char* pTime, char* pTWord);
  double GetRecTypeAndTime(const char* pBuf, char* pType, char* pTime, char *pTrWord = 0);

  //050712 - файл хранит режим ввода
  DWORD GetInputMode() const {return m_dwMode;}

  //050721
  const char* GetLocSubj() const
  {
    return m_hdrMain.chLocSubj;
  }

  const char* GetLocRec() const
  {
    return m_hdrMain.chLocRec;
  }

  //050726
  LRESULT MatExport(void);

  //050802
  CString GetStartDate()
  {
    CString str(m_hdrMain.chStartDate);
    str.Truncate(8);
    return str;

  }

  CString GetStartTime()
  {
    CString str(m_hdrMain.chStartTime);
    str.Truncate(8);
    return str;
  }
};

//--------------------------------------------------------------------------------
class CDizFileDialog: public CFileDialogImpl<CDizFileDialog>
{
private:
  TCHAR m_strPath[MAX_PATH];

public:
	BEGIN_MSG_MAP(CDizFileDialog)
		NOTIFY_CODE_HANDLER(CDN_FILEOK, _OnFileOK)
	END_MSG_MAP()

	CDizFileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		HWND hWndParent = NULL)
		: CFileDialogImpl<CDizFileDialog>(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent)
	{ }

	BOOL OnFileOK(LPOFNOTIFY /*lpon*/)
	{
    GetFolderPath(m_strPath,MAX_PATH);
		return TRUE;
	}
	void OnFolderChange(LPOFNOTIFY /*lpon*/)
	{
	}

  CString GetPath() const
  {
    CString str(m_strPath);
    return str;
  }
};
