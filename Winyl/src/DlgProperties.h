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
#include "DBase.h"
#include "Properties.h"
#include "DlgPageTags.h"
#include "DlgPageCover.h"
#include "DlgPageLyrics.h"

class DlgProperties : public DialogEx
{

public:
	DlgProperties();
	virtual ~DlgProperties();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetOpenLyrics(const std::wstring& lyrics) {isOpenLyrics = true; pageLyrics.SetOpenLyrics(lyrics);}

	inline void SetCallbackChanged(const std::function<void(Properties *properties)>& callback) {callbackChanged = callback;}

	inline void SetDataBase(DBase* ptr) {dBase = ptr;}
	inline void SetLibAudio(LibAudio* ptr) {libAudio = ptr;}
	inline void SetSkinList(SkinList* ptr, ListNodeUnsafe node = nullptr) {skinList = ptr;}
	inline void SetSkinListNode(ListNodeUnsafe node) {listNode = node;}

	std::unique_ptr<Properties> properties;

	DlgPageTags pageTags;
	DlgPageCover pageCover;
	DlgPageLyrics pageLyrics;

private:
	DBase* dBase = nullptr;
	LibAudio* libAudio = nullptr;
	SkinList* skinList = nullptr;
	ListNodeUnsafe listNode = nullptr;

	std::function<void(Properties *properties)> callbackChanged;

	void HidePages();
	void SelectPage(int page);

	bool SaveTags(bool close);

	bool isOpenLyrics = false;

	bool isRadioOpen = false;

	Language* lang = nullptr;

	HWND tabProperties = NULL;
	HWND progressControl = NULL;

	void CreateNextPrevToolbar();

	HWND toolBar = NULL;
	HWND toolTip = NULL;
	HIMAGELIST toolImageList = NULL;

	void ThreadEndFunc();
	void ProgressFunc(int pos);

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
	void OnBnClickedButtonSave();
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnDropFiles(HDROP hDropInfo);
	void OnCommand(WPARAM wParam, LPARAM lParam);
};
