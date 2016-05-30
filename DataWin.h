#pragma once

class CDataWin
{
private:
	int m_nWinLen;
	double *m_pWin;
public:
	CDataWin(int WinLen = 33, const char* WinType = "Hamming");
	virtual ~CDataWin(void);
	double operator[](int idx) const
	{
		if (idx < 0 || idx >= m_nWinLen)
			return 0;
		else
			return m_pWin[idx];
	};
};
