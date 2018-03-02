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

// TagLibCover

#include <string>
#include "ExImage.h"
#include "TagLibReader.h"

class TagLibCover
{

public:
	TagLibCover();
	virtual ~TagLibCover();
	TagLibCover(const TagLibCover&) = delete;
	TagLibCover& operator=(const TagLibCover&) = delete;

	bool ReadCoverFromFile(const std::wstring& file);
	void SaveCoverToTagLibFile(const TagLibReader::File& f);

	ExImage::Source* outImage = nullptr;
	void CreateCoverImage(const char* data, int size);

	std::vector<char>* newCover = nullptr;

	static bool IsCoverJPG(const std::vector<char>& data);
	static bool IsCoverPNG(const std::vector<char>& data);

private:
	void ReadCoverFromID3v2Tags(TagLib::ID3v2::Tag* tag);
	void ReadCoverFromAPETags(TagLib::APE::Tag* tag);
	void ReadCoverFromFLACFile(TagLib::FLAC::File* file);
	void ReadCoverFromOGGTags(TagLib::Ogg::XiphComment* tag);
	void ReadCoverFromASFTags(TagLib::ASF::Tag* tag);
	void ReadCoverFromMP4Tags(TagLib::MP4::Tag* tag);

	void SaveCoverToID3v2Tags(TagLib::ID3v2::Tag* tag);
	void SaveCoverToAPETags(TagLib::APE::Tag* tag);
	void SaveCoverToFLAC(TagLib::FLAC::File* file);
	void SaveCoverToOGGTags(TagLib::Ogg::XiphComment* tag);
	void SaveCoverToASFTags(TagLib::ASF::Tag* tag);
	void SaveCoverToMP4Tags(TagLib::MP4::Tag* tag);
};


