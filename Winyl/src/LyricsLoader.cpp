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
#include "LyricsLoader.h"
#include "UTF.h"
#include "FileSystem.h"
#include <fstream>
#include "TagLibLyrics.h"
#include "HttpClient.h"
#include "Threading.h"
#include "XmlFile.h"

LyricsLoader::LyricsLoader()
{

}

LyricsLoader::~LyricsLoader()
{
	
}

long long LyricsLoader::timePrev = 0;

wchar_t* LyricsLoader::providers[] =
{
	L"lyrics.wikia.com",
	L"musixmatch.com",
	L"azlyrics.com",
	L"letras.mus.br",
	L"lyricsmania.com",
	//L"metrolyrics.com",
	L"songlyrics.com",
	//L"genius.com",
	L"oldielyrics.com",
};
int LyricsLoader::providersCount = sizeof(providers) / sizeof(providers[0]);

int LyricsLoader::GetLyricsProviderByURL(const std::wstring& url)
{
	for (int i = 0; i < providersCount; i++)
	{
		if (url == providers[i])
			return i;
	}

	return -1;
}

bool LyricsLoader::LoadLyricsFromFile(const std::wstring& file)
{
	std::wstring path = PathEx::PathFromFile(file);
	std::wstring fileLyrics = PathEx::NameFromPath(file) + L".txt";

	if (LoadLyricsFromFileImpl(path + fileLyrics) ||
		LoadLyricsFromFileImpl(path + L"lyrics\\" + fileLyrics))
	{
		return true;
	}

	return false;
}

bool LyricsLoader::LoadLyricsFromFileImpl(const std::wstring& file)
{
	std::ifstream stream;
	stream.open(file.c_str());

	if (stream.is_open())
	{
		std::string line;

		while (std::getline(stream, line))
		{
			lines.push_back(UTF::UTF16S(line));
		}

		return true;
	}

	return false;
}

bool LyricsLoader::LoadLyricsFromTags(const std::wstring& file)
{
	TagLibLyrics tagLib;
	if (tagLib.ReadLyricsFromFile(file))
	{
		const std::string& lyrics = tagLib.GetLyrics();

		if (lyrics.empty())
			return false;

		LyricsToLines(lyrics);

		return true;
	}

	return false;
}

std::wstring LyricsLoader::GetLyrics()
{
	std::wstring lyrics;

	for (std::size_t i = 0, size = lines.size(); i < size; ++i)
	{
		if (i > 0)
		{
			lyrics.push_back('\r');
			lyrics.push_back('\n');
		}
		lyrics += lines[i];
	}

	return lyrics;
}

void LyricsLoader::LyricsToLines(const std::string& lyrics)
{
	if (lyrics.empty())
		return;

	for (std::size_t find = 0, start = 0; ; ++find)
	{
		if (lyrics[find] == '\r')
		{
			lines.push_back(UTF::UTF16S(lyrics.substr(start, find - start)));
			if (lyrics[find + 1] == '\n')
				++find;
			start = find + 1;
		}
		else if (lyrics[find] == '\n')
		{
			lines.push_back(UTF::UTF16S(lyrics.substr(start, find - start)));
			start = find + 1;
		}
		else if (lyrics[find] == '\0')
		{
			lines.push_back(UTF::UTF16S(lyrics.substr(start, find - start)));
			break;
		}
	}
}

bool LyricsLoader::LoadLyricsFromInternet(const std::wstring& artist, const std::wstring& title, const std::wstring& provider)
{
	if (artist.empty() || title.empty())
		return false;

	std::string urlArtist = UTF::UTF8S(artist);
	std::string urlTitle = UTF::UTF8S(title);

	if (!FilterInputIsValid(urlArtist) || !FilterInputIsValid(urlTitle))
		return false;

	StringEx::Trim(urlArtist);
	StringEx::Trim(urlTitle);

	long long timeNow = FileSystem::GetTimeNowMs();

	if (timeNow - timePrev <= timeoutLyrics)
		Threading::ThreadSleep((unsigned)std::max(0LL, timeoutLyrics - (timeNow - timePrev)));
	
	if (FilterInputIsAscii(urlArtist) && FilterInputIsAscii(urlTitle))
	{
		FilterInputRemoveBraces(urlTitle);

		std::string lyrics;

		assert(providersCount == 7);

		if (provider.empty())
			lyrics = ProviderLyricsWikiaCom(urlArtist, urlTitle);
		else if (provider == providers[1])
			lyrics = ProviderMusixmatchCom(urlArtist, urlTitle);
		else if (provider == providers[2])
			lyrics = ProviderAZLyricsCom(urlArtist, urlTitle);
		else if (provider == providers[3])
			lyrics = ProviderLetrasMusBr(urlArtist, urlTitle);
		else if (provider == providers[4])
			lyrics = ProviderLyricsManiaCom(urlArtist, urlTitle);
		else if (provider == providers[5])
			lyrics = ProviderSongLyricsCom(urlArtist, urlTitle);
		else if (provider == providers[6])
			lyrics = ProviderOldieLyricsCom(urlArtist, urlTitle);

		LyricsToLines(lyrics);

		int i = 0;
	}
	else
	{
		FilterInputRemoveBraces(urlTitle);

		std::string lyrics;

		if (provider.empty())
			lyrics = ProviderLyricsWikiaCom(urlArtist, urlTitle);
		else if (provider == providers[1])
			lyrics = ProviderMusixmatchCom(urlArtist, urlTitle);

		LyricsToLines(lyrics);

		int i = 0;
	}

	timePrev = FileSystem::GetTimeNowMs();

	if (timePrev - timeNow <= timeoutShow)
		Threading::ThreadSleep((unsigned)std::max(0LL, timeoutShow - (timePrev - timeNow)));

	if (lines.empty())
		return false;

	return true;
}

bool LyricsLoader::FilterInputIsValid(const std::string& str)
{
	//const char chars[] = "()[]{}~@#$%^*+=;:_\"\\";
	const char chars[] = "[]{}~@#$%^*=;:_\"\\";

	if (str.find_first_of(chars) == std::string::npos)
		return true;

	return false;
}

bool LyricsLoader::FilterInputIsAscii(const std::string& str)
{
	const char chars[] = ".,?!&'-/+";

	for (std::size_t i = 0, size = str.size(); i < size; ++i)
	{
		if (!(str[i] == ' ' ||
			(str[i] >= '0' && str[i] <= '9') ||
			(str[i] >= 'a' && str[i] <= 'z') ||
			(str[i] >= 'A' && str[i] <= 'Z') ||
			strchr(chars, str[i]) != nullptr))
			return false;
	}

	return true;
}

std::string LyricsLoader::FilterInputStripSpaces(const std::string& str, const char replace)
{
	const char chars[] = ".,?!&'-/+";

	std::string result;

	bool prevSpace = false;
	for (std::size_t i = 0, size = str.size(); i < size; ++i)
	{
		if (str[i] == ' ' || strchr(chars, str[i]) != nullptr)
		{
			if (replace != '\0' && !prevSpace)
				result.append(1, replace);
			prevSpace = true;
		}
		else
		{
			result.append(1, str[i]);
			prevSpace = false;
		}
	}

	return result;
}

std::string LyricsLoader::FilterInputUriEncode(const std::string &str, bool escapeUnsafe)
{
	// http://en.wikipedia.org/wiki/Percent-encoding

	const char dexchar[] = "0123456789ABCDEF";
	const char reserved[] = "!*'();:@&=+$,/?#[]";

	std::string result;

	for (std::size_t i = 0, size = str.size(); i < size; ++i)
	{
		if ((str[i] >= '0' && str[i] <= '9') || // unreserved
			(str[i] >= 'a' && str[i] <= 'z') || // unreserved
			(str[i] >= 'A' && str[i] <= 'Z') || // unreserved
			str[i] == '-' || str[i] == '_' || // unreserved
			str[i] == '.' || str[i] == '~' || // unreserved
			(!escapeUnsafe && strchr(reserved, str[i]) != nullptr)) // reserved
		{
			result.append(1, str[i]);
		}
		else
		{
			result.append(1, '%');
			result.append(1, dexchar[(str[i] & 0xF0) >> 4]);
			result.append(1, dexchar[str[i] & 0x0F]);
		}
	}

	return result;
}

void LyricsLoader::FilterInputRemoveBraces(std::string& str)
{
	std::size_t find = str.find('(');
	if (find != std::string::npos)
	{
		str = str.substr(0, find);
		StringEx::TrimRight(str);
	}
}

void LyricsLoader::FilterInputLowerAscii(std::string& str)
{
	StringEx::MakeLowerAscii(str);
}

std::string LyricsLoader::FilterOutputFromTo(const std::string &str, const std::string& from, const std::string& to)
{
	std::string result;

	std::size_t findFrom = str.find(from);
	if (findFrom != std::string::npos)
	{
		std::size_t findTo = str.find(to, findFrom + from.size());
		if (findTo != std::string::npos)
		{
			result = str.substr(findFrom + from.size(), findTo - findFrom - from.size());
		}
	}

	return result;
}

std::string LyricsLoader::FilterOutputFromTo(const std::string &str, const std::string& skip, const std::string& from, const std::string& to)
{
	std::string result;

	std::size_t findSkip = str.find(skip);
	if (findSkip != std::string::npos)
	{
		std::size_t findFrom = str.find(from, findSkip + skip.size());
		if (findFrom != std::string::npos)
		{
			std::size_t findTo = str.find(to, findFrom + from.size());
			if (findTo != std::string::npos && findTo > findFrom)
			{
				result = str.substr(findFrom + from.size(), findTo - findFrom - from.size());
			}
		}
	}

	return result;
}

std::string LyricsLoader::FilterOutputHtmlTags(const std::string &str, bool encode, bool prn, bool p2rn, bool plain)
{
	// Remove all \r\n
	// <br/> tag = \r\n
	// <p> tag = \r\n\r\n
	// Remove all HTML tags

	std::string result;
	bool intag = false;

	for (std::size_t find = 0, start = 0; ; ++find)
	{
		if (str[find] == '<')
		{
			intag = true;

			if (encode)
				FilterOutputHtmlEncode(str, start, find, result);
			else
				result.append(str.substr(start, find - start));

			start = find + 1;

			if (str[find + 1] == 'b' && str[find + 2] == 'r' &&
				(str[find + 3] == '>' || str[find + 3] == '/' || str[find + 3] == ' '))
			{
				result.append("\r\n");
			}
			else if (prn && str[find + 1] == 'p' &&
				(str[find + 2] == '>' || str[find + 2] == ' '))
			{
				if (p2rn)
					result.append("\r\n\r\n");
				else
					result.append("\r\n");
			}
		}
		else if (str[find] == '>')
		{
			intag = false;

			start = find + 1;
		}
		else if (intag) // skip everything if inside tag
		{
			if (str[find] == '\0') // except null char
				break;
		}
		else if (str[find] == '\r') // \r\n - Windows new line
		{
			if (encode)
				FilterOutputHtmlEncode(str, start, find, result);
			else
				result.append(str.substr(start, find - start));

			if (str[find + 1] == '\n')
				++find;

			start = find + 1;

			if (plain)
				result.append("\r\n");
		}
		else if (str[find] == '\n') // \n - Unix new line
		{
			if (encode)
				FilterOutputHtmlEncode(str, start, find, result);
			else
				result.append(str.substr(start, find - start));

			start = find + 1;

			if (plain)
				result.append("\r\n");
		}
		else if (str[find] == '\0')
		{
			if (encode)
				FilterOutputHtmlEncode(str, start, find, result);
			else
				result.append(str.substr(start, find - start));

			break;
		}
		else if (str[find] == '\t') // ignore tabs inside lyrics
		{
			start = find + 1;
		}
	}

	return result;
}

void LyricsLoader::FilterOutputHtmlEncode(const std::string &src, std::size_t start, std::size_t end, std::string &dst)
{
	// Encode HTML in format &#123;&#234; to UTF8 text
	// http://en.wikipedia.org/wiki/Character_encodings_in_HTML

	if (start >= end || end > src.size())
		return;

	for (std::size_t i = start; i < end; ++i)
	{
		if (src[i] == '&' && i + 1 < end && src[i + 1] == '#')
		{
			int num = 0;
			for (i = i + 2; i < end && src[i] != ';'; ++i)
			{
				if (src[i] >= '0' && src[i] <= '9')
					num = (num * 10) + (src[i] - '0');
			}

			if (num > 0)
			{
				// To UTF8
				if (num <= 0x7F)
				{
					if ((char)num != '\r' && (char)num != '\n')
						dst.append(1, (char)num);
				}
				else if (num <= 0x07FF)
				{
					dst.append(1, (char)(0xC0 | (num >> 6)));
					dst.append(1, (char)(0x80 | (num & 0x3F)));
				}
				else if (num <= 0xFFFF)
				{
					dst.append(1, (char)(0xE0 |  (num >> 12)));
					dst.append(1, (char)(0x80 | ((num >> 6) & 0x3F)));
					dst.append(1, (char)(0x80 | ( num       & 0x3F)));
				}
				else if (num <= 0x1FFFFF)
				{
					dst.append(1, (char)(0xF0 |  (num >> 18)));
					dst.append(1, (char)(0x80 | ((num >> 12) & 0x3F)));
					dst.append(1, (char)(0x80 | ((num >>  6) & 0x3F)));
					dst.append(1, (char)(0x80 | ( num        & 0x3F)));
				}
			}
		}
		else
		{
			if (src[i] != '\r' && src[i] != '\n')
				dst.append(1, src[i]);
		}
	}
}

void LyricsLoader::FilterOutputTrim(std::string &str)
{
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
}

std::string LyricsLoader::ProviderLyricsWikiaCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = urlArtist;
	std::string title = urlTitle;
	std::replace(artist.begin(), artist.end(), ' ', '_');
	std::replace(title.begin(), title.end(), ' ', '_');
	artist = FilterInputUriEncode(artist, true);
	title = FilterInputUriEncode(title, true);

	std::string url = "http://lyrics.wikia.com/wiki/" + artist + ":" + title;

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div class='lyricbox'>", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, true);
				FilterOutputTrim(lyrics);
			}
		}
	}
/*
	std::string url = "http://lyrics.wikia.com/" + artist + ":" + title + "?action=edit";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "&lt;lyrics>", "&lt;/lyrics>");

			if (!lyrics.empty())
				FilterOutputTrim(lyrics);
		}
	}
*/
	return lyrics;
}

std::string LyricsLoader::ProviderAZLyricsCom(const std::string& urlArtist, const std::string& urlTitle)
{
	// plyrics.com
	// urbanlyrics.com

	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist);
	std::string title = FilterInputStripSpaces(urlTitle);
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://www.azlyrics.com/lyrics/" + artist + "/" + title + ".html";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div class=\"lyricsh\">", "<div>", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderMetroLyricsCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '-');
	std::string title = FilterInputStripSpaces(urlTitle, '-');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "http://www.metrolyrics.com/" + title + "-lyrics-" + artist + ".html";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div id=\"lyrics-body-text\"", "<strong>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, false, true, true);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderSongLyricsCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '-');
	std::string title = FilterInputStripSpaces(urlTitle, '-');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "http://www.songlyrics.com/" + artist + "/" + title + "-lyrics" + "/";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div id=\"songLyricsDiv-outer\">", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, true);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderOldieLyricsCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '_');
	std::string title = FilterInputStripSpaces(urlTitle, '_');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://www.oldielyrics.com/lyrics/" + artist + "/" + title + ".html";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div id=\"lyrics\">", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, false, true, true);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderChartLyricsCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputUriEncode(urlArtist, true);
	std::string title = FilterInputUriEncode(urlTitle, true);

	std::string url = "http://api.chartlyrics.com/apiv1.asmx/SearchLyricDirect?artist=" + artist + "&song=" + title;

	std::string apixml;

	if (HttpClient::GetHttpPage(url, apixml))
	{
		if (!apixml.empty())
		{
			XmlFile xmlFile;
			if (xmlFile.LoadBuffer(apixml.c_str(), apixml.size()))
			{
				XmlNode xmlMain = xmlFile.RootNode().FirstChild("GetLyricResult");
				if (xmlMain)
				{
					XmlNode xmlLyric = xmlMain.FirstChild("Lyric");
					if (xmlLyric)
						lyrics = xmlLyric.Value8();
				}
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderLyricsManiaCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '_');
	std::string title = FilterInputStripSpaces(urlTitle, '_');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://www.lyricsmania.com/" + title + "_lyrics_" + artist + ".html";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div class=\"lyrics-body\"", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderGeniusCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '-');
	std::string title = FilterInputStripSpaces(urlTitle, '-');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://genius.com/" + artist + "-" + title + "-lyrics";

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div class=\"lyrics\">", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderLetrasMusBr(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '_');
	std::string title = FilterInputStripSpaces(urlTitle, '_');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://www.letras.mus.br/winamp.php?musica=" + title + "&artista=" + artist;

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<div id=\"letra-cnt\">", "</div>");

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, false, true, true);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}

std::string LyricsLoader::ProviderMusixmatchCom(const std::string& urlArtist, const std::string& urlTitle)
{
	std::string lyrics;

	std::string artist = FilterInputStripSpaces(urlArtist, '-');
	std::string title = FilterInputStripSpaces(urlTitle, '-');
	FilterInputLowerAscii(artist);
	FilterInputLowerAscii(title);

	std::string url = "https://www.musixmatch.com/lyrics/" + artist + "/" + title;

	if (HttpClient::GetHttpPage(url, lyrics))
	{
		if (!lyrics.empty())
		{
			lyrics = FilterOutputFromTo(lyrics, "<p class=\"mxm-lyrics__content \">", "</span>");

			// Remove script part
			if (!lyrics.empty())
			{
				std::string from = "</p>"; //"googletag.cmd.push";
				std::string to = "});";
				std::size_t findFrom = lyrics.find(from);
				if (findFrom != std::string::npos)
				{
					std::size_t findTo = lyrics.find(to, findFrom + from.size());
					if (findTo != std::string::npos)
					{
						lyrics.erase(findFrom, findTo - findFrom + to.size());
					}
				}
			}

			if (!lyrics.empty())
			{
				lyrics = FilterOutputHtmlTags(lyrics, false, false, false, true);
				FilterOutputTrim(lyrics);
			}
		}
	}

	return lyrics;
}