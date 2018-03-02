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
#include "ZipFile.h"

ZipFile::ZipFile()
{

}

ZipFile::~ZipFile()
{

}

bool ZipFile::OpenFile(const std::wstring& file)
{
	// http://en.wikipedia.org/wiki/Zip_(file_format)#Structure
	// http://www.pkware.com/documents/casestudies/APPNOTE.TXT

	zipStream.open(file.c_str(), std::ios::in|std::ios::binary);
	if (!zipStream.is_open())
		return false;

	zipStream.seekg(0, std::ios::end);
	unsigned int filesize = (unsigned int)zipStream.tellg();
	if (filesize < 32)
	{
		zipStream.close();
		return false;
	}
	zipStream.seekg(-22, std::ios::end);

	// End of central directory record
	char headbuf[22];
	zipStream.read(headbuf, 22);

	if (!(headbuf[0] == 0x50 && headbuf[1] == 0x4b && headbuf[2] == 0x05 && headbuf[3] == 0x06))
	{
		zipStream.close();
		return false;
	}

	unsigned int dirsize = 0; // Size of central directory
	unsigned int diroffset = 0; // Offset to start of central directory

	memcpy(&dirsize, headbuf + 12, 4);
	memcpy(&diroffset, headbuf + 16, 4);

	// Central directory
	zipStream.seekg(diroffset, std::ios::beg);

	char* buffer = new char[dirsize];

	zipStream.read(buffer, dirsize);

	if (zipStream.eof())
	{
		zipStream.close();
		delete[] buffer;
		return false;
	}

	unsigned int testoffset = 0; // To test the integrity of the archive

	for (unsigned int cursor = 0; cursor < dirsize;)
	{
		unsigned int compmethod = 0; // Compression method
		unsigned int zipfilesize = 0; // Uncompressed size
		unsigned int zipfileoffset = 0; // Relative offset of local file header
		unsigned int lengthfilename = 0; // File name length
		unsigned int lengthextrafield = 0; // Extra field length
		unsigned int lengthcomment = 0; // File comment length

		cursor += 10;
		memcpy(&compmethod, buffer + cursor, 2);
		cursor += 14;
		memcpy(&zipfilesize, buffer + cursor, 4);
		cursor += 4;
		memcpy(&lengthfilename, buffer + cursor, 2);
		cursor += 2;
		memcpy(&lengthextrafield, buffer + cursor, 2);
		cursor += 2;
		memcpy(&lengthcomment, buffer + cursor, 2);
		cursor += 10;
		memcpy(&zipfileoffset, buffer + cursor, 4);
		cursor += 4;

		std::string zipfile;
		zipfile.resize(lengthfilename);
		memcpy(&zipfile[0], buffer + cursor, lengthfilename);

		cursor += lengthfilename + lengthextrafield + lengthcomment;
		
		if (compmethod == 0 && zipfilesize > 0) // Only no compression && no directories
		{
			MAP_FILES frame;
			frame.offset = zipfileoffset + (lengthfilename + 30);
			frame.size = zipfilesize;

			zipFiles[zipfile] = frame;
		}

		testoffset += zipfilesize + lengthfilename + 30;
	}

	delete[] buffer;

	if (testoffset != diroffset)
	{
		zipStream.close();
		zipFiles.clear();
		return false;
	}

	return true;
}

void ZipFile::FreeBuffer()
{
	if (zipBuffer)
	{
		delete[] zipBuffer;
		zipBuffer = nullptr;
		zipBufferSize = 0;
	}
}

bool ZipFile::UnzipToBuffer(const std::wstring& file)
{
	// http://stackoverflow.com/questions/2256160/how-bad-is-code-using-stdbasic-stringt-as-a-contiguous-buffer
	std::string fileUTF8;
	int len = WideCharToMultiByte(CP_UTF8, 0, file.c_str(), (int)file.size(), NULL, 0, NULL, NULL);
	if (len > 0)
	{
		fileUTF8.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, file.c_str(), (int)file.size(), &fileUTF8[0], len, NULL, NULL);
	}

	std::replace(fileUTF8.begin(), fileUTF8.end(), '\\', '/');

	std::size_t posFind = fileUTF8.find("./");
	if (posFind != std::string::npos)
		fileUTF8.erase(0, posFind + 2);


	std::map<std::string, MAP_FILES>::iterator it = zipFiles.find(fileUTF8);
	if (it != zipFiles.end())
	{
		zipBufferSize = it->second.size;
		zipBuffer = new char[zipBufferSize];

		zipStream.seekg(it->second.offset);
		zipStream.read(zipBuffer, zipBufferSize);

		return true;
	}
	
	return false;
}

bool ZipFile::CheckFileInZip(const std::wstring& file)
{
	std::string fileUTF8;
	int len = WideCharToMultiByte(CP_UTF8, 0, file.c_str(), (int)file.size(), NULL, 0, NULL, NULL);
	if (len > 0)
	{
		fileUTF8.resize(len);
		WideCharToMultiByte(CP_UTF8, 0, file.c_str(), (int)file.size(), &fileUTF8[0], len, NULL, NULL);
	}

	std::replace(fileUTF8.begin(), fileUTF8.end(), '\\', '/');

	std::map<std::string, MAP_FILES>::iterator it = zipFiles.find(fileUTF8);
	if (it != zipFiles.end())
		return true;
	
	return false;
}

#ifdef PACKSKIN

bool ZipFile::ZipPathToFile(const std::wstring& file, const std::wstring& path, int level)
{
	// if the path ends with '\\' then add a content of the path to a zip file
	// if the path does not end with '\\' then add a folder/file
	// We always use first option so I added assert for this

	assert(!path.empty() && path.back() == '\\');

	zlib_filefunc64_def ffunc;
	fill_fopen64_filefunc(&ffunc);

	zipFile zfile = zipOpen2_64((L"\\\\?\\" + file).c_str(), 0, NULL, &ffunc);

	if (zfile == NULL)
		return false;

	ZipPathRecur(zfile, L"\\\\?\\" + path, L"", level);

	zipClose(zfile, NULL);

	return true;
}

bool ZipFile::UnzipFileToPath(const std::wstring& file, const std::wstring& path)
{
	assert(!path.empty() && path.back() == '\\');

	zlib_filefunc64_def ffunc;
	fill_fopen64_filefunc(&ffunc);

	unzFile zfile = unzOpen2_64((L"\\\\?\\" + file).c_str(), &ffunc);
	
	if (zfile == NULL)
		return false;

	UnzipFile(zfile, L"\\\\?\\" + path);

	unzClose(zfile);

	return true;
}

void ZipFile::ZipPathRecur(zipFile zfile, const std::wstring& folderPath, const std::wstring& folderZip, int level)
{
	std::wstring path = folderPath;
	if (path.back() == '\\')
		path.push_back('*');

	std::unique_ptr<WIN32_FIND_DATAW> fd(new WIN32_FIND_DATAW);
	
	HANDLE findHandle = ::FindFirstFileW(path.c_str(), fd.get());
	if (findHandle != INVALID_HANDLE_VALUE)
	do
	{
		// Skip dots
		if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (fd->cFileName[0] == '.' &&
				(fd->cFileName[1] == '\0' || (fd->cFileName[1] == '.' && fd->cFileName[2] == '\0')))
				continue;
		}

		// Make paths for a file and the file in the zip archive
		std::wstring filePath = folderPath;
		if (filePath.back() == '\\')
			filePath += fd->cFileName;
		std::wstring fileZip = folderZip + fd->cFileName;

		// If it's a folder then the paths must end with a slash
		if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			filePath.push_back('\\');
			fileZip.push_back('/');
		}

		// Use UTF8 encoding for file names in the zip archive
		std::string fileZipUTF8;
		int len = WideCharToMultiByte(CP_UTF8, 0, fileZip.c_str(), (int)fileZip.size(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			fileZipUTF8.resize(len);
			WideCharToMultiByte(CP_UTF8, 0, fileZip.c_str(), (int)fileZip.size(), &fileZipUTF8[0], len, NULL, NULL);
		}

		// Fill file attributes
		zip_fileinfo zi = {};
		zi.external_fa = fd->dwFileAttributes;
		FILETIME ftLocal = {};
		::FileTimeToLocalFileTime(&fd->ftLastWriteTime, &ftLocal);
		::FileTimeToDosDateTime(&ftLocal, ((LPWORD)&zi.dosDate) + 1 , ((LPWORD)&zi.dosDate) + 0);

		if (fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // Add folder
		{
			// Check below about the options. And as a reminder, the folder must be added without compression
			zipOpenNewFileInZip4_64(zfile, fileZipUTF8.c_str(), &zi, NULL, 0, NULL, 0, NULL, 0, 0, 0,
				 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 20, 0x0800, 0);
			zipCloseFileInZip(zfile);

			// Recursively pack all subfolders
			ZipPathRecur(zfile, filePath, fileZip, level);
		}
		else // Add file
		{
			// 0x0800 this is 11 bit (1 << 11) which is responsible for UTF-8 encoding for file names
			// 20 this is the version 2.0 (used by default)
			// (fd->nFileSizeHigh ? 1 : 0) - if the file is too big then we use Zip64
			if (zipOpenNewFileInZip4_64(zfile, fileZipUTF8.c_str(), &zi, NULL, 0, NULL, 0, NULL,
				level ? Z_DEFLATED : 0, level, 0, //Z_DEFLATED, 5, 0,
				-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, NULL, 0, 20, 0x0800, fd->nFileSizeHigh ? 1 : 0) == ZIP_OK)
			{
				HANDLE fileHandle = ::CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (fileHandle)
				{
					const DWORD bufsize = 4096;//4096 * 4;
					DWORD readsize = 0;
					//char* buf = new char[bufsize];
					char buf[bufsize];

					while (::ReadFile(fileHandle, buf, bufsize, &readsize, NULL))
					{
						if (readsize == 0)
							break;
						zipWriteInFileInZip(zfile, buf, readsize);
					}

					::CloseHandle(fileHandle);

					//delete[] buf;
				}
			}

			zipCloseFileInZip(zfile);
		}
	}
	while (::FindNextFileW(findHandle, fd.get()));

	if (findHandle != INVALID_HANDLE_VALUE)
		::FindClose(findHandle);
}

void ZipFile::UnzipFile(zipFile zfile, const std::wstring& folderPath)
{
	if (unzGoToFirstFile(zfile) == UNZ_OK)
	do
	{
		unz_file_info64 fi;
		char filename[256] = {};
		unzGetCurrentFileInfo64(zfile, &fi, filename, sizeof(filename), 0, 0, 0, 0);

		std::wstring filePath;
		UINT codePage = (fi.flag & (1 << 11)) ? CP_UTF8 : CP_ACP; // if bit 11 then UTF8
		int len = MultiByteToWideChar(codePage, 0, filename, -1, NULL, 0) - 1;
		if (len > 0)
		{
			filePath.resize(len);
			MultiByteToWideChar(CP_UTF8, 0, filename, -1, &filePath[0], len);
		}

		std::replace(filePath.begin(), filePath.end(), '/', '\\');

		if (fi.external_fa & FILE_ATTRIBUTE_DIRECTORY) // Folder
		{
			// The folder ends with slash, remove it
			if (!filePath.empty() && filePath.back() == '\\')
				filePath.pop_back();

			filePath = folderPath + filePath;

			//::CreateDirectoryW(filePath.c_str(), NULL);
			if (::GetFileAttributesW(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
				CreateFullPath(filePath, &fi);
		}
		else // File
		{
			if (unzOpenCurrentFile(zfile) == UNZ_OK)
			{
				filePath = folderPath + filePath;

				// Get a path from the file
				std::wstring path;
				std::size_t find = filePath.rfind('\\');
				if (find != std::string::npos)
					path = filePath.substr(0, find);

				// Create the path if not exists
				if (::GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES)
					CreateFullPath(path, &fi);

				HANDLE fileHandle = ::CreateFileW(filePath.c_str(), GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, fi.external_fa, NULL);
				if (fileHandle)
				{
					const DWORD bufsize = 4096;//4096 * 4;
					DWORD readsize = 0;
					DWORD writtensize = 0;
					//char* buf = new char[bufsize];
					char buf[bufsize];

					while ((readsize = unzReadCurrentFile(zfile, buf, bufsize)) != 0)
					{
						if (!::WriteFile(fileHandle, buf, readsize, &writtensize, NULL))
							break;
						if (readsize != writtensize)
							break;
					}

					SetFileTimeWithFileInfo(fileHandle, &fi);

					::CloseHandle(fileHandle);

					//delete[] buf;
				}
			}

			unzCloseCurrentFile(zfile);
		}
	}
	while (unzGoToNextFile(zfile) == UNZ_OK);
}

bool ZipFile::SetFileTimeWithFileInfo(HANDLE fileHandle, unz_file_info64* fi)
{
	if (fi)
	{
		FILETIME ftm, ftLocal;
		if (::DosDateTimeToFileTime((WORD)(fi->dosDate >> 16), (WORD)fi->dosDate, &ftLocal))
		{
			if (::LocalFileTimeToFileTime(&ftLocal, &ftm))
			{
				::SetFileTime(fileHandle, &ftm, &ftm, &ftm);
				return true;
			}
		}
	}
	return false;
}

bool ZipFile::CreateFullPath(const std::wstring& path, unz_file_info64* fi)
{
	//if (::CreateDirectoryW(path.c_str(), NULL))
	//	return true;
	if (CreateDirectoryWithFileInfo(path.c_str(), fi))
		return true;

	size_t start = 3; // Skip drive
	if (path.size() > 2 && path[2] == '?') // Start with "\\?\" = long unicode path
		start += 4; // Skip "\\?\"

	if (path.size() < start)
		return false;

	bool result = false;

	while ((start = path.find('\\', start)) != std::string::npos)
	{
		//result = ::CreateDirectoryW(path.substr(0, start).c_str(), NULL) == TRUE;
		result = CreateDirectoryWithFileInfo(path.substr(0, start).c_str(), fi);

		++start;
	}

	if (path.back() != '\\')
		//result = ::CreateDirectoryW(path.c_str(), NULL) == TRUE;
		result = CreateDirectoryWithFileInfo(path.c_str(), fi);

	return result;
}

bool ZipFile::CreateDirectoryWithFileInfo(const std::wstring& path, unz_file_info64* fi)
{
	bool result = ::CreateDirectoryW(path.c_str(), NULL) == TRUE;
	if (fi && result)
	{
		HANDLE fileHandle = ::CreateFileW(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

		if (fileHandle)
		{
			SetFileTimeWithFileInfo(fileHandle, fi);

			::CloseHandle(fileHandle);
		}

		::SetFileAttributesW(path.c_str(), fi->external_fa | FILE_ATTRIBUTE_DIRECTORY);
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////

voidpf ZipFile::fopen64_file_func (voidpf opaque, const void* filename, int mode)
{
    FILE* file = NULL;
    const wchar_t* mode_fopen = NULL;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
        mode_fopen = L"rb";
    else
    if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        mode_fopen = L"r+b";
    else
    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        mode_fopen = L"wb";

    if ((filename!=NULL) && (mode_fopen != NULL))
        //file = _wfopen((const wchar_t*)filename, mode_fopen);
		_wfopen_s(&file, (const wchar_t*)filename, mode_fopen);
    return file;
}


uLong ZipFile::fread_file_func (voidpf opaque, voidpf stream, void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)fread(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}

uLong ZipFile::fwrite_file_func (voidpf opaque, voidpf stream, const void* buf, uLong size)
{
    uLong ret;
    ret = (uLong)fwrite(buf, 1, (size_t)size, (FILE *)stream);
    return ret;
}

ZPOS64_T ZipFile::ftell64_file_func (voidpf opaque, voidpf stream)
{
    ZPOS64_T ret;
    ret = _ftelli64((FILE *)stream);
    return ret;
}

long ZipFile::fseek64_file_func (voidpf  opaque, voidpf stream, ZPOS64_T offset, int origin)
{
    int fseek_origin=0;
    long ret;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        fseek_origin = SEEK_CUR;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        fseek_origin = SEEK_END;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        fseek_origin = SEEK_SET;
        break;
    default: return -1;
    }
    ret = 0;

    if(_fseeki64((FILE *)stream, offset, fseek_origin) != 0)
                        ret = -1;

    return ret;
}

int ZipFile::fclose_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = fclose((FILE *)stream);
    return ret;
}

int ZipFile::ferror_file_func (voidpf opaque, voidpf stream)
{
    int ret;
    ret = ferror((FILE *)stream);
    return ret;
}

void ZipFile::fill_fopen64_filefunc (zlib_filefunc64_def*  pzlib_filefunc_def)
{
    pzlib_filefunc_def->zopen64_file = fopen64_file_func;
    pzlib_filefunc_def->zread_file = fread_file_func;
    pzlib_filefunc_def->zwrite_file = fwrite_file_func;
    pzlib_filefunc_def->ztell64_file = ftell64_file_func;
    pzlib_filefunc_def->zseek64_file = fseek64_file_func;
    pzlib_filefunc_def->zclose_file = fclose_file_func;
    pzlib_filefunc_def->zerror_file = ferror_file_func;
    pzlib_filefunc_def->opaque = NULL;
}

#endif // PACKSKIN