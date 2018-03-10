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
#include "DlgSkin.h"
#include "FileSystem.h"

DlgSkin::DlgSkin()
{

}

DlgSkin::~DlgSkin()
{
	if (imListSkin)
		::ImageList_Destroy(imListSkin);
}

INT_PTR DlgSkin::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			EndDialog(LOWORD(wParam));
			return TRUE;
		}
		return TRUE;
	case WM_DRAWITEM:
		OnDrawItem((int)wParam, (LPDRAWITEMSTRUCT)lParam);
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgSkin::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::SkinDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::SkinDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::SkinDialog, 2)); 
	::SetDlgItemText(thisWnd, IDC_STATIC_SELECTSKIN, lang->GetLine(Lang::SkinDialog, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_PREVIEW2, lang->GetLine(Lang::SkinDialog, 4));
	::SetDlgItemText(thisWnd, IDC_STATIC_INFO, lang->GetLine(Lang::SkinDialog, 5));
	::SetDlgItemText(thisWnd, IDC_STATIC_AUTHOR, lang->GetLine(Lang::SkinDialog, 6));
	::SetDlgItemText(thisWnd, IDC_STATIC_VERSION, lang->GetLine(Lang::SkinDialog, 7));
	::SetDlgItemText(thisWnd, IDC_STATIC_DESC, lang->GetLine(Lang::SkinDialog, 8));

	listSkin = ::GetDlgItem(thisWnd, IDC_LIST_SKIN);

	// Assign styles and the theme to ListView (Vista/7 selection style like in explorer)
	ListView_SetExtendedListViewStyleEx(listSkin, 0, LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_DOUBLEBUFFER);
	::SetWindowTheme(listSkin, L"explorer", NULL);

	// Load icon
	imListSkin = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);
	if (imListSkin)
	{
		HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_SKIN), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		::ImageList_AddIcon(imListSkin, icon);
		::DestroyIcon(icon);
	
		ListView_SetImageList(listSkin, imListSkin, LVSIL_SMALL);
	}

	CRect rcList;
	::GetClientRect(listSkin, rcList);
	
	LVCOLUMN column = {};
	column.mask = LVCF_WIDTH;
	column.cx = rcList.Width() - ::GetSystemMetrics(SM_CXVSCROLL);
	ListView_InsertColumn(listSkin, 0, &column);

	// Search for subfolders in Skin folder
	std::wstring path = programPath;
	path += L"Skin";
	path.push_back('\\');

	FileSystem::Find find(path);

	while (find.Next())
	{
		if (find.IsDirectory())
		{
			skinNames.push_back(std::make_pair(find.GetFileName(), 0));
		}
		else
		{
			std::wstring ext = PathEx::ExtFromFile(find.GetFileName());
			if (ext == L"wzp")
			{
				std::wstring skinName = PathEx::NameFromFile(find.GetFileName());
				skinNames.push_back(std::make_pair(skinName, 1));
			}
		}
	}

	SetWindowRedraw(listSkin, FALSE);

	for (std::size_t i = 0, size = skinNames.size(); i < size; ++i)
	{
		if (curSkinName == skinNames[i].first &&
			curSkinPack == !!skinNames[i].second)
			selectedSkin = (int)i;

		LVITEM item = {};
		item.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
		item.iItem = 0;
		item.iSubItem = 0;
		item.iImage = 0;
		item.lParam = i;

		std::wstring skinName = skinNames[i].first;
		if (skinNames[i].second == 0)
			skinName.push_back('*');

		item.pszText = const_cast<wchar_t*>(skinName.c_str());

		ListView_InsertItem(listSkin, &item);
	}

	SetWindowRedraw(listSkin, TRUE);

	// Select the current skin

	LVFINDINFO fi = {};
	fi.flags = LVFI_PARAM;
	fi.lParam = (LPARAM)selectedSkin;

	int current = ListView_FindItem(listSkin, -1, &fi);
	if (current > -1)
	{
		LVITEM item = {};
		item.iItem = current;
		item.mask = LVIF_PARAM;
		ListView_GetItem(listSkin, &item);

		selectedSkin = (int)item.lParam;

		ListView_SetSelectionMark(listSkin, current);
		ListView_SetItemState(listSkin, current, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

		ListView_EnsureVisible(listSkin, current, FALSE);
	}
}

void DlgSkin::OnBnClickedCancel()
{

}

void DlgSkin::OnBnClickedOK()
{
	if (skinEngine != L"1.6")
		MessageBox::Show(thisWnd, lang->GetLine(Lang::Message, 2), lang->GetLine(Lang::Message, 5), MessageBox::Icon::Information);
	else
	{
		if (selectedSkin == -1)
			return;

		WindowEx::BeginWaitCursor();

		LRESULT result = ::SendMessage(thisParentWnd, UWM_CHANGESKIN, (WPARAM)skinNames[selectedSkin].second, (LPARAM)skinNames[selectedSkin].first.c_str());
		
		WindowEx::EndWaitCursor();
		::BringWindowToTop(thisWnd);

		if (!result)
			MessageBox::Show(thisWnd, lang->GetLine(Lang::Message, 1), lang->GetLine(Lang::Message, 6), MessageBox::Icon::Warning);
	}
}

void DlgSkin::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	if (nmhdr->code == LVN_ITEMCHANGED)
	{
		NMLISTVIEW* nmlv = reinterpret_cast<NMLISTVIEW*>(lParam);

		if (!(nmlv->uNewState & LVIS_FOCUSED))
			return;

		selectedSkin = (int)nmlv->lParam;

		if (skinNames[selectedSkin].second == 0)
			LoadSkinInfo(skinNames[selectedSkin].first, NULL);
		else
		{
			ZipFile zipFile;
			LoadSkinInfo(skinNames[selectedSkin].first, &zipFile);
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

void DlgSkin::LoadSkinInfo(const std::wstring& folder, ZipFile* zipFile)
{
	std::wstring path = programPath;
	path += L"Skin";
	path.push_back('\\');
	path += folder;

	std::wstring file;
	if (zipFile == NULL)
	{
		path.push_back('\\');
		file = path + L"SkinInfo.xml";
	}
	else
	{
		zipFile->OpenFile(path + L".wzp");
		file = L"SkinInfo.xml";
	}

	std::wstring author;
	std::wstring version;
	std::wstring info;
	std::wstring preview;

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{	
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("SkinInfo");

		if (xmlMain)
		{
			skinEngine = xmlMain.Attribute16("SkinEngine");

			XmlNode xmlAuthor = xmlMain.FirstChild("Author");
			if (xmlAuthor)
				author = xmlAuthor.Attribute16("Line");

			XmlNode xmlVersion = xmlMain.FirstChild("Version");
			if (xmlVersion)
				version = xmlVersion.Attribute16("Line");

			XmlNode xmlInfo = xmlMain.FirstChild("Info");
			if (xmlInfo)
				info = xmlInfo.Attribute16("Line");

			XmlNode xmlPreview = xmlMain.FirstChild("Preview");
			if (xmlPreview)
				preview = xmlPreview.Attribute16("File");
		}
	}

	::SetDlgItemText(thisWnd, IDC_TEXT_AUTHOR, author.c_str());
	::SetDlgItemText(thisWnd, IDC_TEXT_VERSION, version.c_str());
	::SetDlgItemText(thisWnd, IDC_TEXT_INFO, info.c_str());

	if (zipFile == NULL)
		LoadPreview(path + preview, NULL);
	else
		LoadPreview(preview, zipFile);
}

void DlgSkin::LoadPreview(const std::wstring& file, ZipFile* zipFile)
{
	HWND wndPreview = ::GetDlgItem(thisWnd, IDC_STATIC_PREVIEW);

	CRect rc;
	::GetClientRect(wndPreview, rc);

	imPreview.ThumbnailFromFile(file, zipFile, rc.Width(), rc.Height());

	if (imPreview.IsValid())
	{
		if (!::IsWindowVisible(wndPreview))
			::ShowWindow(wndPreview, SW_SHOW);
		else
			::InvalidateRect(wndPreview, NULL, TRUE);
	}
	else
	{
		if (::IsWindowVisible(wndPreview))
			::ShowWindow(wndPreview, SW_HIDE);
	}
}

void DlgSkin::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_STATIC_PREVIEW)
	{
		imPreview.Draw(lpDrawItemStruct->hDC, 0, 0);
	}
}

