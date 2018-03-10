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
#include "Messengers.h"

Messengers::Messengers()
{

}

Messengers::~Messengers()
{

}

void Messengers::MSNPlay(const std::wstring& artist, const std::wstring& title)
{
	std::wstring send;
	
/*	if (csArtist.IsEmpty())
		csSend = L"\\0Music\\01\\0{0}\\0" + csTitle + L"\\0WMContentID\\0";
	else if (!csTitle.IsEmpty())
		csSend = L"\\0Music\\01\\0{0} - {1}\\0" + csTitle + L"\\0" + csArtist  + L"\\0WMContentID\\0";
	else
		csSend = L"\\0Music\\01\\0\\0WMContentID\\0"; // Stop*/

	if (artist.empty())
		send = L"\\0Music\\01\\0{0}\\0" + title + L"\\0\\0";
	else if (!title.empty())
		send = L"\\0Music\\01\\0{0} - {1}\\0" + title + L"\\0" + artist  + L"\\0\\0";
	else
		send = L"\\0Music\\00\\0\\0"; // Stop

	COPYDATASTRUCT cd;
    cd.dwData = 0x547; // MSN_MAGIC_NUMBER;
	cd.cbData = ((int)send.size() + 1) * sizeof(wchar_t);
	cd.lpData = (PVOID)send.c_str();

    HWND wnd = NULL;
	while (wnd = ::FindWindowEx(NULL, wnd, L"MsnMsgrUIManager", NULL))
    {
		::SendMessage(wnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}

void Messengers::MSNStop()
{
	//CString csSend = L"\\0Music\\01\\0\\0WMContentID\\0";
	std::wstring send = L"\\0Music\\00\\0\\0";

	COPYDATASTRUCT cd;
    cd.dwData = 0x547; // MSN_MAGIC_NUMBER;
	cd.cbData = ((int)send.size() + 1) * sizeof(wchar_t);
	cd.lpData = (PVOID)send.c_str();

    HWND wnd = NULL;
	while (wnd = ::FindWindowEx(NULL, wnd, L"MsnMsgrUIManager", NULL))
    {
		::SendMessage(wnd, WM_COPYDATA, 0, (LPARAM)&cd);
    }
}



