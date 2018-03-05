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
#include "CueFile.h"
#include <fstream>
#include "UTF.h"


CueFile::CueFile()
{
}

CueFile::~CueFile()
{
}

bool CueFile::LoadFile(const std::wstring& file)
{
	std::ifstream stream;
	stream.open(file.c_str(), std::ios::binary);

	if (stream.is_open())
	{
		stream.seekg(0, std::ios::end);
		std::size_t fileSize = (std::size_t)stream.tellg();
		stream.seekg(0, std::ios::beg);

		std::vector<char> bytes;

		bytes.resize(fileSize + 4); // Latest 4 bytes = 0 (this simplifies the code below)
		stream.read(&bytes[0], fileSize);

		if (!bytes.empty() && bytes.size() <= maxFileSize)
			return ParseBytes(&bytes[0], bytes.size());
	}

	return false;
}

long long CueFile::GetCueValue(std::size_t index)
{
	int lowPart = cueHeader.tracks[index].frames;
	int highPart = 0;

	if (index < cueHeader.tracks.size() - 1) // For all exclude latest
		highPart = cueHeader.tracks[index + 1].frames;

	long long val = (long long)highPart << 32 | lowPart;

	return val;
}

int CueFile::GetCueTime(std::size_t index, int duration)
{
	if (duration == 0)
		return 0;

	int time = 0;

	if (index < cueHeader.tracks.size() - 1) // For all exclude latest
	{
		int frames = cueHeader.tracks[index + 1].frames - cueHeader.tracks[index].frames;
		time = (frames * 1000 + 75 / 2) / 75;
	}
	else // latest
	{
		int frames = cueHeader.tracks[index].frames;
		int offset = (frames * 1000 + 75 / 2) / 75;
		if (duration > offset)
			time = duration - offset;
	}

	return time;
}

long long CueFile::GetCueFileSize(std::size_t index, int duration, long long filesize)
{
	if (duration == 0 || filesize == 0)
		return 0;

	long long size = 0;

	if (index < cueHeader.tracks.size() - 1) // For all exclude latest
	{
		int frames = cueHeader.tracks[index + 1].frames - cueHeader.tracks[index].frames;
		int time = (frames * 1000 + 75 / 2) / 75;
		size = ((long long)time * filesize + duration / 2) / duration;
	}
	else // latest
	{
		int frames = cueHeader.tracks[index].frames;
		int time = (frames * 1000 + 75 / 2) / 75;
		long long offset = ((long long)time * filesize + duration / 2) / duration;
		if (filesize > offset)
			size = filesize - offset;
	}

	return size;
}

bool CueFile::ParseBytes(const char* p, size_t size)
{
	// Latest 2 bytes should be always 0
	if (!(size > 2 && p[size - 1] == '\0' && p[size - 2] == '\0'))
	{
		assert(false);
		return false;
	}

	if (size > 3 && p[0] == '\xEF' && p[1] == '\xBB' && p[2] == '\xBF') // UTF8 BOM
	{
		return ParseCue(p + 3);
	}
	else if (size > 2 && p[0] == '\xFF' && p[1] == '\xFE') // UTF-16LE BOM (Windows)
	{
		return ParseCue(UTF::UTF8((const wchar_t*)(p + 2)).c_str());
	}
	else if (size > 2 && p[0] == '\xFE' && p[1] == '\xFF') // UTF-16BE BOM
	{
		return false;
	}
	else // No BOM
	{
		if (UTF::IsUTF8(p))
			return ParseCue(p);
		else
			return ParseCue(UTF::UTF8S(UTF::UTF16A(p)).c_str());
	}

	return false;
}

bool CueFile::ParseCue(const char* p)
{
// Useful links:
// https://en.wikipedia.org/wiki/Cue_sheet_%28computing%29
// http://wiki.hydrogenaud.io/index.php?title=Cue_sheet
// http://digitalx.org/cue-sheet/syntax/index.html
// http://digitalx.org/cue-sheet/examples/index.html
/* CUE file example:
REM DISCID 010A0B0C
REM DISCNUMBER 1
REM TOTALDISCS 2
REM GENRE Test Genre
REM DATE 2000
CATALOG 0000123456789
PERFORMER "Test Artist"
TITLE "Test Album"
FILE "Test.mp3" WAVE
  TRACK 01 AUDIO
    TITLE "Test 1"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Test 2"
    INDEX 01 00:10:00
  TRACK 03 AUDIO
    TITLE "Test 3"
    INDEX 01 00:20:00
*/
	CueTrack cueTrack;

	bool header = true;

	for (; *p; ++p)
	{
		if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
			continue;

		// Header
		if (strncmp(p, "REM ", 4) == 0)
		{
			p += 4;

			// Error. Skip if REM after TRACK
			if (!header)
			{
				p += ParseNewLine(p); // Skip REM XXXX
				continue;
			}

			if (strncmp(p, "GENRE ", 6) == 0)
			{
				p += 6;
				p += ParseString(p, cueHeader.genre);
			}
			else if (strncmp(p, "DATE ", 5) == 0)
			{
				p += 5;
				p += ParseString(p, cueHeader.year);
			}
			else if (strncmp(p, "COMMENT ", 8) == 0)
			{
				p += 8;
				p += ParseString(p, cueHeader.comment);
			}
			else if (strncmp(p, "DISCNUMBER ", 11) == 0)
			{
				p += 11;
				p += ParseString(p, cueHeader.discNumber);
			}
			else if (strncmp(p, "TOTALDISCS ", 11) == 0)
			{
				p += 11;
				p += ParseString(p, cueHeader.totalDiscs);
			}
		}

		// File
		else if (strncmp(p, "FILE ", 5) == 0)
		{
			p += 5;
			p += ParseFile(p, cueTrack.file);

			if (!header)
				multipleFiles = true;
		}

		// Header/Track
		else if (strncmp(p, "TITLE ", 6) == 0)
		{
			p += 6;
			if (header)
				p += ParseString(p, cueHeader.album);
			else
				p += ParseString(p, cueTrack.title);
		}
		else if (strncmp(p, "PERFORMER ", 10) == 0)
		{
			p += 10;
			if (header)
				p += ParseString(p, cueHeader.albumArtist);
			else
				p += ParseString(p, cueTrack.artist);
		}
		else if (strncmp(p, "SONGWRITER ", 11) == 0)
		{
			p += 11;
			if (header)
				p += ParseString(p, cueHeader.albumLyricist);
			else
				p += ParseString(p, cueTrack.lyricist);
		}

		// Track
		else if (strncmp(p, "TRACK ", 6) == 0)
		{
			p += 6;
			p += ParseNewLine(p); // Skip XX AUDIO

			if (header)
				header = false;
		}
		else if (strncmp(p, "INDEX 01 ", 9) == 0)
		{
			p += 9;
			p += ParseTime(p, &cueTrack.frames);

			// Error. Stop if empty FILE
			if (cueTrack.file.empty())
				break;

			// Add track

			if (cueTrack.artist.empty())
				cueTrack.artist = cueHeader.albumArtist;
			if (cueTrack.lyricist.empty())
				cueTrack.lyricist = cueHeader.albumLyricist;

			// Do not add if frames lenght is incorrect
			if (multipleFiles || cueHeader.tracks.empty() || cueTrack.frames > cueHeader.tracks.back().frames)
				cueHeader.tracks.push_back(cueTrack);

			cueTrack.title.erase();
			cueTrack.artist.erase();
			cueTrack.lyricist.erase();
			cueTrack.frames = 0;
		}
	}

	if (!cueHeader.tracks.empty())
		return true;

	return false;
}

size_t CueFile::ParseString(const char* p, std::string& out)
{
	const char* pnew = p;
	const char* pend = p;

	// If starts with " or space then remove it
	while (*pnew == '\"' || *pnew == ' ')
	{
		++pnew;
		++pend;
	}

	// Find new line
	while (*pend && *pend != '\r' && *pend != '\n')
		++pend;

	size_t result = pend - p;

	// If ends with " or space then remove it
	while (pend > pnew && (*(pend - 1) == '\"' || *(pend - 1) == ' '))
		--pend;

	out.assign(pnew, pend);

	return result;
}

size_t CueFile::ParseFile(const char* p, std::string& out)
{
	const char* pnew = p;
	const char* pend = p;

	// If starts with space then remove it
	while (*pnew == ' ')
	{
		++pnew;
		++pend;
	}

	if (*pnew == '\"')
	{
		++pnew;
		++pend;

		while (*pend && *pend != '\"' && *pend != '\r' && *pend != '\n')
			++pend;
	}
	else
	{
		while (*pend && *pend != ' ' && *pend != '\r' && *pend != '\n')
			++pend;
	}

	out.assign(pnew, pend);

	// Find new line, skip file format
	while (*pend && *pend != '\r' && *pend != '\n')
		++pend;

	return pend - p;
}

size_t CueFile::ParseTime(const char* p, int* out)
{
	const char* pend = p;

	int i = atoi(pend) * 60 * 75; // 75 because according to CDDA format 1 sec = 75 frames

	while (*pend && *pend != ':' && *pend != '\r' && *pend != '\n')
		++pend;

	if (!*pend || *pend == '\r' || *pend == '\n') // Error
		return pend - p;

	++pend;
	i += atoi(pend) * 75;

	while (*pend && *pend != ':' && *pend != '\r' && *pend != '\n')
		++pend;

	if (!*pend || *pend == '\r' || *pend == '\n') // Error
		return pend - p;

	++pend;
	i += atoi(pend);

	while (*pend && *pend != '\r' && *pend != '\n') // Find new line
		++pend;

	*out = i;

	return pend - p;
}

size_t CueFile::ParseNewLine(const char* p)
{
	const char* pend = p;

	// Find new line
	while (*pend && *pend != '\r' && *pend != '\n')
		++pend;

	return pend - p;
}
