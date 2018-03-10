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
#include "HotKeys.h"
#include "Language.h"

class DlgHotKeys : public DialogEx
{

public:
	DlgHotKeys();
	virtual ~DlgHotKeys();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetHotKeys(HotKeys* keys) {hotKeys = keys;}

	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
private:
	Language* lang = nullptr;
	HotKeys* hotKeys = nullptr;

	HWND listKeys = NULL;
	HIMAGELIST imListKeys = NULL;

	std::wstring ctrlString = L"Ctrl + ";
	std::wstring altString = L"Alt + ";
	std::wstring winString = L"Win + ";
	std::wstring shiftString = L"Shift + ";

	std::vector<HotKeys::structKey> keys;
	std::vector<int> keysState;

	std::wstring NameFromVKey(UINT nVK);
	void AddKeyToList(const std::wstring& typeString, HotKeys::KeyType type);
	void DeleteDuplicated(int index);

	// For control subclassing
	static DlgHotKeys* dlgHotKeys;
	static WNDPROC mainDlgProc;
	static LRESULT CALLBACK DlgKeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
};
