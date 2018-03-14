/*  This file is part of Winyl Player source code.
    Copyright (C) 2008-2018, Alex Kras. <winylplayer@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "HttpClient.h"
#include "winhttp.h"
#include "UTF.h"

HttpClient::HttpClient(void)
{

}

HttpClient::~HttpClient(void)
{

}

int HttpClient::proxyType = 0;
std::wstring HttpClient::proxyHost;
std::wstring HttpClient::proxyPort;
std::wstring HttpClient::proxyLogin;
std::wstring HttpClient::proxyPass;

void HttpClient::SetProxy(int proxy, const std::wstring& host, const std::wstring& port, const std::wstring& login, const std::wstring& pass)
{
	proxyType = proxy;
	proxyHost = host;
	proxyPort = port;
	proxyLogin = login;
	proxyPass = pass;
}

bool HttpClient::GetHttpPage(const std::string& url, std::string& outPage)
{
	return GetHttpPage(UTF::UTF16S(url), outPage);
}

bool HttpClient::GetHttpPage(const std::wstring& url, std::string& outPage)
{
	std::wstring server;
	std::wstring path;
	bool https = false;

	std::size_t find = url.find(':');
	if (find != std::string::npos && url[find + 1] == '/' && url[find + 2] == '/')
	{
		if (find == 5) https = true;

		std::size_t start = find + 3;
		find = url.find('/', start);
		if (find != std::string::npos)
		{
			server = url.substr(start, find - start);
			path = url.substr(find);
		}
		else
			server = url.substr(start);
	}

	if (server.empty())
		return false;

	//const std::wstring userAgent = L"Mozilla/5.0 (Windows NT 6.1; rv:28.0) Gecko/20100101 Firefox/28.0";
	const std::wstring userAgent = L"Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko";

	HINTERNET sessionHandle = NULL;
	HINTERNET connectHandle = NULL;
	HINTERNET requestHandle = NULL;

	if (proxyType == 0)
		sessionHandle = WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_NO_PROXY, 0, WINHTTP_NO_PROXY_BYPASS, 0);
	else
		sessionHandle = WinHttpOpen(userAgent.c_str(), WINHTTP_ACCESS_TYPE_NAMED_PROXY, (proxyHost + L':' + proxyPort).c_str(), WINHTTP_NO_PROXY_BYPASS, 0);

	if (sessionHandle)
	{
		if (!WinHttpSetTimeouts(sessionHandle, 5000, 5000, 5000, 5000))
		{
			WinHttpCloseHandle(sessionHandle);
			return false;
		}

		// By default only SSL3 and TLS1 are enabled in Windows 7 so enable TLS1.1 and TLS1.2 too
		if (https)
		{
			DWORD flags = WINHTTP_FLAG_SECURE_PROTOCOL_SSL3|WINHTTP_FLAG_SECURE_PROTOCOL_TLS1|WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_1|WINHTTP_FLAG_SECURE_PROTOCOL_TLS1_2;
			WinHttpSetOption(sessionHandle, WINHTTP_OPTION_SECURE_PROTOCOLS, &flags, sizeof(flags));
		}

		connectHandle = WinHttpConnect(sessionHandle, server.c_str(), https ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);
	}

	if (connectHandle)
	{
		requestHandle = WinHttpOpenRequest(connectHandle, L"GET", path.c_str(),
			NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, https ? WINHTTP_FLAG_SECURE : 0);
	}

	bool result = false;

	if (requestHandle)
	{
		if (proxyType == 2)
			WinHttpSetCredentials(requestHandle, WINHTTP_AUTH_TARGET_PROXY, WINHTTP_AUTH_SCHEME_BASIC, proxyLogin.c_str(), proxyPass.c_str(), NULL);

		// Test
		//if (https)
		//{
		//	DWORD flags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID|SECURITY_FLAG_IGNORE_CERT_DATE_INVALID|SECURITY_FLAG_IGNORE_UNKNOWN_CA|SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
		//	WinHttpSetOption(requestHandle, WINHTTP_OPTION_SECURITY_FLAGS, &flags, sizeof(flags));	
		//	////void CALLBACK callback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
		//	//WinHttpSetStatusCallback(requestHandle, callback, WINHTTP_CALLBACK_FLAG_SECURE_FAILURE, NULL);
		//}

		if (WinHttpSendRequest(requestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		{
			if (WinHttpReceiveResponse(requestHandle, NULL))
			{
				DWORD statusCode = 0;
				DWORD statusCodeSize = sizeof(statusCode);

				WinHttpQueryHeaders(requestHandle, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
					WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeSize, WINHTTP_NO_HEADER_INDEX);

				if (statusCode == 200)
				{
					int limiter = 0;

					while (true)
					{
						DWORD size = 0;
						if (!WinHttpQueryDataAvailable(requestHandle, &size))
							break;

						if (size == 0)
						{
							result = true;
							break;
						}

						limiter += size;
						if (limiter > 500 * 1024)
						{
							assert(false);
							break;
						}

						char *buffer = new char[size + 1];
						//buffer[0] = '\0';
						ZeroMemory(buffer, size + 1);

						DWORD read = 0;
						if (!WinHttpReadData(requestHandle, (LPVOID)buffer, size, &read))
						{
							delete[] buffer;
							break;
						}

						// This condition should never be reached since WinHttpQueryDataAvailable
						// reported that there are bits to read.
						if (read == 0)
						{
							delete[] buffer;
							result = true;
							break;
						}

						buffer[read] = '\0';
						outPage += buffer;

						delete[] buffer;
					}
				}
			}
		}
	}

	if (requestHandle) WinHttpCloseHandle(requestHandle);
	if (connectHandle) WinHttpCloseHandle(connectHandle);
	if (sessionHandle) WinHttpCloseHandle(sessionHandle);

	if (!result)
		outPage.clear();

	return result;
}
