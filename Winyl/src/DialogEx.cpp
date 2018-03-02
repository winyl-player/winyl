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
#include "DialogEx.h"
#include <cassert>

std::map<HWND, DialogEx*> DialogEx::mapDialogs;

DialogEx::DialogEx()
{

}

DialogEx::~DialogEx()
{
	if (thisWnd)
		::DestroyWindow(thisWnd);
}

INT_PTR DialogEx::ModalDialog(HWND parent, WORD dlgTemplate)
{
	assert(thisWnd == NULL);

	return ::DialogBoxParamW(::GetModuleHandleW(NULL), MAKEINTRESOURCEW(dlgTemplate), parent, MainDialogProc, (LPARAM)this);

	// Test (MFC way modal dialog)

	//http://blogs.msdn.com/b/oldnewthing/archive/2005/04/06/405827.aspx

	//::CreateDialogParam(::GetModuleHandle(NULL), MAKEINTRESOURCE(dlgTemplate), parent, MainDialogProc, (LPARAM)this);

	//if (thisWnd == NULL)
	//	return false;

	//if (thisParentWnd)
	//	EnableWindow(thisParentWnd, FALSE);

	//::ShowWindow(thisWnd, TRUE);

	//MSG msg;
	//msg.message = WM_NULL;
	//stopResult = 0;
	//while (!stopResult)
	//{
	//	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	//	{
	//		if (msg.message == WM_QUIT)
	//			break;
	//		else if (!IsDialogMessage(thisWnd, &msg))
	//		{
	//			TranslateMessage(&msg);
	//			DispatchMessage(&msg);
	//		}
	//	}
	//	else if (!stopResult)
	//		WaitMessage();
	//}
	//if (msg.message == WM_QUIT)
	//	PostQuitMessage((int)msg.wParam);


	//if (thisParentWnd)
	//	EnableWindow(thisParentWnd, TRUE);

	//::DestroyWindow(thisWnd);

	//return stopResult;
}

bool DialogEx::CreateModelessDialog(HWND parent, WORD dlgTemplate)
{
	assert(thisWnd == NULL);

	::CreateDialogParamW(::GetModuleHandleW(NULL), MAKEINTRESOURCEW(dlgTemplate), parent, MainDialogProc, (LPARAM)this);

	if (thisWnd == NULL)
		return false;

	::ShowWindow(thisWnd, SW_SHOW);

	return true;
}

bool DialogEx::IsDialogExMessage(MSG* msg)
{
	if (mapDialogs.empty())
		return false;

	// IsDialogMessage is only needed for modeless dialogs
	// but don't need to check for modeless dialog here
	// because the main message loop waits when modal dialog
	// so it will be working for modeless dialogs only anyway
	for (std::map<HWND, DialogEx*>::iterator it = mapDialogs.begin(), end = mapDialogs.end(); it != end; ++it)
	{
		if (::IsDialogMessage(it->second->thisWnd, msg))
			return true;
	}

	return false;
}

INT_PTR DialogEx::MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		DialogEx* dialogEx = (DialogEx*)lParam;

		mapDialogs[hDlg] = dialogEx;
		dialogEx->thisWnd = hDlg;
		dialogEx->thisParentWnd = ::GetParent(hDlg);
		dialogEx->CenterDialog(dialogEx->thisWnd, dialogEx->thisParentWnd);

		return dialogEx->DialogProc(hDlg, message, wParam, lParam);
	}

	std::map<HWND, DialogEx*>::iterator it = mapDialogs.find(hDlg);
	if (it != mapDialogs.end())
	{
		if (message == WM_DESTROY)
		{
			INT_PTR result = it->second->DialogProc(hDlg, message, wParam, lParam);
			it->second->thisWnd = NULL;
			it->second->thisParentWnd = NULL;
			mapDialogs.erase(it);
			return result;
		}
		else
			return it->second->DialogProc(hDlg, message, wParam, lParam);
	}

	return (INT_PTR)FALSE;
}

INT_PTR DialogEx::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return (INT_PTR)FALSE;
}

void DialogEx::CenterDialog(HWND hDlg, HWND hParentWnd)
{
	// Similar to: MFC -> wincore.cpp -> void CWnd::CenterWindow(CWnd* pAlternateOwner)

	if (hParentWnd == NULL)
		return;

	RECT rcDlg, rcWnd, rcArea;

	::GetWindowRect(hDlg, &rcDlg);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfoW(::MonitorFromWindow(hParentWnd, MONITOR_DEFAULTTONEAREST), &mi);
	rcArea = mi.rcWork;

	if (!::IsIconic(hParentWnd) && ::IsWindowVisible(hParentWnd))
		::GetWindowRect(hParentWnd, &rcWnd);
	else
		rcWnd = rcArea;

	int left = (rcWnd.left + rcWnd.right) / 2 - (rcDlg.right - rcDlg.left) / 2;
	int top = (rcWnd.top + rcWnd.bottom) / 2 - (rcDlg.bottom - rcDlg.top) / 2;

	if (left < rcArea.left)
		left = rcArea.left;
	else if (left + (rcDlg.right - rcDlg.left) > rcArea.right)
		left = rcArea.right - (rcDlg.right - rcDlg.left);

	if (top < rcArea.top)
		top = rcArea.top;
	else if (top + (rcDlg.bottom-rcDlg.top) > rcArea.bottom)
		top = rcArea.bottom - (rcDlg.bottom - rcDlg.top);

	::SetWindowPos(hDlg, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
