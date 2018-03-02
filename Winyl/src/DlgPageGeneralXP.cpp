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

// DlgPageGeneralXP.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPageGeneralXP.h"


// DlgPageGeneralXP dialog

DlgPageGeneralXP::DlgPageGeneralXP()
{

}

DlgPageGeneralXP::~DlgPageGeneralXP()
{

}

INT_PTR DlgPageGeneralXP::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDC_CHECK_SELECT_ALL:
			OnBnClickedCheckSelectAll();
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgPageGeneralXP::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::GeneralPage, 11));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::GeneralPage, 12));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::GeneralPage, 13));
	::SetDlgItemText(thisWnd, IDC_CHECK_SELECT_ALL, lang->GetLine(Lang::GeneralPage, 14));

	listAssoc = ::GetDlgItem(thisWnd, IDC_LIST_ASSOC);

	ListView_SetExtendedListViewStyleEx(listAssoc, 0, LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);

	CRect rcList;
	::GetClientRect(listAssoc, rcList);

	LVCOLUMN column = {};
	column.mask = LVCF_WIDTH|LVCF_TEXT;
	column.cx = rcList.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	column.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::GeneralPage, 15));
	ListView_InsertColumn(listAssoc, 0, &column);

	assoc.PrepareAssocExtXP();

	SetWindowRedraw(listAssoc, FALSE);

	for (int i = 0, size = assoc.GetAssocExtCountXP(); i < size; ++i)
	{
		std::wstring ext = assoc.GetAssocExtXP(i);
		StringEx::MakeUpperAscii(ext);

		LVITEM item = {};
		item.mask = LVIF_TEXT|LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = 0;
		item.lParam = i;
		item.pszText = const_cast<wchar_t*>(ext.c_str());
		int index = ListView_InsertItem(listAssoc, &item);

		ListView_SetCheckState(listAssoc, index, assoc.QueryAssocXP(i));
	}

	SetWindowRedraw(listAssoc, TRUE);
}

void DlgPageGeneralXP::OnBnClickedOK()
{
	for (int i = 0, size = ListView_GetItemCount(listAssoc); i < size; ++i)
	{
		LVITEM item = {};
		item.mask = LVIF_PARAM;
		item.iItem = i;
		item.iSubItem = 0;
		if (ListView_GetItem(listAssoc, &item))
			assoc.SetAssocXP((int)item.lParam, !!ListView_GetCheckState(listAssoc, i));
	}

	assoc.ApplyAssocXP();
}

void DlgPageGeneralXP::OnBnClickedCancel()
{

}

void DlgPageGeneralXP::OnBnClickedCheckSelectAll()
{
	int check = (int)::IsDlgButtonChecked(thisWnd, IDC_CHECK_SELECT_ALL);

	SetWindowRedraw(listAssoc, FALSE);

	for (int i = 0, size = ListView_GetItemCount(listAssoc); i < size; ++i)
		ListView_SetCheckState(listAssoc, i, check);

	SetWindowRedraw(listAssoc, TRUE);
}
