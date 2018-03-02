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

#include "DialogEx.h"
#include "WindowEx.h"
#include "XmlFile.h"
#include "UTF.h"
#include "ExImage.h"
#include "Language.h"
#include "MessageBox.h"
#include "ZipFile.h"


// DlgSkin dialog

class DlgSkin : public DialogEx
{

public:
	DlgSkin();
	virtual ~DlgSkin();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetProgramPath(const std::wstring& path) {programPath = path;}
	inline void SetCurrentSkin(const std::wstring& name, bool isPack) {curSkinName = name; curSkinPack = isPack;}

private:
	Language* lang = nullptr;

	std::wstring programPath;
	std::wstring curSkinName;
	bool curSkinPack = false;
	int selectedSkin = -1;

	std::wstring skinEngine;

	HWND listSkin = NULL;
	HIMAGELIST imListSkin = NULL;

	ExImage imPreview;

	std::vector<std::pair<std::wstring, int>> skinNames;

	void LoadSkinInfo(const std::wstring& folder, ZipFile* zipFile);
	void LoadPreview(const std::wstring& file, ZipFile* zipFile);


private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnNotify(WPARAM wParam, LPARAM lParam);
};
