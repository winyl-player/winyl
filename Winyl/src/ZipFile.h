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

#ifdef PACKSKIN // zlib is only used by PackSkin utility
#define ZLIB_WINAPI
#include "zlib/zip.h"
#include "zlib/unzip.h"
#endif // PACKSKIN

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>

class ZipFile
{
public:
	ZipFile();
	virtual ~ZipFile();

	bool OpenFile(const std::wstring& file);
	bool UnzipToBuffer(const std::wstring& file);
	bool CheckFileInZip(const std::wstring& file);
	void FreeBuffer();
	char* GetBuffer() {return zipBuffer;}
	unsigned GetBufferSize() {return zipBufferSize;}

	static bool ZipPathToFile(const std::wstring& file, const std::wstring& path, int level = 0);
	static bool UnzipFileToPath(const std::wstring& file, const std::wstring& path);

private:
	char* zipBuffer = nullptr;
	unsigned zipBufferSize = 0;

	struct MAP_FILES
	{
		unsigned int offset;
		unsigned int size;
	};

	std::map<std::string, MAP_FILES> zipFiles;
	std::ifstream zipStream;

#ifdef PACKSKIN
	static void ZipPathRecur(zipFile zfile, const std::wstring& folderPath, const std::wstring& folderZip, int level);
	static void UnzipFile(zipFile zfile, const std::wstring& folderPath);

	static bool SetFileTimeWithFileInfo(HANDLE fileHandle, unz_file_info64* fi);

	static bool CreateFullPath(const std::wstring& path, unz_file_info64* fi);
	static bool CreateDirectoryWithFileInfo(const std::wstring& path, unz_file_info64* fi);

	// There is iowin32.c for WinAPI IO in minizip (part of zlib)
	// but it doesn't use buffering for read/write and it's very slow (sometimes it reads/writes by one byte)
	// so use functions from ioapi.c with some modifications for file names in unicode.
	// ---
	// Advanced version of minizip: https://github.com/nmoinvaz/minizip

	static voidpf fopen64_file_func (voidpf opaque, const void* filename, int mode);
	static uLong fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size);
	static uLong fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size);
	static ZPOS64_T ftell64_file_func (voidpf opaque, voidpf stream);
	static long fseek64_file_func (voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin);
	static int fclose_file_func (voidpf opaque, voidpf stream);
	static int ferror_file_func (voidpf opaque, voidpf stream);
	static void fill_fopen64_filefunc (zlib_filefunc64_def*  pzlib_filefunc_def);
#endif // PACKSKIN
};
