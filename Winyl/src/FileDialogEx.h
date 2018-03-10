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

#pragma once

#include <string>
#include <vector>

class FileDialogEx
{

public:
	FileDialogEx();
	virtual ~FileDialogEx();

	typedef struct _FILE_TYPES
    {
		LPCWSTR pszName;
		LPCWSTR pszSpec;
    } FILE_TYPES;

	bool DoModalFile(HWND wnd, bool isSave = false, bool isMulti = false);
	bool DoModalFolder(HWND wnd, bool isMulti = false);
	void SetFileTypes(FILE_TYPES* types, int count);

	inline void SetFolderTitleXP(const std::wstring& title) {folderTitleXP = title;}

	inline const std::wstring& GetFile() {return outFile;}
	inline int GetMultiCount() {return (int)outFiles.size();}
	inline const std::wstring& GetMultiFile(int iIndex) {return outFiles[iIndex];}

private:
	std::vector<std::wstring> outFiles;
	std::wstring outFile;
	FILE_TYPES* extTypes = nullptr;
	int countTypes = 0;

	std::wstring folderTitleXP;

	bool DoModalFileXP(HWND wnd, bool isSave, bool isMulti);
	bool DoModalFileFolderVista(HWND wnd, bool isSave, bool isFolder, bool isMulti);
	bool DoModalFolderXP(HWND wnd);
	bool IsWindowsXP();

	static UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static int CALLBACK BrowseCallbackProc(HWND hDlg, UINT message, LPARAM lParam, LPARAM lpData);
	static WNDPROC oldFolderProc;
	static LRESULT CALLBACK FolderProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static void CenterDialog(HWND hDlg, HWND hParentWnd);

};


