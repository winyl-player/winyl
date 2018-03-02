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
#include "DlgPageLibrary.h"
#include "DlgPageLibraryOpt.h"

// DlgLibrary dialog

class DlgLibrary : public DialogEx
{

public:
	DlgLibrary();
	virtual ~DlgLibrary();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetFirstRun(bool first) {isFirstRun = first;}

	DlgPageLibrary pageLibrary;
	DlgPageLibraryOpt pageLibraryOpt;

private:
	void SelectPage(int page);

	Language* lang = nullptr;

	bool isFirstRun = false;

	HWND tabLibrary = NULL;

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnNotify(WPARAM wParam, LPARAM lParam);
};
