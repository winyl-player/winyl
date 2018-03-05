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

// Properties.cpp : implementation file
//

#include "stdafx.h"
#include "Properties.h"
#include "FileSystem.h"


// Properties

Properties::Properties()
{

}

Properties::~Properties()
{

}

void Properties::Init()
{
	if (skinList == nullptr)
		return;

	if (!listNode)
	{
		if (skinList->GetSelectedSize() == 1)
			listNode = skinList->GetSelectedAt(0);
		else if (skinList->GetSelectedSize() == 0)
			return;
	}

	if (listNode)
	{
		isMultiple = false;

		if (skinList->IsRadio())
		{
			selectedNodes.emplace_back(listNode);

			fields.file = listNode->GetFile();

			isRadioDefault = true;
		}
		else
		{
			DBase::DATABASE_GETINFO info;
			dBase->GetLibFile(listNode->idLibrary, listNode->idPlaylist, &info);

			selectedNodes.emplace_back(listNode);

			fields.file        = info.file;
			fields.track       = info.track;
			fields.disc        = info.disc;
			fields.totalTracks = info.totalTracks;
			fields.totalDiscs  = info.totalDiscs;
			fields.title       = info.title;
			fields.album       = info.album;
			fields.artist      = MergeMultiple(info.artist, info.artists);
			fields.albumArtist = MergeMultiple(info.albumArtist, info.albumArtists);
			fields.composer    = MergeMultiple(info.composer, info.composers);
			fields.genre       = MergeMultiple(info.genre, info.genres);
			fields.year        = info.year;
			fields.lyricist    = MergeMultiple(info.lyricist, info.lyricists);
			fields.comment     = info.comment;

			isPartOfCue = info.cue;

			if (info.type == (int)DBase::Category::UserRadio)
				isRadioCustom = true;
			else if (info.type == (int)DBase::Category::Radio)
				isRadioDefault = true;
			else
			{
				std::wstring ext = PathEx::ExtFromFile(info.file);
				StringEx::MakeUpperAscii(ext);

				if (info.channels == L"2")
					fields.quality = StringEx::Format(L"%s, %s kbps, %s hz, Stereo", ext.c_str(), info.bitrate.c_str(), info.samplerate.c_str());
				else if (info.channels == L"1")
					fields.quality = StringEx::Format(L"%s, %s kbps, %s hz, Mono", ext.c_str(), info.bitrate.c_str(), info.samplerate.c_str());
				else
					fields.quality = StringEx::Format(L"%s, %s kbps, %s hz, %s chans", ext.c_str(), info.bitrate.c_str(), info.samplerate.c_str(), info.channels.c_str());

				if (info.cue)
					fields.quality = L"CUE+" + fields.quality;
			}
		}

		if (isRadioCustom || isRadioDefault)
		{
			if (skinList->GetPlayNode() != listNode)
				return;

			std::wstring meta;
			int type = 0, kbps = 0, chans = 0, freq = 0;
			if (!libAudio->GetRadioInfo(type, kbps, chans, freq, meta))
				return;

			fields.title = meta;

			std::wstring ext;
			switch (type)
			{
				case 1: ext = L"MP3"; break;
				case 2: ext = L"OGG"; break;
				case 3: ext = L"WMA"; break;
				case 4: ext = L"AAC"; break;
				case 5: ext = L"MP4"; break;
				default: ext = L"Unknown";
			}

			if (chans == 2)
				fields.quality = StringEx::Format(L"%s, %i kbps, %i hz, Stereo", ext.c_str(), kbps, freq);
			else if (chans == 1)
				fields.quality = StringEx::Format(L"%s, %i kbps, %i hz, Mono", ext.c_str(), kbps, freq);
			else
				fields.quality = StringEx::Format(L"%s, %i kbps, %i hz, %i chans", ext.c_str(), kbps, freq, chans);
		}
	}
	else
	{
		isMultiple = true;

		for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
		{
			DBase::DATABASE_GETINFO info;
			dBase->GetLibFile(skinList->GetSelectedAt(i)->idLibrary, skinList->GetSelectedAt(i)->idPlaylist, &info);

			if (info.type == 100 || info.type == 1000)
			{
				selectedNodes.clear();
				break;
			}

			selectedNodes.emplace_back(skinList->GetSelectedAt(i));

			if (i == 0)
			{
				fields.track       = info.track;
				fields.disc        = info.disc;
				fields.totalTracks = info.totalTracks;
				fields.totalDiscs  = info.totalDiscs;
				fields.title       = info.title;
				fields.album       = info.album;
				fields.artist      = MergeMultiple(info.artist, info.artists);
				fields.albumArtist = MergeMultiple(info.albumArtist, info.albumArtists);
				fields.composer    = MergeMultiple(info.composer, info.composers);
				fields.genre       = MergeMultiple(info.genre, info.genres);
				fields.year        = info.year;
				fields.lyricist    = MergeMultiple(info.lyricist, info.lyricists);
				fields.comment     = info.comment;
			}
			else
			{
				if (!fields.track.empty() && fields.track != info.track)
					fields.track.clear();
				if (!fields.disc.empty() && fields.disc != info.disc)
					fields.disc.clear();
				if (!fields.totalTracks.empty() && fields.totalTracks != info.totalTracks)
					fields.totalTracks.clear();
				if (!fields.totalDiscs.empty() && fields.totalDiscs != info.totalDiscs)
					fields.totalDiscs.clear();
				if (!fields.title.empty() && fields.title != info.title)
					fields.title.clear();
				if (!fields.album.empty() && fields.album != info.album)
					fields.album.clear();
				if (!fields.artist.empty() && fields.artist != MergeMultiple(info.artist, info.artists))
					fields.artist.clear();
				if (!fields.albumArtist.empty() && fields.albumArtist != MergeMultiple(info.albumArtist, info.albumArtists))
					fields.albumArtist.clear();
				if (!fields.composer.empty() && fields.composer != MergeMultiple(info.composer, info.composers))
					fields.composer.clear();
				if (!fields.genre.empty() && fields.genre != MergeMultiple(info.genre, info.genres))
					fields.genre.clear();
				if (!fields.year.empty() && fields.year != info.year)
					fields.year.clear();
				if (!fields.lyricist.empty() && fields.lyricist != MergeMultiple(info.lyricist, info.lyricists))
					fields.lyricist.clear();
				if (!fields.comment.empty() && fields.comment != info.comment)
					fields.comment.clear();
			}
		}
	}
}

ListNodeUnsafe Properties::NextTrack()
{
	if (listNode)
	{
		ListNodeUnsafe node = skinList->FindNextTrack(listNode, true);
		if (node)
		{
			skinList->RemoveSelection();
			skinList->SetFocusNode(node);
			skinList->ScrollToFocusNode();
			return node;
		}
	}

	return nullptr;
}

ListNodeUnsafe Properties::PrevTrack()
{
	if (listNode)
	{
		ListNodeUnsafe node = skinList->FindPrevTrack(listNode, true);
		if (node)
		{
			skinList->RemoveSelection();
			skinList->SetFocusNode(node);
			skinList->ScrollToFocusNode();
			return node;
		}
	}

	return nullptr;
}

bool Properties::SaveTags()
{
	if (!needSaveTags &&
		(!needSaveCover || (!saveCoverToTags && !saveCoverToFile)) &&
		(!needSaveLyrics || (!saveLyricsToTags && !saveLyricsToFile)))
		return false;

	if (!IsMultiple())
	{
		bool isPlayNode = false;
		if (selectedNodes[0].get() == skinList->GetPlayNode())
			isPlayNode = true;

		//if (isPlayNode) libAudio->TempStop();
		bool result = WriteTags(selectedNodes[0].get());
		//if (isPlayNode) libAudio->TempPlay();

		if (result)
			UpdateTags(selectedNodes[0].get());
	}
	else
	{
		threadWorker.Start(std::bind(&Properties::RunThread, this));
	}

	return true;
}

void Properties::WaitThread()
{
	if (threadWorker.IsJoinable())
		threadWorker.Join();
}

void Properties::RunThread()
{
	dBase->UpdateTagsBegin();
	for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
	{
		bool isPlayNode = false;
		if (selectedNodes[i].get() == skinList->GetPlayNode())
			isPlayNode = true;

		//if (isPlayNode) libAudio->TempStop();
		bool result = WriteTags(selectedNodes[i].get());
		//if (isPlayNode) libAudio->TempPlay();

		if (result)
			UpdateTags(selectedNodes[i].get());
		//else
		//	break;

		funcProgress((int)i + 1);
	}
	dBase->UpdateTagsCommit();

	funcThreadEnd();
}

bool Properties::WriteTags(ListNodeUnsafe node)
{
	// Skip CUE
	if (node->GetCueValue())
	{
		// When CUE and multiple files are selected it's possibly to SaveCoverToFile
		if (needSaveCover && saveCoverToFile)
			SaveCoverToFile(node->GetFile());

		return false;
	}

	if (PathEx::IsURL(node->GetFile()))
		return false;

	if (needSaveLyrics)
	{
		if (saveLyricsToTags)
			tagLibW.lyrics.SetLyrics(newLyrics);
		if (saveLyricsToFile)
			SaveLyricsToFile(node->GetFile());
	}

	if (needSaveCover)
	{
		if (saveCoverToTags)
			tagLibW.cover.newCover = &newCover;
		if (saveCoverToFile)
			SaveCoverToFile(node->GetFile());
	}

	if (needSaveTags)
		return tagLibW.SaveFileTags(node->GetFile());
	else if ((needSaveLyrics && saveLyricsToTags) || (needSaveCover && saveCoverToTags))
		tagLibW.SaveFileTags(node->GetFile());

	return false;
}

void Properties::UpdateTags(ListNodeUnsafe node)
{
	FileSystem::FindFile findFile(node->GetFile());
	if (!findFile.IsFound())
		return;

	TagLibReader tagLib;
	if (!tagLib.ReadFileTags(node->GetFile()))
		return;

	DBase::DATABASE_SONGINFO dataSongInfo;

	dataSongInfo.track       = tagLib.tags.track;
	dataSongInfo.totalTracks = tagLib.tags.totalTracks;
	dataSongInfo.disc        = tagLib.tags.disc;
	dataSongInfo.totalDiscs  = tagLib.tags.totalDiscs;
	dataSongInfo.title       = tagLib.tags.title;
	dataSongInfo.album       = tagLib.tags.album;
	dataSongInfo.artist      = tagLib.tags.artist;
	dataSongInfo.albumArtist = tagLib.tags.albumArtist;
	dataSongInfo.composer    = tagLib.tags.composer;
	dataSongInfo.genre       = tagLib.tags.genre;
	dataSongInfo.year        = tagLib.tags.year;
	dataSongInfo.bpm         = tagLib.tags.bpm;
	dataSongInfo.compilation = tagLib.tags.compilation;
	dataSongInfo.publisher   = tagLib.tags.publisher;
	dataSongInfo.conductor   = tagLib.tags.conductor;
	dataSongInfo.lyricist    = tagLib.tags.lyricist;
	dataSongInfo.remixer     = tagLib.tags.remixer;
	dataSongInfo.grouping    = tagLib.tags.grouping;
	dataSongInfo.subtitle    = tagLib.tags.subtitle;
	dataSongInfo.copyright   = tagLib.tags.copyright;
	dataSongInfo.encodedby   = tagLib.tags.encodedby;
	dataSongInfo.comment     = tagLib.tags.comment;
	dataSongInfo.duration    = tagLib.tags.duration;
	dataSongInfo.channels    = tagLib.tags.channels;
	dataSongInfo.bitrate     = tagLib.tags.bitrate;
	dataSongInfo.samplerate  = tagLib.tags.sampleRate;
	dataSongInfo.cue         = 0;

	dataSongInfo.artists      = tagLib.tags.artists;
	dataSongInfo.albumArtists = tagLib.tags.albumArtists;
	dataSongInfo.genres       = tagLib.tags.genres;
	dataSongInfo.composers    = tagLib.tags.composers;
	dataSongInfo.conductors   = tagLib.tags.conductors;
	dataSongInfo.lyricists    = tagLib.tags.lyricists;

	dataSongInfo.file        = UTF::UTF8S(PathEx::FileFromPath(node->GetFile()));

	// Fill track hash
	std::wstring titleAlbumArtist;
	if (!dataSongInfo.title.empty())
		titleAlbumArtist += UTF::UTF16S(dataSongInfo.title);
	else
		titleAlbumArtist += PathEx::FileFromPath(node->GetFile());
	titleAlbumArtist += UTF::UTF16S(tagLib.tags.album);
	titleAlbumArtist += UTF::UTF16S(tagLib.tags.artist);
	dataSongInfo.trackHash = StringEx::HashFNV1a32(titleAlbumArtist);

	dataSongInfo.size = findFile.GetFileSize();

	dataSongInfo.modified = findFile.GetModified();

	dBase->UpdateTagsNode(skinList, node, &dataSongInfo);
}

bool Properties::LoadCover()
{
	if (!isMultiple)
	{
		if (!selectedNodes.empty())
			return coverLoader.LoadCoverImageTagEditor(selectedNodes[0]->GetFile());
	}

	return false;
}

void Properties::FreeCover()
{
	coverLoader.GetImage().Free();
}

bool Properties::LoadNewCover(const std::wstring& file)
{
	newCover.clear();

	// http://stackoverflow.com/questions/4761529/efficient-way-of-reading-a-file-into-an-stdvectorchar/4761779#4761779

	std::ifstream stream;
	stream.open(file.c_str(), std::ios::binary);

	if (stream.is_open())
	{
		stream.seekg(0, std::ios::end);
		std::size_t fileSize = (std::size_t)stream.tellg();
		stream.seekg(0, std::ios::beg);

		//newCover.reserve(fileSize);
		//newCover.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
		newCover.resize(fileSize);
		stream.read(&newCover[0], fileSize);
	}

	if (!newCover.empty())
	{
		needSaveCover = true;
		isUpdateCovers = true;
		return true;
	}

	return false;
}

void Properties::FreeNewCover()
{
	newCover.clear();
	newCover.shrink_to_fit();

	needSaveCover = false;
	isUpdateCovers = false;
}

bool Properties::SaveCoverToFile(const std::wstring& musicFile)
{
	std::wstring path = PathEx::PathFromFile(musicFile);

	if (coverFolders.insert(path).second == false)
		return true; // Already written

	std::wstring file = path + L"Front";
	if (TagLibCover::IsCoverJPG(newCover))
		file += L".jpg";
	else if (TagLibCover::IsCoverPNG(newCover))
		file += L".png";
	else
		return false;

	std::ofstream stream;
	stream.open(file.c_str(), std::ios::binary);

	if (stream.is_open())
	{
		stream.write(&newCover[0], newCover.size());

		return true;
	}

	return false;
}

bool Properties::LoadLyrics()
{
	if (!isMultiple)
	{
		if (!selectedNodes.empty())
		{
			if (lyricsLoader.LoadLyricsFromTags(selectedNodes[0]->GetFile()))
				return true;
			else
				return lyricsLoader.LoadLyricsFromFile(selectedNodes[0]->GetFile());
		}
	}

	return false;
}

void Properties::FreeLyrics()
{
	lyricsLoader.GetLines().clear();
	lyricsLoader.GetLines().shrink_to_fit();
}

void Properties::SetNewLyrics(const std::wstring& str)
{
	newLyrics = str;

	needSaveLyrics = true;
	isUpdateLyrics = true;
}

bool Properties::SaveLyricsToFile(const std::wstring& musicFile)
{
	std::wstring path = PathEx::PathFromFile(musicFile);
	path += L"Lyrics";
	std::wstring file = path;
	file.push_back('\\');
	file += PathEx::NameFromPath(musicFile);
	file += L".txt";

	if (newLyrics.empty())
		return FileSystem::RemoveFile(file);

	FileSystem::CreateDir(path);

	std::ofstream stream;
	stream.open(file.c_str(), std::ios::binary);

	if (stream.is_open())
	{
		std::string newLyricsData = UTF::UTF8S(newLyrics);

		stream.write(&newLyricsData[0], newLyricsData.size());

		return true;
	}

	return false;
}

std::wstring Properties::MergeMultiple(const std::wstring& text, const std::vector<std::wstring>& array)
{
	std::wstring out = text;
	for (std::size_t i = 0, size = array.size(); i < size; ++i)
	{
		out.push_back(';');
		out.push_back(' ');
		out += array[i];
	}

	return out;
}

std::wstring Properties::SplitMultiple(const std::wstring& text, std::vector<std::wstring>& outArray)
{
	std::wstring outText;

	std::size_t start = 0;
	std::size_t find = 0;
	while ((find = text.find(';', find)) != std::string::npos)
	{
		std::wstring str = text.substr(start, find - start);
		StringEx::Trim(str);

		if (!str.empty())
		{
			if (outText.empty()) //if (start == 0)
				outText = str;
			else
				outArray.push_back(str);
		}

		++find;
		start = find;
	}

	if (start == 0)
		outText = text;
	else
	{
		std::wstring str = text.substr(start);
		StringEx::Trim(str);
		if (!str.empty())
			outArray.push_back(str);
	}

	return outText;
}

void Properties::SetNewTrack(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.track = std::make_pair(str, true);
}
void Properties::SetNewDisc(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.disc = std::make_pair(str, true);
}
void Properties::SetNewTotalTracks(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.totalTracks = std::make_pair(str, true);
}
void Properties::SetNewTotalDiscs(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.totalDiscs = std::make_pair(str, true);
}
void Properties::SetNewTitle(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.title = std::make_pair(str, true);
}
void Properties::SetNewAlbum(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.album = std::make_pair(str, true);
	isUpdateLibrary = true;
}
void Properties::SetNewArtist(const std::wstring& str)
{
	needSaveTags = true;
	std::wstring text = SplitMultiple(str, tagLibW.tags.artists);
	tagLibW.tags.artist = std::make_pair(text, true);
	isUpdateLibrary = true;
}
void Properties::SetNewAlbumArtist(const std::wstring& str)
{
	needSaveTags = true;
	std::wstring text = SplitMultiple(str, tagLibW.tags.albumArtists);
	tagLibW.tags.albumArtist = std::make_pair(text, true);
	isUpdateLibrary = true;
}
void Properties::SetNewComposer(const std::wstring& str)
{
	needSaveTags = true;
	std::wstring text = SplitMultiple(str, tagLibW.tags.composers);
	tagLibW.tags.composer = std::make_pair(text, true);
	isUpdateLibrary = true;
}
void Properties::SetNewGenre(const std::wstring& str)
{
	needSaveTags = true;
	std::wstring text = SplitMultiple(str, tagLibW.tags.genres);
	tagLibW.tags.genre = std::make_pair(text, true);
	isUpdateLibrary = true;
}
void Properties::SetNewYear(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.year = std::make_pair(str, true);
	isUpdateLibrary = true;
}
void Properties::SetNewLyricist(const std::wstring& str)
{
	needSaveTags = true;
	std::wstring text = SplitMultiple(str, tagLibW.tags.lyricists);
	tagLibW.tags.lyricist = std::make_pair(text, true);
}
void Properties::SetNewComment(const std::wstring& str)
{
	needSaveTags = true;
	tagLibW.tags.comment = std::make_pair(str, true);
}
