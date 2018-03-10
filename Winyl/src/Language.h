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

#include "XmlFile.h"
#include "UTF.h"

enum class Lang
{
	MainMenu         = 0,
	TrayMenu         = 1,
	ListMenu         = 2,
	TreeMenu         = 3,
	Library          = 4,
	SkinDialog       = 5,
	LibraryDialog    = 6,
	LanguageDialog   = 7,
	ProcessDialog    = 8,
	PropertiesDialog = 9,
	RenameDialog     = 10,
	AboutDialog      = 11,
	Message          = 12,
	DonateDialog     = 13,
	EqualizerDialog  = 14,
	Playlist         = 15,
	HotkeyDialog     = 16,
	OpenURLDialog    = 17,
	PopupMenu        = 18,
	Tooltips         = 19,
	ConfigDialog     = 20,
	GeneralPage      = 21,
	SystemPage       = 22,
	PopupPage        = 23,
	Search           = 24,
	MiniPage         = 25,
	DefaultList      = 26,
	DialogSmart      = 27,
	SmartFilter      = 28,
	StatusLine       = 29,
	TagsPage         = 30,
	CoverPage        = 31,
	LyricsPage       = 32,
	Lyrics           = 33,
	LyricsMenu       = 34,
	NewVersionDialog = 35,
	EnumCount // Count of all items
};

class Language
{

public:
	Language();
	virtual ~Language();

	void SetProgramPath(const std::wstring& path) {programPath = path;}
	bool LoadLanguage(const std::wstring& language);
	bool ReloadLanguage(const std::wstring& language);
	const wchar_t* GetLine(Lang type, int index);
	const std::wstring& GetLineS(Lang type, int index);
	inline const std::wstring& GetLanguage() {return currentLanguage;}

	std::wstring GetSystemLanguage();

private:
	std::wstring programPath;
	std::wstring currentLanguage;
	std::vector<std::wstring> lines[(std::size_t)Lang::EnumCount];
};


