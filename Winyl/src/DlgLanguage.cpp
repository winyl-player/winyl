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

// DlgLanguage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgLanguage.h"
#include "FileSystem.h"


// DlgLanguage dialog

DlgLanguage::DlgLanguage()
{

}

DlgLanguage::~DlgLanguage()
{
	if (imListLanguage)
		::ImageList_Destroy(imListLanguage);
}

INT_PTR DlgLanguage::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgLanguage::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::LanguageDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::LanguageDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::LanguageDialog, 2)); 
	::SetDlgItemText(thisWnd, IDC_STATIC_SELECT, lang->GetLine(Lang::LanguageDialog, 3));

	listLanguage = ::GetDlgItem(thisWnd, IDC_LIST_LANGUAGE);

	// Assign styles and the theme to ListView (Vista/7 selection style like in explorer)
	ListView_SetExtendedListViewStyleEx(listLanguage, 0, LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_DOUBLEBUFFER);
	::SetWindowTheme(listLanguage, L"explorer", NULL);

	//imListLanguage = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);	
	//ListView_SetImageList(listLanguage, imListLanguage, LVSIL_SMALL);

	// Load icon
	imListLanguage = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);
	if (imListLanguage)
	{
		HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_LANG), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		ImageList_AddIcon(imListLanguage, icon);
		::DestroyIcon(icon);

		ListView_SetImageList(listLanguage, imListLanguage, LVSIL_SMALL);
	}

	CRect rcList;
	::GetClientRect(listLanguage, rcList);
	
	LVCOLUMN column = {};
	column.mask = LVCF_WIDTH;
	column.cx = rcList.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	ListView_InsertColumn(listLanguage, 0, &column);


	// Search for subfolders in Language folder
	std::wstring path = programPath;
	path += L"Language";
	path.push_back('\\');

	FileSystem::Find find(path);

	SetWindowRedraw(listLanguage, FALSE);

	while (find.Next())
	{
		if (find.IsDirectory())
		{
			std::wstring fileName = find.GetFileName();

			/*// Compose a flag icon path
			std::wstring fileIcon = programPath;
			fileIcon += L"Language";
			fileIcon.push_back('\\');
			fileIcon += fileName;
			fileIcon.push_back('\\');
			fileIcon += L"Flag.ico";

			HICON icon = (HICON)::LoadImage(::GetModuleHandle(NULL), fileIcon.c_str(), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR|LR_LOADFROMFILE);
			::ImageList_AddIcon(imListLanguage, icon);
			::DestroyIcon(icon);*/

			LVITEM item = {};
			item.mask = LVIF_TEXT|LVIF_IMAGE;
			item.iItem = 0;
			item.iSubItem = 0;
			//item.iImage = ImageList_GetImageCount(imListLanguage) - 1;
			item.iImage = 0;
			item.pszText = const_cast<wchar_t*>(fileName.c_str());
			ListView_InsertItem(listLanguage, &item);
		}
	}

	// Alternative for column.cx = rcList.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	//ListView_SetColumnWidth(listLanguage, 0, LVSCW_AUTOSIZE_USEHEADER);

	SetWindowRedraw(listLanguage, TRUE);



/*
	/////// Test for button with icon (works properly only on Vista or later)
	LONG_PTR style = ::GetWindowLongPtr(::GetDlgItem(thisWnd, IDOK), GWL_STYLE);
	::SetWindowLongPtr(::GetDlgItem(thisWnd, IDOK), GWL_STYLE, style | BS_ICON); // On WinXP: Old style Button with icon (can add BS_FLAT for better look on XP)
	//::SetWindowLongPtr(::GetDlgItem(thisWnd, IDOK), GWL_STYLE, style | BS_BITMAP); // On WinXP: Default style button without icon
	// LoadIcon doesn't work properly
	//HICON icon = ::LoadIconW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_TRAY_PLAY));
	HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_TRAY_PLAY), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	// Test for button with icon only (on WinXP behave depending on the style above)
	::SendMessageW(::GetDlgItem(thisWnd, IDOK), BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
	// Test for button with icon and text (does nothing on WinXP)
	::SendMessageW(::GetDlgItem(thisWnd, IDCANCEL), BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
	::DestroyIcon(icon); // Don't forget to destroy icon
*/
/*
	/////// Another test for button with icon (works on XP or later, but for icon with text we need manually calculate margins)
	::SetDlgItemTextW(thisWnd, IDOK, L""); // Remove text for testing
	// Test for 1 image for all button states
	HIMAGELIST imageList = ::ImageList_Create(16, 16, ILC_COLOR32, 1, 0);
	HICON icon = (HICON)::LoadImageW(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TRAY_PLAY), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	::ImageList_AddIcon(imageList, icon);
	::DestroyIcon(icon);
	// Test for 6 images for all button states
	// PBS_NORMAL = 1,
	// PBS_HOT = 2,
	// PBS_PRESSED = 3,
	// PBS_DISABLED = 4,
	// PBS_DEFAULTED = 5,
	// PBS_STYLUSHOT = 6, You must specify the bitmap for PBS_STYLUSHOT index, otherwise the bitmap will be invisible half of the time when the button is in focused state.
	//HIMAGELIST imageList = ::ImageList_Create(16, 16, ILC_COLOR32, 6, 0);
	//HICON icon = (HICON)::LoadImageW(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TRAY_PLAY), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//::ImageList_AddIcon(imageList, icon);
	//::DestroyIcon(icon);
	//icon = (HICON)::LoadImageW(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_TRAY_PAUSE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	//::ImageList_AddIcon(imageList, icon);
	//::ImageList_AddIcon(imageList, icon);
	//::ImageList_AddIcon(imageList, icon);
	//::ImageList_AddIcon(imageList, icon);
	//::ImageList_AddIcon(imageList, icon);
	//::DestroyIcon(icon);
	BUTTON_IMAGELIST buttonImageList;
	buttonImageList.himl = imageList;
	buttonImageList.margin = {0, 0, 0, 0};
	buttonImageList.uAlign = BUTTON_IMAGELIST_ALIGN_CENTER;
	// Test for button with icon and text "OK"
	//::SetDlgItemTextW(thisWnd, IDOK, L"OK");
	//buttonImageList.margin.left = 18;
	//buttonImageList.margin.right = -22;
	// OR 
	//buttonImageList.margin.left = 0;
	//buttonImageList.margin.right = -16;
	//buttonImageList.uAlign = BUTTON_IMAGELIST_ALIGN_LEFT;
	Button_SetImageList(::GetDlgItem(thisWnd, IDOK), &buttonImageList);
	//::ImageList_Destroy(imageList); // need to destroy ImageList on WM_DESTROY message
*/
}

void DlgLanguage::OnBnClickedOK()
{
	int index = ListView_GetSelectionMark(listLanguage);
	if (index != -1)
	{
		WCHAR text[MAX_PATH] = {};
		ListView_GetItemText(listLanguage, index, 0, text, MAX_PATH);
		languageName = text;
	}
}

void DlgLanguage::OnBnClickedCancel()
{

}

void DlgLanguage::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	if (nmhdr->code == LVN_ITEMCHANGED)
	{
		NMLISTVIEW* nmlv = reinterpret_cast<NMLISTVIEW*>(lParam);

		if (nmlv->uNewState & LVIS_FOCUSED)
		{
			if (!::IsWindowEnabled(::GetDlgItem(thisWnd, IDOK)))
				::EnableWindow(::GetDlgItem(thisWnd, IDOK), TRUE);
		}
	}
	else if (nmhdr->code == NM_CLICK)
	{
		NMITEMACTIVATE* nmia = reinterpret_cast<NMITEMACTIVATE*>(lParam);

		if (nmia->iItem == -1)
		{
			if (::IsWindowEnabled(::GetDlgItem(thisWnd, IDOK)))
				::EnableWindow(::GetDlgItem(thisWnd, IDOK), FALSE);
		}
	}
	else if (nmhdr->code == NM_DBLCLK)
	{
		NMITEMACTIVATE* nmia = reinterpret_cast<NMITEMACTIVATE*>(lParam);

		if (nmia->iItem != -1)
		{
			OnBnClickedOK();
			EndDialog(IDOK);
		}
	}
}

