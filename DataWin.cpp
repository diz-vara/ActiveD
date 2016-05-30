#include "StdAfx.h"
#include ".\datawin.h"
#include <math.h>

CDataWin::CDataWin(int nWinLen, const char* WinType) : m_pWin(0)
{
const double PI(3.1415926535897932384626433832795);
double a[4];
int nCf;
double dbTmp;

	if (nWinLen < 0 || nWinLen > 65535) 
		throw("Invalid Window length");
	m_nWinLen = nWinLen;
	m_pWin = new double[nWinLen];

	if(strcmp(WinType,"Nuttall") == 0)
	{
		nCf = 4;
		a[0] = 0.3635819;
		a[1] = -0.4891775;
		a[2] = 0.1365995;
		a[3] = -0.0106411;
	}
	else if(strcmp(WinType,"BlackmanHarris") == 0)
	{
		nCf = 4;
		a[0] = 0.35875;
		a[1] = -0.48829;
		a[2] = 0.14128;
		a[3] = -0.01168;
	}
	else if(strcmp(WinType,"Blackman") == 0)
	{
		nCf = 3;
		a[0] = 0.42;
		a[1] = -0.5;
		a[2] = 0.08;
	}
	else if(1 || strcmp(WinType,"Hamming") == 0)
	{
		//default window - Hamming
		nCf = 2;
		a[0] = 0.54;
		a[1] = -0.46;
	}


	for (int i = 0; i < m_nWinLen; i++)
	{
		dbTmp = 0;
		//общий множитель 2*PI*k/(n-1)
		double db2 = (double)i * PI * 2./ (double)(m_nWinLen-1);

		for (int cf = 0; cf < nCf; cf++)
		{
			//cf даст 0 для 0 - и cos станет 1, то есть я получу в чистом виде A0
			dbTmp += a[cf]*cos(cf*db2);
		}
		m_pWin[i] = dbTmp;
//		ATLTRACE("%d\t%.17f\n",i,dbTmp);
	} //end of WinLen cycle

}
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
CDataWin::~CDataWin(void)
{
	SAFE_DELETE_ARRAY(m_pWin);
}
