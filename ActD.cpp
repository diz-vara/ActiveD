// ActD.cpp : main source file for ActD.exe
//

#include "stdafx.h"

#include "resource.h"

#include "ActDView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

#include "ActD.h"
#include <math.h>
#include <AtlBase.h>

CAppModule _Module;
CSettings _Settings;
float g_flNaN;
double g_dbNaN;


//040618 - вставляем модули от Act2
dOPEN_DRIVER_ASYNC OPEN_DRIVER_ASYNC;
dUSB_WRITE USB_WRITE;
dREAD_MULTIPLE_SWEEPS READ_MULTIPLE_SWEEPS;
dREAD_POINTER READ_POINTER;
dCLOSE_DRIVER_ASYNC CLOSE_DRIVER_ASYNC;
dCLOSE_DRIVER CLOSE_DRIVER;
CString g_strHelpFile;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
HMODULE hLib;

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;
	int nRet(-1);

  HANDLE hMutexOneInstance = ::CreateMutex( NULL, FALSE,
      _T("Global\\ActD-87F914F8-81FD-4fcc-B401-31E59913BCC3"));

  BOOL  bAlreadyRunning = ( ::GetLastError() == ERROR_ALREADY_EXISTS || 
                      ::GetLastError() == ERROR_ACCESS_DENIED);

try 
{
		OPEN_DRIVER_ASYNC = 0;
		USB_WRITE = 0;
		READ_MULTIPLE_SWEEPS = 0;
		READ_POINTER = 0;
		CLOSE_DRIVER_ASYNC = 0;
		CLOSE_DRIVER = 0;


		//init


		// To begin with - Load Library
		if(!(hLib = LoadLibrary("Labview_DLL.dll")))
			throw("Drivers are not present");
		if ( !(OPEN_DRIVER_ASYNC = (dOPEN_DRIVER_ASYNC)GetProcAddress(hLib,"OPEN_DRIVER_ASYNC")) ) throw("No OPEN_DRIVER_ASYNC");
		if ( !(USB_WRITE = (dUSB_WRITE)GetProcAddress(hLib,"USB_WRITE")) ) throw("No USB_WRITE");
		if ( !(READ_MULTIPLE_SWEEPS = (dREAD_MULTIPLE_SWEEPS)GetProcAddress(hLib,"READ_MULTIPLE_SWEEPS")) ) throw("No READ_MULTIPLE_SWEEPS");
		if ( !(READ_POINTER = (dREAD_POINTER)GetProcAddress(hLib,"READ_POINTER")) ) throw("No READ_POINTER");
		if ( !(CLOSE_DRIVER_ASYNC = (dCLOSE_DRIVER_ASYNC)GetProcAddress(hLib,"CLOSE_DRIVER_ASYNC")) ) throw("No CLOSE_DRIVER_ASYNC");
		if ( !(CLOSE_DRIVER = (dCLOSE_DRIVER)GetProcAddress(hLib,"CLOSE_DRIVER")) ) throw("No CLOSE_DRIVER");
    if (bAlreadyRunning)
    {
      throw("Not first instance");
    }

}
	catch (char* cp) {
    cp;
    OPEN_DRIVER_ASYNC = 0;
	}

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

  unsigned long nan[2]={0xffffffff, 0x7fffffff};
  g_dbNaN = *( double* )nan;
  g_flNaN = (float)g_dbNaN;


  wndMain.ShowWindow(nCmdShow);

	nRet = theLoop.Run();

	_Module.RemoveMessageLoop();

  CloseHandle(hMutexOneInstance);
	//_CrtDumpMemoryLeaks();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}



actMode::actMode(long lSpeed) : bMk2(false)
{

  if (lSpeed < 0)
  {
    nSpeed = -1;
		nBufChans = 0;
		nPinChans = 0;
		nFrq = 0;
		nTouch = 0;
    return;
  }

  bMk2 = ((lSpeed & 0x40) != 0);
	nSpeed = (short)( (lSpeed & 7)+(lSpeed>>1 & 8));
  if (bMk2) nSpeed += 16;
  ATLTRACE("actMode ctor (%d) = nSpeed = %d\n",lSpeed,nSpeed);

	  switch(nSpeed) {
		  case 0:
			  nBufChans = 258;
			  nPinChans = 256;
			  nFrq = 2048;
			  nTouch = 0;
			  break;
		  case 1:
			  nBufChans = 130;
			  nPinChans = 128;
			  nFrq = 4096;
			  nTouch = 0;
			  break;
		  case 2:
			  nBufChans = 66;
			  nPinChans = 64;
			  nFrq = 8192;
			  nTouch = 0;
			  break;
		  case 3:
			  nBufChans = 34;
			  nPinChans = 32;
			  nFrq = 16384;
			  nTouch = 0;
			  break;
		  case 4:
			  nBufChans = 258;
			  nPinChans = 232;
			  nFrq = 2048;
			  nTouch = 8;
			  break;
		  case 5:
			  nBufChans = 130;
			  nPinChans = 104;
			  nFrq = 4096;
			  nTouch = 8;
			  break;
		  case 6:
			  nBufChans = 66;
			  nPinChans = 40;
			  nFrq = 8192;
			  nTouch = 8;
			  break;
		  case 7:
			  nBufChans = 34;
			  nPinChans = 8;
			  nFrq = 16384;
			  nTouch = 8;
			  break;
		  case 8:
			  //AIB - mode - не знаю что делать....
			  nBufChans = 290;
			  nPinChans = 256;
			  nFrq = 2048;
			  nTouch = 32;
			  break;
		  case 0+16:
		  case 1+16:
		  case 2+16:
		  case 3+16:
			  nBufChans = 610;
			  nPinChans = 152*4;
			  nFrq = 2048;
			  nTouch = 0;
			  break;
		  case 4+16:
			  nBufChans = 282;
			  nPinChans = 256;
			  nFrq = 2048;
			  nTouch = 8;
			  break;
		  case 5+16:
			  nBufChans = 154;
			  nPinChans = 128;
			  nFrq = 4096;
			  nTouch = 8;
			  break;
		  case 6+16:
			  nBufChans = 90;
			  nPinChans = 64;
			  nFrq = 8192;
			  nTouch = 8;
			  break;
		  case 7+16:
			  nBufChans = 58;
			  nPinChans = 32;
			  nFrq = 16384;
			  nTouch = 8;
			  break;
		  case 8+16:
			  //AIB - mode - не знаю что делать....
			  nBufChans = 314;
			  nPinChans = 280;
			  nFrq = 2048;
			  nTouch = 32;
			  break;
		  default:
			  nSpeed = -1;
			  nBufChans = 0;
			  nPinChans = 0;
			  nFrq = 0;
			  nTouch = 0;
			  break;
	  } 
 

}


actMode::actMode(short nMode) : bMk2(false)
{

	nSpeed = nMode;
  ATLTRACE("actMode ctor (short nMode = %d)\n",nMode);

  if (nMode & 16) bMk2 = true;

	switch(nSpeed) {
		case 0:
			nBufChans = 258;
			nPinChans = 256;
			nFrq = 2048;
			nTouch = 0;
			break;
		case 1:
			nBufChans = 130;
			nPinChans = 128;
			nFrq = 4096;
			nTouch = 0;
			break;
		case 2:
			nBufChans = 66;
			nPinChans = 64;
			nFrq = 8192;
			nTouch = 0;
			break;
		case 3:
			nBufChans = 34;
			nPinChans = 32;
			nFrq = 16384;
			nTouch = 0;
			break;
		case 4:
			nBufChans = 258;
			nPinChans = 232;
			nFrq = 2048;
			nTouch = 8;
			break;
		case 5:
			nBufChans = 130;
			nPinChans = 104;
			nFrq = 4096;
			nTouch = 8;
			break;
		case 6:
			nBufChans = 66;
			nPinChans = 40;
			nFrq = 8192;
			nTouch = 8;
			break;
		case 7:
			nBufChans = 34;
			nPinChans = 8;
			nFrq = 16384;
			nTouch = 8;
			break;
		case 8:
			//AIB - mode - не знаю что делать....
			nBufChans = 290;
			nPinChans = 256;
			nFrq = 2048;
			nTouch = 32;
			break;
		  case 0+16:
		  case 1+16:
		  case 2+16:
		  case 3+16:
			  nBufChans = 610;
			  nPinChans = 152*4;
			  nFrq = 2048;
			  nTouch = 0;
			  break;
		  case 4+16:
			  nBufChans = 282;
			  nPinChans = 256;
			  nFrq = 2048;
			  nTouch = 8;
			  break;
		  case 5+16:
			  nBufChans = 154;
			  nPinChans = 128;
			  nFrq = 4096;
			  nTouch = 8;
			  break;
		  case 6+16:
			  nBufChans = 90;
			  nPinChans = 64;
			  nFrq = 8192;
			  nTouch = 8;
			  break;
		  case 7+16:
			  nBufChans = 58;
			  nPinChans = 32;
			  nFrq = 16384;
			  nTouch = 8;
			  break;
		  case 8+16:
			  //AIB - mode - не знаю что делать....
			  nBufChans = 314;
			  nPinChans = 280;
			  nFrq = 2048;
			  nTouch = 32;
			  break;
		default:
			nSpeed = -1;
			nBufChans = 0;
			nPinChans = 0;
			nFrq = 0;
			nTouch = 0;
			break;
	} 

}


double __cdecl Round(double db, int numdigits)
{
double p,r,q,e10;

     e10 = log(10.);
     if ( numdigits  > 0 )
          p = floor(exp(e10*(numdigits)+0.001));
     else if (numdigits < 0) // 24.06.2000 : яЁш єърчрэшш <= 0
          p = 1;                                            // яЁюёЄю юъЁєуы хь фю Ўхыюую
		 else
          p = 1;                                            // яЁюёЄю юъЁєуы хь фю Ўхыюую

		 db = db * p;
     r = modf(db, &q);                       //т r їЁрэшЄё  Ўхыр  ўрёЄ№, т p - фЁюсэр 
     if ( r  > 0.5 ) q += 1;  // схчєёыютэю єтхышўштрхь
     else if ( r < -0.5 ) q -= 1;  // схчєёыютэю єьхэ№°рхь
     else if ( fabs(r) == 0.5 ) {       // ёрьюх шэЄхЁхёэюх: Ёютэю 0.5
          if ( fmod(q, 2) )        // юёЄрЄюъ хёЄ№ - ўшёыю эхўхЄэюх
               q += r*2;                          // p є эрё ышсю 0.5, ышсю -0.5; p*2 = 1 шыш -1
     }
     
     q = q / p;

     return q;
}
//////////
// 041016
void CSettings::SetRegistryKey(LPCTSTR lpszRegistryKey)
{
	TCHAR szPath[MAX_PATH];

	m_strRegistryKey = _T("Software\\");
	m_strRegistryKey += lpszRegistryKey;
  //050729 - changed from CSIDL_LOCAL_APPDATA to CSIDL_COMMON_DOCUMENTS
	SHGetFolderPath(0,CSIDL_COMMON_DOCUMENTS ,NULL, SHGFP_TYPE_CURRENT, szPath);
	m_strConfFile = szPath;
	m_strConfFile += "\\ActD\\";
	CreateDirectory(m_strConfFile,0);
  m_strConfFile += "ActiveD.cfg";
  m_strConfTitle = "ActiveD.cfg";
	m_strConfFile = _Settings.GetConfFileName("\\Settings\\","CfgFile",m_strConfFile);
	m_strConfTitle = _Settings.GetConfFileName("\\Settings\\","CfgTitle",m_strConfTitle);
	//WriteProfileString("ActD","CfgPath",szPath);
	//m_strConfTitle = szPath;
	//m_strConfFile += m_strConfTitle;
	return;
}


CString CSettings::GetConfFileName(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
		CString strKey = m_strRegistryKey + lpszSection;
		CRegKey key;

		if(key.Open(HKEY_CURRENT_USER, strKey, KEY_READ) == ERROR_SUCCESS)
		{
			DWORD dwcbNeeded = 0;
			DWORD dwType;
			if(key.QueryValue(lpszEntry, &dwType, 0, &dwcbNeeded) == ERROR_SUCCESS)
			{
				CString strVal;
				LPTSTR pszVal = strVal.GetBuffer(dwcbNeeded);
				
				if(key.QueryValue(lpszEntry, &dwType, pszVal, &dwcbNeeded) == ERROR_SUCCESS)
				{
          strKey = strVal;
					strVal.ReleaseBuffer();
					key.Close();
					return strKey;
				}
				else
				{
					strVal.ReleaseBuffer(0);
				}
			}
      else
      {
        key.SetStringValue(lpszEntry, lpszDefault);
      }

		}
    else
    {
      key.Create(HKEY_CURRENT_USER, strKey);
      key.SetStringValue(lpszEntry, lpszDefault);
    }

		key.Close();
		return CString(lpszDefault);
}


CString CSettings::SetConfFile(LPCTSTR lpszDefault, LPCTSTR lpszTitle)
{
  m_strConfFile = lpszDefault;
  m_strConfTitle = lpszTitle;
		CString strKey = m_strRegistryKey + "\\Settings";
		CRegKey key;

		if(key.Open(HKEY_CURRENT_USER, strKey, KEY_WRITE) == ERROR_SUCCESS)
		{
      key.SetStringValue("CfgFile", lpszDefault);
      key.SetStringValue("CfgTitle", lpszTitle);
		}

		key.Close();
		return CString(lpszDefault);
}


void CSettings::SaveColors()
{
const char Section[] = "Colors";
CString str;

	WriteValue(Section, "BGcolor",m_colorBG);
	WriteValue(Section, "ColorSet", m_nColSet);
	WriteValue(Section, "ColorOne", m_colorOne);
	WriteValue(Section, "Color2Odd", m_color2Odd);
	WriteValue(Section, "Color2Even", m_color2Even);
	if (m_nNumColors > m_arColors.GetSize())
		m_nNumColors = m_arColors.GetSize();
	WriteValue(Section, "NumColors", m_nNumColors);
	WriteValue(Section, "ColArSize", m_arColors.GetSize());
	for (int i = 0; i < m_arColors.GetSize(); i++)
	{
		str.Format("Color_%d",i);
		WriteValue(Section, str, m_arColors[i]);
	}
  //050429 - sep color for bipolars
  WriteValue(Section, "bBPsep",m_bBPsep);
	WriteValue(Section, "BGBPcolor",m_colorBGBP);

  //050504 - TriggerColors
  WriteStruct(Section,"TrigColors", m_colorTrig, sizeof(m_colorTrig));
  WriteValue(Section,"TrigAct",m_wTrigAct);

  //050504 21:17 - загрузка параметров программы
  WriteValue("General","ProgLoad",m_bProgLoad);
  WriteValue("General","ProgSave",m_bProgSave);
  WriteValue("General","WinLoad",m_bWinLoad);
  WriteValue("General","WinLoadInd",m_nWinLoadInd);

  //050505 18:55 - сохранение смещений и их цветов
   WriteValue(Section,"OffsMv1",m_nOffsMV[0]);
   WriteValue(Section,"OffsMv2",m_nOffsMV[1]);
   WriteValue(Section,"OffsMv3",m_nOffsMV[2]);
   WriteValue(Section,"OffsMv4",m_nOffsMV[3]);

   WriteValue(Section,"OffsColor1", m_colorOffs[0]);
   WriteValue(Section,"OffsColor2", m_colorOffs[1]);
   WriteValue(Section,"OffsColor3", m_colorOffs[2]);
   WriteValue(Section,"OffsColor4", m_colorOffs[3]);
   WriteValue(Section,"OffsColor5", m_colorOffs[4]);
   WriteValue(Section,"OffsColorAnavail", m_colorOffsAnavail);
   WriteValue(Section,"OffsColorOff", m_colorOffsOff);
   WriteValue(Section,"OffsColorNone", m_colorOffsNone);

    //050712 - система хранения файлов
    WriteValue("General","FileFolder",m_bFileFolder);
    WriteValue("General", "Folder", m_strFolder);
    WriteValue("General","ExpSubFolder",m_bExpSubFolder);
    WriteValue("General","SubjSubFolder",m_bSubjSubFolder);
    WriteValue("General", "MatlabFolder", m_strMatFolder);
    
    //050715 - BdfPlus
    WriteValue("General","BdfPLUS",m_bBdfPlus);
}

bool CSettings::LoadColors()
{
COLORREF defColor = RGB(0,255,0);
const char Section[] = "Colors";
CString str;

	m_colorBG =  GetInt(Section, "BGcolor",colorDefBG);
	m_nColSet =  GetInt(Section, "ColorSet",2);
	m_colorOne = GetInt(Section, "ColorOne",colorDefOne);
	m_color2Odd = GetInt(Section, "Color2Odd", colorDef2[0]);
	m_color2Even = GetInt(Section, "Color2Even", colorDef2[1]);
	m_nNumColors = GetInt(Section, "NumColors", 8);
	int Num = GetInt(Section, "ColArSize", 8);
	m_arColors.RemoveAll();
	for (int i = 0; i < Num; i++)
	{
		str.Format("Color_%d",i);
		m_arColors.Add(GetInt(Section, str, colorDefC[i%8]));
	}
  //050429 - sep color for bipolars
  m_bBPsep = GetInt(Section, "bBPsep",0);
	m_colorBGBP = GetInt(Section, "BGBPcolor",colorDefBG);

  //050504 - TriggerColors
  try
  {
    GetStruct(Section,"TrigColors", m_colorTrig, sizeof(m_colorTrig));
  }
  catch(...)
  {
      for (int i = 0; i < 8; i++)
      {
        m_colorTrig[i] = m_colorTrig[i+8] = colorDefC[i];
      }
  }
  m_wTrigAct = (WORD)GetInt(Section,"TrigAct",m_wTrigAct);


  //050504 21:17 - загрузка параметров программы
  m_bProgLoad = (BOOL)GetInt("General","ProgLoad",1);
  m_bProgSave = (BOOL)GetInt("General","ProgSave",1);
  m_bWinLoad  = (BOOL)GetInt("General","WinLoad",1);
  m_nWinLoadInd  = (BOOL)GetInt("General","WinLoadInd",1);

  //050505 18:38 - загрузка смещений и их цветов
  m_nOffsMV[0] = GetInt(Section,"OffsMv1",30);
  m_nOffsMV[1] = GetInt(Section,"OffsMv2",80);
  m_nOffsMV[2] = GetInt(Section,"OffsMv3",150);
  m_nOffsMV[3] = GetInt(Section,"OffsMv4",200);

  m_colorOffs[0] = GetInt(Section,"OffsColor1", RGB(  0,255,  0));
  m_colorOffs[1] = GetInt(Section,"OffsColor2", RGB(100,200,  0));
  m_colorOffs[2] = GetInt(Section,"OffsColor3", RGB(200,200,  0));
  m_colorOffs[3] = GetInt(Section,"OffsColor4", RGB(220,150,  0));
  m_colorOffs[4] = GetInt(Section,"OffsColor5", RGB(255,  0,  0));
  m_colorOffsNone = GetInt(Section,"OffsColorNone", RGB(0,  100,  220));
  m_colorOffsAnavail = GetInt(Section,"OffsColorAnavail", GetSysColor(COLOR_BTNFACE));
  m_colorOffsOff = GetInt(Section,"OffsColorOff", RGB(100,100,100));


  //050712 - система хранения файлов
  m_bFileFolder = (BOOL)GetInt("General","FileFolder",1);
  m_strFolder = GetString("General", "Folder", "");
  m_bExpSubFolder = (BOOL)GetInt("General","ExpSubFolder",1);
  m_bSubjSubFolder = (BOOL)GetInt("General","SubjSubFolder",1);
  m_strMatFolder = GetString("General", "MatlabFolder", "");

  //050715 - BdfPlus
  m_bBdfPlus = (BOOL)GetInt("General","BdfPLUS",1);

	return false;
}


//050505 18:58
// выдает цвет, соответствующий смещению
COLORREF CSettings::GetOffsColor(int nIdx) const
{
  if (nIdx == 0)
    return m_colorOffsOff;
  if (nIdx < 0 || nIdx > 5)
    return m_colorOffsNone;
  else
    return m_colorOffs[nIdx - 1];
}


int CSettings::GetOffsIdx(int nOffs) const
{
  for (int i = 0; i < 4; i++)
  {
    if (nOffs < m_nOffsMV[i])
      return i;
  }
  return 4;

}

//050506 - Acqusition and Trigger parameters
BOOL CSettings::LoadAcqAndTrigParams()
{
const char Section[] = "AcqAndTrig";

  m_acqtrParams.nFrameLen = GetInt(Section,"FrameLen",1000);
  m_acqtrParams.nPreTrig = GetInt(Section,"PreTrig",0);
  m_acqtrParams.nPostTrig = GetInt(Section,"PostTrig",1000);
  m_acqtrParams.nIdle = GetInt(Section,"TrigIdleTime",300);
  m_acqtrParams.nAcqMode = GetInt(Section,"AcqMode",0);
  m_acqtrParams.bAnd[0] = GetInt(Section,"And0",0);
  m_acqtrParams.bAnd[1] = GetInt(Section,"And1",0);
  m_acqtrParams.bAnd[2] = GetInt(Section,"And2",0);
  m_acqtrParams.dwKeyStart = GetInt(Section,"KeyStart",0);
  m_acqtrParams.dwKeyStop = GetInt(Section,"KeyStop",0);
  m_acqtrParams.wTrLoHi = GetInt(Section,"TrigLoHi",0);
  m_acqtrParams.wTrHiLo = GetInt(Section,"TrigHiLo",0);
  m_acqtrParams.wHi[0] = GetInt(Section,"Gate0Hi",0);
  m_acqtrParams.wLo[0] = GetInt(Section,"Gate0Lo",0);
  m_acqtrParams.wHi[1] = GetInt(Section,"Gate1Hi",0);
  m_acqtrParams.wLo[1] = GetInt(Section,"Gate1Lo",0);
  m_acqtrParams.wHi[2] = GetInt(Section,"Gate2Hi",0);
  m_acqtrParams.wLo[2] = GetInt(Section,"Gate2Lo",0);
  m_uTimer = GetInt(Section,"MainTimer",250);

  return TRUE;
}

void CSettings::WriteAcqAndTrigParams()
{
const char Section[] = "AcqAndTrig";

  WriteValue(Section,"MainTimer",m_uTimer);
  WriteValue(Section,"FrameLen",m_acqtrParams.nFrameLen);
  WriteValue(Section,"PreTrig",m_acqtrParams.nPreTrig);
  WriteValue(Section,"PostTrig",m_acqtrParams.nPostTrig);
  WriteValue(Section,"TrigIdleTime",m_acqtrParams.nIdle);
  WriteValue(Section,"AcqMode",m_acqtrParams.nAcqMode);
  WriteValue(Section,"And0",m_acqtrParams.bAnd[0]);
  WriteValue(Section,"And1",m_acqtrParams.bAnd[1]);
  WriteValue(Section,"And2",m_acqtrParams.bAnd[2]);
  WriteValue(Section,"KeyStart",m_acqtrParams.dwKeyStart);
  WriteValue(Section,"KeyStop",m_acqtrParams.dwKeyStop);
  WriteValue(Section,"TrigLoHi",m_acqtrParams.wTrLoHi);
  WriteValue(Section,"TrigHiLo",m_acqtrParams.wTrHiLo);
  WriteValue(Section,"Gate0Hi",m_acqtrParams.wHi[0]);
  WriteValue(Section,"Gate0Lo",m_acqtrParams.wLo[0]);
  WriteValue(Section,"Gate1Hi",m_acqtrParams.wHi[1]);
  WriteValue(Section,"Gate1Lo",m_acqtrParams.wLo[1]);
  WriteValue(Section,"Gate2Hi",m_acqtrParams.wHi[2]);
  WriteValue(Section,"Gate2Lo",m_acqtrParams.wLo[2]);

}

//050506 2100
BOOL CSettings::IsChanNames()
{
char Buf[22];

  DWORD dw = GetPrivateProfileSection("ChanNames",Buf,20,m_strConfFile);
  if (dw < 18)
    return 0;
  else
    return 1;


}

BOOL CSettings::LoadChanNames(char *pNames)
{
CString str;
/*
char defName[16][8] = 
{
"Fp1",
"Fp2",
"F4",
"Fz",
"F3",
"T7",
"C3",
"Cz",
"C4",
"T8",
"P4",
"Pz",
"P3",
"O1",
"Oz",
"O2"
};

    for(int i = 0; i < 16; i++)
    {
      str.Format("Chan%d",i+1);
      GetPrivateProfileString("ChanNames",str,defName[i],pNames+i*8,7,m_strConfFile);
    }
*/
    for( int i = 0; i < 256; i++)
    {
      str.Format("Chan%d",i+1);
      GetPrivateProfileString("ChanNames",str,"",pNames+i*8,7,m_strConfFile);
    }

    for(i = 0; i < 8; i++)
    {
      str.Format("Tou%d",i+1);
      GetPrivateProfileString("ChanNames",str,"",pNames+(i+256)*8,7,m_strConfFile);
    }
  return true;
}

BOOL CSettings::SaveChanNames(char *pNames)
{
CString str;

  for(int i = 0; i < 256; i++)
    {
      str.Format("Chan%d",i+1);
      *(pNames+i*8+7) = 0;
      WritePrivateProfileString("ChanNames",str,pNames+i*8,m_strConfFile);
    }

    for(i = 0; i < 8; i++)
    {
      str.Format("Tou%d",i+1);
      *(pNames+i*8+7) = 0;
      WritePrivateProfileString("ChanNames",str,pNames+(i+256)*8,m_strConfFile);
    }


  return 1;
}
/////////////////////////////////////////////////////////////////////////////
// class _ChanW
int _ChanW::Put(long Data) 
{
	int nRes(1);
	if (m_pBuf == 0 || m_dwLen <=0 /*|| m_nFreeCnt < 1*/)
		return 0;

  if (m_dwWpos >= m_dwLen)
  {
    ATLTRACE("Writebuffer!!!!!!@!!!!!");
    m_dwWpos = 0;
  }
	
	m_pBuf[m_dwWpos++] = Data; 

  //проверка на конец буфера
  if (m_dwWpos >= m_dwLen)
	{
		m_dwWpos = 0; 
		nRes = -1; //признак перехода
	}

  m_nFreeCnt--;
  m_nUseCnt++;
  return nRes;
}

long _ChanW::Get() 
{
	if (m_pBuf == 0 || m_dwLen <=0 /*|| m_nUseCnt < 1*/)
		return -1;

	if (m_dwRpos >= (long)m_dwLen)
	{
		m_dwRpos = 0;
	}
  m_nUseCnt--;
  m_nFreeCnt++;
	return m_pBuf[m_dwRpos++];
}


void _ChanW::ResetRW(int nNew) 
{
  //050412 - при начале ставлю указатель в минус первую точку
  if (m_bBeg && m_pBuf)
  {
    m_bBeg = false;
    m_dwRpos = -1;
    m_dwWposOld = 0;
    m_pBuf[m_dwRpos] = m_pBuf[m_dwWposOld];
  }
  else
  {
    //050411 - ставлю указатель чтения на 1 позади записи
    if (m_dwWposOld > 0)
      m_dwRpos = m_dwWposOld - 1;
    else 
      m_dwRpos = m_dwLen - 1;
  }
  //ATLTRACE("ResetRW R = %d, W = %d, W-R = %d\n", m_dwRpos, m_dwWpos, (m_dwWpos-m_dwRpos)%m_dwLen);
  m_nNew = nNew;

}


int Find(UINT *Array, int nSize, UINT nValue)
{
  for (int i = 0; i < nSize; i++)
  {
    if (nValue == Array[i])
      return i;
  }
  return -1;
}

/////////////////////////////////////////////////////////////////////////////

