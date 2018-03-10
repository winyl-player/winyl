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
#include "DlgPageLibraryOpt.h"

DlgPageLibraryOpt::DlgPageLibraryOpt()
{

}

DlgPageLibraryOpt::~DlgPageLibraryOpt()
{

}

INT_PTR DlgPageLibraryOpt::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	}

	return FALSE;
}

void DlgPageLibraryOpt::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_BASIC, lang->GetLine(Lang::LibraryDialog, 10));
	::SetDlgItemText(thisWnd, IDC_STATIC_ADVANCED, lang->GetLine(Lang::LibraryDialog, 11));
	::SetDlgItemText(thisWnd, IDC_CHECK_IGNORE_DELETED, lang->GetLine(Lang::LibraryDialog, 12));
	::SetDlgItemText(thisWnd, IDC_CHECK_REMOVE_MISSING, lang->GetLine(Lang::LibraryDialog, 13));
	::SetDlgItemText(thisWnd, IDC_CHECK_FIND_MOVED, lang->GetLine(Lang::LibraryDialog, 14));
	::SetDlgItemText(thisWnd, IDC_CHECK_RESCAN_ALL, lang->GetLine(Lang::LibraryDialog, 15));


	::CheckDlgButton(thisWnd, IDC_CHECK_REMOVE_MISSING, isRemoveMissing);
	::CheckDlgButton(thisWnd, IDC_CHECK_IGNORE_DELETED, isIgnoreDeleted);
	::CheckDlgButton(thisWnd, IDC_CHECK_FIND_MOVED, BST_UNCHECKED);
	::CheckDlgButton(thisWnd, IDC_CHECK_RESCAN_ALL, BST_UNCHECKED);
}

void DlgPageLibraryOpt::Save()
{
	isRemoveMissing = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_REMOVE_MISSING);
	isIgnoreDeleted = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_IGNORE_DELETED);

	isFindMoved = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_FIND_MOVED);
	isRescanAll = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_RESCAN_ALL);
}
