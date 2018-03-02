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

#include "ExImage.h"
#include "bass/bass.h"
#include "UTF.h"

// CoverLoader

class CoverLoader
{

public:
	CoverLoader();
	virtual ~CoverLoader();

	bool LoadCoverImage(const std::wstring& file);
	bool LoadCoverImageTagEditor(const std::wstring& file);
	ExImage::Source& GetImage() {return coverImage;}

private:
	ExImage::Source coverImage;

	// Search for a cover in the track folder by a name or extension
	bool FindImageByName(const std::wstring& path, const std::wstring& match, std::wstring& file);
	bool FindImageByExt(const std::wstring& path, std::wstring& file);

	// Load cover from the track
	bool LoadCoverFromTrack(const std::wstring& file);
};


