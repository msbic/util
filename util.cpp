#include "util.h"
#include <Iptypes.h>
#include <Iphlpapi.h>
#include <regex>
#include <algorithm>
#include <QRegExp>
#include <cctype>
#include <strstream>
#include <QDebug>

struct upper
{
    int operator()(int c)
    {
        return toupper(c);
    }
};

void ToUpperCase(string& s)
{
    transform(s.begin(), s.end(), s.begin(), upper());
}

bool IsValidEmail (const QString& sEmail)
{
    bool bRet = false;
    QRegExp emailRegex("[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}");

    bRet = emailRegex.exactMatch(sEmail);
    return bRet;
}


bool IsValidPhone (const QString& sPhoneNum)
{
    bool bRet = false;

    QRegExp phoneRegex ("((((\\(\\d{3}\\))|(\\d{3}-))\\d{3}-\\d{4})|(\\+?\\d{2}((-| )\\d{1,8}){1,5}))(( x| ext)\\d{1,5}){0,1}");

    bRet = phoneRegex.exactMatch(sPhoneNum);

    return bRet;
}

// Match string against reg. expression
bool IsValidEmail (const string& sEmail)
{
    bool bRet = true;
    regex sEmailRegex ("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}$");


    bRet = regex_match (sEmail, sEmailRegex);

    return bRet;
}

template <class S> void TrimString (S& str);

void TrimString (string &str)
{
    TrimString<string> (str);
}

void TrimString (wstring &str)
{
    TrimString<wstring> (str);
}

// Replaces all occurences of sSearch with sReplace in string sData
void SearchReplace (const string& sSearch, const string& sReplace, string& sData)
{
    size_t nPos = sData.find(sSearch);
    while (nPos != string::npos)
    {
        sData.replace(nPos, sSearch.size(), sReplace);
        nPos = sData.find(sSearch, nPos);
    }
}

// Converts 4 bytes of ASCII encoded hex into
// a decimal number
unsigned int AsciiToDecimal (const unsigned char* buf)
{	
	unsigned int nRes = 0;
	for (int i = 3; i >= 0; --i)
	{
		unsigned char nDigit = AsciiDigitToDecimal (buf [i]);
				
		// (1 << 4 * (3 - i) ==> raise 16 to the power of 3 - i
		nRes |= nDigit * (1 << 4 * (3 - i));
	}
	return nRes;
}

// void SEHTransFunc (unsigned int nCode, EXCEPTION_POINTERS* pExp)
// This is a funtion that does the translation between Windows structured
// exception handling (SEH) to normal C++ exceptions
// use _set_se_translator to set this function to handle SEH
void SEHTransFunc (unsigned int nCode, EXCEPTION_POINTERS* pExp)
{
	char strMsg [64] = "";
    sprintf (strMsg, "Windows error code: %u", nCode);
	throw exception (strMsg);
}


// string ConvertToHexString (const unsigned char* str, unsigned long nLength)
// A function that converts const char* str to it's hex. representation 
string ConvertToHexString (const unsigned char* str, unsigned long nLength)
{	
	string sRet;

    stringstream oss;
    for (size_t i = 0; i < nLength; i++)
    {
        oss << "0x" << hex << uppercase << setw (2) << setfill ('0') << (int) str [i] << " ";
    }
    sRet = oss.str ();

	return sRet;
}

// Removes leading and trailing spaces 
// from the string str
template <class S> void TrimString (S& str)
{
    if (str.length () > 0)
    {
        while (isspace (str.at (0)))
        {
            str.erase (0, 1);
        }

        while (isspace (str.at (str.length () - 1)))
        {
            str.erase (str.length () - 1, 1);
        }
    }
}

// A function that splits the string into substrings separated by the cDelimiter
// and appends them to vList
template <class S, class C> void SplitString (const S& strToSplit, vector <S>& vList, C cDelimiter)
{
	// Check if there is at least one instance of the delimiter
    if (!strToSplit.empty() && strToSplit.find (cDelimiter) != string::npos)
    {
        // Empty the list just in case
        vList.clear ();
        S token ("");
        basic_stringstream <C> iss (strToSplit);
        // Add all substrings to the list
        while (getline (iss, token, cDelimiter))
        {
            TrimString (token);
            vList.push_back (token);
        }
    }
}

// bool IsValidHandle (HANDLE handle)
// A function that checks whether a handle is valid
// by comparing it to NULL and INVALID_HANDLE_VALUE
bool IsValidHandle (HANDLE handle)
{
	return ((handle != NULL) && (handle != INVALID_HANDLE_VALUE));
}

// string GetProgramVersion (const string& strPath)
// A function that returns a version into strVersion of a program pointed to by strPath
string GetProgramVersion (const string& strPath)
{
	string strVersion ("");

	unsigned long nDummy = 0;
    unsigned long nFVISize = GetFileVersionInfoSizeA (strPath.c_str (), &nDummy);
    vector <char> sVersion(nFVISize);
	
    if (nFVISize > 0)
    {
        if (GetFileVersionInfoA (strPath.c_str (), 0, nFVISize, &sVersion[0]))
        {
            VS_FIXEDFILEINFO* pFfi;
            unsigned int nLen = 0;
            if (VerQueryValueA (&sVersion[0] , "\\" , reinterpret_cast <void **>(&pFfi), &nLen))
            {
                unsigned long nFileVersionMS = 0, nFileVersionLS = 0;
                nFileVersionMS = pFfi->dwFileVersionMS;
                nFileVersionLS = pFfi->dwFileVersionLS;
                unsigned long nLeft = 0, nSecLeft = 0, nSecRight = 0, nRight = 0;
                nLeft = HIWORD (nFileVersionMS);
                nSecLeft = LOWORD (nFileVersionMS);
                nSecRight = HIWORD (nFileVersionLS);
                nRight = LOWORD (nFileVersionLS);
                stringstream stream;
                stream << nLeft << "." << nSecLeft << "." << nSecRight << "." << nRight << endl;
                strVersion = stream.str ();
            }

        } // if (GetFileVersionInfo (strPath, 0, nFVISize, strVersion))
    }

	return strVersion;
}

// string GetErrorMessage ()
// A helper function that retrieves the system error
// based on error code returned by GetLastError ()
string GetErrorMessage ()
{
	string strErrMsg ("");
	void* lpMsgBuf;
	// Get system error
    if (FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER |
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						GetLastError(),
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                        (LPSTR) &lpMsgBuf,
						0,
						NULL))
	{
		strErrMsg = reinterpret_cast <char *> (lpMsgBuf);
		LocalFree (lpMsgBuf);
	}
	return strErrMsg;		
}


// string GetWinFileTimeStamp (WIN32_FIND_DATA& wfd)
// A function that gets the Last Write Date 
// from the wfd parameter and converts it
// into a string with format:
// dd-mm-yyyy_hh-mm-ss
string GetWinFileTimeStamp (WIN32_FIND_DATA& wfd)
{
	ostringstream os;
	FILETIME ftLocal;
	SYSTEMTIME st;

	// Convert time store in wfd to local time and 
	// then to system time
	FileTimeToLocalFileTime (&wfd.ftLastWriteTime, &ftLocal);
	FileTimeToSystemTime (&ftLocal, &st);
	// Format date
	os << setw (2) << setfill ('0') << st.wDay << "-" <<
		setw (2) << setfill ('0') << st.wMonth << "-" <<
		st.wYear << "_" <<
		setw (2) << setfill ('0') << st.wHour << "-" <<
		setw (2) << setfill ('0') << st.wMinute << "-" <<
		setw (2) << setfill ('0') << st.wSecond;

	return os.str ();
}

// string GetTimeStamp (const string& sDelim, bool bFourDigitYear)
// A function that gets current system time
// and converts it into a string with format:
// yy(yy)mmddhhmmss
string GetTimeStamp (const string& sDelim, bool bFourDigitYear)
{
	ostringstream os;
	time_t t = time (NULL);
    struct tm* tme = localtime (&t);

    if (tme)
    {
        int nYear = tme->tm_year;
        if (bFourDigitYear)
        {
           nYear += 1900;
        }
        else
        {
            nYear -= 100; // Only valid after 2000
        }

        // Format date
        os << nYear << sDelim <<
            setw (2) << setfill ('0') << tme->tm_mon + 1 << sDelim <<
            setw (2) << setfill ('0') << tme->tm_mday << sDelim <<

            setw (2) << setfill ('0') << tme->tm_hour << sDelim <<
            setw (2) << setfill ('0') << tme->tm_min << sDelim <<
            setw (2) << setfill ('0') << tme->tm_sec;
    }

	return os.str ();
}


// string GetMACAddress ()
// A function that returns computers first MAC address
// in string format
string GetMACAddress () 
{
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information
    ostringstream os;                      // for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer

    if (dwStatus == ERROR_SUCCESS)
    {
        // Format a string in HEX
        int size = sizeof (AdapterInfo[0].Address);
        for (int i = 0; i < size; i++)
        {
            os << hex << uppercase << (int) AdapterInfo[0].Address [i];
        }
    }

	return os.str ();
}

// A function that returns local IP address
string GetLocalIP ()
{
    string sLocalIP;

	// Init WinSock
	WSADATA wsaData;
	int nRet = WSAStartup(0x101, &wsaData);
    if (nRet == 0)
    {
        // Get the local hostname
        char strHostName[255] = "";
        nRet = gethostname(strHostName, 255);
        if (nRet == 0)
        {
            struct hostent *hostEntry = NULL;
            hostEntry = gethostbyname(strHostName);

            if (hostEntry != NULL)
            {
                char* strLocalIP = NULL;
                strLocalIP = inet_ntoa (*(struct in_addr *) *hostEntry->h_addr_list);

                if (strLocalIP != NULL)
                {
                    sLocalIP = strLocalIP;
                }
            }
        }
    }

    WSACleanup();
    return sLocalIP;
}

#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

void PrintStack( void )
{
    BOOL                result;
    HANDLE              process;
    HANDLE              thread;
    CONTEXT             context;
    STACKFRAME64        stack;
    ULONG               frame;
    IMAGEHLP_SYMBOL64*   symbol = (IMAGEHLP_SYMBOL64*)malloc(sizeof( IMAGEHLP_SYMBOL64 ) + 256);
    DWORD64             displacement;
    char name[ 256 ] = "";


    RtlCaptureContext( &context );
    memset( &stack, 0, sizeof( STACKFRAME64 ) );

    process                = GetCurrentProcess();
    thread                 = GetCurrentThread();
    displacement           = 0;
    stack.AddrPC.Offset    = context.Eip;
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Offset = context.Esp;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Offset = context.Ebp;
    stack.AddrFrame.Mode   = AddrModeFlat;

    for( frame = 0; ; frame++ )
    {
        result = StackWalk64
        (
            IMAGE_FILE_MACHINE_I386,
            process,
            thread,
            &stack,
            &context,
            NULL,
            SymFunctionTableAccess64,
            SymGetModuleBase64,
            NULL
        );

        symbol->SizeOfStruct  = sizeof( IMAGEHLP_SYMBOL64 ) + 256;
        symbol->MaxNameLength = 255;

        int nRes = SymInitialize(process, NULL, TRUE);
        if(SymGetSymFromAddr64( process, ( ULONG64 )stack.AddrPC.Offset, &displacement, symbol ))
        {
            strcpy(name, symbol->Name);
            // UnDecorateSymbolName( symbol->Name, ( PSTR )name, 256, UNDNAME_COMPLETE );

            fprintf
            (
                        stderr,
                "Frame %lu:\n"
                "    Symbol name:    %s\n"
                "    PC address:     0x%08LX\n"
                "    Stack address:  0x%08LX\n"
                "    Frame address:  0x%08LX\n"
                "\n",
                frame,
                symbol->Name,
                ( ULONG64 )stack.AddrPC.Offset,
                ( ULONG64 )stack.AddrStack.Offset,
                ( ULONG64 )stack.AddrFrame.Offset
            );
        }
        else
        {
            DWORD dwRet = GetLastError();
            dwRet = dwRet;
        }

        if( !result )
        {
            break;
        }
    }
    free(symbol);
}

