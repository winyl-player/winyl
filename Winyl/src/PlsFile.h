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

#include "XmlFile.h"
#include "UTF.h"

class PlsFile
{

public:
	PlsFile();
	virtual ~PlsFile();

	int GetPlsSize() {return (int)tracks.size();}
	const std::wstring& GetPlsFile(int index) {return tracks[index].file;}
	const std::wstring& GetPlsTitle(int index) {return tracks[index].title;}
	const std::wstring& GetPlsTime(int index) {return tracks[index].time;}

	bool LoadPlaylist(const std::wstring& file);

	bool IsRadioURL(int index) {return PathEx::IsURL(tracks[index].file);}

private:
	bool LoadM3U(const std::wstring& file, bool isUTF8); // M3U Playlist
	bool LoadPLS(const std::wstring& file); // PLS Playlist
	bool LoadASX(const std::wstring& file); // Advanced Stream Redirector
	bool LoadXSPF(const std::wstring& file); // XML Shareable Playlist Format

	std::wstring currentPath;

	struct structTrack
	{
		std::wstring file;
		std::wstring title;
		std::wstring time;
	};

	void AddTrack(structTrack& track);
	void EmptyTrack(structTrack& track);
	void FixTrackFile(structTrack& track, bool checkNoDrive = false, bool checkFileUrl = false);

	std::vector<structTrack> tracks;
};


