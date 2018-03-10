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
#include <vector>

class LyricsLoader
{

public:
	LyricsLoader();
	~LyricsLoader();
	
	bool LoadLyricsFromFile(const std::wstring& file);
	bool LoadLyricsFromTags(const std::wstring& file);
	bool LoadLyricsFromInternet(const std::wstring& artist, const std::wstring& title, const std::wstring& provider);
	std::vector<std::wstring>& GetLines() {return lines;}
	std::wstring GetLyrics();

	std::vector<std::wstring>&& MoveLines() {return std::move(lines);}

	static const wchar_t* GetLyricsProvider(int index) {return providers[index];}
	static int GetLyricsProviderCount() {return providersCount;}
	static int GetLyricsProviderByURL(const std::wstring& url);

private:
	std::vector<std::wstring> lines;

	bool LoadLyricsFromFileImpl(const std::wstring& file);

	void LyricsToLines(const std::string& lyrics);

	const int timeoutLyrics = 3000;
	const int timeoutShow = 500;

	static wchar_t* providers[];
	static int providersCount;

	static long long timePrev;

	bool FilterInputIsValid(const std::string& str);
	bool FilterInputIsAscii(const std::string& str);
	std::string FilterInputStripSpaces(const std::string& str, const char replace = '\0');
	std::string FilterInputUriEncode(const std::string &str, bool escapeUnsafe);
	void FilterInputRemoveBraces(std::string& str);
	void FilterInputLowerAscii(std::string& str);

	std::string FilterOutputFromTo(const std::string &str, const std::string& from, const std::string& to);
	std::string FilterOutputFromTo(const std::string &str, const std::string& skip, const std::string& from, const std::string& to);
	std::string FilterOutputHtmlTags(const std::string &str, bool encode = false, bool prn = false, bool p2rn = false, bool plain = false);
	void FilterOutputHtmlEncode(const std::string &src, std::size_t start, std::size_t end, std::string &dst);
	void FilterOutputTrim(std::string &str);

	std::string ProviderLyricsWikiaCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderAZLyricsCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderMetroLyricsCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderSongLyricsCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderOldieLyricsCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderChartLyricsCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderLyricsManiaCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderGeniusCom(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderLetrasMusBr(const std::string& urlArtist, const std::string& urlTitle);
	std::string ProviderMusixmatchCom(const std::string& urlArtist, const std::string& urlTitle);
};

