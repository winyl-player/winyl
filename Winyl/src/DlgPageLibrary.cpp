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
#include "DlgPageLibrary.h"
#include "FileDialogEx.h"

// DlgPageLibrary dialog

DlgPageLibrary::DlgPageLibrary()
{

}

DlgPageLibrary::~DlgPageLibrary()
{
	if (imListFolder)
		::ImageList_Destroy(imListFolder);
}

INT_PTR DlgPageLibrary::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ADD:
			OnBnClickedButtonAdd();
			return TRUE;
		case IDC_BUTTON_DELETE:
			OnBnClickedButtonDelete();
			return TRUE;
		}
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgPageLibrary::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_BUTTON_ADD, lang->GetLine(Lang::LibraryDialog, 5));
	::SetDlgItemText(thisWnd, IDC_BUTTON_DELETE, lang->GetLine(Lang::LibraryDialog, 6));
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE1, lang->GetLine(Lang::LibraryDialog, 7));
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE2, lang->GetLine(Lang::LibraryDialog, 8));

	::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE), FALSE);

	paths = (*libraryFolders);

	if (isPortableVersion)
	{
		for (std::size_t i = 0, size = paths.size(); i < size; ++i)
		{
			if (!paths[i].empty() && paths[i][0] == '?')
				paths[i][0] = programPath[0];
		}
	}

	listFolders = ::GetDlgItem(thisWnd, IDC_LIST_FOLDERS);

	// Assign styles and the theme to ListView (Vista/7 selection style like in explorer)
	ListView_SetExtendedListViewStyleEx(listFolders, 0, LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_DOUBLEBUFFER);
	::SetWindowTheme(listFolders, L"explorer", NULL);

	// Load icon
	imListFolder = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);
	if (imListFolder)
	{
		HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_FOLDER), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		::ImageList_AddIcon(imListFolder, icon);
		::DestroyIcon(icon);

		ListView_SetImageList(listFolders, imListFolder, LVSIL_SMALL);
	}

	CRect rcList;
	::GetClientRect(listFolders, rcList);

	LVCOLUMN column = {};
	column.mask = LVCF_WIDTH;
	column.cx = rcList.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	ListView_InsertColumn(listFolders, 0, &column);

	SetWindowRedraw(listFolders, FALSE);

	for (std::size_t i = 0, size = paths.size(); i < size; ++i)
	{
		// Remove slash for folders (not drives)
		std::wstring path = paths[i];
		if (path.size() > 3 && path.back() == '\\')
			path.pop_back();

		LVITEM item = {};
		item.mask = LVIF_TEXT|LVIF_IMAGE;
		item.iItem = (int)i;
		item.iSubItem = 0;
		item.iImage = 0;
		item.pszText = const_cast<wchar_t*>(path.c_str());
		ListView_InsertItem(listFolders, &item);
	}

	SetWindowRedraw(listFolders, TRUE);
}

void DlgPageLibrary::Save()
{
	if (isPortableVersion)
	{
		for (std::size_t i = 0, size = paths.size(); i < size; ++i)
		{
			if (!paths[i].empty())
			{
				std::wstring drive(1, paths[i][0]);
				drive = StringEx::ToLowerUS(drive);
				std::wstring programDrive(1, programPath[0]);
				programDrive = StringEx::ToLowerUS(programDrive);
				if (drive[0] == programDrive[0])
					paths[i][0] = '?';
			}
		}
	}

	(*libraryFolders) = paths;
}

void DlgPageLibrary::OnBnClickedButtonAdd()
{
	FileDialogEx fileDialog;
	fileDialog.SetFolderTitleXP(lang->GetLineS(Lang::LibraryDialog, 9));

	if (fileDialog.DoModalFolder(thisWnd))
	{
		LVITEM item = {};
		item.mask = LVIF_TEXT|LVIF_IMAGE;
		item.iItem = ListView_GetItemCount(listFolders);
		item.iSubItem = 0;
		item.iImage = 0;
		item.pszText = const_cast<wchar_t*>(fileDialog.GetFile().c_str());
		ListView_InsertItem(listFolders, &item);

		// Add slash for folders (FileDialog returns without slash)
		std::wstring path = fileDialog.GetFile();
		if (!path.empty() && path.back() != '\\')
			path.push_back('\\');

		paths.push_back(path);
	}
}

void DlgPageLibrary::OnBnClickedButtonDelete()
{
	int index = ListView_GetSelectionMark(listFolders);
	if (index != -1)
	{
		paths.erase(paths.begin() + index);
		ListView_DeleteItem(listFolders, index);
	}

	if (ListView_GetSelectedCount(listFolders) == 0)
		::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE), FALSE);
}

void DlgPageLibrary::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	if (nmhdr->code == LVN_ITEMCHANGED)
	{
		NMLISTVIEW* nmlv = reinterpret_cast<NMLISTVIEW*>(lParam);

		if (nmlv->uNewState & LVIS_FOCUSED)
		{
			if (!::IsWindowEnabled(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE)))
				::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE), TRUE);
		}
	}
	if (nmhdr->code == NM_CLICK)
	{
		NMITEMACTIVATE* nmia = reinterpret_cast<NMITEMACTIVATE*>(lParam);

		if (nmia->iItem == -1)
		{
			if (::IsWindowEnabled(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE)))
				::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_DELETE), FALSE);
		}
	}
}
