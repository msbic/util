#ifndef _UDP_LOG_CLIENT_
#define _UDP_LOG_CLIENT_

/*#include "CriticalSectionLocker.h"*/
#include <cstdarg>
#include <exception>
#include <deque>

string LocalTime()
{
	SYSTEMTIME systime;		
	GetLocalTime(&systime);

	auto thID = ::GetCurrentThreadId();
	char buf[128] = "";
	sprintf_s(buf, "%02u:%02u:%02u.%03u: Thread: 0x%04x", systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds, thID);
	return buf;
}	

class UDPLogClient
{
public:
	UDPLogClient(unsigned int nPort, const string& strHost) :
	  m_sHost(strHost),
	  m_nPort(nPort),
	  m_sock(INVALID_SOCKET),
	  m_bInitialized(false),
		m_hLogThread(nullptr),
		m_bThreadRunning(true)
	{		
	}

	~UDPLogClient()
	{		
		if (m_hLogThread)
		{
			{
				EnterCriticalSection(&m_queLock);
				m_bThreadRunning = false;
				SetEvent(m_hLogEvent);
				LeaveCriticalSection(&m_queLock);
			}
			
			DWORD dwWait = WaitForSingleObject(m_hLogThread, 50);
			if (WAIT_OBJECT_0 == dwWait)
			{
				CloseHandle(m_hLogThread);
			}
			LogLine("*********** " + LocalTime() + " Log ended *****************");
		}
		if (m_hLogEvent)
		{
			CloseHandle(m_hLogEvent);
		}

		DeleteCriticalSection(&m_queLock);
		closesocket(m_sock);
	}

	bool Init()
	{
		bool bRet = true;
		InitializeCriticalSection(&m_queLock);
		m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_sock == INVALID_SOCKET) 
		{
			bRet = false;
		}

		if(bRet)
		{
			m_addr.sin_family = AF_INET;
			m_addr.sin_port = htons(m_nPort);
			m_addr.sin_addr.s_addr = inet_addr(m_sHost.c_str());
			if (m_addr.sin_addr.s_addr == INADDR_NONE)
			{
				bRet = false;
			}
		}

		if (bRet)
		{
			DWORD nThreadID = 0;
			m_bInitialized = true;
			/*m_hLogThread = CreateThread(nullptr, 0, LogThreadRoutine, reinterpret_cast<void*>(this), 0, &nThreadID);
			m_hLogEvent = CreateEvent(nullptr, TRUE, FALSE, L"");
			if (m_hLogThread != nullptr && m_hLogEvent != nullptr)
			{				
				m_bInitialized = true;
			}
			else
			{
				bRet = false;
			}*/
		}

		return bRet;
	}

	bool IsInitialized() const
	{
		return m_bInitialized;
	}	

	void Log(const vector<string>& logLines)
	{		
		DoLog(logLines);
		/*EnterCriticalSection(&m_queLock);
		m_logQueue.push_back(logLines);
		SetEvent(m_hLogEvent);
		LeaveCriticalSection(&m_queLock);*/
	}

private:

	/*__declspec(noinline) static DWORD WINAPI LogThreadRoutine(void* pArg)
	{
		UDPLogClient* pInstance = reinterpret_cast<UDPLogClient*>(pArg);
		if (pInstance)
		{
			pInstance->LogLine("*********** " + LocalTime() + " Log started *****************");
			vector<string> logLines;
			bool bRunning = true;
			while (bRunning)
			{
				WaitForSingleObject(pInstance->m_hLogEvent, INFINITE);
				{
					EnterCriticalSection(&(pInstance->m_queLock));
					bRunning = pInstance->m_bThreadRunning;
					if (! pInstance->m_logQueue.empty())
					{
						logLines = pInstance->m_logQueue.front();
						pInstance->m_logQueue.pop_front();
					}
					ResetEvent(pInstance->m_hLogEvent);
					LeaveCriticalSection(&(pInstance->m_queLock));
				}

				if (! logLines.empty())
				{
					pInstance->DoLog(logLines);
					logLines.clear();
					logLines.shrink_to_fit();
				}
			}		
			pInstance->LogLine("*********** " + LocalTime() + " logging thread exited *****************");
		}
		else
		{
			string s("Object instance is null: ");
			s += __FUNCTION__;
			throw std::exception(s.data());
		}

		return 0;
	}*/
	
	__declspec(noinline) void DoLog(const vector<string>& logData)
	{
		ASSERT(m_bInitialized);

		if (! m_bInitialized)
		{
			string s("UDPLogClient not initialized: ");
			s += __FUNCTION__;
			throw std::exception(s.data());
		}

		/*CriticalSectionLocker lock(&m_socketLock);*/
		int nRet = 0;
		
		for (auto i = logData.cbegin(); i != logData.cend() && nRet != SOCKET_ERROR; ++i)
		{
			nRet = sendto(m_sock, i->c_str(), i->length(), 0, (SOCKADDR *)&m_addr, sizeof(m_addr));
		
			if (nRet == SOCKET_ERROR)
			{
				nRet = WSAGetLastError();
			}
		}
	}

	template <class T>
	__declspec(noinline) void LogLine(T&& str)
	{
		ASSERT(m_bInitialized);

		if (! m_bInitialized)
		{
			string s("UDPLogClient not initialized: ");
			s += __FUNCTION__;
			throw std::exception(s.data());
		}

		/*CriticalSectionLocker lock(&m_socketLock);*/
		
		int nRet = sendto(m_sock, str.c_str(), str.length(), 0, (SOCKADDR *)&m_addr, sizeof(m_addr));
		if (nRet == SOCKET_ERROR)
		{
			nRet = WSAGetLastError();
		}
	}
private:
	UDPLogClient();
	UDPLogClient(const UDPLogClient&);
	UDPLogClient& operator=(const UDPLogClient&);
private:
	string m_sHost;
	unsigned int m_nPort;
	CRITICAL_SECTION m_queLock;
	SOCKET m_sock;
	bool m_bInitialized;
	sockaddr_in m_addr;
	HANDLE m_hLogThread;
	HANDLE m_hLogEvent;
	bool m_bThreadRunning;
	deque < vector<string> > m_logQueue;
};

//UDPLogClient g_logger(10000, "127.0.0.1");

#define LOG_0(cacheObj, format) cacheObj.Log(format ## " %s:%d", __FUNCTION__, __LINE__)
#define LOG_1(cacheObj, format, data) cacheObj.Log(format ## " %s:%d", data, __FUNCTION__, __LINE__)
#define LOG_2(cacheObj, format, data1, data2) cacheObj.Log(format ## " %s:%d", data1, data2, __FUNCTION__, __LINE__)
#define LOG_3(cacheObj, format, data1, data2, data3) cacheObj.Log(format ## " %s:%d", data1, data2, data3, __FUNCTION__, __LINE__)
#define LOG_4(cacheObj, format, data1, data2, data3, data4) cacheObj.Log(format ## " %s:%d", data1, data2, data3, data4, __FUNCTION__, __LINE__)
#define LOG_5(cacheObj, format, data1, data2, data3, data4, data5) cacheObj.Log(format ## " %s:%d", data1, data2, data3, data4, data5, __FUNCTION__, __LINE__)

#define LOG_M0(format) m_logCache.Log(format ## " %s:%d", __FUNCTION__, __LINE__)
#define LOG_M1(format, data) m_logCache.Log(format ## " %s:%d", data, __FUNCTION__, __LINE__)
#define LOG_M2(format, data1, data2) m_logCache.Log(format ## " %s:%d", data1, data2, __FUNCTION__, __LINE__)
#define LOG_M3(format, data1, data2, data3) m_logCache.Log(format ## " %s:%d", data1, data2, data3, __FUNCTION__, __LINE__)
#define LOG_M4(format, data1, data2, data3, data4) m_logCache.Log(format ## " %s:%d", data1, data2, data3, data4, __FUNCTION__, __LINE__)
#define LOG_M5(format, data1, data2, data3, data4, data5) m_logCache.Log(format ## " %s:%d", data1, data2, data3, data4, data5, __FUNCTION__, __LINE__)

class LogCache
{
public:
	LogCache(bool bLogEnabled , int nCacheSize = 16):
		m_nMaxCacheSize(nCacheSize),
		m_pLogger(new UDPLogClient(10000, "127.0.0.1")),
		m_bLogEnabled(bLogEnabled)
	  {		  
			if (! m_pLogger->Init())
			{
				throw std::exception("Could not initialize logger object");
			}
		  m_cache.reserve(nCacheSize);
		  InitializeCriticalSection(&m_cacheLock);
	  }

	~LogCache()
	{
		Flush();
		DeleteCriticalSection(&m_cacheLock);
	}

	void Flush(const vector<string>& cache)
	{		
		ASSERT(m_pLogger);
		m_pLogger->Log(m_cache);
		m_cache.clear();
	}

	void Flush()
	{
		Flush(m_cache);
	}

	 template<class T>
	 void Log(T&& sLog)
	 {
		 if (m_bLogEnabled)
		 {
			 EnterCriticalSection(&m_cacheLock);
			 /*auto logString (LocalTime() + ": " + sLog);*/
			 m_cache.emplace_back(std::forward<T>(LocalTime() + ": " + sLog));
			 if (m_cache.size() >= m_nMaxCacheSize)
			 {
				 Flush();				
			 }
			 LeaveCriticalSection(&m_cacheLock);
		 }
	 }

	__declspec(noinline) void LogCache::Log(const char* format, ... )
	{
		if (m_bLogEnabled && format)
		{
	   		va_list args;
	   		int len;
	   		char * buffer = nullptr;

			va_start (args, format);
	   		len = _vscprintf(format, args) // _vscprintf doesn't count
										+ 1; // terminating '\0'
    		vector<char> v(len);
	   		buffer = &v[0];
			vsprintf_s(buffer, len, format, args);
	   		Log(string(&v[0]));
		}
	} 	
private:
	LogCache();
	LogCache(const LogCache&);
	LogCache& operator=(const LogCache&);
private:
	vector<string> m_cache;
	size_t m_nMaxCacheSize;
	unique_ptr<UDPLogClient> m_pLogger;
	bool m_bLogEnabled;
	CRITICAL_SECTION m_cacheLock;
};

#endif