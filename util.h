// Function declarations of utility functions used in the project
#ifndef _UTIL_H_
#define _UTIL_H_

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <exception>
#include <algorithm>
#include <cstdio>
#include <QString>

using namespace std;


unsigned int AsciiToDecimal (const unsigned char* buf);
string GetProgramVersion (const string& strPath);
string GetErrorMessage ();
string GetTimeStamp (const string& sDelim = "-", bool bFourDigitYear = true);
string GetWinFileTimeStamp (WIN32_FIND_DATA& wfd);
string GetMACAddress ();
string GetLocalIP ();
bool IsValidHandle (HANDLE);
void SplitString (const string& strToSplit, vector <string>& vList, char cDelimiter);
void SplitString (const wstring& strToSplit, vector <wstring>& vList, wchar_t cDelimiter);
void TrimString (string& str);
void TrimString (wstring& str);
string ConvertToHexString (const unsigned char* str, unsigned long nLength);
void SEHTransFunc (unsigned int, EXCEPTION_POINTERS*);
void SearchReplace (const string& sSearch, const string& sReplace, string& sData);
void PrintStack();
bool IsValidEmail(const string& sEmail);
bool IsValidEmail (const QString& sEmail);
bool IsValidPhone (const QString& sPhoneNum);
void ToUpperCase(string& s);

inline char DecimalDigitToAscii (unsigned char nDecDigit)
{
    char nAsciiDigit = -1;

    if (nDecDigit >= 0 && nDecDigit <= 9)
    {
        nAsciiDigit = nDecDigit + 48;
    }

    return nAsciiDigit;
}

// inline unsigned char AsciiDigitToDecimal (unsigned char nHexDigit)
// Convert an hex digit encoded in ASCII to a digit
inline unsigned char AsciiDigitToDecimal (unsigned char nHexDigit)
{
	unsigned char nDecDigit = 0;
		
	// nHexDigit - 48 gives the decimal representation
	// if it's between '0' and '9'
	if (nHexDigit >= '0' && nHexDigit <= '9')
	{
		nDecDigit = nHexDigit - 48;
	}
	// buf [i] - 54 gives the decimal representation
	// if it's between 'A' and 'F'
	else if (nHexDigit >= 'A' && nHexDigit <= 'F')
	{
		nDecDigit = nHexDigit - 55;
	}
	else
	{
		nDecDigit = 0;
	}

	return nDecDigit;
}

inline bool IsPrintableChar (char c)
{
	bool bRet = (c >= 32 && c <= 126);
	return bRet;
}

inline bool IsValidIP (const char* strIP)
{
	bool bRet = true;
	if (! strIP)
		bRet = false;
	else
	{
		if (inet_addr (strIP) == INADDR_NONE)
			bRet = false;
	}
	return bRet;
}

#endif
