#include "StdAfx.h"
#include ".\datafile.h"
#include "MainFrm.h"
#include "HdrDlg.h"

Long2Bytes::Long2Bytes(int24 I24)
{
  for (int i = 0; i < 3; i++)
  {
    Byte[i] = I24.Byte[i];
  }
  if (I24.Byte[2] < 0)
    Byte[3] = -1;
  else
    Byte[3] = 0;
}

CDataFile::CDataFile(const CMainFrame* pMainFrm) :
			m_nChanNum(0)
		 //,m_pFChans(0)
		 ,m_pBuf24(0)
		 ,m_pWChans(0)
		 ,m_pcsWChans(0)
		 ,m_hFile(INVALID_HANDLE_VALUE)
		 ,m_dwCounter(0)
		 ,m_nRecCnt(0)
		 ,m_u64StartTime(0)
		 ,m_u64RunTime(0)
		 ,m_nFrq(0)
    ,m_bPlus(0)
    ,m_pChanNames(0)
    ,m_nCode(0)
    ,m_nExtChans(0)
    ,m_nPinChans(0)
    ,m_nRecSize(0)
    ,m_nHdrSize(0)
    ,m_nRecordedChans(0)
    ,m_pHdr(0)
    ,m_nTrLen(0)
    ,m_nChLen(0)
    ,m_pMainFrm(pMainFrm)
    ,m_i64Zero(0LL)
    ,m_dwMode(0)
    ,m_bSkipped(false)
    ,m_nAnnLen(0)
{
  for(int i = 0; i < 4; i++)
  {
    m_Pointers[i].bUsed = 0;
    m_Pointers[i].bPlus = 0;
    m_Pointers[i].nType = 0;
    m_Pointers[i].m_ChanCounters = 0;
    m_Pointers[i].pBuf = 0;
  }
  m_pChanNames = new char[TOTCHANS*8];
  ZeroMemory(m_pChanNames,TOTCHANS*8);
  m_i64StartPos.QuadPart = 0;
  m_i64CurrPos.QuadPart = 0;
  m_i64Zero;
  m_PntRead.pBuf = 0;

  //050804
  if (!InitializeCriticalSectionAndSpinCount(&m_csFile, 0x80000400) ) 
      return;

}

CDataFile::~CDataFile(void)
{
  //ATLTRACE("CDataFileSize %d",sizeof(*this));
  SAFE_DELETE_ARRAY(m_pChanNames);
  //SAFE_DELETE_ARRAY(m_pFChans);
  SAFE_DELETE_ARRAY(m_pBuf24);
  SAFE_DELETE_ARRAY(m_pWChans);
  SAFE_DELETE(m_pHdr);
  //050804
  DeleteCriticalSection(&m_csFile);

  for(int i = 0; i < 4; i++)
  {
    SAFE_DELETE_ARRAY(m_Pointers[i].m_ChanCounters);
  }
}


//&m_arChanW, m_nChans,m_nExtChans,m_pMainMap,m_LeftDlg.GetChanNames() ,&m_atrParams, &m_csWChans, this);
const char* const CDataFile::FileInit(CSimpleArray<_ChanW> *parChanW, AcqTrParams* pTrParams, CRITICAL_SECTION *pCS, DWORD dwMode)
{
_ChanW chanWork;
CChanWrite chanWrite;
int nLength(0);
DWORD dwOffs(0);
int i;
int chan;
LARGE_INTEGER i64Zero;

  i64Zero.QuadPart = 0;


  m_bNeedStart = false;
  m_bNeedStop = false;
  m_bWriting = false;
  m_bNeedReOpen = false;
  m_bSkipped = false;
  m_flNewTime = 0;

  m_pcsWChans = pCS;

  SAFE_DELETE_ARRAY(m_pWChans);
  //SAFE_DELETE_ARRAY(m_pFChans);
  SAFE_DELETE_ARRAY(m_pBuf24);

  m_dwMode = dwMode;

  for(i = 0; i < 4; i++)
  {
    SAFE_DELETE_ARRAY(m_Pointers[i].m_ChanCounters);
  }
  
  m_nRecordedChans = parChanW->GetSize();
  m_pWChans = new _ChanW[m_nRecordedChans];

  m_nDur = pTrParams->nFrameLen;
  m_flDur = (double)m_nDur/1000.;



  //создаю новый заголовок подо все каналы 
  int nFChans(m_nRecordedChans);
  m_bPlus = _Settings.IsFileFormatPlus();
  int nPlus = (m_bPlus) ? 1:0;

  if (m_bPlus)
  {
    m_nAnnLen = 64;
    dwOffs = m_nAnnLen;
    nFChans++;
  }
  else 
    dwOffs = 0;


  sprintf(m_hdrMain.chChanNum,"%d",nFChans);
  sprintf(m_hdrMain.chRecDur,"%.3f", m_flDur);
  
  if (m_bPlus)
    strncpy(m_hdrMain.chVersion,"24BIT BDF+D",43);
  else
    strncpy(m_hdrMain.chVersion,"24BIT                ",43);

  SAFE_DELETE(m_pHdr);
  m_pHdr = new CBDFheader(nFChans);
  //устанавливаю размер
  sprintf(m_hdrMain.chNumBytes,"%d",m_pHdr->GetSize());
  //и копирую в него содержимое
  CopyMemory(m_pHdr->m_pMainHdr, &m_hdrMain, sizeof(BDFh));

  //массив завожу на ВСЕ каналы - но он может быть не полностью заполнен
	m_arFChans.RemoveAll();// = new CChanWrite[m_nRecordedChans];

  //обнуление массива имен каналов
  ZeroMemory(m_pChanNames,TOTCHANS*8);
  ZeroMemory(m_bMap, TOTCHANS);

  if (m_bPlus)
  {
    m_pHdr->SetChanType(0,2);
    m_pHdr->SetChanLabel(0,"Annotat",0);
    m_pHdr->SetSamples(0, 64);
    m_pHdr->SetChanRes(0,"EDF Annotation - text, 64*3 bytes");
  }


  m_nChanNum = 0;   //счетчик реальных каналов
  m_nPinChans = 0;
  m_nExtChans = 0;

  for (i = 0; i < m_nRecordedChans; i++)
  {

    chanWork = parChanW->operator [](i);
    chan = chanWork.GetChanNum();


    if (chan < 0)
    {
      m_cwTrChan.nNum = chan;
      m_cwTrChan.nLength = (int)Round((double)m_nDur * chanWork.GetFrq() / 1000.,0);
      m_cwTrChan.nType = (chan >= 0) ? 1:0;
      m_nTFrq = chanWork.GetFrq();
      m_nTrLen = m_nTFrq * m_nDur / 1000;
      m_cwTrChan.nPreLen = (int)Round((double)pTrParams->nPreTrig * chanWork.GetFrq() / 1000.,0);

      //m_nChanNum++;
    }
    else
    {
      chanWrite.nNum = chan;
      chanWrite.nLength = (int)Round((double)m_nDur * chanWork.GetFrq() / 1000.);
      chanWrite.nType = (chan >= 0) ? 1:0;
      chanWrite.dwOffs = dwOffs;
      ATLTRACE("_Chan %d offset %d\n",chan,dwOffs);
      chanWrite.nPreLen = (int)Round((double)pTrParams->nPreTrig * chanWork.GetFrq() / 1000.,0);
      chanWork.GetChanLabel(m_pChanNames+chan*8);
      chanWrite.flCal = 1;
      chanWrite.nOff = 0;
      m_nFrq = chanWork.GetFrq();
      if (chan >= 256)
        m_nExtChans = 8;
      else if (chan > m_nPinChans-1)
        m_nPinChans = chan+1;


      m_bMap[chan] = -1;
      m_nChLen = m_nDur * chanWork.GetFrq() / 1000;
      m_pHdr->SetSamples(m_nChanNum+nPlus, m_nChLen);

			m_arFChans.Add(chanWrite); //[m_nChanNum++] = chanWrite;
      m_pHdr->SetChanType(m_nChanNum+nPlus,1, m_nFrq,chanWork.GetChanNum());
      m_pHdr->SetChanLabel(m_nChanNum+nPlus,chanWork.GetChanLabel(), m_bPlus);
      m_nChanNum++;
      dwOffs += chanWrite.nLength;
    }
    
  }

  //050715 - помещаю тригканал на последнее место
  m_pHdr->SetChanType(m_nChanNum+nPlus,0);
  m_pHdr->SetChanLabel(m_nChanNum+nPlus, "Trigger", m_bPlus);
  m_pHdr->SetSamples(m_nChanNum+nPlus, m_nTrLen);

  m_cwTrChan.dwOffs = dwOffs;
  ATLTRACE("TrigChan %d offset %d\n",m_cwTrChan.nNum,dwOffs);
  dwOffs += m_cwTrChan.nLength;


	//if (m_nChanNum == 0)
	//	SAFE_DELETE_ARRAY(m_pFChans);
  m_nRecSize = dwOffs;
  if (dwOffs)
  {
    //выделяю буфер четырехкратного размера
    m_pBuf24 = new int24[dwOffs*4];
  }

  for (i = 0; i < 4; i++)
  {
    m_Pointers[i].bUsed = 0;
    m_Pointers[i].nType = 0;
    m_Pointers[i].bPlus = m_bPlus;
    m_Pointers[i].pBuf = m_pBuf24 + dwOffs * i;
    //в следующих строчках +1 - для тригканала (это - для записи, так что он есть всегда!!!)
    if ( 0 != (m_Pointers[i].m_ChanCounters = new int[m_nRecordedChans]))
			ZeroMemory(m_Pointers[i].m_ChanCounters,(m_nRecordedChans)*sizeof(int));
  }
  m_dwCounter = 0;
  //050520 - сразу помечаю первый буфер как записыываемый
  // 050523 - не надо!!!
  //m_Pointers[0].bUsed = true;
  DWORD dwW(0);
  SetFilePointer(m_hFile,0,0,FILE_BEGIN);
  WriteFile(m_hFile,m_pHdr->m_pBuf,m_pHdr->GetSize(), &dwW,0);
  SetFilePointerEx(m_hFile,i64Zero,&m_i64StartPos, FILE_CURRENT);

  return m_bMap;

}


LRESULT CDataFile::NewFrame(int Num, CSimpleArray<_ChanW> *parChanW)
{
int nCnt;
DWORD dwTrig;
WORD wTrigCommand;
WORD wManCommand;
WORD wTrig;
//int24 *pBuf;
int24 *pChanBuf;
long lValue;
int nBuf(0);
int nNewB(0);
CString str;
DWORD dwOffs;
DWORD dwW;
//050701 - pointer to annotation
char *pAnn;

//050805
static WORD wOldTrig(0);
WORD wTSet;
WORD wTClear;
int *pCounter;

  if (parChanW->GetSize() != m_nRecordedChans )
  {
    ATLTRACE("Number of channels changed\n");
    return 0;
  }

  //ATLTRACE("Frame received\n");
  
  //050804
  EnterCriticalSection(&m_csFile);


  //050523 - проверка на записанные буфера
  if (m_bNeedReOpen)
  {
    //ATLTRACE("Closing buffers ");
    CloseAndReopen();
    for (nBuf = 0; nBuf < 4; nBuf++)
    {
      if (m_Pointers[nBuf].bUsed == 2)
      {
        m_Pointers[nBuf].bUsed = 0;
        //ATLTRACE("%d(%08X) ", nBuf, m_Pointers[nBuf].pBuf);
      }
    }
    //ATLTRACE("\n");
  }


  int nPointsReсeived = Num;

  //pBuf = m_Pointers[m_nCurrentBuf].pBuf;

  for (int nChanIdx = 0; nChanIdx < m_nRecordedChans; nChanIdx++)
  {
    m_pWChans[nChanIdx] = parChanW->operator [](nChanIdx);
    //??????????051129 - где-то здесь я и промахиваюсь.... 
    // но почему только в режиме децимации без децимации триггера??????
    // (промашка возникает, если убрать след. строчку)
    m_pWChans[nChanIdx].Get(); //skip first (repeated) value
  }

  int nDecRate;

  bool bNeedNew(false);

  for (nCnt = 0; nCnt < nPointsReсeived; nCnt++)
  {
    nDecRate = 0;
    for (int nChanIdx = 0; nChanIdx < m_nRecordedChans; nChanIdx++)
    {
        //pCounter = m_Pointers[m_nCurrentBuf].m_ChanCounters + nChanIdx;
        //pChanBuf = pBuf + m_pChans[nChanIdx].dwOffs;  

      for (int nDecRate = 0; nDecRate < m_pWChans[nChanIdx].GetDecRate(); nDecRate++ )
      {

        lValue = m_pWChans[nChanIdx].Get();
        if (m_pWChans[nChanIdx].GetChanNum() == -1)
        {
          m_dwCounter++;

          

          bNeedNew = false;

          dwTrig = (DWORD)lValue;
          wTrig = (WORD)(dwTrig & 0xFFFF);
          wTSet = (wTrig ^ wOldTrig) & wTrig;
          wTClear = (wTrig ^ wOldTrig) & (~wTrig);

          wTrigCommand = (WORD)(dwTrig>>16);
          wManCommand = wTrigCommand & 0x00F0;
          wTrigCommand &= 0x0F00;
          if (wManCommand)
          {
            //ATLTRACE("ManCommand %04X  ", wManCommand);
            switch(wManCommand)
            {
            case 0x0040: //man end
              m_bNeedStop = true;
              break;
            case 0x0080:  //man start
              if (!m_bWriting)
                bNeedNew = true;
              m_bNeedStop = false;
              break;
            case 0x00C0:  //man start-man end
              if (!m_bWriting)
                bNeedNew = true;
              m_bNeedStop = true;
              break;
            default:
              break;
            }

          }
          if (wTrigCommand)
          {
            //ATLTRACE("TrigCommand %04X  ", wTrigCommand);
            switch(wTrigCommand)
            {
            case 0x0100:
              ATLTRACE("Gate OFF at %d\n", nCnt);
              m_bNeedStop = true;
              break;
            case 0x0200:
              bNeedNew = true;
              m_bNeedStop = false;
              ATLTRACE("Gate ON at %d\n",nCnt);
              break;
            case 0x0300:
              bNeedNew = true;
              m_bNeedStop = true;
              ATLTRACE("Gate ON-OFF\n");
              break;
            case 0x0400:
              ATLTRACE("TRIG ON\n");
              //050729 - я хочу отменить все незаконченные свободные буфера
              for (int nB = 0; nB < 4; nB++)
              {
                if (m_Pointers[nB].bUsed == 1 && m_Pointers[nB].nType == 0)
                {
                  m_nRecNum--;
                  m_Pointers[nB].bUsed = 0;
                  ZeroMemory(m_Pointers[nB].m_ChanCounters,m_nRecordedChans*sizeof(int));
                }
              }
              bNeedNew = false;
              //здес нужно найти следующий свободный буфер
              if ( (nNewB = FindFreeBuf()) >= 0)
              {
                ATLTRACE("PreTrig writings to buf %d\n", nNewB);
                // записать в него все претриггерные точки
                WritePre(nNewB, nCnt, wTrig, wTSet, wTClear);

                m_bWriting = true;
                m_bNeedStop = true;
              }
              else
              {
                ATLTRACE("!~!!! No free buffers...\n");
              }
              break;
            } //end of switch
          } //end of wTrigCommand

          if (bNeedNew && !m_bWriting) //пришел свежий гейт - а запись не ведется
          {
            //свежий гейт - нужен новый буфер????
            if ((nNewB = FindFreeBuf()) >= 0)
            {
              if (m_nRecCnt == 0)
              {
                SetStartTime(m_dwCounter-1);
              }
              m_nRecCnt++;


              //ATLTRACE("NewBuf %d\n",nNewB);
              m_Pointers[nNewB].bUsed = 1;
              m_Pointers[nNewB].nType = 0;
              
              //050714 - учитываю запрет записи при начале буфера
              
              if (IsFile() == GENERIC_WRITE && !m_pMainFrm->m_bNoRec)
                m_Pointers[nNewB].nRec = m_nRecNum++;
              else
                m_Pointers[nNewB].nRec = -1;
              

              m_bWriting = true;
              if (m_bPlus)
              {
                pAnn = (char*)m_Pointers[nNewB].pBuf;
                ZeroMemory(pAnn,64*3);
                m_flNewTime = GetPointTime(m_dwCounter-1);
                sprintf(pAnn,"+%.3f\024NEW\024 0x%04X  set%04X clr%04X\024\0", 
                  m_flNewTime, wTrig, wTSet, wTClear);
              }

            }
            else
            {
              //ATLTRACE("No free buffers...\n");
            }
          }
          dwOffs = m_cwTrChan.dwOffs;
          wOldTrig = wTrig;
        } // end of TrigChan
        else
        {
          //analog channel
          //-1 здесь будет проходит до тех пор, пока тригканал будет идти первым
          dwOffs = m_arFChans[nChanIdx-1].dwOffs;
        }

        //if (m_bWriting) //этот флаг может либо уже стоять - либо был установлен только что????
        //{
          for (nBuf = 0; nBuf < 4; nBuf++)
          {
            if (m_Pointers[nBuf].bUsed == 1)
            {
              pCounter = (m_Pointers[nBuf].m_ChanCounters + nChanIdx);
              //проверяю только на тригканале (он идет первым)
              if (m_pWChans[nChanIdx].GetChanNum() == -1 && *pCounter >= m_cwTrChan.nLength)
              {
                ATLTRACE("\nEnd of Buf %d \n", nBuf);
                if (IsFile() == GENERIC_WRITE && m_Pointers[nBuf].nRec >= 0)
                {
                  _Settings.m_nFileMode = 3;
                  SetFilePointerEx(m_hFile, m_i64Zero, 0, FILE_END);
                  WriteFile(m_hFile, m_Pointers[nBuf].pBuf, m_nRecSize*3, &dwW, 0);
                }

                //Если инициализация прошла, отправляем сообщение в главное окно
                if (m_pMainFrm)
                {
                  ::PostMessage(m_pMainFrm->m_hWnd,WM_DIZ_SNAPPLOT,(WPARAM)(&m_Pointers[nBuf]),(LPARAM)(m_Pointers[nBuf].nRec));
                  ATLTRACE("Posting WM_DIZ_SNAPPLOT with nRec = %d\n",m_Pointers[nBuf].nRec);
                }


                //помечаю буфер как нуждающийся в завершении
                m_Pointers[nBuf].bUsed = 2;
                //и чищу его счетчики
                ZeroMemory(m_Pointers[nBuf].m_ChanCounters,m_nRecordedChans*sizeof(int));
                m_bNeedReOpen = true;

                if (m_bNeedStop)
                {
                  ATLTRACE("Recording stopped (%d)\n", nBuf);
                  m_bWriting = false;
                }
                else
                {
                  nNewB = FindFreeBuf(nBuf);
                  if (nNewB >= 0)
                  {
                    ATLTRACE("Switching to Buffer %d\n", nNewB);
                    m_Pointers[nNewB].bUsed = 1;
                    m_Pointers[nNewB].nType = 0;
                    if (m_bPlus)
                    {
                      pAnn = (char*)m_Pointers[nNewB].pBuf;
                      ZeroMemory(pAnn,64*3);
                      float flTime(GetPointTime(m_dwCounter-1));
                      
                      //wTrig = (WORD)( (long)m_Pointers[nNewB].pBuf[m_cwTrChan.dwOffs] & 0xFFFF);
                      if (m_bSkipped && !m_pMainFrm->m_bNoRec)
                      {
                        m_bSkipped = false;
                        m_flNewTime = flTime;
                        sprintf(pAnn,"+%.3f\024NEW\024 0x%04X  set%04X clr%04X\024\0", 
                          m_flNewTime, wTrig, wTSet, wTClear);
                      }
                      else
                      {
                        sprintf(pAnn,"+%.3f\024cnt %.3f\0240x%04X\024\0", flTime, flTime - m_flNewTime, wTrig);
                      }
                    }

                    if (IsFile() == GENERIC_WRITE && !m_pMainFrm->m_bNoRec)
                      m_Pointers[nNewB].nRec = m_nRecNum++;
                    else
                    {
                      m_Pointers[nNewB].nRec = -1;
                      m_bSkipped = true;
                    }
                  }
                  else
                  {
                    ATLTRACE("No free buffers....\n");
                    m_bWriting = false;
                  }
                }
              }
              else
                m_bWriting = true;

              if (m_bWriting)
              {
                pChanBuf = m_Pointers[nBuf].pBuf+dwOffs;
                pChanBuf[(*pCounter)++] = lValue;
              }
            } // конец условия использования буфера
          } //конец цикла по буферам
        //} //конец услвоия m_bWriting
      } //конец цикла по децимационым повторам

    } //конец цикла по каналам

    //обнуляю флаг нужды в записи - если нужно, он будет переустановлен
    //m_bWriting = false;

  } //конец цикла по полученным отсчетам

  //если уже ничего не пишем - но есть что закрывать
  if (!m_bWriting && m_bNeedReOpen)
  {
    //ATLTRACE("Closing buffers ");
    CloseAndReopen();
    for (nBuf = 0; nBuf < 4; nBuf++)
    {
      if (m_Pointers[nBuf].bUsed == 2)
      {
        m_Pointers[nBuf].bUsed = 0;
        //ATLTRACE("%d", nBuf);
      }
    }
    //ATLTRACE("\n");
  
  }

  //050804
  LeaveCriticalSection(&m_csFile);

  return S_OK;
}

DWORD CDataFile::Run()
{
  CMessageLoop theLoop;
	//_Module.AddMessageLoop(&theLoop);
	m_idThread = GetCurrentThreadId();
  theLoop.AddMessageFilter(this);

	ATLTRACE("FileThread-Run Thread %x\n", GetCurrentThreadId());


	DWORD nRet = theLoop.Run();
  ATLTRACE("FileThread %X exit \n",m_idThread );
	//_Module.RemoveMessageLoop();
	return nRet;
}


BOOL CDataFile::PreTranslateMessage(MSG* pMsg)
{

  if (pMsg->message == WM_DIZ_NEWFRAME)
  {
    if (m_pcsWChans)
    {
      EnterCriticalSection(m_pcsWChans);
      //ATLTRACE("FILE csWChans entered\n");
    }
    NewFrame((int)pMsg->wParam, (CSimpleArray<_ChanW>*)pMsg->lParam);
    if (m_pcsWChans)
    {
      LeaveCriticalSection(m_pcsWChans);
      //ATLTRACE("FILE csWChans leaved\n");
    }
    return TRUE;
  }
  return false;
}





int CDataFile::FindFreeBuf(int nCurBuf)
{
int nNewB(0);

    for (nNewB = nCurBuf; nNewB < 4; nNewB++)
    {
      if (m_Pointers[nNewB].bUsed == 0)
        break;
    }

    if (nNewB > 3)
    {
      for (nNewB = 0; nNewB < nCurBuf; nNewB++)
        if (m_Pointers[nNewB].bUsed == false)
          break;
      if (nNewB == nCurBuf)
        nNewB = -1;
    }

    //ATLTRACE("FindFreeBuf(%d) retured %d\n", nCurBuf, nNewB);
  return nNewB;
}



//Эта ф-ия должна произвести запись в указанный буфер
// претриггерных данных - до (но не включая)
// точку nCnt. Информация о кол-ве точек - в рабочем массиве каналов

int CDataFile::WritePre(int nBuf, int nPreCnt, WORD wTrig, WORD wSet, WORD wClear)
{
int nCnt;
int nPreLen;
int24 *pBuf;
int24 *pChanBuf;
int *pCounter;
CString str;
//050701-pointer to annotation
char *pAnn;

  //050714 - учитываю запрет записи при начале буфера
  if (IsFile() == GENERIC_WRITE && !m_pMainFrm->m_bNoRec)
    m_Pointers[nBuf].nRec = m_nRecNum++;
  else
    m_Pointers[nBuf].nRec = -1;


  m_Pointers[nBuf].bUsed = 1;
  m_Pointers[nBuf].nType = 1;

  pBuf = m_Pointers[nBuf].pBuf;
  if (m_nRecCnt == 0)
  {
    SetStartTime((long)m_dwCounter - m_cwTrChan.nPreLen);
  }

  m_nRecCnt++;

  for (int nChanIdx = 0; nChanIdx < m_nRecordedChans; nChanIdx++)
  {
    if (nChanIdx == 0)
    {
      pChanBuf = pBuf + m_cwTrChan.dwOffs;  
      nPreLen = m_cwTrChan.nPreLen;
    }
    else
    {
      pChanBuf = pBuf + m_arFChans[nChanIdx-1].dwOffs;  
      nPreLen = m_arFChans[nChanIdx-1].nPreLen;
    }

      pCounter = m_Pointers[nBuf].m_ChanCounters + nChanIdx;
      m_pWChans[nChanIdx].Reverse(nPreLen);
      for (nCnt = 0; nCnt < nPreLen; nCnt++)
      {
         pChanBuf[(*pCounter)++] = m_pWChans[nChanIdx].Get();
      } // end of counter loop
      if (!m_pWChans[nChanIdx].CheckPos())
        ATLTRACE("промахнулся на канале %d",nChanIdx);
  } // end of chan loop

  //1. Обязательно должна быть хотя бы одна посттриггерная точка!!!
  //2. Надо каким-то образом проверять, что в итоге я оказался там же, где и был....
  if (m_bPlus)
  {
    pAnn = (char*)pBuf;
    ZeroMemory(pAnn,64*3);
    pAnn += sprintf(pAnn,"+%.3f\024Trg\024", GetPointTime(m_dwCounter - m_cwTrChan.nPreLen));
    pAnn++; //trailing zero
    pAnn += sprintf(pAnn,"+%.3f\0240x%04X set%04X clr%04X\024",
      GetPointTime(m_dwCounter), wTrig, wSet, wClear);
    pAnn++; //trailing zero

    ATLTRACE((char*)pBuf);
    ATLTRACE("\n");
  }

  return 0;
}

//ф-ия должна закрывать файл (если таковой есть) - и заново открывать его
BOOL CDataFile::CloseAndReopen()
{
  if (IsFile() == GENERIC_WRITE)
  {
    FlushFileBuffers(m_hFile);
    CloseHandle(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
    m_hFile = CreateFile(m_strFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (m_hFile == INVALID_HANDLE_VALUE)
    {
      MessageBox(m_pMainFrm->m_hWnd,"Error opening file","ERROR",MB_OK|MB_ICONSTOP);
      m_nCode = 0;
      _Settings.m_nFileMode = m_nCode;
    }
  }

  m_bNeedReOpen = false;
  return TRUE;
}



BOOL CDataFile::NewFile() 
{
CString str;
char Path[MAX_PATH];

  ZeroMemory(&m_hdrMain, sizeof(BDFh));

  strncpy(m_hdrMain.chID," BIOSEMI",8);
  m_hdrMain.chID[0] = -1;
  strncpy(m_hdrMain.chRecNum,"-1",8);

  CHeaderDlg dlgH;


  if (dlgH.DoModal() != IDOK)
    return false;

  CFileDialog dlg(FALSE, "*.bdf",0,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"Biosemi *.bdf files\0*.bdf");

  //На этом месте установка необходимых кодов уже проверена
  if (_Settings.IsFileFolder())
  {
    str = _Settings.GetFileFolder();
    //корневой каталог для данных
    if (str.IsEmpty())
    {
      //здесь нужно вызвать фолдер-браузинг с возможностью создания
      BROWSEINFO bi;

      bi.hwndOwner = m_pMainFrm->m_hWnd;
      bi.pidlRoot = 0;
      bi.pszDisplayName = Path;
      bi.lpszTitle = "Select folder for data storage";
      bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
      bi.lpfn = 0;
      bi.lParam = 0;
      bi.iImage = 0;
      ITEMIDLIST* pIT = SHBrowseForFolder(&bi);
      if (pIT == 0)
        return false;

	    IMalloc* pMalloc;
	    SHGetMalloc(&pMalloc); 
	    SHGetPathFromIDList(pIT,Path);
	    pMalloc->Free(pIT);

      _Settings.SetFileFolder(Path);
      str = Path;
    }

    //здесь у нас базовый фолдер установлен


    if (_Settings.IsExpSubFolder())
    {
      str += "\\";
      str += dlgH.GetExpStr();
    }

    if (_Settings.IsSubjSubFolder())
    {
      str += "\\";
      str += dlgH.GetSubjStr();
    }
    str += "\\";

    strncpy(Path,str,MAX_PATH);
    HANDLE hDir = CreateFile(Path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,0);
    if (hDir == INVALID_HANDLE_VALUE)
    {
      str.Format("Directory %s does not exist.\n   Create it?",Path);
      if (IDYES == ::MessageBox(m_pMainFrm->m_hWnd,str,"no directory", MB_YESNO))
        SHCreateDirectoryEx(0,Path,0);
      else
        Path[0] = 0;
    }
    else
    {
      //MessageBox(m_pMainFrm->m_hWnd,Path,"debug info",MB_OK);
      CloseHandle(hDir);
    }
    dlg.m_ofn.lpstrInitialDir = Path;
  }


  CString strName = dlgH.GetSubjStr();
  SYSTEMTIME stm;
  GetLocalTime(&stm);
  char chDate[10];
  GetDateFormat(MAKELCID(MAKELANGID(9,0),SORT_DEFAULT),0,&stm,"_yyMMdd",chDate,8); //
  strName += chDate;
  GetTimeFormat(0,TIME_FORCE24HOURFORMAT,&stm,"_hhmm_",chDate,7); //
  strName += chDate;

  strncpy(dlg.m_ofn.lpstrFile,strName,dlg.m_ofn.nMaxFileTitle);


  dlgH.GetLocSubj(m_hdrMain.chLocSubj);
  dlgH.GetLocRec(m_hdrMain.chLocRec);
  dlgH.GetShortDate(m_hdrMain.chStartDate);

  if (dlg.DoModal() == IDOK)
  {
    m_nRecNum = 0;
    m_nRecCnt = 0;

    m_strFile = dlg.m_ofn.lpstrFile;
    try
    {
      m_hFile = CreateFile(m_strFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
      if (m_hFile == INVALID_HANDLE_VALUE)
        throw("Error opening file");
      //WriteFile(m_hFile, &m_hdrMain,sizeof(m_hdrMain), &dwW,0);
      m_strFileTitle = dlg.m_szFileTitle;
      m_strFileTitle.Truncate(m_strFileTitle.Find('.'));

    }
    catch(char *cp)
    {
      if (cp)
        MessageBox(m_pMainFrm->m_hWnd,cp,"ERROR",MB_OK|MB_ICONSTOP);
      if (m_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);
      m_hFile = INVALID_HANDLE_VALUE;
      return false;
    }

  }
  else
    return false;


  m_nRecCnt = 0; 
  m_u64StartTime = 0;
  m_nCode = GENERIC_WRITE;
  _Settings.m_nFileMode = m_nCode;

  return true;


}


void CDataFile::SetStartTime(long Cnt)
{
char chDate[10];
SYSTEMTIME stm;
FILETIME ftm;
U64 U;
  //Если пишем - определить стартовое время
  // я инкрементирую счетчик в начале - так что мне нужна пред. точка
  ZeroMemory(chDate,10);
  ATLTRACE("SetStartTime(%ld)\n", Cnt);

  m_dwStartPoint = Cnt;
  m_flNewTime = 0;
  if (m_u64RunTime && m_nTFrq)
  {
    U = m_u64StartTime = m_u64RunTime + (unsigned hyper)m_dwStartPoint*10000000/m_nTFrq;
    ftm = U;
    FileTimeToSystemTime(&ftm,&stm);
    GetTimeFormat(0,TIME_FORCE24HOURFORMAT,&stm,"hh.mm.ss",chDate,8); //
    strncpy(m_hdrMain.chStartTime,chDate,8);


  }
}


//возвращает время (в мс) относительного "стартового"
float CDataFile::GetPointTime(DWORD cnt)
{
DWORD Time(0);

  //ATLTRACE("GetPointTime(%ld)\n", cnt);
  if (m_u64StartTime == 0 || m_nTFrq == 0)
    return 0;

  //1. Время в сотнях наносекунд от начала текущего участка
  unsigned hyper u64Tm = (unsigned hyper)cnt*10000000/m_nTFrq;
  unsigned hyper u64T0 = u64Tm;
  //2. Добавив время нажатия, получу абсолютное время
  u64Tm += m_u64RunTime;
  //3. Вычитаем стартовое время - получаем смещение
  u64Tm -= m_u64StartTime;

  //ATLTRACE("%lu %lu %I64d %I64u %I64u %I64u\n", cnt, m_dwStartPoint, (m_u64StartTime-m_u64RunTime)/10000, m_u64StartTime, m_u64RunTime, u64T0);
  //ATLTRACE("Point Time %d = %f\n", cnt, (float)(u64Tm/10000)/1000.f);
  return (float)(u64Tm/10000)/1000.f;
 
  // таким образом, если RunTime один и тот же, получим
  //(cnt- m_dwStartPoint)*1000000/m_nFrq

}

LRESULT CDataFile::Open()
{
DWORD dwBytesRead;
long lDistHigh(0);
CString str;
static char chChan;
static int nChan;
LARGE_INTEGER i64Zero;
const char *cp;

  i64Zero.QuadPart = 0;
  m_bPlus = false;

  if (m_hFile != INVALID_HANDLE_VALUE)
    return 0;

  m_pHdr = new CBDFheader(0);
  CFileDialog dlg(TRUE, 0,0,0,"Biosemi *.bdf files\0*.bdf\0All Files (*.*)\0*.*");
  dlg.m_ofn.lpstrInitialDir = _Settings.GetFileFolder();

  if (dlg.DoModal() == IDOK)
  {
    CString strFile = dlg.m_ofn.lpstrFile;
    try
    {
      m_hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
      if (m_hFile == INVALID_HANDLE_VALUE)
        throw("Error opening file");

      if (!ReadFile(m_hFile, m_pHdr->m_pBuf, m_pHdr->GetSize(), &dwBytesRead, 0))
        throw("Error reading file header");

      if (dwBytesRead != m_pHdr->GetSize())
        throw("Error reading file header: wrong size of main header");

      ATLTRACE(m_pHdr->m_pMainHdr->chChanNum);
      int nChans = atoi(m_pHdr->m_pMainHdr->chChanNum);

      if (strstr(m_pHdr->m_pMainHdr->chVersion, "BDF+" ) )
        m_bPlus = true;
      else
        m_bPlus = false;

      if (nChans > 0)
      {
        SAFE_DELETE(m_pHdr);
        m_pHdr = new CBDFheader(nChans);
        m_nHdrSize = m_pHdr->GetSize();

        SetFilePointerEx(m_hFile,i64Zero,0,FILE_BEGIN);
        if (!ReadFile(m_hFile, m_pHdr->m_pBuf, m_pHdr->GetSize(), &dwBytesRead, 0))
          throw("Error reading file header(whole)");

        if (dwBytesRead != m_pHdr->GetSize())
          throw("Error reading file header (whole): wrong size");

        CopyMemory(&m_hdrMain,m_pHdr->m_pMainHdr,sizeof(m_hdrMain));

        m_flDur = atof(m_pHdr->m_pMainHdr->chRecDur);
        m_nDur = (int)(m_flDur*1000.);
        m_nRecNum = atoi(m_pHdr->m_pMainHdr->chRecNum);
        m_nRecordedChans = nChans;

        m_nChLen = 0;
        m_nTrLen = 0;

        m_strFileTitle = dlg.m_szFileTitle;
        m_strFileTitle.Truncate(m_strFileTitle.Find('.'));

        //обнуление массива имен каналов
        ZeroMemory(m_pChanNames,TOTCHANS*8);
        ZeroMemory(m_bMap, TOTCHANS);
        int nMaxPin(0);
        m_nRecSize = 0;
      
        //SAFE_DELETE_ARRAY(m_pFChans);
        //m_pFChans = new CChanWrite[nChans];
				m_arFChans.RemoveAll();

        int nNonRepeatingChan(0);
				CChanWrite cw;
        int nPinChan(0);
        int nExtChan(0);
        char cLabel[16];

        //для старых номеров каналов
        bool bOldChanNums(true);
        //предварительное определение того, стоят ли в поле номера или метки
        for (int chan = 0; chan < nChans; chan++)
        {
          if (strncmp(m_pHdr->GetChanType(chan),"Active",6) == 0)
          {
            str = m_pHdr->GetChanLabel(chan);
            if (sscanf(str,"EXG%d",&nChan) == 2 && nChan > 0 && nChan <= 32)
              continue;
            else if (sscanf(str,"%c%d",&chChan, &nChan) == 2 && chChan >= 'A' && chChan <= 'H' && nChan > 0 && nChan <= 32)
              continue;
            else
            {
              bOldChanNums = false;
              break;
            }
          }
        }

        for (chan = 0; chan < nChans; chan++)
        {

          if (strncmp(m_pHdr->GetChanType(chan),"Active",6) == 0)
          {
            //Active Electrode
            cw.nType = 1;
            cw.nDur = m_nDur;
            cw.nLength = m_pHdr->GetSamples(chan);
            cw.dwOffs = m_nRecSize;

            if (m_nChLen == 0)
            {
              m_nChLen = m_pHdr->GetSamples(chan);
              m_nFrq = (int)Round((double)m_nChLen/m_flDur,0);
            }
            else if (m_nChLen != m_pHdr->GetSamples(chan))
            {
              throw("Can't process file with different acquizition rates");
            }

            str = m_pHdr->GetChanLabel(chan);
            nChan = -1;

            if (m_bPlus)
            {
              //обрабока меток и номеров для моего варианта
              cp = strstr(m_pHdr->GetChanType(chan),"#");
              if (cp)
              {
                cp++;
                if (strncmp(cp,"EXG",3) == 0)
                {
                  //EXG-channel
                  sscanf(cp,"EXG%d#",&nChan);
                  nChan = 256+nChan-1;
                }
                else
                {
                  //Pin-channel
                  sscanf(cp,"%c%d#",&chChan, &nChan);
                  nChan = (int)(chChan-'A')*32 + nChan -1;
                }
                if (nChan >= 0 && nChan < TOTCHANS)
                {
                  strncpy(m_pChanNames+nChan*8,m_pHdr->GetChanLabel(chan)+4,8);
                }
              }
              else
              {
                //не сложилось
                nChan = nMaxPin++;
                chChan = (char)(nChan/32)+'A';
              }
              if (nChan < 256)
              {
                if (nMaxPin < nChan+1) nMaxPin = nChan+1;
              }
              else
              {
                m_nExtChans = 8;
              }
            }
            else
            {
              strncpy(cLabel,str,16);
              //обработка меток и номеров для ActiView 
              if (sscanf(str,"EXG%d",&nChan) > 0 && nChan > 0 && nChan < 32)
              {
                nChan = 256+nChan -1;
                m_nExtChans = 8;
              }
              else
              {
                if (bOldChanNums)
                {
                  //определяем номер канала из метки
                  sscanf(str,"%c%d",&chChan,&nChan);
                  nChan = (int)(chChan - 'A')*32+nChan-1;
                  if (nMaxPin < nChan+1) nMaxPin = nChan+1;
                }
                else
                {
                  //метка-меткой, а номер - отдельно
                  nChan = nMaxPin++;
                }
              }
              if (nChan >= 0 && nChan < TOTCHANS)
              {
                strncpy(m_pChanNames+nChan*8,cLabel,8);
              }

            }

            //050624 - таким образом я буду отсекать дублированные каналы
            if (!m_bMap[nChan])
            {
              m_bMap[nChan] = -1;
              ATLTRACE(str);
              cw.nNum = nChan;
              //вот здесь я должен ставить не значения по умолчанию - а прочитанные из файла!!!
              ReadCf(chan,&cw);
              //cw.flCal = 1;
              //cw.flOff = 0;

							m_arFChans.Add(cw);
              nNonRepeatingChan++;
            }
          } //end of ACTIVE channels
          else if (strncmp(m_pHdr->GetChanType(chan),"Trigg",5) == 0)
          {
            m_cwTrChan.nNum = -1;
            m_cwTrChan.nType = 0;
            m_cwTrChan.nDur = m_nDur;
            m_cwTrChan.nLength = m_pHdr->GetSamples(chan);
            m_cwTrChan.dwOffs = m_nRecSize;
            if (m_nTrLen == 0)
            {
              m_nTrLen = m_pHdr->GetSamples(chan);
              m_nTFrq = (int)Round((double)m_nTrLen/m_flDur,0);
            }
            //nNonRepeatingChan++;
          } //end of Trigg channel
          else if (strncmp(m_pHdr->GetChanType(chan),"EDF ann",7) == 0)
          {
            m_bPlus = true;
            m_nAnnLen = m_pHdr->GetSamples(chan);
          } //end of annotation channel
          m_nRecSize += m_pHdr->GetSamples(chan);

        }
        m_nPinChans = nMaxPin;
        m_nChanNum =  nNonRepeatingChan;

        
        //здесь я должен зафиксировать место (как базу для начала) и вычислить размер записи
        SetFilePointerEx(m_hFile,i64Zero,&m_i64StartPos, FILE_CURRENT);
        GetFileSizeEx(m_hFile,&m_szFileSize);

        m_nRecCnt = (int)((m_szFileSize.QuadPart - m_nHdrSize)/(m_nRecSize*3));
        if (m_nRecCnt < 1)
          throw("No records in file.");
        if (m_nRecCnt != m_nRecNum)
        {
          str.Format("Nuber of records mismatch.\n(%d records in header, %d found)\nCorrect?",
            m_nRecNum, m_nRecCnt);
          if(IDYES != MessageBox(0,str, "Error", MB_YESNO))
            throw(0);
          else
            m_nRecNum = m_nRecCnt;
        }
        
        //Ага, а теперь нужно прочитать первую запись. Куда????

       SAFE_DELETE_ARRAY(m_pBuf24);
       m_pBuf24 = new int24[m_nRecSize];
       m_PntRead.pBuf = m_pBuf24;
       m_PntRead.bPlus = m_bPlus;
        
        m_nCode = GENERIC_READ;
        _Settings.m_nFileMode = m_nCode;

        return S_OK;
      } //end of if (nChans > 0)
    
    }
    catch(char *cp)
    {
      if (cp)
        MessageBox(m_pMainFrm->m_hWnd,cp,"ERROR",MB_OK|MB_ICONSTOP);
      if (m_hFile != INVALID_HANDLE_VALUE)
        CloseHandle(m_hFile);
      m_hFile = INVALID_HANDLE_VALUE;
    }
  }

  return E_FAIL;
}

LRESULT CDataFile::Close()
{
DWORD dwW;

  if (IsFile())
  {
    //050701 - write number of records
    sprintf(m_hdrMain.chRecNum,"%.7d",m_nRecNum);
    SetFilePointerEx(m_hFile, m_i64Zero,0, FILE_BEGIN);
    WriteFile(m_hFile, &m_hdrMain,sizeof(m_hdrMain), &dwW,0);

    CloseHandle(m_hFile);
    SAFE_DELETE(m_pHdr);
    m_hFile = INVALID_HANDLE_VALUE;
    m_nCode = 0;
    _Settings.m_nFileMode = m_nCode;

    m_bPlus = false;
    m_nRecNum = 0;
    m_nTrLen = 0;
    m_nChLen = 0;
    m_nAnnLen = 0;
    SAFE_DELETE_ARRAY(m_pBuf24);

  }
  return S_OK;
}


bool CDataFile::ReadRecord(int nRecN, Pointer24* pReadPointer)
{
  DWORD dwBytesRead;

  if (!IsFile() || m_nRecSize == 0 || pReadPointer == 0 || pReadPointer->pBuf == 0)
    return false;

  if (nRecN < 0 || nRecN >= m_nRecNum)
    return false;

  //050804
  EnterCriticalSection(&m_csFile);

  pReadPointer->bPlus = m_bPlus;
  m_i64CurrPos.QuadPart = m_i64StartPos.QuadPart + (__int64)m_nRecSize*3*nRecN;;

  SetFilePointerEx(m_hFile,m_i64CurrPos,0,FILE_BEGIN);
  DWORD dwErr;
  if ( !ReadFile(m_hFile,pReadPointer->pBuf,m_nRecSize*3,&dwBytesRead,0))
  {
    dwErr = GetLastError();
    //050804
    LeaveCriticalSection(&m_csFile);
    return false;
  }

  if (dwBytesRead != m_nRecSize*3)
  {
    dwErr = GetLastError();
    //050804
    LeaveCriticalSection(&m_csFile);
    return false;
  }
  pReadPointer->nRec = nRecN;
  //::PostMessage(m_pMainFrm->m_hWnd,WM_DIZ_SNAPPLOT,(WPARAM)(pReadPointer),(LPARAM)m_pChans);
  //050804
  LeaveCriticalSection(&m_csFile);

  return true;

}

///////////////////////////////////////////////////////////////////
///050701
double  CDataFile::GetRecTypeAndTime(int nRec, char* pType, char* pTime, char* pTWord)
{
char *pStr;

  if (!m_bPlus || nRec < 0 || nRec >= m_nRecNum)
    return 0;
  else
  {
    ReadRecord(nRec, &m_PntRead);
    pStr = (char*)m_pBuf24;
    return GetRecTypeAndTime(pStr, pType, pTime, pTWord);
  }
}

//050714
double CDataFile::GetRecTypeAndTime(const char* pBuf, char* pType, char* pTime, char* pTrWord)
{
static char cTime[13], cType[13];
static char cTmp[64*3+1];
char *cp, *pStr;
static DWORD wTrig(0x0), wSet(0), wClr(0);
double flBaseTime(0), flBegTime(0);


    ZeroMemory(cTime,13);
    ZeroMemory(cType,13);
    CopyMemory(cTmp,pBuf,64*3);


    cp = cTmp;
    pStr = cTmp;
    while(*cp++ >= 32) ;
    *--cp = 0;
    sscanf(pStr,"%12s", cTime);
    pStr += strlen(pStr) + 1;
    cp = pStr;
    while(*cp++ >= 32) ;
    *--cp = 0;
    sscanf(pStr,"%12s", cType);
    
    if (strncmp(cType,"cnt",3) == 0)
    {
      while(*pStr++ > 32) ;
      if (sscanf(pStr,"%lf", &flBaseTime) < 1)
        flBaseTime = 0;

    }

    pStr += strlen(pStr) + 1;
    while (*pStr <= 32) pStr++;
    cp = pStr;
    while(*cp++ >= 32) ;
    *--cp = 0;
    sscanf(pStr,"0x%04X", &wTrig);

    //попытка вычленить второе поле
    if (strncmp(cType, "Trg", 3) == 0)
    {
      if (*pStr)
      {
        cp = pStr;
        if (sscanf(cTime,"%lf",&flBegTime) == 1 && sscanf(pStr,"%lf",&flBaseTime) == 1)
        {
          flBaseTime -= flBegTime;
          if (flBaseTime > 0)
            flBaseTime = -Round(flBaseTime,3);
          else
            flBaseTime = 0;
        }
        //while(*cp++ >= 32) ;
        pStr += strlen(pStr) + 1;
        //pStr = cp;
        while(*cp++ >= 32) ;
        *--cp = 0;
        sscanf(pStr,"0x%04X set%04X clr%04X",&wTrig, &wSet, &wClr);
      }
    }

    strncpy(pType,cType,12);
    strncpy(pTime,cTime,12);
    if (pTrWord)
      sprintf(pTrWord,"%04X %04X %04X",wTrig, wSet, wClr);
    return flBaseTime;
}

//050726
LRESULT CDataFile::MatExport(void)
{
char Path[MAX_PATH];
char cType[20], cTime[20], cTWord[20];

  if (IsFile() == 0 || m_nRecNum < 1)
    return E_FAIL;

  int24 *pBuf24(0);

  if (m_PntRead.pBuf == 0)
  {
    pBuf24 = new int24[m_nRecSize];
    m_PntRead.pBuf = pBuf24;
    m_PntRead.bPlus = m_bPlus;
  }

  CDizFileDialog dlg(FALSE, "*.mat",0,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY|OFN_EXPLORER ,"Matlab files (*.mat)\0*.mat");

  CString str = _Settings.GetMatlabFolder();
  //корневой каталог для данных
  if (!str.IsEmpty())
  {
    strncpy(Path,str,MAX_PATH-1);
    dlg.m_ofn.lpstrInitialDir = Path;
  }

  strncpy(dlg.m_ofn.lpstrFile,m_strFileTitle,dlg.m_ofn.nMaxFileTitle);

  if (dlg.DoModal() != IDOK)
    return E_FAIL;

  //dlg.GetFilePath(Path,MAX_PATH-5);
  _Settings.SetMatlabFolder(dlg.GetPath());
  CString strFile = dlg.m_ofn.lpstrFile;

  double *pDB(0);
  double *pTr(0);
  double *pAnn(0);
  double *pRecNames(0);
  double *pGTime(0);  //Global time (for all trials)
  double *pITime(0);  //Individual trial time
  double *pType(0);
  double *pLat(0);
  double *pTWord(0);
  double *pTSet(0);
  double *pTClr(0);



  char chRecName[10];
  const char *pCh;
  int i, nSz, j, chan;
  long Lng;
  DWORD dwOffs;
  bool bNoTrigTime(true);
  double flTime;
  double fl;

  HANDLE hFile(INVALID_HANDLE_VALUE);
  try
  {
    hFile = CreateFile(strFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
      throw("Error opening file");


    SAFE_DELETE_ARRAY(pDB);
    nSz = m_strFileTitle.GetLength();
    pDB = new double[nSz];
    pCh = m_strFileTitle;
    for (i = 0; i < nSz; i++)
    {
      pDB[i] = (double)(*pCh++);
    }
    MatWrite(hFile,"FileTitle",1,nSz,1,pDB);

    nSz = 80;
    SAFE_DELETE_ARRAY(pDB);
    pDB = new double[nSz];
    pCh = m_hdrMain.chLocSubj;
    for (i = 0; i < nSz; i++)
    {
      pDB[i] = (double)(*pCh++);
    }
    MatWrite(hFile,"LocalPatient",1,80,1,pDB);

    pCh = m_hdrMain.chLocRec;
    for (i = 0; i < nSz; i++)
    {
      pDB[i] = (double)(*pCh++);
    }
    MatWrite(hFile,"LocalRecording",1,80,1,pDB);

    *pDB = (double)m_nChanNum;
    MatWrite(hFile,"NumberOfChannels",0,1,1,pDB);
  
    *pDB = (double)m_nRecNum;
    MatWrite(hFile,"NumberOfRecords",0,1,1,pDB);

    //channel labels
    SAFE_DELETE_ARRAY(pDB);
    pDB = new double[m_nChanNum*8];


    for (i = 0; i < m_nChanNum; i++)
    {
      chan = m_arFChans[i].nNum;
      pCh = m_pChanNames + chan*8;
      for (j = 0; j < 8; j++)
      {
        pDB[i+m_nChanNum*j] = (double)(*pCh++);
      }

    }
    MatWrite(hFile,"ChanNames",1,8,m_nChanNum,pDB);


    pTr = new double[m_nRecNum*m_nTrLen];
    if (m_bPlus)
    {
      pAnn = new double[m_nRecNum*m_nAnnLen*3];
    }
    
    pRecNames = new double[m_nRecNum*8];

    SAFE_DELETE_ARRAY(pDB);
    pDB = new double[m_nChanNum*m_nChLen];

    if (m_nFrq > 0)
    {
      pGTime = new double[m_nChLen];
      for (i = 0; i < m_nChLen; i++)
      {
        fl = (double)i * 1./(double)m_nFrq;
        pGTime[i] = fl;
      }
    }

    if (pAnn)
    {
      pITime = new double[m_nRecNum*m_nChLen];
      pLat = new double[m_nRecNum];
      pType = new double[m_nRecNum*4];
      pTWord = new double[m_nRecNum*4];
      pTSet = new double[m_nRecNum*4];
      pTClr = new double[m_nRecNum*4];
    }

    for (int nRec = 0; nRec < m_nRecNum; nRec++)
    {
      ReadRecord(nRec, &m_PntRead);

      sprintf(chRecName,"Rec%05d", nRec+1);
      for (i = 0; i < 8; i++)
      {
        pRecNames[i*m_nRecNum+nRec] = (double)chRecName[i];
      }

      if (pAnn)
      {
        pCh = (char*)m_PntRead.pBuf;
        flTime = GetRecTypeAndTime(pCh, cType, cTime, cTWord);

        //определям, нужно ли формировать инд. и общие временные оси
        if (m_nFrq)
        {
          for (i = 0; i < m_nChLen; i++)
          {
            fl = (double)i * 1./(double)m_nFrq;
            pITime[i*m_nRecNum+nRec] = fl + flTime;
          }
        }

        //если это триггерная реализация, а мы еще не строили триг-временную ось - переносим...
        if (strncmp(cType, "Trg", 3) == 0 && bNoTrigTime)
        {
          bNoTrigTime = false;
          for (i = 0; i < m_nChLen; i++)
          {
            pGTime[i] = pITime[i*m_nRecNum+nRec];
          }

        }

        for (i = 0; i < 3; i++)
        {
          pType[i*m_nRecNum+nRec] = (double)cType[i];
        }

        for (i = 0; i < 4; i++)
        {
          pTWord[i*m_nRecNum+nRec] = (double)cTWord[i];
          pTSet[i*m_nRecNum+nRec] = (double)cTWord[i+5];
          pTClr[i*m_nRecNum+nRec] = (double)cTWord[i+10];
        }

        for (i = 0; i < m_nAnnLen*3; i++)
        {
          pAnn[i*m_nRecNum+nRec] = (double)(*pCh++);
        }

        if (sscanf(cTime,"%lf",&flTime) < 1)
          flTime = 0;
        pLat[nRec] = flTime;
      }

      if (pTr)
      {
        for (i = 0; i < m_nTrLen; i++)
        {
          Lng = (long)m_PntRead.pBuf[m_cwTrChan.dwOffs+i] & 0xFFFF;
          pTr[i+nRec*m_nTrLen] = (double)Lng;
        }
      }

      //самое главное - сами данные
      for (i = 0; i < m_nChanNum; i++)
      {
        long nOff = m_arFChans[i].nOff;
        //коэффициент делим на 32 - чтобы получить микровольты
        double flCf = m_arFChans[i].flCal / 32.;
        dwOffs = m_arFChans[i].dwOffs;
        for (j = 0; j < m_nChLen; j++)
        {
          Lng = (long)m_PntRead.pBuf[dwOffs+j] + nOff;
          pDB[i+j*m_nChanNum] = (double)Lng * flCf;
        }

      }
      MatWrite(hFile,chRecName,0,m_nChLen,m_nChanNum,pDB);
    }//end of record cycle

    

    if (pAnn)
    {
      MatWrite(hFile,"Annotation",1,m_nAnnLen*3,m_nRecNum,pAnn);
      MatWrite(hFile,"Latency",0,1,m_nRecNum,pLat);
      MatWrite(hFile,"Type",1,3,m_nRecNum,pType);
      MatWrite(hFile,"TWORD",1,4,m_nRecNum,pTWord);
      MatWrite(hFile,"TSET",1,4,m_nRecNum,pTSet);
      MatWrite(hFile,"TCLR",1,4,m_nRecNum,pTClr);
      MatWrite(hFile,"RecTime",0,m_nChLen,m_nRecNum,pITime);
    }

    if (pRecNames)
      MatWrite(hFile,"RecNames",1,8,m_nRecNum,pRecNames);

    if (pTr)
      MatWrite(hFile,"Trigger",0,m_nRecNum,m_nTrLen,pTr);

    if (pGTime)
      MatWrite(hFile,"times",0,1,m_nChLen,pGTime);

    *pDB = (double)m_nFrq;
    MatWrite(hFile,"srate",0,1,1,pDB);

    if (m_nTFrq != 0 && m_nTFrq != m_nFrq)
    {
      *pDB = (double)m_nTFrq;
      MatWrite(hFile,"TRIGsrate",0,1,1,pDB);
    }

    str.Format("data = zeros(%d, %d, %d); ", m_nChanNum, m_nChLen, m_nRecNum);
    char ComStr[] = "for k = 1:NumberOfRecords, data(:,:,k) = eval(RecNames(k,:)); clear(RecNames(k,:)); end; clear RecNames;";
    str += ComStr;

    nSz = str.GetLength();
    SAFE_DELETE_ARRAY(pDB);
    pDB = new double[nSz];
    pCh = str;
    for (i = 0; i < nSz; i++)
    {
      pDB[i] = (double)(*pCh++);
    }
    MatWrite(hFile,"CommandString",1,nSz,1,pDB);

    MessageBox(m_pMainFrm->m_hWnd,
      "Data written to file. \n\n      Type\n\n eval(CommandString)\n\n to build 3-d array (channesl*time*records)",
      "Data Written to file",MB_OK|MB_ICONINFORMATION);

  }
  catch(char *cp)
  {
    if (*cp)
    {
      MessageBox(m_pMainFrm->m_hWnd,cp,"ERROR",MB_OK|MB_ICONSTOP);
    }
  }

  SAFE_DELETE_ARRAY(pDB);
  SAFE_DELETE_ARRAY(pTr);
  SAFE_DELETE_ARRAY(pAnn);
  SAFE_DELETE_ARRAY(pRecNames);
  SAFE_DELETE_ARRAY(pGTime);
  SAFE_DELETE_ARRAY(pITime);
  SAFE_DELETE_ARRAY(pType);
  SAFE_DELETE_ARRAY(pLat);
  SAFE_DELETE_ARRAY(pTWord);
  SAFE_DELETE_ARRAY(pTSet);
  SAFE_DELETE_ARRAY(pTClr);

  if (hFile != INVALID_HANDLE_VALUE)
    CloseHandle(hFile);


  if (pBuf24 != 0)
  {
    SAFE_DELETE_ARRAY(pBuf24);
    m_PntRead.pBuf = 0;
  }
  return S_OK;
}

void CDataFile::MatWrite(HANDLE hFile, const char* cName, int nT, int nCol, int nRow, double* pBuf)
{
struct {
	int nText;
	int nRows;
	int nCols;
	int nComplex;
	int nNameLen;
} VarHdr;
DWORD dwWritten;

	size_t szHdr = sizeof(VarHdr);
	VarHdr.nCols = nCol;
	VarHdr.nRows = nRow;
	VarHdr.nText = nT;
	VarHdr.nComplex = 0;
	VarHdr.nNameLen = strlen(cName) + 1;

	WriteFile(hFile,&VarHdr,szHdr,&dwWritten,0);
	WriteFile(hFile,cName,VarHdr.nNameLen,&dwWritten,0);
	WriteFile(hFile,pBuf,sizeof(double) * nRow * nCol,&dwWritten,0);


}

//28 июля 2005 г. 14:45:07
bool CDataFile::ReadCf(int chan, CChanWrite *pCw)
{
    static long dgMax(0);
    static long dgMin(0);
    static long phMax(0);
    static long phMin(0);
    static double cf = 1;
    static double flUnit = 1.;
    static long off(0);
    const char *cp(0);

  try
  {

    if (m_pHdr == 0)
      throw("No Header");
    cp = m_pHdr->GetUnit(chan);
    if (cp == 0) throw("No unit");
    else if (strncmp(cp,"uV",2) == 0)
      flUnit = 1.;
    else if (strncmp(cp,"mV",2) == 0)
      flUnit = 1000.;
    else if (strncmp(cp,"V",1) == 0)
      flUnit = 1000000.;
    else if (strncmp(cp,"nV",1) == 0)
      flUnit = 0.001;
    else
      flUnit = 1.;



    
    if (sscanf(m_pHdr->GetDgMax(chan), "%ld", &dgMax) < 1) throw ("Inv DgMax\n");
    if (sscanf(m_pHdr->GetDgMin(chan), "%ld", &dgMin) < 1) throw ("Inv DgMin\n");;
    if (dgMax <= dgMin) throw("Dg Max <= Min\n");
    if (sscanf(m_pHdr->GetPhMax(chan), "%ld", &phMax) < 1) throw ("Inv PhMax\n");
    if (sscanf(m_pHdr->GetPhMin(chan), "%ld", &phMin) < 1) throw ("Inv PhMin\n");;
    if (phMax == phMin) throw("Ph Max == Min\n");

    //поправочные коэффициенты нужны, чтобы получить точные 0 и 1....
    cf = ((double)phMax - (double)phMin)/((double)dgMax - (double)dgMin);
    cf = cf * flUnit;
    off = (long)Round( (double)dgMin * cf, 0);
    off = phMin - off;
    
    pCw->flCal = cf * 32;
    off *= 32;
    pCw->nOff = off;
  }
  catch(char* cp)
  {
    ATLTRACE(cp);
    pCw->flCal = 1;
    pCw->nOff = 0;
    return false;
  }

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// class BDFheader
CBDFheader::CBDFheader(int nChans, char* pBuf)
{
char *pC(0);

  m_nChanNum = nChans;
  m_szSize = (nChans+1)*256;
  if (pBuf)
    m_pBuf = pBuf;
  else
  {
    m_pBuf = new char[m_szSize];
    ZeroMemory(m_pBuf,m_szSize);
  }

  ATLTRACE("m_pBuf %08X\n",m_pBuf);
  m_pMainHdr = new(m_pBuf) BDFh;

  pC = m_pBuf + sizeof(*m_pMainHdr);

  m_pcLabels = new(pC) char[nChans][16]; 
  pC += sizeof(*m_pcLabels)*nChans;

  m_pcTransd = new(pC) char[nChans][80];
  pC += sizeof(*m_pcTransd)*nChans;

  m_pcPhDim = new(pC) char[nChans][8];
  pC += sizeof(*m_pcPhDim)*nChans;

  m_pcPhMin = new(pC) char[nChans][8];
  pC += sizeof(*m_pcPhMin)*nChans;

  m_pcPhMax = new(pC) char[nChans][8];
  pC += sizeof(*m_pcPhMax)*nChans;

  m_pcDgMin = new(pC) char[nChans][8];
  pC += sizeof(*m_pcDgMin)*nChans;

  m_pcDgMax = new(pC) char[nChans][8];
  pC += sizeof(*m_pcDgMax)*nChans;

  m_pcPreFilt = new(pC) char[nChans][80];
  pC += sizeof(*m_pcPreFilt)*nChans;

  m_pcSamples = new(pC) char[nChans][8];
  pC += sizeof(*m_pcSamples)*nChans;

  m_pcReserved = new(pC) char[nChans][32];
  pC += sizeof(*m_pcReserved)*nChans;

  ATLASSERT( (pC-m_pBuf) == m_szSize);

}


CBDFheader::~CBDFheader()
{
  //delete m_pMainHdr;
  SAFE_DELETE_ARRAY(m_pBuf);
}

