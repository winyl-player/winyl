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
#include "DlgSmartTracks.h"

DlgSmartTracks::DlgSmartTracks()
{

}

DlgSmartTracks::~DlgSmartTracks()
{

}

INT_PTR DlgSmartTracks::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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
		case IDC_CHECK_PLAY_COUNT:
			OnBnClickedCheckPlayCount();
			return TRUE;
		case IDC_CHECK_RATING:
			OnBnClickedCheckRating();
			return TRUE;
		case IDC_CHECK_LAST_PLAYED:
			OnBnClickedCheckLastPlayed();
			return TRUE;
		case IDC_CHECK_LAST_ADDED:
			OnBnClickedCheckLastAdded();
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgSmartTracks::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::SmartFilter, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::SmartFilter, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::SmartFilter, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_TIMES, lang->GetLine(Lang::SmartFilter, 16));

	::SetDlgItemText(thisWnd, IDC_CHECK_PLAY_COUNT, lang->GetLine(Lang::SmartFilter, 6));
	::SetDlgItemText(thisWnd, IDC_CHECK_RATING, lang->GetLine(Lang::SmartFilter, 7));
	::SetDlgItemText(thisWnd, IDC_CHECK_LAST_PLAYED, lang->GetLine(Lang::SmartFilter, 8));
	::SetDlgItemText(thisWnd, IDC_CHECK_LAST_ADDED, lang->GetLine(Lang::SmartFilter, 9));

	::SetDlgItemText(thisWnd, IDC_STATIC_FOLLOWING_RULES, lang->GetLine(Lang::SmartFilter, 5));

	HWND comboAllAny = ::GetDlgItem(thisWnd, IDC_COMBO_ALL_ANY);
	ComboBox_AddString(comboAllAny, lang->GetLine(Lang::SmartFilter, 3));
	ComboBox_AddString(comboAllAny, lang->GetLine(Lang::SmartFilter, 4));

	if (!smart->isAny)
		ComboBox_SetCurSel(comboAllAny, 0);
	else
		ComboBox_SetCurSel(comboAllAny, 1);

	HWND comboPlayCount = ::GetDlgItem(thisWnd, IDC_COMBO_PLAY_COUNT);
	ComboBox_AddString(comboPlayCount, lang->GetLine(Lang::SmartFilter, 11));
	ComboBox_AddString(comboPlayCount, lang->GetLine(Lang::SmartFilter, 12));
	ComboBox_AddString(comboPlayCount, lang->GetLine(Lang::SmartFilter, 13));

	if (smart->playCount)
	{
		::CheckDlgButton(thisWnd, IDC_CHECK_PLAY_COUNT, BST_CHECKED);
		OnBnClickedCheckPlayCount();
		ComboBox_SetCurSel(comboPlayCount, smart->playCount - 1);
		::SetDlgItemInt(thisWnd, IDC_EDIT_TIMES, (UINT)smart->playCountNum, FALSE);
	}
	else
	{
		ComboBox_SetCurSel(comboPlayCount, 0);
		::SetDlgItemInt(thisWnd, IDC_EDIT_TIMES, 0, FALSE);
	}

	HWND comboRating = ::GetDlgItem(thisWnd, IDC_COMBO_RATING);
	ComboBox_AddString(comboRating, lang->GetLine(Lang::SmartFilter, 11));
	ComboBox_AddString(comboRating, lang->GetLine(Lang::SmartFilter, 12));
	ComboBox_AddString(comboRating, lang->GetLine(Lang::SmartFilter, 13));

	HWND comboRatingStar = ::GetDlgItem(thisWnd, IDC_COMBO_RATING_STAR);
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 18));
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 19));
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 20));
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 21));
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 22));
	ComboBox_AddString(comboRatingStar, lang->GetLine(Lang::SmartFilter, 23));
	
	if (smart->rating)
	{
		::CheckDlgButton(thisWnd, IDC_CHECK_RATING, BST_CHECKED);
		OnBnClickedCheckRating();
		ComboBox_SetCurSel(comboRating, smart->rating - 1);
		ComboBox_SetCurSel(comboRatingStar, smart->ratingNum);
	}
	else
	{
		ComboBox_SetCurSel(comboRating, 0);
		ComboBox_SetCurSel(comboRatingStar, 0);
	}

	HWND comboLastPlayed = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED);
	ComboBox_AddString(comboLastPlayed, lang->GetLine(Lang::SmartFilter, 14));
	ComboBox_AddString(comboLastPlayed, lang->GetLine(Lang::SmartFilter, 15));

	HWND comboLastPlayedDate = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED_DATE);
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 24));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 25));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 26));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 27));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 28));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 29));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 30));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 31));
	ComboBox_AddString(comboLastPlayedDate, lang->GetLine(Lang::SmartFilter, 32));

	if (smart->lastPlayed)
	{
		::CheckDlgButton(thisWnd, IDC_CHECK_LAST_PLAYED, BST_CHECKED);
		OnBnClickedCheckLastPlayed();
		ComboBox_SetCurSel(comboLastPlayed, smart->lastPlayed - 1);
		ComboBox_SetCurSel(comboLastPlayedDate, smart->lastPlayedNum);
	}
	else
	{
		ComboBox_SetCurSel(comboLastPlayed, 0);
		ComboBox_SetCurSel(comboLastPlayedDate, 0);
	}

	HWND comboLastAdded = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED);
	ComboBox_AddString(comboLastAdded, lang->GetLine(Lang::SmartFilter, 14));
	ComboBox_AddString(comboLastAdded, lang->GetLine(Lang::SmartFilter, 15));

	HWND comboLastAddedDate = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED_DATE);
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 24));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 25));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 26));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 27));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 28));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 29));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 30));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 31));
	ComboBox_AddString(comboLastAddedDate, lang->GetLine(Lang::SmartFilter, 32));

	if (smart->dateAdded)
	{
		::CheckDlgButton(thisWnd, IDC_CHECK_LAST_ADDED, BST_CHECKED);
		OnBnClickedCheckLastAdded();
		ComboBox_SetCurSel(comboLastAdded, smart->dateAdded - 1);
		ComboBox_SetCurSel(comboLastAddedDate, smart->dateAddedNum);
	}
	else
	{
		ComboBox_SetCurSel(comboLastAdded, 0);
		ComboBox_SetCurSel(comboLastAddedDate, 0);
	}
}

void DlgSmartTracks::OnBnClickedOK()
{
	HWND comboAllAny = ::GetDlgItem(thisWnd, IDC_COMBO_ALL_ANY);
	if (ComboBox_GetCurSel(comboAllAny) == 0)
		smart->isAny = false;
	else
		smart->isAny = true;

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_PLAY_COUNT))
	{
		HWND comboPlayCount = ::GetDlgItem(thisWnd, IDC_COMBO_PLAY_COUNT);
		smart->playCount = ComboBox_GetCurSel(comboPlayCount) + 1;
		smart->playCountNum = (int)::GetDlgItemInt(thisWnd, IDC_EDIT_TIMES, NULL, FALSE);
	}
	else
	{
		smart->playCount = 0;
		smart->playCountNum = 0;
	}

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_RATING))
	{
		HWND comboRating = ::GetDlgItem(thisWnd, IDC_COMBO_RATING);
		HWND comboRatingStar = ::GetDlgItem(thisWnd, IDC_COMBO_RATING_STAR);
		smart->rating = ComboBox_GetCurSel(comboRating) + 1;
		smart->ratingNum = ComboBox_GetCurSel(comboRatingStar);
	}
	else
	{
		smart->rating = 0;
		smart->ratingNum = 0;
	}

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LAST_PLAYED))
	{
		HWND comboLastPlayed = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED);
		HWND comboLastPlayedDate = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED_DATE);
		smart->lastPlayed = ComboBox_GetCurSel(comboLastPlayed) + 1;
		smart->lastPlayedNum = ComboBox_GetCurSel(comboLastPlayedDate);
	}
	else
	{
		smart->lastPlayed = 0;
		smart->lastPlayedNum = 0;
	}

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LAST_ADDED))
	{
		HWND comboLastAdded = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED);
		HWND comboLastAddedDate = ::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED_DATE);
		smart->dateAdded = ComboBox_GetCurSel(comboLastAdded) + 1;
		smart->dateAddedNum = ComboBox_GetCurSel(comboLastAddedDate);
	}
	else
	{
		smart->dateAdded = 0;
		smart->dateAddedNum = 0;
	}
}

void DlgSmartTracks::OnBnClickedCancel()
{

}

void DlgSmartTracks::OnBnClickedCheckPlayCount()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_PLAY_COUNT))
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_PLAY_COUNT), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TIMES), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_TIMES), TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_PLAY_COUNT), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TIMES), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_TIMES), FALSE);
	}
}

void DlgSmartTracks::OnBnClickedCheckRating()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_RATING))
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_RATING), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_RATING_STAR), TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_RATING), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_RATING_STAR), FALSE);
	}
}

void DlgSmartTracks::OnBnClickedCheckLastPlayed()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LAST_PLAYED))
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED_DATE), TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_PLAYED_DATE), FALSE);
	}
}

void DlgSmartTracks::OnBnClickedCheckLastAdded()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_LAST_ADDED))
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED_DATE), TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_LAST_ADDED_DATE), FALSE);
	}
}
