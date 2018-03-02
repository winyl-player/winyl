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

// DBase

#include "stdafx.h"
#include <random>
#include <chrono>
#include "XmlFile.h"
#include "sqlite3/sqlite3/src/sqlite3.h"
#include "SkinList.h"
#include "SkinTree.h"
#include "Language.h"
#include "UTF.h"

class DBase
{
public:
	DBase();
	virtual ~DBase();

public:
	// Forward declaration
	class SQLRequest;
	class SQLFile
	{
	friend class SQLRequest;
	private:
		sqlite3* db = nullptr;
	public:
		inline SQLFile()
		{

		}
		inline ~SQLFile()
		{
			assert(db == nullptr);
		}
		SQLFile(const SQLFile&) = delete;
		SQLFile& operator=(const SQLFile&) = default; // Beware
		bool operator==(const SQLFile& r) {return db == r.db;}
		bool operator!=(const SQLFile& r) {return !(*this == r);}
		explicit operator bool() {return (db ? true : false);}
		inline void Null() {db = nullptr;} // Beware
		sqlite3* get() {return db;}

		inline bool OpenCreate(const std::string& file)
		{
			assert(db == nullptr);
			bool result = sqlite3_open(file.c_str(), &db) == SQLITE_OK;
			if (!result && db)
			{
				sqlite3_close(db);
				db = nullptr;
			}
			assert(result == true);
			return result;
		}
		inline bool OpenWrite(const std::string& file)
		{
			assert(db == nullptr);
			bool result = sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr) == SQLITE_OK;
			if (!result && db)
			{
				sqlite3_close(db);
				db = nullptr;
			}
			assert(result == true);
			return result;
		}
		inline bool OpenRead(const std::string& file)
		{
			assert(db == nullptr);
			bool result = sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK;
			if (!result && db)
			{
				sqlite3_close(db);
				db = nullptr;
			}
			assert(result == true);
			return result;
		}
		inline bool Close()
		{
			assert(db != nullptr);
			if (db)
			{
				bool result = sqlite3_close(db) == SQLITE_OK;
				assert(result == true);
				if (result) db = nullptr;
				return result;
			}
			return false;
		}
		inline bool OpenCreate(const std::wstring& file) {return OpenCreate(UTF::UTF8S(file));}
		inline bool OpenWrite(const std::wstring& file) {return OpenWrite(UTF::UTF8S(file));}
		inline bool OpenRead(const std::wstring& file) {return OpenRead(UTF::UTF8S(file));}
	};

public:
	inline void SetLanguage(Language* language) {lang = language;}
	Language* lang = nullptr;

	SQLFile dbLibrary;  // Library database (always open)
	SQLFile dbPlaylist; // Opened playlist database
	SQLFile dbPlayOpen; // Playing playlist database
	SQLFile dbPlayTemp; // Additional database for advanced actions in the playlist
	SQLFile dbCue; // Cue sheets cache database

	bool isSmartlistOpen = false; // Is smartlist open?
	bool isSmartlistPlay = false; // Is smartlist playing?

	long long libraryLastInsertID = 0;

	std::atomic<bool> isStopSearch = false;
	inline void SetStopSearch(bool stop) {isStopSearch = stop;}
	inline bool IsStopSearch() {return isStopSearch;}

	bool isPortableVersion = false;
	void SetPortableVersion(bool isPortable) {isPortableVersion = isPortable;}

	struct DATABASE_SONGINFO
	{
		std::string file;
		std::string path;
		long long added = 0; // Date time when added to the library

		long long cue = 0;
		long long size = 0; // File size
		long long modified = 0; // Date time when file last modified

		int fileHash = 0;
		int trackHash = 0;
		std::string track;
		std::string totalTracks;
		std::string disc;
		std::string totalDiscs;
		std::string title;
		std::string album;
		std::string artist;
		std::string albumArtist;
		std::string composer;
		std::string genre;
		std::string year;
		std::string bpm;
		std::string compilation;
		std::string publisher;
		std::string conductor;
		std::string lyricist;
		std::string remixer;
		std::string grouping;
		std::string subtitle;
		std::string copyright;
		std::string encodedby;
		std::string comment;
		std::vector<std::string> artists;
		std::vector<std::string> albumArtists;
		std::vector<std::string> composers;
		std::vector<std::string> genres;
		std::vector<std::string> conductors;
		std::vector<std::string> lyricists;
		int duration = 0;
		int channels = 0;
		int bitrate = 0;
		int samplerate = 0;
	};

	struct DATABASE_GETINFO
	{
		std::wstring file;

		std::wstring track;
		std::wstring totalTracks;
		std::wstring disc;
		std::wstring totalDiscs;
		std::wstring title;
		std::wstring album;
		std::wstring artist;
		std::wstring albumArtist;
		std::wstring composer;
		std::wstring year;
		std::wstring genre;
		std::wstring lyricist;
		std::wstring comment;

		std::vector<std::wstring> artists;
		std::vector<std::wstring> albumArtists;
		std::vector<std::wstring> composers;
		std::vector<std::wstring> genres;
		std::vector<std::wstring> conductors;
		std::vector<std::wstring> lyricists;

		std::wstring bitrate;
		std::wstring samplerate;
		std::wstring channels;
		int type = 0;
		bool cue = false;
	};

	struct SmartList
	{
		std::wstring fromString;
		int from = 0;
		int type = 0;
		int order = 0;
		int count = 50;
		bool isRandom = true;
		bool isAutoUpdate = true;

		bool isAny = false;
		int playCount = 0;
		int playCountNum = 0;
		int rating = 0;
		int ratingNum = 0;
		int lastPlayed = 0;
		int lastPlayedNum = 0;
		int dateAdded = 0;
		int dateAddedNum = 0;
		int tracksCount = 0;
		int tracksCountNum = 0;

		bool IsFilter()
		{
			if (type == 0 && (playCount || rating || lastPlayed || dateAdded))
				return true;
			else if (type == 1 && tracksCount)
				return true;

			return false;
		}

		static std::string GetSmartDate(int index)
		{
			switch (index)
			{
				case 0: return "strftime('%s','now','-1 day')";
				case 1: return "strftime('%s','now','-2 days')";
				case 2: return "strftime('%s','now','-3 days')";
				case 3: return "strftime('%s','now','-7 days')";
				case 4: return "strftime('%s','now','-14 days')";
				case 5: return "strftime('%s','now','-1 month')";
				case 6: return "strftime('%s','now','-3 months')";
				case 7: return "strftime('%s','now','-6 months')";
				case 8: return "strftime('%s','now','-1 year')";
			}
			return "";
		}
	};

	struct Collapsed
	{
		static const int Album = 1;
		static const int Folder = 2;
		//static const int Next = 4;
	};

	enum class Category
	{
		//Audiobook = 1,
		Radio = 10,
		UserRadio = 11
	};

	void CreateTableLibrary(const SQLFile& db); // Create table for the library
	void CreateTablePlaylist(const SQLFile& db); // Create table for a playlist
	void CreateTableSmartlist(const SQLFile& db); // Create table for a smartlist (not auto updating, for auto updating we no need table)
	void CreateTableCue(const SQLFile& db); // Create table for cue sheets cache

	void InsertMultipleValues(long long id, SQLRequest& sqlInsert, DATABASE_SONGINFO* tags);

	// Begin: Functions to add and update a data in the library database //

	int GetCountLibFromPls();
	void DeleteLibFromPls();
	void FindFirstLibFromPlsFile(SQLRequest& sqlSelect);
	bool FindNextLibFromPlsFile(SQLRequest& sqlSelect, long long& outID,
		std::wstring& outPath, std::wstring& outFile, long long& outModified, long long& outSize, bool& outCue);
	long long GetLastAddedToLib();

	int GetCueCountLibFromPls();
	void FindFirstCueLibFromPlsFile(SQLRequest& sqlSelect);
	bool FindNextCueLibFromPlsFile(SQLRequest& sqlSelect, long long& outID, std::wstring& outPath, std::wstring& outFile,
		long long& outModified, long long& outSize, std::wstring& outRefFile, int& outRefHash);

	// Add a file to the library
	bool AddFileToLibrary(DATABASE_SONGINFO* tags);
	
	// Update tags because a file was modified
	void UpdateTagsModified(long long id, DATABASE_SONGINFO* tags);

	// Update tags because a file was moved
	void UpdateTagsFileMove(long long id, DATABASE_SONGINFO* tags);

	// Add a file to cue cache
	void AddCueFile(bool noDrive, const std::wstring& path, const std::wstring& file,
		int hash, long long size, long long modified, const std::wstring& refFile, int refHash);

	// Update a file in cue cache
	void UpdateCueFile(long long id, long long size, long long modified, const std::wstring& refFile, int refHash);

	// Update cues in the library
	bool UpdateCueLibrary(bool noDrive, const std::wstring& path, const std::wstring& file, int hash);

	// Delete cue image file from the library
	bool DeleteCueImage(bool noDrive, const std::wstring& path, const std::wstring& file, int hash);

	// Check that a file (part of cue) is existing in the library
	bool CheckCueFile(bool noDrive, const std::wstring& path, const std::wstring& file,
		int hash, long long size, long long modified, long long& outID, bool flag = false, bool isRescanAll = false);

	// Check that a file is existing in the library
	bool CheckFile(bool noDrive, const std::wstring& path, const std::wstring& file,
		int hash, long long cue, long long& outID, long long& outModified, long long& outSize, bool &outCue, bool flag = false);
	
	// Search for a file by tags
	bool CheckTags(long long& outID, DATABASE_SONGINFO* tags);

	// End: Functions to add and update a data in the library database //

	// Begin: Functions to add a data to the playlist database //

	// The following two functions can add a file to opened and additional playlist depending on isTemp
	// By default add to opened playlist (bool isTemp = false)
	
	// Add a file to the playlist (opened or additional)
	bool AddFileToPlaylist(int index, DATABASE_SONGINFO* tags, bool isTemp = false);

	// Add a file to the playlist (opened or additional) from Library or struct.
	void AddFileToPlaylistFrom(long long id, int index, long long added, bool isTemp = false);
	
	// Add url to the playlist (opened or additional)
	void AddURLToPlaylist(int index, long long added, const std::wstring& url, const std::wstring& title, bool isTemp = false);
	
	// End: Functions to add a data to the playlist database //

	// Begin: Functions to fill controls with a data //

	// All following functions directly works with SkinList and SkinTree controls

	void FillTreeAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeComposer(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeGenre(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeYear(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeNode(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect);
	void FillTreeNodeAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect);
	void FillTreeNodeArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect);

	void FillTreeArtistAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeComposerArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeComposerAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeGenreArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeGenreAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeYearArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode);
	void FillTreeYearAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode);

	void FillTreeFolder(SkinTree* skinTree, TreeNodeUnsafe treeNode, std::vector<std::wstring>* libraryFolders);
	void FillTreeFolderSub(SkinTree* skinTree, TreeNodeUnsafe treeNode);

	void FillListAlbum(SkinList* skinList, const std::wstring& value);
	void FillListArtist(SkinList* skinList, const std::wstring& value);
	void FillListArtistAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album);
	void FillListComposer(SkinList* skinList, const std::wstring& value);
	void FillListComposerArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist);
	void FillListComposerAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album);
	void FillListGenre(SkinList* skinList, const std::wstring& value);
	void FillListGenreArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist);
	void FillListGenreAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album);
	void FillListYear(SkinList* skinList, const std::wstring& value);
	void FillListYearArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist);
	void FillListYearAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album);
	void FillList(SkinList* skinList, SQLRequest& sqlSelect, bool isPlaylist = false);

	void FillPlaylist(SkinList* skinList, const std::wstring& file);
	void FillPlaylistOpenFile(SkinList* skinList, const std::wstring& file, int start);
	void FillPlaylistNowPlaying(SkinList* skinList, const std::wstring& file, int start);
	void FillPlay(SkinList* skinList, SQLRequest& sqlSelect, bool isPlaylist = false, bool isSelect = false, bool isNowPlaying = false);

	void FillListSearchTrack(SkinList* skinList, const std::wstring& value);
	void FillListSearchAlbum(SkinList* skinList, const std::wstring& value);
	void FillListSearchArtist(SkinList* skinList, const std::wstring& value);
	void FillListSearchAll(SkinList* skinList, const std::wstring& value);

	void FillListFolder(SkinList* skinList, const std::wstring& value);

	// End: Functions for fill controls with a data //

	// Return a song info (used only in properties dialog)
	void GetLibFile(long long idLibrary, long long idPlaylist, DATABASE_GETINFO* info);
	// Return tags to display (used to display in the skin)
	void GetSongTags(long long idLibrary, long long idPlaylist, std::wstring& file, std::wstring& title, std::wstring& album,
					std::wstring& artist, std::wstring& genre, std::wstring& year, bool isPlay = true);


	void OpenLibrary();

	std::wstring programPath;
	std::wstring profilePath;

	inline void SetProgramPath(const std::wstring& path) {programPath = path;}
	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}

	// Create a playlist (return the created playlist in the tree control or nullptr if error)
	TreeNodeUnsafe CreatePlaylist(SkinTree* skinTree, const std::wstring& name, bool isDefault = false);

	bool LoadPlaylist(SkinTree* skinTree); // Load all playlists to the tree control
	bool SavePlaylist(SkinTree* skinTree); // Save all playlists from the tree control
	void DeletePlaylist(SkinTree* skinTree); // Delete a playlist selected in the tree control

	void OpenPlaylist(const std::wstring& fileName); // Open playlist
	void ClosePlaylist(); // Close playlist
	bool IsPlaylistOpen() {return (dbPlaylist ? true : false);} // Is playlist open?
	void ReturnNowPlaying(); // Now Playing playlist becomes current open
	void NewNowPlaying(); // The current open playlist becomes Now Playing
	void CloseNowPlaying(); // Close Now Playing playlist
	inline bool IsNowPlayingOpen() {return (dbPlayOpen && (dbPlayOpen == dbPlaylist));} // Now Playing playlist is open and is current?
	inline bool IsNowPlaying() {return (dbPlayOpen ? true : false);} // Now Playing playlist is open?

	int GetPlaylistMax(); // Return the latest track ID in the playlist database

	// Delete selected tracks from the library database
	void DeleteFromLibrary(SkinList* skinList, std::vector<std::wstring>* libraryFolders = nullptr);

	// Delete selected tracks from the playlist database
	void DeleteFromPlaylist(SkinList* skinList);

	// Update the index of elements in the playlist database
	void SwapPlaylist(SkinList* skinList);

	// Add to a playlist from: library, other playlist or radio list
	int FromLibraryToPlaylist(SkinList* skinList, const std::wstring& fileName);
	int FromPlaylistToPlaylist(SkinList* skinList, const std::wstring& fileName);
	int FromRadioToPlaylist(SkinList* skinList, const std::wstring& fileName);

	void Begin(); // Begin transaction in the library database
	void Commit(); // Commit transaction in the library database
	void Vacuum(); // Shrink the library database

	void PlayBegin(); // Begin transaction in the open playlist database
	void PlayCommit(); // Commit transaction in the open playlist database
	void PlayVacuum(); // Shrink the open playlist database

	void TempBegin(); // Begin transaction in the additional database
	void TempCommit(); // Commit transaction in the additional database
	void TempVacuum(); // Shrink the additional playlist database

	void CueBegin(); // Begin transaction in the cue database
	void CueCommit(); // Commit transaction in the cue database
	
	void OpenTemp(const std::wstring& fileName); // Open the additional database
	void CloseTemp(); // Close the additional database
	int GetTempMax(); // Return the latest track ID in the additional database

	void SetRating(long long idLibrary, long long idPlaylist, int rating, bool isPlay); // Set track rating
	void IncreaseCount(long long idLibrary, long long idPlaylist); // Increase track play count
	void IncreaseSkip(long long idLibrary, long long idPlaylist); // Increase track skip count

	bool IsLibraryEmpty(); // The library is empty?
	void RestoreDeleted(); // Restore tracks deleted from the library

	void SortPlaylist(int start, const std::wstring& name); // Sort playlist (before add files to playlist need to sort them by tags)
	void SortHelperAttach();
	void SortHelperDetach();

	// Fill smartlist (fill list control or fill smartlist database)
	void FillSmartlist(SkinList* skinList, const std::wstring& fileName, bool isUpdate = false);

	bool OpenSmartlist(const std::wstring& fileName, SmartList& smart); // Open smartlist
	bool SaveSmartlist(const std::wstring& fileName, SmartList& smart); // Save smartlist

	bool LoadTreeSmartlists(SkinTree* skinTree); // Load all smartlists to the tree control
	bool SaveTreeSmartlists(SkinTree* skinTree); // Save all smartlists from the tree control

	void DeleteTreeSmartlist(SkinTree* skinTree); // Delete a smartlist selected in the tree control

	// Add from the library to a smartlist by SQLRequest
	void FromLibraryToSmartlist(const std::wstring& fileName, SQLRequest &sqlSelect);

	// Create a smartlist (return created smartlist in the tree control or null if error)
	TreeNodeUnsafe CreateSmartlist(SkinTree* skinTree, const std::wstring& name, SmartList& smart);
	
	// Create default smartlists (it is located in this class for simplicity, here is not quite the place for this)
	void CreateMySmartlists(SkinTree* skinTree);

	// Delete selected elements from the smartlist database
	void DeleteFromSmartlist(SkinList* skinList, const std::wstring& file);

	bool IsSmartlistOpen() {return isSmartlistOpen;} // Is smartlist open?

	// Compound filters for smartlists
	void SmartPrepareSelect(SmartList &smart, std::string& select, std::vector<std::wstring>& values);
	void SmartFilterAlbums(SmartList &smart, std::string& select, std::vector<int>& values);
	void SmartFilterTracks(SmartList &smart, std::string& select, std::vector<int>& values);

	void UpdateTagsBegin();
	void UpdateTagsCommit();
	void UpdateTagsEditor(long long id, DATABASE_SONGINFO* tags, bool isPlaylist);
	void UpdateTagsNode(SkinList* skinList, ListNodeUnsafe listNode, DATABASE_SONGINFO* tags);

	// Extensions for SQLite, compare strings with ignore case etc.
	static int CompareStringsXP(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsLikeXP(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFolderSelectXP(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFolderGroupXP(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFileXP(void* context, int len1, const void* str1, int len2, const void* str2);

	static int CompareStrings(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsLike(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFolderSelect(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFolderGroup(void* context, int len1, const void* str1, int len2, const void* str2);
	static int CompareStringsFile(void* context, int len1, const void* str1, int len2, const void* str2);

	//static int CompareStringsNum(void* context, int len1, const void* str1, int len2, const void* str2);
	//static int CompareStringToNum(const char* str, int len);

	bool IsEqualPaths(const std::wstring& str1, size_t len1, const std::wstring& str2, size_t len2);

public:
	void MemFlagAttach();
	void MemFlagDetach();
	void SetUpdateAll();
	void SetUpdateEnd();
	void SetUpdateOK(long long id);
	void DeleteNotUpdated();
	void SetUpdateAllCue();
	void SetUpdateEndCue();
	void SetUpdateCueOK(long long id);
	void DeleteNotUpdatedCue();

public:
	// Helper for database requests
	class SQLRequest
	{
	private:
		sqlite3_stmt* ppVm = nullptr;

	public:
		SQLRequest()
		{

		}
		SQLRequest(const SQLRequest&) = delete;
		SQLRequest& operator=(const SQLRequest&) = delete;
		SQLRequest(const SQLFile& db, const char* text)
		{
			assert(db.db != nullptr);
			if (db.db) sqlite3_prepare_v2(db.db, text, -1, &ppVm, &text);
		}
		~SQLRequest()
		{
			if (ppVm) sqlite3_finalize(ppVm);
		}
		inline void Prepare(const SQLFile& db, const char* text)
		{
			assert(ppVm == nullptr);
			assert(db.db != nullptr);
			if (db.db) sqlite3_prepare_v2(db.db, text, -1, &ppVm, &text);
		}
		inline bool IsPrepared()
		{
			return ppVm ? true : false;
		}
		inline void Finalize()
		{
			assert(ppVm != nullptr);
			if (ppVm)
			{
				sqlite3_finalize(ppVm);
				ppVm = nullptr;
			}
		}
		inline void Step()
		{
			assert(ppVm != nullptr);
			int result = sqlite3_step(ppVm);
			assert(result == SQLITE_ROW || result == SQLITE_DONE);
		}
		inline bool StepRow()
		{
			assert(ppVm != nullptr);
			if (sqlite3_step(ppVm) == SQLITE_ROW)
				return true;

			return false;
		}
		inline bool StepDone()
		{
			assert(ppVm != nullptr);
			if (sqlite3_step(ppVm) == SQLITE_DONE)
				return true;

			return false;
		}
		inline void BindTextRaw(int number, const char* text)
		{
			assert(ppVm != nullptr);
			sqlite3_bind_text(ppVm, number, text, -1, SQLITE_TRANSIENT);
		}
		inline void BindInt(int number, int integer)
		{
			assert(ppVm != nullptr);
			sqlite3_bind_int(ppVm, number, integer);
		}
		inline void BindInt64(int number, long long integer64)
		{
			assert(ppVm != nullptr);
			sqlite3_bind_int64(ppVm, number, (sqlite3_int64)integer64);
		}
		inline void BindNull(int number)
		{
			assert(ppVm != nullptr);
			sqlite3_bind_null(ppVm, number);
		}
		inline int ColumnInt(int column)
		{
			assert(ppVm != nullptr);
			return sqlite3_column_int(ppVm, column);
		}
		inline long long ColumnInt64(int column)
		{
			assert(ppVm != nullptr);
			return (long long)sqlite3_column_int64(ppVm, column);
		}
		inline const char* ColumnTextRaw(int column)
		{
			assert(ppVm != nullptr);
			return (const char*)sqlite3_column_text(ppVm, column);
		}
		inline bool ColumnIsNull(int column)
		{
			assert(ppVm != nullptr);
			if (sqlite3_column_type(ppVm, column) == SQLITE_NULL)
				return true;

			return false;
		}
		inline void Reset()
		{
			assert(ppVm != nullptr);
			sqlite3_reset(ppVm);
		}
		inline void ClearBindings()
		{
			assert(ppVm != nullptr);
			sqlite3_clear_bindings(ppVm);
		}
		inline void StepReset()
		{
			Step();
			Reset();
		}
		inline void StepResetClear()
		{
			Step();
			Reset();
			ClearBindings();
		}
		inline static void Exec(const SQLFile& db, const char* text)
		{
			SQLRequest sql(db, text);
			sql.Step();
		}
		inline static bool ExecRow(const SQLFile& db, const char* text)
		{
			SQLRequest sql(db, text);
			return sql.StepRow();
		}
		inline static bool ExecDone(const SQLFile& db, const char* text)
		{
			SQLRequest sql(db, text);
			return sql.StepDone();
		}
		inline void BindText8(int number, const std::string& text)
		{
			BindTextRaw(number, text.c_str());
		}
		inline void BindText16(int number, const std::wstring& text)
		{
			BindTextRaw(number, UTF::UTF8S(text).c_str());
		}
		inline std::string ColumnText8(int column)
		{
			const char* rawtext = ColumnTextRaw(column);

			return rawtext ? rawtext : std::string();
		}
		inline bool ColumnText8(int column, std::string& text)
		{
			const char* rawtext = ColumnTextRaw(column);
			if (rawtext)
			{
				text = rawtext;
				return true;
			}
			return false;
		}
		inline std::wstring ColumnText16(int column)
		{
			const char* rawtext = ColumnTextRaw(column);

			return rawtext ? UTF::UTF16(rawtext) : std::wstring();
		}
		inline bool ColumnText16(int column, std::wstring& text)
		{
			const char* rawtext = ColumnTextRaw(column);
			if (rawtext)
			{
				text = UTF::UTF16(rawtext);
				return true;
			}
			return false;
		}

		//inline static bool Exec(sqlite3* db, const char* text)
		//{
		//	sqlite3_stmt* ppVmExec = nullptr;
		//	sqlite3_prepare_v2(db, text, -1, &ppVmExec, &text);
		//	if (ppVmExec)
		//	{
		//		int result = sqlite3_step(ppVmExec);
		//		sqlite3_finalize(ppVmExec);
		//
		//		if (result == SQLITE_ROW)
		//			return true;
		//	}
		//	return false;
		//}
	};
};


