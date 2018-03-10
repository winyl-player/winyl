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

class DlgAbout : public DialogEx
{

public:
	DlgAbout();
	virtual ~DlgAbout();

	inline void SetLanguage(Language* language) {lang = language;}

private:
	Language* lang = nullptr;

	HWND wndSite = NULL;
	HWND wndMail = NULL;
	HWND wndTr = NULL;

	HWND CreateLink(int idStatic, const std::wstring& text, const std::wstring& link);

	//void HelperScreenToClient(HWND wnd, LPRECT rect)
	//{
	//	::ScreenToClient(wnd, (LPPOINT)rect);
	//	::ScreenToClient(wnd, ((LPPOINT)rect)+1);
	//	if (::GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
	//		CRect::SwapLeftRight(rect);
	//};

	//void HelperClientToScreen(HWND wnd, LPRECT rect)
	//{
	//	::ClientToScreen(wnd, (LPPOINT)rect);
	//	::ClientToScreen(wnd, ((LPPOINT)rect)+1);
	//	if (::GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL)
	//		CRect::SwapLeftRight(rect);
	//};

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnNotify(WPARAM wParam, LPARAM lParam);
};
