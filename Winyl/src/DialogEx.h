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

class DialogEx
{
public:
	DialogEx();
	virtual ~DialogEx();
	DialogEx(const DialogEx&) = delete;
	DialogEx& operator=(const DialogEx&) = delete;

	INT_PTR ModalDialog(HWND parent, WORD dlgTemplate);
	bool CreateModelessDialog(HWND parent, WORD dlgTemplate);

	inline HWND Wnd() {assert(thisWnd); return thisWnd;}

	void CenterDialog(HWND hDlg, HWND hParentWnd);

	// Add 'if (DialogEx::IsDialogMsg(&msg)) continue;' to beginning of the main message loop (in WinylApp.cpp)
	// See IsDialogMessage WinAPI function to know why this is needed for modeless dialogs
	static bool IsDialogExMessage(MSG* msg);

protected:
	HWND thisWnd = NULL;
	HWND thisParentWnd = NULL;

	//INT_PTR stopResult = 0;
	void EndDialog(INT_PTR result)
	{
		//stopResult = result;
		::EndDialog(thisWnd, result);
	}

private:
	// http://www.codeguru.com/forum/showthread.php?t=287823&highlight=WndProc
	// http://blogs.msdn.com/b/oldnewthing/archive/2005/04/22/410773.aspx
	static std::map<HWND, DialogEx*> mapDialogs;
	
	static INT_PTR CALLBACK MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	virtual INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};
