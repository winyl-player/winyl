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

#include "TagLibReader.h"


class TagLibLyrics
{
public:
	TagLibLyrics();
	virtual ~TagLibLyrics();
	TagLibLyrics(const TagLibLyrics&) = delete;
	TagLibLyrics& operator=(const TagLibLyrics&) = delete;

	bool ReadLyricsFromFile(const std::wstring& file);
	bool ReadFileTagsFromTagLibFile(const TagLibReader::File& f);
	void SaveLyricsToTagLibFile(const TagLibReader::File& f, int id3v2version = 0);

	const std::string& GetLyrics() {return lyrics;}
	void SetLyrics(const std::wstring& lyrc) {newLyrics = std::make_pair(lyrc, true);}

private:
	std::string lyrics;
	std::pair<std::wstring, bool> newLyrics = {{}, false};

	void ReadLyricsFromID3v2Tags(TagLib::ID3v2::Tag* tag);
	void ReadLyricsFromAPETags(TagLib::APE::Tag* tag);
	void ReadLyricsFromOGGTags(TagLib::Ogg::XiphComment* tag);
	void ReadLyricsFromASFTags(TagLib::ASF::Tag* tag);
	void ReadLyricsFromMP4Tags(TagLib::MP4::Tag* tag);

	void SaveLyricsToID3v2Tags(TagLib::ID3v2::Tag* tag, int version);
	void SaveLyricsToAPETags(TagLib::APE::Tag* tag);
	void SaveLyricsToOGGTags(TagLib::Ogg::XiphComment* tag);
	void SaveLyricsToASFTags(TagLib::ASF::Tag* tag);
	void SaveLyricsToMP4Tags(TagLib::MP4::Tag* tag);
};

