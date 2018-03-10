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

#include <string>
#include "TagLibReader.h"
#include "TagLibLyrics.h"
#include "TagLibCover.h"

// forward declaration
namespace TagLib
{
	class FileRef;
	class AudioProperties;
	namespace ID3v2 { class Tag; }
	namespace ID3v1 { class Tag; }
	namespace APE { class Tag; }
	namespace Ogg { class XiphComment; }
	namespace ASF { class Tag; }
	namespace MP4 { class Tag; }
}

class TagLibWriter
{
friend class TagLibLyrics;

public:
	TagLibWriter();
	virtual ~TagLibWriter();
	TagLibWriter(const TagLibWriter&) = delete;
	TagLibWriter& operator=(const TagLibWriter&) = delete;

	bool SaveFileTags(const std::wstring& file);
	bool SaveFileTagsToTagLibFile(const TagLibReader::File& f);

	struct TAGS
	{
		std::pair<std::wstring, bool> title       = {{}, false};
		std::pair<std::wstring, bool> album       = {{}, false};
		std::pair<std::wstring, bool> artist      = {{}, false};
		std::pair<std::wstring, bool> albumArtist = {{}, false};
		std::pair<std::wstring, bool> genre       = {{}, false};
		std::pair<std::wstring, bool> composer    = {{}, false};
		std::pair<std::wstring, bool> publisher   = {{}, false};
		std::pair<std::wstring, bool> conductor   = {{}, false};
		std::pair<std::wstring, bool> lyricist    = {{}, false};
		std::pair<std::wstring, bool> grouping    = {{}, false};
		std::pair<std::wstring, bool> subtitle    = {{}, false};
		std::pair<std::wstring, bool> copyright   = {{}, false};
		std::pair<std::wstring, bool> encodedby   = {{}, false};
		std::pair<std::wstring, bool> remixer     = {{}, false};
		std::pair<std::wstring, bool> comment     = {{}, false};
		//std::pair<std::wstring, bool> mood        = {{}, false};

		std::vector<std::wstring> artists;
		std::vector<std::wstring> albumArtists;
		std::vector<std::wstring> genres;
		std::vector<std::wstring> composers;
		std::vector<std::wstring> conductors;
		std::vector<std::wstring> lyricists;

		std::pair<std::wstring, bool> bpm         = {{}, false};
		std::pair<std::wstring, bool> year        = {{}, false};
		std::pair<std::wstring, bool> track       = {{}, false};
		std::pair<std::wstring, bool> disc        = {{}, false};
		std::pair<std::wstring, bool> totalTracks = {{}, false};
		std::pair<std::wstring, bool> totalDiscs  = {{}, false};
	} tags;

	TagLibCover cover;
	TagLibLyrics lyrics;

private:
	void SaveID3v2Tags(TagLib::ID3v2::Tag* tag, int version);
	void SaveID3v2TagFrameText(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text, std::vector<std::wstring>* array = nullptr);
	void SaveID3v2TagFrameComment(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text);
	static void SaveID3v2TagFrameLyrics(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text);
	//void SaveID3v2TagFrameTxxx(TagLib::ID3v2::Tag* tag, int version, char* id, const std::wstring& text);
	void FixID3v2Encoding(TagLib::ID3v2::Tag* tag, int version);
	void SaveID3v1Tags(TagLib::ID3v1::Tag* tag);
	void SaveAPETags(TagLib::APE::Tag* tag);
	static void SaveAPETagItem(TagLib::APE::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array = nullptr);
	void SaveOGGTags(TagLib::Ogg::XiphComment* tag);
	static void SaveOGGTagComment(TagLib::Ogg::XiphComment* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array = nullptr);
	void SaveASFTags(TagLib::ASF::Tag* tag);
	static void SaveASFTagAttribute(TagLib::ASF::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array = nullptr);
	void SaveASFTagAttributeArray(TagLib::ASF::Tag* tag, char* id, std::vector<std::wstring>& array);
	void SaveMP4Tags(TagLib::MP4::Tag* tag);
	static void SaveMP4TagItem(TagLib::MP4::Tag* tag, char* id, const std::wstring& text, std::vector<std::wstring>* array = nullptr);
	void SaveMP4TagItemInt(TagLib::MP4::Tag* tag, char* id, const std::wstring& text);
	void SaveMP4TagItemIntPair(TagLib::MP4::Tag* tag, char* id, const std::wstring& text1, const std::wstring& text2);

	std::wstring MergeNumberTotal(const std::wstring& inNumber, const std::wstring& inTotal);
	std::wstring MixNumberTotal(const std::string& oldNumber, const std::string& oldTotal,
		const std::pair<std::wstring, bool>& number, std::pair<std::wstring, bool>& total);
	std::pair<std::wstring, std::wstring> MixNumberTotal2(const std::string& oldNumber, const std::string& oldTotal,
		const std::pair<std::wstring, bool>& number, std::pair<std::wstring, bool>& total);

	TagLibReader loader;
};
