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
#include "Settings.h"
#include "LibAudio.h"
#include "Radio.h"
#include "MessageBox.h"

class DlgPageSystem : public DialogEx
{

public:
	DlgPageSystem();
	virtual ~DlgPageSystem();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetSettings(Settings* set) {settings = set;}
	inline void SetLibAudio(LibAudio* bass) {libAudio = bass;}
	inline void SetMainWnd(HWND wnd) {wndMain = wnd;}
	inline void SetRadio(Radio* ptr) {radio = ptr;}

	void SaveSettings();
	void CancelSettings();

private:
	Language* lang = nullptr;
	Settings* settings = nullptr;
	LibAudio* libAudio = nullptr;
	Radio* radio = nullptr;
	HWND wndMain = NULL;
	HWND comboDevice = NULL;
	int oldDriver = 0;

	void AddDirectSoundDevices();
	void AddWasapiDevices();
	void AddAsioDevices();
	void EnableProxy(bool isEnable);
	void EnableProxyAuth(bool isEnable);

	std::wstring HelperGetDlgItemText(HWND wnd, int id)
	{
		std::wstring result;
		HWND control = ::GetDlgItem(wnd, id);
		if (control)
		{
			int size = ::GetWindowTextLength(control);
			if (size > 0)
			{
				result.resize(size);
				::GetWindowText(control, &result[0], size + 1);
			}
		}
		return result;
	}

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedRadioDirectSound();
	void OnBnClickedRadioWasapi();
	void OnBnClickedRadioAsio();
	void OnBnClickedButtonAsioConfig();
	void OnBnClickedCheckProxy();
	void OnBnClickedCheckProxyAuth();
};
