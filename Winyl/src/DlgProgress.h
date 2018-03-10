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
#include "Language.h"
#include "Progress.h"

class DlgProgress : public DialogEx
{

public:
	DlgProgress();
	virtual ~DlgProgress();

	inline void SetLanguage(Language* pointer) {lang = pointer;}
	inline void SetMessageWnd(HWND wnd) {wndMessage = wnd;}
	inline void SetTaskbarMessage(UINT message) {wmTaskbarButtonCreated = message;}

	void DestroyOnStop();

	Progress progress;

private:
	HWND progressControl = NULL;
	AutoCom<ITaskbarList3> taskListControl;

	Language* lang = nullptr;
	HWND wndMessage = NULL;

	bool isDestroyOnStop = false;

	UINT wmTaskbarButtonCreated = 0;
	bool isMarqueeTaskbarButton = false;

	void Finish();
	
	void UpdateProgressMarquee(bool isMarquee);
	void UpdateProgressRange(int pos, int total);
	void UpdateProgressPos(int pos, int total);
	void UpdateProgressText(const std::wstring& text);
	void UpdateProgressTextEmpty(bool cancel);

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnBnClickedButtonMinimize();
	void OnTaskbarButtonCreated();
};
