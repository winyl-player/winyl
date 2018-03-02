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

// TagLibWriter

#include <string>
#include <vector>
#include <memory>

// forward declaration
namespace TagLib
{
	class File;
	class FileStream;
	class FileRef;
	class AudioProperties;
	namespace ID3v2 { class Tag; }
	namespace ID3v1 { class Tag; }
	namespace APE { class Tag; }
	namespace Ogg { class XiphComment; }
	namespace ASF { class Tag; }
	namespace MP4 { class Tag; }
	namespace FLAC { class File; }
}

class TagLibReader
{
friend class TagLibLyrics;

public:
	class File
	{
		public:
			File(const std::wstring& file, bool openReadOnly, bool readAudioProperties);
			virtual ~File();
			File(const File&) = delete;
			File& operator=(const File&) = delete;
			TagLib::File* file() const { return filePtr.get(); }
			bool IsOpen() const { return isFileOpen; }
		private:
			std::unique_ptr<TagLib::File> filePtr;
			std::unique_ptr<TagLib::FileStream> fileStreamPtr;
			bool isFileOpen = false;
	};

public:
	TagLibReader();
	virtual ~TagLibReader();
	TagLibReader(const TagLibReader&) = delete;
	TagLibReader& operator=(const TagLibReader&) = delete;

	bool ReadFileTags(const std::wstring& file);
	bool ReadFileTagsFromTagLibFile(const TagLibReader::File& f);

	struct TAGS
	{
		std::string title;
		std::string album;
		std::string artist;
		std::string albumArtist;
		std::string genre;
		std::string composer;
		std::string publisher;
		std::string conductor;
		std::string lyricist;
		std::string grouping;
		std::string subtitle;
		std::string copyright;
		std::string encodedby;
		std::string remixer;
		std::string comment;
		std::string compilation;
		//std::string mood;

		std::vector<std::string> artists;
		std::vector<std::string> albumArtists;
		std::vector<std::string> genres;
		std::vector<std::string> composers;
		std::vector<std::string> conductors;
		std::vector<std::string> lyricists;

		std::string bpm;
		std::string year;
		std::string track;
		std::string disc;
		std::string totalTracks;
		std::string totalDiscs;

		int bitrate    = 0;
		int channels   = 0;
		int duration   = 0;
		int sampleRate = 0;
	} tags;

private:
	void ReadAudioProperties(TagLib::AudioProperties* properties);
	void ReadID3v2Tags(TagLib::ID3v2::Tag* tag);
	bool ReadID3v2TagFrameText(TagLib::ID3v2::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	bool ReadID3v2TagFrameComment(TagLib::ID3v2::Tag* tag, char* id, std::string& outText);
	bool ReadID3v2TagFrameTextGenre(TagLib::ID3v2::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	static bool ReadID3v2TagFrameLyrics(TagLib::ID3v2::Tag* tag, char* id, std::string& outText);
	//bool ReadID3v2TagFrameTxxx(TagLib::ID3v2::Tag* tag, char* id, std::string& outText);
	void ReadID3v1Tags(TagLib::ID3v1::Tag* tag);
	void ReadAPETags(TagLib::APE::Tag* tag);
	static bool ReadAPETagItem(TagLib::APE::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	void ReadOGGTags(TagLib::Ogg::XiphComment* tag);
	static bool ReadOGGTagComment(TagLib::Ogg::XiphComment* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	void ReadASFTags(TagLib::ASF::Tag* tag);
	static bool ReadASFTagAttributeText(TagLib::ASF::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	bool ReadASFTagAttributeTextArray(TagLib::ASF::Tag* tag, char* id, std::vector<std::string>& outArray);
	bool ReadASFTagAttributeTextOrInt(TagLib::ASF::Tag* tag, char* id, std::string& outText);
	bool ReadASFTagAttributeBool(TagLib::ASF::Tag* tag, char* id, std::string& outText);
	void ReadMP4Tags(TagLib::MP4::Tag* tag);
	static bool ReadMP4TagItemText(TagLib::MP4::Tag* tag, char* id, std::string& outText, std::vector<std::string>* outArray = nullptr);
	bool ReadMP4TagItemIntPair(TagLib::MP4::Tag* tag, char* id, std::string& outText1, std::string& outText2);
	bool ReadMP4TagItemInt(TagLib::MP4::Tag* tag, char* id, std::string& outText);
	bool ReadMP4TagItemBool(TagLib::MP4::Tag* tag, char* id, std::string& outText);

	void SplitNumberTotal(std::string& inoutNumber, std::string& outTotal);
	void SplitMultiplies(const std::string& text, std::string& outText, std::vector<std::string>& outArray);
};

