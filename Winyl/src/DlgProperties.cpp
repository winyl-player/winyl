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
#include "DlgProperties.h"

DlgProperties::DlgProperties()
{

}

DlgProperties::~DlgProperties()
{
	if (toolImageList)
		::ImageList_Destroy(toolImageList);
}

INT_PTR DlgProperties::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDC_BUTTON_SAVE:
			OnBnClickedButtonSave();
			return TRUE;
		}
		OnCommand(wParam, lParam);
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		return 0;
	}

	return FALSE;
}

void DlgProperties::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::PropertiesDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::PropertiesDialog, 1));
	::SetDlgItemText(thisWnd, IDC_BUTTON_SAVE, lang->GetLine(Lang::PropertiesDialog, 2));

	properties.reset(new Properties());
	properties->SetSkinList(skinList);
	properties->SetSkinListNode(listNode);
	properties->SetDataBase(dBase);
	properties->SetLibAudio(libAudio);
	properties->Init();

	pageTags.SetProperties(&properties);
	pageCover.SetProperties(&properties);
	pageLyrics.SetProperties(&properties);

	pageTags.SetLanguage(lang);
	pageCover.SetLanguage(lang);
	pageLyrics.SetLanguage(lang);

	isRadioOpen = properties->IsRadioDefault() || properties->IsRadioCustom() || properties->IsPartOfCue();

	tabProperties = ::GetDlgItem(thisWnd, IDC_TAB_PROPERTIES);

	TC_ITEM item = {};
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 3));
	TabCtrl_InsertItem(tabProperties, 0, &item);
	if (!isRadioOpen)
	{
		item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 4));
		TabCtrl_InsertItem(tabProperties, 1, &item);
		item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 5));
		TabCtrl_InsertItem(tabProperties, 2, &item);
	}
	else
		::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_SAVE), FALSE);

	CRect rcTab;
	::GetWindowRect(tabProperties, rcTab);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcTab, 2);

	TabCtrl_AdjustRect(tabProperties, FALSE, rcTab);

	pageTags.CreateModelessDialog(thisWnd, IDD_DLGPAGETAGS);
	::MoveWindow(pageTags.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageTags.Wnd());
	::ShowWindow(pageTags.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageTags.Wnd(), ETDT_ENABLETAB);

	pageCover.CreateModelessDialog(thisWnd, IDD_DLGPAGECOVER);
	::MoveWindow(pageCover.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageCover.Wnd());
	::ShowWindow(pageCover.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageCover.Wnd(), ETDT_ENABLETAB);

	pageLyrics.CreateModelessDialog(thisWnd, IDD_DLGPAGELYRICS);
	::MoveWindow(pageLyrics.Wnd(), rcTab.left, rcTab.top, rcTab.Width(), rcTab.Height(), FALSE);
	::BringWindowToTop(pageLyrics.Wnd());
	::ShowWindow(pageLyrics.Wnd(), SW_HIDE);
	::EnableThemeDialogTexture(pageLyrics.Wnd(), ETDT_ENABLETAB);

	if (isOpenLyrics)
	{
		SelectPage(2);
		TabCtrl_SetCurSel(tabProperties, 2);
	}
	else
	{
		SelectPage(0);
		TabCtrl_SetCurSel(tabProperties, 0);
	}

	if (!isOpenLyrics && !properties->IsMultiple())
		CreateNextPrevToolbar();

	progressControl = ::GetDlgItem(thisWnd, IDC_PROGRESS_TAGS);

	if (properties->IsEmpty())
		::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_SAVE), FALSE);

	pageTags.Fill();
	pageLyrics.Fill();
}

void DlgProperties::OnBnClickedOK()
{
	properties->WaitThread();
}

void DlgProperties::OnBnClickedButtonSave()
{
	SaveTags(true);
}

bool DlgProperties::SaveTags(bool close)
{
	if (properties->IsRadioDefault() || properties->IsRadioCustom())
		return false;

	pageTags.Save();
	pageCover.Save();
	pageLyrics.Save();

	bool result = false;

	if (!properties->IsMultiple())
	{
		WindowEx::BeginWaitCursor();
		result = properties->SaveTags();
		WindowEx::EndWaitCursor();
		if (close)
			EndDialog(IDOK);
	}
	else
	{
		::EnableWindow(thisWnd, FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDCANCEL), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_SAVE), FALSE);

		::SendMessage(progressControl, PBM_SETRANGE32, 0, properties->GetProgressCount());
		::ShowWindow(progressControl, SW_SHOW);

		namespace arg = std::placeholders;

		properties->SetThreadEndFunc(std::bind(&DlgProperties::ThreadEndFunc, this));
		properties->SetProgressFunc(std::bind(&DlgProperties::ProgressFunc, this, arg::_1));

		result = properties->SaveTags();

		if (!result)
			EndDialog(IDOK);
	}

	return result;
}

void DlgProperties::OnBnClickedCancel()
{
	pageCover.SaveOptions();
	pageLyrics.SaveOptions();
}

void DlgProperties::ThreadEndFunc()
{
	::PostMessage(thisWnd, WM_COMMAND, IDOK, 0);
}

void DlgProperties::ProgressFunc(int pos)
{
	::SendMessage(progressControl, PBM_SETPOS, pos, 0);
}

void DlgProperties::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);
	if (nmhdr->hwndFrom == tabProperties && nmhdr->code == TCN_SELCHANGE)
	{
		::SetFocus(tabProperties);

		SelectPage(TabCtrl_GetCurSel(tabProperties));
	}
}

void DlgProperties::HidePages()
{
	LONG_PTR exStyle = ::GetWindowLongPtr(thisWnd, GWL_EXSTYLE);
	if (exStyle & WS_EX_ACCEPTFILES)
		::SetWindowLongPtr(thisWnd, GWL_EXSTYLE, exStyle & ~WS_EX_ACCEPTFILES);

	::ShowWindow(pageTags.Wnd(), SW_HIDE);
	::ShowWindow(pageCover.Wnd(), SW_HIDE);
	::ShowWindow(pageLyrics.Wnd(), SW_HIDE);
}

void DlgProperties::SelectPage(int page)
{
	if (page == 0)
	{
		HidePages();
		::ShowWindow(pageTags.Wnd(), SW_SHOW);
	}
	else if (page == 1)
	{
		pageCover.LoadCover();

		HidePages();
		::ShowWindow(pageCover.Wnd(), SW_SHOW);
		LONG_PTR exStyle = ::GetWindowLongPtr(thisWnd, GWL_EXSTYLE);
		if (!(exStyle & WS_EX_ACCEPTFILES))
			::SetWindowLongPtr(thisWnd, GWL_EXSTYLE, exStyle | WS_EX_ACCEPTFILES);
	}
	else if (page == 2)
	{
		pageLyrics.LoadLyrics();

		HidePages();
		::ShowWindow(pageLyrics.Wnd(), SW_SHOW);
	}
}

void DlgProperties::OnDropFiles(HDROP hDropInfo)
{
	UINT numberFiles = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (numberFiles > 0)
	{
		std::wstring coverFile;

		wchar_t file[MAX_PATH];
		if (::DragQueryFile(hDropInfo, 0, file, sizeof(file) / sizeof(wchar_t)))
			coverFile = file;

		::DragFinish(hDropInfo);

		if (!coverFile.empty())
			pageCover.LoadNewCover(file);
	}
	else
		::DragFinish(hDropInfo);
}

void DlgProperties::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == 10000 || LOWORD(wParam) == 10001)
	{
		if (SaveTags(false))
			callbackChanged(properties.get());

		pageTags.Reset();
		pageCover.Reset();
		pageLyrics.Reset();

		ListNodeUnsafe node = nullptr;

		if (LOWORD(wParam) == 10000)
			node = properties->PrevTrack();
		else if (LOWORD(wParam) == 10001)
			node = properties->NextTrack();

		if (node)
		{
			bool isRadioOpenOld = isRadioOpen;

			properties.reset(new Properties());
			properties->SetSkinList(skinList);
			properties->SetSkinListNode(node);
			properties->SetDataBase(dBase);
			properties->SetLibAudio(libAudio);
			properties->Init();

			// For radio
			isRadioOpen = properties->IsRadioDefault() || properties->IsRadioCustom() || properties->IsPartOfCue();
			if (isRadioOpen != isRadioOpenOld)
			{
				if (isRadioOpen)
				{
					::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_SAVE), FALSE);

					if (TabCtrl_GetCurSel(tabProperties) > 0)
					{
						TabCtrl_SetCurSel(tabProperties, 0);
						::SetFocus(tabProperties);
						TabCtrl_DeleteItem(tabProperties, 1);
						TabCtrl_DeleteItem(tabProperties, 1);
						HidePages();
						SelectPage(0);
					}
					else
					{
						TabCtrl_DeleteItem(tabProperties, 1);
						TabCtrl_DeleteItem(tabProperties, 1);
						::InvalidateRect(pageTags.Wnd(), NULL, TRUE);
					}
					pageTags.DisableTags(true);
					pageTags.ResetRadio();
				}
				else
				{
					::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_SAVE), TRUE);

					TC_ITEM item = {};
					item.mask = TCIF_TEXT;
					item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 4));
					TabCtrl_InsertItem(tabProperties, 1, &item);
					item.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 5));
					TabCtrl_InsertItem(tabProperties, 2, &item);
					::InvalidateRect(pageTags.Wnd(), NULL, TRUE);
					pageTags.DisableTags(false);
				}
			}

			pageTags.Fill();
			pageLyrics.Fill();

			pageCover.SetUnloaded();
			pageLyrics.SetUnloaded();

			int tabCurSel = TabCtrl_GetCurSel(tabProperties);
			if (tabCurSel == 1) // Cover
				pageCover.LoadCover();
			else if (tabCurSel == 2) // Lyrics
				pageLyrics.LoadLyrics();
		}
	}
}

void DlgProperties::CreateNextPrevToolbar()
{
	HINSTANCE instance = ::GetModuleHandle(NULL);

	CRect rcTool;
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_STATIC_NEXT_PREV), rcTool);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcTool, 2);
	//rcTool.left += (rcTool.Width() - 22) / 2; // For High DPI
	//rcTool.top += 1;

	toolImageList = ::ImageList_Create(24, 24, ILC_COLOR32, 0, 0);
	HICON iconPrev = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_TAG_PREV), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);
	HICON iconNext = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_TAG_NEXT), IMAGE_ICON, 24, 24, LR_DEFAULTCOLOR);
	::ImageList_AddIcon(toolImageList, iconPrev);
	::ImageList_AddIcon(toolImageList, iconNext);
	DestroyIcon(iconPrev);
	DestroyIcon(iconNext);

	toolBar = ::CreateWindowEx(0, TOOLBARCLASSNAME, L"", CCS_NODIVIDER|CCS_NORESIZE|TBSTYLE_LIST|TBSTYLE_FLAT|
		WS_CHILD|WS_VISIBLE, rcTool.left, rcTool.top, rcTool.Width(), rcTool.Height(), thisWnd, NULL, instance, NULL);

	::SendMessage(toolBar, TB_SETIMAGELIST, 0, (LPARAM)toolImageList);

	::SendMessage(toolBar, TB_SETBUTTONSIZE, 0, MAKELPARAM(24, rcTool.Height() - 1));

	// Tooltips for toolbar
	toolTip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, L"", WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, toolBar, NULL, instance, NULL);

	TOOLINFO ti2 = {};
	ti2.cbSize = sizeof(TOOLINFO);

	ti2.hwnd = toolBar;
	ti2.uId = 10000;
	ti2.lpszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 6));
	::SendMessage(toolTip, TTM_ADDTOOL, 0, (LPARAM)&ti2);

	ti2.uId = 10001;
	ti2.lpszText = const_cast<wchar_t*>(lang->GetLine(Lang::PropertiesDialog, 7));
	::SendMessage(toolTip, TTM_ADDTOOL, 0, (LPARAM)&ti2);

	::SendMessage(toolTip, TTM_ACTIVATE, TRUE, 0);
	::SendMessage(toolBar, TB_SETTOOLTIPS, (WPARAM)toolTip, 0);

	// Buttons for toolbar
	TBBUTTON button = {};
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;

	button.idCommand = 10000;
	button.iBitmap = 0;
	button.iString = 0;
	::SendMessage(toolBar, TB_INSERTBUTTON, 0, (LPARAM)&button);

	button.idCommand = 10001;
	button.iBitmap = 1;
	::SendMessage(toolBar, TB_INSERTBUTTON, 1, (LPARAM)&button);
}

