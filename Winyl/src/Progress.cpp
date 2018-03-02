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

#include "stdafx.h"
#include "Progress.h"

Progress::Progress()
{
}

Progress::~Progress()
{
}

void Progress::Init()
{
	// Get current drive for portable version
	if (isPortableVersion)
	{
		if (programPath[1] == ':') // Only for drives
		{
			programDrive = programPath[0];
			programDrive = StringEx::ToLowerUS(programDrive);
		}
	}

	// Get current time
	addedTime = FileSystem::GetTimeNow();

	if (!isNewPlaylist)
		funcUpdateProgressMarquee(true);

	if (isPortableVersion && !isPlaylist && !isNewPlaylist)
	{
		for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
		{
			if (!libraryFolders[i].empty() && libraryFolders[i][0] == '?')
				libraryFolders[i][0] = programPath[0];
		}
	}

	//threadWorker.StartBackground(std::bind(&DlgProgress::RunThread, this));
	threadWorker.Start(std::bind(&Progress::RunThread, this));
}

void Progress::Cancel()
{
	isStopThread = true;
}

void Progress::WaitForJoin()
{
	if (threadWorker.IsJoinable())
		threadWorker.Join();
}

void Progress::UpdateProgressText(const std::wstring& text)
{
	if (isProcessingText)
		isProcessingText = false;
	funcUpdateProgressText(text);
}

void Progress::UpdateProgressTextEmpty(bool cancel)
{
	if (cancel)
	{
		if (isProcessingText)
			isProcessingText = false;
		funcUpdateProgressTextEmpty(true);
	}
	else
	{
		if (!isProcessingText)
		{
			isProcessingText = true;
			funcUpdateProgressTextEmpty(false);
		}
	}
}

bool Progress::IsMusicFile(const std::wstring& file, bool* outCue)
{
	std::wstring ext = PathEx::ExtFromFile(file);

	if (ext.empty())
		return false;

	if (outCue && ext == L"cue")
	{
		*outCue = true;
		return true;
	}
	else if (ext == L"mp3")
		return true;
	else if (ext == L"ogg")
		return true;
	else if (ext == L"oga")
		return true;
	else if (ext == L"flac")
		return true;
	else if (ext == L"fla")
		return true;
	else if (ext == L"ape")
		return true;
	else if (ext == L"wma")
		return true;
	else if (ext == L"asf")
		return true;
	else if (ext == L"aac")
		return true;
	else if (ext == L"mp4")
		return true;
	else if (ext == L"m4a")
		return true;
	else if (ext == L"m4b")
		return true;
	else if (ext == L"m4r")
		return true;
	//else if (ext == L"m4p")
	//	return true;
	else if (ext == L"wav")
		return true;
	else if (ext == L"aif")
		return true;
	else if (ext == L"aiff")
		return true;
	else if (ext == L"wv")
		return true;
	else if (ext == L"mpc")
		return true;
	else if (ext == L"tta")
		return true;
	else if (ext == L"opus")
		return true;
	else if (ext == L"spx")
		return true;

	return false;
}

void Progress::CalculateFolder(const std::wstring& folder)
{
	assert(!folder.empty() && folder.back() == '\\');

	if (isStopThread)
		return;

	FileSystem::Find find(folder);

	while (find.Next())
	{
		if (isStopThread)
			return;

		if (find.IsDirectory())
		{
			// Do not add hidden folders to the library but add to playlists
			if (find.IsHidden() && !isPlaylist && !isNewPlaylist)
				continue;

			CalculateFolder(folder + find.GetFileName() + L"\\");
		}
		else
		{
			bool cue = false;
			if (IsMusicFile(find.GetFileName(), &cue))
			{
				if (cue) cueFiles.emplace_back(FileStruct{folder, find.GetFileName(), find.GetFileSize(), find.GetModified()});
				numberFiles++;
			}
		}
	}
}

bool Progress::CalculateFile(const std::wstring& file)
{
	assert(!file.empty() && file.back() != '\\');

	if (isStopThread)
		return true;

	FileSystem::FindFile findFile(file);

	if (findFile.IsFound())
	{
		if (!findFile.IsDirectory())
		{
			bool cue = false;
			if (IsMusicFile(findFile.GetFileName(), &cue))
			{
				if (cue) cueFiles.emplace_back(FileStruct{PathEx::PathFromFile(file),
					findFile.GetFileName(), findFile.GetFileSize(), findFile.GetModified()});
				numberFiles++;
				return true;
			}
		}
	}

	return false;
}

void Progress::AddFolderToLibrary(const std::wstring& folder, bool isLibraryEmpty)
{
	assert(!folder.empty() && folder.back() == '\\');

	if (isStopThread)
		return;

	FileSystem::Find find(folder);

	while (find.Next())
	{
		if (isStopThread)
			return;

		if (find.IsDirectory())
		{
			if (find.IsHidden())
				continue;

			AddFolderToLibrary(folder + find.GetFileName() + L"\\", isLibraryEmpty);
		}
		else
		{
			if (IsMusicFile(find.GetFileName()))
			{
				AddToLibrary(isLibraryEmpty, folder, find.GetFileName(), find.GetFileSize(), find.GetModified());

				funcUpdateProgressPos(++progressPos, numberFiles);
			}
		}
	}
}

void Progress::AddFolderToPlaylist(const std::wstring& folder, bool isTempPlaylist)
{
	assert(!folder.empty() && folder.back() == '\\');

	if (isStopThread)
		return;

	FileSystem::Find find(folder);

	while (find.Next())
	{
		if (isStopThread)
			return;

		std::wstring name = find.GetFileName();

		if (find.IsDirectory())
		{
			AddFolderToPlaylist(folder + name + L"\\", isTempPlaylist);
		}
		else
		{
			if (IsMusicFile(name))
			{
				UpdateProgressText(name);

				AddToPlaylist(isTempPlaylist, folder, name, find.GetFileSize(), find.GetModified());

				funcUpdateProgressPos(++progressPos, numberFiles);
			}
		}
	}
}

bool Progress::AddFileToPlaylist(const std::wstring& file, bool isTempPlaylist)
{
	assert(!file.empty() && file.back() != '\\');

	if (isStopThread)
		return true;

	FileSystem::FindFile findFile(file);

	if (findFile.IsFound())
	{
		if (!findFile.IsDirectory())
		{
			std::wstring name = findFile.GetFileName();

			if (IsMusicFile(name))
			{
				UpdateProgressText(name);

				std::wstring folder = PathEx::PathFromFile(file);

				AddToPlaylist(isTempPlaylist, folder, name, findFile.GetFileSize(), findFile.GetModified());

				funcUpdateProgressPos(++progressPos, numberFiles);

				return true;
			}
		}
	}

	return false;
}

void Progress::AddURLToPlaylist(const std::wstring& url, const std::wstring& title, bool isTempPlaylist)
{
	if (isStopThread)
		return;

	if (title.empty())
		dBase->AddURLToPlaylist(++numberOffset, addedTime, url, url, isTempPlaylist);
	else
		dBase->AddURLToPlaylist(++numberOffset, addedTime, url, title, isTempPlaylist);

	funcUpdateProgressPos(++progressPos, numberFiles);
}

int Progress::GetFileHash(const std::wstring& file, bool& outNoDrive)
{
	std::wstring fileLowerUS = StringEx::ToLowerUS(file);

	outNoDrive = false;
	if (isPortableVersion)
	{
		if (!programDrive.empty() && programDrive[0] == fileLowerUS[0])
		{
			fileLowerUS[0] = '?';
			outNoDrive = true;
		}
	}

	return StringEx::HashFNV1a32(fileLowerUS);
}

void Progress::AddToLibrary(bool isLibraryEmpty, const std::wstring& path, const std::wstring& file, long long fileSize, long long fileTime)
{
	bool noDrive = false;
	int fileHash = GetFileHash(path + file, noDrive);

	long long id = 0;
	long long size = 0;
	long long modified = 0;
	bool cue = false;

	if (dBase->CheckFile(noDrive, path, file, fileHash, 0, id, modified, size, cue, true)) // Already added
	{
		if (cue) return; // Do not add if the file is part of cue

		if (!isRescanAll && fileSize == size && fileTime == modified) // File isn't changed
		{
			UpdateProgressTextEmpty();

			dBase->SetUpdateOK(id);
		}
		else // File is added but tags are changed, need to update
		{
			UpdateProgressText(file);

			DBase::DATABASE_SONGINFO dataSongInfo;
			FillSongStruct(noDrive, path, file, fileHash, fileSize, fileTime, &dataSongInfo);

			dBase->UpdateTagsModified(id, &dataSongInfo);
			dBase->SetUpdateOK(id);
		}
	}
	else // File isn't added, need to add
	{
		UpdateProgressText(file);

		DBase::DATABASE_SONGINFO dataSongInfo;
		FillSongStruct(noDrive, path, file, fileHash, fileSize, fileTime, &dataSongInfo);

		long long idTags = 0;

		if (isFindMoved && !isLibraryEmpty && dBase->CheckTags(idTags, &dataSongInfo)) // Check tags, maybe it's moved file
		{
			dBase->UpdateTagsFileMove(idTags, &dataSongInfo);
			dBase->SetUpdateOK(idTags);
		}
		else // Add new file
			dBase->AddFileToLibrary(&dataSongInfo);
	}
}

void Progress::AddToPlaylist(bool isTempPlaylist, const std::wstring& path, const std::wstring& file, long long fileSize, long long fileTime, bool fast)
{
	bool noDrive = false;
	int fileHash = GetFileHash(path + file, noDrive);

	long long id = 0;
	long long size = 0;
	long long modified = 0;
	bool cue = false;

	if (dBase->CheckFile(noDrive, path, file, fileHash, 0, id, modified, size, cue) && (!fast || !cue)) // Already added
	{
		if (!fast && cue) return; // Do not add if the file is part of cue

		if (fileSize == size && fileTime == modified) // File isn't changed
		{
			dBase->AddFileToPlaylistFrom(id, ++numberOffset, addedTime, isTempPlaylist);
		}
		else // File is added but tags are changed, need to update
		{
			DBase::DATABASE_SONGINFO dataSongInfo;
			FillSongStruct(noDrive, path, file, fileHash, fileSize, fileTime, &dataSongInfo);

			dBase->UpdateTagsModified(id, &dataSongInfo);
			dBase->AddFileToPlaylistFrom(id, ++numberOffset, addedTime, isTempPlaylist);
		}
	}
	else // Add new file
	{
		DBase::DATABASE_SONGINFO dataSongInfo;
		FillSongStruct(noDrive, path, file, fileHash, fileSize, fileTime, &dataSongInfo);

		if (isAddAllToLibrary)
		{
			dBase->AddFileToLibrary(&dataSongInfo);
			dBase->AddFileToPlaylistFrom(dBase->GetLastAddedToLib(), ++numberOffset, addedTime, isTempPlaylist);
		}
		else
			dBase->AddFileToPlaylist(++numberOffset, &dataSongInfo, isTempPlaylist);
	}
}

void Progress::AddCueToLibrary(bool isLibraryEmpty, const std::wstring& path, const std::wstring& file, long long cueSize, long long cueTime)
{
	bool noDriveCue = false;
	int cueHash = GetFileHash(path + file, noDriveCue);

	long long cueID = 0;
	if (dBase->CheckCueFile(noDriveCue, path, file, cueHash, cueSize, cueTime, cueID, true, isRescanAll))
	{
		UpdateProgressTextEmpty();
		return;
	}

	UpdateProgressText(file);

	std::wstring reffile;
	bool refFound = false;
	long long refSize = 0;
	long long refTime = 0;

	CueFile cueFile;
	if (cueFile.LoadFile(path + file) && !cueFile.IsMultipleFiles())
	{
		reffile = UTF::UTF16S(cueFile.GetCue().tracks[0].file);

		FileSystem::FindFile findRef(path + reffile);
		if (findRef.IsFound())
		{
			refFound = true;
			refSize = findRef.GetFileSize();
			refTime = findRef.GetModified();
		}
		else // Try to find music file with same name as cue
		{
			FileSystem::Find findRefFile(path, PathEx::NameFromFile(reffile) + L".*");
			while (findRefFile.Next())
			{
				if (IsMusicFile(findRefFile.GetFileName()))
				{
					refFound = true;
					reffile = findRefFile.GetFileName();
					refSize = findRefFile.GetFileSize();
					refTime = findRefFile.GetModified();
				}
			}
		}
	}

	bool noDrive = false;
	int refHash = reffile.empty() ? 0 : GetFileHash(path + reffile, noDrive);

	if (cueID)
		dBase->UpdateCueFile(cueID, cueSize, cueTime, reffile, refHash);
	else
		dBase->AddCueFile(noDriveCue, path, file, cueHash, cueSize, cueTime, reffile, refHash);

	if (!refFound)
		return;

	///////

	dBase->DeleteCueImage(noDrive, path, reffile, refHash);

	std::unique_ptr<TagLibReader> tagLib;

	long long id = 0;
	long long size = 0;
	long long modified = 0;
	bool cue = false;

	for (std::size_t i = 0, isize = cueFile.GetCue().tracks.size(); i < isize; ++i)
	{
		long long cueValue = cueFile.GetCueValue(i);

		// The following part is similar to AddToLibrary function we can refactor it later
		// but right now I don't want to break something that already tested and working

		if (dBase->CheckFile(noDrive, path, reffile, refHash, cueValue, id, modified, size, cue, true)) // Already added
		{
			if (!isRescanAll && cueID == 0 && refTime == modified) // File isn't changed
			{
				dBase->SetUpdateOK(id);
			}
			else // File is added but tags are changed, need to update
			{
				DBase::DATABASE_SONGINFO dataSongInfo;
				FillSongStruct(noDrive, path, reffile, refHash, refSize, refTime, &dataSongInfo, &tagLib, &cueFile, i);

				dBase->UpdateTagsModified(id, &dataSongInfo);
				dBase->SetUpdateOK(id);
			}
		}
		else // File isn't added, need to add
		{
			DBase::DATABASE_SONGINFO dataSongInfo;
			FillSongStruct(noDrive, path, reffile, refHash, refSize, refTime, &dataSongInfo, &tagLib, &cueFile, i);

			long long idTags = 0;

			if (isFindMoved && !isLibraryEmpty && dBase->CheckTags(idTags, &dataSongInfo)) // Check tags, maybe it's moved file
			{
				dBase->UpdateTagsFileMove(idTags, &dataSongInfo);
				dBase->SetUpdateOK(idTags);
			}
			else // Add new file
				dBase->AddFileToLibrary(&dataSongInfo);
		}
	}
}

bool Progress::AddCueToPlaylist(bool isTempPlaylist, const std::wstring& path, const std::wstring& file, long long cueSize, long long cueTime)
{
	std::wstring reffile;
	bool refFound = false;
	long long refSize = 0;
	long long refTime = 0;

	CueFile cueFile;
	if (cueFile.LoadFile(path + file) && !cueFile.IsMultipleFiles())
	{
		reffile = UTF::UTF16S(cueFile.GetCue().tracks[0].file);

		FileSystem::FindFile findRef(path + reffile);
		if (findRef.IsFound())
		{
			refFound = true;
			refSize = findRef.GetFileSize();
			refTime = findRef.GetModified();
		}
		else // Try to find music file with same name as cue
		{
			FileSystem::Find findRefFile(path, PathEx::NameFromFile(reffile) + L".*");
			while (findRefFile.Next())
			{
				if (IsMusicFile(findRefFile.GetFileName()))
				{
					refFound = true;
					reffile = findRefFile.GetFileName();
					refSize = findRefFile.GetFileSize();
					refTime = findRefFile.GetModified();
				}
			}
		}
	}

	bool noDriveCue = false;
	int cueHash = GetFileHash(path + file, noDriveCue);

	bool noDrive = false;
	int refHash = reffile.empty() ? 0 : GetFileHash(path + reffile, noDrive);

	long long cueID = 0;
	if (!dBase->CheckCueFile(noDriveCue, path, file, cueHash, cueSize, cueTime, cueID))
	{
		if (cueID)
			dBase->UpdateCueFile(cueID, cueSize, cueTime, reffile, refHash);
	}

	if (!refFound)
		return false;

	if (!cueID)
		dBase->AddCueFile(noDriveCue, path, file, cueHash, cueSize, cueTime, reffile, refHash);

	///////

	std::unique_ptr<TagLibReader> tagLib;

	long long id = 0;
	long long size = 0;
	long long modified = 0;
	bool cue = false;

	for (std::size_t i = 0, isize = cueFile.GetCue().tracks.size(); i < isize; ++i)
	{
		long long cueValue = cueFile.GetCueValue(i);

		// The following part is similar to AddToPlaylist function we can refactor it later
		// but right now I don't want to break something that already tested and working

		if (dBase->CheckFile(noDrive, path, reffile, refHash, cueValue, id, modified, size, cue)) // Already added
		{
			if (cueID == 0 && refTime == modified) // File isn't changed
			{
				dBase->AddFileToPlaylistFrom(id, ++numberOffset, addedTime, isTempPlaylist);
			}
			else // File is added but tags are changed, need to update
			{
				DBase::DATABASE_SONGINFO dataSongInfo;
				FillSongStruct(noDrive, path, reffile, refHash, refSize, refTime, &dataSongInfo, &tagLib, &cueFile, i);

				dBase->UpdateTagsModified(id, &dataSongInfo);
				dBase->AddFileToPlaylistFrom(id, ++numberOffset, addedTime, isTempPlaylist);
			}
		}
		else // File isn't added, need to add
		{
			DBase::DATABASE_SONGINFO dataSongInfo;
			FillSongStruct(noDrive, path, reffile, refHash, refSize, refTime, &dataSongInfo, &tagLib, &cueFile, i);

			if (isAddAllToLibrary)
			{
				dBase->AddFileToLibrary(&dataSongInfo);
				dBase->AddFileToPlaylistFrom(dBase->GetLastAddedToLib(), ++numberOffset, addedTime, isTempPlaylist);
			}
			else // Add new file
				dBase->AddFileToPlaylist(++numberOffset, &dataSongInfo, isTempPlaylist);
		}
	}

	return true;
}

void Progress::FillSongStruct(bool noDrive, const std::wstring& path, const std::wstring& file, int fileHash, long long fileSize, long long fileTime,
	DBase::DATABASE_SONGINFO* dataSongInfo, std::unique_ptr<TagLibReader> *tag, CueFile *cue, std::size_t i)
{
	if (tag == nullptr)
	{
		TagLibReader tagLib;
		tagLib.ReadFileTags(path + file);

		dataSongInfo->track       = tagLib.tags.track;
		dataSongInfo->totalTracks = tagLib.tags.totalTracks;
		dataSongInfo->disc        = tagLib.tags.disc;
		dataSongInfo->totalDiscs  = tagLib.tags.totalDiscs;
		dataSongInfo->title       = tagLib.tags.title;
		dataSongInfo->album       = tagLib.tags.album;
		dataSongInfo->artist      = tagLib.tags.artist;
		dataSongInfo->albumArtist = tagLib.tags.albumArtist;
		dataSongInfo->composer    = tagLib.tags.composer;
		dataSongInfo->genre       = tagLib.tags.genre;
		dataSongInfo->year        = tagLib.tags.year;
		dataSongInfo->bpm         = tagLib.tags.bpm;
		dataSongInfo->compilation = tagLib.tags.compilation;
		dataSongInfo->publisher   = tagLib.tags.publisher;
		dataSongInfo->conductor   = tagLib.tags.conductor;
		dataSongInfo->lyricist    = tagLib.tags.lyricist;
		dataSongInfo->remixer     = tagLib.tags.remixer;
		dataSongInfo->grouping    = tagLib.tags.grouping;
		dataSongInfo->subtitle    = tagLib.tags.subtitle;
		dataSongInfo->copyright   = tagLib.tags.copyright;
		dataSongInfo->encodedby   = tagLib.tags.encodedby;
		dataSongInfo->comment     = tagLib.tags.comment;
		dataSongInfo->duration    = tagLib.tags.duration;
		dataSongInfo->channels    = tagLib.tags.channels;
		dataSongInfo->bitrate     = tagLib.tags.bitrate;
		dataSongInfo->samplerate  = tagLib.tags.sampleRate;
		dataSongInfo->cue         = 0;
		dataSongInfo->size        = fileSize;

		dataSongInfo->artists      = tagLib.tags.artists;
		dataSongInfo->albumArtists = tagLib.tags.albumArtists;
		dataSongInfo->genres       = tagLib.tags.genres;
		dataSongInfo->composers    = tagLib.tags.composers;
		dataSongInfo->conductors   = tagLib.tags.conductors;
		dataSongInfo->lyricists    = tagLib.tags.lyricists;
	}
	else
	{
		if (!tag->get())
		{
			tag->reset(new TagLibReader());
			tag->get()->ReadFileTags(path + file);
		}

		// Merge tags from cue and file tags
		const CueFile::CueHeader& cueHeader = cue->GetCue();
		const CueFile::CueTrack& cueTrack = cueHeader.tracks[i];

		dataSongInfo->track       = std::to_string(i + 1);
		dataSongInfo->totalTracks = std::to_string(cueHeader.tracks.size());
		dataSongInfo->title       = cueTrack.title;
		
		dataSongInfo->cue         = cue->GetCueValue(i);
		dataSongInfo->duration    = cue->GetCueTime(i, tag->get()->tags.duration);
		dataSongInfo->size        = cue->GetCueFileSize(i, tag->get()->tags.duration, fileSize);

		dataSongInfo->album       = !cueHeader.album.empty() ? cueHeader.album : tag->get()->tags.album;
		dataSongInfo->year        = !cueHeader.year.empty() ? cueHeader.year : tag->get()->tags.year;
		dataSongInfo->comment     = !cueHeader.comment.empty() ? cueHeader.comment : tag->get()->tags.comment;

		dataSongInfo->disc        = !cueHeader.discNumber.empty() ? cueHeader.discNumber : tag->get()->tags.disc;
		dataSongInfo->totalDiscs  = !cueHeader.totalDiscs.empty() ? cueHeader.totalDiscs : tag->get()->tags.totalDiscs;

		dataSongInfo->artist      = !cueTrack.artist.empty() ? cueTrack.artist : tag->get()->tags.artist;
		dataSongInfo->lyricist    = !cueTrack.lyricist.empty() ? cueTrack.lyricist : tag->get()->tags.lyricist;
		dataSongInfo->albumArtist = !cueHeader.albumArtist.empty() ? cueHeader.albumArtist : tag->get()->tags.albumArtist;
		dataSongInfo->genre       = !cueHeader.genre.empty() ? cueHeader.genre : tag->get()->tags.genre;

		if (cueTrack.artist.empty())       {dataSongInfo->artists      = tag->get()->tags.artists;}
		if (cueTrack.lyricist.empty())     {dataSongInfo->lyricists    = tag->get()->tags.lyricists;}
		if (cueHeader.albumArtist.empty()) {dataSongInfo->albumArtists = tag->get()->tags.albumArtists;}
		if (cueHeader.genre.empty())       {dataSongInfo->genres       = tag->get()->tags.genres;}

		dataSongInfo->composer    = tag->get()->tags.composer;
		dataSongInfo->bpm         = tag->get()->tags.bpm;
		dataSongInfo->compilation = tag->get()->tags.compilation;
		dataSongInfo->publisher   = tag->get()->tags.publisher;
		dataSongInfo->conductor   = tag->get()->tags.conductor;
		dataSongInfo->remixer     = tag->get()->tags.remixer;
		dataSongInfo->grouping    = tag->get()->tags.grouping;
		dataSongInfo->copyright   = tag->get()->tags.copyright;
		dataSongInfo->encodedby   = tag->get()->tags.encodedby;
		dataSongInfo->channels    = tag->get()->tags.channels;
		dataSongInfo->bitrate     = tag->get()->tags.bitrate;
		dataSongInfo->samplerate  = tag->get()->tags.sampleRate;

		dataSongInfo->composers    = tag->get()->tags.composers;
		dataSongInfo->conductors   = tag->get()->tags.conductors;
	}

	// Fill file hash
	dataSongInfo->fileHash = fileHash;

	// Fill track hash
	std::wstring titleAlbumArtist;
	if (!dataSongInfo->title.empty())
		titleAlbumArtist += UTF::UTF16S(dataSongInfo->title);
	else
		titleAlbumArtist += file;
	titleAlbumArtist += UTF::UTF16S(dataSongInfo->album);
	titleAlbumArtist += UTF::UTF16S(dataSongInfo->artist);
	dataSongInfo->trackHash = StringEx::HashFNV1a32(titleAlbumArtist);

	//dataSongInfo->size = fileSize;

	dataSongInfo->modified = fileTime;

	dataSongInfo->path = UTF::UTF8S(path);
	if (noDrive)
		dataSongInfo->path[0] = '?';
	dataSongInfo->file = UTF::UTF8S(file);
	dataSongInfo->added = addedTime;
}

void Progress::CheckLibraryFiles()
{
	if (isStopThread)
		return;

	UpdateProgressTextEmpty();

	dBase->DeleteLibFromPls();

	int numberCues = dBase->GetCueCountLibFromPls();

	numberFiles += dBase->GetCountLibFromPls() + numberCues;

	funcUpdateProgressRange(progressPos, numberFiles);

	// Cue files first
	if (numberCues) CheckCueLibraryFiles();

	DBase::SQLRequest sqlSelect;
	dBase->FindFirstLibFromPlsFile(sqlSelect);

	long long id = 0;
	std::wstring path;
	std::wstring file;
	long long modified = 0;
	long long size = 0;
	bool cue = false;

	while (dBase->FindNextLibFromPlsFile(sqlSelect, id, path, file, modified, size, cue))
	{
		if (isStopThread)
			return;

		if (cue) // Just skip all cue for now
		{
			funcUpdateProgressPos(++progressPos, numberFiles);
			continue;
		}

		if (isPortableVersion && path[0] == '?')
			path[0] = programPath[0];

		FileSystem::FindFile findFile(path + file);

		if (findFile.IsFound())
		{
			long long fileSize = findFile.GetFileSize();
			long long fileTime = findFile.GetModified();

			///////

			if (!isRescanAll && fileSize == size && fileTime == modified)
			{
				UpdateProgressTextEmpty();

				dBase->SetUpdateOK(id);
			}
			else
			{
				std::wstring filename = findFile.GetFileName();

				UpdateProgressText(filename);

				bool noDrive = false;
				int fileHash = GetFileHash(path + filename, noDrive);

				DBase::DATABASE_SONGINFO dataSongInfo;
				FillSongStruct(noDrive, path, filename, fileHash, fileSize, fileTime, &dataSongInfo);

				dBase->UpdateTagsModified(id, &dataSongInfo);
				dBase->SetUpdateOK(id);
			}
		}

		funcUpdateProgressPos(++progressPos, numberFiles);
	}
}

void Progress::CheckCueLibraryFiles()
{
	DBase::SQLRequest sqlSelect;
	dBase->FindFirstCueLibFromPlsFile(sqlSelect);

	long long id = 0;
	std::wstring path;
	std::wstring file;
	long long modified = 0;
	long long size = 0;
	std::wstring reffile;
	int refhash = 0;

	while (dBase->FindNextCueLibFromPlsFile(sqlSelect, id, path, file, modified, size, reffile, refhash))
	{
		if (isStopThread)
			return;

		if (reffile.empty())
			continue;

		bool noDrive = false;
		if (isPortableVersion && path[0] == '?')
		{
			noDrive = true;
			path[0] = programPath[0];
		}

		FileSystem::FindFile findFile(path + file);

		if (findFile.IsFound())
		{
			long long fileSize = findFile.GetFileSize();
			long long fileTime = findFile.GetModified();

			if (!isRescanAll && fileSize == size && fileTime == modified)
			{
				UpdateProgressTextEmpty();

				// Leave cue only if the library does have files from this cue
				if (dBase->UpdateCueLibrary(noDrive, path, reffile, refhash))
					dBase->SetUpdateCueOK(id);
			}
			else // Just use AddCueToLibrary here, it's not effective but it works
				AddCueToLibrary(false, path, file, fileSize, fileTime);
		}

		funcUpdateProgressPos(++progressPos, numberFiles);
	}
}

void Progress::RunThread()
{
	if (isPlaylist)
		ThreadPlaylist();
	else if (isNewPlaylist)
		ThreadNewPlaylist();
	else
		ThreadLibrary();

	funcFinish();
}

void Progress::ThreadLibrary()
{
	// Calculate the number of files
	for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
	{
		CalculateFolder(libraryFolders[i]);
		if (isStopThread) break;
	}

	if (isStopThread) // Exit if press stop
		return;

	// Check if the database is empty
	bool isLibraryEmpty = dBase->IsLibraryEmpty();

	dBase->MemFlagAttach();
	dBase->Begin();
	dBase->CueBegin();

	//if (!isLibraryEmpty)
	dBase->SetUpdateAll();
	dBase->SetUpdateAllCue();


	// Prepare the progress bar
	funcUpdateProgressMarquee(false);
	funcUpdateProgressRange(0, numberFiles);

	//LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
	//LARGE_INTEGER counter1; QueryPerformanceCounter(&counter1);

	for (std::size_t i = 0, size = cueFiles.size(); i < size; ++i)
	{
		AddCueToLibrary(isLibraryEmpty, cueFiles[i].path, cueFiles[i].file, cueFiles[i].fileSize, cueFiles[i].fileTime);
		if (isStopThread) break;

		funcUpdateProgressPos(++progressPos, numberFiles);
	}

	for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
	{
		AddFolderToLibrary(libraryFolders[i], isLibraryEmpty);
		if (isStopThread) break;
	}

	//LARGE_INTEGER counter2; QueryPerformanceCounter(&counter2);
	//double perfDiffMs = (counter2.QuadPart - counter1.QuadPart) *1000.0 / freq.QuadPart;
	//int i = 0;
	//::MessageBoxW(NULL, std::to_wstring(perfDiffMs).c_str(), L"", 0);

	if (isAddAllToLibrary)
	{
		CheckLibraryFiles();
	}

	// Because Vacuum can be slow
	UpdateProgressTextEmpty(isStopThread);


	if (!isLibraryEmpty && !isStopThread)
	{
		if (isRemoveMissing)
			dBase->DeleteNotUpdated();

		if (!isIgnoreDeleted)
			dBase->RestoreDeleted();

		dBase->DeleteNotUpdatedCue();
	}

	dBase->SetUpdateEndCue();
	dBase->SetUpdateEnd();

	dBase->Commit();
	dBase->CueCommit();
	dBase->MemFlagDetach();
	// VACUUM causes UI thread to hang when access to db, need to do someting with this
	//dBase->Vacuum(); // Don't use VACUUM it's very slow and lock the database
}

void Progress::ThreadPlaylist()
{
	// Calculate the number of files
	for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
	{
		if (!CalculateFile(libraryFolders[i]))
		{
			if (libraryFolders[i].size() > 3) // Path
				CalculateFolder(libraryFolders[i] + L"\\");
			else // Disk
				CalculateFolder(libraryFolders[i]);
		}
		if (isStopThread) break;
	}

	if (isStopThread) // Exit if press stop
		return;

	dBase->SortHelperAttach();

	dBase->PlayBegin();
	if (isAddAllToLibrary)
	{
		dBase->Begin();
		dBase->CueBegin();
	}

	// Prepare the progress bar
	funcUpdateProgressMarquee(false);
	funcUpdateProgressRange(0, numberFiles);

	for (std::size_t i = 0, size = cueFiles.size(); i < size; ++i)
	{
		UpdateProgressText(cueFiles[i].file);

		AddCueToPlaylist(false, cueFiles[i].path, cueFiles[i].file, cueFiles[i].fileSize, cueFiles[i].fileTime);
		if (isStopThread) break;

		funcUpdateProgressPos(++progressPos, numberFiles);
	}

	for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
	{
		if (!AddFileToPlaylist(libraryFolders[i], false))
		{
			if (libraryFolders[i].size() > 3) // Path
				AddFolderToPlaylist(libraryFolders[i] + L"\\", false);
			else // Disk
				AddFolderToPlaylist(libraryFolders[i], false);
		}
		if (isStopThread) break;
	}

	// Because Vacuum can be slow
	UpdateProgressTextEmpty(isStopThread);

	if (isAddAllToLibrary)
	{
		dBase->Commit();
		dBase->CueCommit();
	}

	// Sort the playlist before add
	dBase->SortPlaylist(numberStart, namePlaylist);

	dBase->PlayCommit();

	dBase->SortHelperDetach();

	//dBase->PlayVacuum(); // Don't use VACUUM it's very slow and lock the database
}

void Progress::ThreadNewPlaylist()
{
	// Load playlist
	PlsFile plsFile;
	plsFile.LoadPlaylist(filePlaylist);

	if (isStopThread) // Exit if press stop
		return;

	// Calculate the number of files
	for (int i = 0, size = plsFile.GetPlsSize(); i < size; ++i)
	{
		if (plsFile.IsRadioURL(i))
			numberFiles++;
		else
			CalculateFile(plsFile.GetPlsFile(i));

		if (isStopThread) break;
	}

	if (isStopThread) // Exit if press stop
		return;

	// Open temp playlist and use it further
	dBase->OpenTemp(fileDatabase);
	dBase->TempBegin();
	if (isAddAllToLibrary)
	{
		dBase->Begin();
		dBase->CueBegin();
	}

	// Prepare the progress bar
	funcUpdateProgressRange(0, numberFiles);

	for (std::size_t i = 0, size = cueFiles.size(); i < size; ++i)
	{
		UpdateProgressText(cueFiles[i].file);

		AddCueToPlaylist(true, cueFiles[i].path, cueFiles[i].file, cueFiles[i].fileSize, cueFiles[i].fileTime);
		if (isStopThread) break;

		funcUpdateProgressPos(++progressPos, numberFiles);
	}

	for (int i = 0, size = plsFile.GetPlsSize(); i < size; ++i)
	{
		if (plsFile.IsRadioURL(i))
			AddURLToPlaylist(plsFile.GetPlsFile(i), plsFile.GetPlsTitle(i), true);
		else
			AddFileToPlaylist(plsFile.GetPlsFile(i), true);

		if (isStopThread) break;
	}

	// Because Vacuum can be slow
	UpdateProgressTextEmpty(isStopThread);

	dBase->TempCommit();
	if (isAddAllToLibrary)
	{
		dBase->Commit();
		dBase->CueCommit();
	}

	//dBase->TempVacuum(); // Don't use VACUUM it's very slow and lock the database
	dBase->CloseTemp();
}

bool Progress::FastAddFileToPlaylist(const std::wstring& musicfile, int start, bool& isFolder)
{
	// Get current drive for portable version
	if (isPortableVersion)
	{
		if (programPath[1] == ':') // Only for drives
		{
			programDrive = programPath[0];
			programDrive = StringEx::ToLowerUS(programDrive);
		}
	}

	numberOffset = start;

	// Get current time
	addedTime = FileSystem::GetTimeNow();

	FileSystem::FindFile findFile(musicfile);

	if (findFile.IsFound())
	{
		if (!findFile.IsDirectory())
		{
			std::wstring file = findFile.GetFileName();

			bool cueFile = false;
			if (IsMusicFile(file, &cueFile))
			{
				if (!cueFile)
				{
					std::wstring path = PathEx::PathFromFile(musicfile);

					long long fileSize = findFile.GetFileSize();
					long long fileTime = findFile.GetModified();

					AddToPlaylist(false, path, file, fileSize, fileTime, true);

					return true;
				}
				else
				{
					dBase->PlayBegin();
					if (isAddAllToLibrary)
					{
						dBase->Begin();
						dBase->CueBegin();
					}

					std::wstring path = PathEx::PathFromFile(musicfile);

					long long cueSize = findFile.GetFileSize();
					long long cueTime = findFile.GetModified();

					bool result = AddCueToPlaylist(false, path, file, cueSize, cueTime);

					dBase->PlayCommit();
					if (isAddAllToLibrary)
					{
						dBase->Commit();
						dBase->CueCommit();
					}

					return result;
				}
			}
		}
		else
		{
			isFolder = true;
		}
	}

	return false;
}

