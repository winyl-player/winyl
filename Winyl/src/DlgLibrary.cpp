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

// DlgLibrary.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgLibrary.h"


// DlgLibrary dialog

DlgLibrary::DlgLibrary()
{

}

DlgLibrary::~DlgLibrary()
{

}

INT_PTR DlgLibrary::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		// Without WS_CLIPCHILDREN controls are blinking a bit when change a tab
		LONG_PTR style;
		style = ::GetWindowLongPtr(thisWnd, GWL_STYLE);
		::SetWindowLongPtr(thisWnd, GWL_STYLE, style | WS_CLIPCHILDREN);

		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnBnClickedOK();
			EndDialog(LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			EndDialog(LOWORD(wParam));
			return TRUE;
		}
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgLibrary::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::LibraryDialog, 0));

	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::LibraryDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::LibraryDialog, 2));

	pageLibrary.SetLanguage(lang);
	pageLibraryOpt.SetLanguage(lang);

	tabLibrary = ::GetDlgItem(thisWnd, IDC_TAB_LIBRARY);

	TC_ITEM item = {};
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::LibraryDialog, 3));
	TabCtrl_InsertItem(tabLibrary, 0, &item);
	if (!isFirstRun)
	{
		item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::LibraryDialog, 4));
		TabCtrl_InsertItem(tabLibrary, 1, &item);
	}

	CRect rcTab;
	::GetWindowRect(tabLibrary, rcTab);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcTab, 2);

	TabCtrl_AdjustRect(tabLibrary, FALSE, rcTab);

	pageLibrary.CreateModelessDialog(thisWnd, IDD_DLGPAGELIBRARY);
	::MoveWindow(pageLibrary.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageLibrary.Wnd());
	::ShowWindow(pageLibrary.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageLibrary.Wnd(), ETDT_ENABLETAB);

	pageLibraryOpt.CreateModelessDialog(thisWnd, IDD_DLGPAGELIBRARYOPT);
	::MoveWindow(pageLibraryOpt.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageLibraryOpt.Wnd());
	::ShowWindow(pageLibraryOpt.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageLibraryOpt.Wnd(), ETDT_ENABLETAB);

	SelectPage(0);
	TabCtrl_SetCurSel(tabLibrary, 0);
}

void DlgLibrary::OnBnClickedOK()
{
	//if (HIWORD(GetAsyncKeyState(VK_CONTROL)))
	//	isUpdateAll = true;

	pageLibrary.Save();
	pageLibraryOpt.Save();
}

void DlgLibrary::OnBnClickedCancel()
{
	pageLibraryOpt.Save();
}

void DlgLibrary::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
	if (nmhdr->hwndFrom == tabLibrary && nmhdr->code == TCN_SELCHANGE)
	{
		::ShowWindow(pageLibrary.Wnd(), SW_HIDE);
		::ShowWindow(pageLibraryOpt.Wnd(), SW_HIDE);

		::SetFocus(tabLibrary);

		SelectPage(TabCtrl_GetCurSel(tabLibrary));
	}
}

void DlgLibrary::SelectPage(int page)
{
	if (page == 0)
	{
		::ShowWindow(pageLibrary.Wnd(), SW_SHOW);
	}
	else if (page == 1)
	{
		::ShowWindow(pageLibraryOpt.Wnd(), SW_SHOW);
	}
}
