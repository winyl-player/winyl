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

// PackSkin.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>

#include "../../ZipFile.h"

#ifndef _WIN64
#pragma comment(lib, "../../zlib/zlibstat.lib")
#else
#pragma comment(lib, "../../zlib/x64/zlibstat.lib")
#endif

// Forward declaration
bool PackSkin(const std::wstring& folder);
bool UnpackSkin(const std::wstring& file);

int _tmain(int argc, _TCHAR* argv[])
{
	bool isUnpack = false;
	std::wstring str;

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '/' || argv[i][0] == '-')
		{
			wchar_t *param = (argv[i] + 1);

			if (lstrcmpi(param, L"u") == 0)
				isUnpack = true;
			if (lstrcmpi(param, L"unpack") == 0)
				isUnpack = true;
			if (lstrcmpi(param, L"unzip") == 0)
				isUnpack = true;
		}
		else
		{
			if (str.empty())
				str = argv[i];
		}
	}

	if (!str.empty())
	{
		if (!isUnpack)
			PackSkin(str);
		else
			UnpackSkin(str);
	}
	else
	{
		//puts("Do not run this program from program files folder.");
		//puts("\n");
		puts("Pack skin:    PackSkin.exe <skin folder>");
		puts("Unpack skin:  PackSkin.exe -u <skin file>");
		//puts("\n");
		system("pause");
		//_getch();
	}

	return 0;
}

bool PackSkin(const std::wstring& folder)
{
	wchar_t program[MAX_PATH*2];
	program[0] = '\0';
	::GetModuleFileNameW(::GetModuleHandleW(NULL), program, MAX_PATH*2);
	
	std::wstring programPath = program;
	programPath = programPath.substr(0, programPath.rfind('\\') + 1);

	std::wstring zipPath = programPath + folder + L"\\";
	std::wstring zipFile = programPath + folder + L".wzp";

	return ZipFile::ZipPathToFile(zipFile, zipPath);
}

bool UnpackSkin(const std::wstring& file)
{
	wchar_t program[MAX_PATH*2];
	program[0] = '\0';
	::GetModuleFileNameW(::GetModuleHandleW(NULL), program, MAX_PATH*2);
	
	std::wstring programPath = program;
	programPath = programPath.substr(0, programPath.rfind('\\') + 1);

	std::wstring folder;

	std::size_t find = file.rfind('.');

	if (find != std::wstring::npos)
		folder = file.substr(0, find);
	else
		folder = file;

	std::wstring zipPath = programPath + folder;
	std::wstring zipFile = programPath + folder + L".wzp";

	if (::GetFileAttributesW(zipFile.c_str()) == INVALID_FILE_ATTRIBUTES)
		return false;

	if (::CreateDirectoryW(zipPath.c_str(), NULL))
	{
		zipPath.push_back('\\');

		return ZipFile::UnzipFileToPath(zipFile, zipPath);
	}

	return false;
}



