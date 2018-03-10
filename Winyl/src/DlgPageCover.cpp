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
#include "DlgPageCover.h"
#include "FileDialogEx.h"

DlgPageCover::DlgPageCover()
{

}

DlgPageCover::~DlgPageCover()
{

}

INT_PTR DlgPageCover::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_COVER_LOAD:
			OnBnClickedCoverLoad();
			return TRUE;
		case IDC_BUTTON_COVER_GOOGLE:
			OnBnClickedCoverGoogle();
			return TRUE;
		}
		return TRUE;
	case WM_DRAWITEM:
		OnDrawItem((int)wParam, (LPDRAWITEMSTRUCT)lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgPageCover::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_COVER2, L"");
	::SetDlgItemText(thisWnd, IDC_BUTTON_COVER_LOAD, lang->GetLine(Lang::CoverPage, 0));
	::SetDlgItemText(thisWnd, IDC_BUTTON_COVER_REMOVE, lang->GetLine(Lang::CoverPage, 1));
	::SetDlgItemText(thisWnd, IDC_STATIC_NOCOVER, lang->GetLine(Lang::CoverPage, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_DRAGCOVER, lang->GetLine(Lang::CoverPage, 4));
	::SetDlgItemText(thisWnd, IDC_CHECK_SAVE_COVER_TAGS, lang->GetLine(Lang::CoverPage, 5));
	::SetDlgItemText(thisWnd, IDC_CHECK_SAVE_COVER_FILE, lang->GetLine(Lang::CoverPage, 6));

	::CheckDlgButton(thisWnd, IDC_CHECK_SAVE_COVER_TAGS, isSaveCoverTags);
	::CheckDlgButton(thisWnd, IDC_CHECK_SAVE_COVER_FILE, isSaveCoverFile);
}

void DlgPageCover::Save()
{
	SaveOptions();

	properties->get()->SetSaveCoverToTags(isSaveCoverTags);
	properties->get()->SetSaveCoverToFile(isSaveCoverFile);
}

void DlgPageCover::SaveOptions()
{
	isSaveCoverTags = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SAVE_COVER_TAGS);
	isSaveCoverFile = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SAVE_COVER_FILE);
}

void DlgPageCover::LoadCover()
{
	if (!isLoaded)
	{
		isLoaded = true;

		if (properties->get()->IsMultiple())
		{
			::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_NOCOVER), SW_HIDE);
			return;
		}

		WindowEx::BeginWaitCursor();

		HWND wndCover = ::GetDlgItem(thisWnd, IDC_STATIC_COVER);

		CRect rc;
		::GetClientRect(wndCover, rc);

		image.Clear();
		if (properties->get()->LoadCover())
		{
			image.ThumbnailFromSource(properties->get()->coverLoader.GetImage(), rc.Width(), rc.Height());
			properties->get()->FreeCover();
		}

		if (image.IsValid())
		{
			if (!::IsWindowVisible(wndCover))
			{
				::ShowWindow(wndCover, SW_SHOW);
				::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DRAGCOVER), SW_HIDE);
				if (!properties->get()->IsMultiple())
					::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_NOCOVER), SW_HIDE);
			}
			else
				::InvalidateRect(wndCover, NULL, TRUE);
		}
		else
		{
			if (::IsWindowVisible(wndCover))
			{
				::ShowWindow(wndCover, SW_HIDE);
				::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DRAGCOVER), SW_SHOW);
				if (!properties->get()->IsMultiple())
					::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_NOCOVER), SW_SHOW);
			}
		}

		WindowEx::EndWaitCursor();
	}
}

void DlgPageCover::LoadNewCover(const std::wstring& file)
{
	WindowEx::BeginWaitCursor();

	properties->get()->LoadNewCover(file);

	HWND wndCover = ::GetDlgItem(thisWnd, IDC_STATIC_COVER);

	CRect rc;
	::GetClientRect(wndCover, rc);

	if (image.IsValid())
		image.Clear();

	if (!properties->get()->GetNewCover().empty())
	{
		ExImage::Source source;
		source.LoadBuffer(properties->get()->GetNewCover().data(), (int)properties->get()->GetNewCover().size());
		if (source.IsValid())
			image.ThumbnailFromSource(source, rc.Width(), rc.Height());
	}

	if (image.IsValid())
	{
		if (!::IsWindowVisible(wndCover))
		{
			::ShowWindow(wndCover, SW_SHOW);
			::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DRAGCOVER), SW_HIDE);
			if (!properties->get()->IsMultiple())
				::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_NOCOVER), SW_HIDE);
		}
		else
			::InvalidateRect(wndCover, NULL, TRUE);
	}
	else
	{
		if (::IsWindowVisible(wndCover))
		{
			::ShowWindow(wndCover, SW_HIDE);
			::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DRAGCOVER), SW_SHOW);
			if (!properties->get()->IsMultiple())
				::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_NOCOVER), SW_SHOW);
		}
	}

	WindowEx::EndWaitCursor();
}

void DlgPageCover::Reset()
{
	properties->get()->FreeNewCover();
}

void DlgPageCover::OnBnClickedCoverLoad()
{
	FileDialogEx fileDialog;

	FileDialogEx::FILE_TYPES fileTypes[] =
	{
		{L"All Image Files", L"*.jpeg;*.jpg;*.png"},
		{L"JPEG Files (*.jpeg; *.jpg;)", L"*.jpeg;*.jpg"},
		{L"PNG Files (*.png)", L"*.png"}
	};
	int countTypes = sizeof(fileTypes) / sizeof(fileTypes[0]);

	fileDialog.SetFileTypes(fileTypes, countTypes);

	if (fileDialog.DoModalFile(thisWnd, false, false))
	{
		LoadNewCover(fileDialog.GetFile());
	}
}

void DlgPageCover::OnBnClickedCoverGoogle()
{
	std::wstring album = properties->get()->fields.album;
	std::wstring artist;
	if (!properties->get()->fields.albumArtist.empty())
		artist = properties->get()->fields.albumArtist;
	else
		artist = properties->get()->fields.artist;

	std::wstring url = L"https://www.google.com/search?q=" + artist + L"+" + album + L"+cover&tbs=imgo:1,isz:lt,islt:qsvga&tbm=isch";

	::ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void DlgPageCover::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_STATIC_COVER)
	{
		image.Draw(lpDrawItemStruct->hDC, 0, 0);
	}
}
