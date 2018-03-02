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

// SkinEdit

#include "WindowEx.h"
#include "XmlFile.h"
#include "UTF.h"
#include "ExImage.h"

class SkinEdit : public WindowEx
{

public:
	SkinEdit();
	virtual ~SkinEdit();

	bool NewWindow(HWND parent);
	bool LoadSkin(const std::wstring& file, ZipFile* zipFile);

	const std::wstring& GetSearchText() {return searchText;}
	void SetStandartText(const std::wstring& text);

	bool IsSearchEmpty();
	void SearchClear();

	bool IsFocus();
	void SetFocus();

	void SetFocusWnd(HWND wnd) {wndFocus = wnd;}

private:
	HWND edit = NULL;
	HFONT font = NULL;
	HBRUSH brushBack = NULL;
	COLORREF colorBack = 0x00FFFFFF;

	COLORREF colorText = 0x00000000;
	COLORREF colorText2 = 0x00000000;

	COLORREF colorCurrent = 0x00000000;

	int editHeight = 0;

	bool isEditChange = false;

	std::wstring standartText;
	std::wstring searchText;

	void OnEnChangeEdit();
	void OnEnSetFocusEdit();
	void OnEnKillFocusEdit();

	std::wstring HelperGetWindowText(HWND wnd)
	{
		std::wstring result;
		int size = ::GetWindowTextLength(wnd);
		if (size > 0)
		{
			result.resize(size);
			::GetWindowText(wnd, &result[0], size + 1);
		}
		return result;
	}

	HWND wndFocus = NULL;

	// Subclass control
	static SkinEdit* skinEdit;
	static WNDPROC mainDlgProc;
	static LRESULT CALLBACK EditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	HBRUSH OnCtlColor(HDC hDC, HWND hWnd);
	void OnSize(UINT nType, int cx, int cy);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnCommand(WPARAM wParam, LPARAM lParam);
};
