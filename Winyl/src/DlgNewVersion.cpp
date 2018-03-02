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

// DlgNewVersion.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgNewVersion.h"
#include "UTF.h"
#include "FileSystem.h"


// DlgNewVersion dialog

DlgNewVersion::DlgNewVersion()
{

}

DlgNewVersion::~DlgNewVersion()
{

}

INT_PTR DlgNewVersion::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		//CenterDialog(hDlg, wndMessage);
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnBnClickedOK();
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			return TRUE;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgNewVersion::OnInitDialog()
{
	// Disable close button
	//LONG_PTR style = GetWindowLongPtr(thisWnd, GWL_STYLE);
	//SetWindowLongPtr(thisWnd, GWL_STYLE, style & ~WS_SYSMENU);
	::EnableMenuItem(::GetSystemMenu(thisWnd, FALSE), SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);

	::SetWindowText(thisWnd, L"Winyl");
	if (!convertPortableTo && !convertPortableFrom)
		::SetDlgItemText(thisWnd, IDC_STATIC_NEWVERSION, lang->GetLine(Lang::NewVersionDialog, 0));
	else
		::SetDlgItemText(thisWnd, IDC_STATIC_NEWVERSION, lang->GetLine(Lang::NewVersionDialog, 1));

	HWND progressControl = ::GetDlgItem(thisWnd, IDC_PROGRESS_NEWVER);

	LONG_PTR progressStyle = ::GetWindowLongPtr(progressControl, GWL_STYLE);
	::SetWindowLongPtr(progressControl, GWL_STYLE, progressStyle | PBS_MARQUEE);

	::SendMessageW(progressControl, PBM_SETMARQUEE, TRUE, 0);

	threadWorker.Start(std::bind(&DlgNewVersion::RunThread, this));
}

void DlgNewVersion::OnBnClickedOK()
{
	if (!isThreadDone)
		return;

	if (threadWorker.IsJoinable())
		threadWorker.Join();

	EndDialog(IDOK);
}

void DlgNewVersion::OnBnClickedCancel()
{
	if (!isThreadDone)
		return;

	if (threadWorker.IsJoinable())
		threadWorker.Join();

	EndDialog(IDCANCEL);
}

void DlgNewVersion::RunThread()
{
	if (ThreadUpdate())
	{
		isThreadDone = true;
		::PostMessageW(thisWnd, WM_COMMAND, IDOK, 0);
	}
	else
	{
		isThreadDone = true;
		::PostMessageW(thisWnd, WM_COMMAND, IDCANCEL, 0);
	}
}

bool DlgNewVersion::ThreadUpdate()
{
	if (convertPortableTo || convertPortableFrom)
	{
		ConvertPortable();
		return true;
	}

	if (IsLibraryOld())
	{
		// If update from version before 2.4 need to do this
		FileSystem::Rename(profilePath + L"Playlist", profilePath + L"Playlists");
		FileSystem::Rename(profilePath + L"Playlist.xml", profilePath + L"Playlists.xml");
		FileSystem::Rename(profilePath + L"HotKey.xml", profilePath + L"HotKeys.xml");
		ConvertHotKeysXML();

		//
		if (!FileSystem::Rename(profilePath + L"Library.db", profilePath + L"LibraryOLD1.db"))
			return false;

		FileSystem::Rename(profilePath + L"Playlists", profilePath + L"PlaylistsOLD1");
		FileSystem::Rename(profilePath + L"Smartlists", profilePath + L"SmartlistsOLD1");

		if (!FileSystem::CreateDir(profilePath + L"Playlists"))
			return false;
		if (!FileSystem::CreateDir(profilePath + L"Smartlists"))
			return false;

		ConvertLibrary();
		ConvertPlaylists();
		ConvertSmartlists();

		FileSystem::RemoveFile(profilePath + L"LibraryOLD1.db");
		FileSystem::RemoveDirs(profilePath + L"PlaylistsOLD1");
		FileSystem::RemoveDirs(profilePath + L"SmartlistsOLD1");

		return true;
	}

	return false;
}

bool DlgNewVersion::IsLibraryOld()
{
	bool result = false;

	std::wstring fileLibrary = profilePath;
	fileLibrary += L"Library.db";

	DBase::SQLFile dbLibrary;

	if (dbLibrary.OpenWrite(fileLibrary))
	{
		DBase::SQLRequest sqlSelect(dbLibrary, "PRAGMA table_info(library);");

		for (int i = 0; i < 5 && sqlSelect.StepRow(); ++i)
		{
			if (i == 4  && sqlSelect.ColumnText8(1) == "file") // file is 5th column = old format
				result = true;
		}
	}

	dbLibrary.Close();

	return result;
}

bool DlgNewVersion::ConvertLibrary()
{
	std::wstring fileLibrary = profilePath;
	fileLibrary += L"LibraryOLD1.db";

	DBase::SQLFile dbLibrary;

	if (dbLibrary.OpenWrite(fileLibrary))
	{
		ConvertLibraryDB(dbLibrary);
	}

	dbLibrary.Close();

	return true;
}

bool DlgNewVersion::ConvertLibraryDB(const DBase::SQLFile &db)
{
	DBase::SQLRequest sqlSelect(db,
		"SELECT id,flag,deleted,play,file,size,strftime('%s','1970-01-01 01:02:03'),strftime('%s', IFNULL(added,'2000-01-01 00:00:00')),"
		"number,title,artist,album,year,genre,albumartist,composer,mood,"
		"time,type,bitrate,sample,channel,rating,love,count,strftime('%s', lastcount),skip,strftime('%s', lastskip),"
		"equalizer FROM library ORDER by id;");

	if (sqlSelect.IsPrepared())
	{
		std::wstring fileLibrary = profilePath;
		fileLibrary += L"Library.db";

		DBase::SQLFile dbLibrary;

		dbLibrary.OpenCreate(fileLibrary);

		CreateTableLibrary(dbLibrary);

		DBase::SQLRequest::Exec(dbLibrary, "BEGIN;");

		DBase::SQLRequest sqlInsert(dbLibrary,
			"INSERT INTO library (id,flag,deleted,added,disabled,collapsed,cue,filehash,path,file,filesize,modified,category," // 13
			"trackhash,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year," // 25
			"bpm,compilation,publisher,conductor,lyricist,remixer,grouping,subtitle,copyright,encodedby,comment," // 36
			"duration,channels,bitrate,samplerate," // 40
			"rating,loverating,albumrating,folderrating,playcount,lastplayed,skipcount,lastskipped," // 48
			"bookmark,replaygain,equalizer,keywords" // 52
			") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");

		if (sqlInsert.IsPrepared())
		{
			while (sqlSelect.StepRow())
			{
				sqlInsert.BindInt64(1, sqlSelect.ColumnInt64(0)); // fromto: id

				sqlInsert.BindTextRaw(3, sqlSelect.ColumnTextRaw(2)); // fromto: deleted

				sqlInsert.BindTextRaw(4, sqlSelect.ColumnTextRaw(7)); // fromto: added

				std::wstring file = sqlSelect.ColumnText16(4); // from: file

				sqlInsert.BindInt(8, StringEx::HashFNV1a32(StringEx::ToLowerUS(file))); // to: filehash
				sqlInsert.BindText16(9, PathEx::PathFromFile(file)); // to: path
				sqlInsert.BindText16(10, PathEx::FileFromPath(file)); // to: file

				sqlInsert.BindTextRaw(11, sqlSelect.ColumnTextRaw(5)); // from: size to: filesize
				sqlInsert.BindTextRaw(12, sqlSelect.ColumnTextRaw(6)); // fromto: modified

				//

				const char* title = sqlSelect.ColumnTextRaw(9); // from: title
				const char* album = sqlSelect.ColumnTextRaw(11); // from: album
				const char* artist = sqlSelect.ColumnTextRaw(10); // from: artist

				if (title && strcmp(title, "") != 0)
					sqlInsert.BindInt(14, StringEx::HashFNV1a32(UTF::UTF16(title) + UTF::UTF16(album) + UTF::UTF16(artist))); // to: trackhash
				else
					sqlInsert.BindInt(14, StringEx::HashFNV1a32(PathEx::FileFromPath(file) + UTF::UTF16(album) + UTF::UTF16(artist))); // to: trackhash

				if (title && strcmp(title, "") != 0)
					sqlInsert.BindTextRaw(19, title); // to: title

				if (album && strcmp(album, "") != 0)
					sqlInsert.BindTextRaw(20, album); // to: album

				if (artist && strcmp(artist, "") != 0)
					sqlInsert.BindTextRaw(21, artist); // to: artist

				const char* track = sqlSelect.ColumnTextRaw(8); // from: number
				if (track && strcmp(track, "") != 0)
					sqlInsert.BindTextRaw(15, track); // to: track

				const char* albumartist = sqlSelect.ColumnTextRaw(14); // from: albumartist
				if (albumartist && strcmp(albumartist, "") != 0)
					sqlInsert.BindTextRaw(22, albumartist); // to: albumartist

				const char* composer = sqlSelect.ColumnTextRaw(15); // from: composer
				if (composer && strcmp(composer, "") != 0)
					sqlInsert.BindTextRaw(23, composer); // to: composer

				const char* genre = sqlSelect.ColumnTextRaw(13); // from: genre
				if (genre && strcmp(genre, "") != 0)
					sqlInsert.BindTextRaw(24, genre); // to: genre

				const char* year = sqlSelect.ColumnTextRaw(12); // from: year
				if (year && strcmp(year, "") != 0)
					sqlInsert.BindTextRaw(25, year); // to: year

				//

				if (sqlSelect.ColumnTextRaw(17))
				{
					int duration = sqlSelect.ColumnInt(17) * 1000; // from: time
					sqlInsert.BindInt(37, duration); // to: duration
				}

				sqlInsert.BindTextRaw(38, sqlSelect.ColumnTextRaw(21)); // from: channel to: channels

				sqlInsert.BindTextRaw(39, sqlSelect.ColumnTextRaw(19)); // fromto: bitrate

				sqlInsert.BindTextRaw(40, sqlSelect.ColumnTextRaw(20)); // from: sample to: samplerate

				//

				sqlInsert.BindTextRaw(41, sqlSelect.ColumnTextRaw(22)); // fromto: rating

				int playCount = sqlSelect.ColumnInt(24); // from: count
				const char* lastplayed = sqlSelect.ColumnTextRaw(25); // from: lastcount

				if (playCount > 0 && lastplayed && strcmp(lastplayed, "") != 0)
				{
					sqlInsert.BindInt(45, playCount); // to: playcount
					sqlInsert.BindTextRaw(46, lastplayed); // to: lastplayed
				}

				int skipCount = sqlSelect.ColumnInt(26); // from: skip
				const char* lastskipped = sqlSelect.ColumnTextRaw(27); // from: lastskip

				if (skipCount > 0 && lastskipped && strcmp(lastskipped, "") != 0)
				{
					sqlInsert.BindInt(47, skipCount); // to: skipcount
					sqlInsert.BindTextRaw(48, lastskipped); // to: lastskipped
				}

				sqlInsert.StepResetClear();
			}
		}

		sqlInsert.Finalize();

		DBase::SQLRequest::Exec(dbLibrary, "COMMIT;");

		dbLibrary.Close();
	}

	return true;
}

bool DlgNewVersion::ConvertPlaylist(const std::wstring& file)
{
	bool result = false;

	std::wstring filePlaylist = profilePath;
	filePlaylist += L"PlaylistsOLD1";
	filePlaylist.push_back('\\');
	filePlaylist += file;

	DBase::SQLFile dbPlaylist;

	bool isOpen = dbPlaylist.OpenWrite(filePlaylist);

	if (isOpen)
	{
		DBase::SQLRequest sqlSelect(dbPlaylist, "PRAGMA table_info(playlist);");

		for (int i = 0; i < 5 && sqlSelect.StepRow(); ++i)
		{
			if (i == 4  && sqlSelect.ColumnText8(1) == "file") // file is 5th column = old format
				result = true;
		}

		if (result)
		{
			ConvertPlaylistDB(dbPlaylist, file);
		}
	}

	if (!isOpen || !result) // Create empty playlist if something wrong
	{
		std::wstring filePlaylistNew = profilePath;
		filePlaylistNew += L"Playlists";
		filePlaylistNew.push_back('\\');
		filePlaylistNew += file;

		DBase::SQLFile dbPlaylistNew;

		dbPlaylistNew.OpenCreate(filePlaylistNew);

		CreateTablePlaylist(dbPlaylistNew);

		dbPlaylistNew.Close();
	}

	dbPlaylist.Close();

	return result;
}

bool DlgNewVersion::ConvertPlaylistDB(const DBase::SQLFile &db, const std::wstring& file)
{
	DBase::SQLRequest sqlSelect(db,
		"SELECT id,idlib,idx,play,file,size,strftime('%s', modified),strftime('%s', IFNULL(added,'2000-01-01 00:00:00')),"
		"number,title,artist,album,year,genre,albumartist,composer,mood,"
		"time,type,bitrate,sample,channel,rating,love,count,strftime('%s', lastcount),skip,strftime('%s', lastskip),"
		"equalizer FROM playlist ORDER by id;");

	if (sqlSelect.IsPrepared())
	{
		std::wstring filePlaylistNew = profilePath;
		filePlaylistNew += L"Playlists";
		filePlaylistNew.push_back('\\');
		filePlaylistNew += file;

		DBase::SQLFile dbPlaylist;

		dbPlaylist.OpenCreate(filePlaylistNew);

		CreateTablePlaylist(dbPlaylist);

		DBase::SQLRequest::Exec(dbPlaylist, "BEGIN;");

		DBase::SQLRequest sqlInsert(dbPlaylist,
			"INSERT INTO playlist (id,idlib,idx,added,disabled,collapsed,cue,filehash,path,file,filesize,modified,category," // 13
			"trackhash,track,totaltracks,disc,totaldiscs,title,album,artist,albumartist,composer,genre,year," // 25
			"bpm,compilation,publisher,conductor,lyricist,remixer,grouping,subtitle,copyright,encodedby,comment," // 36
			"duration,channels,bitrate,samplerate," // 40
			"rating,loverating,albumrating,folderrating,playcount,lastplayed,skipcount,lastskipped," // 48
			"bookmark,replaygain,equalizer,keywords" // 52
			") VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");

		if (sqlInsert.IsPrepared())
		{
			while (sqlSelect.StepRow())
			{
				sqlInsert.BindInt64(1, sqlSelect.ColumnInt64(0)); // fromto: id

				int type = sqlSelect.ColumnInt(18); // from: type

				int idlib = sqlSelect.ColumnInt(1); // from: idlib

				if (idlib > 0)
					sqlInsert.BindInt(2, idlib); // to: idlib

				sqlInsert.BindInt(3, sqlSelect.ColumnInt(2)); // fromto: idx

				sqlInsert.BindTextRaw(4, sqlSelect.ColumnTextRaw(7)); // fromto: added

				if (idlib > 0)
				{
					sqlInsert.StepResetClear();
					continue;
				}

				std::wstring file = sqlSelect.ColumnText16(4); // from: file

				if (type != 100 && type != 1000)
				{
					sqlInsert.BindInt(8, StringEx::HashFNV1a32(StringEx::ToLowerUS(file))); // to: filehash
					sqlInsert.BindText16(9, PathEx::PathFromFile(file)); // to: path
					sqlInsert.BindText16(10, PathEx::FileFromPath(file)); // to: file
				}
				else
					sqlInsert.BindText16(10, file); // to: file

				sqlInsert.BindTextRaw(11, sqlSelect.ColumnTextRaw(5)); // from: size to: filesize
				sqlInsert.BindTextRaw(12, sqlSelect.ColumnTextRaw(6)); // fromto: modified

				if (type == 100)
					sqlInsert.BindInt(13, 11); // to: category
				else if (type == 1000)
					sqlInsert.BindInt(13, 10); // to: category
				else
					type = 0;

				//

				const char* title = sqlSelect.ColumnTextRaw(9); // from: title
				const char* album = sqlSelect.ColumnTextRaw(11); // from: album
				const char* artist = sqlSelect.ColumnTextRaw(10); // from: artist

				if (type == 0)
				{
				if (title && strcmp(title, "") != 0)
					sqlInsert.BindInt(14, StringEx::HashFNV1a32(UTF::UTF16(title) + UTF::UTF16(album) + UTF::UTF16(artist))); // to: trackhash
				else
					sqlInsert.BindInt(14, StringEx::HashFNV1a32(PathEx::FileFromPath(file) + UTF::UTF16(album) + UTF::UTF16(artist))); // to: trackhash
				}

				if (title && strcmp(title, "") != 0)
					sqlInsert.BindTextRaw(19, title); // to: title

				if (album && strcmp(album, "") != 0)
					sqlInsert.BindTextRaw(20, album); // to: album

				if (artist && strcmp(artist, "") != 0)
					sqlInsert.BindTextRaw(21, artist); // to: artist

				const char* track = sqlSelect.ColumnTextRaw(8); // from: number
				if (track && strcmp(track, "") != 0)
					sqlInsert.BindTextRaw(15, track); // to: track

				const char* albumartist = sqlSelect.ColumnTextRaw(14); // from: albumartist
				if (albumartist && strcmp(albumartist, "") != 0)
					sqlInsert.BindTextRaw(22, albumartist); // to: albumartist

				const char* composer = sqlSelect.ColumnTextRaw(15); // from: composer
				if (composer && strcmp(composer, "") != 0)
					sqlInsert.BindTextRaw(23, composer); // to: composer

				const char* genre = sqlSelect.ColumnTextRaw(13); // from: genre
				if (genre && strcmp(genre, "") != 0)
					sqlInsert.BindTextRaw(24, genre); // to: genre

				const char* year = sqlSelect.ColumnTextRaw(12); // from: year
				if (year && strcmp(year, "") != 0)
					sqlInsert.BindTextRaw(25, year); // to: year

				//

				if (sqlSelect.ColumnTextRaw(17))
				{
					int duration = sqlSelect.ColumnInt(17) * 1000; // from: time
					sqlInsert.BindInt(37, duration); // to: duration
				}

				sqlInsert.BindTextRaw(38, sqlSelect.ColumnTextRaw(21)); // from: channel to: channels

				sqlInsert.BindTextRaw(39, sqlSelect.ColumnTextRaw(19)); // fromto: bitrate

				sqlInsert.BindTextRaw(40, sqlSelect.ColumnTextRaw(20)); // from: sample to: samplerate

				//

				sqlInsert.BindTextRaw(41, sqlSelect.ColumnTextRaw(22)); // fromto: rating

				int playCount = sqlSelect.ColumnInt(24); // from: count
				const char* lastplayed = sqlSelect.ColumnTextRaw(25); // from: lastcount

				if (playCount > 0 && lastplayed && strcmp(lastplayed, "") != 0)
				{
					sqlInsert.BindInt(45, playCount); // to: playcount
					sqlInsert.BindTextRaw(46, lastplayed); // to: lastplayed
				}

				int skipCount = sqlSelect.ColumnInt(26); // from: skip
				const char* lastskipped = sqlSelect.ColumnTextRaw(27); // from: lastskip

				if (skipCount > 0 && lastskipped && strcmp(lastskipped, "") != 0)
				{
					sqlInsert.BindInt(47, skipCount); // to: skipcount
					sqlInsert.BindTextRaw(48, lastskipped); // to: lastskipped
				}

				sqlInsert.StepResetClear();
			}
		}

		sqlInsert.Finalize();

		DBase::SQLRequest::Exec(dbPlaylist, "COMMIT;");

		dbPlaylist.Close();
	}

	return true;
}

bool DlgNewVersion::ConvertPlaylists()
{
	bool needSave = false;

	std::wstring filePlaylists = profilePath;
	filePlaylists += L"Playlists.xml";

	XmlFile xmlFile;
	if (xmlFile.LoadFile(filePlaylists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Playlists");

		// If update from version before 2.4 need to do this
		bool isOldVersion = false;
		if (!xmlMain)
		{
			xmlMain = xmlFile.RootNode().FirstChild("Playlist");
			if (xmlMain)
			{
				isOldVersion = true;
				xmlMain.SetName("Playlists");
				needSave = true;
			}
		}

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
			{
				std::wstring file = xmlNode.Attribute16("File");

				if (!file.empty())
				{
					// If update from version before 2.4 need to do this
					if (isOldVersion)
					{
						file = PathEx::NameFromFile(file);
						xmlNode.SetAttribute16("File", file);
					}

					ConvertPlaylist(file + L".db");
				}
			}
		}

		if (needSave)
			xmlFile.SaveFile(filePlaylists);
	}

	return true;
}

bool DlgNewVersion::ConvertSmartlists()
{
	std::wstring fileSmartlists = profilePath;
	fileSmartlists += L"Smartlists.xml";

	XmlFile xmlFile;
	if (xmlFile.LoadFile(fileSmartlists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Smartlists");
		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
			{
				std::wstring file = xmlNode.Attribute16("File");
				if (!file.empty())
				{
					std::wstring fileSmartlistXML = profilePath;
					fileSmartlistXML += L"SmartlistsOLD1";
					fileSmartlistXML.push_back('\\');
					fileSmartlistXML += file + L".xml";

					bool isAutoUpdate = true;
					XmlFile xmlFileSmartlist;
					if (xmlFileSmartlist.LoadFile(fileSmartlistXML))
					{
						XmlNode xmlMainSmartlist = xmlFileSmartlist.RootNode().FirstChild("Smartlist");
						if (xmlMainSmartlist)
						{
							XmlNode xmlOptions = xmlMainSmartlist.FirstChild("Options");
							if (xmlOptions)
								xmlOptions.Attribute("AutoUpdate", &isAutoUpdate);
						}

						std::wstring fileSmartlistNew = profilePath;
						fileSmartlistNew += L"Smartlists";
						fileSmartlistNew.push_back('\\');
						fileSmartlistNew += file + L".xml";

						xmlFileSmartlist.SaveFile(fileSmartlistNew);
					}

					if (!isAutoUpdate)
					{
						std::wstring fileSmartlistNew = profilePath;
						fileSmartlistNew += L"Smartlists";
						fileSmartlistNew.push_back('\\');
						fileSmartlistNew += file + L".db";

						DBase::SQLFile dbSmartlistNew;

						dbSmartlistNew.OpenCreate(fileSmartlistNew);

						CreateTableSmartlist(dbSmartlistNew);

						dbSmartlistNew.Close();
					}
				}
			}
		}
	}

	return true;
}

bool DlgNewVersion::ConvertHotKeysXML()
{
	bool needSave = false;

	std::wstring fileHotKeys = profilePath;
	fileHotKeys += L"HotKeys.xml";

	XmlFile xmlFile;
	if (xmlFile.LoadFile(fileHotKeys))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("HotKey");
		if (xmlMain)
		{
			needSave = true;
			xmlMain.SetName("HotKeys");
		}

		if (needSave)
			xmlFile.SaveFile(fileHotKeys);
	}

	return true;
}

void DlgNewVersion::CreateTableLibrary(const DBase::SQLFile& db)
{
	DBase::SQLRequest::Exec(db, "PRAGMA cache_size = 10000;"); // Use this instead of previous

	DBase::SQLRequest::Exec(db, "PRAGMA foreign_keys = ON;");

	DBase::SQLRequest::Exec(db, "BEGIN;");

	DBase::SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS library ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"  // Track ID (Primary key)
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

	DBase::SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS storage ("
		"spk INTEGER PRIMARY KEY,"
		"sid INTEGER REFERENCES library(id) ON DELETE CASCADE,"
		"sidx INTEGER,"
		"skey INTEGER,"
		"svalue TEXT);"
	);

	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS cue_index ON library(cue) WHERE cue IS NOT NULL;");
	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS filehash_index ON library(filehash);");
	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS trackhash_index ON library(trackhash);");

	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS sid_index ON storage(sid);");
	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS skey_index ON storage(skey);");

	DBase::SQLRequest::Exec(db, "COMMIT;");
}

void DlgNewVersion::CreateTablePlaylist(const DBase::SQLFile& db)
{
	DBase::SQLRequest::Exec(db, "PRAGMA foreign_keys = ON;");

	DBase::SQLRequest::Exec(db, "BEGIN;");

	DBase::SQLRequest::Exec(db,
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

	DBase::SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS storage ("
		"spk INTEGER PRIMARY KEY,"
		"sid INTEGER REFERENCES playlist(id) ON DELETE CASCADE,"
		"sidx INTEGER,"
		"skey INTEGER,"
		"svalue TEXT);"
	);

	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS sid_index ON storage(sid);");
	DBase::SQLRequest::Exec(db, "CREATE INDEX IF NOT EXISTS skey_index ON storage(skey);");

	DBase::SQLRequest::Exec(db, "COMMIT;");
}

void DlgNewVersion::CreateTableSmartlist(const DBase::SQLFile& db)
{
	DBase::SQLRequest::Exec(db,
		"CREATE TABLE IF NOT EXISTS smartlist ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT," // Track ID (Primary key)
		"idlib INTEGER,"           // Track ID in the library database
		"idx INTEGER,"             // Track index (for order in the playlist)
		"added INTEGER,"           // Date time when added to the smartlist
		"disabled INTEGER,"        // Disabled state
		"collapsed INTEGER);"      // Collapsed state
	);
}

void DlgNewVersion::ConvertPortable()
{
	std::wstring drive16(1, programPath[0]);
	std::string drive = UTF::UTF8S(drive16);
	if (drive.empty())
		return;

	ConvertPortableLibraryXML(convertPortableTo, drive);
	ConvertPortableLibraryDB(convertPortableTo, drive);
	ConvertPortablePlaylists(convertPortableTo, drive);
	//ConvertPortableSmartlists(convertPortableTo, drive);
}

void DlgNewVersion::ConvertPortableLibraryXML(bool toPortable, const std::string& drive)
{
	std::wstring fileLibrary = profilePath + L"Library.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(fileLibrary))
	{
		bool needSave = false;

		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Library");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild("Folder"); xmlNode; xmlNode = xmlNode.NextChild("Folder"))
			{
				std::string path = xmlNode.Attribute8("Path");
				if (!path.empty())
				{
					if (toPortable)
					{
						std::string pathLower(1, path[0]);
						std::string driveLower(1, drive[0]);
						pathLower = UTF::UTF8S(StringEx::ToLowerUS(UTF::UTF16S(pathLower)));
						driveLower = UTF::UTF8S(StringEx::ToLowerUS(UTF::UTF16S(driveLower)));

						if (pathLower[0] == driveLower[0])
						{
							path[0] = '?';
							xmlNode.SetAttribute8("Path", path);
							needSave = true;
						}
					}
					else // fromPortable
					{
						if (path[0] == '?')
						{
							path[0] = drive[0];
							xmlNode.SetAttribute8("Path", path);
							needSave = true;
						}
					}
				}
			}
		}

		if (needSave)
			xmlFile.SaveFile(fileLibrary);
	}
}

void DlgNewVersion::ConvertPortableLibraryDB(bool toPortable, const std::string& drive)
{
	std::wstring fileLibrary = profilePath + L"Library.db";

	DBase::SQLFile dbLibrary;

	if (dbLibrary.OpenWrite(fileLibrary))
	{
		// "UPDATE library SET path=?||substr(path,2) WHERE substr(path,1,1)==?;"
		DBase::SQLRequest sqlSelect(dbLibrary, "SELECT id,path,file FROM library;");
		DBase::SQLRequest sqlUpdate(dbLibrary, "UPDATE library SET filehash=?,path=? WHERE id=?;");

		DBase::SQLRequest::Exec(dbLibrary, "BEGIN;");
		ConvertPortableDB(toPortable, drive, sqlSelect, sqlUpdate);
		DBase::SQLRequest::Exec(dbLibrary, "COMMIT;");
	}

	dbLibrary.Close();
}

void DlgNewVersion::ConvertPortablePlaylists(bool toPortable, const std::string& drive)
{
	std::wstring filePlaylists = profilePath + L"Playlists.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(filePlaylists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Playlists");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild("Play"); xmlNode; xmlNode = xmlNode.NextChild("Play"))
			{
				std::wstring file = xmlNode.Attribute16("File");
				if (!file.empty())
				{
					std::wstring filePlaylist = profilePath;
					filePlaylist += L"Playlists";
					filePlaylist.push_back('\\');
					filePlaylist += file + L".db";

					DBase::SQLFile dbPlaylist;

					if (dbPlaylist.OpenWrite(filePlaylist))
					{
						DBase::SQLRequest sqlSelect(dbPlaylist, "SELECT id,path,file FROM playlist;");
						DBase::SQLRequest sqlUpdate(dbPlaylist, "UPDATE playlist SET filehash=?,path=? WHERE id=?;");

						DBase::SQLRequest::Exec(dbPlaylist, "BEGIN;");
						ConvertPortableDB(toPortable, drive, sqlSelect, sqlUpdate);
						DBase::SQLRequest::Exec(dbPlaylist, "COMMIT;");
					}

					dbPlaylist.Close();
				}
			}
		}
	}
}

void DlgNewVersion::ConvertPortableSmartlists(bool toPortable, const std::string& drive)
{
	std::wstring fileSmartlists = profilePath + L"Smartlists.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(fileSmartlists))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Smartlists");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild("Smart"); xmlNode; xmlNode = xmlNode.NextChild("Smart"))
			{
				std::wstring file = xmlNode.Attribute16("File");
				if (!file.empty())
				{
					std::wstring fileSmartlistXML = profilePath;
					fileSmartlistXML += L"Smartlists";
					fileSmartlistXML.push_back('\\');
					fileSmartlistXML += file + L".xml";

					bool isAutoUpdate = true;
					XmlFile xmlFileSmartlist;
					if (xmlFileSmartlist.LoadFile(fileSmartlistXML))
					{
						XmlNode xmlMainSmartlist = xmlFileSmartlist.RootNode().FirstChild("Smartlist");
						if (xmlMainSmartlist)
						{
							XmlNode xmlOptions = xmlMainSmartlist.FirstChild("Options");
							if (xmlOptions)
								xmlOptions.Attribute("AutoUpdate", &isAutoUpdate);
						}
					}

					if (!isAutoUpdate)
					{
						std::wstring fileSmartlist = profilePath;
						fileSmartlist += L"Smartlists";
						fileSmartlist.push_back('\\');
						fileSmartlist += file + L".db";

						DBase::SQLFile dbSmartlist;

						if (dbSmartlist.OpenWrite(fileSmartlist))
						{
							DBase::SQLRequest sqlSelect(dbSmartlist, "SELECT id,path,file FROM smartlist;");
							DBase::SQLRequest sqlUpdate(dbSmartlist, "UPDATE smartlist SET filehash=?,path=? WHERE id=?;");

							DBase::SQLRequest::Exec(dbSmartlist, "BEGIN;");
							ConvertPortableDB(toPortable, drive, sqlSelect, sqlUpdate);
							DBase::SQLRequest::Exec(dbSmartlist, "COMMIT;");
						}

						dbSmartlist.Close();
					}
				}
			}
		}
	}
}

void DlgNewVersion::ConvertPortableDB(bool toPortable, const std::string& drive, DBase::SQLRequest& sqlSelect, DBase::SQLRequest& sqlUpdate)
{
	while (sqlSelect.StepRow())
	{
		int id = sqlSelect.ColumnInt(0);

		std::string path = sqlSelect.ColumnText8(1);
		if (path.empty()) // url
			continue;

		std::string file = sqlSelect.ColumnText8(2);

		bool needUpdate = false;

		if (toPortable)
		{
			std::string pathLower(1, path[0]);
			std::string driveLower(1, drive[0]);
			pathLower = UTF::UTF8S(StringEx::ToLowerUS(UTF::UTF16S(pathLower)));
			driveLower = UTF::UTF8S(StringEx::ToLowerUS(UTF::UTF16S(driveLower)));

			if (pathLower[0] == driveLower[0])
			{
				path[0] = '?';
				needUpdate = true;
			}
		}
		else // fromPortable
		{
			if (path[0] == '?')
			{
				path[0] = drive[0];
				needUpdate = true;
			}
		}

		if (needUpdate)
		{
			std::wstring pathFile = UTF::UTF16S(path + file);

			sqlUpdate.BindInt(1, StringEx::HashFNV1a32(StringEx::ToLowerUS(pathFile)));
			sqlUpdate.BindText8(2, path);
			sqlUpdate.BindInt(3, id);

			sqlUpdate.StepReset();
		}
	}
}
