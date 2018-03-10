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
#include "resource.h"
#include "DlgConfig.h"

DlgConfig::DlgConfig()
{

}

DlgConfig::~DlgConfig()
{

}

INT_PTR DlgConfig::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void DlgConfig::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::ConfigDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::ConfigDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::ConfigDialog, 2));

	pageGeneral.SetLanguage(lang);
	pageSystem.SetLanguage(lang);
	pagePopup.SetLanguage(lang);
	pageMini.SetLanguage(lang);

	pageGeneral.SetSettings(settings);
	pageSystem.SetSettings(settings);
	pagePopup.SetSettings(settings);
	pageMini.SetSettings(settings);

	tabConfig = ::GetDlgItem(thisWnd, IDC_TAB_CONFIG);

	TC_ITEM item = {};
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::ConfigDialog, 3));
	TabCtrl_InsertItem(tabConfig, 0, &item);
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::ConfigDialog, 4));
	TabCtrl_InsertItem(tabConfig, 1, &item);
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::ConfigDialog, 5));
	TabCtrl_InsertItem(tabConfig, 2, &item);
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::ConfigDialog, 6));
	TabCtrl_InsertItem(tabConfig, 3, &item);

	CRect rcTab;
	//::GetClientRect(tabConfig, rcTab);
	::GetWindowRect(tabConfig, rcTab);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcTab, 2);

	TabCtrl_AdjustRect(tabConfig, FALSE, rcTab);

	pageGeneral.CreateModelessDialog(thisWnd, IDD_DLGPAGEGENERAL);
	::MoveWindow(pageGeneral.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageGeneral.Wnd());
	::ShowWindow(pageGeneral.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageGeneral.Wnd(), ETDT_ENABLETAB);

	pageSystem.CreateModelessDialog(thisWnd, IDD_DLGPAGESYSTEM);
	::MoveWindow(pageSystem.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageSystem.Wnd());
	::ShowWindow(pageSystem.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageSystem.Wnd(), ETDT_ENABLETAB);

	pagePopup.CreateModelessDialog(thisWnd, IDD_DLGPAGEPOPUP);
	::MoveWindow(pagePopup.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pagePopup.Wnd());
	::ShowWindow(pagePopup.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pagePopup.Wnd(), ETDT_ENABLETAB);

	pageMini.CreateModelessDialog(thisWnd, IDD_DLGPAGEMINI);
	::MoveWindow(pageMini.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageMini.Wnd());
	::ShowWindow(pageMini.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageMini.Wnd(), ETDT_ENABLETAB);

	SelectPage(openPage);
	TabCtrl_SetCurSel(tabConfig, openPage);
}

void DlgConfig::OnBnClickedOK()
{
	pageGeneral.SaveSettings();
	pageSystem.SaveSettings();
	pagePopup.SaveSettings();
	pageMini.SaveSettings();
}

void DlgConfig::OnBnClickedCancel()
{
	pageGeneral.CancelSettings();
	pageSystem.CancelSettings();
	pagePopup.CancelSettings();
	pageMini.CancelSettings();
}

void DlgConfig::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
	if (nmhdr->hwndFrom == tabConfig && nmhdr->code == TCN_SELCHANGE)
	{
		::ShowWindow(pageGeneral.Wnd(), SW_HIDE);
		::ShowWindow(pageSystem.Wnd(), SW_HIDE);
		::ShowWindow(pagePopup.Wnd(), SW_HIDE);
		::ShowWindow(pageMini.Wnd(), SW_HIDE);

		::SetFocus(tabConfig);

		SelectPage(TabCtrl_GetCurSel(tabConfig));
	}
}

void DlgConfig::SelectPage(int page)
{
	if (page == 0)
	{
	//	::BringWindowToTop(pageGeneral.Wnd());
		::ShowWindow(pageGeneral.Wnd(), SW_SHOW);
	}
	else if (page == 1)
	{
	//	::BringWindowToTop(pageSystem.Wnd());
		::ShowWindow(pageSystem.Wnd(), SW_SHOW);
	}
	else if (page == 2)
	{
	//	::BringWindowToTop(pagePopup.Wnd());
		::ShowWindow(pagePopup.Wnd(), SW_SHOW);
	}
	else if (page == 3)
	{
	//	::BringWindowToTop(pageMini.Wnd());
		::ShowWindow(pageMini.Wnd(), SW_SHOW);
	}
}

