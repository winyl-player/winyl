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

#include "Threading.h"
#include "DBase.h"
#include "LibAudio.h"
#include "TagLibWriter.h"
#include <functional>
#include <set>
#include "CoverLoader.h"
#include "LyricsLoader.h"

// Properties

class Properties
{

public:
	Properties();
	virtual ~Properties();

	inline void SetDataBase(DBase* ptr) {dBase = ptr;}
	inline void SetLibAudio(LibAudio* ptr) {libAudio = ptr;}
	inline void SetSkinList(SkinList* ptr, ListNodeUnsafe node = nullptr) {skinList = ptr;}
	inline void SetSkinListNode(ListNodeUnsafe node) {listNode = node;}
	inline ListNodeUnsafe GetSkinListNode() {return listNode;}

	ListNodeUnsafe NextTrack();
	ListNodeUnsafe PrevTrack();

	void Init();

	inline bool IsRadioDefault() {return isRadioDefault;}
	inline bool IsRadioCustom() {return isRadioCustom;}
	inline bool IsPartOfCue() {return isPartOfCue;}

	inline bool IsMultiple() {return isMultiple;}

	inline bool IsEmpty() {return selectedNodes.empty();}

	inline bool IsUpdateLibrary() {return isUpdateLibrary;}
	inline bool IsUpdateCovers() {return isUpdateCovers;}
	inline bool IsUpdateLyrics() {return isUpdateLyrics;}

	inline int GetProgressCount() {return (int)selectedNodes.size();}

	void SetNewTrack(const std::wstring& str);
	void SetNewDisc(const std::wstring& str);
	void SetNewTotalTracks(const std::wstring& str);
	void SetNewTotalDiscs(const std::wstring& str);
	void SetNewTitle(const std::wstring& str);
	void SetNewAlbum(const std::wstring& str);
	void SetNewArtist(const std::wstring& str);
	void SetNewAlbumArtist(const std::wstring& str);
	void SetNewComposer(const std::wstring& str);
	void SetNewGenre(const std::wstring& str);
	void SetNewYear(const std::wstring& str);
	void SetNewLyricist(const std::wstring& str);
	void SetNewComment(const std::wstring& str);

	bool SaveTags();
	void WaitThread();

	inline void SetSaveCoverToTags(bool save) {saveCoverToTags = save;}
	inline void SetSaveCoverToFile(bool save) {saveCoverToFile = save;}

	inline void SetSaveLyricsToTags(bool save) {saveLyricsToTags = save;}
	inline void SetSaveLyricsToFile(bool save) {saveLyricsToFile = save;}

	bool LoadCover();
	void FreeCover();
	bool LoadNewCover(const std::wstring& file);
	void FreeNewCover();
	bool SaveCoverToFile(const std::wstring& musicFile);
	inline const std::vector<char>& GetNewCover() {return newCover;}

	bool LoadLyrics();
	void FreeLyrics();
	void SetNewLyrics(const std::wstring& str);
	bool SaveLyricsToFile(const std::wstring& musicFile);

	inline void SetThreadEndFunc(const std::function<void()>& func) {funcThreadEnd = func;}
	inline void SetProgressFunc(const std::function<void(int)>& func) {funcProgress = func;}

	struct FIELDS
	{
		std::wstring track;
		std::wstring totalTracks;
		std::wstring disc;
		std::wstring totalDiscs;
		std::wstring title;
		std::wstring album;
		std::wstring artist;
		std::wstring albumArtist;
		std::wstring composer;
		std::wstring genre;
		std::wstring year;
		std::wstring comment;
		std::wstring lyricist;

		std::wstring file;
		std::wstring quality;
	} fields;

	CoverLoader coverLoader;
	LyricsLoader lyricsLoader;

private:
	DBase* dBase = nullptr;
	LibAudio* libAudio = nullptr;
	SkinList* skinList = nullptr;
	ListNodeUnsafe listNode = nullptr;
	TagLibWriter tagLibW;
	std::vector<char> newCover;
	std::wstring newLyrics;

	bool saveCoverToTags = false;
	bool saveCoverToFile = false;
	bool saveLyricsToTags = false;
	bool saveLyricsToFile = false;

	bool needSaveTags = false;
	bool needSaveCover = false;
	bool needSaveLyrics = false;

	std::set<std::wstring> coverFolders;

	Threading::Thread threadWorker;
	void RunThread();

	std::vector<ListNodeSafe> selectedNodes;

	bool isMultiple = false;

	bool isRadioDefault = false;
	bool isRadioCustom = false;
	bool isPartOfCue = false;

	bool isUpdateLibrary = false;
	bool isUpdateCovers = false;
	bool isUpdateLyrics = false;

	std::function<void()> funcThreadEnd;
	std::function<void(int)> funcProgress;

	bool WriteTags(ListNodeUnsafe node);
	void UpdateTags(ListNodeUnsafe node);

	std::wstring MergeMultiple(const std::wstring& text, const std::vector<std::wstring>& array);
	std::wstring SplitMultiple(const std::wstring& text, std::vector<std::wstring>& outArray);
};
