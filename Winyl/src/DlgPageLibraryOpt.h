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
#include "Language.h"


// DlgPageLibraryOpt dialog

class DlgPageLibraryOpt : public DialogEx
{

public:
	DlgPageLibraryOpt();
	~DlgPageLibraryOpt();

	inline void SetLanguage(Language* language) {lang = language;}

	inline void SetRemoveMissing(bool remove) {isRemoveMissing = remove;}
	inline void SetIgnoreDeleted(bool ignore) {isIgnoreDeleted = ignore;}
	inline bool IsRemoveMissing() {return isRemoveMissing;}
	inline bool IsIgnoreDeleted() {return isIgnoreDeleted;}
	inline bool IsFindMoved() {return isFindMoved;}
	inline bool IsRescanAll() {return isRescanAll;}

	void Save();

private:
	Language* lang = nullptr;

	bool isRemoveMissing = false;
	bool isIgnoreDeleted = false;

	bool isFindMoved = false;
	bool isRescanAll = false;

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
};

