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

// DlgSmart.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgSmart.h"


// DlgSmart dialog

DlgSmart::DlgSmart()
{

}

DlgSmart::~DlgSmart()
{
	if (toolImageList)
		::ImageList_Destroy(toolImageList);
}

INT_PTR DlgSmart::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	PreTranslateMessage(message);

	switch (message)
	{
	case WM_INITDIALOG:
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
		OnCommand(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgSmart::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::DialogSmart, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::DialogSmart, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::DialogSmart, 2));

	::SetDlgItemText(thisWnd, IDC_STATIC_NAME, lang->GetLine(Lang::DialogSmart, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_FROM, lang->GetLine(Lang::DialogSmart, 4));
	::SetDlgItemText(thisWnd, IDC_STATIC_OPTIONS, lang->GetLine(Lang::DialogSmart, 11));

	::SetDlgItemText(thisWnd, IDC_STATIC_SELECT, lang->GetLine(Lang::DialogSmart, 12));
	::SetDlgItemText(thisWnd, IDC_STATIC_COUNT, lang->GetLine(Lang::DialogSmart, 13));

	::SetDlgItemText(thisWnd, IDC_CHECK_RANDOM, lang->GetLine(Lang::DialogSmart, 14));
	::SetDlgItemText(thisWnd, IDC_CHECK_UPDATE, lang->GetLine(Lang::DialogSmart, 15));

	comboFrom = ::GetDlgItem(thisWnd, IDC_COMBO_FROM);
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 5));
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 6));
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 7));
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 8));
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 9));
	ComboBox_AddString(comboFrom, lang->GetLine(Lang::DialogSmart, 10));
	ComboBox_SetCurSel(comboFrom, smart.from);

	comboType = ::GetDlgItem(thisWnd, IDC_COMBO_TYPE);
	ComboBox_AddString(comboType, lang->GetLine(Lang::DialogSmart, 21));
	ComboBox_AddString(comboType, lang->GetLine(Lang::DialogSmart, 22));
	ComboBox_SetCurSel(comboType, smart.type);

	comboFilter = ::GetDlgItem(thisWnd, IDC_COMBO_FILTER);
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 23));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 24));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 25));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 26));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 27));
	if (smart.type == 0)
	{
		ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 28));
		ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 29));
	}
	ComboBox_SetCurSel(comboFilter, smart.order);

	::CheckDlgButton(thisWnd, IDC_CHECK_RANDOM, smart.isRandom);
	::CheckDlgButton(thisWnd, IDC_CHECK_UPDATE, smart.isAutoUpdate);

	::SetDlgItemInt(thisWnd, IDC_EDIT_COUNT, (UINT)smart.count, FALSE);

	::SetDlgItemText(thisWnd, IDC_EDIT_NAME, smartName.c_str());

	////////////////////////////////////////////////////////////////////////////////

	HINSTANCE instance = ::GetModuleHandle(NULL);

	std::wstring tipString = lang->GetLineS(Lang::DialogSmart, 17);
	tipString.push_back('\n');
	tipString += lang->GetLineS(Lang::DialogSmart, 18);
	tipString.push_back('\n');
	tipString += lang->GetLineS(Lang::DialogSmart, 19);

	CRect rc;
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_EDIT_FROM), rc);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rc, 2);

	toolTip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, L"", WS_POPUP|TTS_BALLOON|TTS_ALWAYSTIP|TTS_NOPREFIX, //|TTS_CLOSE
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, thisWnd, NULL, instance, NULL);

	TOOLINFO ti = {};
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_ABSOLUTE|TTF_CENTERTIP; /*|TTF_TRACK|TTF_TRANSPARENT*/
    ti.hwnd = thisWnd;
    ti.lpszText = (LPWSTR)tipString.c_str();
    ti.uId = 0;
	ti.rect = rc;

	::SendMessage(toolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	::SendMessage(toolTip, TTM_SETMAXTIPWIDTH, 0, 1000);
	::SendMessage(toolTip, TTM_SETTITLE, TTI_INFO, (LPARAM)lang->GetLine(Lang::DialogSmart, 16));
	::SendMessage(toolTip, TTM_ACTIVATE, TRUE, 0);

	CRect rcInfo;
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_STATIC_TOOLTIP), rcInfo);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcInfo, 2);
	rcInfo.left += (rcInfo.Width() - 22) / 2; // For High DPI
	rcInfo.top += 1;

	CRect rcPlus;
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_STATIC_PLUS), rcPlus);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcPlus, 2);
	rcPlus.left += (rcPlus.Width() - 22) / 2; // For High DPI
	rcPlus.top += 1;

	toolImageList = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);
	HICON iconInfo = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_INFO), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON iconPlus = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_PLUS), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	HICON iconTick = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_TICK), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	::ImageList_AddIcon(toolImageList, iconInfo);
	::ImageList_AddIcon(toolImageList, iconPlus);
	::ImageList_AddIcon(toolImageList, iconTick);
	DestroyIcon(iconInfo);
	DestroyIcon(iconPlus);
	DestroyIcon(iconTick);

	toolBarInfo = ::CreateWindowEx(0, TOOLBARCLASSNAME, L"", CCS_NODIVIDER|CCS_NORESIZE|TBSTYLE_LIST|TBSTYLE_FLAT|
		WS_CHILD|WS_VISIBLE|WS_DISABLED, rcInfo.left, rcInfo.top, rcInfo.Width(), rcInfo.Height(), thisWnd, NULL, instance, NULL);

	toolBarPlus = ::CreateWindowEx(0, TOOLBARCLASSNAME, L"", CCS_NODIVIDER|CCS_NORESIZE|TBSTYLE_LIST|TBSTYLE_FLAT|
		WS_CHILD|WS_VISIBLE, rcPlus.left, rcPlus.top, rcPlus.Width(), rcPlus.Height(), thisWnd, NULL, instance, NULL);

	::SendMessage(toolBarInfo, TB_SETIMAGELIST, 0, (LPARAM)toolImageList);
	::SendMessage(toolBarPlus, TB_SETIMAGELIST, 0, (LPARAM)toolImageList);

	::SendMessage(toolBarInfo, TB_SETBUTTONSIZE, 0, MAKELPARAM(16, rcInfo.Height() - 1));
	::SendMessage(toolBarPlus, TB_SETBUTTONSIZE, 0, MAKELPARAM(16, rcPlus.Height() - 1));

	// Tooltips for toolbar
	toolTipInfo = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, L"", WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, toolBarInfo, NULL, instance, NULL);

	toolTipPlus = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, L"", WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, toolBarPlus, NULL, instance, NULL);

	TOOLINFO ti2 = {};
	ti2.cbSize = sizeof(TOOLINFO);

	ti2.hwnd = toolBarInfo;
	ti2.uId = 10000;
	ti2.lpszText = (LPWSTR)lang->GetLine(Lang::DialogSmart, 16);
	::SendMessage(toolTipInfo, TTM_ADDTOOL, 0, (LPARAM)&ti2);
	::SendMessage(toolTipInfo, TTM_ACTIVATE, TRUE, 0);

	ti2.hwnd = toolBarPlus;
	ti2.uId = 10001;
	ti2.lpszText = (LPWSTR)lang->GetLine(Lang::DialogSmart, 20);
	::SendMessage(toolTipPlus, TTM_ADDTOOL, 0, (LPARAM)&ti2);
	::SendMessage(toolTipPlus, TTM_ACTIVATE, TRUE, 0);

	::SendMessage(toolBarInfo, TB_SETTOOLTIPS, (WPARAM)toolTipInfo, 0);
	::SendMessage(toolBarPlus, TB_SETTOOLTIPS, (WPARAM)toolTipPlus, 0);

	// Buttons for toolbar
	TBBUTTON button = {};
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	
	button.idCommand = 10000;
	button.iBitmap = 0;
	button.iString = 0;
	::SendMessage(toolBarInfo, TB_INSERTBUTTON, 0, (LPARAM)&button);

	button.idCommand = 10001;
	button.iBitmap = 1;
	::SendMessage(toolBarPlus, TB_INSERTBUTTON, 0, (LPARAM)&button);


	////////////////////////////////////////////////////////////////////////////////
	if (smart.from)
	{
		OnCbnSelchangeComboFrom();
		::SetDlgItemText(thisWnd, IDC_EDIT_FROM, smart.fromString.c_str());
	}

	if (smart.IsFilter())
		UpdateToolPlus();
}

void DlgSmart::OnCbnSelchangeComboFrom()
{
	if (ComboBox_GetCurSel(comboFrom) > 0)
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_FROM), TRUE);
		::EnableWindow(toolBarInfo, TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_FROM), FALSE);
		::EnableWindow(toolBarInfo, FALSE);
	}
}

void DlgSmart::OnCbnSelendokComboType()
{
	ComboBox_ResetContent(comboFilter);

	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 23));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 24));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 25));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 26));
	ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 27));
	if (ComboBox_GetCurSel(comboType) == 0)
	{
		ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 28));
		ComboBox_AddString(comboFilter, lang->GetLine(Lang::DialogSmart, 29));
	}
	ComboBox_SetCurSel(comboFilter, 0);


	if (ComboBox_GetCurSel(comboType) == 0)
		::SetDlgItemInt(thisWnd, IDC_EDIT_COUNT, 50, FALSE);
	else
		::SetDlgItemInt(thisWnd, IDC_EDIT_COUNT, 5, FALSE);

	smart.type = ComboBox_GetCurSel(comboType);
	UpdateToolPlus();
}

void DlgSmart::PreTranslateMessage(UINT message)
{
	if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN)
		//message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
	{
		if (::IsWindowVisible(toolTip))
		{
			::ReleaseCapture();
			TOOLINFO ti = {};
			ti.cbSize = sizeof(TOOLINFO);
			ti.hwnd = thisWnd;
			ti.uId = 0;
			::SendMessage(toolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		}
	}
}


void DlgSmart::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (lParam == (LPARAM)comboFrom)
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
			OnCbnSelchangeComboFrom();

	}
	else if (lParam == (LPARAM)comboType)
	{
		if (HIWORD(wParam) == CBN_SELENDOK)
			OnCbnSelendokComboType();
	}
	if (LOWORD(wParam) == 10000)
	{
		if (!::IsWindowVisible(toolTip))
		{
			TOOLINFO ti;
			ti.cbSize = sizeof(TOOLINFO);
			ti.hwnd = thisWnd;
			ti.uId = 0;
			//CRect rc;
			//::GetWindowRect(::GetDlgItem(thisWnd, IDC_EDIT_FROM), rc);
			//::SendMessage(toolTip, TTM_TRACKPOSITION, 0, MAKELPARAM(rc.left+rc.Width()/2, rc.bottom));
			::SendMessage(toolTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
			if (::IsWindowVisible(toolTip))
				::SetCapture(thisWnd);
		}
	}
	else if (LOWORD(wParam) == 10001)
	{
		smart.type = ComboBox_GetCurSel(comboType);
		if (smart.type == 0)
		{
			DlgSmartTracks dlg;
			dlg.SetLanguage(lang);
			dlg.SetSmartlist(&smart);
			if (dlg.ModalDialog(thisWnd, IDD_DLGSMARTTRACKS) == IDOK)
				UpdateToolPlus();
		}
		else if (smart.type == 1)
		{
			DlgSmartAlbums dlg;
			dlg.SetLanguage(lang);
			dlg.SetSmartlist(&smart);
			if (dlg.ModalDialog(thisWnd, IDD_DLGSMARTALBUMS) == IDOK)
				UpdateToolPlus();
		}
	}
}

void DlgSmart::UpdateToolPlus()
{
	TBBUTTONINFO info = {};
	info.cbSize = sizeof(TBBUTTONINFO);
	info.dwMask = TBIF_BYINDEX|TBIF_IMAGE;
	if (!smart.IsFilter())
		info.iImage = 1;
	else
		info.iImage = 2;
	::SendMessage(toolBarPlus, TB_SETBUTTONINFO, 0, (LPARAM)&info);
}

void DlgSmart::OnBnClickedOK()
{
	smartName = HelperGetDlgItemText(thisWnd, IDC_EDIT_NAME);

	smart.fromString = HelperGetDlgItemText(thisWnd, IDC_EDIT_FROM);
	smart.from = ComboBox_GetCurSel(comboFrom);
	smart.type = ComboBox_GetCurSel(comboType);
	smart.order = ComboBox_GetCurSel(comboFilter);
	smart.isRandom = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_RANDOM);
	smart.isAutoUpdate = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_UPDATE);
	
	smart.count = (int)::GetDlgItemInt(thisWnd, IDC_EDIT_COUNT, NULL, FALSE);
}

void DlgSmart::OnBnClickedCancel()
{

}
