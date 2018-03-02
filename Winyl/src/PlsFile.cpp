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

// PlsFile.cpp : implementation file
//

#include "stdafx.h"
#include "PlsFile.h"


// PlsFile

PlsFile::PlsFile()
{

}

PlsFile::~PlsFile()
{

}

bool PlsFile::LoadPlaylist(const std::wstring& file)
{
	currentPath = PathEx::PathFromFile(file);

	std::wstring ext = PathEx::ExtFromFile(file);

	if (ext.empty())
		return false;

	if (ext == L"m3u")
		return LoadM3U(file, false);
	else if (ext == L"m3u8")
		return LoadM3U(file, true);
	else if (ext == L"pls")
		return LoadPLS(file);
	else if (ext == L"asx")
		return LoadASX(file);
	else if (ext == L"xspf")
		return LoadXSPF(file);

	return false;
}

bool PlsFile::LoadM3U(const std::wstring& file, bool isUTF8)
{
	std::ifstream stream;
	stream.open(file.c_str());

	if (stream.is_open())
	{
		structTrack track;
		std::string line;

		for (int i = 0; std::getline(stream, line); ++i)
		{
			// UTF8 BOM
			if (i == 0 && line.size() >= 3 &&
				line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF')
			{
				isUTF8 = true;
				line.erase(0, 3);
			}

			if (line.empty())
				continue;
			else if (StringEx::IsEqualAscii(line, "#EXTM3U", 7))
				continue;
			else if (StringEx::IsEqualAscii(line, "#EXTINF:", 8))
			{
				std::size_t findDot = line.find(',', 8);
				if (findDot != std::string::npos)
				{
					if (isUTF8)
					{
						track.title = UTF::UTF16S(line.substr(findDot + 1));
						track.time = UTF::UTF16S(line.substr(8, findDot - 8));
					}
					else
					{
						track.title = UTF::UTF16AS(line.substr(findDot + 1));
						track.time = UTF::UTF16AS(line.substr(8, findDot - 8));
					}
				}
				else if (isUTF8)
					track.title = UTF::UTF16S(line.substr(8));
				else
					track.title = UTF::UTF16AS(line.substr(8));
			}
			else
			{
				if (isUTF8)
					track.file = UTF::UTF16S(line);
				else
					track.file = UTF::UTF16AS(line);

				FixTrackFile(track, true);

				AddTrack(track);
				EmptyTrack(track);
			}
		}

		return true;
	}

	return false;
}

bool PlsFile::LoadPLS(const std::wstring& file)
{
	std::ifstream stream;
	stream.open(file.c_str());

	if (stream.is_open())
	{
		structTrack track;
		std::string line;

		bool isUTF8 = false;

		for (int i = 0; std::getline(stream, line); ++i)
		{
			// UTF8 BOM
			if (i == 0 && line.size() >= 3 &&
				line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF')
			{
				isUTF8 = true;
				line.erase(0, 3);
			}

			if (line.empty())
				continue;
			else if (StringEx::IsEqualAscii(line, "[playlist]", 10))
				continue;
			else if (StringEx::IsEqualAscii(line, "File", 4))
			{
				AddTrack(track);
				EmptyTrack(track);

				std::size_t findEqual = line.find('=', 4);
				if (findEqual != std::string::npos)
				{
					if (isUTF8)
						track.file = UTF::UTF16S(line.substr(findEqual + 1));
					else
						track.file = UTF::UTF16AS(line.substr(findEqual + 1));

					FixTrackFile(track, true);
				}
			}
			else if (StringEx::IsEqualAscii(line, "Title", 5))
			{
				std::size_t findEqual = line.find('=', 5);

				if (findEqual != std::string::npos)
				{
					if (isUTF8)
						track.title = UTF::UTF16S(line.substr(findEqual + 1));
					else
						track.title = UTF::UTF16AS(line.substr(findEqual + 1));
				}
			}
			else if (StringEx::IsEqualAscii(line, "Length", 6))
			{
				std::size_t findEqual = line.find('=', 6);
				
				if (findEqual != std::string::npos)
				{
					if (isUTF8)
						track.time = UTF::UTF16S(line.substr(findEqual + 1));
					else
						track.time = UTF::UTF16AS(line.substr(findEqual + 1));
				}
			}
		}

		AddTrack(track);

		return true;
	}

	return false;
}

void PlsFile::AddTrack(structTrack& track)
{
	if (!track.file.empty())
		tracks.push_back(track);
}

void PlsFile::EmptyTrack(structTrack& track)
{
	track.file.clear();
	track.title.clear();
	track.time.clear();
}

void PlsFile::FixTrackFile(structTrack& track, bool checkNoDrive, bool checkFileUrl)
{
	StringEx::Trim(track.file);

	if (checkFileUrl)
	{
		const std::wstring fileUrl = L"file:///";
		if (StringEx::IsEqualAscii(track.file, fileUrl, fileUrl.size()))
			track.file.erase(0, fileUrl.size());
	}

	if (PathEx::IsURL(track.file))
		return;

	StringEx::Replace(track.file, '/', '\\');

	// First, check that the file doesn't start with backslash, Winamp saves in this format
	if (checkNoDrive && currentPath.size() > 2 && track.file.size() > 2 &&
		track.file[0] == '\\' && track.file[1] != '\\')
		track.file = currentPath.substr(0, 2) + track.file;
	else if (track.file.size() > 2 && track.file[1] != ':') // Relative path
		track.file = currentPath + track.file;
}

bool PlsFile::LoadASX(const std::wstring& file)
{
	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChildNoCase("asx");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChildNoCase("entry"); xmlNode; xmlNode = xmlNode.NextChildNoCase("entry"))
			{
				structTrack track;

				XmlNode xmlTitle = xmlNode.FirstChildNoCase("title");
				if (xmlTitle)
				{
					const char* title = xmlTitle.Value();
					track.title = UTF::UTF16(title);
				}

				XmlNode xmlRef = xmlNode.FirstChildNoCase("ref");
				if (xmlRef)
				{
					const char* href = xmlRef.AttributeNoCase("href");
					track.file = UTF::UTF16(href);
				}

				FixTrackFile(track, false, true);

				AddTrack(track);
			}
		}

		return true;
	}

	return false;
}

bool PlsFile::LoadXSPF(const std::wstring& file)
{
	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChildNoCase("playlist");

		if (xmlMain)
			xmlMain = xmlMain.FirstChildNoCase("tracklist");
		else
			return false;

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChildNoCase("track"); xmlNode; xmlNode = xmlNode.NextChildNoCase("track"))
			{
				structTrack track;

				XmlNode xmlTitle = xmlNode.FirstChildNoCase("title");
				if (xmlTitle)
				{
					const char* title = xmlTitle.Value();
					track.title = UTF::UTF16(title);
				}

				XmlNode xmlLocation = xmlNode.FirstChildNoCase("location");
				if (xmlLocation)
				{
					const char* location = xmlLocation.Value();
					track.file = UTF::UTF16(location);
				}

				FixTrackFile(track, false, true);

				AddTrack(track);
			}
		}

		return true;
	}
	
	return false;
}

