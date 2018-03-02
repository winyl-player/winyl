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
#include "Threading.h"
#include "DBase.h"
#include "XmlFile.h"

// DlgNewVersion dialog

class DlgNewVersion : public DialogEx
{

public:
	DlgNewVersion();
	virtual ~DlgNewVersion();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}
	inline void SetProgramPath(const std::wstring& path) {programPath = path;}
	inline void SetConvertPortable(bool portableTo, bool portableFrom) {convertPortableTo = portableTo; convertPortableFrom = portableFrom;}

private:
	Language* lang = nullptr;

	Threading::Thread threadWorker;
	std::atomic<bool> isThreadDone = false;

	std::wstring profilePath;
	std::wstring programPath;
	bool convertPortableTo = false;
	bool convertPortableFrom = false;

	void RunThread();
	bool ThreadUpdate();
	bool IsLibraryOld();
	bool ConvertLibrary();
	bool ConvertLibraryDB(const DBase::SQLFile& db);
	bool ConvertPlaylist(const std::wstring& file);
	bool ConvertPlaylistDB(const DBase::SQLFile &db, const std::wstring& file);
	bool ConvertPlaylists();
	bool ConvertSmartlists();
	bool ConvertHotKeysXML();
	void CreateTableLibrary(const DBase::SQLFile& db);
	void CreateTablePlaylist(const DBase::SQLFile& db);
	void CreateTableSmartlist(const DBase::SQLFile& db);

	void ConvertPortable();
	void ConvertPortableLibraryXML(bool toPortable, const std::string& drive);
	void ConvertPortableLibraryDB(bool toPortable, const std::string& drive);
	void ConvertPortablePlaylists(bool toPortable, const std::string& drive);
	void ConvertPortableSmartlists(bool toPortable, const std::string& drive);
	void ConvertPortableDB(bool toPortable, const std::string& drive, DBase::SQLRequest& sqlSelect, DBase::SQLRequest& sqlUpdate);

private: // Messages
	INT_PTR DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnInitDialog();
	void OnBnClickedOK();
	void OnBnClickedCancel();
};

