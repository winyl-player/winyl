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
#include "DlgPageTags.h"

DlgPageTags::DlgPageTags()
{

}

DlgPageTags::~DlgPageTags()
{

}

INT_PTR DlgPageTags::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void DlgPageTags::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_TITLE, lang->GetLine(Lang::TagsPage, 0));
	::SetDlgItemText(thisWnd, IDC_STATIC_ARTIST, lang->GetLine(Lang::TagsPage, 1));
	::SetDlgItemText(thisWnd, IDC_STATIC_ALBUM, lang->GetLine(Lang::TagsPage, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_ALBUMARTIST, lang->GetLine(Lang::TagsPage, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_COMPOSER, lang->GetLine(Lang::TagsPage, 4));
	::SetDlgItemText(thisWnd, IDC_STATIC_YEAR, lang->GetLine(Lang::TagsPage, 5));
	::SetDlgItemText(thisWnd, IDC_STATIC_TRACK, lang->GetLine(Lang::TagsPage, 6));
	::SetDlgItemText(thisWnd, IDC_STATIC_TRACKTOTAL, lang->GetLine(Lang::TagsPage, 8));
	::SetDlgItemText(thisWnd, IDC_STATIC_DISC, lang->GetLine(Lang::TagsPage, 7));
	::SetDlgItemText(thisWnd, IDC_STATIC_DISCTOTAL, lang->GetLine(Lang::TagsPage, 8));
	::SetDlgItemText(thisWnd, IDC_STATIC_GENRE, lang->GetLine(Lang::TagsPage, 9));
	::SetDlgItemText(thisWnd, IDC_STATIC_COMMENT, lang->GetLine(Lang::TagsPage, 10));
	::SetDlgItemText(thisWnd, IDC_STATIC_FILE2, lang->GetLine(Lang::TagsPage, 11));
	::SetDlgItemText(thisWnd, IDC_STATIC_QUALITY, lang->GetLine(Lang::TagsPage, 12));

	::SetDlgItemText(thisWnd, IDC_CHECK_TITLE, lang->GetLine(Lang::TagsPage, 0));
	::SetDlgItemText(thisWnd, IDC_CHECK_ARTIST, lang->GetLine(Lang::TagsPage, 1));
	::SetDlgItemText(thisWnd, IDC_CHECK_ALBUM, lang->GetLine(Lang::TagsPage, 3));
	::SetDlgItemText(thisWnd, IDC_CHECK_ALBUMARTIST, lang->GetLine(Lang::TagsPage, 2));
	::SetDlgItemText(thisWnd, IDC_CHECK_COMPOSER, lang->GetLine(Lang::TagsPage, 4));
	::SetDlgItemText(thisWnd, IDC_CHECK_YEAR, lang->GetLine(Lang::TagsPage, 5));
	::SetDlgItemText(thisWnd, IDC_CHECK_TRACK, lang->GetLine(Lang::TagsPage, 6));
	::SetDlgItemText(thisWnd, IDC_CHECK_TRACKTOTAL, lang->GetLine(Lang::TagsPage, 8));
	::SetDlgItemText(thisWnd, IDC_CHECK_DISC, lang->GetLine(Lang::TagsPage, 7));
	::SetDlgItemText(thisWnd, IDC_CHECK_DISCTOTAL, lang->GetLine(Lang::TagsPage, 8));
	::SetDlgItemText(thisWnd, IDC_CHECK_GENRE, lang->GetLine(Lang::TagsPage, 9));
	::SetDlgItemText(thisWnd, IDC_CHECK_COMMENT, lang->GetLine(Lang::TagsPage, 10));
}

void DlgPageTags::EnableMultiple()
{
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_TITLE),       SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_ARTIST),      SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_ALBUM),       SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_ALBUMARTIST), SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_COMPOSER),    SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_YEAR),        SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_TRACK),       SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_TRACKTOTAL),  SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DISC),        SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_DISCTOTAL),   SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_GENRE),       SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_STATIC_COMMENT),     SW_HIDE);

	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_TITLE),        SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_ARTIST),       SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_ALBUM),        SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_ALBUMARTIST),  SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_COMPOSER),     SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_YEAR),         SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_TRACK),        SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_TRACKTOTAL),   SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_DISC),         SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_DISCTOTAL),    SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_GENRE),        SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_COMMENT),      SW_SHOW);
}

void DlgPageTags::DisableTags(bool disable)
{
/*
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TITLE),       FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_ARTIST),      FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_ALBUM),       FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_ALBUMARTIST), FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_COMPOSER),    FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_YEAR),        FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TRACK),       FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TRACKTOTAL),  FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_DISC),        FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_DISCTOTAL),   FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_GENRE),       FALSE);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_COMMENT),     FALSE);
*/
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_TITLE),       EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_ARTIST),      EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_ALBUM),       EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_ALBUMARTIST), EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_COMPOSER),    EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_YEAR),        EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_TRACK),       EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_TRACKTOTAL),  EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_DISC),        EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_DISCTOTAL),   EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_GENRE),       EM_SETREADONLY, disable, 0);
	::SendMessage(::GetDlgItem(thisWnd, IDC_EDIT_COMMENT),     EM_SETREADONLY, disable, 0);

}

void DlgPageTags::Fill()
{
	if (properties->get()->IsEmpty())
	{
		DisableTags(true);
		return;
	}

	if (properties->get()->IsMultiple())
	{
		EnableMultiple();
	}
	else
	{
		// Radio added from radio list or by user
		if (properties->get()->IsRadioDefault() || properties->get()->IsRadioCustom())
		{
			::SetDlgItemText(thisWnd, IDC_EDIT_TITLE, properties->get()->fields.title.c_str());
			::SetDlgItemText(thisWnd, IDC_EDIT_FILE, properties->get()->fields.file.c_str());
			::SetDlgItemText(thisWnd, IDC_EDIT_QUALITY, properties->get()->fields.quality.c_str());
			DisableTags(true);
			return;
		}
		else if (properties->get()->IsPartOfCue()) // The file is part of cue
			DisableTags(true);
	}

	enChange = false;
	::SetDlgItemText(thisWnd, IDC_EDIT_TITLE, properties->get()->fields.title.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_ARTIST, properties->get()->fields.artist.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_ALBUM, properties->get()->fields.album.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_ALBUMARTIST, properties->get()->fields.albumArtist.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_COMPOSER, properties->get()->fields.composer.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_YEAR, properties->get()->fields.year.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_TRACK, properties->get()->fields.track.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_TRACKTOTAL, properties->get()->fields.totalTracks.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_DISC, properties->get()->fields.disc.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_DISCTOTAL, properties->get()->fields.totalDiscs.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_GENRE, properties->get()->fields.genre.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_COMMENT, properties->get()->fields.comment.c_str());

	::SetDlgItemText(thisWnd, IDC_EDIT_FILE, properties->get()->fields.file.c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_QUALITY, properties->get()->fields.quality.c_str());
	enChange = true;
}

void DlgPageTags::Reset()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACK))
		::CheckDlgButton(thisWnd, IDC_CHECK_TRACK, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISC))
		::CheckDlgButton(thisWnd, IDC_CHECK_DISC, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACKTOTAL))
		::CheckDlgButton(thisWnd, IDC_CHECK_TRACKTOTAL, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISCTOTAL))
		::CheckDlgButton(thisWnd, IDC_CHECK_DISCTOTAL, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TITLE))
		::CheckDlgButton(thisWnd, IDC_CHECK_TITLE, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUM))
		::CheckDlgButton(thisWnd, IDC_CHECK_ALBUM, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ARTIST))
		::CheckDlgButton(thisWnd, IDC_CHECK_ARTIST, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUMARTIST))
		::CheckDlgButton(thisWnd, IDC_CHECK_ALBUMARTIST, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMPOSER))
		::CheckDlgButton(thisWnd, IDC_CHECK_COMPOSER, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_GENRE))
		::CheckDlgButton(thisWnd, IDC_CHECK_GENRE, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_YEAR))
		::CheckDlgButton(thisWnd, IDC_CHECK_YEAR, BST_UNCHECKED);
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMMENT))
		::CheckDlgButton(thisWnd, IDC_CHECK_COMMENT, BST_UNCHECKED);
}

void DlgPageTags::ResetRadio()
{
	enChange = false;
	::SetDlgItemText(thisWnd, IDC_EDIT_TITLE, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_ARTIST, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_ALBUM, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_ALBUMARTIST, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_COMPOSER, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_YEAR, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_TRACK, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_TRACKTOTAL, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_DISC, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_DISCTOTAL, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_GENRE, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_COMMENT, L"");

	::SetDlgItemText(thisWnd, IDC_EDIT_FILE, L"");
	::SetDlgItemText(thisWnd, IDC_EDIT_QUALITY, L"");
	enChange = true;
}

void DlgPageTags::Save()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACK))
		properties->get()->SetNewTrack(HelperGetDlgItemText(thisWnd, IDC_EDIT_TRACK));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISC))
		properties->get()->SetNewDisc(HelperGetDlgItemText(thisWnd, IDC_EDIT_DISC));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACKTOTAL))
		properties->get()->SetNewTotalTracks(HelperGetDlgItemText(thisWnd, IDC_EDIT_TRACKTOTAL));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISCTOTAL))
		properties->get()->SetNewTotalDiscs(HelperGetDlgItemText(thisWnd, IDC_EDIT_DISCTOTAL));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TITLE))
		properties->get()->SetNewTitle(HelperGetDlgItemText(thisWnd, IDC_EDIT_TITLE));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUM))
		properties->get()->SetNewAlbum(HelperGetDlgItemText(thisWnd, IDC_EDIT_ALBUM));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ARTIST))
		properties->get()->SetNewArtist(HelperGetDlgItemText(thisWnd, IDC_EDIT_ARTIST));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUMARTIST))
		properties->get()->SetNewAlbumArtist(HelperGetDlgItemText(thisWnd, IDC_EDIT_ALBUMARTIST));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMPOSER))
		properties->get()->SetNewComposer(HelperGetDlgItemText(thisWnd, IDC_EDIT_COMPOSER));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_GENRE))
		properties->get()->SetNewGenre(HelperGetDlgItemText(thisWnd, IDC_EDIT_GENRE));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_YEAR))
		properties->get()->SetNewYear(HelperGetDlgItemText(thisWnd, IDC_EDIT_YEAR));
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMMENT))
		properties->get()->SetNewComment(HelperGetDlgItemText(thisWnd, IDC_EDIT_COMMENT));
}

void DlgPageTags::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (HIWORD(wParam) != EN_CHANGE || !enChange)
		return;

	switch (LOWORD(wParam))
	{
	case IDC_EDIT_TRACK:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACK))
			::CheckDlgButton(thisWnd, IDC_CHECK_TRACK, BST_CHECKED);
		break;
	case IDC_EDIT_DISC:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISC))
			::CheckDlgButton(thisWnd, IDC_CHECK_DISC, BST_CHECKED);
		break;
	case IDC_EDIT_TRACKTOTAL:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACKTOTAL))
			::CheckDlgButton(thisWnd, IDC_CHECK_TRACKTOTAL, BST_CHECKED);
		break;
	case IDC_EDIT_DISCTOTAL:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_DISCTOTAL))
			::CheckDlgButton(thisWnd, IDC_CHECK_DISCTOTAL, BST_CHECKED);
		break;
	case IDC_EDIT_TITLE:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_TITLE))
			::CheckDlgButton(thisWnd, IDC_CHECK_TITLE, BST_CHECKED);
		break;
	case IDC_EDIT_ALBUM:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUM))
			::CheckDlgButton(thisWnd, IDC_CHECK_ALBUM, BST_CHECKED);
		break;
	case IDC_EDIT_ARTIST:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ARTIST))
			::CheckDlgButton(thisWnd, IDC_CHECK_ARTIST, BST_CHECKED);
		break;
	case IDC_EDIT_ALBUMARTIST:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUMARTIST))
			::CheckDlgButton(thisWnd, IDC_CHECK_ALBUMARTIST, BST_CHECKED);
		break;
	case IDC_EDIT_COMPOSER:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMPOSER))
			::CheckDlgButton(thisWnd, IDC_CHECK_COMPOSER, BST_CHECKED);
		break;
	case IDC_EDIT_GENRE:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_GENRE))
			::CheckDlgButton(thisWnd, IDC_CHECK_GENRE, BST_CHECKED);
		break;
	case IDC_EDIT_YEAR:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_YEAR))
			::CheckDlgButton(thisWnd, IDC_CHECK_YEAR, BST_CHECKED);
		break;
	case IDC_EDIT_COMMENT:
		if (!::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMMENT))
			::CheckDlgButton(thisWnd, IDC_CHECK_COMMENT, BST_CHECKED);
		break;
	}
}
