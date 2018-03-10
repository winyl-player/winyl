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
#include "FileDialogEx.h"

FileDialogEx::FileDialogEx()
{

}

FileDialogEx::~FileDialogEx()
{

}

bool FileDialogEx::DoModalFile(HWND wnd, bool isSave, bool isMulti)
{
	outFile.clear();
	outFiles.clear();

	if (IsWindowsXP())
		return DoModalFileXP(wnd, isSave, isMulti);
	else
		return DoModalFileFolderVista(wnd, isSave, false, isMulti);
}

bool FileDialogEx::DoModalFolder(HWND wnd, bool isMulti)
{
	outFile.clear();
	outFiles.clear();

	if (IsWindowsXP())
		return DoModalFolderXP(wnd);
	else
		return DoModalFileFolderVista(wnd, false, true, isMulti);
}

void FileDialogEx::SetFileTypes(FILE_TYPES* types, int count)
{
	extTypes = types;
	countTypes = count;
}

bool FileDialogEx::DoModalFileXP(HWND wnd, bool isSave, bool isMulti)
{
	wchar_t myDocuments[MAX_PATH];
	myDocuments[0] = '\0';
	::SHGetSpecialFolderPath(NULL, myDocuments, CSIDL_MYDOCUMENTS, FALSE);

	wchar_t fileName[4096]; // 2048
	fileName[0] = '\0';

	OPENFILENAME ofn = {};
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = fileName;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(fileName) / sizeof(wchar_t);

	ofn.hwndOwner = wnd;
	ofn.lpstrInitialDir = myDocuments;
	ofn.hInstance = ::GetModuleHandle(NULL);
	ofn.lpfnHook = OFNHookProc;
	ofn.Flags = OFN_ENABLEHOOK|OFN_EXPLORER|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
	if (isMulti)
		ofn.Flags |= OFN_ALLOWMULTISELECT;

	// Fill filter
	size_t bufSize = sizeof(wchar_t);

	for (int i = 0; i < countTypes; ++i)
	{
		bufSize += wcslen(extTypes[i].pszName) * sizeof(wchar_t) + sizeof(wchar_t);
		bufSize += wcslen(extTypes[i].pszSpec) * sizeof(wchar_t) + sizeof(wchar_t);
	}

	char* bufFilter = new char[bufSize];
	char* bufCursor = bufFilter;

	for (int i = 0; i < countTypes; ++i)
	{
		size_t sizeName = wcslen(extTypes[i].pszName) * sizeof(wchar_t) + sizeof(wchar_t);
		size_t sizeSpec = wcslen(extTypes[i].pszSpec) * sizeof(wchar_t) + sizeof(wchar_t);

		//memcpy(bufCursor, extTypes[i].pszName, sizeName);
		memcpy_s(bufCursor, sizeName, extTypes[i].pszName, sizeName);
		bufCursor += sizeName;

		//memcpy(bufCursor, extTypes[i].pszSpec, sizeSpec);
		memcpy_s(bufCursor, sizeSpec, extTypes[i].pszSpec, sizeSpec);
		bufCursor += sizeSpec;
	}
	memset(bufCursor, 0, sizeof(wchar_t));

	ofn.lpstrFilter = (wchar_t*)bufFilter;
	// End fill filter

	// Show dialog
	BOOL result = FALSE;
	if (!isSave)
		result = ::GetOpenFileName(&ofn);
	else
		result = ::GetSaveFileName(&ofn);

	// Free filter
	delete[] bufFilter;
	bufFilter = nullptr;


	// Parse result
	if (result)
	{
		if (!isMulti)
		{
			outFile = ofn.lpstrFile;

			if (!outFile.empty())
				return true;
		}
		else
		{
			std::wstring path = ofn.lpstrFile;

			bufCursor = (char*)ofn.lpstrFile + (path.size() * sizeof(wchar_t));
			bufCursor += sizeof(wchar_t);

			if (*bufCursor == '\0') // Only one file selected
			{
				outFiles.push_back(path);
			}
			else // Multiple files selected
			{
				if (path.size() > 0 && path[path.size() - 1] != '\\')
					path.push_back('\\');

				std::wstring file = (wchar_t*)bufCursor;
				for (;;)
				{
					outFiles.push_back(path + file);

					bufCursor += (file.size() * sizeof(wchar_t)) + sizeof(wchar_t);

					if (*bufCursor != '\0')
						file = (wchar_t*)bufCursor;
					else
						break;
				}
			}

			if (!outFiles.empty())
				return true;
		}
	}

	return false;
}

bool FileDialogEx::DoModalFileFolderVista(HWND wnd, bool isSave, bool isFolder, bool isMulti)
{
	if (!isSave)
	{
		IFileOpenDialog* dlg = nullptr;
		HRESULT hr = ::CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg));
		//HRESULT hr = ::CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg));
 
		if (FAILED(hr))
			return false;

		if (!isFolder)
			dlg->SetFileTypes(countTypes, (COMDLG_FILTERSPEC*)extTypes);
		else
		{
			DWORD fos = 0;
			dlg->GetOptions(&fos);
			dlg->SetOptions(fos|FOS_PICKFOLDERS);
//			dlg->SetTitle(L"A Single-Selection Dialog");
		}

		if (!isMulti)
		{
			hr = dlg->Show(wnd);

			if (SUCCEEDED(hr))
			{
				IShellItem* item = nullptr;
				hr = dlg->GetResult(&item);
 
				if (SUCCEEDED(hr))
				{
					LPOLESTR pwsz = nullptr;
					hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
 
					if (SUCCEEDED(hr))
					{
						outFile = pwsz;
						CoTaskMemFree(pwsz);
					}

					item->Release();
					dlg->Release();

					if (!outFile.empty())
						return true;

					return false;
				}
			}
		}
		else
		{
			DWORD fos = 0;
			dlg->GetOptions(&fos);
			dlg->SetOptions(fos|FOS_ALLOWMULTISELECT);

			hr = dlg->Show(wnd);

			if (SUCCEEDED(hr))
			{
				IShellItemArray* items = nullptr;
				hr = dlg->GetResults(&items);

				if (SUCCEEDED(hr))
				{
					DWORD countItems = 0;
					hr = items->GetCount(&countItems);

					if (SUCCEEDED(hr))
					{
						for (DWORD i = 0; i < countItems; ++i)
						{
							IShellItem* item = nullptr;
							hr = items->GetItemAt(i, &item);
 
							if (SUCCEEDED(hr))
							{
								LPOLESTR pwsz = nullptr;
								hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
 
								if (SUCCEEDED(hr))
								{
									outFiles.push_back(pwsz);
									CoTaskMemFree(pwsz);
								}

								item->Release();
							}
						}

						items->Release();
						dlg->Release();

						if (!outFiles.empty())
							return true;

						return false;
					}
				}
			}
		}

		dlg->Release();
	}
	else
	{
		IFileSaveDialog* dlg = nullptr;
		HRESULT hr = ::CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg));
		//HRESULT hr = ::CoCreateInstance(__uuidof(FileSaveDialog), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&dlg));
 
		if (FAILED(hr))
			return false;

		dlg->SetFileTypes(countTypes, (COMDLG_FILTERSPEC*)extTypes);

		hr = dlg->Show(wnd);

		if (SUCCEEDED(hr))
		{
			IShellItem* item = nullptr;
			hr = dlg->GetResult(&item);
 
			if (SUCCEEDED(hr))
			{
				LPOLESTR pwsz = nullptr;
				hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pwsz);
 
				if (SUCCEEDED(hr))
				{
					outFile = pwsz;
					CoTaskMemFree(pwsz);
				}

				item->Release();
				dlg->Release();

				if (!outFile.empty())
					return true;

				return false;
			}
		}

		dlg->Release();
	}

	return false;
}

bool FileDialogEx::DoModalFolderXP(HWND wnd)
{
	BROWSEINFO bi = {};

	wchar_t dispname[MAX_PATH];
	dispname[0] = '\0';
	bi.hwndOwner = wnd;
	bi.lpszTitle = folderTitleXP.c_str();
	bi.pszDisplayName = dispname;
	bi.ulFlags = BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE|BIF_NONEWFOLDERBUTTON;
	bi.lpfn = BrowseCallbackProc;

	LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

	if (pidl) // Get path from ID
	{
		dispname[0] = '\0';
		::SHGetPathFromIDListW(pidl, dispname);
		::CoTaskMemFree(pidl);//::ILFree(pidl);
		outFile = dispname;

		if (!outFile.empty())
			return true;
	}

	return false;
}

bool FileDialogEx::IsWindowsXP()
{
	OSVERSIONINFO osvi = {};
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osvi);

	if (osvi.dwMajorVersion < 6)
		return true;

	return false;
}

UINT_PTR FileDialogEx::OFNHookProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NOTIFY)
	{
		OFNOTIFY* notify = (OFNOTIFY*)lParam;
		switch(notify->hdr.code)
		{
		case CDN_INITDONE:
			HWND dlg;
			dlg = ::GetParent(hDlg);
			CenterDialog(dlg, ::GetParent(dlg));
			return (UINT_PTR)TRUE;
		case CDN_SELCHANGE:
			return (UINT_PTR)TRUE;
		case CDN_FOLDERCHANGE:
			return (UINT_PTR)TRUE;
		case CDN_SHAREVIOLATION:
			return (UINT_PTR)TRUE;
		case CDN_HELP:
			return (UINT_PTR)TRUE;
		case CDN_FILEOK:
			return (UINT_PTR)TRUE;
		case CDN_TYPECHANGE:
			return (UINT_PTR)TRUE;
		}
	}

	return (UINT_PTR)FALSE;
}

WNDPROC FileDialogEx::oldFolderProc = nullptr;

int FileDialogEx::BrowseCallbackProc(HWND hDlg, UINT message, LPARAM lParam, LPARAM lpData)
{
	switch (message)
	{
	case BFFM_INITIALIZED:
		oldFolderProc = (WNDPROC)(LONG_PTR)::GetWindowLongPtr(hDlg, GWLP_WNDPROC);
		::SetWindowLongPtr(hDlg, GWLP_WNDPROC, (LONG_PTR)&FolderProc);
		break;
	}

	return 0;
}

LRESULT FileDialogEx::FolderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  
{	
	if (message == WM_SHOWWINDOW && wParam == TRUE)
	{
		CenterDialog(hWnd, ::GetParent(hWnd));
	}

	return oldFolderProc(hWnd, message, wParam, lParam);  
}

void FileDialogEx::CenterDialog(HWND hDlg, HWND hParentWnd)
{
	if (hParentWnd == NULL)
		return;

	RECT rcDlg, rcWnd, rcArea;

	::GetWindowRect(hDlg, &rcDlg);

	MONITORINFO mi;
	mi.cbSize = sizeof(mi);
	::GetMonitorInfo(::MonitorFromWindow(hParentWnd, MONITOR_DEFAULTTONEAREST), &mi);
	rcArea = mi.rcWork;

	if (!::IsIconic(hParentWnd) && ::IsWindowVisible(hParentWnd))
		::GetWindowRect(hParentWnd, &rcWnd);
	else
		rcWnd = rcArea;

	int left = (rcWnd.left + rcWnd.right) / 2 - (rcDlg.right - rcDlg.left) / 2;
	int top = (rcWnd.top + rcWnd.bottom) / 2 - (rcDlg.bottom - rcDlg.top) / 2;

	if (left < rcArea.left)
		left = rcArea.left;
	else if (left + (rcDlg.right - rcDlg.left) > rcArea.right)
		left = rcArea.right - (rcDlg.right - rcDlg.left);

	if (top < rcArea.top)
		top = rcArea.top;
	else if (top + (rcDlg.bottom-rcDlg.top) > rcArea.bottom)
		top = rcArea.bottom - (rcDlg.bottom - rcDlg.top);

	::SetWindowPos(hDlg, NULL, left, top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}
