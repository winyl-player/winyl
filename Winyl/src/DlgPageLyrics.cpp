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
#include "DlgPageLyrics.h"


DlgPageLyrics::DlgPageLyrics()
{

}

DlgPageLyrics::~DlgPageLyrics()
{

}

INT_PTR DlgPageLyrics::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgPageLyrics::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_LYRICIST, lang->GetLine(Lang::LyricsPage, 0));
	::SetDlgItemText(thisWnd, IDC_CHECK_LYRICIST, lang->GetLine(Lang::LyricsPage, 0));
	::SetDlgItemText(thisWnd, IDC_CHECK_SAVE_LYRICS_TAGS, lang->GetLine(Lang::LyricsPage, 1));
	::SetDlgItemText(thisWnd, IDC_CHECK_SAVE_LYRICS_FILE, lang->GetLine(Lang::LyricsPage, 2));

	::CheckDlgButton(thisWnd, IDC_CHECK_SAVE_LYRICS_TAGS, isSaveLyricsTags);
	::CheckDlgButton(thisWnd, IDC_CHECK_SAVE_LYRICS_FILE, isSaveLyricsFile);
}

void DlgPageLyrics::Fill()
{
	if (properties->get()->IsMultiple())
	{
		::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_TITLE), SW_HIDE);
		::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_LYRICIST), SW_SHOW);
	}

	enChange = false;
	::SetDlgItemText(thisWnd, IDC_EDIT_LYRICIST, properties->get()->fields.lyricist.c_str());
	enChange = true;
}

void DlgPageLyrics::Reset()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LYRICIST))
		::CheckDlgButton(thisWnd, IDC_CHECK_LYRICIST, BST_UNCHECKED);

	lyricsChanged = false;
	isOpenLyrics = false;
}

void DlgPageLyrics::Save()
{
	SaveOptions();

	properties->get()->SetSaveLyricsToTags(isSaveLyricsTags);
	properties->get()->SetSaveLyricsToFile(isSaveLyricsFile);

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LYRICIST))
		properties->get()->SetNewLyricist(HelperGetDlgItemText(thisWnd, IDC_EDIT_LYRICIST));

	if (lyricsChanged)
		properties->get()->SetNewLyrics(HelperGetDlgItemText(thisWnd, IDC_EDIT_LYRICS));
}

void DlgPageLyrics::SaveOptions()
{
	isSaveLyricsTags = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SAVE_LYRICS_TAGS);
	isSaveLyricsFile = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SAVE_LYRICS_FILE);
}

void DlgPageLyrics::LoadLyrics()
{
	if (!isLoaded)
	{
		isLoaded = true;

		if (isOpenLyrics)
		{
			::SetDlgItemText(thisWnd, IDC_EDIT_LYRICS, openLyrics.c_str());
			lyricsChanged = true;
			return;
		}

		if (properties->get()->IsMultiple())
		{
			::ShowWindow(::GetDlgItem(thisWnd, IDC_EDIT_LYRICS), SW_HIDE);
			::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SAVE_LYRICS_TAGS), SW_HIDE);
			::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SAVE_LYRICS_FILE), SW_HIDE);
			return;
		}

		WindowEx::BeginWaitCursor();

		enChange = false;
		if (properties->get()->LoadLyrics())
		{
			::SetDlgItemText(thisWnd, IDC_EDIT_LYRICS, properties->get()->lyricsLoader.GetLyrics().c_str());
			properties->get()->FreeLyrics();
		}
		else
			::SetDlgItemText(thisWnd, IDC_EDIT_LYRICS, L"");
		enChange = true;

		WindowEx::EndWaitCursor();
	}
}

void DlgPageLyrics::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != EN_CHANGE || !enChange)
		return;

	switch (LOWORD(wParam))
	{
	case IDC_EDIT_LYRICIST:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_LYRICIST))
			::CheckDlgButton(thisWnd, IDC_CHECK_LYRICIST, BST_CHECKED);
		break;
	case IDC_EDIT_LYRICS:
		lyricsChanged = true;
		break;
	}
}
