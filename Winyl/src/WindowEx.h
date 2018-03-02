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

#include <map>
#include <cassert>

class WindowEx
{
public:
	WindowEx();
	virtual ~WindowEx();
	WindowEx(const WindowEx&) = delete;
	WindowEx& operator=(const WindowEx&) = delete;

	bool CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle,
		int x, int y, int cx, int cy, const wchar_t* title, HICON icon, HICON iconSm, bool isDoubleClicks = false);
	bool CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle,
		RECT* rc, const wchar_t* title, HICON icon, HICON iconSm, bool isDoubleClicks = false);
	bool CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle, bool isDoubleClicks = false);

	inline HWND Wnd() {assert(thisWnd); return thisWnd;}
	inline bool IsWnd() {return (thisWnd ? true : false);}
	inline HWND ParentWnd() {assert(thisParentWnd); return thisParentWnd;}
	inline bool IsParentWnd() {return (thisParentWnd ? true : false);}

	static void BeginWaitCursor() {::SetCursor(::LoadCursorW(NULL, IDC_WAIT));}
	static void EndWaitCursor() {::SetCursor(::LoadCursorW(NULL, IDC_ARROW));}

protected:
	HWND thisWnd = NULL;
	HWND thisParentWnd = NULL;

private:
	// http://www.codeguru.com/forum/showthread.php?t=287823&highlight=WndProc
	// http://blogs.msdn.com/b/oldnewthing/archive/2005/04/22/410773.aspx
	static std::map<HWND, WindowEx*> mapWindows;
	
	static LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
