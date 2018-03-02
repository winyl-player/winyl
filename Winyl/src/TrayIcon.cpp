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

// TrayIcon.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TrayIcon.h"

// TrayIcon

TrayIcon::TrayIcon()
{

}

TrayIcon::~TrayIcon()
{

}

void TrayIcon::NewIcon(HWND wnd, HICON icon)
{
	NOTIFYICONDATA ni = {};
	ni.cbSize = sizeof(NOTIFYICONDATA);
	ni.uID = 737; // Random
	ni.hWnd = wnd;
	ni.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	ni.hIcon = icon;
	ni.uCallbackMessage = UWM_TRAYMSG;
	ni.szTip[0] = '\0';

	Shell_NotifyIcon(NIM_ADD, &ni);
}

void TrayIcon::SetString(HWND wnd, const std::wstring& str)
{
	NOTIFYICONDATA ni = {};
	ni.cbSize = sizeof(NOTIFYICONDATA);
	ni.uID = 737; // Random
	ni.hWnd = wnd;
	ni.uFlags = NIF_TIP;

	std::wstring tip = str;

	std::size_t tipSize = sizeof(ni.szTip) / sizeof(wchar_t) - 1; // -1 because null terminator
	if (tip.size() > tipSize)
		tip.resize(tipSize);

	memcpy(ni.szTip, tip.c_str(), tip.size() * sizeof(wchar_t) + sizeof(wchar_t));

	::Shell_NotifyIcon(NIM_MODIFY, &ni);
}

void TrayIcon::SetNullString(HWND wnd)
{
	NOTIFYICONDATA ni = {};
	ni.cbSize = sizeof(NOTIFYICONDATA);
	ni.uID = 737; // Random
	ni.hWnd = wnd;
	ni.uFlags = NIF_TIP;

	ni.szTip[0] = '\0';

	::Shell_NotifyIcon(NIM_MODIFY, &ni);
}

void TrayIcon::DeleteIcon(HWND wnd)
{
	NOTIFYICONDATA ni = {};
	ni.cbSize = sizeof(NOTIFYICONDATA);
	ni.uID = 737; // Random
	ni.hWnd = wnd;

	::Shell_NotifyIcon(NIM_DELETE, &ni);
}

void TrayIcon::Minimize(HWND wnd, bool needFast)
{
	if (!needFast)
	{
		::ShowWindow(wnd, SW_MINIMIZE);
	}
	else
	{
		::SetFocus(wnd);

		WINDOWPLACEMENT wp = {};
		wp.length = sizeof(WINDOWPLACEMENT);

		::GetWindowPlacement(wnd, &wp);
		wp.showCmd = SW_MINIMIZE;
		::SetWindowPlacement(wnd, &wp);
	}
}

void TrayIcon::Restore(HWND wnd, bool needFast, bool needMaximize)
{
	if (!needFast)
	{
		::ShowWindow(wnd, SW_RESTORE);
	}
	else
	{
		WINDOWPLACEMENT wp = {};
		wp.length = sizeof(WINDOWPLACEMENT);

		::GetWindowPlacement(wnd, &wp);
		// needMaximize - this is a fix, the window restore incorrectly without it
		if (!needMaximize)
			wp.showCmd = SW_RESTORE;
		else
			wp.showCmd = SW_SHOWMAXIMIZED;
		::SetWindowPlacement(wnd, &wp);
	}
}

void TrayIcon::Maximize(HWND wnd, bool needFast)
{
	if (!needFast)
	{
		::ShowWindow(wnd, SW_MAXIMIZE);
	}
	else
	{
		WINDOWPLACEMENT wp = {};
		wp.length = sizeof(WINDOWPLACEMENT);

		::GetWindowPlacement(wnd, &wp);
		wp.showCmd = SW_MAXIMIZE;
		::SetWindowPlacement(wnd, &wp);
	}
}

void TrayIcon::SizeMinimized(HWND wnd)
{
	if (isHideToTray || isMiniPlayer)
		::ShowWindow(wnd, SW_HIDE);
}
