// ActD.h

#pragma once

#ifndef TRIAL 
# define TRIAL 0
#endif


#define TOTCHANS 32*20

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_DELETE_HANDLE(p) { if(p) { DeleteObject(p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#define WM_DIZ_CHANGEMODE			WM_USER+1
#define WM_DIZ_MAPCHANGED			WM_USER+2
#define WM_DIZ_VIEW_ACTIVATE	WM_USER+4
#define WM_DIZ_VIEW_CLOSE			WM_USER+3
#define WM_DIZ_DLG_ACTIVATE		WM_USER+5
#define WM_DIZ_DLG_CLOSE		  WM_USER+6

#define WM_DIZ_PLOT		        WM_USER+10
#define WM_DIZ_TEST		        WM_USER+20
#define WM_DIZ_SETACTIVE		  WM_USER+21
#define WM_DIZ_SETFRQ		      WM_USER+22
#define WM_DIZ_CFCHANGED      WM_USER+23
#define WM_DIZ_CHANGE         WM_USER+24
#define WM_DIZ_VTIMER         WM_USER+25
#define WM_DIZ_NEWFRAME       WM_USER+26
#define WM_DIZ_CHANGETIMER    WM_USER+27
#define WM_DIZ_SETZERO        WM_USER+28
#define WM_DIZ_SETSCROLL      WM_USER+29
#define WM_DIZ_SNAPINIT       WM_USER+30 //050531 (WPARAM)(&m_arChanW), (LPARAM)(&m_atrParams)
#define WM_DIZ_SNAPPLOT       WM_USER+31 //050531 (WPARAM)(&m_arChanW), (LPARAM)(&m_atrParams)
#define WM_DIZ_READFRAME      WM_USER+32
#define WM_DIZ_SHOWMESS       WM_USER+33
#define WM_DIZ_QUIT           WM_USER+34




typedef HANDLE (*dOPEN_DRIVER_ASYNC)(void);
typedef unsigned char (*dUSB_WRITE)(HANDLE HAN, unsigned char *data);
typedef unsigned char (*dREAD_MULTIPLE_SWEEPS)(HANDLE HAN, long *data, long nBytesToRead);
typedef unsigned char (*dREAD_POINTER)(HANDLE HAN, unsigned long *Pointer);
typedef unsigned char (*dCLOSE_DRIVER_ASYNC)(HANDLE HAN);
typedef unsigned char (*dCLOSE_DRIVER)(HANDLE HAN);

class actMode {
private:
	short nSpeed;
	int nFrq;
	//общее количество буферизумых каналов
	int nBufChans;
	//количество "обычных" каналов
	int nPinChans;
	int nTouch;
  bool bMk2;
public:
	//ctors
	actMode(long lSpeed = 0); 
	actMode(short nMode);

	short GetMode() const {return nSpeed;}
	int GetFrq() const {return nFrq;}
	//количество забуферизованных каналов
	int GetBufChans() const {return nBufChans;}
	//количество "обычных" каналов
	int GetPinChans() const {return nPinChans;}
	int GetTouch() const {return nTouch;}
};



#define BUFSIZE 1024*1024*4				//32M

class fltCf2
{
public:
	double b[3];
	double a[3];
	//ctor
	fltCf2(double b0 = -1, double b1 = -1, double b2 = -1, double a0 = -1, double a1 = -1, double a2 = -1)
	{
		b[0] = b0; b[1] = b1;  b[2] = b2;
		a[0] = a0; a[1] = a1;  a[2] = a2;
	}
	bool Is() {return (a[0] != -1);}
};

class fltCf1
{
public:
	double b[2];
	double a[2];
	//ctor
	fltCf1(double b0 = -1, double b1 = -1, double a0 = -1, double a1 = -1)
	{
		b[0] = b0; b[1] = b1;
		a[0] = a0; a[1] = a1;
	}
	bool Is() {return (a[0] != -1);}

};


double __cdecl Round(double db, int numdigits = -1);


//050506 - Acqusition and Trigger parameters
class AcqTrParams
{
public:
  int nFrameLen;
  int nPreTrig;
  int nPostTrig;
  int nAcqMode;
  BOOL bAnd[3];
  WORD wTrLoHi;
  WORD wTrHiLo;
  WORD wHi[3];
  WORD wLo[3];
  DWORD dwKeyStart;
  DWORD dwKeyStop;
  int nIdle;

  //ctor
  AcqTrParams() :
    nFrameLen(2000),
    nPreTrig(0),
    nPostTrig(2000),
    nAcqMode(0),
    wTrLoHi(0),
    wTrHiLo(0),
    dwKeyStart(0),
    dwKeyStop(0),
    nIdle(250)
    {
      for (int i = 0; i < 3; i++)
      {
        bAnd[i] = 0;
        wHi[i] = 0;
        wLo[i] = 0;
      }

    }

} ;



class CSettings
{
friend class CTrigViewDlg;
friend class CProgSetPage;
friend class COffColPage;
friend class CAcqTrigDlg;

private:
	CString m_strRegistryKey;
	LPTSTR m_pszAppName;
	LPTSTR m_pszProfileName;
	//041014
	CString m_strConfFile; 
	CString m_strConfTitle; 
protected:
	COLORREF  colorDefBG;
	COLORREF  colorDefOne;
	COLORREF colorDef2[2];
	COLORREF colorDefC[8];
  

public:
	CSettings() :
    m_bBPsep(false),
		m_colorBG(0), m_colorBGBP(0),
		m_colorOne(RGB(10,10,250)),
		m_color2Even(RGB(0,200,0)), 
		m_color2Odd(RGB(200,0,0)),
		m_nColSet(0),
		m_nNumColors(7),
		colorDefBG(0),
		colorDefOne(RGB(0,255,0)),
    m_wTrigAct(0xFFFC),
    m_bProgLoad(0),
    m_bProgSave(0),
    m_bWinLoad(0),
    m_nWinLoadInd(0),
    m_bBdfPlus(true)
		{
			colorDef2[0] = RGB(0,255,0);
			colorDef2[1] = RGB(255,0,0);

			colorDefC[0] = RGB(000,255,000);
			colorDefC[1] = RGB(000,128,255);
			colorDefC[2] = RGB(255,255,000);
			colorDefC[3] = RGB(255,128,000);
			colorDefC[4] = RGB(255,000,255);
			colorDefC[5] = RGB(255,000,000);
			colorDefC[6] = RGB(128,128,255);
			colorDefC[7] = RGB(128,128, 64);

      for (int i = 0; i < 8; i++)
      {
        m_colorTrig[i] = m_colorTrig[i+8] = colorDefC[i];
      }
		
		}

	void SetRegistryKey(LPCTSTR lpszRegistryKey = "DIZ\\ActD");
	CString GetConfFileName(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);

	//050428(home) - write and read structure
	void WriteStruct(LPCTSTR sSect, LPCTSTR sKey, void *ptr, size_t size)
	{
    //ATLTRACE("WriteStruct %08X %d\n", ptr,size);
		::WritePrivateProfileStruct(sSect, sKey, ptr, size, m_strConfFile);
	}

	BOOL GetStruct(LPCTSTR sSect, LPCTSTR sKey, void *ptr, size_t size)
	{
    //ATLTRACE("GetStruct %08X %d\n", ptr,size);
    BOOL bRet = ::GetPrivateProfileStruct(sSect, sKey, ptr, size, m_strConfFile);
    if (!bRet) 
      throw(0);
    return bRet;
	}

	void WriteValue(LPCTSTR sSect, LPCTSTR sKey, int Value)
	{
		CString str;
		str.Format("%d",Value);
		::WritePrivateProfileString(sSect, sKey, str,m_strConfFile);
	}

	void WriteValue(LPCTSTR sSect, LPCTSTR sKey, DWORD Value)
	{
		CString str;
		str.Format("%d",Value);
		::WritePrivateProfileString(sSect, sKey, str,m_strConfFile);
	}

	void WriteValue(LPCTSTR sSect, LPCTSTR sKey, LPCTSTR sStr)
	{
		::WritePrivateProfileString(sSect, sKey, sStr,m_strConfFile);
	}

	void WriteValue(LPCTSTR sSect, LPCTSTR sKey, double dbValue)
	{
	CString str;
		str.Format("%f",dbValue);
		::WritePrivateProfileString(sSect, sKey, str,m_strConfFile);
	}
	
	int GetInt(LPCTSTR sSect, LPCTSTR sKey, int nDefault = 0)
	{
		return ::GetPrivateProfileInt(sSect, sKey, nDefault ,m_strConfFile);
	}
	
  CString GetString(LPCTSTR sSect, LPCTSTR sKey, LPCTSTR pDefault = 0)
  {
    char strTmp[258];
    ::GetPrivateProfileString(sSect, sKey, pDefault, strTmp, 256, m_strConfFile);
    return CString(strTmp);
  }
	
	bool IsFileFormatPlus()
  {
    return (m_bBdfPlus != 0);
  }
	
	void SaveColors();
	bool LoadColors();

  //050506 - Acqusition and Trigger parameters
  BOOL LoadAcqAndTrigParams();
  void WriteAcqAndTrigParams();
  AcqTrParams GetAcqAndTrigParams() const {return m_acqtrParams;}

  //050611 - colors for non-avail and off 
  COLORREF GetOffColor() const
  {
    return m_colorOffsOff;
  }
  COLORREF GetNonAvailColor() const
  {
    return m_colorOffsAnavail;
  }

  UINT GetTimer()
  {
    if (m_uTimer >= 50 && m_uTimer <= 1000)
      return (UINT)m_uTimer;
    else
      return 250;
  }

  //050801
  CString SetConfFile(LPCTSTR lpszDefault, LPCTSTR lpszTitle = 0);
  CString GetConfFileName() const { return m_strConfFile;}
  CString GetConfFileTitle() const { return m_strConfTitle;}

protected:
	COLORREF m_colorBG;
	COLORREF m_colorBGBP;
	COLORREF m_colorOne;
	COLORREF m_color2Odd;
	COLORREF m_color2Even;
	CSimpleArray<COLORREF> m_arColors;
  COLORREF m_colorTrig[16];
  WORD m_wTrigAct;
	int m_nColSet;
	int m_nNumColors;
  //050429 - separate color for bipolar background
  BOOL m_bBPsep;

  //050504 - program and window settings
  BOOL m_bProgLoad;
  BOOL m_bProgSave;
  BOOL m_bWinLoad;
  //0 for def, 1 for Ind
  int m_nWinLoadInd;

  //050505 - offset mapping
  int m_nOffsMV[4];
  COLORREF m_colorOffs[5];
  //050610 - color offs for NONE
  COLORREF m_colorOffsAnavail;
  COLORREF m_colorOffsOff;
  COLORREF m_colorOffsNone;

  //050506 - Acqusition and Trigger parameters
  AcqTrParams m_acqtrParams;

  //050526 - BdfPlus file format
  BOOL m_bBdfPlus;

  //050712 - система хранения файлов
  BOOL m_bFileFolder;
  CString m_strFolder;
  BOOL m_bExpSubFolder;
  BOOL m_bSubjSubFolder;
  int m_uTimer;

  //050726
  CString m_strMatFolder;

public:

  int m_nFileMode;
  BOOL IsFileFolder() const {return m_bFileFolder;}
  BOOL IsExpSubFolder() const {return m_bExpSubFolder;}
  BOOL IsSubjSubFolder() const {return m_bSubjSubFolder;}
  CString GetFileFolder() const {return m_strFolder;}
  CString SetFileFolder(const char* sFolder) {m_strFolder = sFolder; return m_strFolder;}

  COLORREF GetBgColor() const {return m_colorBG;}
	
  COLORREF GetBPColor() const 
  {
    if (m_bBPsep)
      return m_colorBGBP;
    else
      return m_colorBG;
  }

  const COLORREF*  GetTrigColors() const 
  {
    return m_colorTrig;
  }

  WORD GetTrigWord() const
  {
    return m_wTrigAct;
  }

	COLORREF GetColor(int Chan) const {
		if (m_nColSet == 0)
			return m_colorOne;
		else if (m_nColSet == 1)
		{
			if (Chan % 2)
				return m_color2Odd;
			else
				return m_color2Even;
		}
		else 
		{
			return m_arColors[Chan%m_nNumColors];
		}
	}

  //возвращает -1, если не нужно, 0 для деф и 1 для инд
  int GetWinLoad() const
  {
    if (!m_bWinLoad)
      return -1;
    else
      return m_nWinLoadInd;
  }

  BOOL GetProgLoad() const
  {
    return m_bProgLoad;
  }

  BOOL GetProgSave() const
  {
    return m_bProgSave;
  }
  
  //050505 18:58
  // выдает цвет, соответствующий смещению
  COLORREF GetOffsColor(int nIdx) const;
  int GetOffsIdx(int nMV) const;

  //050506 2100
  BOOL IsChanNames();
  BOOL LoadChanNames(char *pNames);
  BOOL SaveChanNames(char *pNames);

  //050726
  CString GetMatlabFolder() const {return m_strMatFolder;}
  CString SetMatlabFolder(const char* sFolder) 
  {
    m_strMatFolder = sFolder; return m_strMatFolder;
  }

};




//050207 - ввожу классы каналов

class _ChanW
{
public:
	//ctor
	_ChanW(short NumOff = -1, short Num = -1, DWORD Off = 0, DWORD Len = 0, int nFrq = 0, const char* pName = 0, int Rate = 1) : 
      m_nNumG(NumOff), 
      m_nNum(Num),
      m_dwOffset(Off), 
      m_dwLen(Len), 
		  m_dwWpos(0), 
      m_dwRpos(0), 
      m_pBuf(0), 
      m_nUseCnt(0), 
      m_nFreeCnt(Len), 
      m_dwWposOld(0),
      m_bBeg(true) ,
      m_nFrq(nFrq),
      m_nDecRate(Rate)
      {
        ZeroMemory(m_chNum,8);
        if (m_nNum < 0)
        {
          strcpy(m_chNum,"Trig");
          strcpy(m_chLabel,"TRIGGER");
        }
        else if (m_nNum < 256)
        {
          sprintf(m_chNum,"%c%d",(char)(m_nNum/32)+'A', m_nNum%32 + 1);
        }
        else
        {
          sprintf(m_chNum,"EXT%d",m_nNum-256 + 1);
        }
        ZeroMemory(m_chLabel,8);
        if (pName)
          strncpy(m_chLabel,pName,8);
      }

	short GetChanOff() const {return m_nNumG;}
	short GetChanNum() const {return m_nNum;}
	long* SetBuff(long* pBuf) 
  {
    //050412 - буфер начинается на одну точку позже
    m_pBuf = pBuf + m_dwOffset + 1 ; 
    m_bBeg = true; 
    return m_pBuf;
  }
	long* GetBuff() const {return m_pBuf;}
	DWORD GetLen() const {return m_dwLen;}
  int GetFrq() const {return m_nFrq;}
	int Put(long Data);
	long Get();
  int GetUsed() const {return m_nUseCnt;}
  int GetFree() const {return m_nFreeCnt;}
  void ResetRW(int nD);
  //050411 - запомненная позиция записи (перед началом очередного ввода)
  void StoreW() {m_dwWposOld = m_dwWpos; m_nNew = 0;}
  int GetNew() const {return m_nNew;}

  //050518
  int GetDecRate() const {return m_nDecRate;}

  //050520 - откатывает указатель назад на nCnt точек
  void Reverse(int nRevCnt)
  {
    m_dwRposOld = m_dwRpos;
    m_dwRpos -= nRevCnt;
    if (m_dwRpos < 0)
      m_dwRpos += m_dwLen;
  }

  bool CheckPos() 
  {
    if (m_dwRpos == m_dwRposOld)
      return true;

    m_dwRpos = m_dwRposOld;
    return false;
  }

  //050609 - получение метки канала - 8 байт!!!
  const char* const GetChanLabel() const {return m_chLabel;}
  void GetChanLabel(char* pDst) const 
  {
    CopyMemory(pDst, m_chLabel, 8);
  }

protected:
	//номер в глобальном списке
	short m_nNumG;
	//номер в картах
	short m_nNum;
	//смещение в буфере
	DWORD m_dwOffset;
	//количество записей
	DWORD m_dwLen;
	//адрес буфера канала
	long *m_pBuf;
	DWORD m_dwWpos;
	
  long m_dwRpos;
  long m_dwRposOld;

  int m_nUseCnt;
  int m_nFreeCnt;
  //050411 - запомненная позиция записи (перед началом очередного ввода)
  DWORD m_dwWposOld;
  //050411 - флаг первого ввода
  bool m_bBeg;
  //050419 - кол-во новых точек
  int m_nNew;
  //050419 - отсчетов на секунду
  int m_nFrq;
  //050518 - обратная децимация: во сколько раз данного канала больше
  int m_nDecRate;
  //050507 - имя канала
  char m_chNum[8];
  char m_chLabel[8];
};

class $ChanG
{
};

extern CSettings _Settings;
extern dUSB_WRITE USB_WRITE;
extern int Find(UINT *Array, int nSize, UINT nValue);
extern float g_flNaN;
extern double g_dbNaN;
extern CString g_strHelpFile;

#define SIZE(a) (sizeof(a)/sizeof(UINT))

//050514 - defines for trigger and gate states
#define GATE_ON 1
#define GATE_OFF 2
#define TRIG_ON 4
//#define TRIG_OFF -2
//#define TRIG_ON_GATE_ON 3
#define MAN_ON 8
#define MAN_OFF 16

class U64
{
protected:
  union
  {
    struct 
    {
      DWORD dwLow;
      DWORD dwHigh;
    };
    unsigned hyper uQuad;
  };
public:
  U64(DWORD dw = 0): dwLow(dw), dwHigh(0) {}
  U64(U64& u) : uQuad(u.uQuad) {}
  U64(FILETIME ftm) : dwLow(ftm.dwLowDateTime), dwHigh(ftm.dwHighDateTime) {}

  const U64& operator = (FILETIME ftm) 
  {
    dwLow = ftm.dwLowDateTime;
    dwHigh = ftm.dwHighDateTime;
    return *this;
  }

  const U64& operator = (unsigned hyper U) 
  {
    uQuad = U;
    return *this;
  }

  operator unsigned hyper() {return uQuad;}
  operator LARGE_INTEGER() 
  {
    LARGE_INTEGER L;
    L.QuadPart = uQuad;
    return L;
  }
  operator FILETIME() 
  {
    FILETIME ftm;
    ftm.dwLowDateTime = dwLow;
    ftm.dwHighDateTime = dwHigh;
    return ftm;
  }

};



class int24;

class Long2Bytes
{
public:
  union
  {
    long Lng;
    char Byte[4];
  };
  Long2Bytes(long L = 0) : Lng(L) {}
  Long2Bytes(int24 I24);
};

class int24
{
  friend class Long2Bytes;
private:
  char Byte[3];
public:
  //ctors
  int24(long Long = 0) 
  { 
    Long2Bytes Tmp = Long;
    for (int i = 0; i < 3; i++)
      Byte[i] = Tmp.Byte[i];
  }
  int24& operator=( const long Long) 
  {
    Long2Bytes Tmp = Long;
    for (int i = 0; i < 3; i++)
      Byte[i] = Tmp.Byte[i];
    return *this;
  }
  operator long() { Long2Bytes LB(*this); return LB.Lng; } 
};


typedef struct Pointer24
{
  //0 - free, 1 - needs data, 2 - writed, needs close/reopen
  BYTE bUsed;
  int nRec;
  bool bPlus;
  int nType; //050729 - 0 - free, 1 - trig
  int24* pBuf;
  int *m_ChanCounters;
} Pointer24;

#define cRed    RGB(255,100,0)
#define cGreen  RGB(0,255,100)
#define  cBlue  RGB(0,100,200)


