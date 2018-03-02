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
#include <vector>

class CueFile
{
public:
	CueFile();
	~CueFile();

	bool LoadFile(const std::wstring& file);

	struct CueTrack
	{
		std::string file;
		std::string title;
		std::string artist;
		std::string lyricist;
		int frames = 0;
	};

	struct CueHeader
	{
		std::string album;
		std::string albumArtist;
		std::string albumLyricist;
		std::string genre;
		std::string year;
		std::string comment;
		std::string discNumber;
		std::string totalDiscs;

		std::vector<CueTrack> tracks;
	};

	const CueHeader& GetCue() {return cueHeader;}
	bool IsMultipleFiles() {return multipleFiles;}

private:
	bool ParseBytes(const char* p, size_t size);
	bool ParseCue(const char* p);
	int ParseString(const char* p, std::string& out);
	int ParseFile(const char* p, std::string& out);
	int ParseTime(const char* p, int* out);
	int ParseNewLine(const char* p);

	CueHeader cueHeader;
	bool multipleFiles = false; // Multiple files in CUE

	const std::size_t maxFileSize = 1024 * 1024; // 1MB

public:
	// Helpers

	long long GetCueValue(std::size_t index);
	int GetCueTime(std::size_t index, int duration);
	long long GetCueFileSize(std::size_t index, int duration, long long filesize);

	// If end offset != 0
	static bool IsLenght(long long cue)
	{
		return (int)((cue >> 32) & 0xffffffffff) ? true : false;
	}

	// If end offset != 0 and end offset of first cue == start offset of second
	static bool IsNextCue(long long cue, long long cueNext)
	{
		return (int)((cue >> 32) & 0xffffffffff) && (int)((cue >> 32) & 0xffffffffff) == (int)(cueNext & 0xffffffffff);
	}

	// Get offset and convert it to seconds in double
	static double GetOffset(long long cue)
	{
		return (int)(cue & 0xffffffffff) / 75.0;
	}

	// Get lenght (end offset - start offset) and convert it to seconds in double
	static double GetLenght(long long cue)
	{
		return ((int)((cue >> 32) & 0xffffffffff) - (int)(cue & 0xffffffffff)) / 75.0;
	}
};

