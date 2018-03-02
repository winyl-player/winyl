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

#pragma once

#include "UTF.h"

class HttpClient
{
public:
	HttpClient(void);
	virtual ~HttpClient(void);

	// Global set proxy for this class
	static void SetProxy(int proxy, const std::wstring& host, const std::wstring& port, const std::wstring& login, const std::wstring& pass);
	static bool GetHttpPage(const std::wstring& url, std::string& outPage);
	static bool GetHttpPage(const std::string& url, std::string& outPage);

protected:
	static int proxyType;
	static std::wstring proxyHost;
	static std::wstring proxyPort;
	static std::wstring proxyLogin;
	static std::wstring proxyPass;
};

