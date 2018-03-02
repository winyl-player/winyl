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
#include <algorithm>
#include <cassert>

// Important Note!
// The following WINAPI functions: WideCharToMultiByte, MultiByteToWideChar, LCMapString, LCMapStringEx, FoldString
// return number of characters in the translated string,
// but be aware that if -1 is used as input for a source string length
// then the number of characters will be with null terminator (+1 char).
// For example, because of this I did -1 for len in UTF16(const char* utf8) func.
// ---
// Also NormalizeString always return bigger length than needed,
// I added asserts to Normalize functions to don't forget it if NormalizeString will be used.

class UTF
{
public:
	inline static std::string UTF8S(const std::wstring& utf16)
	{
		std::string utf8;

		//int len = ((int)utf16.size() * 4) + 1;
		int len = ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), (int)utf16.size(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			utf8.resize(len);
			::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), (int)utf16.size(), &utf8[0], len, NULL, NULL);
		}

		return utf8;
	}

	inline static std::wstring UTF16S(const std::string& utf8)
	{
		std::wstring utf16;

		//int len = (int)utf8.size() + 1;
		int len = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), NULL, 0);
		if (len > 0)
		{
			utf16.resize(len);
			::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.size(), &utf16[0], len);
		}

		return utf16;
	}

	inline static std::string UTF8(const wchar_t* utf16)
	{
		std::string utf8;

		if (utf16 == nullptr)
			return utf8;

		//int len = (int)(wcslen(utf16) * 4) + 1;
		int len = ::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL) - 1;
		if (len > 0)
		{
			utf8.resize(len);
			::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, &utf8[0], len, NULL, NULL);
		}

		return utf8;
	}

	inline static std::wstring UTF16(const char* utf8)
	{
		std::wstring utf16;

		if (utf8 == nullptr)
			return utf16;

		//int len = (int)strlen(utf8) + 1;
		int len = ::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0) - 1;
		if (len > 0)
		{
			utf16.resize(len);
			::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, &utf16[0], len);
		}

		return utf16;
	}

	inline static std::wstring UTF16A(const char* ansi)
	{
		std::wstring utf16;

		if (ansi == nullptr)
			return utf16;

		int len = ::MultiByteToWideChar(CP_ACP, 0, ansi, -1, NULL, 0) - 1;
		if (len > 0)
		{
			utf16.resize(len);
			::MultiByteToWideChar(CP_ACP, 0, ansi, -1, &utf16[0], len);
		}

		return utf16;
	}

	inline static std::wstring UTF16AS(const std::string& ansi)
	{
		std::wstring utf16;
		
		int len = ::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), (int)ansi.size(), NULL, 0);
		if (len > 0)
		{
			utf16.resize(len);
			::MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), (int)ansi.size(), &utf16[0], len);
		}

		return utf16;
	}
	inline static std::string ANSI16S(const std::wstring& utf16)
	{
		std::string ansi;

		int len = ::WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), (int)utf16.size(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			ansi.resize(len);
			::WideCharToMultiByte(CP_ACP, 0, utf16.c_str(), (int)utf16.size(), &ansi[0], len, NULL, NULL);
		}

		return ansi;
	}

	static bool IsUTF8(const char* str)
	{
		// http://stackoverflow.com/questions/1031645/how-to-detect-utf-8-in-plain-c

		if (!str)
			return false;

		const unsigned char* bytes = (const unsigned char*)str;
		while (*bytes)
		{
			//if ((// ASCII
			//	// use bytes[0] <= 0x7F to allow ASCII control characters
			//	bytes[0] == 0x09 ||
			//	bytes[0] == 0x0A ||
			//	bytes[0] == 0x0D ||
			//	(0x20 <= bytes[0] && bytes[0] <= 0x7E)))
			if (bytes[0] <= 0x7F)
			{
				bytes += 1;
				continue;
			}

			if ((// non-overlong 2-byte
				(0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
				(0x80 <= bytes[1] && bytes[1] <= 0xBF))) 
			{
				bytes += 2;
				continue;
			}

			if ((// excluding overlongs
				bytes[0] == 0xE0 &&
				(0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
				(0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
				(// straight 3-byte
					((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
						bytes[0] == 0xEE ||
						bytes[0] == 0xEF) &&
					(0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
					(0x80 <= bytes[2] && bytes[2] <= 0xBF)) ||
				(// excluding surrogates
					bytes[0] == 0xED &&
					(0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
					(0x80 <= bytes[2] && bytes[2] <= 0xBF)))
			{
				bytes += 3;
				continue;
			}

			if ((// planes 1-3
				bytes[0] == 0xF0 &&
				(0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
				(0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
				(0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
				(// planes 4-15
					(0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
					(0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
					(0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
					(0x80 <= bytes[3] && bytes[3] <= 0xBF)) ||
				(// plane 16
					bytes[0] == 0xF4 &&
					(0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
					(0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
					(0x80 <= bytes[3] && bytes[3] <= 0xBF)))
			{
				bytes += 4;
				continue;
			}

			return false;
		}

		return true;
	}
};

class StringEx
{
public:
	template<typename T>
	inline static void TrimLeft(std::basic_string<T>& str)
	{
		if (!str.empty())
			str.erase(0, str.find_first_not_of(' '));
	}
	template<typename T>
	inline static void TrimRight(std::basic_string<T>& str)
	{
		if (!str.empty())
			str.erase(str.find_last_not_of(' ') + 1);
	}
	template<typename T>
	inline static void Trim(std::basic_string<T>& str)
	{
		if (!str.empty())
		{
			str.erase(str.find_last_not_of(' ') + 1); // erase(pos) == resize(pos) -> very fast -> must be first
			str.erase(0, str.find_first_not_of(' '));
		}
	}
	template<typename T>
	inline static void MakeLowerAscii(std::basic_string<T>& str)
	{
		for (std::size_t i = 0, size = str.size(); i < size; ++i)
		{
			if (str[i] >= 'A' && str[i] <= 'Z')
			{
				str[i] = str[i] ^ 32;
			}
		}
	}
	template<typename T>
	inline static void MakeUpperAscii(std::basic_string<T>& str)
	{
		for (std::size_t i = 0, size = str.size(); i < size; ++i)
		{
			if (str[i] >= 'a' && str[i] <= 'z')
			{
				str[i] = str[i] ^ 32;
			}
		}
	}
	//template<typename T>
	//inline static std::basic_string<T> ToLowerAscii(const std::basic_string<T>& str)
	//{
	//	std::basic_string<T> result = str;
	//	MakeLowerAscii(result);
	//	return result;
	//}
	//template<typename T>
	//inline static std::basic_string<T> ToUpperAscii(const std::basic_string<T>& str)
	//{
	//	std::basic_string<T> result = str;
	//	MakeUpperAscii(result);
	//	return result;
	//}
	template<typename T>
	inline static void Replace(std::basic_string<T>& str, char from, char to)
	{
		std::replace(str.begin(), str.end(), from, to);
	}
	inline static void ReplaceString(std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		std::size_t index = str.find(from);
		while (index != std::string::npos)
		{
			str.replace(index, from.size(), to);
			index = str.find(from, index + to.size());
		}
	}
	inline static int IsEqual(const std::wstring& str1, const std::wstring& str2)
	{
		return ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, str1.c_str(), -1, str2.c_str(), -1) == CSTR_EQUAL;
	}
	inline static int IsEqual(const std::wstring& str1, const std::wstring& str2, std::size_t len)
	{
		len = std::min(len, str1.size());
		len = std::min(len, str2.size());

		return ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE, str1.c_str(), len, str2.c_str(), len) == CSTR_EQUAL;
	}

	// http://stackoverflow.com/questions/1068134/comparing-wstring-with-ignoring-the-case
	// http://stackoverflow.com/questions/13656014/extremely-fast-is-iequal-case-insensitive-equality-comparison
private:
	template<typename T>
	inline static bool IsEqualAsciiImpl(const T& str1, const T& str2, std::size_t len)
	{
		for (std::size_t i = 0; i < len; ++i)
		{
			assert(str1[i] != '\0');
			assert(str2[i] != '\0');

			if (str1[i] == str2[i])
				continue;
			else if ((str1[i] >= 'a' && str1[i] <= 'z') || (str1[i] >= 'A' && str1[i] <= 'Z'))
			{
				if (str1[i] != (str2[i] ^ 32))
					return false;
			}
			else
				return false;
		}

		return true;
	}
public:
	inline static bool IsEqualAsciiRaw(const char* str1, const char* str2)
	{
		std::size_t len1 = strlen(str1);
		std::size_t len2 = strlen(str2);

		if (len1 != len2)
			return false;

		return IsEqualAsciiImpl(str1, str2, len1);
	}
	inline static bool IsEqualAsciiRaw(const char* str1, const char* str2, std::size_t len)
	{
		std::size_t len1 = strlen(str1);
		std::size_t len2 = strlen(str2);

		if (len > len1 || len > len2)
			return false;

		return IsEqualAsciiImpl(str1, str2, len);
	}
	inline static bool IsEqualAsciiRaw(const wchar_t* str1, const wchar_t* str2)
	{
		std::size_t len1 = wcslen(str1);
		std::size_t len2 = wcslen(str2);

		if (len1 != len2)
			return false;

		return IsEqualAsciiImpl(str1, str2, len1);
	}
	inline static bool IsEqualAsciiRaw(const wchar_t* str1, const wchar_t* str2, std::size_t len)
	{
		std::size_t len1 = wcslen(str1);
		std::size_t len2 = wcslen(str2);

		if (len > len1 || len > len2)
			return false;

		return IsEqualAsciiImpl(str1, str2, len);
	}
	inline static bool IsEqualAscii(const std::string& str1, const std::string& str2)
	{
		if (str1.size() != str2.size())
			return false;

		return IsEqualAsciiImpl(str1.c_str(), str2.c_str(), str1.size());
	}
	inline static bool IsEqualAscii(const std::string& str1, const std::string& str2, std::size_t len)
	{
		if (len > str1.size() || len > str2.size())
			return false;

		return IsEqualAsciiImpl(str1.c_str(), str2.c_str(), len);
	}
	inline static bool IsEqualAscii(const std::wstring& str1, const std::wstring& str2)
	{
		if (str1.size() != str2.size())
			return false;

		return IsEqualAsciiImpl(str1.c_str(), str2.c_str(), str1.size());
	}
	inline static bool IsEqualAscii(const std::wstring& str1, const std::wstring& str2, std::size_t len)
	{
		if (len > str1.size() || len > str2.size())
			return false;

		return IsEqualAsciiImpl(str1.c_str(), str2.c_str(), len);
	}

//	inline static void FromInt(std::string& str, int i)
//	{
//		char temp[64];
//		_itoa_s(i, temp, 10);
//		str = temp;
//	}
//	inline static void FromInt(std::wstring& str, int i)
//	{
//		wchar_t temp[64];
//		temp[0] = '\0';
//		_itow_s(i, temp, 10);
//		str = temp;
//	}

	inline static std::wstring Format(const wchar_t* format, ...)
	{
		wchar_t temp[128];
		temp[0] = '\0';
		va_list args;
		va_start(args, format);
		//vswprintf(temp, format, args);
		vswprintf_s(temp, format, args);
		va_end(args);
		return temp;
	}

	inline static std::wstring FormatFloat(const wchar_t* format, ...)
	{
		wchar_t temp[128];
		temp[0] = '\0';
		va_list args;
		va_start(args, format);
		//vswprintf(temp, format, args);
		vswprintf_s(temp, format, args);
		va_end(args);
		std::wstring str = temp;

		// Fix the float string. Example:
		// 14.08 -> 14,08
		// 14.00 -> 14

		wchar_t sDecimal[16]; sDecimal[0] = '\0';
		::GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, sDecimal, sizeof(sDecimal) / sizeof(wchar_t));

		wchar_t decimalPoint = '.';
		if (sDecimal[0])
			decimalPoint = sDecimal[0];

		int foundDot = 0;
		bool removeDot = true;
		for (std::size_t i = 0, size = str.size(); i < size; ++i)
		{
			if (foundDot)
			{
				if (str[i] != '0')
					removeDot = false;
			}
			else if (str[i] == '.')
			{
				str[i] = decimalPoint;
				foundDot = i;
			}
		}
		if (removeDot)
			str.resize(foundDot);

		return str;
	}

	inline static int HashFNV1a32(const std::wstring& str)
	{
		unsigned int hash = 2166136261;

		for (std::size_t i = 0, size = str.size(); i < size; ++i)
		{
			hash ^= str[i];
			hash *= 16777619;
		}

		return (int)hash;
	}

	inline static long long HashFNV1a64(const std::wstring& str)
	{
		unsigned long long hash = 14695981039346656037ULL;

		for (std::size_t i = 0, size = str.size(); i < size; ++i)
		{
			hash ^= str[i];
			hash *= 1099511628211ULL;
		}

		return (long long)hash;
	}



	inline static std::wstring ToUpper(const std::wstring& text)
	{
		std::wstring upper;

		int len = ::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			upper.resize(len);
			::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &upper[0], len);
			//::LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &upper[0], len, 0, 0, 0);
		}

		return upper;
	}

	// Do not use invariant locale, this is Microsoft specific. Use en-US instead.
	/*inline static std::wstring ToUpperInvariant(const std::wstring& text)
	{
		std::wstring upper;

		int len = ::LCMapStringW(LOCALE_INVARIANT, LCMAP_UPPERCASE, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			upper.resize(len);
			::LCMapStringW(LOCALE_INVARIANT, LCMAP_UPPERCASE, text.c_str(), (int)text.size(), &upper[0], len);
			//::LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE, text.c_str(), (int)text.size(), &upper[0], len, 0, 0, 0);
		}

		return upper;
	}*/

	inline static std::wstring ToUpperUS(const std::wstring& text)
	{
		std::wstring upper;

		int len = ::LCMapStringW(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(L"en-US", LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			upper.resize(len);
			::LCMapStringW(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &upper[0], len);
			//::LCMapStringEx(L"en-US", LCMAP_UPPERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &upper[0], len, 0, 0, 0);
		}

		return upper;
	}

	inline static std::wstring ToLower(const std::wstring& text)
	{
		std::wstring lower;

		int len = ::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			lower.resize(len);
			::LCMapStringW(LOCALE_USER_DEFAULT, LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &lower[0], len);
			//::LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &lower[0], len, 0, 0, 0);
		}

		return lower;
	}

	// Do not use invariant locale, this is Microsoft specific. Use en-US instead.
	/*inline static std::wstring ToLowerInvariant(const std::wstring& text)
	{
		std::wstring lower;

		int len = ::LCMapStringW(LOCALE_INVARIANT, LCMAP_LOWERCASE, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			lower.resize(len);
			::LCMapStringW(LOCALE_INVARIANT, LCMAP_LOWERCASE, text.c_str(), (int)text.size(), &lower[0], len);
			//::LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_LOWERCASE, text.c_str(), (int)text.size(), &lower[0], len, 0, 0, 0);
		}

		return lower;
	}*/

	inline static std::wstring ToLowerUS(const std::wstring& text)
	{
		std::wstring lower;

		int len = ::LCMapStringW(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::LCMapStringEx(L"en-US", LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), NULL, 0, 0, 0, 0);
		if (len > 0)
		{
			lower.resize(len);
			::LCMapStringW(MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT), LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &lower[0], len);
			//::LCMapStringEx(L"en-US", LCMAP_LOWERCASE|LCMAP_LINGUISTIC_CASING, text.c_str(), (int)text.size(), &lower[0], len, 0, 0, 0);
		}

		return lower;
	}

	// Quote from boost:
	// We use FoldString, under Vista it actually does normalization;
	// under XP and below it does something similar, half job, better then nothing

	inline static std::wstring NormalizeC(const std::wstring& text)
	{
		std::wstring result;

		int len = ::FoldStringW(MAP_PRECOMPOSED, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::NormalizeString(NormalizationC, text.c_str(), (int)text.size(), NULL, 0);
		if (len > 0)
		{
			result.resize(len);
			::FoldStringW(MAP_PRECOMPOSED, text.c_str(), (int)text.size(), &result[0], len);
			//len = ::NormalizeString(NormalizationC, text.c_str(), (int)text.size(), &result[0], len);
			//assert(result.size() == wcslen(result.c_str())); // see Important Note
			//result.resize(len);
		}

		return result;
	}

	inline static std::wstring NormalizeD(const std::wstring& text)
	{
		std::wstring result;

		int len = ::FoldStringW(MAP_COMPOSITE, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::NormalizeString(NormalizationD, text.c_str(), (int)text.size(), NULL, 0);
		if (len > 0)
		{
			result.resize(len);
			::FoldStringW(MAP_COMPOSITE, text.c_str(), (int)text.size(), &result[0], len);
			//len = ::NormalizeString(NormalizationD, text.c_str(), (int)text.size(), &result[0], len);
			//assert(result.size() == wcslen(result.c_str())); // see Important Note
			//result.resize(len);
		}

		return result;
	}

	inline static std::wstring NormalizeKC(const std::wstring& text)
	{
		std::wstring result;

		int len = ::FoldStringW(MAP_FOLDCZONE, text.c_str(), (int)text.size(), NULL, 0);
		//int len = ::NormalizeString(NormalizationKC, text.c_str(), (int)text.size(), NULL, 0);
		if (len > 0)
		{
			result.resize(len);
			::FoldStringW(MAP_FOLDCZONE, text.c_str(), (int)text.size(), &result[0], len);
			//len = ::NormalizeString(NormalizationKC, text.c_str(), (int)text.size(), &result[0], len);
			//assert(result.size() == wcslen(result.c_str())); // see Important Note
			//result.resize(len);
		}

		return result;
	}

	inline static std::wstring NormalizeKD(const std::wstring& text)
	{
		std::wstring result;

		int len = ::FoldStringW(MAP_FOLDCZONE|MAP_COMPOSITE, text.c_str(), -1, NULL, 0);
		//int len = ::NormalizeString(NormalizationKD, text.c_str(), -1, NULL, 0);
		if (len > 0)
		{
			result.resize(len);
			::FoldStringW(MAP_FOLDCZONE|MAP_COMPOSITE, text.c_str(), -1, &result[0], len);
			//len = ::NormalizeString(NormalizationKD, text.c_str(), -1, &result[0], len);
			//assert(result.size() == wcslen(result.c_str())); // see Important Note
			//result.resize(len);
		}

		return result;
	}
};

class PathEx
{
public:
	inline static std::string NameFromFile(const std::string& file)
	{
		std::string result = file;

		std::size_t find = file.rfind('.');
		if (find != std::string::npos)
			result = file.substr(0, find);

		return result;
	}
	inline static std::wstring NameFromFile(const std::wstring& file)
	{
		std::wstring result = file;

		std::size_t find = file.rfind('.');
		if (find != std::string::npos)
			result = file.substr(0, find);

		return result;
	}
	inline static std::wstring ExtFromFile(const std::wstring& file)
	{
		std::wstring result;

		std::size_t find = file.rfind('.');
		if (find != std::string::npos)
		{
			result = file.substr(find + 1);
			//std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			StringEx::MakeLowerAscii(result);
		}

		return result;
	}
	inline static std::wstring PathFromFile(const std::wstring& pathFile)
	{
		std::wstring result;

		std::size_t find = pathFile.rfind('\\');
		if (find != std::string::npos)
			result = pathFile.substr(0, find + 1);

		return result;
	}
	inline static std::wstring FileFromPath(const std::wstring& pathFile)
	{
		std::wstring result;

		std::size_t find = pathFile.rfind('\\');
		if (find != std::string::npos)
			result = pathFile.substr(find + 1);

		return result;
	}
	inline static std::wstring NameFromPath(const std::wstring& pathFile)
	{
		// FileFromPath + NameFromPath
		std::wstring result;

		std::size_t find = pathFile.rfind('\\');
		if (find != std::string::npos)
		{
			result = pathFile.substr(find + 1);

			find = result.rfind('.');
			if (find != std::string::npos)
				result = result.substr(0, find);
		}

		return result;
	}
	inline static bool IsURL(const std::wstring& path)
	{
		// http:// , https:// , ftp:// , mms://
		if (path.find(L"://") != std::string::npos)
			return true;

		return false;
	}
	//inline static std::string DateTimeString(int year, int month, int day, int hour, int minute, int second)
	//{
	//	char temp[128];
	//	temp[0] = '\0';
	//	//sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
	//	sprintf_s(temp, "%04d-%02d-%02d %02d:%02d:%02d", year, month, day, hour, minute, second);
	//	return temp;
	//}
};
