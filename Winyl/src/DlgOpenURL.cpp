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

// DlgOpenURL.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgOpenURL.h"


// DlgOpenURL dialog

DlgOpenURL::DlgOpenURL()
{

}

DlgOpenURL::~DlgOpenURL()
{

}

INT_PTR DlgOpenURL::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		}
		return TRUE;
	}

	return FALSE;
}

void DlgOpenURL::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::OpenURLDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::OpenURLDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::OpenURLDialog, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_NAME, lang->GetLine(Lang::OpenURLDialog, 4));
	::SetDlgItemText(thisWnd, IDC_STATIC_URL, lang->GetLine(Lang::OpenURLDialog, 5));
	
	::SetDlgItemText(thisWnd, IDC_EDIT_NAME, lang->GetLine(Lang::OpenURLDialog, 3));
}

void DlgOpenURL::OnBnClickedOK()
{
	newName = HelperGetDlgItemText(thisWnd, IDC_EDIT_NAME);
	newURL  = HelperGetDlgItemText(thisWnd, IDC_EDIT_URL);
}

void DlgOpenURL::OnBnClickedCancel()
{

}

