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

#include <string>

class TrayIcon
{

public:
	TrayIcon();
	virtual ~TrayIcon();

	void NewIcon(HWND wnd, HICON icon);
	void DeleteIcon(HWND wnd);
	void SetString(HWND wnd, const std::wstring& str);
	void SetNullString(HWND wnd);

	void Minimize(HWND wnd, bool needFast);
	void Restore(HWND wnd, bool needFast, bool needMaximize = false);
	void Maximize(HWND wnd, bool needFast);
	void SizeMinimized(HWND wnd);

	inline void SetHideToTray(bool isHide) {isHideToTray = isHide;}
	inline void SetMiniPlayer(bool isMini) {isMiniPlayer = isMini;}

private:
	bool isHideToTray = false;
	bool isMiniPlayer = false;
};


