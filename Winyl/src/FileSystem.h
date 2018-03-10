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

#include <windows.h>
#include <string>
#include <memory>
#include "UTF.h"

namespace FileSystem
{

bool Exists(const std::wstring& file);
bool CreateDir(const std::wstring& path);
bool RemoveDir(const std::wstring& path);
bool RemoveFile(const std::wstring& file);
bool Rename(const std::wstring& oldname, const std::wstring& newname);
bool RemoveDirs(const std::wstring& path);
std::wstring GetProgramPath();
std::wstring GetAppDataPath();
std::wstring GetCurrentDir();

long long GetTimeNow();
long long GetTimeNowMs();

class Find
{
public:
	Find(const std::wstring& path);
	Find(const std::wstring& path, const std::wstring& match);
	virtual ~Find();
	Find(const Find&) = delete;
	Find& operator=(const Find&) = delete;

	bool Next();
	bool IsDirectory();
	bool IsHidden();
	std::wstring GetFileName();
	long long GetFileSize();
	long long GetModified();

private:
	std::wstring findPath;
	HANDLE findHandle = INVALID_HANDLE_VALUE;
	std::unique_ptr<WIN32_FIND_DATAW> fd;

	bool FindNext();
	bool IsDots();
};

class FindFile
{
public:
	FindFile(const std::wstring& file);
	virtual ~FindFile();
	FindFile(const FindFile&) = delete;
	FindFile& operator=(const FindFile&) = delete;
	bool IsFound() {return (findHandle != INVALID_HANDLE_VALUE ? true : false);}

	bool IsDirectory();
	std::wstring GetFileName();
	long long GetFileSize();
	long long GetModified();

private:
	bool IsDots();

	HANDLE findHandle = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW fd;
};

} // namespace FileSystem

