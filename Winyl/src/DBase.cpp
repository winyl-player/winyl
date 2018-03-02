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

// DBase.cpp : implementation file
//

#include "stdafx.h"
#include "DBase.h"
#include "FileSystem.h"


// DBase

DBase::DBase()
{
	// SQLite must be compiled with SQLITE_USE_URI=1, but to be sure, do the same thing in runtime.
	// See also OpenLibrary function for comment about SQLITE_DEFAULT_FOREIGN_KEYS.
	if (sqlite3_config(SQLITE_CONFIG_URI, 1) != SQLITE_OK)
		assert(false); // sqlite3_config must be called before other sqlite functions
}

DBase::~DBase()
{
	// Close databases

	if (dbLibrary)
		dbLibrary.Close();

	if (dbCue)
		dbCue.Close();

	if (dbPlayOpen)
	{
		if (dbPlayOpen != dbPlaylist)
			dbPlayOpen.Close();

		dbPlayOpen.Null();
	}

	if (dbPlaylist)
		dbPlaylist.Close();

	if (dbPlayTemp)
		dbPlayTemp.Close();
}
/*
int DBase::CompareStringToNum(const char* str, int len)
{
	int value = 0;
	int sign = 1;
	int cnt = 0;

	while (cnt < len && *str == ' ')
	{
		cnt++;
		str++;
	}

	if (cnt < len && *str == '-')
		sign = -1;

	while (cnt < len && *str != '\0')
	{
		if (*str >= '0' && *str <= '9')
			value = (value * 10) + (*str - '0');
		else
			break;

		cnt++;
		str++;
	}

	return value * sign;
}

int DBase::CompareStringsNum(void* context, int len1, const void* str1, int len2, const void* str2)
{
	int i1 = CompareStringToNum((const char*)str1, len1);
	int i2 = CompareStringToNum((const char*)str2, len2);

	if (i1 > i2)
		return 1;
	else if (i1 < i2)
		return -1;

	return 0;
}
*/
int DBase::CompareStringsXP(void* context, int len1, const void* str1, int len2, const void* str2)
{
	//return lstrcmpi((const wchar_t*)str1, (const wchar_t*)str2);
	return CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, (LPCWSTR)str1, -1, (LPCWSTR)str2, -1) - CSTR_EQUAL;
}

int DBase::CompareStringsLikeXP(void* context, int len1, const void* str1, int len2, const void* str2)
{
	if (len1 < len2)
		return -1;

	// Find substring
	int i = 0, size = len1/2 - len2/2; // see assert
	for (wchar_t* first = (wchar_t*)str1;*first && i <= size; ++first, ++i)
	{
		//assert((size_t)len2/2 <= wcslen(first));
		if (CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, (LPCWSTR)first, len2/2, (LPCWSTR)str2, len2/2) == CSTR_EQUAL)
			return 0;
	}

	return -1;
}

int DBase::CompareStringsFolderSelectXP(void* context, int len1, const void* str1, int len2, const void* str2)
{
	if (len1 < len2)
		return -1;

	if (CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, (LPCWSTR)str1, len2/2, (LPCWSTR)str2, len2/2) == CSTR_EQUAL)
		return 0;

	return -1;
}

int DBase::CompareStringsFolderGroupXP(void* context, int len1, const void* str1, int len2, const void* str2)
{
	const wchar_t* find1 = wcsrchr((const wchar_t*)str1, '\\');
	const wchar_t* find2 = wcsrchr((const wchar_t*)str2, '\\');

	return CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE,
		(LPWSTR)str1, (DWORD)(find1 - (LPWSTR)str1) + 1, (LPWSTR)str2, (DWORD)(find2 - (LPWSTR)str2) + 1) - CSTR_EQUAL;


	// Test
	//const wchar_t* find1 = wcsrchr((const wchar_t*)str1, '\\');
	//const wchar_t* find2 = wcsrchr((const wchar_t*)str2, '\\');

	//unsigned int count1 = find1 ? find1 - (const wchar_t*)str1 + 1 : 0;
	//unsigned int count2 = find2 ? find2 - (const wchar_t*)str2 + 1 : 0;

	//int ret = 0;
	//if (count1 <= count2)
	//	ret = wcsncmp((const wchar_t*)str1, (const wchar_t*)str2, count1);
	//else
	//	ret = wcsncmp((const wchar_t*)str1, (const wchar_t*)str2, count2);

	//if (ret == 0)
	//	ret = count1 - count2;

	//return ret;
}

int DBase::CompareStringsFileXP(void* context, int len1, const void* str1, int len2, const void* str2)
{
	return CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, (LPCWSTR)str1, -1, (LPCWSTR)str2, -1) - CSTR_EQUAL;
}

int DBase::CompareStrings(void* context, int len1, const void* str1, int len2, const void* str2)
{
	return futureWin->CompareStringEx(LOCALE_NAME_USER_DEFAULT, LINGUISTIC_IGNORECASE, (LPCWSTR)str1, -1, (LPCWSTR)str2, -1, 0, 0, 0) - CSTR_EQUAL;
}

int DBase::CompareStringsLike(void* context, int len1, const void* str1, int len2, const void* str2)
{
	if (len1 < len2)
		return -1;

	if (futureWin->FindNLSStringEx(LOCALE_NAME_USER_DEFAULT, FIND_FROMSTART|LINGUISTIC_IGNORECASE, (LPCWSTR)str1, -1, (LPCWSTR)str2, -1,
		NULL, 0, 0, 0) > -1)
		return 0;

	return -1;
}

int DBase::CompareStringsFolderSelect(void* context, int len1, const void* str1, int len2, const void* str2)
{
	if (len1 < len2)
		return -1;

	if (futureWin->CompareStringOrdinal((LPCWSTR)str1, len2/2, (LPCWSTR)str2, len2/2, TRUE) == CSTR_EQUAL)
		return 0;

	return -1;
}

int DBase::CompareStringsFolderGroup(void* context, int len1, const void* str1, int len2, const void* str2)
{
	const wchar_t* find1 = wcsrchr((const wchar_t*)str1, '\\');
	const wchar_t* find2 = wcsrchr((const wchar_t*)str2, '\\');

	return futureWin->CompareStringOrdinal(
		(LPCWSTR)str1, (DWORD)(find1 - (LPCWSTR)str1) + 1, (LPCWSTR)str2, (DWORD)(find2 - (LPCWSTR)str2) + 1, TRUE) - CSTR_EQUAL;
}

int DBase::CompareStringsFile(void* context, int len1, const void* str1, int len2, const void* str2)
{
	return futureWin->CompareStringOrdinal((LPCWSTR)str1, len1/2, (LPCWSTR)str2, len2/2, TRUE) - CSTR_EQUAL;
}

bool DBase::IsEqualPaths(const std::wstring& str1, size_t len1, const std::wstring& str2, size_t len2)
{
	assert(len1 <= str1.size());
	assert(len2 <= str2.size());

	if (len1 != len2)
		return false;

	if (futureWin->IsVistaOrLater())
		return futureWin->CompareStringOrdinal((LPCWSTR)str1.c_str(), (int)len1, (LPCWSTR)str2.c_str(), (int)len2, TRUE) == CSTR_EQUAL;
	else
		return CompareStringW(LOCALE_INVARIANT, NORM_IGNORECASE, (LPCWSTR)str1.c_str(), (int)len1, (LPCWSTR)str2.c_str(), (int)len2) == CSTR_EQUAL;
}

void DBase::OpenLibrary()
{
	std::wstring file = profilePath + L"Library.db";
	
	dbLibrary.OpenCreate(file);

	//// Test load to memory
	//DBase::SQLFile dbLibraryFile;
	//dbLibraryFile = dbLibrary;
	//dbLibrary.Null();
	//dbLibrary.OpenCreate(":memory:");
	//sqlite3_backup *pBackup = sqlite3_backup_init(dbLibrary.get(), "main", dbLibraryFile.get(), "main");
	//sqlite3_backup_step(pBackup, -1);
	//sqlite3_backup_finish(pBackup);
	//dbLibraryFile.Close();
	//// Test save from memory
	//int c1 = sqlite3_changes(dbLibrary.get());
	//int c2 = sqlite3_total_changes(dbLibrary.get());
	//DBase::SQLFile dbLibraryTemp;
	//dbLibraryTemp.OpenCreate(profilePath + L"Test.db");
	//SQLRequest::Exec(dbLibraryTemp, "PRAGMA journal_mode = OFF;");
	////SQLRequest::Exec(dbLibraryTemp, "PRAGMA locking_mode = EXCLUSIVE;");
	////SQLRequest::Exec(dbLibraryTemp, "PRAGMA synchronous = OFF;");
	//pBackup = sqlite3_backup_init(dbLibraryTemp.get(), "main", dbLibrary.get(), "main");
	//sqlite3_backup_step(pBackup, -1);
	//sqlite3_backup_finish(pBackup);
	//dbLibraryTemp.Close();

	//// Test some pragmas
	//SQLRequest::Exec(dbLibrary, "PRAGMA locking_mode=EXCLUSIVE;"); // PRAGMA journal_size_limit
	//SQLRequest::Exec(dbLibrary, "PRAGMA synchronous=NORMAL;");
	//SQLRequest::Exec(dbLibrary, "PRAGMA journal_mode=WAL;");
	//// Can be changed back with
	//SQLRequest::Exec(dbLibrary, "PRAGMA journal_mode=DELETE;");
	//// Maybe truncate? (if DELETE and EXCLUSIVE or WAL)
	//SQLRequest::Exec(dbLibrary, "PRAGMA journal_size_limit=0;");

	file = profilePath + L"Cue.db";

	dbCue.OpenCreate(file);

	// SQLite must be compiled with SQLITE_DEFAULT_FOREIGN_KEYS=1, use these asserts to check this,
	// but to be sure we still use PRAGMA foreign_keys = ON.
	// Note: Instead of this it would be better to use sqlite3_compileoption_used to check both
	// SQLITE_DEFAULT_FOREIGN_KEYS and SQLITE_USE_URI, but it seems the function does not work for these options.
	// See also constructor of DBase for comment about SQLITE_USE_URI.
#ifndef NDEBUG
	SQLRequest sqlCheckForeignKeys(dbLibrary, "PRAGMA foreign_keys;");
	assert(sqlCheckForeignKeys.StepRow() == true);
	assert(sqlCheckForeignKeys.ColumnInt(0) == 1);
	sqlCheckForeignKeys.Finalize();
#endif

	//sqlite3_create_collation(dbLibrary.get(), "MYNUM", SQLITE_UTF8, nullptr, CompareStringsNum);
	if (futureWin->IsVistaOrLater())
	{
		sqlite3_create_collation(dbLibrary.get(), "MYCASE", SQLITE_UTF16LE, nullptr, CompareStrings);
		sqlite3_create_collation(dbLibrary.get(), "LIKECASE", SQLITE_UTF16LE, nullptr, CompareStringsLike);
		sqlite3_create_collation(dbLibrary.get(), "FSELECT", SQLITE_UTF16LE, nullptr, CompareStringsFolderSelect);
		sqlite3_create_collation(dbLibrary.get(), "FGROUP", SQLITE_UTF16LE, nullptr, CompareStringsFolderGroup);
		sqlite3_create_collation(dbLibrary.get(), "FILECASE", SQLITE_UTF16LE, nullptr, CompareStringsFile);
		sqlite3_create_collation(dbCue.get(), "FILECASE", SQLITE_UTF16LE, nullptr, CompareStringsFile);
	}
	else
	{
		sqlite3_create_collation(dbLibrary.get(), "MYCASE", SQLITE_UTF16LE, nullptr, CompareStringsXP);
		sqlite3_create_collation(dbLibrary.get(), "LIKECASE", SQLITE_UTF16LE, nullptr, CompareStringsLikeXP);
		sqlite3_create_collation(dbLibrary.get(), "FSELECT", SQLITE_UTF16LE, nullptr, CompareStringsFolderSelectXP);
		sqlite3_create_collation(dbLibrary.get(), "FGROUP", SQLITE_UTF16LE, nullptr, CompareStringsFolderGroupXP);
		sqlite3_create_collation(dbLibrary.get(), "FILECASE", SQLITE_UTF16LE, nullptr, CompareStringsFileXP);
		sqlite3_create_collation(dbCue.get(), "FILECASE", SQLITE_UTF16LE, nullptr, CompareStringsFileXP);
	}

	CreateTableLibrary(dbLibrary);
	CreateTableCue(dbCue);
}

void DBase::OpenPlaylist(const std::wstring& fileName)
{
	std::wstring file = profilePath;
	file += L"Playlists";
	file.push_back('\\');
	file += fileName + L".db";

	dbPlaylist.OpenCreate(file);

	CreateTablePlaylist(dbPlaylist);
}

void DBase::ClosePlaylist()
{
	if (dbPlaylist)
	{
		if (dbPlaylist != dbPlayOpen)
			dbPlaylist.Close();

		dbPlaylist.Null();
	}
	if (isSmartlistOpen)
	{
		isSmartlistOpen = false;
	}
}

void DBase::ReturnNowPlaying()
{
	if (dbPlayOpen)
	{
		dbPlaylist = dbPlayOpen;
	}
	else if (isSmartlistPlay)
	{
		isSmartlistOpen = true;
	}
}

void DBase::NewNowPlaying()
{
	if (dbPlaylist) // If playlist is open it becomes Now Playing
	{
		// If assert then something bad is happening
		assert(!dbPlayOpen);

		if (!dbPlayOpen) // Can we really do this?
			dbPlayOpen = dbPlaylist;
	}
	if (isSmartlistOpen)
	{
		isSmartlistPlay = true;
	}
}

void DBase::CloseNowPlaying()
{
	if (dbPlayOpen)
	{
		if (dbPlayOpen != dbPlaylist)
			dbPlayOpen.Close();
		
		dbPlayOpen.Null();
	}
	if (isSmartlistPlay)
	{
		isSmartlistPlay = false;
	}
}

void DBase::CreateTableLibrary(const SQLFile& db)
{
	//SQLRequest::Exec(db, "PRAGMA cache_spill = OFF;"); // See DeleteNotUpdated
	SQLRequest::Exec(db, "PRAGMA cache_size = 10000;"); // Use this instead of previous

	SQLRequest::Exec(db, "PRAGMA foreign_keys = ON;");

	// Library tables already created
	if (SQLRequest::ExecRow(db, "SELECT name FROM sqlite_master WHERE type='table' AND name='library';"))
		return;

	SQLRequest::Exec(db, "BEGIN;");

	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS library ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT," // Track ID (Primary key)
		"flag INTEGER,"            // Update flag
		"deleted INTEGER,"         // Is track deleted?
		"added INTEGER,"           // Date time when added to the library
		"disabled INTEGER,"        // Disabled state
		"collapsed INTEGER,"       // Collapsed state
		"cue INTEGER,"             // CUE
		"filehash INTEGER,"        // File hash
		"path TEXT,"               // File path
		"file TEXT,"               // File name
		"filesize INTEGER,"        // File size
		"modified INTEGER,"        // Date time when file last modified
		"category INTEGER,"        // Track category
		"trackhash INTEGER,"       // Track hash
		"track TEXT,"              // Track number in album
		"totaltracks TEXT,"        // Total tracks in album
		"disc TEXT,"               // Disc number
		"totaldiscs TEXT,"         // Total discs
		"title TEXT,"              // Track title
		"album TEXT,"              // Track album
		"artist TEXT,"             // Track artist
		"albumartist TEXT,"        // Track album artist
		"composer TEXT,"           // Track composer
		"genre TEXT,"              // Track genre
		"year TEXT,"               // Track release year
		"bpm TEXT,"                // Track BPM
		"compilation TEXT,"        // Track Part of Compilation
		"publisher TEXT,"          // Track publisher
		"conductor TEXT,"          // Track conductor
		"lyricist TEXT,"           // Track lyricist
		"remixer TEXT,"            // Track remixer
		"grouping TEXT,"           // Track grouping
		"subtitle TEXT,"           // Track subtitle
		"copyright TEXT,"          // Track copyright
		"encodedby TEXT,"          // Track encoded by
		"comment TEXT,"            // Track comment
		"duration INTEGER,"        // Track time length
		"channels INTEGER,"        // Track number of channels
		"bitrate INTEGER,"         // Track bitrate
		"samplerate INTEGER,"      // Track sample rate
		"rating INTEGER,"          // Track rating
		"loverating INTEGER,"      // Track love rating
		"albumrating INTEGER,"     // Album rating
		"folderrating INTEGER,"    // Folder rating
		"playcount INTEGER,"       // Track play count
		"lastplayed INTEGER,"      // Track date time of last play
		"skipcount INTEGER,"       // Track skin count
		"lastskipped INTEGER,"     // Track date time of last skip
		"bookmark INTEGER,"        // Track bookmark
		"replaygain TEXT,"         // ReplayGain values
		"equalizer TEXT,"          // Track equalizer settings
		"keywords TEXT);"          // Keywords/tags for track
	);

	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS storage ("
		"spk INTEGER PRIMARY KEY,"
		"sid INTEGER REFERENCES library(id) ON DELETE CASCADE,"
		"sidx INTEGER,"
		"skey INTEGER,"
		"svalue TEXT);"
	);

	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS cue_index ON library(cue) WHERE cue IS NOT NULL;");
	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS filehash_index ON library(filehash);");
	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS trackhash_index ON library(trackhash);");

	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS sid_index ON storage(sid);");
	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS skey_index ON storage(skey);");

	SQLRequest::Exec(db, "COMMIT;");
}

void DBase::CreateTablePlaylist(const SQLFile& db)
{
	SQLRequest::Exec(db, "PRAGMA foreign_keys = ON;");

	// Playlist tables already created
	if (SQLRequest::ExecRow(db, "SELECT name FROM sqlite_master WHERE type='table' AND name='playlist';"))
		return;

	SQLRequest::Exec(db, "BEGIN;");

	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS playlist ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"  // Track ID (Primary key)
		"idlib INTEGER,"           // Track ID in the library database
		"idx INTEGER,"             // Track index (for order in the playlist)
		"added INTEGER,"           // Date time when added to the playlist
		"disabled INTEGER,"        // Disabled state
		"collapsed INTEGER,"       // Collapsed state
		"cue INTEGER,"             // CUE
		"filehash INTEGER,"        // File hash
		"path TEXT,"               // File path
		"file TEXT,"               // File name
		"filesize INTEGER,"        // File size
		"modified INTEGER,"        // Date time when file last modified
		"category INTEGER,"        // Track category
		"trackhash INTEGER,"       // Track hash
		"track TEXT,"              // Track number in album
		"totaltracks TEXT,"        // Total tracks in album
		"disc TEXT,"               // Disc number
		"totaldiscs TEXT,"         // Total discs
		"title TEXT,"              // Track title
		"album TEXT,"              // Track album
		"artist TEXT,"             // Track artist
		"albumartist TEXT,"        // Track album artist
		"composer TEXT,"           // Track composer
		"genre TEXT,"              // Track genre
		"year TEXT,"               // Track release year
		"bpm TEXT,"                // Track BPM
		"compilation TEXT,"        // Track Part of Compilation
		"publisher TEXT,"          // Track publisher
		"conductor TEXT,"          // Track conductor
		"lyricist TEXT,"           // Track lyricist
		"remixer TEXT,"            // Track remixer
		"grouping TEXT,"           // Track grouping
		"subtitle TEXT,"           // Track subtitle
		"copyright TEXT,"          // Track copyright
		"encodedby TEXT,"          // Track encoded by
		"comment TEXT,"            // Track comment
		"duration INTEGER,"        // Track time length
		"channels INTEGER,"        // Track number of channels
		"bitrate INTEGER,"         // Track bitrate
		"samplerate INTEGER,"      // Track sample rate
		"rating INTEGER,"          // Track rating
		"loverating INTEGER,"      // Track love rating
		"albumrating INTEGER,"     // Album rating
		"folderrating INTEGER,"    // Folder rating
		"playcount INTEGER,"       // Track play count
		"lastplayed INTEGER,"      // Track date time of last play
		"skipcount INTEGER,"       // Track skin count
		"lastskipped INTEGER,"     // Track date time of last skip
		"bookmark INTEGER,"        // Track bookmark
		"replaygain TEXT,"         // ReplayGain values
		"equalizer TEXT,"          // Track equalizer settings
		"keywords TEXT);"          // Keywords/tags for track
	);

	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS storage ("
		"spk INTEGER PRIMARY KEY,"
		"sid INTEGER REFERENCES playlist(id) ON DELETE CASCADE,"
		"sidx INTEGER,"
		"skey INTEGER,"
		"svalue TEXT);"
	);

	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS sid_index ON storage(sid);");
	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS skey_index ON storage(skey);");

	SQLRequest::Exec(db, "COMMIT;");
}

void DBase::CreateTableSmartlist(const SQLFile& db)
{
	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS smartlist ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT," // Track ID (Primary key)
		"idlib INTEGER,"           // Track ID in the library database
		"idx INTEGER,"             // Track index (for order in the playlist)
		"added INTEGER,"           // Date time when added to the smartlist
		"disabled INTEGER,"        // Disabled state
		"collapsed INTEGER);"      // Collapsed state
	);
}

void DBase::CreateTableCue(const SQLFile& db)
{
	if (SQLRequest::ExecRow(db, "SELECT name FROM sqlite_master WHERE type='table' AND name='cue';"))
		return;

	SQLRequest::Exec(db, "BEGIN;");

	SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS cue ("
		"id INTEGER PRIMARY KEY,"
		"filehash INTEGER,"
		"path TEXT,"
		"file TEXT,"
		"filesize INTEGER,"
		"modified INTEGER,"
		"refhash INTEGER,"
		"reffile TEXT);"
	);

	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS filehash_index ON cue(filehash);");
	SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS refhash_index ON cue(refhash) WHERE refhash IS NOT NULL;");

	SQLRequest::Exec(db, "COMMIT;");
}

TreeNodeUnsafe DBase::CreatePlaylist(SkinTree* skinTree, const std::wstring& name, bool isDefault)
{
	if (skinTree == nullptr)
		return nullptr;

	std::wstring folder = profilePath;
	folder += L"Playlists";

	// Create the folder for playlists
	FileSystem::CreateDir(folder);

	TreeNodeUnsafe resultNode = nullptr;

	if (isDefault)
	{
		std::wstring file = folder;
		file.push_back('\\');
		file += L"default.db";
	
		SQLFile dbNew;

		dbNew.OpenCreate(file);
		CreateTablePlaylist(dbNew);
		dbNew.Close();

		resultNode = skinTree->AddPlaylist(name, L"default", true, true);
		SavePlaylist(skinTree);

		return resultNode;
	}

	//std::random_device randomDevice;
	auto randomSeed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 randomEngine((unsigned)randomSeed);
	std::uniform_int_distribution<int> randomInt(0x00, 0xFF);
	auto randomHex = [&](){return randomInt(randomEngine);};

	// Try to create database three times in the case of matching file names
	for (int i = 0; i < 3; ++i)
	{
		// Generate 4 random numbers
		int random1 = randomHex();
		int random2 = randomHex();
		int random3 = randomHex();
		int random4 = randomHex();

		// Make a database name from the random numbers
		std::wstring random = StringEx::Format(L"%.2x%.2x%.2x%.2x", random1, random2, random3, random4);

		std::wstring file = folder;
		file.push_back('\\');
		file += random;
		file += L".db";
	
		SQLFile dbNew;

		// Create a new database (if a database with the same name doesn't exist)
		if (!FileSystem::Exists(file))
		{
			dbNew.OpenCreate(file);
			CreateTablePlaylist(dbNew);
			dbNew.Close();

			resultNode = skinTree->AddPlaylist(name, random);
			SavePlaylist(skinTree);

			break;
		}
	}

	return resultNode;
}

bool DBase::LoadPlaylist(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return false;

	std::wstring filePlaylists = profilePath + L"Playlists.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(filePlaylists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Playlists");

		if (xmlMain)
		{
			// Load all elements
			for (XmlNode xmlNode = xmlMain.FirstChild("Play"); xmlNode; xmlNode = xmlNode.NextChild("Play"))
			{
				bool isDefault = false;
				xmlNode.Attribute("Default", &isDefault);
				std::wstring name = xmlNode.Attribute16("Name");
				std::wstring file = xmlNode.Attribute16("File");

				skinTree->InsertPlaylist(skinTree->GetPlaylistNode(), name, file, false, isDefault);
			}
		}
	}
	else
		return false;

	return true;
}

bool DBase::SavePlaylist(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return false;

	TreeNodeUnsafe playNode = skinTree->GetPlaylistNode();

	if (playNode == nullptr) // Playlists are disabled
		return false;

	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("Playlists");
	
	for (TreeNodeUnsafe treeNode = playNode->Child(); treeNode != nullptr; treeNode = treeNode->Next())
	{
		XmlNode xmlPlay = xmlMain.AddChild("Play");

		xmlPlay.AddAttribute16("Name", treeNode->GetTitle());
		xmlPlay.AddAttribute16("File", treeNode->GetValue());
		if (skinTree->GetDefPlaylistNode() == treeNode)
			xmlPlay.AddAttribute("Default", 1);
	}
	
	std::wstring file = profilePath + L"Playlists.xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void DBase::DeletePlaylist(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return;

	TreeNodeUnsafe focusNode = skinTree->GetFocusNode();

	if (focusNode)
	{
		// Delete the database file
		std::wstring file = profilePath;
		file += L"Playlists";
		file.push_back('\\');
		file += focusNode->GetValue() + L".db";

		if (FileSystem::RemoveFile(file))
		{
			// If the file has been deleted then delete the node
			skinTree->DeleteSelected(skinTree->GetFocusNode());

			// And save new nodes state
			SavePlaylist(skinTree);
		}
	}
}

bool DBase::AddFileToLibrary(DATABASE_SONGINFO* tags)
{
	SQLRequest sqlInsert(dbLibrary,
		"INSERT INTO library (flag,added,cue,filehash,path,file,filesize,modified,trackhash,track,totaltracks,"
		"disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,bpm,compilation,publisher,conductor,lyricist,"
		"remixer,grouping,subtitle,copyright,encodedby,comment,duration,channels,bitrate,samplerate)"
		" VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");


	// 1. Update flag
	sqlInsert.BindNull(1);

	// 2. Date time when added to the library
	assert(tags->added != 0);
	sqlInsert.BindInt64(2, tags->added);

	// 3. CUE
	if (tags->cue)
		sqlInsert.BindInt64(3, tags->cue);
	else
		sqlInsert.BindNull(3);

	// 4. File hash
	sqlInsert.BindInt(4, tags->fileHash);

	// 5. File path
	assert(!tags->path.empty());
	sqlInsert.BindText8(5, tags->path);

	// 6. File name
	assert(!tags->file.empty());
	sqlInsert.BindText8(6, tags->file);

	// 7. File size
	sqlInsert.BindInt64(7, tags->size);

	// 8. Date time when file last modified
	assert(tags->modified != 0);
	sqlInsert.BindInt64(8, tags->modified);

	// 9. Track hash
	sqlInsert.BindInt(9, tags->trackHash);

	// 10. Track number
	if (!tags->track.empty())
		sqlInsert.BindText8(10, tags->track);
	else
		sqlInsert.BindNull(10);

	// 11. Total tracks
	if (!tags->totalTracks.empty())
		sqlInsert.BindText8(11, tags->totalTracks);
	else
		sqlInsert.BindNull(11);

	// 12. Disc number
	if (!tags->disc.empty())
		sqlInsert.BindText8(12, tags->disc);
	else
		sqlInsert.BindNull(12);

	// 13. Total discs
	if (!tags->totalDiscs.empty())
		sqlInsert.BindText8(13, tags->totalDiscs);
	else
		sqlInsert.BindNull(13);

	// 14. Track title
	if (!tags->title.empty())
		sqlInsert.BindText8(14, tags->title);
	else
		sqlInsert.BindNull(14);

	// 15. Track album
	if (!tags->album.empty())
		sqlInsert.BindText8(15, tags->album);
	else
		sqlInsert.BindNull(15);

	// 16. Track artist
	if (!tags->artist.empty())
		sqlInsert.BindText8(16, tags->artist);
	else
		sqlInsert.BindNull(16);

	// 17. Track album artist
	if (!tags->albumArtist.empty())
		sqlInsert.BindText8(17, tags->albumArtist);
	else
		sqlInsert.BindNull(17);

	// 18. Track composer
	if (!tags->composer.empty())
		sqlInsert.BindText8(18, tags->composer);
	else
		sqlInsert.BindNull(18);

	// 19. Track genre
	if (!tags->genre.empty())
		sqlInsert.BindText8(19, tags->genre);
	else
		sqlInsert.BindNull(19);

	// 20. Track release year
	if (!tags->year.empty())
		sqlInsert.BindText8(20, tags->year);
	else
		sqlInsert.BindNull(20);

	// 21. Track BPM
	if (!tags->bpm.empty())
		sqlInsert.BindText8(21, tags->bpm);
	else
		sqlInsert.BindNull(21);

	// 22. Track compilation
	if (!tags->compilation.empty())
		sqlInsert.BindText8(22, tags->compilation);
	else
		sqlInsert.BindNull(22);

	// 23. Track publisher
	if (!tags->publisher.empty())
		sqlInsert.BindText8(23, tags->publisher);
	else
		sqlInsert.BindNull(23);

	// 24. Track conductor
	if (!tags->conductor.empty())
		sqlInsert.BindText8(24, tags->conductor);
	else
		sqlInsert.BindNull(24);

	// 25. Track lyricist
	if (!tags->lyricist.empty())
		sqlInsert.BindText8(25, tags->lyricist);
	else
		sqlInsert.BindNull(25);

	// 26. Track remixer
	if (!tags->remixer.empty())
		sqlInsert.BindText8(26, tags->remixer);
	else
		sqlInsert.BindNull(26);

	// 27. Track grouping
	if (!tags->grouping.empty())
		sqlInsert.BindText8(27, tags->grouping);
	else
		sqlInsert.BindNull(27);

	// 28. Track subtitle
	if (!tags->subtitle.empty())
		sqlInsert.BindText8(28, tags->subtitle);
	else
		sqlInsert.BindNull(28);

	// 29. Track copyright
	if (!tags->copyright.empty())
		sqlInsert.BindText8(29, tags->copyright);
	else
		sqlInsert.BindNull(29);

	// 30. Track encoded by
	if (!tags->encodedby.empty())
		sqlInsert.BindText8(30, tags->encodedby);
	else
		sqlInsert.BindNull(30);

	// 31. Track comment
	if (!tags->comment.empty())
		sqlInsert.BindText8(31, tags->comment);
	else
		sqlInsert.BindNull(31);

	// 32. Track time length
	sqlInsert.BindInt(32, tags->duration);

	// 33. Track number of channels
	sqlInsert.BindInt(33, tags->channels);

	// 34. Track bitrate
	sqlInsert.BindInt(34, tags->bitrate);

	// 35. Track sample rate
	sqlInsert.BindInt(35, tags->samplerate);


	bool stepDone = sqlInsert.StepDone();

	libraryLastInsertID = 0;

	if (stepDone)
	{
		libraryLastInsertID = (long long)sqlite3_last_insert_rowid(dbLibrary.get());

		SQLRequest sqlDeleteM(dbLibrary,
			"DELETE FROM storage WHERE sid=?");
		sqlDeleteM.BindInt64(1, libraryLastInsertID);
		sqlDeleteM.Step();

		if (!tags->genres.empty() || !tags->artists.empty() || !tags->composers.empty() ||
			!tags->albumArtists.empty() || !tags->conductors.empty() || !tags->lyricists.empty())
		{
			SQLRequest sqlInsertM(dbLibrary,
				"INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

			InsertMultipleValues(libraryLastInsertID, sqlInsertM, tags);
		}
	}

	return stepDone;
}

bool DBase::AddFileToPlaylist(int index, DATABASE_SONGINFO* tags, bool isTemp)
{
	SQLRequest sqlInsert(!isTemp ? dbPlaylist : dbPlayTemp,
		"INSERT INTO playlist (idx,added,cue,filehash,path,file,filesize,modified,trackhash,track,totaltracks,"
		"disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,bpm,compilation,publisher,conductor,lyricist,"
		"remixer,grouping,subtitle,copyright,encodedby,comment,duration,channels,bitrate,samplerate)"
		" VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");


	// 1. Track index (for order in the playlist)
	sqlInsert.BindInt(1, index);

	// 2. Date time when added to the library
	assert(tags->added != 0);
	sqlInsert.BindInt64(2, tags->added);

	// 3. CUE
	if (tags->cue)
		sqlInsert.BindInt64(3, tags->cue);
	else
		sqlInsert.BindNull(3);

	// 4. File hash
	sqlInsert.BindInt(4, tags->fileHash);

	// 5. File path
	assert(!tags->path.empty());
	sqlInsert.BindText8(5, tags->path);

	// 6. File name
	assert(!tags->file.empty());
	sqlInsert.BindText8(6, tags->file);

	// 7. File size
	sqlInsert.BindInt64(7, tags->size);

	// 8. Date time when file last modified
	assert(tags->modified != 0);
	sqlInsert.BindInt64(8, tags->modified);

	// 9. Track hash
	sqlInsert.BindInt(9, tags->trackHash);

	// 10. Track number
	if (!tags->track.empty())
		sqlInsert.BindText8(10, tags->track);
	else
		sqlInsert.BindNull(10);

	// 11. Total tracks
	if (!tags->totalTracks.empty())
		sqlInsert.BindText8(11, tags->totalTracks);
	else
		sqlInsert.BindNull(11);

	// 12. Disc number
	if (!tags->disc.empty())
		sqlInsert.BindText8(12, tags->disc);
	else
		sqlInsert.BindNull(12);

	// 13. Total discs
	if (!tags->totalDiscs.empty())
		sqlInsert.BindText8(13, tags->totalDiscs);
	else
		sqlInsert.BindNull(13);

	// 14. Track title
	if (!tags->title.empty())
		sqlInsert.BindText8(14, tags->title);
	else
		sqlInsert.BindNull(14);

	// 15. Track album
	if (!tags->album.empty())
		sqlInsert.BindText8(15, tags->album);
	else
		sqlInsert.BindNull(15);

	// 16. Track artist
	if (!tags->artist.empty())
		sqlInsert.BindText8(16, tags->artist);
	else
		sqlInsert.BindNull(16);

	// 17. Track album artist
	if (!tags->albumArtist.empty())
		sqlInsert.BindText8(17, tags->albumArtist);
	else
		sqlInsert.BindNull(17);

	// 18. Track composer
	if (!tags->composer.empty())
		sqlInsert.BindText8(18, tags->composer);
	else
		sqlInsert.BindNull(18);

	// 19. Track genre
	if (!tags->genre.empty())
		sqlInsert.BindText8(19, tags->genre);
	else
		sqlInsert.BindNull(19);

	// 20. Track release year
	if (!tags->year.empty())
		sqlInsert.BindText8(20, tags->year);
	else
		sqlInsert.BindNull(20);

	// 21. Track BPM
	if (!tags->bpm.empty())
		sqlInsert.BindText8(21, tags->bpm);
	else
		sqlInsert.BindNull(21);

	// 22. Track compilation
	if (!tags->compilation.empty())
		sqlInsert.BindText8(22, tags->compilation);
	else
		sqlInsert.BindNull(22);

	// 23. Track publisher
	if (!tags->publisher.empty())
		sqlInsert.BindText8(23, tags->publisher);
	else
		sqlInsert.BindNull(23);

	// 24. Track conductor
	if (!tags->conductor.empty())
		sqlInsert.BindText8(24, tags->conductor);
	else
		sqlInsert.BindNull(24);

	// 25. Track lyricist
	if (!tags->lyricist.empty())
		sqlInsert.BindText8(25, tags->lyricist);
	else
		sqlInsert.BindNull(25);

	// 26. Track remixer
	if (!tags->remixer.empty())
		sqlInsert.BindText8(26, tags->remixer);
	else
		sqlInsert.BindNull(26);

	// 27. Track grouping
	if (!tags->grouping.empty())
		sqlInsert.BindText8(27, tags->grouping);
	else
		sqlInsert.BindNull(27);

	// 28. Track subtitle
	if (!tags->subtitle.empty())
		sqlInsert.BindText8(28, tags->subtitle);
	else
		sqlInsert.BindNull(28);

	// 29. Track copyright
	if (!tags->copyright.empty())
		sqlInsert.BindText8(29, tags->copyright);
	else
		sqlInsert.BindNull(29);

	// 30. Track encoded by
	if (!tags->encodedby.empty())
		sqlInsert.BindText8(30, tags->encodedby);
	else
		sqlInsert.BindNull(30);

	// 31. Track comment
	if (!tags->comment.empty())
		sqlInsert.BindText8(31, tags->comment);
	else
		sqlInsert.BindNull(31);

	// 32. Track time length
	sqlInsert.BindInt(32, tags->duration);

	// 33. Track number of channels
	sqlInsert.BindInt(33, tags->channels);

	// 34. Track bitrate
	sqlInsert.BindInt(34, tags->bitrate);

	// 35. Track sample rate
	sqlInsert.BindInt(35, tags->samplerate);


	bool stepDone = sqlInsert.StepDone();

	if (stepDone)
	{
		long long lastInsert = (long long)sqlite3_last_insert_rowid(!isTemp ? dbPlaylist.get() : dbPlayTemp.get());

		//SQLRequest sqlDeleteM(!isTemp ? dbPlaylist : dbPlayTemp,
		//	"DELETE FROM storage WHERE sid=?");
		//sqlDeleteM.BindInt64(1, lastInsert);
		//sqlDeleteM.Step();

		if (!tags->genres.empty() || !tags->artists.empty() || !tags->composers.empty() ||
			!tags->albumArtists.empty() || !tags->conductors.empty() || !tags->lyricists.empty())
		{
			SQLRequest sqlInsertM(!isTemp ? dbPlaylist : dbPlayTemp,
				"INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

			InsertMultipleValues(lastInsert, sqlInsertM, tags);
		}
	}

	return stepDone;
}

void DBase::InsertMultipleValues(long long id, SQLRequest& sqlInsert, DATABASE_SONGINFO* tags)
{
	sqlInsert.BindInt64(1, id);

	for (std::size_t i = 0, size = tags->artists.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 1);
		if (!tags->artists[i].empty())
			sqlInsert.BindText8(4, tags->artists[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
	for (std::size_t i = 0, size = tags->albumArtists.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 2);
		if (!tags->albumArtists[i].empty())
			sqlInsert.BindText8(4, tags->albumArtists[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
	for (std::size_t i = 0, size = tags->composers.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 3);
		if (!tags->composers[i].empty())
			sqlInsert.BindText8(4, tags->composers[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
	for (std::size_t i = 0, size = tags->genres.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 4);
		if (!tags->genres[i].empty())
			sqlInsert.BindText8(4, tags->genres[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
	for (std::size_t i = 0, size = tags->conductors.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 5);
		if (!tags->conductors[i].empty())
			sqlInsert.BindText8(4, tags->conductors[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
	for (std::size_t i = 0, size = tags->lyricists.size(); i < size; ++i)
	{
		sqlInsert.BindInt(2, i + 1);
		sqlInsert.BindInt(3, 6);
		if (!tags->lyricists[i].empty())
			sqlInsert.BindText8(4, tags->lyricists[i]);
		else
			sqlInsert.BindNull(4);
		sqlInsert.StepReset();
	}
}

void DBase::UpdateTagsModified(long long id, DATABASE_SONGINFO* tags)
{
	// Update tags because a file was modified
	// We skip "file" and "added" because the file was just updated.
	// Note: We do not change any ratings.

	SQLRequest sqlUpdate(dbLibrary,
		"UPDATE library SET flag=?,cue=?,filesize=?,modified=?,trackhash=?,track=?,totaltracks=?,disc=?,totaldiscs=?,"
		"title=?,album=?,artist=?,albumartist=?,composer=?,genre=?,year=?,bpm=?,compilation=?,publisher=?,conductor=?,lyricist=?,"
		"remixer=?,grouping=?,subtitle=?,copyright=?,encodedby=?,comment=?,duration=?,channels=?,bitrate=?,samplerate=? WHERE id=?;");

	// 17. Track ID which we update
	sqlUpdate.BindInt64(32, id);

	// 1. Update flag
	sqlUpdate.BindNull(1);

	// 2. CUE
	if (tags->cue)
		sqlUpdate.BindInt64(2, tags->cue);
	else
		sqlUpdate.BindNull(2);

	// 3. File size
	sqlUpdate.BindInt64(3, tags->size);

	// 4. Date time when file last modified
	assert(tags->modified != 0);
	sqlUpdate.BindInt64(4, tags->modified);

	// 5. Track hash
	sqlUpdate.BindInt(5, tags->trackHash);

	// 6. Track number in album
	if (!tags->track.empty())
		sqlUpdate.BindText8(6, tags->track);
	else
		sqlUpdate.BindNull(6);

	// 7. Total tracks in album
	if (!tags->totalTracks.empty())
		sqlUpdate.BindText8(7, tags->totalTracks);
	else
		sqlUpdate.BindNull(7);

	// 8. Disc number
	if (!tags->disc.empty())
		sqlUpdate.BindText8(8, tags->disc);
	else
		sqlUpdate.BindNull(8);

	// 9. Total discs
	if (!tags->totalDiscs.empty())
		sqlUpdate.BindText8(9, tags->totalDiscs);
	else
		sqlUpdate.BindNull(9);

	// 10. Track title
	if (!tags->title.empty())
		sqlUpdate.BindText8(10, tags->title);
	else
		sqlUpdate.BindNull(10);

	// 11. Track album
	if (!tags->album.empty())
		sqlUpdate.BindText8(11, tags->album);
	else
		sqlUpdate.BindNull(11);

	// 12. Track artist
	if (!tags->artist.empty())
		sqlUpdate.BindText8(12, tags->artist);
	else
		sqlUpdate.BindNull(12);

	// 13. Track album artist
	if (!tags->albumArtist.empty())
		sqlUpdate.BindText8(13, tags->albumArtist);
	else
		sqlUpdate.BindNull(13);

	// 14. Track composer
	if (!tags->composer.empty())
		sqlUpdate.BindText8(14, tags->composer);
	else
		sqlUpdate.BindNull(14);

	// 15. Track genre
	if (!tags->genre.empty())
		sqlUpdate.BindText8(15, tags->genre);
	else
		sqlUpdate.BindNull(15);

	// 16. Track release year
	if (!tags->year.empty())
		sqlUpdate.BindText8(16, tags->year);
	else
		sqlUpdate.BindNull(16);

	// 17. Track BPM
	if (!tags->bpm.empty())
		sqlUpdate.BindText8(17, tags->bpm);
	else
		sqlUpdate.BindNull(17);

	// 18. Track compilation
	if (!tags->bpm.empty())
		sqlUpdate.BindText8(18, tags->bpm);
	else
		sqlUpdate.BindNull(18);

	// 19. Track publisher
	if (!tags->publisher.empty())
		sqlUpdate.BindText8(19, tags->publisher);
	else
		sqlUpdate.BindNull(19);

	// 20. Track conductor
	if (!tags->conductor.empty())
		sqlUpdate.BindText8(20, tags->conductor);
	else
		sqlUpdate.BindNull(20);

	// 21. Track lyricist
	if (!tags->lyricist.empty())
		sqlUpdate.BindText8(21, tags->lyricist);
	else
		sqlUpdate.BindNull(21);

	// 22. Track remixer
	if (!tags->remixer.empty())
		sqlUpdate.BindText8(22, tags->remixer);
	else
		sqlUpdate.BindNull(22);

	// 23. Track grouping
	if (!tags->grouping.empty())
		sqlUpdate.BindText8(23, tags->grouping);
	else
		sqlUpdate.BindNull(23);

	// 24. Track subtitle
	if (!tags->subtitle.empty())
		sqlUpdate.BindText8(24, tags->subtitle);
	else
		sqlUpdate.BindNull(24);

	// 25. Track copyright
	if (!tags->copyright.empty())
		sqlUpdate.BindText8(25, tags->copyright);
	else
		sqlUpdate.BindNull(25);

	// 26. Track encoded by
	if (!tags->encodedby.empty())
		sqlUpdate.BindText8(26, tags->encodedby);
	else
		sqlUpdate.BindNull(26);

	// 27. Track comment
	if (!tags->comment.empty())
		sqlUpdate.BindText8(27, tags->comment);
	else
		sqlUpdate.BindNull(27);


	// 28. Track time length
	sqlUpdate.BindInt(28, tags->duration);

	// 29. Track number of channels
	sqlUpdate.BindInt(29, tags->channels);

	// 30. Track bitrate
	sqlUpdate.BindInt(30, tags->bitrate);

	// 31. Track sample rate
	sqlUpdate.BindInt(31, tags->samplerate);


	bool stepDone = sqlUpdate.StepDone();

	if (stepDone)
	{
		long long lastInsert = (long long)sqlite3_last_insert_rowid(dbLibrary.get());

		SQLRequest sqlDeleteM(dbLibrary,
			"DELETE FROM storage WHERE sid=?");
		sqlDeleteM.BindInt64(1, lastInsert);
		sqlDeleteM.Step();

		if (!tags->genres.empty() || !tags->artists.empty() || !tags->composers.empty() ||
			!tags->albumArtists.empty() || !tags->conductors.empty() || !tags->lyricists.empty())
		{
			SQLRequest sqlInsertM(dbLibrary,
				"INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

			InsertMultipleValues(lastInsert, sqlInsertM, tags);
		}
	}
}

void DBase::UpdateTagsFileMove(long long id, DATABASE_SONGINFO* tags)
{
	// Update tags because a file was moved
	// Note: We do not change any ratings.

	SQLRequest sqlUpdate(dbLibrary,
		"UPDATE library SET flag=?,added=?,cue=?,filehash=?,path=?,file=?,filesize=?,modified=?,trackhash=?,track=?,totaltracks=?,disc=?,totaldiscs=?,"
		"title=?,album=?,artist=?,albumartist=?,composer=?,genre=?,year=?,bpm=?,compilation=?,publisher=?,conductor=?,lyricist=?,"
		"remixer=?,grouping=?,subtitle=?,copyright=?,encodedby=?,comment=?,duration=?,channels=?,bitrate=?,samplerate=? WHERE id=?;");


	// 26. Track ID which we update
	sqlUpdate.BindInt64(36, id);

	// 1. Update flag
	sqlUpdate.BindNull(1);

	// 2. Date time when added to the library
	assert(tags->added != 0);
	sqlUpdate.BindInt64(2, tags->added);

	// 3. CUE
	sqlUpdate.BindNull(3);

	// 4. File hash
	sqlUpdate.BindInt(4, tags->fileHash);

	// 5. File path
	assert(!tags->path.empty());
	sqlUpdate.BindText8(5, tags->path);

	// 6. File name
	assert(!tags->file.empty());
	sqlUpdate.BindText8(6, tags->file);

	// 7. File size
	sqlUpdate.BindInt64(7, tags->size);

	// 8. Date time when file last modified
	assert(tags->modified != 0);
	sqlUpdate.BindInt64(8, tags->modified);

	// 9. Track hash
	sqlUpdate.BindInt(9, tags->trackHash);

	// 10. Track number in album
	if (!tags->track.empty())
		sqlUpdate.BindText8(10, tags->track);
	else
		sqlUpdate.BindNull(10);

	// 11. Total tracks in album
	if (!tags->totalTracks.empty())
		sqlUpdate.BindText8(11, tags->totalTracks);
	else
		sqlUpdate.BindNull(11);

	// 12. Disc number
	if (!tags->disc.empty())
		sqlUpdate.BindText8(12, tags->disc);
	else
		sqlUpdate.BindNull(12);

	// 13. Total discs
	if (!tags->totalDiscs.empty())
		sqlUpdate.BindText8(13, tags->totalDiscs);
	else
		sqlUpdate.BindNull(13);

	// 14. Track title
	if (!tags->title.empty())
		sqlUpdate.BindText8(14, tags->title);
	else
		sqlUpdate.BindNull(14);

	// 15. Track album
	if (!tags->album.empty())
		sqlUpdate.BindText8(15, tags->album);
	else
		sqlUpdate.BindNull(15);

	// 16. Track artist
	if (!tags->artist.empty())
		sqlUpdate.BindText8(16, tags->artist);
	else
		sqlUpdate.BindNull(16);

	// 17. Track album artist
	if (!tags->albumArtist.empty())
		sqlUpdate.BindText8(17, tags->albumArtist);
	else
		sqlUpdate.BindNull(17);

	// 18. Track composer
	if (!tags->composer.empty())
		sqlUpdate.BindText8(18, tags->composer);
	else
		sqlUpdate.BindNull(18);

	// 19. Track genre
	if (!tags->genre.empty())
		sqlUpdate.BindText8(19, tags->genre);
	else
		sqlUpdate.BindNull(19);

	// 20. Track release year
	if (!tags->year.empty())
		sqlUpdate.BindText8(20, tags->year);
	else
		sqlUpdate.BindNull(20);

	// 21. Track BPM
	if (!tags->bpm.empty())
		sqlUpdate.BindText8(21, tags->bpm);
	else
		sqlUpdate.BindNull(21);

	// 22. Track compilation
	if (!tags->compilation.empty())
		sqlUpdate.BindText8(22, tags->compilation);
	else
		sqlUpdate.BindNull(22);

	// 23. Track publisher
	if (!tags->publisher.empty())
		sqlUpdate.BindText8(23, tags->publisher);
	else
		sqlUpdate.BindNull(23);

	// 24. Track conductor
	if (!tags->conductor.empty())
		sqlUpdate.BindText8(24, tags->conductor);
	else
		sqlUpdate.BindNull(24);

	// 25. Track lyricist
	if (!tags->lyricist.empty())
		sqlUpdate.BindText8(25, tags->lyricist);
	else
		sqlUpdate.BindNull(25);

	// 26. Track remixer
	if (!tags->remixer.empty())
		sqlUpdate.BindText8(26, tags->remixer);
	else
		sqlUpdate.BindNull(26);

	// 27 Track grouping
	if (!tags->grouping.empty())
		sqlUpdate.BindText8(27, tags->grouping);
	else
		sqlUpdate.BindNull(27);

	// 28. Track subtitle
	if (!tags->subtitle.empty())
		sqlUpdate.BindText8(28, tags->subtitle);
	else
		sqlUpdate.BindNull(28);

	// 29. Track copyright
	if (!tags->copyright.empty())
		sqlUpdate.BindText8(29, tags->copyright);
	else
		sqlUpdate.BindNull(29);

	// 30. Track encoded by
	if (!tags->encodedby.empty())
		sqlUpdate.BindText8(30, tags->encodedby);
	else
		sqlUpdate.BindNull(30);

	// 31. Track comment
	if (!tags->comment.empty())
		sqlUpdate.BindText8(31, tags->comment);
	else
		sqlUpdate.BindNull(31);


	// 32. Track time length
	sqlUpdate.BindInt(32, tags->duration);

	// 33. Track number of channels
	sqlUpdate.BindInt(33, tags->channels);

	// 34. Track bitrate
	sqlUpdate.BindInt(34, tags->bitrate);

	// 35. Track sample rate
	sqlUpdate.BindInt(35, tags->samplerate);
	
	
	bool stepDone = sqlUpdate.StepDone();

	if (stepDone)
	{
		long long lastInsert = (long long)sqlite3_last_insert_rowid(dbLibrary.get());

		SQLRequest sqlDeleteM(dbLibrary,
			"DELETE FROM storage WHERE sid=?");
		sqlDeleteM.BindInt64(1, lastInsert);
		sqlDeleteM.Step();

		if (!tags->genres.empty() || !tags->artists.empty() || !tags->composers.empty() ||
			!tags->albumArtists.empty() || !tags->conductors.empty() || !tags->lyricists.empty())
		{
			SQLRequest sqlInsertM(dbLibrary,
				"INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

			InsertMultipleValues(lastInsert, sqlInsertM, tags);
		}
	}
}

void DBase::SetUpdateCueOK(long long id)
{
	SQLRequest sqlUpdate(dbCue,
		"UPDATE memflag SET fflag=NULL WHERE fid=? AND fflag=1;");

	sqlUpdate.BindInt64(1, id);
	sqlUpdate.Step();
}

void DBase::AddCueFile(bool noDrive, const std::wstring& path, const std::wstring& file,
	int hash, long long size, long long modified, const std::wstring& refFile, int refHash)
{
	SQLRequest sqlInsert(dbCue,
		"INSERT INTO cue (filehash,path,file,filesize,modified,refhash,reffile) VALUES (?,?,?,?,?,?,?);");

	sqlInsert.BindInt(1, hash);
	if (!noDrive)
		sqlInsert.BindText16(2, path);
	else
	{
		std::wstring newpath = path;
		newpath[0] = '?';
		sqlInsert.BindText16(2, newpath);
	}
	sqlInsert.BindText16(3, file);
	sqlInsert.BindInt64(4, size);
	sqlInsert.BindInt64(5, modified);
	if (!refFile.empty())
	{
		sqlInsert.BindInt(6, refHash);
		sqlInsert.BindText16(7, refFile);
	}
	else
	{
		sqlInsert.BindNull(6);
		sqlInsert.BindNull(7);
	}

	sqlInsert.Step();
}

void DBase::UpdateCueFile(long long id, long long size, long long modified, const std::wstring& refFile, int refHash)
{
	SQLRequest sqlUpdate(dbCue,
		"UPDATE cue SET filesize=?,modified=?,refhash=?,reffile=? WHERE id=?;");

	sqlUpdate.BindInt64(5, id);
	sqlUpdate.BindInt64(1, size);
	sqlUpdate.BindInt64(2, modified);
	if (!refFile.empty())
	{
		sqlUpdate.BindInt(3, refHash);
		sqlUpdate.BindText16(4, refFile);
	}
	else
	{
		sqlUpdate.BindNull(3);
		sqlUpdate.BindNull(4);
	}

	sqlUpdate.Step();
}

bool DBase::UpdateCueLibrary(bool noDrive, const std::wstring& path, const std::wstring& file, int hash)
{
	// Update library
	SQLRequest sqlSelectLib(dbLibrary,
		"SELECT id,cue FROM library WHERE filehash=? AND file=? COLLATE FILECASE AND path=? COLLATE FILECASE;");

	sqlSelectLib.BindInt(1, hash);
	sqlSelectLib.BindText16(2, file);
	if (!noDrive)
		sqlSelectLib.BindText16(3, path);
	else
	{
		std::wstring newpath = path;
		newpath[0] = '?';
		sqlSelectLib.BindText16(3, newpath);
	}

	bool result = false;

	while (sqlSelectLib.StepRow())
	{
		result = true;

		long long id = sqlSelectLib.ColumnInt64(0);
		SetUpdateOK(id);

		// DeleteCueImage do the same job, but to make sure also use this
		if (sqlSelectLib.ColumnIsNull(1))
		{
			SQLRequest sqlDeleteLib(dbLibrary,
				"DELETE FROM library WHERE id=?;");

			sqlDeleteLib.BindInt64(1, id);
			sqlDeleteLib.Step();
		}
	}

	return result;
}

bool DBase::DeleteCueImage(bool noDrive, const std::wstring& path, const std::wstring& file, int hash)
{
	SQLRequest sqlDelete(dbLibrary,
		"DELETE FROM library WHERE filehash=? AND cue IS NULL AND file=? COLLATE FILECASE AND path=? COLLATE FILECASE;");

	sqlDelete.BindInt(1, hash);
	sqlDelete.BindText16(2, file);
	if (!noDrive)
		sqlDelete.BindText16(3, path);
	else
	{
		std::wstring newpath = path;
		newpath[0] = '?';
		sqlDelete.BindText16(3, newpath);
	}

	return sqlDelete.StepDone();
}

bool DBase::CheckCueFile(bool noDrive, const std::wstring& path, const std::wstring& file,
	int hash, long long size, long long modified, long long& outID, bool flag, bool isRescanAll)
{
	SQLRequest sqlSelect(dbCue,
		"SELECT id,filesize,modified,refhash,reffile FROM cue WHERE filehash=? AND file=? COLLATE FILECASE AND path=? COLLATE FILECASE LIMIT 1;");

	sqlSelect.BindInt(1, hash);
	sqlSelect.BindText16(2, file);
	if (!noDrive)
		sqlSelect.BindText16(3, path);
	else
	{
		std::wstring newpath = path;
		newpath[0] = '?';
		sqlSelect.BindText16(3, newpath);
	}

	if (sqlSelect.StepRow())
	{
		outID = sqlSelect.ColumnInt64(0);
		long long outSize = sqlSelect.ColumnInt64(1);
		long long outModified = sqlSelect.ColumnInt64(2);
		int outRefHash = sqlSelect.ColumnInt(3);
		std::wstring outRefFile = sqlSelect.ColumnText16(4);

		if (flag)
			SetUpdateCueOK(outID);

		if (!isRescanAll && size == outSize && modified == outModified)
		{
			if (flag)
				UpdateCueLibrary(noDrive, path, outRefFile, outRefHash);

			return true;
		}
	}

	return false;
}

bool DBase::CheckFile(bool noDrive, const std::wstring& path, const std::wstring& file,
	int hash, long long cue, long long& outID, long long& outModified, long long& outSize, bool &outCue, bool flag)
{
	const char* select = nullptr;

	if (cue == 0)
		select = "SELECT id,cue,filesize,modified FROM library WHERE filehash=?1 AND file=?2 COLLATE FILECASE AND path=?3 COLLATE FILECASE LIMIT 1;";
	else
		select = "SELECT id,cue,filesize,modified FROM library WHERE filehash=?1 AND cue=?4 AND file=?2 COLLATE FILECASE AND path=?3 COLLATE FILECASE LIMIT 1;";

	SQLRequest sqlSelect(dbLibrary, select);

	sqlSelect.BindInt(1, hash);
	sqlSelect.BindText16(2, file);
	if (!noDrive)
		sqlSelect.BindText16(3, path);
	else
	{
		std::wstring newpath = path;
		newpath[0] = '?';
		sqlSelect.BindText16(3, newpath);
	}
	if (cue)
		sqlSelect.BindInt64(4, cue);

	if (sqlSelect.StepRow())
	{
		outID = sqlSelect.ColumnInt64(0);
		outCue = !sqlSelect.ColumnIsNull(1);
		outSize = sqlSelect.ColumnInt64(2);
		outModified = sqlSelect.ColumnInt64(3);

		return true;
	}

	return false;
}

bool DBase::CheckTags(long long& outID, DATABASE_SONGINFO* tags)
{
	const char* select = nullptr;

	if (!tags->title.empty())
		select = "SELECT id FROM library,memflag WHERE trackhash=? AND id=fid AND fflag=1 AND title IS ? AND album IS ? AND artist IS ? AND year IS ? AND track IS ? AND disc IS ? LIMIT 1;";
	else
		select = "SELECT id FROM library,memflag WHERE trackhash=? AND id=fid AND fflag=1 AND file IS ? AND album IS ? AND artist IS ? AND year IS ? AND track IS ? AND disc IS ? LIMIT 1;";

	SQLRequest sqlSelect(dbLibrary, select);

	sqlSelect.BindInt(1, tags->trackHash);

	if (!tags->title.empty())
		sqlSelect.BindText8(2, tags->title);
	else
		sqlSelect.BindText8(2, tags->file);

	if (!tags->album.empty())
		sqlSelect.BindText8(3, tags->album);

	if (!tags->artist.empty())
		sqlSelect.BindText8(4, tags->artist);

	if (!tags->year.empty())
		sqlSelect.BindText8(5, tags->year);

	if (!tags->track.empty())
		sqlSelect.BindText8(6, tags->track);

	if (!tags->disc.empty())
		sqlSelect.BindText8(7, tags->disc);

	if (sqlSelect.StepRow())
	{
		outID = sqlSelect.ColumnInt64(0);
		return true;
	}

	return false;
}

long long DBase::GetLastAddedToLib()
{
	return libraryLastInsertID;
}

int DBase::GetCountLibFromPls()
{
	SQLRequest sqlSelect(dbLibrary,
		"SELECT COUNT(*) FROM memflag WHERE fflag=1;");

	sqlSelect.Step();

	return sqlSelect.ColumnInt(0);
}

void DBase::FindFirstLibFromPlsFile(SQLRequest& sqlSelect)
{
	sqlSelect.Prepare(dbLibrary, "SELECT id,cue,path,file,filesize,modified FROM library,memflag WHERE id=fid AND fflag=1 ORDER BY id;");
}

bool DBase::FindNextLibFromPlsFile(SQLRequest& sqlSelect, long long& outID,
	std::wstring& outPath, std::wstring& outFile, long long& outModified, long long& outSize, bool& outCue)
{
	if (sqlSelect.StepRow())
	{
		outID = sqlSelect.ColumnInt64(0);
		outCue = !sqlSelect.ColumnIsNull(1);
		outPath = sqlSelect.ColumnText16(2);
		outFile = sqlSelect.ColumnText16(3);
		outSize = sqlSelect.ColumnInt64(4);
		outModified = sqlSelect.ColumnInt64(5);

		return true;
	}

	return false;
}

int DBase::GetCueCountLibFromPls()
{
	SQLRequest sqlSelect(dbCue,
		"SELECT COUNT(*) FROM memflag WHERE fflag=1;");

	sqlSelect.Step();

	return sqlSelect.ColumnInt(0);
}

void DBase::FindFirstCueLibFromPlsFile(SQLRequest& sqlSelect)
{
	sqlSelect.Prepare(dbCue, "SELECT id,path,file,filesize,modified,reffile,refhash FROM cue,memflag WHERE id=fid AND fflag=1 ORDER BY id;");
}

bool DBase::FindNextCueLibFromPlsFile(SQLRequest& sqlSelect, long long& outID, std::wstring& outPath, std::wstring& outFile,
	long long& outModified, long long& outSize, std::wstring& outRefFile, int& outRefHash)
{
	if (sqlSelect.StepRow())
	{
		outID = sqlSelect.ColumnInt64(0);
		outPath = sqlSelect.ColumnText16(1);
		outFile = sqlSelect.ColumnText16(2);
		outSize = sqlSelect.ColumnInt64(3);
		outModified = sqlSelect.ColumnInt64(4);
		outRefFile = sqlSelect.ColumnText16(5);
		outRefHash = sqlSelect.ColumnInt(6);

		return true;
	}

	return false;
}

void DBase::MemFlagAttach()
{
	SQLRequest::Exec(dbLibrary, "ATTACH DATABASE ':memory:' AS memory_db;");
	SQLRequest::Exec(dbCue, "ATTACH DATABASE ':memory:' AS memory_db;");
}

void DBase::MemFlagDetach()
{
	SQLRequest::Exec(dbLibrary, "DETACH DATABASE memory_db;");
	SQLRequest::Exec(dbCue, "DETACH DATABASE memory_db;");
}

void DBase::SetUpdateAll()
{
	// Previously I used flag column for this, but it very slow when update flag for entire database
	// from other thread (for 60000 tracks, 4 sec from main thread, and 40+ sec! from other). So now I use temp table.
	//SQLRequest::Exec(dbLibrary, "UPDATE library SET flag=1 WHERE flag IS NULL;");

	SQLRequest::Exec(dbLibrary, "CREATE TABLE memory_db.memflag (fpk INTEGER PRIMARY KEY,fid INTEGER,fflag INTEGER);");

	// With database lock
	SQLRequest::Exec(dbLibrary, "INSERT INTO memflag (fid,fflag) SELECT id,1 FROM library;");

	// Without database lock (test)
	//SQLRequest sqlSelect(dbLibrary, "SELECT id FROM library;");
	//SQLRequest sqlInsert(dbLibrary, "INSERT INTO memflag (fid,fflag) VALUES (?,?);");
	//while (sqlSelect.StepRow())
	//{
	//	sqlInsert.BindInt64(1, sqlSelect.ColumnInt64(0));
	//	sqlInsert.BindInt(2, 1);
	//	sqlInsert.StepReset();
	//}

	SQLRequest::Exec(dbLibrary, "CREATE INDEX memory_db.memflag_index ON memflag(fid);");
}

void DBase::SetUpdateEnd()
{
	SQLRequest::Exec(dbLibrary, "DROP TABLE IF EXISTS memflag;");
}

void DBase::SetUpdateAllCue()
{
	SQLRequest::Exec(dbCue, "CREATE TABLE memory_db.memflag (fpk INTEGER PRIMARY KEY,fid INTEGER,fflag INTEGER);");

	SQLRequest::Exec(dbCue, "INSERT INTO memflag (fid,fflag) SELECT id,1 FROM cue;");

	SQLRequest::Exec(dbCue, "CREATE INDEX memory_db.memflag_index ON memflag(fid);");
}

void DBase::SetUpdateEndCue()
{
	SQLRequest::Exec(dbCue, "DROP TABLE IF EXISTS memflag;");
}

void DBase::SetUpdateOK(long long id)
{
	SQLRequest sqlUpdate(dbLibrary,
		"UPDATE memflag SET fflag=NULL WHERE fid=? AND fflag=1;");

	sqlUpdate.BindInt64(1, id);

	sqlUpdate.Step();
}

void DBase::DeleteNotUpdated()
{
	// It seems SQLite is extremely slow when deleting massive data with indexes
	// and again from other thread. With my tests delete 60000 tracks takes 3+ min, but 20000 only 4 sec
	// It looks like a bug in SQLite. We can drop and recreate indexes here to avoid this, but I don't like it.
	// ---
	// It seems PRAGMA cache_spill=OFF can help, but it increases memory usage of commits, so don't use it.
	// OK we can just increase cache to avoid this, use PRAGMA cache_size = 10000.
	// ---
	// It seems all the above is not actual for new versions of SQLite, but I'll leave it for history.

	// With database lock
	SQLRequest::Exec(dbLibrary, "DELETE FROM library WHERE id IN (SELECT fid FROM memflag WHERE fflag=1);");
	
	// Without database lock (test)
	//SQLRequest sqlSelect(dbLibrary, "SELECT fid FROM memflag WHERE fflag=1;");
	//SQLRequest sqlDelete(dbLibrary, "DELETE FROM library WHERE id=?;");
	//while (sqlSelect.StepRow())
	//{
	//	sqlDelete.BindInt64(1, sqlSelect.ColumnInt64(0));
	//	sqlDelete.StepReset();
	//}
}

void DBase::DeleteNotUpdatedCue()
{
	SQLRequest::Exec(dbCue, "DELETE FROM cue WHERE id IN (SELECT fid FROM memflag WHERE fflag=1);");
}

void DBase::RestoreDeleted()
{
	// With database lock
	SQLRequest::Exec(dbLibrary, "UPDATE library SET deleted=NULL WHERE deleted=1;");

	// Without database lock (test)
	//SQLRequest sqlSelect(dbLibrary, "SELECT id FROM library WHERE deleted=1;");
	//SQLRequest sqlUpdate(dbLibrary, "UPDATE library SET deleted=NULL WHERE id=?;");
	//while (sqlSelect.StepRow())
	//{
	//	sqlUpdate.BindInt64(1, sqlSelect.ColumnInt64(0));
	//	sqlUpdate.StepReset();
	//}
}

void DBase::DeleteLibFromPls()
{
	// With database lock
	SQLRequest::Exec(dbLibrary, "DELETE FROM library WHERE id IN (SELECT fid FROM memflag WHERE fflag=1) AND deleted=1;");

	// Without database lock (test)
	//SQLRequest sqlSelect(dbLibrary, "SELECT fid FROM memflag WHERE fflag=1;");
	//SQLRequest sqlDelete(dbLibrary, "DELETE FROM library WHERE id=? AND deleted=1;");
	//while (sqlSelect.StepRow())
	//{
	//	sqlDelete.BindInt64(1, sqlSelect.ColumnInt64(0));
	//	sqlDelete.StepReset();
	//}
}

bool DBase::IsLibraryEmpty()
{
	SQLRequest sqlSelect(dbLibrary,
		"SELECT id FROM library;");

	if (sqlSelect.StepRow())
		return false;

	return true;
}

void DBase::Begin()
{
	SQLRequest::Exec(dbLibrary, "BEGIN;");
}

void DBase::Commit()
{
	SQLRequest::Exec(dbLibrary, "COMMIT;");
}

void DBase::Vacuum()
{
	SQLRequest::Exec(dbLibrary, "VACUUM;");
}

void DBase::PlayBegin()
{
	SQLRequest::Exec(dbPlaylist, "BEGIN;");
}

void DBase::PlayCommit()
{
	SQLRequest::Exec(dbPlaylist, "COMMIT;");
}

void DBase::PlayVacuum()
{
	SQLRequest::Exec(dbPlaylist, "VACUUM;");
}

void DBase::CueBegin()
{
	SQLRequest::Exec(dbCue, "BEGIN;");
}

void DBase::CueCommit()
{
	SQLRequest::Exec(dbCue, "COMMIT;");
}

void DBase::SetRating(long long idLibrary, long long idPlaylist, int rating, bool isPlay)
{
	// Adjust the rating
	rating = std::max(0, std::min(5, rating));
	rating *= 20;

	if (idLibrary && dbLibrary) // Set rating in the library
	{
		SQLRequest sqlUpdate(dbLibrary,
			"UPDATE library SET rating=? WHERE id=?;");

		sqlUpdate.BindInt(1, rating);
		sqlUpdate.BindInt64(2, idLibrary);

		sqlUpdate.Step();
	}
	else if (idPlaylist) // Set rating in the playlist
	{
		if (isPlay && dbPlayOpen) // In the playing playlist
		{
			SQLRequest sqlUpdate(dbPlayOpen,
				"UPDATE playlist SET rating=? WHERE id=?;");

			sqlUpdate.BindInt(1, rating);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}
		else if (dbPlaylist) // In the current playlist
		{
			SQLRequest sqlUpdate(dbPlaylist,
				"UPDATE playlist SET rating=? WHERE id=?;");

			sqlUpdate.BindInt(1, rating);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}
	}
}

void DBase::IncreaseCount(long long idLibrary, long long idPlaylist)
{
	long long timeNow = FileSystem::GetTimeNow();

	if (idLibrary && dbLibrary) // Increase play count in the library
	{
		SQLRequest sqlUpdate(dbLibrary,
			"UPDATE library SET playcount=IFNULL(playcount,0)+1,lastplayed=? WHERE id=?;");

		sqlUpdate.BindInt64(1, timeNow);
		sqlUpdate.BindInt64(2, idLibrary);

		sqlUpdate.Step();
	}
	else if (idPlaylist) // Increase play count in the playlist
	{
		if (dbPlayOpen) // In the playing playlist
		{
			SQLRequest sqlUpdate(dbPlayOpen,
				"UPDATE playlist SET playcount=IFNULL(playcount,0)+1,lastplayed=? WHERE id=?;");

			sqlUpdate.BindInt64(1, timeNow);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}
		/*else if (dbPlaylist) // In the current playlist
		{
			SQLRequest sqlUpdate(dbPlaylist,
				"UPDATE playlist SET playcount=IFNULL(playcount,0)+1,lastplayed=? WHERE id=?;");

			sqlUpdate.BindInt64(1, timeNow);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}*/
	}
}

void DBase::IncreaseSkip(long long idLibrary, long long idPlaylist)
{
	long long timeNow = FileSystem::GetTimeNow();

	if (idLibrary && dbLibrary) // Increase skip count in the library
	{
		SQLRequest sqlUpdate(dbLibrary,
			"UPDATE library SET skipcount=IFNULL(skipcount,0)+1,lastskipped=? WHERE id=?;");

		sqlUpdate.BindInt64(1, timeNow);
		sqlUpdate.BindInt64(2, idLibrary);

		sqlUpdate.Step();
	}
	else if (idPlaylist) // Increase skip count in the playlist
	{
		if (dbPlayOpen) // In the playing playlist
		{
			SQLRequest sqlUpdate(dbPlayOpen,
				"UPDATE playlist SET skipcount=IFNULL(skipcount,0)+1,lastskipped=? WHERE id=?;");

			sqlUpdate.BindInt64(1, timeNow);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}
		/*else if (dbPlaylist) // In the current playlist
		{
			SQLRequest sqlUpdate(dbPlaylist,
				"UPDATE playlist SET skipcount=IFNULL(skipcount,0)+1,lastskipped=? WHERE id=?;");

			sqlUpdate.BindInt64(1, timeNow);
			sqlUpdate.BindInt64(2, idPlaylist);

			sqlUpdate.Step();
		}*/
	}
}

void DBase::GetSongTags(long long idLibrary, long long idPlaylist, std::wstring& file, std::wstring& title,
						std::wstring& album, std::wstring& artist, std::wstring &genre, std::wstring& year, bool isPlay)
{
	SQLRequest sqlSelect;

	if (idLibrary && dbLibrary) // Tags from the library
	{
		sqlSelect.Prepare(dbLibrary,
			"SELECT file,title,album,IFNULL(artist,albumartist),genre,CAST(year AS INTEGER) FROM library WHERE id=?;");

		sqlSelect.BindInt64(1, idLibrary);
	}
	else if (idPlaylist) // Tags from the playlist
	{
		if (isPlay && dbPlayOpen)
		{
			sqlSelect.Prepare(dbPlayOpen,
				"SELECT file,title,album,IFNULL(artist,albumartist),genre,CAST(year AS INTEGER) FROM playlist WHERE id=?;");

			sqlSelect.BindInt64(1, idPlaylist);
		}
		else if (dbPlaylist)
		{
			sqlSelect.Prepare(dbPlaylist,
				"SELECT file,title,album,IFNULL(artist,albumartist),genre,CAST(year AS INTEGER) FROM playlist WHERE id=?;");

			sqlSelect.BindInt64(1, idPlaylist);
		}
	}

	if (sqlSelect.IsPrepared())
	{
		if (sqlSelect.StepRow())
		{
			file   = sqlSelect.ColumnText16(0);
			title  = sqlSelect.ColumnText16(1);
			album  = sqlSelect.ColumnText16(2);
			artist = sqlSelect.ColumnText16(3);
			genre  = sqlSelect.ColumnText16(4);
			year   = sqlSelect.ColumnText16(5);
		}
	}
}

void DBase::FillTreeAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		//"SELECT album FROM library WHERE deleted IS NULL GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");
		"SELECT DISTINCT album COLLATE MYCASE FROM library WHERE deleted IS NULL ORDER BY album COLLATE MYCASE;");

	FillTreeNode(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		//"SELECT artist FROM library WHERE deleted IS NULL GROUP BY artist COLLATE MYCASE ORDER BY artist COLLATE MYCASE;");
		//"SELECT DISTINCT IFNULL(albumartist,artist) COLLATE MYCASE FROM library WHERE deleted IS NULL ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE;");
		"SELECT art FROM ("
		"SELECT IFNULL(albumartist,artist) AS art FROM library WHERE deleted IS NULL"
		" UNION ALL "
		"SELECT svalue AS art FROM storage,library WHERE skey IN (1,2) AND sid=id AND deleted IS NULL"
		" AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END"
		") GROUP BY art COLLATE MYCASE ORDER BY art COLLATE MYCASE;");

	FillTreeNode(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeComposer(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT cmp FROM ("
		"SELECT composer AS cmp FROM library WHERE deleted IS NULL"
		" UNION ALL "
		"SELECT svalue AS cmp FROM storage,library WHERE skey=3 AND sid=id AND deleted IS NULL"
		") GROUP BY cmp COLLATE MYCASE ORDER BY cmp COLLATE MYCASE;");

	FillTreeNode(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeGenre(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		//"SELECT genre FROM library WHERE deleted IS NULL GROUP BY genre COLLATE MYCASE ORDER BY genre COLLATE MYCASE;");
		//"SELECT DISTINCT genre COLLATE MYCASE FROM library WHERE deleted IS NULL ORDER BY genre COLLATE MYCASE;");
		"SELECT gen FROM ("
		"SELECT genre AS gen FROM library WHERE deleted IS NULL"
		" UNION ALL "
		"SELECT svalue AS gen FROM storage,library WHERE skey=4 AND sid=id AND deleted IS NULL"
		") GROUP BY gen COLLATE MYCASE ORDER BY gen COLLATE MYCASE;");

	FillTreeNode(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeYear(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		//"SELECT year FROM library WHERE deleted IS NULL GROUP BY year COLLATE MYCASE ORDER BY year DESC;");
		"SELECT DISTINCT CAST(year AS INTEGER) FROM library WHERE deleted IS NULL ORDER BY CAST(year AS INTEGER) DESC;");

	FillTreeNode(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeNode(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect)
{
	bool isOther = false;

	while (sqlSelect.StepRow())
	{
		std::wstring value;
		if (sqlSelect.ColumnText16(0, value))
			skinTree->InsertNode(treeNode, value, value);
		else
			isOther = true;
	}

	if (isOther)
		skinTree->InsertNode(treeNode, lang->GetLineS(Lang::Library, 0), L"");
}

void DBase::FillTreeNodeAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect)
{
	bool isOther = false;

	while (sqlSelect.StepRow())
	{
		std::wstring album;
		if (sqlSelect.ColumnText16(0, album))
			skinTree->InsertAlbum(treeNode, album, album);
		else
			isOther = true;
	}

	if (isOther)
		skinTree->InsertAlbum(treeNode, lang->GetLineS(Lang::Library, 0), L"");
}

void DBase::FillTreeNodeArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode, SQLRequest& sqlSelect)
{
	bool isOther = false;

	while (sqlSelect.StepRow())
	{
		std::wstring artist;
		if (sqlSelect.ColumnText16(0, artist))
			skinTree->InsertArtist(treeNode, artist, artist);
		else
			isOther = true;
	}

	if (isOther)
		skinTree->InsertArtist(treeNode, lang->GetLineS(Lang::Library, 0), L"");
}

void DBase::FillTreeArtistAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT album FROM library WHERE deleted IS NULL"
		" AND (IFNULL(albumartist,artist) IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?1 COLLATE MYCASE))"
		" GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	FillTreeNodeAlbum(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeComposerArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT cmp FROM ("
		"SELECT IFNULL(albumartist,artist) AS cmp FROM library WHERE deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" UNION ALL "
		"SELECT svalue AS cmp FROM storage,library WHERE skey IN (1,2) AND sid=id AND deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END"
		") GROUP BY cmp COLLATE MYCASE ORDER BY cmp COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	FillTreeNodeArtist(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeComposerAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT album FROM library WHERE deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	if (!treeNode->GetArtist().empty())
		sqlSelect.BindText16(2, treeNode->GetArtist());

	FillTreeNodeAlbum(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeGenreArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT art FROM ("
		"SELECT IFNULL(albumartist,artist) AS art FROM library WHERE deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" UNION ALL "
		"SELECT svalue AS art FROM storage,library WHERE skey IN (1,2) AND sid=id AND deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END"
		") GROUP BY art COLLATE MYCASE ORDER BY art COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	FillTreeNodeArtist(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeGenreAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT album FROM library WHERE deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	if (!treeNode->GetArtist().empty())
		sqlSelect.BindText16(2, treeNode->GetArtist());

	FillTreeNodeAlbum(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeYearArtist(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT art FROM ("
		"SELECT IFNULL(albumartist,artist) AS art FROM library WHERE deleted IS NULL AND CAST(year AS INTEGER) IS ?1"
		" UNION ALL "
		"SELECT svalue AS art FROM storage,library WHERE skey IN (1,2) AND sid=id AND deleted IS NULL AND CAST(year AS INTEGER) IS ?1"
		" AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END"
		") GROUP BY art COLLATE MYCASE ORDER BY art COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	FillTreeNodeArtist(skinTree, treeNode, sqlSelect);
}

void DBase::FillTreeYearAlbum(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT album FROM library WHERE deleted IS NULL"
		" AND CAST(year AS INTEGER) IS ?1"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");

	if (!treeNode->GetValue().empty())
		sqlSelect.BindText16(1, treeNode->GetValue());

	if (!treeNode->GetArtist().empty())
		sqlSelect.BindText16(2, treeNode->GetArtist());

	FillTreeNodeAlbum(skinTree, treeNode, sqlSelect);
}

void DBase::FillListAlbum(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	// " WHERE deleted IS NULL AND album IS ? ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,disc,track;");
	// As a reminder: ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE can be replaced to
	// ORDER BY albumartist COLLATE MYCASE,artist COLLATE MYCASE. Need to test perfomance of both.

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND album IS ? COLLATE MYCASE"
		" ORDER BY albumartist COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListArtist(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (IFNULL(albumartist,artist) IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?1 COLLATE MYCASE))"
		" ORDER BY CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListArtistAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (IFNULL(albumartist,artist) IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?1 COLLATE MYCASE))"
		" AND album IS ?2 COLLATE MYCASE"
		" ORDER BY CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!album.empty())
		sqlSelect.BindText16(2, album);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListComposer(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);
	else
		sqlSelect.BindNull(1);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListComposerArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" ORDER BY CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListComposerAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (composer IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=3 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" AND album IS ?3 COLLATE MYCASE"
		" ORDER BY CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	if (!album.empty())
		sqlSelect.BindText16(3, album);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListGenre(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);
	else
		sqlSelect.BindNull(1);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListGenreArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" ORDER BY CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListGenreAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND (genre IS ?1 COLLATE MYCASE OR id IN (SELECT sid FROM storage WHERE skey=4 AND svalue=?1 COLLATE MYCASE))"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" AND album IS ?3 COLLATE MYCASE"
		" ORDER BY CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	if (!album.empty())
		sqlSelect.BindText16(3, album);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListYear(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND CAST(year AS INTEGER) IS ?"
		" ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListYearArtist(SkinList* skinList, const std::wstring& value, const std::wstring& artist)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND CAST(year AS INTEGER) IS ?1"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" ORDER BY CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListYearAlbum(SkinList* skinList, const std::wstring& value, const std::wstring& artist, const std::wstring& album)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),"
		"title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating"
		" FROM library WHERE deleted IS NULL"
		" AND CAST(year AS INTEGER) IS ?1"
		" AND (IFNULL(albumartist,artist) IS ?2 COLLATE MYCASE OR id IN (SELECT sid FROM storage"
		" WHERE skey IN (1,2) AND CASE WHEN albumartist IS NULL THEN skey=1 ELSE skey=2 END AND svalue=?2 COLLATE MYCASE))"
		" AND album IS ?3 COLLATE MYCASE"
		" ORDER BY CAST(disc AS INTEGER),CAST(track AS INTEGER);");

	if (!value.empty())
		sqlSelect.BindText16(1, value);

	if (!artist.empty())
		sqlSelect.BindText16(2, artist);

	if (!album.empty())
		sqlSelect.BindText16(3, album);

	FillList(skinList, sqlSelect);

	skinList->SetControlRedraw(true);
}

void DBase::FillListSearchTrack(SkinList* skinList, const std::wstring& value)
{
	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
		" WHERE deleted IS NULL AND title IS ? COLLATE LIKECASE ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 1000;");

	sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);
}

void DBase::FillListSearchAlbum(SkinList* skinList, const std::wstring& value)
{
	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
		" WHERE deleted IS NULL AND album IS ? COLLATE LIKECASE ORDER BY albumartist COLLATE MYCASE, album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 1000;");

	sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);
}

void DBase::FillListSearchArtist(SkinList* skinList, const std::wstring& value)
{
	SQLRequest sqlSelect(dbLibrary,
		//"SELECT id,file,size,number,title,artist,albumartist,album,year,genre,time,rating FROM library WHERE deleted IS NULL AND IFNULL(albumartist,artist)=? COLLATE LIKECASE ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,year DESC,album COLLATE MYCASE,number LIMIT 1000;");
		// Further I noticed, if we need to find artist and albumartist separately, if we use "=" then it does not work, but if we use "IS" then it does, I don't know why. Affect our collation LIKECASE, which doesn't compare but find the substring.
		//"SELECT id,file,size,number,title,artist,albumartist,album,year,genre,time,rating FROM library WHERE deleted IS NULL AND (artist IS ?1 COLLATE LIKECASE OR albumartist IS ?1 COLLATE LIKECASE) ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,year DESC,album COLLATE MYCASE,number LIMIT 500;");
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
		" WHERE deleted IS NULL AND (artist IS ?1 COLLATE LIKECASE OR albumartist IS ?1 COLLATE LIKECASE) ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 1000;");

	// Reminder: artist LIKE '%'||?||'%'

	sqlSelect.BindText16(1, value);

	FillList(skinList, sqlSelect);
}

void DBase::FillListSearchAll(SkinList* skinList, const std::wstring& value)
{
	if (!isStopSearch)
	{
		SQLRequest sqlSelect(dbLibrary,
		   "SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
		   " WHERE deleted IS NULL AND title IS ? COLLATE LIKECASE ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 200;");

		sqlSelect.BindText16(1, value);

		FillList(skinList, sqlSelect);
	}

	if (!isStopSearch)
	{
		SQLRequest sqlSelect(dbLibrary,
			"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
			" WHERE deleted IS NULL AND album IS ? COLLATE LIKECASE ORDER BY albumartist COLLATE MYCASE, album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 300;");

		sqlSelect.BindText16(1, value);

		FillList(skinList, sqlSelect);
	}

	if (!isStopSearch)
	{
		SQLRequest sqlSelect(dbLibrary,
			"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
			" WHERE deleted IS NULL AND (artist IS ?1 COLLATE LIKECASE OR albumartist IS ?1 COLLATE LIKECASE) ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER) LIMIT 500;");

		sqlSelect.BindText16(1, value);

		FillList(skinList, sqlSelect);
	}
}

void DBase::FillList(SkinList* skinList, SQLRequest& sqlSelect, bool isPlaylist)
{
	ListNodeUnsafe headNode = nullptr;
	ListNodeUnsafe oldHeadNode = nullptr;
	std::wstring oldAlbum;
	std::wstring oldArtist;
	int oldDisc = 0;

	while (sqlSelect.StepRow())
	{
		if (isStopSearch)
			return;

		std::wstring file = sqlSelect.ColumnText16(2) + sqlSelect.ColumnText16(3); // Path + File
		if (isPortableVersion && file[0] == '?')
			file[0] = programPath[0];

		int newDisc = sqlSelect.ColumnInt(6);
		std::wstring newAlbum = sqlSelect.ColumnText16(8); // Album
		std::wstring newArtist = sqlSelect.ColumnText16(10); // Album Artist

		// When the album changed then add a header for this album
		if (headNode == nullptr || newDisc != oldDisc ||
			!StringEx::IsEqual(newAlbum, oldAlbum) || !StringEx::IsEqual(newArtist, oldArtist))
		{
			// Add a header
			headNode = skinList->InsertHead(nullptr, file);

			// Fill texts for the header
			for (std::size_t i = 0, size = skinList->skinHead.size(); i < size; ++i)
			{
				SkinListElement::Type type = skinList->skinHead[i]->type;

				if (type == SkinListElement::Type::Artist)
				{
					const char* text = sqlSelect.ColumnTextRaw(10); // Album artist
					if (text == nullptr) text = sqlSelect.ColumnTextRaw(9); // Artist
					if (text)
						skinList->SetNodeString2(headNode, type, UTF::UTF16(text));
					else
						skinList->SetNodeString2(headNode, type, lang->GetLineS(Lang::Playlist, 0));
				}
				else if (type == SkinListElement::Type::Album)
				{
					const char* text = sqlSelect.ColumnTextRaw(8);
					if (text)
					{
						if (newDisc < 2)
							skinList->SetNodeString2(headNode, type, UTF::UTF16(text));
						else
						{
							if (newDisc == 2 && oldHeadNode && oldAlbum == newAlbum)
							{
								skinList->SetNodeString2(oldHeadNode, type,
									oldHeadNode->GetLabel(type) + L" (" + lang->GetLineS(Lang::Playlist, 1) + L"1)");
							}
							skinList->SetNodeString2(headNode, type,
								UTF::UTF16(text) + L" (" + lang->GetLineS(Lang::Playlist, 1) + std::to_wstring(newDisc) + L")");
						}
					}
					else
						skinList->SetNodeString2(headNode, type, lang->GetLineS(Lang::Playlist, 0));
				}
				else if (type == SkinListElement::Type::Year)
				{
					const char* text = sqlSelect.ColumnTextRaw(12);
					if (text)
						skinList->SetNodeString(headNode, type, UTF::UTF16(text));
				}
				else if (type == SkinListElement::Type::Genre)
				{
					const char* text = sqlSelect.ColumnTextRaw(11);
					if (text)
						skinList->SetNodeString(headNode, type, UTF::UTF16(text));
				}
				else if (type == SkinListElement::Type::ArtistAlbum)
				{
					const char* artist = sqlSelect.ColumnTextRaw(10); // Album Artist
					if (artist == nullptr) artist = sqlSelect.ColumnTextRaw(9); // Artist
					const char* album = sqlSelect.ColumnTextRaw(8); // Album

					if (artist)
						skinList->SetNodeString2(headNode, SkinListElement::Type::Artist, UTF::UTF16(artist));
					else
						skinList->SetNodeString2(headNode, SkinListElement::Type::Artist, lang->GetLineS(Lang::Playlist, 0));

					if (album)
					{
						if (newDisc < 2)
							skinList->SetNodeString2(headNode, SkinListElement::Type::Album, UTF::UTF16(album));
						else
						{
							if (newDisc == 2 && oldHeadNode && oldAlbum == newAlbum)
							{
								skinList->SetNodeString2(oldHeadNode, SkinListElement::Type::Album,
									oldHeadNode->GetLabel(SkinListElement::Type::Album) + L" (" + lang->GetLineS(Lang::Playlist, 1) + L"1)");
							}
							skinList->SetNodeString2(headNode, SkinListElement::Type::Album,
									UTF::UTF16(album) + L" (" + lang->GetLineS(Lang::Playlist, 1) + std::to_wstring(newDisc) + L")");
						}
					}
					else
						skinList->SetNodeString2(headNode, SkinListElement::Type::Album, lang->GetLineS(Lang::Playlist, 0));
				}
			}

			oldDisc = newDisc;
			oldAlbum = newAlbum;
			oldArtist = newArtist;
			oldHeadNode = headNode;
		}

		// We have the header, add a track to it
		if (headNode)
		{
			// Get track rating and track ID
			long long idLibrary = sqlSelect.ColumnInt64(0);
			long long idPlaylist = 0;
			if (isPlaylist)
				idPlaylist = sqlSelect.ColumnInt64(15);

			int rating = sqlSelect.ColumnInt(14);

			// Adjust the rating
			rating /= 20;
			rating = std::max(0, std::min(5, rating));

			long long cue = sqlSelect.ColumnInt64(1);
			int time = (sqlSelect.ColumnInt(13) + 1000 / 2) / 1000;
			unsigned size = (unsigned)sqlSelect.ColumnInt(4);

			// Add a track
			ListNodeUnsafe trackNode = skinList->InsertTrack(headNode, file, idLibrary, idPlaylist, rating, time, size, cue);

			// Fill texts for the track
			for (std::size_t i = 0, size = skinList->skinTrack.size(); i < size; ++i)
			{
				if (!skinList->skinTrack[i]->isStateLibrary)
					continue;

				SkinListElement::Type type = skinList->skinTrack[i]->type;

				if ((int)type >= 0)
				{
					const char* text = nullptr;

					switch (type)
					{
					case SkinListElement::Type::Title: // Title
						text = sqlSelect.ColumnTextRaw(7);
						if (text == nullptr) text = sqlSelect.ColumnTextRaw(3); // File Name
						break;
					case SkinListElement::Type::Album: // Album
						text = sqlSelect.ColumnTextRaw(8);
						break;
					case SkinListElement::Type::Artist: // Artist
						text = sqlSelect.ColumnTextRaw(10);
						if (text == nullptr) text = sqlSelect.ColumnTextRaw(9);
						break;
					case SkinListElement::Type::Genre: // Genre
						text = sqlSelect.ColumnTextRaw(11);
						break;
					case SkinListElement::Type::Year: // Year
						text = sqlSelect.ColumnTextRaw(12);
						break;
					case SkinListElement::Type::Track: // Track number
						text = sqlSelect.ColumnTextRaw(5);
						break;
					case SkinListElement::Type::Time: // Track time length
						wchar_t str[100];
						swprintf_s(str, L"%d:%.2d", time / 60, time % 60);
						skinList->SetNodeString(trackNode, type, str);
						continue;
					}

					if (text)
						skinList->SetNodeString(trackNode, type, UTF::UTF16(text));
				}
				else if (type == SkinListElement::Type::ArtistTitle)
				{
					const char* artist = sqlSelect.ColumnTextRaw(9); // Artist
					if (artist == nullptr) artist = sqlSelect.ColumnTextRaw(10); // Album artist
					const char* title = sqlSelect.ColumnTextRaw(7); // Title
					if (title == nullptr) title = sqlSelect.ColumnTextRaw(3); // File Name

					if (artist)
						skinList->SetNodeString(trackNode, SkinListElement::Type::Artist, UTF::UTF16(artist));

					if (title)
						skinList->SetNodeString(trackNode, SkinListElement::Type::Title, UTF::UTF16(title));
				}
			}
		}
	}
}

void DBase::GetLibFile(long long idLibrary, long long idPlaylist, DATABASE_GETINFO* info)
{
	SQLRequest sqlSelect;
	SQLRequest sqlSelectM;
	const char *selectM = "SELECT svalue FROM storage WHERE skey=? AND sid=? ORDER BY sidx;";

	if (idLibrary && dbLibrary) // Tags from the library
	{
		sqlSelect.Prepare(dbLibrary,
			"SELECT cue,path,file,category,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,lyricist,comment,channels,bitrate,samplerate FROM library WHERE id=?;");

		sqlSelect.BindInt64(1, idLibrary);

		sqlSelectM.Prepare(dbLibrary, selectM);
		sqlSelectM.BindInt64(2, idLibrary);
	}
	else if (idPlaylist && dbPlaylist) // Tags from the playlist
	{
		sqlSelect.Prepare(dbPlaylist,
			"SELECT cue,path,file,category,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,lyricist,comment,channels,bitrate,samplerate FROM playlist WHERE id=?;");

		sqlSelect.BindInt64(1, idPlaylist);

		sqlSelectM.Prepare(dbPlaylist, selectM);
		sqlSelectM.BindInt64(2, idPlaylist);
	}

	if (sqlSelect.IsPrepared())
	{
		if (sqlSelect.StepRow())
		{
			info->cue         = !sqlSelect.ColumnIsNull(0);
			info->file        = sqlSelect.ColumnText16(1) + sqlSelect.ColumnText16(2);
			if (isPortableVersion && info->file[0] == '?')
				info->file[0] = programPath[0];
			info->type        = sqlSelect.ColumnInt(3);
			info->track       = sqlSelect.ColumnText16(4);
			info->totalTracks = sqlSelect.ColumnText16(5);
			info->disc        = sqlSelect.ColumnText16(6);
			info->totalDiscs  = sqlSelect.ColumnText16(7);
			info->title       = sqlSelect.ColumnText16(8);
			info->album       = sqlSelect.ColumnText16(9);
			info->artist      = sqlSelect.ColumnText16(10);
			info->albumArtist = sqlSelect.ColumnText16(11);
			info->composer    = sqlSelect.ColumnText16(12);
			info->genre       = sqlSelect.ColumnText16(13);
			info->year        = sqlSelect.ColumnText16(14);
			info->lyricist    = sqlSelect.ColumnText16(15);
			info->comment     = sqlSelect.ColumnText16(16);
			info->channels    = sqlSelect.ColumnText16(17);
			info->bitrate     = sqlSelect.ColumnText16(18);
			info->samplerate  = sqlSelect.ColumnText16(19);

			sqlSelectM.BindInt(1, 1);
			while (sqlSelectM.StepRow())
				info->artists.push_back(sqlSelectM.ColumnText16(0));

			sqlSelectM.Reset();
			sqlSelectM.BindInt(1, 2);
			while (sqlSelectM.StepRow())
				info->albumArtists.push_back(sqlSelectM.ColumnText16(0));

			sqlSelectM.Reset();
			sqlSelectM.BindInt(1, 3);
			while (sqlSelectM.StepRow())
				info->composers.push_back(sqlSelectM.ColumnText16(0));

			sqlSelectM.Reset();
			sqlSelectM.BindInt(1, 4);
			while (sqlSelectM.StepRow())
				info->genres.push_back(sqlSelectM.ColumnText16(0));

			sqlSelectM.Reset();
			sqlSelectM.BindInt(1, 5);
			while (sqlSelectM.StepRow())
				info->conductors.push_back(sqlSelectM.ColumnText16(0));

			sqlSelectM.Reset();
			sqlSelectM.BindInt(1, 6);
			while (sqlSelectM.StepRow())
				info->lyricists.push_back(sqlSelectM.ColumnText16(0));
		}
	}
}

void DBase::FillPlaylist(SkinList* skinList, const std::wstring& file)
{
	std::string fileDB = UTF::UTF8S(profilePath + L"Playlists" + L"\\" + file + L".db");
	SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + fileDB + "' AS playlist_db;").c_str());

	// Select tracks from the playlist on the basis of: first select all linked with the library, then all others, and then sort them by order in the playlist.
	// The query is not much optimized, but most of the time the number of tracks in a playlist is not that big, so it doesn't matter.

	SQLRequest sqlSelect(dbLibrary,
		"SELECT library.id,library.cue,library.path,library.file,library.filesize,CAST(library.track AS INTEGER),CAST(library.disc AS INTEGER),library.title,library.album,"
		"library.artist,library.albumartist,library.genre,CAST(library.year AS INTEGER),library.duration,library.rating,playlist.id,playlist.idx FROM library,playlist"
		" WHERE playlist.idlib=library.id"
		" UNION ALL "
		"SELECT 0,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,id,idx FROM playlist WHERE idlib IS NULL"
		" ORDER BY playlist.idx;");

	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();
	skinList->EnableSwap(true);
	FillPlay(skinList, sqlSelect, true, false, false);
	skinList->SetControlRedraw(true);

	//SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");

	//////////////////
	// Experimental: Delete files from the playlist which are not linked with the library

	if (dbPlaylist != dbPlayOpen) // If the playlist is playing the do nothing
	{
		int tracksCount = 0;
		{
			SQLRequest sqlCount(dbPlaylist, "SELECT COUNT(*) FROM playlist;");
			if (sqlCount.StepRow())
				tracksCount = sqlCount.ColumnInt(0);
		}

		if (tracksCount != skinList->GetTracksCount())
		{
			assert(dbPlaylist != dbPlayOpen);
			// Need to close and reopen the playlist database otherwise DELETE does not work
			//dbPlaylist.Close();
			//SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + fileDB + "' AS playlist_db;").c_str());

			// Beware! Never use IN with != in WHERE instead of NOT IN with ==
			SQLRequest sqlDelete(dbLibrary,
				"DELETE FROM playlist WHERE idlib IS NOT NULL AND id NOT IN (SELECT playlist.id FROM playlist,library"
				" WHERE playlist.idlib==library.id);");
			sqlDelete.Step();

			//SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");
			//dbPlaylist.OpenCreate(fileDB);
		}
	}

	SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");
}

void DBase::FillPlaylistOpenFile(SkinList* skinList, const std::wstring& file, int start)
{
	// To attach in readonly or readwrite mode (by default readwritecreate) SQLite must be compiled with SQLITE_USE_URI=1
	// SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE 'file:" + UTF::UTF8S(fileDB) + "?mode=rw' AS playlist_db;").c_str());

	std::wstring fileDB = profilePath + L"Playlists" + L"\\" + file + L".db";
	SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + UTF::UTF8S(fileDB) + "' AS playlist_db;").c_str());

	SQLRequest sqlSelect(dbLibrary,
		"SELECT library.id,library.cue,library.path,library.file,library.filesize,CAST(library.track AS INTEGER),CAST(library.disc AS INTEGER),library.title,library.album,"
		"library.artist,library.albumartist,library.genre,CAST(library.year AS INTEGER),library.duration,library.rating,playlist.id,playlist.idx FROM library,playlist"
		" WHERE playlist.idx>?1 AND playlist.idlib=library.id"
		" UNION ALL "
		"SELECT 0,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,id,idx FROM playlist WHERE idlib IS NULL AND idx>?1"
		" ORDER BY playlist.idx;");

	sqlSelect.BindInt(1, start);

	skinList->SetControlRedraw(false);
	skinList->EnableSwap(true);
	FillPlay(skinList, sqlSelect, true, true, false);
	skinList->SetControlRedraw(true);

	SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");
}

void DBase::FillPlaylistNowPlaying(SkinList* skinList, const std::wstring& file, int start)
{
	if (!dbPlayOpen)
		return;

	std::wstring fileDB = profilePath + L"Playlists" + L"\\" + file + L".db";
	SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + UTF::UTF8S(fileDB) + "' AS playlist_db;").c_str());

	SQLRequest sqlSelect(dbLibrary,
	   "SELECT library.id,library.cue,library.path,library.file,library.filesize,CAST(library.track AS INTEGER),CAST(library.disc AS INTEGER),library.title,library.album,"
	   "library.artist,library.albumartist,library.genre,CAST(library.year AS INTEGER),library.duration,library.rating,playlist.id,playlist.idx FROM library,playlist"
	   " WHERE playlist.idx>?1 AND playlist.idlib=library.id"
	   " UNION ALL "
	   "SELECT 0,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,id,idx FROM playlist WHERE idlib IS NULL AND idx>?1"
	   " ORDER BY playlist.idx;");

	sqlSelect.BindInt(1, start);

	FillPlay(skinList, sqlSelect, true, true, true);

	SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");
}

void DBase::FillPlay(SkinList* skinList, SQLRequest& sqlSelect, bool isPlaylist, bool isSelect, bool isNowPlaying)
{
	if (!isNowPlaying)
		skinList->SetViewPlaylist(true);

	while (sqlSelect.StepRow())
	{
		// Get track rating and track ID
		long long idLibrary = sqlSelect.ColumnInt64(0);
		long long idPlaylist = 0;
		if (isPlaylist)
			idPlaylist = sqlSelect.ColumnInt64(15);

		int rating = sqlSelect.ColumnInt(14);

		// Adjust the rating
		rating /= 20;
		rating = std::max(0, std::min(5, rating));

		long long cue = sqlSelect.ColumnInt64(1);
		int time = (sqlSelect.ColumnInt(13) + 1000 / 2) / 1000;
		unsigned size = (unsigned)sqlSelect.ColumnInt(4);

		std::wstring file = sqlSelect.ColumnText16(2) + sqlSelect.ColumnText16(3); // Path + File
		if (isPortableVersion && file[0] == '?')
			file[0] = programPath[0];

		// Add a track
		ListNodeUnsafe trackNode = nullptr;
		if (!isNowPlaying)
			trackNode = skinList->InsertTrack(nullptr, file, idLibrary, idPlaylist, rating, time, size, cue, isSelect);
		else
			trackNode = skinList->InsertTrackToNowPlaying(nullptr, file, idLibrary, idPlaylist, rating, time, size, cue);

		// Fill texts for the track
		for (std::size_t i = 0, size = skinList->skinTrack.size(); i < size; ++i)
		{
			if (!skinList->skinTrack[i]->isStatePlaylist)
				continue;

			SkinListElement::Type type = skinList->skinTrack[i]->type;

			if ((int)type >= 0)
			{
				const char* text = nullptr;

				switch (type)
				{
				case SkinListElement::Type::Title: // Title
					text = sqlSelect.ColumnTextRaw(7);
					if (text == nullptr) text = sqlSelect.ColumnTextRaw(3); // File Name
					break;
				case SkinListElement::Type::Album: // Album
					text = sqlSelect.ColumnTextRaw(8);
					break;
				case SkinListElement::Type::Artist: // Artist
					text = sqlSelect.ColumnTextRaw(9); // Artist
					if (text == nullptr) sqlSelect.ColumnTextRaw(10); // Album Artist
					break;
				case SkinListElement::Type::Genre: // Genre
					text = (char*)sqlSelect.ColumnTextRaw(11);
					break;
				case SkinListElement::Type::Year: // Year
					text = (char*)sqlSelect.ColumnTextRaw(12);
					break;
				case SkinListElement::Type::Track: // Track number
					text = sqlSelect.ColumnTextRaw(5);
					break;
				case SkinListElement::Type::Time: // Track time length
					wchar_t str[100];
					swprintf_s(str, L"%d:%.2d", time / 60, time % 60);
					skinList->SetNodeString(trackNode, type, str);
					continue;
				}

				if (text)
					skinList->SetNodeString(trackNode, type, UTF::UTF16(text));
			}
			else if (type == SkinListElement::Type::ArtistTitle)
			{
				const char* artist = sqlSelect.ColumnTextRaw(9); // Artist
				if (artist == nullptr) artist = sqlSelect.ColumnTextRaw(10); // Album Artist
				const char* title = sqlSelect.ColumnTextRaw(7); // Title
				if (title == nullptr) title = sqlSelect.ColumnTextRaw(3); // File Name

				if (artist)
					skinList->SetNodeString(trackNode, SkinListElement::Type::Artist, UTF::UTF16(artist));

				if (title)
					skinList->SetNodeString(trackNode, SkinListElement::Type::Title, UTF::UTF16(title));
			}
		}
	}

	skinList->ResetIndex(isNowPlaying);
}

int DBase::GetPlaylistMax()
{
	int result = 0;

	if (dbPlaylist)
	{
		SQLRequest sqlSelect(dbPlaylist,
			"SELECT max(idx) FROM playlist;");

		if (sqlSelect.StepRow())
			result = sqlSelect.ColumnInt(0);
	}

	return result;
}

void DBase::DeleteFromLibrary(SkinList* skinList, std::vector<std::wstring>* libraryFolders)
{
	SQLRequest::Exec(dbLibrary, "SAVEPOINT spdeletefromlibrary");

	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		bool isUpdate = false;

		if (libraryFolders == nullptr)
			isUpdate = true;
		else
		{
			// Let's find out if the file belongs to one of the library folders
			const std::wstring& file = skinList->GetSelectedAt(i)->GetFile();

			for (std::size_t j = 0, size2 = libraryFolders->size(); j < size2; j++)
			{
				std::wstring folder = libraryFolders->at(j);

				if (isPortableVersion && !folder.empty() && folder[0] == '?')
					folder[0] = programPath[0];

				if (file.size() > folder.size() && StringEx::IsEqual(file, folder, folder.size()))
				{
					isUpdate = true;
					break;
				}
			}
		}

		if (isUpdate)
		{
			SQLRequest sqlUpdate(dbLibrary,
				"UPDATE library SET deleted = 1 WHERE id=?;");

			sqlUpdate.BindInt64(1, skinList->GetSelectedAt(i)->idLibrary);
			sqlUpdate.Step();
		}
		else
		{
			SQLRequest sqlDelete(dbLibrary,
				"DELETE FROM library WHERE id=?");

			sqlDelete.BindInt64(1, skinList->GetSelectedAt(i)->idLibrary);
			sqlDelete.Step();
		}
	}

	SQLRequest::Exec(dbLibrary, "RELEASE spdeletefromlibrary");

	skinList->SetControlRedraw(false);

	skinList->DeleteSelected();

	skinList->SetControlRedraw(true);
}

void DBase::DeleteFromPlaylist(SkinList* skinList)
{
	if (!dbPlaylist)
		return;

	PlayBegin();

	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		SQLRequest sqlDelete(dbPlaylist,
			"DELETE FROM playlist WHERE id=?;");

		sqlDelete.BindInt64(1, skinList->GetSelectedAt(i)->idPlaylist);

		sqlDelete.Step();
	}

	PlayCommit();

	skinList->SetControlRedraw(false);
	skinList->DeleteSelected();
	skinList->ResetIndex();
	skinList->SetControlRedraw(true);
}

void DBase::SwapPlaylist(SkinList* skinList)
{
	if (!dbPlaylist)
		return;

	PlayBegin();

	int i = 0;
	for (ListNodeUnsafe listNode = skinList->GetRootNode()->Child(); listNode != nullptr; listNode = listNode->Next())
	{
		++i;
		SQLRequest sqlUpdate(dbPlaylist,
			"UPDATE playlist SET idx=? WHERE id=?;");

		sqlUpdate.BindInt(1, i);
		sqlUpdate.BindInt64(2, listNode->idPlaylist);

		sqlUpdate.Step();
	}

	PlayCommit();
}

void DBase::AddURLToPlaylist(int index, long long added, const std::wstring& url, const std::wstring& title, bool isTemp)
{
	if (url.empty())
		return;

	SQLRequest sqlInsert(!isTemp ? dbPlaylist : dbPlayTemp,
		"INSERT INTO playlist (idx,added,category,file,title) VALUES (?,?,?,?,?);");

	sqlInsert.BindInt(1, index);
	sqlInsert.BindInt64(2, added);
	sqlInsert.BindInt(3, (int)Category::UserRadio);

	sqlInsert.BindText16(4, url);
	if (!title.empty())
		sqlInsert.BindText16(5, title);

	sqlInsert.Step();
}

void DBase::AddFileToPlaylistFrom(long long id, int index, long long added, bool isTemp)
{
	assert(id != 0);

	SQLRequest sqlInsert(!isTemp ? dbPlaylist : dbPlayTemp,
		"INSERT INTO playlist (idlib,idx,added) VALUES (?,?,?);");

	sqlInsert.BindInt64(1, id);
	sqlInsert.BindInt(2, index);
	sqlInsert.BindInt64(3, added);

	sqlInsert.Step();
}

int DBase::FromLibraryToPlaylist(SkinList* skinList, const std::wstring& fileName)
{
	OpenTemp(fileName);

	int start = GetTempMax();
	int oldStart = start;

	long long addedTime = FileSystem::GetTimeNow();

	TempBegin();
	
	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		AddFileToPlaylistFrom(skinList->GetSelectedAt(i)->idLibrary, ++start, addedTime, true);
	}

	TempCommit();
	CloseTemp();

	return oldStart;
}

int DBase::FromPlaylistToPlaylist(SkinList* skinList, const std::wstring& fileName)
{
	OpenTemp(fileName);

	int start = GetTempMax();
	int oldStart = start;

	long long addedTime = FileSystem::GetTimeNow();

	TempBegin();

	// Get fields from the current playlist
	SQLRequest sqlSelect(dbPlaylist,
		"SELECT idlib,disabled,collapsed,cue,filehash,path,file,filesize,modified,category,"
		"trackhash,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,"
		"bpm,compilation,publisher,conductor,lyricist,remixer,grouping,subtitle,copyright,encodedby,comment,"
		"duration,channels,bitrate,samplerate,"
		"rating,loverating,albumrating,folderrating,playcount,lastplayed,skipcount,lastskipped,"
		"replaygain,equalizer,keywords"
		" FROM playlist WHERE id=? LIMIT 1;");

	// Add the fields to another playlist
	SQLRequest sqlInsert(dbPlayTemp,
		"INSERT INTO playlist (idlib,idx,added,disabled,collapsed,cue,filehash,path,file,filesize,modified,category,"
		"trackhash,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year,"
		"bpm,compilation,publisher,conductor,lyricist,remixer,grouping,subtitle,copyright,encodedby,comment,"
		"duration,channels,bitrate,samplerate,"
		"rating,loverating,albumrating,folderrating,playcount,lastplayed,skipcount,lastskipped,"
		"replaygain,equalizer,keywords"
		") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");

	SQLRequest sqlSelectM(dbPlaylist, "SELECT sidx,skey,svalue FROM storage WHERE sid=?;");
	SQLRequest sqlInsertM(dbPlayTemp, "INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		sqlSelect.BindInt64(1, skinList->GetSelectedAt(i)->idPlaylist);

		if (sqlSelect.StepRow())
		{
			bool isPlaylist = sqlSelect.ColumnIsNull(0);

			if (isPlaylist)
				sqlInsert.BindNull(1);
			else
				sqlInsert.BindInt64(1, sqlSelect.ColumnInt64(0));

			sqlInsert.BindInt(2, ++start);
			sqlInsert.BindInt64(3, addedTime);

			if (isPlaylist)
			{
				// It's only BindTextRaw here now, maybe use for?
				sqlInsert.BindTextRaw(4, sqlSelect.ColumnTextRaw(1));
				sqlInsert.BindTextRaw(5, sqlSelect.ColumnTextRaw(2));
				sqlInsert.BindTextRaw(6, sqlSelect.ColumnTextRaw(3));
				sqlInsert.BindTextRaw(7, sqlSelect.ColumnTextRaw(4));
				sqlInsert.BindTextRaw(8, sqlSelect.ColumnTextRaw(5));
				sqlInsert.BindTextRaw(9, sqlSelect.ColumnTextRaw(6));
				sqlInsert.BindTextRaw(10, sqlSelect.ColumnTextRaw(7));
				sqlInsert.BindTextRaw(11, sqlSelect.ColumnTextRaw(8));
				sqlInsert.BindTextRaw(12, sqlSelect.ColumnTextRaw(9));
				sqlInsert.BindTextRaw(13, sqlSelect.ColumnTextRaw(10));
				sqlInsert.BindTextRaw(14, sqlSelect.ColumnTextRaw(11));
				sqlInsert.BindTextRaw(15, sqlSelect.ColumnTextRaw(12));
				sqlInsert.BindTextRaw(16, sqlSelect.ColumnTextRaw(13));
				sqlInsert.BindTextRaw(17, sqlSelect.ColumnTextRaw(14));
				sqlInsert.BindTextRaw(18, sqlSelect.ColumnTextRaw(15));
				sqlInsert.BindTextRaw(19, sqlSelect.ColumnTextRaw(16));
				sqlInsert.BindTextRaw(20, sqlSelect.ColumnTextRaw(17));
				sqlInsert.BindTextRaw(21, sqlSelect.ColumnTextRaw(18));
				sqlInsert.BindTextRaw(22, sqlSelect.ColumnTextRaw(19));
				sqlInsert.BindTextRaw(23, sqlSelect.ColumnTextRaw(20));
				sqlInsert.BindTextRaw(24, sqlSelect.ColumnTextRaw(21));
				sqlInsert.BindTextRaw(25, sqlSelect.ColumnTextRaw(22));
				sqlInsert.BindTextRaw(26, sqlSelect.ColumnTextRaw(23));
				sqlInsert.BindTextRaw(27, sqlSelect.ColumnTextRaw(24));
				sqlInsert.BindTextRaw(28, sqlSelect.ColumnTextRaw(25));
				sqlInsert.BindTextRaw(29, sqlSelect.ColumnTextRaw(26));
				sqlInsert.BindTextRaw(30, sqlSelect.ColumnTextRaw(27));
				sqlInsert.BindTextRaw(31, sqlSelect.ColumnTextRaw(28));
				sqlInsert.BindTextRaw(32, sqlSelect.ColumnTextRaw(29));
				sqlInsert.BindTextRaw(33, sqlSelect.ColumnTextRaw(30));
				sqlInsert.BindTextRaw(34, sqlSelect.ColumnTextRaw(31));
				sqlInsert.BindTextRaw(35, sqlSelect.ColumnTextRaw(32));
				sqlInsert.BindTextRaw(36, sqlSelect.ColumnTextRaw(33));
				sqlInsert.BindTextRaw(37, sqlSelect.ColumnTextRaw(34));
				sqlInsert.BindTextRaw(38, sqlSelect.ColumnTextRaw(35));
				sqlInsert.BindTextRaw(39, sqlSelect.ColumnTextRaw(36));
				sqlInsert.BindTextRaw(40, sqlSelect.ColumnTextRaw(37));
				sqlInsert.BindTextRaw(41, sqlSelect.ColumnTextRaw(38));
				sqlInsert.BindTextRaw(42, sqlSelect.ColumnTextRaw(39));
				sqlInsert.BindTextRaw(43, sqlSelect.ColumnTextRaw(40));
				sqlInsert.BindTextRaw(44, sqlSelect.ColumnTextRaw(41));
				sqlInsert.BindTextRaw(45, sqlSelect.ColumnTextRaw(42));
				sqlInsert.BindTextRaw(46, sqlSelect.ColumnTextRaw(43));
				sqlInsert.BindTextRaw(47, sqlSelect.ColumnTextRaw(44));
				sqlInsert.BindTextRaw(48, sqlSelect.ColumnTextRaw(45));
				sqlInsert.BindTextRaw(49, sqlSelect.ColumnTextRaw(46));
				sqlInsert.BindTextRaw(50, sqlSelect.ColumnTextRaw(47));
			}

			sqlInsert.StepReset();

			if (isPlaylist)
			{
				long long lastInsert = (long long)sqlite3_last_insert_rowid(dbPlayTemp.get());

				sqlSelectM.BindInt64(1, skinList->GetSelectedAt(i)->idPlaylist);
				while (sqlSelectM.StepRow())
				{
					sqlInsertM.BindInt64(1, lastInsert);
					sqlInsertM.BindInt(2, sqlSelectM.ColumnInt(0));
					sqlInsertM.BindInt(3, sqlSelectM.ColumnInt(1));
					sqlInsertM.BindTextRaw(4, sqlSelectM.ColumnTextRaw(2));
					sqlInsertM.StepReset();
				}
				sqlSelectM.Reset();
			}
		}

		sqlSelect.Reset();
	}

	TempCommit();

	sqlInsert.Finalize();
	sqlInsertM.Finalize();
	CloseTemp();

	return oldStart;
}

int DBase::FromRadioToPlaylist(SkinList* skinList, const std::wstring& fileName)
{
	OpenTemp(fileName);

	int start = GetTempMax();
	int oldStart = start;

	long long addedTime = FileSystem::GetTimeNow();

	TempBegin();

	// Add fields to the playlist
	SQLRequest sqlInsert(dbPlayTemp,
		"INSERT INTO playlist (idx,added,category,file,title,artist,albumartist) VALUES (?,?,?,?,?,?,?);");

	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		sqlInsert.BindInt(1, ++start);
		sqlInsert.BindInt64(2, addedTime);
		sqlInsert.BindInt(3, (int)Category::Radio);

		sqlInsert.BindText16(4, skinList->GetSelectedAt(i)->GetFile());
		sqlInsert.BindText16(5, skinList->GetSelectedAt(i)->GetLabel(SkinListElement::Type::Title));
		sqlInsert.BindText16(6, skinList->GetSelectedAt(i)->GetLabel(SkinListElement::Type::Artist));
		sqlInsert.BindText16(7, skinList->GetSelectedAt(i)->GetLabel(SkinListElement::Type::Artist));

		sqlInsert.StepReset();
	}

	TempCommit();

	sqlInsert.Finalize();
	CloseTemp();

	return oldStart;
}

void DBase::TempBegin()
{
	SQLRequest::Exec(dbPlayTemp, "BEGIN;");
}

void DBase::TempCommit()
{
	SQLRequest::Exec(dbPlayTemp, "COMMIT;");
}

void DBase::TempVacuum()
{
	SQLRequest::Exec(dbPlayTemp, "VACUUM;");
}

void DBase::OpenTemp(const std::wstring& fileName)
{
	std::wstring file = profilePath;
	file += L"Playlists";
	file.push_back('\\');
	file += fileName + L".db";

	dbPlayTemp.OpenCreate(file);
	SQLRequest::Exec(dbPlayTemp, "PRAGMA foreign_keys = ON;");
}

void DBase::CloseTemp()
{
	if (dbPlayTemp)
		dbPlayTemp.Close();
}

int DBase::GetTempMax()
{
	int result = 0;

	if (dbPlayTemp)
	{
		SQLRequest sqlSelect(dbPlayTemp,
			"SELECT max(idx) FROM playlist;");

		if (sqlSelect.StepRow())
			result = sqlSelect.ColumnInt(0);
	}

	return result;
}

void DBase::SortHelperAttach()
{
	std::wstring fileDB = profilePath + L"Library.db";
	SQLRequest::Exec(dbPlaylist, ("ATTACH DATABASE '" + UTF::UTF8S(fileDB) + "' AS library_db;").c_str());
}

void DBase::SortHelperDetach()
{
	SQLRequest::Exec(dbPlaylist, "DETACH DATABASE library_db;");
}

void DBase::SortPlaylist(int start, const std::wstring& name)
{
	if (!dbPlaylist)
		return;

	//sqlite3_create_collation(dbPlaylist.get(), "MYNUM", SQLITE_UTF8, nullptr, CompareStringsNum);
	if (futureWin->IsVistaOrLater())
		sqlite3_create_collation(dbPlaylist.get(), "MYCASE", SQLITE_UTF16LE, nullptr, CompareStrings);
	else
		sqlite3_create_collation(dbPlaylist.get(), "MYCASE", SQLITE_UTF16LE, nullptr, CompareStringsXP);

	//std::wstring fileDB = profilePath + L"Playlists" + L"\\" + name + L".db";
	//SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + UTF::UTF8S(fileDB) + "' AS playlist_db;").c_str());

	SQLRequest sqlSelect(dbPlaylist,
		//"SELECT id FROM playlist WHERE idx>? ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,year DESC,album COLLATE MYCASE,number,title COLLATE MYCASE;");
		//"SELECT id FROM playlist WHERE idx>? ORDER BY IFNULL(albumartist,artist) COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,path,CAST(disc AS INTEGER),CAST(track AS INTEGER),file;");
		"SELECT playlist.id,library.path,library.file,CAST(library.track AS INTEGER) AS track2,CAST(library.disc AS INTEGER) as disc2,library.album,"
		"IFNULL(library.albumartist,library.artist) AS artist2,CAST(library.year AS INTEGER) AS year2 FROM playlist,library WHERE playlist.idx>?1 AND playlist.idlib=library.id"
		" UNION ALL "
		"SELECT id,path,file,CAST(track AS INTEGER) AS track2,CAST(disc AS INTEGER) as disc2,album,IFNULL(albumartist,artist) AS artist2,CAST(year AS INTEGER) AS year2 FROM playlist WHERE idx>?1 AND idlib IS NULL"
		" ORDER BY artist2 COLLATE MYCASE,year2 DESC,album COLLATE MYCASE,path,disc2,track2,file;");

	sqlSelect.BindInt(1, start);

	//PlayBegin();

	SQLRequest sqlUpdate(dbPlaylist,
		"UPDATE playlist SET idx=? WHERE id=?;");

	while (sqlSelect.StepRow())
	{
		sqlUpdate.BindInt(1, ++start);
		sqlUpdate.BindInt64(2, sqlSelect.ColumnInt64(0));

		sqlUpdate.StepReset();
	}

	//PlayCommit();

	//SQLRequest::Exec(dbLibrary, "DETACH DATABASE playlist_db;");
}

void DBase::FillTreeFolder(SkinTree* skinTree, TreeNodeUnsafe treeNode, std::vector<std::wstring>* libraryFolders)
{
	if (libraryFolders == nullptr)
	{
		FillTreeFolderSub(skinTree, treeNode);
		return;
	}

	for (std::size_t i = 0, size = libraryFolders->size(); i < size; ++i)
	{
		std::wstring path = libraryFolders->at(i);
		if (!path.empty() && path.back() != '\\')
			path.push_back('\\');

		std::wstring name = path;
		if (name.size() > 3) // If not a drive
		{
			name.pop_back();
			std::size_t find = name.rfind('\\');
			if (find > 3)
				name = path.substr(0, 3) + L".." + name.substr(find + 1);
		}

		if (isPortableVersion && !name.empty() && name[0] == '?')
			name[0] = programPath[0];

		skinTree->InsertNode(treeNode, name, path);
	}
}

void DBase::FillTreeFolderSub(SkinTree* skinTree, TreeNodeUnsafe treeNode)
{
	skinTree->SetControlRedraw(false);

	SQLRequest sqlSelect(dbLibrary,
		"SELECT path FROM library WHERE deleted IS NULL AND path=? COLLATE FSELECT GROUP BY path COLLATE FILECASE ORDER BY path COLLATE FILECASE;");// GROUP BY file COLLATE FOLDER2 ORDER BY file COLLATE FOLDER3;");// GROUP BY album COLLATE MYCASE ORDER BY album COLLATE MYCASE;");

	sqlSelect.BindText16(1, treeNode->GetValue());

	std::wstring lastPath = treeNode->GetValue();
	TreeNodeUnsafe lastNode = treeNode;

	// Just in case we check again, the data should be perfect for the algorithm
	if (!lastPath.empty() && lastPath.back() != '\\')
		lastPath.push_back('\\');


	while (sqlSelect.StepRow())
	{
		const char* text = sqlSelect.ColumnTextRaw(0);

		if (text)
		{
			// Uncomment to see what the algorithm do
			//skinTree->InsertNode(treeNode, UTF::UTF16(text), UTF::UTF16(text));
			//continue;

			std::wstring path = UTF::UTF16(text);

			if (!IsEqualPaths(path, std::min(path.size(), lastPath.size()), lastPath, lastPath.size()))
			{
				std::wstring tempPath = path;

				// Go back through the path to get needed lastNode
				do
				{
					std::size_t find = lastPath.rfind('\\', lastPath.size() - 2);
					if (find == std::string::npos || find == 0) // For add a drive
					{
						lastPath.clear();
						lastNode = lastNode->Parent();
						break;
					}

					lastPath = lastPath.substr(0, find + 1);
					
					if (tempPath.size() > lastPath.size())
					{
						find = tempPath.rfind('\\', tempPath.size() - 2);
						if (find == std::string::npos)
							return; // Error

						tempPath = tempPath.substr(0, find + 1);
					}

					lastNode = lastNode->Parent();

					if (lastNode == nullptr)
						return; // Error
				}
				while (!IsEqualPaths(tempPath, std::min(tempPath.size(), lastPath.size()), lastPath, lastPath.size()));
			}

			// Go forward through the path to add a node to the tree
			std::size_t i = lastPath.size();
			while (i != std::string::npos)
			{
				std::size_t j = path.find('\\', i) + 1;
				if (j == std::string::npos || j == 0)
					break; // Either this is a file and skip or error
				else if (j == 2)
				{
					i = j;
					continue;
				}

				if (i > 0)
					lastNode = skinTree->InsertFolder(lastNode, path.substr(i, j - i - 1), path.substr(0, j));
				else // Drive
				{
					if (j == 1)
					{
						lastNode = skinTree->InsertFolder(lastNode, lang->GetLineS(Lang::Library, 12) +
							L" (NET)", L"\\\\");
					}
					else
					{
						if (isPortableVersion && path[0] == '?')
						{
							//lastNode = skinTree->InsertFolder(lastNode, lang->GetLineS(Lang::Library, 12),
							//	path.substr(0, j));
							lastNode = skinTree->InsertFolder(lastNode, lang->GetLineS(Lang::Library, 12) +
								L" (" + programPath.substr(0, 2) + L")", path.substr(0, j));
						}
						else
						{
							lastNode = skinTree->InsertFolder(lastNode, lang->GetLineS(Lang::Library, 12) +
								L" (" + path.substr(i, j - i - 1) + L")", path.substr(0, j));
						}
					}
				}

				i = j;
			}

			lastPath = path;
		}
	}
}

void DBase::FillListFolder(SkinList* skinList, const std::wstring& value)
{
	skinList->SetControlRedraw(false);
	skinList->DeleteAllNode();

	SQLRequest sqlSelect(dbLibrary,
		"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library"
		" WHERE deleted IS NULL AND path=? COLLATE FSELECT ORDER BY path COLLATE FILECASE,CAST(disc AS INTEGER),CAST(track AS INTEGER),file;");// LIMIT 5000;");


	if (!value.empty())
		sqlSelect.BindText16(1, value);
	else
		sqlSelect.BindNull(1);


	std::wstring rootFolder = value;
	if (rootFolder.size() > 3) // If not a drive
	{
		rootFolder.pop_back();
		std::size_t find = rootFolder.rfind('\\');
		if (find != std::string::npos)
			rootFolder = rootFolder.substr(find + 1);
	}

	ListNodeUnsafe headNode = nullptr;
	std::wstring oldPath;

	while (sqlSelect.StepRow())
	{
		if (isStopSearch)
			return;

		std::wstring path = sqlSelect.ColumnText16(2);
		std::wstring file = path + sqlSelect.ColumnText16(3); // Path + File
		if (isPortableVersion && file[0] == '?')
			file[0] = programPath[0];
		//std::wstring path = file.substr(0, file.rfind('\\') + 1);

		// When the path changed then add a header for this path
		if (headNode == nullptr || !StringEx::IsEqual(path, oldPath))
		{
			oldPath = path;

			// Add a header
			headNode = skinList->InsertHead(nullptr, file);

			// Fill texts for the header
			for (std::size_t i = 0, size = skinList->skinHead.size(); i < size; ++i)
			{
				SkinListElement::Type type = skinList->skinHead[i]->type;

				if (type == SkinListElement::Type::Artist)
				{
					skinList->SetNodeString2(headNode, type, lang->GetLineS(Lang::Playlist, 2));
				}
				else if (type == SkinListElement::Type::Album)
				{
					std::wstring folder = path;
					folder.erase(0, value.size());

					if (!folder.empty())
						folder.pop_back();
					else
						folder = rootFolder;

					skinList->SetNodeString(headNode, SkinListElement::Type::Album, folder);
				}
				else if (type == SkinListElement::Type::ArtistAlbum)
				{
					skinList->SetNodeString2(headNode, SkinListElement::Type::Artist, lang->GetLineS(Lang::Playlist, 2));

					std::wstring folder = path;
					folder.erase(0, value.size());

					if (!folder.empty())
						folder.pop_back();
					else
						folder = rootFolder;

					skinList->SetNodeString(headNode, SkinListElement::Type::Album, folder);
				}
			}
		}

		// We have the header, add a track to it
		if (headNode)
		{
			// Get track rating and track ID
			long long idLibrary = sqlSelect.ColumnInt64(0);
			int rating = sqlSelect.ColumnInt(14);

			// Adjust the rating
			rating /= 20;
			rating = std::max(0, std::min(5, rating));

			long long cue = sqlSelect.ColumnInt64(1);
			int time = (sqlSelect.ColumnInt(13) + 1000 / 2) / 1000;
			unsigned size = (unsigned)sqlSelect.ColumnInt(4);

			// Add a track
			ListNodeUnsafe trackNode = skinList->InsertTrack(headNode, file, idLibrary, 0, rating, time, size, cue);

			// Fill texts for the track
			for (std::size_t i = 0, size = skinList->skinTrack.size(); i < size; ++i)
			{
				if (!skinList->skinTrack[i]->isStateLibrary)
					continue;

				SkinListElement::Type type = skinList->skinTrack[i]->type;

				if ((int)type >= 0)
				{
					const char* text = nullptr;

					switch (type)
					{
					case SkinListElement::Type::Title: // Title
						text = sqlSelect.ColumnTextRaw(7);
						if (text == nullptr) text = sqlSelect.ColumnTextRaw(3); // File Name
						break;
					case SkinListElement::Type::Album: // Album
						text = sqlSelect.ColumnTextRaw(8);
						break;
					case SkinListElement::Type::Artist: // Artist
						text = sqlSelect.ColumnTextRaw(9); // Artist
						if (text == nullptr) text = sqlSelect.ColumnTextRaw(10); // Album artist
						break;
					case SkinListElement::Type::Genre: // Genre
						text = sqlSelect.ColumnTextRaw(11);
						break;
					case SkinListElement::Type::Year: // Year
						text = sqlSelect.ColumnTextRaw(12);
						break;
					case SkinListElement::Type::Track: // Track number
						text = sqlSelect.ColumnTextRaw(5);
						break;
					case SkinListElement::Type::Time: // Track time length
						wchar_t str[100];
						swprintf_s(str, L"%d:%.2d", time / 60, time % 60);
						skinList->SetNodeString(trackNode, type, str);
						continue;
					}

					if (text)
						skinList->SetNodeString(trackNode, type, UTF::UTF16(text));
				}
				else if (type == SkinListElement::Type::ArtistTitle)
				{
					const char* artist = sqlSelect.ColumnTextRaw(9); // Artist
					if (artist == nullptr) artist = sqlSelect.ColumnTextRaw(10); // Album artist
					const char* title = sqlSelect.ColumnTextRaw(7); // Title
					if (title == nullptr) title = sqlSelect.ColumnTextRaw(3); // File Name

					if (artist)
						skinList->SetNodeString(trackNode, SkinListElement::Type::Artist, UTF::UTF16(artist));

					if (title)
						skinList->SetNodeString(trackNode, SkinListElement::Type::Title, UTF::UTF16(title));
				}
			}
		}
	}

	skinList->SetControlRedraw(true);
}

void DBase::SmartPrepareSelect(SmartList &smart, std::string& select, std::vector<std::wstring>& values)
{
	if (smart.from == 0)
		return;

	// For year we use a more complicated parsing
	bool isYears = false;
	std::vector<int> years;
	if (smart.from == 4)
		isYears = true;

	bool isFolders = false;
	if (smart.from == 5)
		isFolders = true;

	bool isExclude = false; // If true then exclude all items
	if (!smart.fromString.empty() && smart.fromString[0] == '-')
	{
		isExclude = true;
		smart.fromString.erase(0, 1);
	}

	std::vector<char> contains;

	std::size_t midFrom = 0;
	for (std::size_t i = 0, size = smart.fromString.size(); i <= size; ++i)
	{
		// Separate all items separated by '+' (and '-' for year)
		if (i == size || smart.fromString[i] == '+' || (isYears && smart.fromString[i] == '-'))
		{
			std::wstring value = smart.fromString.substr(midFrom, i - midFrom);
			StringEx::Trim(value);
			midFrom = i + 1;
			if (!value.empty())
			{
				if (isFolders && value.back() != '\\')
					value.push_back('\\');

				if (value[0] == '*' && !isYears && !isFolders)
				{
					values.push_back(value.substr(1));
					contains.push_back(1);
				}
				else
				{
					values.push_back(value);
					contains.push_back(0);
				}

				if (isYears)
				{
					// First, check for incorrect record. For example: 2000-2008-2010
					if (!years.empty() && years.back() == 1)
						years.push_back(0);
					else // Next, mark in years if couple or a single value
					{
						if (i < size && smart.fromString[i] == '-')
							years.push_back(1);
						else
							years.push_back(0);
					}
				}
			}
		}
	}

	// If years ended with '-', fix it
	if (!years.empty())
		years.back() = 0;

	// If values are empty add null value
	if (values.empty())
	{
		values.push_back(L"");
		if (isYears)
			years.push_back(0);
	}

	// Note. I don't know why but in SQLite "IS" and "IS NOT" sometimes works much faster than "=" and "<>".
	// For example our first condition (artist=? OR albumartist=?) much slower than (artist IS ? OR albumartist IS ?)
	// So we use "IS" instead of "=". Also we use it in all our conditions because we need a comparison with NULL.
	// Also we use "IS" in our last condition (file IS ?) because "=" doesn't work with multiple values.
	// It doesn't work because we use our own collation COLLATE FSELECT.
	// It seems all that above is not actual with new versions of SQLite, performance is equal. But I leave as is.

	bool isYearSkip = false;
	for (std::size_t i = 0, size = values.size(); i < size; ++i)
	{
		if (i == 0)
			select += "AND (";
		else if (!isYearSkip)
		{
			if (!isExclude)
				select += " OR ";
			else
				select += " AND ";
		}
		else
			isYearSkip = false;

		switch (smart.from)
		{
		case 1: // Artist
		{
			std::string n = std::to_string(i + 1);
			if (contains[i] == 0)
			{
				if (!isExclude)
					//request += "IFNULL(albumartist,artist)=? COLLATE MYCASE";
					//request += "(artist=?"+n+" COLLATE MYCASE OR albumartist=?"+n+" COLLATE MYCASE)";
					select += "(artist IS ?"+n+" COLLATE MYCASE OR albumartist IS ?"+n+" COLLATE MYCASE)";
				else
					//request += "IFNULL(albumartist,artist)<>? COLLATE MYCASE";
					//request += "(artist<>?"+n+" COLLATE MYCASE AND albumartist<>?"+n+" COLLATE MYCASE)";
					select += "(artist IS NOT ?"+n+" COLLATE MYCASE AND albumartist IS NOT ?"+n+" COLLATE MYCASE)";
			}
			else
			{
				if (!isExclude)
					select += "(artist IS ?"+n+" COLLATE LIKECASE OR albumartist IS ?"+n+" COLLATE LIKECASE)";
				else
					select += "(artist IS NOT ?"+n+" COLLATE LIKECASE AND albumartist IS NOT ?"+n+" COLLATE LIKECASE)";
			}
			break;
		}
		case 2: // Album
			if (contains[i] == 0)
			{
				if (!isExclude)
					select += "album IS ? COLLATE MYCASE";
				else
					select += "album IS NOT ? COLLATE MYCASE";
			}
			else
			{
				if (!isExclude)
					select += "album IS ? COLLATE LIKECASE";
				else
					select += "album IS NOT ? COLLATE LIKECASE";
			}
			break;
		case 3: // Genre
			if (contains[i] == 0)
			{
				if (!isExclude)
					select += "genre IS ? COLLATE MYCASE";
				else
					select += "genre IS NOT ? COLLATE MYCASE";
			}
			else
			{
				if (!isExclude)
					select += "genre IS ? COLLATE LIKECASE";
				else
					select += "genre IS NOT ? COLLATE LIKECASE";
			}
			break;
		case 4: // Year
			if (years[i] == 1)
			{
				isYearSkip = true;
				if (!isExclude)
					select += "CAST(year AS INTEGER) BETWEEN CAST(? AS INTEGER) AND CAST(? AS INTEGER)";
				else
					select += "CAST(year AS INTEGER) NOT BETWEEN CAST(? AS INTEGER) AND CAST(? AS INTEGER)";
			}
			else if (i == 0 || years[i - 1] == 0)
			{
				if (!isExclude)
					select += "CAST(year AS INTEGER) IS ?";
				else
					select += "CAST(year AS INTEGER) IS NOT ?";
			}
			break;
		case 5: // Folder
			if (!isExclude)
				select += "path IS ? COLLATE FSELECT";
			else
				select += "path IS NOT ? COLLATE FSELECT";
			break;
		}
	}

//	if (smart.from == 1) // If from Artists then duplicate all items
//	{
//		std::vector<std::wstring> temp(values.size() * 2);
//		for (std::size_t i = 0, size = values.size(); i < size; ++i)
//		{
//			temp[i * 2]     = values[i];
//			temp[i * 2 + 1] = values[i];
//		}
//		values = temp;
//	}

	if (!values.empty())
		select += ") ";
}

void DBase::SmartFilterAlbums(SmartList &smart, std::string& select, std::vector<int>& values)
{
	bool isHaving = false;

	if (smart.order)
	{
		isHaving = true;

		select += "GROUP BY album COLLATE MYCASE HAVING ";

		// Filter to exclude not played, not rated etc. tracks
		switch (smart.order)
		{
		case 1: // Most Played
		case 2: // Rarely Played
			select += "SUM(playcount)>0 ";
			break;
		case 3: // Top Rated
		case 4: // Lowest Rated
			select += "SUM(rating)>0 ";
			break;
		}
	}

	if (smart.tracksCount)
	{
		if (!isHaving)
			select += "GROUP BY album COLLATE MYCASE HAVING ";
		else
			select += "AND ";

		isHaving = true;

		switch (smart.tracksCount)
		{
		case 1: // Equal to
			select += "COUNT(*)=? ";
			break;
		case 2: // Greater than
			select += "COUNT(*)>? ";
			break;
		case 3: // Less than
			select += "COUNT(*)<? ";
			break;
		}

		values.push_back(smart.tracksCountNum);
	}

	if (!isHaving)
		select += "GROUP BY album COLLATE MYCASE ";

	if (smart.order)
	{
		switch (smart.order)
		{
		case 1: // Most Played
			select += "ORDER BY AVG(IFNULL(playcount,0)) DESC";
			break;
		case 2: // Rarely Played
			select += "ORDER BY AVG(IFNULL(playcount,0))";
			break;
		case 3: // Top Rated
			select += "ORDER BY AVG(IFNULL(rating,0)) DESC";
			break;
		case 4: // Lowest Rated
			select += "ORDER BY AVG(IFNULL(rating,0))";
			break;
		}
	}
	else if (!smart.isRandom) // Default sorting
		select += "ORDER BY artist COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER)";

	if (smart.count && smart.order)
	{
		select += " LIMIT ?";
		values.push_back(smart.count);
	}

	if (smart.isRandom)
	{
		if (smart.order)
			select = "SELECT * FROM (" + select + ") ";
		
		select += "ORDER BY RANDOM()";
	}

	if (smart.count && smart.order == 0)
	{
		select += " LIMIT ?";
		values.push_back(smart.count);
	}

	select += ";";
}

void DBase::SmartFilterTracks(SmartList &smart, std::string& select, std::vector<int>& values)
{
	if (smart.order)
	{
		// Filter to exclude not played, not rated etc. tracks
		switch (smart.order)
		{
		case 1: // Most Played
		case 2: // Rarely Played
			if (!smart.playCount) // Only without additional filter
				select += "AND IFNULL(playcount,0)>0 ";
			break;
		case 3: // Top Rated
		case 4: // Lowest Rated
			if (!smart.rating) // Only without additional filter
				select += "AND IFNULL(rating,0)>0 ";
			break;
		case 5: // Recently Played
			if (!smart.lastPlayed) // Only without additional filter
				select += "AND lastplayed IS NOT NULL ";
			break;
		case 6: // Recently Added
			if (!smart.dateAdded) // Only without additional filter
				select += "AND added IN (SELECT MAX(added) FROM library) ";
			break;
		}
	}

	bool isAdvFilter = false;

	if (smart.playCount)
	{
		if (!isAdvFilter)
		{
			isAdvFilter = true;
			select += "AND (";
		}

		switch (smart.playCount)
		{
		case 1: // Equal to
			select += "IFNULL(playcount,0)=?";
			break;
		case 2: // Greater than
			select += "IFNULL(playcount,0)>?";
			break;
		case 3: // Less than
			select += "IFNULL(playcount,0)<?";
			break;
		}

		values.push_back(smart.playCountNum);
	}

	if (smart.rating)
	{
		if (!isAdvFilter)
		{
			isAdvFilter = true;
			select += "AND (";
		}
		else if (smart.isAny)
			select += " OR ";
		else
			select += " AND ";

		switch (smart.rating)
		{
		case 1: // Equal to
			select += "IFNULL(rating,0)=?";
			break;
		case 2: // Greater than
			select += "IFNULL(rating,0)>?";
			break;
		case 3: // Less than
			select += "IFNULL(rating,0)<?";
			break;
		}

		values.push_back(smart.ratingNum * 20);
	}

	if (smart.lastPlayed)
	{
		if (!isAdvFilter)
		{
			isAdvFilter = true;
			select += "AND (";
		}
		else if (smart.isAny)
			select += " OR ";
		else
			select += " AND ";

		switch (smart.lastPlayed)
		{
		case 1: // In the last
			select += "lastplayed>" + SmartList::GetSmartDate(smart.lastPlayedNum);
			break;
		case 2: // Before
			select += "lastplayed<" + SmartList::GetSmartDate(smart.lastPlayedNum);
			break;
		}
	}

	if (smart.dateAdded)
	{
		if (!isAdvFilter)
		{
			isAdvFilter = true;
			select += "AND (";
		}
		else if (smart.isAny)
			select += " OR ";
		else
			select += " AND ";

		switch (smart.dateAdded)
		{
		case 1: // In the last
			select += "added>" + SmartList::GetSmartDate(smart.dateAddedNum);
			break;
		case 2: // Before
			select += "added<" + SmartList::GetSmartDate(smart.dateAddedNum);
			break;
		}
	}

	if (isAdvFilter)
		select += ") ";

	if (smart.order && smart.order != 6)
	{
		switch (smart.order)
		{
		case 1: // Most Played
			select += "ORDER BY playcount DESC";
			break;
		case 2: // Rarely Played
			select += "ORDER BY playcount";
			break;
		case 3: // Top Rated
			select += "ORDER BY rating DESC";
			break;
		case 4: // Lowest Rated
			select += "ORDER BY rating";
			break;
		case 5: // Recently Played
			// Here we add additional condition in WHERE to exclude not played tracks
			select += "ORDER BY lastplayed DESC";
			break;
		//case 6: // Recently Added
		//	select += "ORDER BY added DESC";
		//	break;
		}
	}
	else if (!smart.isRandom) // Default sorting
		select += "ORDER BY artist COLLATE MYCASE,CAST(year AS INTEGER) DESC,album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER)";

	if (smart.count && smart.order)
	{
		select += " LIMIT ?";
		values.push_back(smart.count);
	}

	if (smart.isRandom)
	{
		if (smart.order)
			select = "SELECT * FROM (" + select + ") ";
		
		select += "ORDER BY RANDOM()";
	}

	if (smart.count && smart.order == 0)
	{
		select += " LIMIT ?";
		values.push_back(smart.count);
	}

	select += ";";
}

void DBase::FillSmartlist(SkinList* skinList, const std::wstring& fileName, bool isUpdate)
{
	isSmartlistOpen = true;

	SmartList smart;
	OpenSmartlist(fileName, smart);

	if ((smart.isAutoUpdate || isUpdate) && smart.type == 0)
	{
		std::vector<std::wstring> valuesFrom;
		std::vector<int> values;

		std::string select = "SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,cue,filehash FROM library WHERE deleted IS NULL ";

		SmartPrepareSelect(smart, select, valuesFrom);
		SmartFilterTracks(smart, select, values);

		//MessageBoxA(NULL, request.c_str(), "", NULL);

		SQLRequest sqlSelect(dbLibrary, select.c_str());

		int column = 0;

		for (std::size_t i = 0, size = valuesFrom.size(); i < size; ++i)
		{
			if (!valuesFrom[i].empty())
				sqlSelect.BindText16(++column, valuesFrom[i]);
			else
				sqlSelect.BindNull(++column);
		}

		for (std::size_t i = 0, size = values.size(); i < size; ++i)
			sqlSelect.BindInt(++column, values[i]);

		if (!smart.isAutoUpdate)
			FromLibraryToSmartlist(fileName, sqlSelect);
		else
		{
			skinList->SetControlRedraw(false);
			skinList->DeleteAllNode();
			FillPlay(skinList, sqlSelect);
			skinList->SetControlRedraw(true);
		}
	}
	else if ((smart.isAutoUpdate || isUpdate) && smart.type == 1)
	{
		std::vector<std::wstring> valuesFrom;
		std::vector<int> values;

		std::string select = "SELECT album FROM library WHERE album IS NOT NULL AND deleted IS NULL ";

		SmartPrepareSelect(smart, select, valuesFrom);
		SmartFilterAlbums(smart, select, values);

		//MessageBoxA(NULL, request.c_str(), "", NULL);

		SQLRequest::Exec(dbLibrary, "ATTACH DATABASE ':memory:' AS mem;");

		SQLRequest::Exec(dbLibrary, "CREATE TABLE mem.abm (id2 INTEGER PRIMARY KEY, album2 TEXT);");

		SQLRequest sqlSelectFill(dbLibrary, ("INSERT INTO abm (album2) " + select).c_str());

		int column = 0;

		for (std::size_t i = 0, size = valuesFrom.size(); i < size; ++i)
		{
			if (!valuesFrom[i].empty())
				sqlSelectFill.BindText16(++column, valuesFrom[i]);
			else
				sqlSelectFill.BindNull(++column);
		}

		for (std::size_t i = 0, size = values.size(); i < size; ++i)
			sqlSelectFill.BindInt(++column, values[i]);

		sqlSelectFill.Step();

		SQLRequest sqlSelect(dbLibrary,
			//"SELECT id,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,cue,filehash FROM library,abm"
			//" WHERE album=album2 COLLATE MYCASE ORDER BY id2,CAST(disc AS INTEGER),CAST(track AS INTEGER);");
			"SELECT id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating,cue,filehash FROM library"
			" WHERE album COLLATE MYCASE IN (SELECT album2 FROM abm) ORDER BY album COLLATE MYCASE,CAST(disc AS INTEGER),CAST(track AS INTEGER);");

		if (!smart.isAutoUpdate)
			FromLibraryToSmartlist(fileName, sqlSelect);
		else
		{
			skinList->SetControlRedraw(false);
			skinList->DeleteAllNode();
			FillList(skinList, sqlSelect);
			skinList->SetControlRedraw(true);
		}

		SQLRequest::Exec(dbLibrary, "DETACH DATABASE mem;");
	}


	if (!smart.isAutoUpdate)
	{
		std::wstring file = profilePath;
		file += L"Smartlists";
		file.push_back('\\');
		file += fileName + L".db";

		SQLRequest::Exec(dbLibrary, ("ATTACH DATABASE '" + UTF::UTF8S(file) + "' AS smartlist_db;").c_str());
		
		SQLRequest sqlSelect(dbLibrary,
			"SELECT library.id,cue,path,file,filesize,CAST(track AS INTEGER),CAST(disc AS INTEGER),title,album,artist,albumartist,genre,CAST(year AS INTEGER),duration,rating FROM library,smartlist"
			" WHERE smartlist.idlib=library.id ORDER BY smartlist.idx;");

		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();

		if (smart.type == 0)
			FillPlay(skinList, sqlSelect, true);
		else if (smart.type == 1)
			FillList(skinList, sqlSelect, true);

		skinList->SetControlRedraw(true);

		SQLRequest::Exec(dbLibrary, "DETACH DATABASE smartlist_db;");
	}

	// Delete the smartlist database if smartlist changed to auto update
	if (isUpdate && smart.isAutoUpdate)
	{
		std::wstring file = profilePath;
		file += L"Smartlists";
		file.push_back('\\');
		file += fileName + L".db";

		if (FileSystem::Exists(file))
			FileSystem::RemoveFile(file);
	}
}

TreeNodeUnsafe DBase::CreateSmartlist(SkinTree* skinTree, const std::wstring& name, SmartList& smart)
{
	if (skinTree == nullptr)
		return nullptr;

	std::wstring folder = profilePath;
	folder += L"Smartlists";

	// Create the folder for smartlists
	FileSystem::CreateDir(folder);

	TreeNodeUnsafe resultNode = nullptr;

	auto randomSeed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 randomEngine((unsigned)randomSeed);
	std::uniform_int_distribution<int> randomInt(0x00, 0xFF);
	auto randomHex = [&](){return randomInt(randomEngine);};

	// Try to create database three times in the case of matching file names
	for (int i = 0; i < 3; ++i)
	{
		// Generate 4 random numbers
		int random1 = randomHex();
		int random2 = randomHex();
		int random3 = randomHex();
		int random4 = randomHex();

		// Make a database name from the random numbers
		std::wstring random = StringEx::Format(L"%.2x%.2x%.2x%.2x", random1, random2, random3, random4);

		std::wstring file = folder;
		file.push_back('\\');
		file += random;
		file += L".xml";

		// Create a new database (if a database with the same name doesn't exist)
		if (!FileSystem::Exists(file))
		{
			SaveSmartlist(random, smart);
			resultNode = skinTree->AddSmartlist(name, random);

			SaveTreeSmartlists(skinTree);
			break;
		}
	}

	return resultNode;
}

void DBase::CreateMySmartlists(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return;

	std::wstring folder = profilePath;
	folder += L"Smartlists";

	// Create the folder for smartlists
	FileSystem::CreateDir(folder);

	int numSmart = 0;

	auto randomSeed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 randomEngine((unsigned)randomSeed);
	std::uniform_int_distribution<int> randomInt(0x00, 0xFF);
	auto randomHex = [&](){return randomInt(randomEngine);};

	// Try to create database three times in the case of matching file names
	for (int i = 0; i < 30; ++i)
	{
		// Generate 4 random numbers
		int random1 = randomHex();
		int random2 = randomHex();
		int random3 = randomHex();
		int random4 = randomHex();

		// Make a database name from the random numbers
		std::wstring random = StringEx::Format(L"%.2x%.2x%.2x%.2x", random1, random2, random3, random4);

		std::wstring file = folder;
		file.push_back('\\');
		file += random;
		file +=	L".xml";

		// Create a new database (if a database with the same name doesn't exist)
		if (!FileSystem::Exists(file))
		{
			numSmart++;

			std::wstring name;
			SmartList smart;

			switch (numSmart)
			{
			case 1:
				name = lang->GetLineS(Lang::DefaultList, 4);
				smart.type = 0;
				smart.count = 50;
				break;
			case 2:
				name = lang->GetLineS(Lang::DefaultList, 5);
				smart.type = 1;
				smart.count = 5;
				break;
			case 3:
				name = lang->GetLineS(Lang::DefaultList, 6);
				smart.type = 0;
				smart.count = 100;
				smart.order = 5;
				smart.isRandom = false;
				break;
			case 4:
				name = lang->GetLineS(Lang::DefaultList, 7);
				smart.type = 0;
				smart.count = 25;
				smart.order = 1;
				smart.isRandom = false;
				break;
			case 5:
				name = lang->GetLineS(Lang::DefaultList, 8);
				smart.type = 0;
				smart.count = 25;
				smart.order = 3;
				smart.isRandom = false;
				break;
			case 6:
				name = lang->GetLineS(Lang::DefaultList, 9);
				smart.from = 4;
				smart.fromString = L"1990 - 1999";
				smart.type = 0;
				smart.count = 100;
				break;
			}

			SaveSmartlist(random, smart);
			skinTree->AddSmartlist(name, random);

			if (numSmart >= 6)
				break;
		}
	}

	SaveTreeSmartlists(skinTree);
}

bool DBase::SaveSmartlist(const std::wstring& fileName, SmartList& smart)
{
	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("Smartlist");

	if (xmlMain)
	{
		XmlNode xmlFrom = xmlMain.AddChild("From");
		if (xmlFrom)
		{
			xmlFrom.AddAttribute("Type", smart.from);
			if (!smart.fromString.empty())
				xmlFrom.AddAttribute16("String", smart.fromString);
		}

		XmlNode xmlOptions = xmlMain.AddChild("Options");
		if (xmlOptions)
		{
			xmlOptions.AddAttribute("Type", smart.type);
			xmlOptions.AddAttribute("Order", smart.order);
			xmlOptions.AddAttribute("Count", smart.count);
			xmlOptions.AddAttribute("Random", smart.isRandom);
			xmlOptions.AddAttribute("AutoUpdate", smart.isAutoUpdate);
		}

		XmlNode xmlFilter;
		if (smart.IsFilter())
			xmlFilter = xmlMain.AddChild("Filter");

		if (xmlFilter)
		{
			if (smart.isAny)
				xmlFilter.AddAttribute("Any", true);

			if (smart.type == 0)
			{
				if (smart.playCount)
				{
					xmlFilter.AddAttribute("PlayCount", smart.playCount);
					xmlFilter.AddAttribute("PlayCountNum", smart.playCountNum);
				}

				if (smart.rating)
				{
					xmlFilter.AddAttribute("Rating", smart.rating);
					xmlFilter.AddAttribute("RatingNum", smart.ratingNum);
				}

				if (smart.lastPlayed)
				{
					xmlFilter.AddAttribute("LastPlayed", smart.lastPlayed);
					xmlFilter.AddAttribute("LastPlayedNum", smart.lastPlayedNum);
				}

				if (smart.dateAdded)
				{
					xmlFilter.AddAttribute("DateAdded", smart.dateAdded);
					xmlFilter.AddAttribute("DateAddedNum", smart.dateAddedNum);
				}
			}
			else if (smart.type == 1)
			{
				if (smart.tracksCount)
				{
					xmlFilter.AddAttribute("TracksCount", smart.tracksCount);
					xmlFilter.AddAttribute("TracksCountNum", smart.tracksCountNum);
				}
			}
		}
	}

	std::wstring file = profilePath;
	file += L"Smartlists";
	file.push_back('\\');
	file += fileName + L".xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

bool DBase::OpenSmartlist(const std::wstring& fileName, SmartList& smart)
{
	std::wstring file = profilePath;
	file += L"Smartlists";
	file.push_back('\\');
	file += fileName + L".xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Smartlist");

		if (xmlMain)
		{
			XmlNode xmlFrom = xmlMain.FirstChild("From");
			if (xmlFrom)
			{
				xmlFrom.Attribute("Type", &smart.from);
				smart.fromString = xmlFrom.Attribute16("String");
			}

			XmlNode xmlOptions = xmlMain.FirstChild("Options");
			if (xmlOptions)
			{
				xmlOptions.Attribute("Type", &smart.type);
				xmlOptions.Attribute("Order", &smart.order);
				xmlOptions.Attribute("Count", &smart.count);
				xmlOptions.Attribute("Random", &smart.isRandom);
				xmlOptions.Attribute("AutoUpdate", &smart.isAutoUpdate);
			}

			XmlNode xmlFilter = xmlMain.FirstChild("Filter");
			if (xmlFilter)
			{
				xmlFilter.Attribute("Any", &smart.isAny);

				if (xmlFilter.Attribute("PlayCount", &smart.playCount))
					xmlFilter.Attribute("PlayCountNum", &smart.playCountNum);
				if (xmlFilter.Attribute("Rating", &smart.rating))
					xmlFilter.Attribute("RatingNum", &smart.ratingNum);
				if (xmlFilter.Attribute("LastPlayed", &smart.lastPlayed))
					xmlFilter.Attribute("LastPlayedNum", &smart.lastPlayedNum);
				if (xmlFilter.Attribute("DateAdded", &smart.dateAdded))
					xmlFilter.Attribute("DateAddedNum", &smart.dateAddedNum);
				if (xmlFilter.Attribute("TracksCount", &smart.tracksCount))
					xmlFilter.Attribute("TracksCountNum", &smart.tracksCountNum);
			}
		}
	}
	else
		return false;

	return true;
}

bool DBase::LoadTreeSmartlists(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return false;

	std::wstring fileSmartlists = profilePath + L"Smartlists.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(fileSmartlists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Smartlists");

		if (xmlMain)
		{
			// Load all elements
			for (XmlNode xmlNode = xmlMain.FirstChild("Smart"); xmlNode; xmlNode = xmlNode.NextChild("Smart"))
			{
				std::wstring name = xmlNode.Attribute16("Name");
				std::wstring file = xmlNode.Attribute16("File");

				skinTree->InsertSmartlist(skinTree->GetSmartlistNode(), name, file);
			}
		}
	}
	else
		return false;

	return true;
}

bool DBase::SaveTreeSmartlists(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return false;

	TreeNodeUnsafe smartNode = skinTree->GetSmartlistNode();

	if (smartNode == nullptr) // Smartlists are disabled
		return true;

	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("Smartlists");
	
	for (TreeNodeUnsafe treeNode = smartNode->Child(); treeNode != nullptr; treeNode = treeNode->Next())
	{
		XmlNode xmlSmart = xmlMain.AddChild("Smart");

		xmlSmart.AddAttribute16("Name", treeNode->GetTitle());
		xmlSmart.AddAttribute16("File", treeNode->GetValue());
	}
	
	std::wstring file = profilePath + L"Smartlists.xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void DBase::FromLibraryToSmartlist(const std::wstring& fileName, SQLRequest &sqlSelect)
{
	SQLFile dbSmartlist;

	std::wstring file = profilePath;
	file += L"Smartlists";
	file.push_back('\\');
	file += fileName + L".db";

	dbSmartlist.OpenCreate(file);

	if (!dbSmartlist)
		return;

	CreateTableSmartlist(dbSmartlist);

	long long addedTime = FileSystem::GetTimeNow();

	SQLRequest::Exec(dbSmartlist, "BEGIN;");

	SQLRequest::Exec(dbSmartlist, "DROP TABLE IF EXISTS smartlist;");
	CreateTableSmartlist(dbSmartlist);

	// Add fields to the smartlist database
	SQLRequest sqlInsert(dbSmartlist,
		"INSERT INTO smartlist (idlib,idx,added) VALUES (?,?,?);");

	int start = 0;
	while (sqlSelect.StepRow())
	{
		sqlInsert.BindInt64(1, sqlSelect.ColumnInt64(0));
		sqlInsert.BindInt(2, ++start);
		sqlInsert.BindInt64(3, addedTime);

		sqlInsert.StepReset();
	}

	SQLRequest::Exec(dbSmartlist, "COMMIT;");

	sqlInsert.Finalize();

	dbSmartlist.Close();
}

void DBase::DeleteTreeSmartlist(SkinTree* skinTree)
{
	if (skinTree == nullptr)
		return;

	TreeNodeUnsafe focusNode = skinTree->GetFocusNode();

	if (focusNode)
	{
		// Delete the database file
		std::wstring file = profilePath;
		file += L"Smartlists";
		file.push_back('\\');
		file += focusNode->GetValue();

		std::wstring fileDB = file + L".db";
		bool dbExists = false;

		if (FileSystem::Exists(fileDB))
		{
			if (!FileSystem::RemoveFile(fileDB))
				dbExists = true;
		}

		if (!dbExists && FileSystem::RemoveFile(file + L".xml"))
		{
			// If the file has been deleted then delete the node
			skinTree->DeleteSelected(skinTree->GetFocusNode());

			// And save new nodes state
			SaveTreeSmartlists(skinTree);
		}
	}
}

void DBase::DeleteFromSmartlist(SkinList* skinList, const std::wstring& file)
{
	std::wstring fileDB = profilePath;
	fileDB += L"Smartlists";
	fileDB.push_back('\\');
	fileDB += file + L".db";

	// The file with the smartlist database doesn't exist, so this is not auto updating playlist
	if (!FileSystem::Exists(fileDB))
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteSelected();
		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
		return;
	}

	SQLFile dbSmartlist;
	dbSmartlist.OpenCreate(fileDB);

	if (!dbSmartlist)
		return;

	SQLRequest::Exec(dbSmartlist, "BEGIN;");

	for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
	{
		SQLRequest sqlDelete(dbSmartlist,
			"DELETE FROM smartlist WHERE id=?;");

		sqlDelete.BindInt64(1, skinList->GetSelectedAt(i)->idPlaylist);

		sqlDelete.Step();
	}

	SQLRequest::Exec(dbSmartlist, "COMMIT;");

	dbSmartlist.Close();

	skinList->SetControlRedraw(false);
	skinList->DeleteSelected();
	skinList->ResetIndex();
	skinList->SetControlRedraw(true);
}

void DBase::UpdateTagsBegin()
{
	SQLRequest::Exec(dbLibrary, "SAVEPOINT spupdatetags");
	if (dbPlaylist)
		SQLRequest::Exec(dbPlaylist, "SAVEPOINT spupdatetags");
}

void DBase::UpdateTagsCommit()
{
	SQLRequest::Exec(dbLibrary, "RELEASE spupdatetags");
	if (dbPlaylist)
		SQLRequest::Exec(dbPlaylist, "RELEASE spupdatetags");
}

void DBase::UpdateTagsEditor(long long id, DATABASE_SONGINFO* tags, bool isPlaylist)
{
	// Update tags through tags editor (through properties dialog)
	// We skip "file" and "added" because the file was just updated.
	// Note: We do not change any ratings.

	const char* update = nullptr;

	if (!isPlaylist)
	{
		update = "UPDATE library SET filesize=?,modified=?,trackhash=?,track=?,totaltracks=?,disc=?,totaldiscs=?,"
				"title=?,album=?,artist=?,albumartist=?,composer=?,genre=?,year=?,bpm=?,compilation=?,publisher=?,"
				"conductor=?,lyricist=?,remixer=?,grouping=?,subtitle=?,copyright=?,encodedby=?,comment=? WHERE id=?;";
	}
	else
	{
		update = "UPDATE playlist SET filesize=?,modified=?,trackhash=?,track=?,totaltracks=?,disc=?,totaldiscs=?,"
				"title=?,album=?,artist=?,albumartist=?,composer=?,genre=?,year=?,bpm=?,compilation=?,publisher=?,"
				"conductor=?,lyricist=?,remixer=?,grouping=?,subtitle=?,copyright=?,encodedby=?,comment=? WHERE id=?;";
	}


	SQLRequest sqlUpdate(!isPlaylist ? dbLibrary : dbPlaylist, update);

	// 26. Track ID which we update
	sqlUpdate.BindInt64(26, id);

	// 1. File size
	sqlUpdate.BindInt64(1, tags->size);

	// 2. Date time when file last modified
	assert(tags->modified != 0);
	sqlUpdate.BindInt64(2, tags->modified);

	// 3. Track hash
	sqlUpdate.BindInt(3, tags->trackHash);

	// 4. Track number in album
	if (!tags->track.empty())
		sqlUpdate.BindText8(4, tags->track);
	else
		sqlUpdate.BindNull(4);

	// 5. Total tracks in album
	if (!tags->totalTracks.empty())
		sqlUpdate.BindText8(5, tags->totalTracks);
	else
		sqlUpdate.BindNull(5);

	// 6. Disc number
	if (!tags->disc.empty())
		sqlUpdate.BindText8(6, tags->disc);
	else
		sqlUpdate.BindNull(6);

	// 7. Total discs
	if (!tags->totalDiscs.empty())
		sqlUpdate.BindText8(7, tags->totalDiscs);
	else
		sqlUpdate.BindNull(7);

	// 8. Track title
	if (!tags->title.empty())
		sqlUpdate.BindText8(8, tags->title);
	else
		sqlUpdate.BindNull(8);

	// 9. Track album
	if (!tags->album.empty())
		sqlUpdate.BindText8(9, tags->album);
	else
		sqlUpdate.BindNull(9);

	// 10. Track artist
	if (!tags->artist.empty())
		sqlUpdate.BindText8(10, tags->artist);
	else
		sqlUpdate.BindNull(10);

	// 11. Track album artist
	if (!tags->albumArtist.empty())
		sqlUpdate.BindText8(11, tags->albumArtist);
	else
		sqlUpdate.BindNull(11);

	// 12. Track composer
	if (!tags->composer.empty())
		sqlUpdate.BindText8(12, tags->composer);
	else
		sqlUpdate.BindNull(12);

	// 13. Track genre
	if (!tags->genre.empty())
		sqlUpdate.BindText8(13, tags->genre);
	else
		sqlUpdate.BindNull(13);

	// 14. Track release year
	if (!tags->year.empty())
		sqlUpdate.BindText8(14, tags->year);
	else
		sqlUpdate.BindNull(14);

	// 15. Track BPM
	if (!tags->bpm.empty())
		sqlUpdate.BindText8(15, tags->bpm);
	else
		sqlUpdate.BindNull(15);

	// 16. Track compilation
	if (!tags->compilation.empty())
		sqlUpdate.BindText8(16, tags->compilation);
	else
		sqlUpdate.BindNull(16);

	// 17. Track publisher
	if (!tags->publisher.empty())
		sqlUpdate.BindText8(17, tags->publisher);
	else
		sqlUpdate.BindNull(17);

	// 18. Track conductor
	if (!tags->conductor.empty())
		sqlUpdate.BindText8(18, tags->conductor);
	else
		sqlUpdate.BindNull(18);

	// 19. Track lyricist
	if (!tags->lyricist.empty())
		sqlUpdate.BindText8(19, tags->lyricist);
	else
		sqlUpdate.BindNull(19);

	// 20. Track remixer
	if (!tags->remixer.empty())
		sqlUpdate.BindText8(20, tags->remixer);
	else
		sqlUpdate.BindNull(20);

	// 21. Track grouping
	if (!tags->grouping.empty())
		sqlUpdate.BindText8(21, tags->grouping);
	else
		sqlUpdate.BindNull(21);

	// 22. Track subtitle
	if (!tags->subtitle.empty())
		sqlUpdate.BindText8(22, tags->subtitle);
	else
		sqlUpdate.BindNull(22);

	// 23. Track copyright
	if (!tags->copyright.empty())
		sqlUpdate.BindText8(23, tags->copyright);
	else
		sqlUpdate.BindNull(23);

	// 24. Track encoded by
	if (!tags->encodedby.empty())
		sqlUpdate.BindText8(24, tags->encodedby);
	else
		sqlUpdate.BindNull(24);

	// 25. Track comment
	if (!tags->comment.empty())
		sqlUpdate.BindText8(25, tags->comment);
	else
		sqlUpdate.BindNull(25);


	bool stepDone = sqlUpdate.StepDone();

	if (stepDone)
	{
		SQLRequest sqlDeleteM(!isPlaylist ? dbLibrary : dbPlaylist,
			"DELETE FROM storage WHERE sid=?");
		sqlDeleteM.BindInt64(1, id);
		sqlDeleteM.Step();

		if (!tags->genres.empty() || !tags->artists.empty() || !tags->composers.empty() ||
			!tags->albumArtists.empty() || !tags->conductors.empty() || !tags->lyricists.empty())
		{
			SQLRequest sqlInsertM(!isPlaylist ? dbLibrary : dbPlaylist,
				"INSERT INTO storage (sid,sidx,skey,svalue) VALUES (?,?,?,?);");

			InsertMultipleValues(id, sqlInsertM, tags);
		}
	}
}


void DBase::UpdateTagsNode(SkinList* skinList, ListNodeUnsafe listNode, DATABASE_SONGINFO* tags)
{
	// Update tags in the library and playlist database
	if (listNode->idLibrary)
		UpdateTagsEditor(listNode->idLibrary, tags, false);
	else if (listNode->idPlaylist)
		UpdateTagsEditor(listNode->idPlaylist, tags, true);

	// Fill texts for the track
	for (std::size_t i = 0, size = skinList->skinTrack.size(); i < size; ++i)
	{
		SkinListElement::Type type = skinList->skinTrack[i]->type;

		if ((int)type >= 0)
		{
			switch (type)
			{
			case SkinListElement::Type::Title: // Title
				if (!tags->title.empty())
					skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->title));
				else
					skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->file));
				break;
			case SkinListElement::Type::Album: // Album
				skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->album));
				break;
			case SkinListElement::Type::Artist: // Artist
				if (!tags->artist.empty())
					skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->artist));
				else
					skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->albumArtist));
				break;
			case SkinListElement::Type::Genre: // Genre
				skinList->SetNodeString(listNode, type, UTF::UTF16S(tags->genre));
				break;
			case SkinListElement::Type::Year: // Year
				if (!tags->year.empty())
				{
					int year = atoi(tags->year.c_str());
					skinList->SetNodeString(listNode, type, std::to_wstring(year));
				}
				else
					skinList->SetNodeString(listNode, type, L"");
				break;
			case SkinListElement::Type::Track: // Track number
				if (!tags->track.empty())
				{
					int track = atoi(tags->track.c_str());
					skinList->SetNodeString(listNode, type, std::to_wstring(track));
				}
				else
					skinList->SetNodeString(listNode, type, L"");
				break;
			}
		}
		else if (type == SkinListElement::Type::ArtistTitle)
		{
			skinList->SetNodeString(listNode, SkinListElement::Type::Artist, UTF::UTF16S(tags->artist));
			if (!tags->title.empty())
				skinList->SetNodeString(listNode, SkinListElement::Type::Title, UTF::UTF16S(tags->title));
			else
				skinList->SetNodeString(listNode, SkinListElement::Type::Title, UTF::UTF16S(tags->file));
		}
	}

	// Also change a header of the album, if the first track changed
	if (listNode->Parent() != skinList->GetRootNode() && listNode == listNode->Parent()->Child())
	{
		ListNodeUnsafe headNode = listNode->Parent();

		// Fill texts for the header
		for (std::size_t i = 0, size = skinList->skinHead.size(); i < size; ++i)
		{
			SkinListElement::Type type = skinList->skinHead[i]->type;

			if ((int)type >= 0)
			{
				switch (type)
				{
				case SkinListElement::Type::Album: // Album
					if (!tags->album.empty())
						skinList->SetNodeString(headNode, type, UTF::UTF16S(tags->album));
					else
						skinList->SetNodeString(headNode, type, lang->GetLineS(Lang::Playlist, 0));
					break;
				case SkinListElement::Type::Artist: // Artist
					if (!tags->albumArtist.empty())
						skinList->SetNodeString(headNode, type, UTF::UTF16S(tags->albumArtist));
					else if (!tags->artist.empty())
						skinList->SetNodeString(headNode, type, UTF::UTF16S(tags->artist));
					else
						skinList->SetNodeString(headNode, type, lang->GetLineS(Lang::Playlist, 0));
					break;
				case SkinListElement::Type::Genre: // Genre
					skinList->SetNodeString(headNode, type, UTF::UTF16S(tags->genre));
					break;
				case SkinListElement::Type::Year: // Year
					if (!tags->year.empty())
					{
						int year = atoi(tags->year.c_str());
						skinList->SetNodeString(headNode, type, std::to_wstring(year));
					}
					else
						skinList->SetNodeString(headNode, type, L"");
					break;
				}
			}
			else if (type == SkinListElement::Type::ArtistAlbum)
			{
				std::wstring album = UTF::UTF16S(tags->album);
				std::wstring artist = UTF::UTF16S(tags->albumArtist);
				if (artist.empty())
					artist = UTF::UTF16S(tags->artist);

				if (!artist.empty())
					skinList->SetNodeString(headNode, SkinListElement::Type::Artist, artist);
				else
					skinList->SetNodeString(headNode, SkinListElement::Type::Artist, lang->GetLineS(Lang::Playlist, 0));

				if (!album.empty())
					skinList->SetNodeString(headNode, SkinListElement::Type::Album, album);
				else
					skinList->SetNodeString(headNode, SkinListElement::Type::Album, lang->GetLineS(Lang::Playlist, 0));
			}
		}
	}
}
