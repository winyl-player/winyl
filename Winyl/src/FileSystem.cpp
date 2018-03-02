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
#include "FileSystem.h"
#include <chrono>

namespace FileSystem
{

bool Exists(const std::wstring& file)
{
	WIN32_FILE_ATTRIBUTE_DATA fd;
	if (::GetFileAttributesExW(file.c_str(), GetFileExInfoStandard, &fd))
		return true;

	return false;
}

bool CreateDir(const std::wstring& path)
{
	if (::CreateDirectoryW(path.c_str(), NULL))
		return true;

	return false;
}

bool RemoveDir(const std::wstring& path)
{
	if (::RemoveDirectoryW(path.c_str()))
		return true;

	return false;
}

bool RemoveFile(const std::wstring& file)
{
	if (::DeleteFileW(file.c_str()))
		return true;

	return false;
}

bool Rename(const std::wstring& oldname, const std::wstring& newname)
{
	if (::MoveFileW(oldname.c_str(), newname.c_str()))
		return true;

	return false;
}

bool RemoveDirs(const std::wstring& path)
{
	assert(!path.empty());
	assert(path.back() != '\\');

	std::wstring pathSlash = path;
	pathSlash.push_back('\\');
	std::wstring pathStar = pathSlash;
	pathStar.push_back('*');

	std::unique_ptr<WIN32_FIND_DATAW> fd(new WIN32_FIND_DATAW);

	HANDLE findHandle = INVALID_HANDLE_VALUE;
	if (futureWin->IsSevenOrLater())
		findHandle = ::FindFirstFileExW(pathStar.c_str(), FindExInfoBasic, fd.get(), FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	else
		findHandle = ::FindFirstFileExW(pathStar.c_str(), FindExInfoStandard, fd.get(), FindExSearchNameMatch, NULL, 0);

	if (findHandle != INVALID_HANDLE_VALUE)
	do
	{
		if (fd->cFileName[0] == '.' &&
			(fd->cFileName[1] == '\0' || (fd->cFileName[1] == '.' && fd->cFileName[2] == '\0')))
			continue;

		if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			RemoveDirs(pathSlash + fd->cFileName);
		}
		else
		{
			::DeleteFileW((pathSlash + fd->cFileName).c_str());
		}
	}
	while (::FindNextFileW(findHandle, fd.get()));

	if (findHandle != INVALID_HANDLE_VALUE)
		::FindClose(findHandle);

	if (::RemoveDirectoryW(path.c_str()))
		return true;

	return false;
}

std::wstring GetProgramPath()
{
	wchar_t program[MAX_PATH * 2];
	program[0] = '\0';
	::GetModuleFileNameW(::GetModuleHandleW(NULL), program, MAX_PATH * 2);

	std::wstring programPath = program;
	programPath = programPath.substr(0, programPath.rfind('\\') + 1);

	return programPath;
}

std::wstring GetAppDataPath()
{
	wchar_t profile[MAX_PATH];
	profile[0] = '\0';
	::SHGetSpecialFolderPathW(NULL, profile, CSIDL_APPDATA, FALSE); //CSIDL_COMMON_APPDATA

	std::wstring profilePath = profile;
	profilePath.push_back('\\');

	return profilePath;
}

std::wstring GetCurrentDir()
{
	wchar_t directory[MAX_PATH * 2];
	directory[0] = '\0';
	::GetCurrentDirectoryW(MAX_PATH * 2, directory);

	std::wstring currentDir = directory;
	currentDir.push_back('\\');

	return currentDir;
}

long long GetTimeNow()
{
	SYSTEMTIME st = {};
	::GetSystemTime(&st);

	FILETIME ft = {};
	::SystemTimeToFileTime(&st, &ft);

	ULARGE_INTEGER ull = {};
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (long long)(ull.QuadPart / 10000000ULL - 11644473600ULL);
}

long long GetTimeNowMs()
{
//	return std::chrono::duration_cast<std::chrono::milliseconds>(
//		std::chrono::system_clock::now().time_since_epoch()).count();

	SYSTEMTIME st = {};
	::GetSystemTime(&st);

	FILETIME ft = {};
	::SystemTimeToFileTime(&st, &ft);

	ULARGE_INTEGER ull = {};
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;
	return (long long)(ull.QuadPart / 10000ULL - 11644473600000ULL);
}

// Class Find

Find::Find(const std::wstring& path)
{
	assert(!path.empty());
	assert(path.back() == '\\');

	findPath = path;
	findPath.push_back('*');
}

Find::Find(const std::wstring& path, const std::wstring& match)
{
	assert(!path.empty());
	assert(path.back() == '\\');
	assert(!match.empty());

	findPath = path + match;
}

Find::~Find()
{
	if (findHandle != INVALID_HANDLE_VALUE)
		::FindClose(findHandle);
}

bool Find::Next()
{
	if (findHandle == INVALID_HANDLE_VALUE)
	{
		fd = std::make_unique<WIN32_FIND_DATAW>();

		if (futureWin->IsSevenOrLater())
			findHandle = ::FindFirstFileExW(findPath.c_str(), FindExInfoBasic, fd.get(), FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
		else
			findHandle = ::FindFirstFileExW(findPath.c_str(), FindExInfoStandard, fd.get(), FindExSearchNameMatch, NULL, 0);

		if (findHandle != INVALID_HANDLE_VALUE)
		{
			if (IsDots()) // Skip "." and ".."
				return FindNext();

			return true;
		}

		fd.reset();
		return false;
	}

	return FindNext();
}

bool Find::FindNext()
{
	while (::FindNextFileW(findHandle, fd.get()))
	{
		if (IsDots()) // Skip "." and ".."
			continue;
		else
			return true;
	}

	::FindClose(findHandle);
	findHandle = INVALID_HANDLE_VALUE;

	return false;
}

bool Find::IsDots()
{
	//if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (fd->cFileName[0] == '.' &&
			(fd->cFileName[1] == '\0' || (fd->cFileName[1] == '.' && fd->cFileName[2] == '\0')))
			return true;
	}

	return false;
}

bool Find::IsDirectory()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}

	return false;
}

bool Find::IsHidden()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		if (fd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			return true;
	}

	return false;
}

std::wstring Find::GetFileName()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		return fd->cFileName;
	}

	return std::wstring();
}

long long Find::GetFileSize()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		ULARGE_INTEGER sz = {fd->nFileSizeLow, fd->nFileSizeHigh};
		return (long long)sz.QuadPart;
	}

	return 0;
}

long long Find::GetModified()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		//SYSTEMTIME st;
		//if (::FileTimeToSystemTime(&fd->ftLastWriteTime, &st))
		//	return PathEx::DateTimeString(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		ULARGE_INTEGER ull;
		ull.LowPart = fd->ftLastWriteTime.dwLowDateTime;
		ull.HighPart = fd->ftLastWriteTime.dwHighDateTime;
		return (long long)(ull.QuadPart / 10000000ULL - 11644473600ULL);
	}

	return 0;
}

// Class FindFile

FindFile::FindFile(const std::wstring& file)
{
	if (futureWin->IsSevenOrLater())
		findHandle = ::FindFirstFileExW(file.c_str(), FindExInfoBasic, &fd, FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);
	else
		findHandle = ::FindFirstFileExW(file.c_str(), FindExInfoStandard, &fd, FindExSearchNameMatch, NULL, 0);

	if (findHandle != INVALID_HANDLE_VALUE)
	{
		::FindClose(findHandle);

		if (IsDots())
			findHandle = INVALID_HANDLE_VALUE;
		else
			findHandle = NULL;
	}
}

FindFile::~FindFile()
{

}

bool FindFile::IsDots()
{
	//if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (fd.cFileName[0] == '.' &&
			(fd.cFileName[1] == '\0' || (fd.cFileName[1] == '.' && fd.cFileName[2] == '\0')))
			return true;
	}

	return false;
}

bool FindFile::IsDirectory()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return true;
	}

	return false;
}

std::wstring FindFile::GetFileName()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		return fd.cFileName;
	}

	return std::wstring();
}

long long FindFile::GetFileSize()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		ULARGE_INTEGER sz = {fd.nFileSizeLow, fd.nFileSizeHigh};
		return (long long)sz.QuadPart;
	}

	return 0;
}

long long FindFile::GetModified()
{
	assert(findHandle != INVALID_HANDLE_VALUE);
	if (findHandle != INVALID_HANDLE_VALUE)
	{
		//SYSTEMTIME st;
		//if (::FileTimeToSystemTime(&fd.ftLastWriteTime, &st))
		//	return PathEx::DateTimeString(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		ULARGE_INTEGER ull;
		ull.LowPart = fd.ftLastWriteTime.dwLowDateTime;
		ull.HighPart = fd.ftLastWriteTime.dwHighDateTime;
		return (long long)(ull.QuadPart / 10000000ULL - 11644473600ULL);
	}

	return 0;
}

} // namespace FileSystem
