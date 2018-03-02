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

#include "StdAfx.h"
#include "MessageBox.h"
#include <cassert>

MessageBox::MessageBox()
{

}

MessageBox::~MessageBox()
{

}

bool MessageBox::Show(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon, Button buttons)
{
	if (futureWin->IsVistaOrLater())
		return ShowVista(parent, title, instruction, icon, buttons);
	else
		return ShowXP(parent, title, instruction, icon, buttons);
}

bool MessageBox::ShowXP(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon, Button buttons)
{
	UINT uType = 0;

	if (icon == Icon::Error)
		uType |= MB_ICONERROR;
	else if (icon == Icon::Warning)
		uType |= MB_ICONWARNING;
	else if (icon == Icon::Information)
		uType |= MB_ICONINFORMATION;
	else if (icon == Icon::Question)
		uType |= MB_ICONQUESTION;

	if (buttons == Button::OK)
		uType |= MB_OK;
	else if (buttons == Button::OKCancel)
		uType |= MB_OKCANCEL;
	else if (buttons == Button::YesNo)
		uType |= MB_YESNO;

	assert(hhkXP == NULL);
	hhkXP = ::SetWindowsHookExW(WH_CBT, &CBTProc, 0, ::GetCurrentThreadId());

	int result = ::MessageBoxW(parent, instruction, title, uType);

	if (result == IDOK || result == IDYES)
		return true;

	return false;
}

bool MessageBox::ShowVista(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon, Button buttons)
{
	LPWSTR pszIcon = TD_INFORMATION_ICON;
	DWORD dwButtons = TDCBF_OK_BUTTON;

	if (icon == Icon::Error)
		pszIcon = TD_ERROR_ICON;
	else if (icon == Icon::Warning)
		pszIcon = TD_WARNING_ICON;
	else if (icon == Icon::Information)
		pszIcon = TD_INFORMATION_ICON;
	else if (icon == Icon::Question)
		pszIcon = IDI_QUESTION;

	if (buttons == Button::OK)
		dwButtons = TDCBF_OK_BUTTON;
	else if (buttons == Button::OKCancel)
		dwButtons = TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON;
	else if (buttons == Button::YesNo)
		dwButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON;

	int result = 0;
	TASKDIALOGCONFIG config = {};
	config.cbSize = sizeof(TASKDIALOGCONFIG);
	config.hwndParent = parent;
	config.hInstance = NULL;
	config.dwFlags = TDF_POSITION_RELATIVE_TO_WINDOW | TDF_ALLOW_DIALOG_CANCELLATION; // | 0x10000000; // 0x10000000 = TDF_SIZE_TO_CONTENT
	config.cxWidth = 0;
	config.pszWindowTitle = title;
	config.pszMainInstruction = instruction;
	config.pszContent = NULL;
	config.pszMainIcon = pszIcon;
	config.dwCommonButtons = dwButtons;

	if (futureWin->TaskDialogIndirect(&config, &result, NULL, NULL) != S_OK)
		return ShowXP(parent, title, instruction, icon, buttons);

	if (result == IDOK || result == IDYES)
		return true;

	return false;
}


HHOOK MessageBox::hhkXP = NULL;

LRESULT MessageBox::CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_ACTIVATE)
	{
		// Similar to: MFC -> wincore.cpp -> void CWnd::CenterWindow(CWnd* pAlternateOwner)

		HWND hDlg = (HWND)wParam;
		HWND hParentWnd = ::GetParent(hDlg);
		if (hParentWnd && !::IsIconic(hParentWnd) && ::IsWindowVisible(hParentWnd))
		{
			RECT rcDlg, rcWnd, rcArea;

			::GetWindowRect(hDlg, &rcDlg);
			::GetWindowRect(hParentWnd, &rcWnd);

			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			::GetMonitorInfoW(::MonitorFromWindow(hParentWnd, MONITOR_DEFAULTTONEAREST), &mi);
			rcArea = mi.rcWork;

			int left = (rcWnd.left + rcWnd.right) / 2 - (rcDlg.right - rcDlg.left) / 2;
			int top = (rcWnd.top + rcWnd.bottom) / 2 - (rcDlg.bottom - rcDlg.top) / 2;

			if (left < rcArea.left)
				left = rcArea.left;
			else if (left + (rcDlg.right - rcDlg.left) > rcArea.right)
				left = rcArea.right - (rcDlg.right - rcDlg.left);

			if (top < rcArea.top)
				top = rcArea.top;
			else if (top + (rcDlg.bottom - rcDlg.top) > rcArea.bottom)
				top = rcArea.bottom - (rcDlg.bottom - rcDlg.top);

			::SetWindowPos(hDlg, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}

		::UnhookWindowsHookEx(hhkXP);
		hhkXP = NULL;
	}
	else
		::CallNextHookEx(hhkXP, nCode, wParam, lParam);

	return 0;
}
