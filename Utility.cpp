#include "stdafx.h"

#include "utility.h"

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool Utility::FetchURL(LPCTSTR lpszServer, LPCTSTR lpszURL, LPBYTE* ppData)
{
	HINTERNET hInternet = InternetOpen(L"Whatever", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	wchar_t accept[512];
#ifdef _2003
	wcscpy(accept, L"*/*");
#else
	wcscpy_s(accept, 512, L"*/*");
#endif
	HINTERNET hHTTP = InternetConnect(hInternet, lpszServer, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
//	CString str;
//	str.Format("/mt?v=w2.4&x=%d&y=%d&zoom=%d", x, y, m_zoom);
	HINTERNET hRequest = HttpOpenRequest(hHTTP, L"GET", lpszURL, NULL, NULL, NULL, INTERNET_FLAG_NO_AUTH, 0);
	HttpSendRequest(hRequest, NULL, 0, 0, 0);

	// Get the length of the file.            
	DWORD statusCode = 0;
	{
		DWORD index = 0;
		char bufQuery[32] ;
		DWORD dwLengthBufQuery = sizeof(bufQuery);
		BOOL bQuery = ::HttpQueryInfo(hRequest,
									HTTP_QUERY_STATUS_CODE, 
									bufQuery, 
									&dwLengthBufQuery, &index);
		statusCode = atol(bufQuery);
	}
	if (statusCode == 404)
	{
	//	TRACE("Not found|!?\n");
		return false;
	}

	// Get the length of the file.            
	DWORD index = 0;
	char bufQuery[32];
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	BOOL bQuery = ::HttpQueryInfo(hRequest,
								HTTP_QUERY_CONTENT_LENGTH, 
								bufQuery, 
								&dwLengthBufQuery, &index);

	// Convert length from ASCII string to a DWORD.
	DWORD contentLength = (DWORD)atol(bufQuery);
//	TRACE("Content-length: %d\n", contentLength);
	*ppData = new BYTE[contentLength];

	DWORD dw = 0;
	DWORD total = 0;
	DWORD dw2 = 0;

	do
	{
		if (!InternetQueryDataAvailable(hRequest, &dw, 0, 0))
		{
		}
		else
		{
			unsigned char* data;
//			TRACE("Data available: %d\n", dw);
			data = new unsigned char[dw + 1];

			if (!InternetReadFile(hRequest, data, dw, &dw2))
			{
				delete [] data;
				break;
			}
			else
			{
//				TRACE("Read %d of %d\n", dw2, dw);
				data[dw2] = 0;
				memcpy(&ppData[total], data, dw2);
				delete [] data;
				total += dw2;
				if (total == contentLength)
				{
//					CloseHandle(hFile);
					break;
				}
			}
		}
	} while (TRUE);

	InternetCloseHandle(hInternet);

	return true;
}

bool FetchURL(LPCTSTR lpszServer, LPCTSTR lpszURL, LPCTSTR lpszFilename)
{
	HINTERNET hInternet = InternetOpen(L"Whatever", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	wchar_t accept[512];
#ifdef _2003
	wcscpy(accept, L"*/*");
#else
	wcscpy_s(accept, 512, L"*/*");
#endif
	HINTERNET hHTTP = InternetConnect(hInternet, lpszServer, INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
//	CString str;
//	str.Format("/mt?v=w2.4&x=%d&y=%d&zoom=%d", x, y, m_zoom);
	HINTERNET hRequest = HttpOpenRequest(hHTTP, L"GET", lpszURL, NULL, NULL, NULL, INTERNET_FLAG_NO_AUTH, 0);
	HttpSendRequest(hRequest, NULL, 0, 0, 0);

	// Get the length of the file.            
	DWORD statusCode = 0;
	{
		DWORD index = 0;
		char bufQuery[32] ;
		DWORD dwLengthBufQuery = sizeof(bufQuery);
		BOOL bQuery = ::HttpQueryInfo(hRequest,
									HTTP_QUERY_STATUS_CODE, 
									bufQuery, 
									&dwLengthBufQuery, &index);
		statusCode = atol(bufQuery);
	}
	if (statusCode == 404)
	{
	//	TRACE("Not found|!?\n");
		return false;
	}

	// Get the length of the file.            
	DWORD index = 0;
	char bufQuery[32];
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	BOOL bQuery = ::HttpQueryInfo(hRequest,
								HTTP_QUERY_CONTENT_LENGTH, 
								bufQuery, 
								&dwLengthBufQuery, &index);

	// Convert length from ASCII string to a DWORD.
	DWORD contentLength = (DWORD)atol(bufQuery);
//	TRACE("Content-length: %d\n", contentLength);
	HANDLE hFile = CreateFile(lpszFilename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

	DWORD dw = 0;
	DWORD total = 0;
	DWORD dw2 = 0;

	do
	{
		if (!InternetQueryDataAvailable(hRequest, &dw, 0, 0))
		{
		}
		else
		{
			unsigned char* data;
//			TRACE("Data available: %d\n", dw);
			data = new unsigned char[dw + 1];

			if (!InternetReadFile(hRequest, data, dw, &dw2))
			{
				delete [] data;
				break;
			}
			else
			{
//				TRACE("Read %d of %d\n", dw2, dw);
				data[dw2] = 0;
				WriteFile(hFile, data, dw2, &dw2, 0);
				delete [] data;
				total += dw2;
				if (total == contentLength)
				{
					CloseHandle(hFile);
					break;
				}
			}
		}
	} while (TRUE);

	InternetCloseHandle(hInternet);

	return true;
}


