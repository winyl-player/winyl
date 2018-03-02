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
#include "Threading.h"
#include "UTF.h"
#include "Language.h"
#include "DBase.h"
#include "LibAudio.h"
#include "PlsFile.h"
#include "FileSystem.h"
#include "TagLibReader.h"
#include "CueFile.h"

class Progress
{

public:
	Progress();
	virtual ~Progress();

	void Init();
	void Cancel();
	void WaitForJoin();

	bool IsAddToPlaylist() {return isPlaylist || isNewPlaylist;}

	void SetFuncFinish(const std::function<void(void)>& func) {funcFinish = func;}

	void SetFuncUpdateProgressMarquee(const std::function<void(bool)>& func) {funcUpdateProgressMarquee = func;}
	void SetFuncUpdateProgressRange(const std::function<void(int, int)>& func) {funcUpdateProgressRange = func;}
	void SetFuncUpdateProgressPos(const std::function<void(int, int)>& func) {funcUpdateProgressPos = func;}
	void SetFuncUpdateProgressText(const std::function<void(const std::wstring&)>& func) {funcUpdateProgressText = func;}
	void SetFuncUpdateProgressTextEmpty(const std::function<void(bool)>& func) {funcUpdateProgressTextEmpty = func;}


	inline void SetDataBase(DBase* pointer) {dBase = pointer;}

	void SetLibraryFolders(const std::vector<std::wstring>& folders) {libraryFolders = folders;}

	inline void SetPlaylist(int start, const std::wstring& name) {isPlaylist = true; numberStart = start; numberOffset = start; namePlaylist = name;}
	inline void SetNewPlaylist(const std::wstring& file, const std::wstring& dbase) {isNewPlaylist = true; filePlaylist = file; fileDatabase = dbase;}
	inline void SetIgnoreDeleted(bool ignore) {isIgnoreDeleted = ignore;}
	inline void SetRemoveMissing(bool remove) {isRemoveMissing = remove;}
	inline void SetAddAllToLibrary(bool enable) {isAddAllToLibrary = enable;}
	inline void SetFindMoved(bool enable) {isFindMoved = enable;}
	inline void SetRescanAll(bool enable) {isRescanAll = enable;}

	bool FastAddFileToPlaylist(const std::wstring& musicfile, int start, bool& isFolder);

	inline void SetPortableVersion(bool isPortable) {isPortableVersion = isPortable;}
	inline void SetProgramPath(const std::wstring& path) {programPath = path;}

private:
	bool isPortableVersion = false;
	std::wstring programPath;
	std::wstring programDrive;

	DBase* dBase = nullptr;

	bool isPlaylist = false;
	bool isNewPlaylist = false;
	bool isAddAllToLibrary = false;

	bool isIgnoreDeleted = true;
	bool isRemoveMissing = true;

	bool isFindMoved = false;
	bool isRescanAll = false;

	std::vector<std::wstring> libraryFolders;

	std::wstring filePlaylist;
	std::wstring fileDatabase;

	std::wstring namePlaylist;

	int progressPos = 0;

	std::function<void(void)> funcFinish;

	std::function<void(bool)> funcUpdateProgressMarquee;
	std::function<void(int, int)> funcUpdateProgressRange;
	std::function<void(int, int)> funcUpdateProgressPos;
	std::function<void(const std::wstring&)> funcUpdateProgressText;
	std::function<void(bool)> funcUpdateProgressTextEmpty;

	bool isProcessingText = false; // Do not duplicate callbacks with "Processing..." text
	void Progress::UpdateProgressText(const std::wstring& text);
	void Progress::UpdateProgressTextEmpty(bool cancel = false);

	struct FileStruct
	{
		std::wstring path;
		std::wstring file;
		long long fileSize;
		long long fileTime;
	};

	std::deque<FileStruct> cueFiles;

	int numberFiles = 0;
	int numberStart = 0;
	int numberOffset = 0;
	long long addedTime = 0;
	std::atomic<bool> isStopThread = false;

	Threading::Thread threadWorker;
	void RunThread();

	bool CalculateFile(const std::wstring& folder);
	void CalculateFolder(const std::wstring& folder);

	void AddFolderToPlaylist(const std::wstring& folder, bool isTempPlaylist);
	void AddFolderToLibrary(const std::wstring& folder, bool isLibraryEmpty);
	bool AddFileToPlaylist(const std::wstring& file, bool isTempPlaylist);
	void AddURLToPlaylist(const std::wstring& url, const std::wstring& title, bool isTempPlaylist);

	void AddToLibrary(bool isLibraryEmpty, const std::wstring& path, const std::wstring& file, long long fileSize, long long fileTime);
	void AddToPlaylist(bool isTempPlaylist, const std::wstring& path, const std::wstring& file, long long fileSize, long long fileTime, bool fast = false);

	void AddCueToLibrary(bool isLibraryEmpty, const std::wstring& path, const std::wstring& file, long long cueSize, long long cueTime);
	bool AddCueToPlaylist(bool isTempPlaylist, const std::wstring& path, const std::wstring& file, long long cueSize, long long cueTime);

	bool IsMusicFile(const std::wstring& file, bool* outCue = nullptr);

	int GetFileHash(const std::wstring& file, bool& outNoDrive);

	void FillSongStruct(bool noDrive, const std::wstring& path, const std::wstring& file, int fileHash, long long fileSize, long long fileTime,
		DBase::DATABASE_SONGINFO* dataSongInfo, std::unique_ptr<TagLibReader> *tag = nullptr, CueFile *cue = nullptr, std::size_t i = 0);

	void CheckLibraryFiles();
	void CheckCueLibraryFiles();

	void ThreadLibrary();
	void ThreadPlaylist();
	void ThreadNewPlaylist();
};

