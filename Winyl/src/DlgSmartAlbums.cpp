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
#include "DlgSmartAlbums.h"

DlgSmartAlbums::DlgSmartAlbums()
{

}

DlgSmartAlbums::~DlgSmartAlbums()
{

}

INT_PTR DlgSmartAlbums::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDC_CHECK_TRACKS_COUNT:
			OnBnClickedCheckTracksCount();
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgSmartAlbums::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::SmartFilter, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::SmartFilter, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::SmartFilter, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_TRACKS_COUNT, lang->GetLine(Lang::SmartFilter, 17));

	::SetDlgItemText(thisWnd, IDC_CHECK_TRACKS_COUNT, lang->GetLine(Lang::SmartFilter, 10));

	HWND comboTracksCount = ::GetDlgItem(thisWnd, IDC_COMBO_TRACKS_COUNT);
	ComboBox_AddString(comboTracksCount, lang->GetLine(Lang::SmartFilter, 11));
	ComboBox_AddString(comboTracksCount, lang->GetLine(Lang::SmartFilter, 12));
	ComboBox_AddString(comboTracksCount, lang->GetLine(Lang::SmartFilter, 13));

	if (smart->tracksCount)
	{
		::CheckDlgButton(thisWnd, IDC_CHECK_TRACKS_COUNT, BST_CHECKED);
		OnBnClickedCheckTracksCount();
		ComboBox_SetCurSel(comboTracksCount, smart->tracksCount - 1);
		::SetDlgItemInt(thisWnd, IDC_EDIT_TRACKS_COUNT, (UINT)smart->tracksCountNum, FALSE);
	}
	else
	{
		ComboBox_SetCurSel(comboTracksCount, 0);
		::SetDlgItemInt(thisWnd, IDC_EDIT_TRACKS_COUNT, 5, FALSE);
	}
}

void DlgSmartAlbums::OnBnClickedOK()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACKS_COUNT))
	{
		HWND comboTracksCount = ::GetDlgItem(thisWnd, IDC_COMBO_TRACKS_COUNT);
		smart->tracksCount = ComboBox_GetCurSel(comboTracksCount) + 1;
		smart->tracksCountNum = (int)::GetDlgItemInt(thisWnd, IDC_EDIT_TRACKS_COUNT, NULL, FALSE);
	}
	else
	{
		smart->tracksCount = 0;
		smart->tracksCountNum = 0;
	}
}

void DlgSmartAlbums::OnBnClickedCancel()
{

}

void DlgSmartAlbums::OnBnClickedCheckTracksCount()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_TRACKS_COUNT))
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_TRACKS_COUNT), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TRACKS_COUNT), TRUE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_TRACKS_COUNT), TRUE);
	}
	else
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_COMBO_TRACKS_COUNT), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_TRACKS_COUNT), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_TRACKS_COUNT), FALSE);
	}
}
